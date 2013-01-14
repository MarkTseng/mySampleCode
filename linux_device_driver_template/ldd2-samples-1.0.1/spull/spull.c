/*
 * spull.c -- the Simple Partitionable Utility
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: spull.c,v 1.17 2001/07/18 22:28:18 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif


#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h> /* printk() */
#include <linux/malloc.h> /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/timer.h>
#include <linux/types.h>  /* size_t */
#include <linux/fcntl.h>        /* O_ACCMODE */
#include <linux/hdreg.h>  /* HDIO_GETGEO */

#include <asm/system.h>   /* cli(), *_flags */


#define MAJOR_NR spull_major /* force definitions on in blk.h */
int spull_major; /* must be declared before including blk.h */

#define SPULL_SHIFT 4                         /* max 16 partitions  */
#define SPULL_MAXNRDEV 4                      /* max 4 device units */
#define DEVICE_NR(device) (MINOR(device)>>SPULL_SHIFT)
#define DEVICE_NAME "pd"                      /* name for messaging */
#define DEVICE_INTR spull_intrptr         /* pointer to the bottom half */
#define DEVICE_NO_RANDOM                  /* no entropy to contribute */
#define DEVICE_REQUEST spull_request
#define DEVICE_OFF(d) /* do-nothing */

#include <linux/blk.h>


#include "spull.h"        /* local definitions */
#ifdef HAVE_BLKPG_H
#include <linux/blkpg.h>  /* blk_ioctl() */
#endif

/*
 * Non-prefixed symbols are static. They are meant to be assigned at
 * load time. Prefixed symbols are not static, so they can be used in
 * debugging. They are hidden anyways by register_symtab() unless
 * SPULL_DEBUG is defined.
 */
static int major    = SPULL_MAJOR;
static int devs     = SPULL_DEVS;
static int rahead   = SPULL_RAHEAD;
static int size     = SPULL_SIZE;
static int irq      = 0;

static int blksize  = SPULL_BLKSIZE;

MODULE_PARM(major, "i");
MODULE_PARM(devs, "i");
MODULE_PARM(rahead, "i");
MODULE_PARM(size, "i");
MODULE_PARM(blksize, "i");
MODULE_PARM(irq, "i");
MODULE_AUTHOR("Alessandro Rubini");

int spull_devs, spull_rahead, spull_size;
int spull_blksize, spull_irq;

/* The following items are obtained through kmalloc() in spull_init() */

Spull_Dev *spull_devices = NULL;
int *spull_sizes = NULL;

/* Forwards */
int spull_revalidate(kdev_t i_rdev);
#ifdef LINUX_24
struct block_device_operations spull_bdops;
#endif

/*
 * Time for our genhd structure.
 */
struct gendisk spull_gendisk = {
    major:              0,              /* Major number assigned later */
    major_name:         "pd",           /* Name of the major device */
    minor_shift:        SPULL_SHIFT,    /* Shift to get device number */
    max_p:              1 << SPULL_SHIFT, /* Number of partitions */
#ifdef LINUX_24                         
    fops:               &spull_bdops,   /* Block dev operations */
#endif                                  
/* everything else is dynamic */
};

struct hd_struct *spull_partitions = NULL;

/*
 * Flag used in "irq driven" mode to mark when we have operations
 * outstanding.
 */
volatile int spull_busy = 0;


/*
 * Open and close
 */

int spull_open (struct inode *inode, struct file *filp)
{
    Spull_Dev *dev; /* device information */
    int num = DEVICE_NR(inode->i_rdev);

    if (num >= spull_devs) return -ENODEV;
    dev = spull_devices + num;

    /* kill the timer associated to the device: it might be active */
    del_timer(&dev->timer);

    spin_lock(&dev->lock);

    /*
     * If no data area is there, allocate it. Clear its head as
     * well to prevent memory corruption due to bad partition info.
     */
    if (!dev->data) {
        dev->data = vmalloc(dev->size);
        memset(dev->data,0,2048);
    }
    if (!dev->data)
    {
        spin_unlock(&dev->lock);
        return -ENOMEM;
    }
    
    dev->usage++;
    MOD_INC_USE_COUNT;
    spin_unlock(&dev->lock);

    return 0;          /* success */
}



int spull_release (struct inode *inode, struct file *filp)
{
    Spull_Dev *dev = spull_devices + DEVICE_NR(inode->i_rdev);

    spin_lock(&dev->lock);
    dev->usage--;

    /*
     * If the device is closed for the last time, start a timer
     * to release RAM in half a minute. The function and argument
     * for the timer have been setup in spull_init()
     */
    if (!dev->usage) {
        dev->timer.expires = jiffies + 60 * HZ;
        add_timer(&dev->timer);
        /* but flush it right now */
        fsync_dev(inode->i_rdev);
        invalidate_buffers(inode->i_rdev);
    }

    MOD_DEC_USE_COUNT;
    spin_unlock(&dev->lock);
    return 0;
}



/*
 * The timer function. As argument it receives the device
 */
void spull_expires(unsigned long data)
{
    Spull_Dev *dev = (Spull_Dev *)data;

    spin_lock(&dev->lock);
    if (dev->usage || !dev->data) {
        spin_unlock(&dev->lock);
        printk(KERN_WARNING "spull: timer mismatch for device %i\n",
               dev - spull_devices);
        return;
    }
    PDEBUG("freeing device %i\n",dev - spull_devices);
    vfree(dev->data);
    dev->data=0;
    spin_unlock(&dev->lock);
    return;
}    

/*
 * The ioctl() implementation
 */

int spull_ioctl (struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{
    int err, size;
    struct hd_geometry geo;

    PDEBUG("ioctl 0x%x 0x%lx\n", cmd, arg);
    switch(cmd) {

      case BLKGETSIZE:
        /* Return the device size, expressed in sectors */
        err = ! access_ok (VERIFY_WRITE, arg, sizeof(long));
        if (err) return -EFAULT;
        size = spull_gendisk.part[MINOR(inode->i_rdev)].nr_sects;
	if (copy_to_user((long *) arg, &size, sizeof (long)))
	    return -EFAULT;
        return 0;

      case BLKFLSBUF: /* flush */
        if (! capable(CAP_SYS_RAWIO)) return -EACCES; /* only root */
        fsync_dev(inode->i_rdev);
        invalidate_buffers(inode->i_rdev);
        return 0;

      case BLKRAGET: /* return the readahead value */
        err = ! access_ok(VERIFY_WRITE, arg, sizeof(long));
        if (err) return -EFAULT;
        PUT_USER(read_ahead[MAJOR(inode->i_rdev)],(long *) arg);
        return 0;

      case BLKRASET: /* set the readahead value */
        if (!capable(CAP_SYS_RAWIO)) return -EACCES;
        if (arg > 0xff) return -EINVAL; /* limit it */
        read_ahead[MAJOR(inode->i_rdev)] = arg;
        return 0;

      case BLKRRPART: /* re-read partition table */
        return spull_revalidate(inode->i_rdev);

      case HDIO_GETGEO:
        /*
         * get geometry: we have to fake one...  trim the size to a
         * multiple of 64 (32k): tell we have 16 sectors, 4 heads,
         * whatever cylinders. Tell also that data starts at sector. 4.
         */
        err = ! access_ok(VERIFY_WRITE, arg, sizeof(geo));
        if (err) return -EFAULT;
        size = spull_size * blksize / SPULL_HARDSECT;
        geo.cylinders = (size & ~0x3f) >> 6;
	geo.heads = 4;
	geo.sectors = 16;
	geo.start = 4;
	if (copy_to_user((void *) arg, &geo, sizeof(geo)))
	    return -EFAULT;
        return 0;

      default:
        /*
         * For ioctls we don't understand, let the block layer handle them.
         */
        return blk_ioctl(inode->i_rdev, cmd, arg);
    }

    return -ENOTTY; /* unknown command */
}

/*
 * Support for removable devices
 */

int spull_check_change(kdev_t i_rdev)
{
    int minor = DEVICE_NR(i_rdev);
    Spull_Dev *dev = spull_devices + minor;

    if (minor >= spull_devs) /* paranoid */
        return 0;

    PDEBUG("check_change for dev %i\n",minor);

    if (dev->data)
        return 0; /* still valid */
    return 1; /* expired */
}




/*
 * The file operations
 */

#ifdef LINUX_24
struct block_device_operations spull_bdops = {
    open:       spull_open,
    release:    spull_release,
    ioctl:      spull_ioctl,
    revalidate: spull_revalidate,
    check_media_change: spull_check_change,
};
#else

#ifdef LINUX_20
void spull_release_20 (struct inode *inode, struct file *filp)
{
        (void) spull_release (inode, filp);
}
#define spull_release spull_release_20
#endif

struct file_operations spull_bdops = {
    read:       block_read,
    write:      block_write,
    ioctl:      spull_ioctl,
    open:       spull_open,
    release:    spull_release,
    fsync:      block_fsync,
    check_media_change: spull_check_change,
    revalidate: spull_revalidate
};
# endif /* LINUX_24 */


/*
 * Note no locks taken out here.  In a worst case scenario, we could drop
 * a chunk of system memory.  But that should never happen, since validation
 * happens at open or mount time, when locks are held.
 */
int spull_revalidate(kdev_t i_rdev)
{
    /* first partition, # of partitions */
    int part1 = (DEVICE_NR(i_rdev) << SPULL_SHIFT) + 1;
    int npart = (1 << SPULL_SHIFT) -1;

    /* first clear old partition information */
    memset(spull_gendisk.sizes+part1, 0, npart*sizeof(int));
    memset(spull_gendisk.part +part1, 0, npart*sizeof(struct hd_struct));
    spull_gendisk.part[DEVICE_NR(i_rdev) << SPULL_SHIFT].nr_sects =
            spull_size << 1;

    /* then fill new info */
    printk(KERN_INFO "Spull partition check: (%d) ", DEVICE_NR(i_rdev));
    register_disk(&spull_gendisk, i_rdev, SPULL_MAXNRDEV, &spull_bdops,
                    spull_size << 1);
    return 0;
}




/*
 * Block-driver specific functions
 */

/*
 * Find the device for this request.
 */
static Spull_Dev *spull_locate_device(const struct request *req)
{
    int devno;
    Spull_Dev *device;

    /* Check if the minor number is in range */
    devno = DEVICE_NR(req->rq_dev);
    if (devno >= spull_devs) {
        static int count = 0;
        if (count++ < 5) /* print the message at most five times */
            printk(KERN_WARNING "spull: request for unknown device\n");
        return NULL;
    }
    device = spull_devices + devno;
    return device;
}


/*
 * Perform an actual transfer.
 */
static int spull_transfer(Spull_Dev *device, const struct request *req)
{
    int size, minor = MINOR(req->rq_dev);
    u8 *ptr;
    
    ptr = device->data +
            (spull_partitions[minor].start_sect + req->sector)*SPULL_HARDSECT;
    size = req->current_nr_sectors*SPULL_HARDSECT;
    /*
     * Make sure that the transfer fits within the device.
     */
    if (req->sector + req->current_nr_sectors >
                    spull_partitions[minor].nr_sects) {
        static int count = 0;
        if (count++ < 5)
            printk(KERN_WARNING "spull: request past end of partition\n");
        return 0;
    }
    /*
     * Looks good, do the transfer.
     */
    switch(req->cmd) {
        case READ:
            memcpy(req->buffer, ptr, size); /* from spull to buffer */
            return 1;
        case WRITE:
            memcpy(ptr, req->buffer, size); /* from buffer to spull */
            return 1;
        default:
            /* can't happen */
            return 0;
        }
}



#ifdef LINUX_24
void spull_request(request_queue_t *q)
#else           
void spull_request()
#endif  
{
    Spull_Dev *device;
    int status;
    long flags;

    while(1) {
        INIT_REQUEST;  /* returns when queue is empty */

        /* Which "device" are we using?  (Is returned locked) */
        device = spull_locate_device (CURRENT);
        if (device == NULL) {
            end_request(0);
            continue;
        }
	spin_lock_irqsave(&device->lock, flags);

        /* Perform the transfer and clean up. */
        status = spull_transfer(device, CURRENT);
        spin_unlock_irqrestore(&device->lock, flags);
        end_request(status); /* success */
    }
}


/*
 * The fake interrupt-driven request
 */
struct timer_list spull_timer; /* the engine for async invocation */

#ifdef LINUX_24
void spull_irqdriven_request(request_queue_t *q)
#else                                           
void spull_irqdriven_request()                  
#endif                                          
{
    Spull_Dev *device;
    int status;
    long flags;

    /* If we are already processing requests, don't do any more now. */
    if (spull_busy)
            return;

    while(1) {
        INIT_REQUEST;  /* returns when queue is empty */

        /* Which "device" are we using? */
        device = spull_locate_device (CURRENT);
        if (device == NULL) {
            end_request(0);
            continue;
        }
	spin_lock_irqsave(&device->lock, flags);
	
        /* Perform the transfer and clean up. */
        status = spull_transfer(device, CURRENT);
        spin_unlock_irqrestore(&device->lock, flags);
        /* ... and wait for the timer to expire -- no end_request(1) */
        spull_timer.expires = jiffies + spull_irq;
        add_timer(&spull_timer);
        spull_busy = 1;
        return;
    }
}


/* this is invoked when the timer expires */
void spull_interrupt(unsigned long unused)
{
    unsigned long flags;

    spin_lock_irqsave(&io_request_lock, flags);
    end_request(1);    /* This request is done - we always succeed */

    spull_busy = 0;  /* We have io_request_lock, no conflict with request */
    if (! QUEUE_EMPTY) /* more of them? */
#ifdef LINUX_24                         
        spull_irqdriven_request(NULL);  /* Start the next transfer */
#else                                   
        spull_irqdriven_request();      
#endif                                  
    spin_unlock_irqrestore(&io_request_lock, flags);
}




/*
 * Finally, the module stuff
 */

int spull_init(void)
{
    int result, i;

    /*
     * Copy the (static) cfg variables to public prefixed ones to allow
     * snoozing with a debugger.
     */

    spull_major    = major;
    spull_devs     = devs;
    spull_rahead   = rahead;
    spull_size     = size;
    spull_blksize  = blksize;

    /*
     * Register your major, and accept a dynamic number
     */
    result = register_blkdev(spull_major, "spull", &spull_bdops);
    if (result < 0) {
        printk(KERN_WARNING "spull: can't get major %d\n",spull_major);
        return result;
    }
    if (spull_major == 0) spull_major = result; /* dynamic */
    major = spull_major; /* Use `major' later on to save typing */
    spull_gendisk.major = major; /* was unknown at load time */

    /* 
     * allocate the devices -- we can't have them static, as the number
     * can be specified at load time
     */

    spull_devices = kmalloc(spull_devs * sizeof (Spull_Dev), GFP_KERNEL);
    if (!spull_devices)
        goto fail_malloc;
    memset(spull_devices, 0, spull_devs * sizeof (Spull_Dev));
    for (i=0; i < spull_devs; i++) {
        /* data and usage remain zeroed */
        spull_devices[i].size = blksize * spull_size;
        init_timer(&(spull_devices[i].timer));
        spull_devices[i].timer.data = (unsigned long)(spull_devices+i);
        spull_devices[i].timer.function = spull_expires;
        spin_lock_init(&spull_devices[i].lock);
    }

    /*
     * Assign the other needed values: request, rahead, size, blksize,
     * hardsect. All the minor devices feature the same value.
     * Note that `spull' defines all of them to allow testing non-default
     * values. A real device could well avoid setting values in global
     * arrays if it uses the default values.
     */

    read_ahead[major] = spull_rahead;
    result = -ENOMEM; /* for the possible errors */

    spull_sizes = kmalloc( (spull_devs << SPULL_SHIFT) * sizeof(int),
                          GFP_KERNEL);
    if (!spull_sizes)
        goto fail_malloc;

    /* Start with zero-sized partitions, and correctly sized units */
    memset(spull_sizes, 0, (spull_devs << SPULL_SHIFT) * sizeof(int));
    for (i=0; i< spull_devs; i++)
        spull_sizes[i<<SPULL_SHIFT] = spull_size;
    blk_size[MAJOR_NR] = spull_gendisk.sizes = spull_sizes;

    /* Allocate the partitions array. */
    spull_partitions = kmalloc( (spull_devs << SPULL_SHIFT) *
                               sizeof(struct hd_struct), GFP_KERNEL);
    if (!spull_partitions)
        goto fail_malloc;

    memset(spull_partitions, 0, (spull_devs << SPULL_SHIFT) *
           sizeof(struct hd_struct));
    /* fill in whole-disk entries */
    for (i=0; i < spull_devs; i++) 
        spull_partitions[i << SPULL_SHIFT].nr_sects =
		spull_size*(blksize/SPULL_HARDSECT);
    spull_gendisk.part = spull_partitions;
    spull_gendisk.nr_real = spull_devs;
#ifndef LINUX_24
    spull_gendisk.max_nr = spull_devs;
#endif

    /*
     * Put our gendisk structure on the list.
     */
    spull_gendisk.next = gendisk_head;
    gendisk_head = &spull_gendisk; 

    /* dump the partition table to see it */
    for (i=0; i < spull_devs << SPULL_SHIFT; i++)
        PDEBUGG("part %i: beg %lx, size %lx\n", i,
               spull_partitions[i].start_sect,
               spull_partitions[i].nr_sects);

    /*
     * Allow interrupt-driven operation, if "irq=" has been specified
     */
    spull_irq = irq; /* copy the static variable to the visible one */
    if (spull_irq) {
        PDEBUG("setting timer\n");
        spull_timer.function = spull_interrupt;
        blk_init_queue(BLK_DEFAULT_QUEUE(major), spull_irqdriven_request);
    }
    else
        blk_init_queue(BLK_DEFAULT_QUEUE(major), spull_request);

#ifdef NOTNOW
    for (i = 0; i < spull_devs; i++)
            register_disk(NULL, MKDEV(major, i), 1, &spull_bdops,
                            spull_size << 1);
#endif

#ifndef SPULL_DEBUG
    EXPORT_NO_SYMBOLS; /* otherwise, leave global symbols visible */
#endif

    printk ("<1>spull: init complete, %d devs, size %d blks %d\n",
                    spull_devs, spull_size, spull_blksize);
    return 0; /* succeed */

  fail_malloc:
    read_ahead[major] = 0;
    if (spull_sizes) kfree(spull_sizes);
    if (spull_partitions) kfree(spull_partitions);
    blk_size[major] = NULL;
    if (spull_devices) kfree(spull_devices);

    unregister_blkdev(major, "spull");
    return result;
}

void spull_cleanup(void)
{
    int i;
    struct gendisk **gdp;
/*
 * Before anything else, get rid of the timer functions.  Set the "usage"
 * flag on each device as well, under lock, so that if the timer fires up
 * just before we delete it, it will either complete or abort.  Otherwise
 * we have nasty race conditions to worry about.
 */
    for (i = 0; i < spull_devs; i++) {
        Spull_Dev *dev = spull_devices + i;
        del_timer(&dev->timer);
        spin_lock(&dev->lock);
        dev->usage++;
        spin_unlock(&dev->lock);
    }

    /* flush it all and reset all the data structures */

/*
 * Unregister the device now to avoid further operations during cleanup.
 */
    unregister_blkdev(major, "spull");

    for (i = 0; i < (spull_devs << SPULL_SHIFT); i++)
        fsync_dev(MKDEV(spull_major, i)); /* flush the devices */
    blk_cleanup_queue(BLK_DEFAULT_QUEUE(major));
    read_ahead[major] = 0;
    kfree(blk_size[major]); /* which is gendisk->sizes as well */
    blk_size[major] = NULL;
    kfree(spull_gendisk.part);
    kfree(blksize_size[major]);
    blksize_size[major] = NULL;

    /*
     * Get our gendisk structure off the list.
     */
    for (gdp = &gendisk_head; *gdp; gdp = &((*gdp)->next))
        if (*gdp == &spull_gendisk) {
            *gdp = (*gdp)->next;
            break;
        }

    /* finally, the usual cleanup */
    for (i=0; i < spull_devs; i++) {
        if (spull_devices[i].data)
            vfree(spull_devices[i].data);
    }
    kfree(spull_devices);
}


module_init(spull_init);
module_exit(spull_cleanup);
