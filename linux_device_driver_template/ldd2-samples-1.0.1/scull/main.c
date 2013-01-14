/*
 * main.c -- the bare scull char module
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
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/kernel.h>   /* printk() */
#include <linux/malloc.h>   /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>    /* O_ACCMODE */

#include <asm/system.h>     /* cli(), *_flags */

#include "scull.h"          /* local definitions */

/*
 * I don't use static symbols here, because we export no symbols
 */

int scull_major =   SCULL_MAJOR;
int scull_nr_devs = SCULL_NR_DEVS;    /* number of bare scull devices */
int scull_quantum = SCULL_QUANTUM;
int scull_qset =    SCULL_QSET;

MODULE_PARM(scull_major,"i");
MODULE_PARM(scull_nr_devs,"i");
MODULE_PARM(scull_quantum,"i");
MODULE_PARM(scull_qset,"i");
MODULE_AUTHOR("Alessandro Rubini");

Scull_Dev *scull_devices; /* allocated in scull_init_module */

/*
 * Different minors behave differently, so let's use multiple fops
 */


struct file_operations *scull_fop_array[]={
    &scull_fops,      /* type 0 */
    &scull_priv_fops, /* type 1 */
    &scull_pipe_fops, /* type 2 */
    &scull_sngl_fops, /* type 3 */
    &scull_user_fops, /* type 4 */
    &scull_wusr_fops  /* type 5 */
};
#define SCULL_MAX_TYPE 5


int scull_trim(Scull_Dev *dev)
{
    Scull_Dev *next, *dptr;
    int qset = dev->qset;   /* "dev" is not-null */
    int i;

    for (dptr = dev; dptr; dptr = next) { /* all the list items */
        if (dptr->data) {
            for (i = 0; i < qset; i++)
                if (dptr->data[i])
                    kfree(dptr->data[i]);
            kfree(dptr->data);
            dptr->data=NULL;
        }
        next=dptr->next;
        if (dptr != dev) kfree(dptr); /* all of them but the first */
    }
    dev->size = 0;
    dev->quantum = scull_quantum;
    dev->qset = scull_qset;
    dev->next = NULL;
    return 0;
}
#ifdef SCULL_DEBUG /* use proc only if debugging */
/*
 * The proc filesystem: function to read and entry
 */

int scull_read_procmem(char *buf, char **start, off_t offset,
                   int count, int *eof, void *data)
{
    int i, j, len = 0;
    int limit = count - 80; /* Don't print more than this */

    for (i = 0; i < scull_nr_devs && len <= limit; i++) {
        Scull_Dev *d = &scull_devices[i];
        if (down_interruptible(&d->sem))
                return -ERESTARTSYS;
        len += sprintf(buf+len,"\nDevice %i: qset %i, q %i, sz %li\n",
                       i, d->qset, d->quantum, d->size);
        for (; d && len <= limit; d = d->next) { /* scan the list */
            len += sprintf(buf+len, "  item at %p, qset at %p\n", d, d->data);
            if (d->data && !d->next) /* dump only the last item - save space */
                for (j = 0; j < d->qset; j++) {
                    if (d->data[j])
                        len += sprintf(buf+len,"    % 4i: %8p\n",j,d->data[j]);
                }
        }
        up(&scull_devices[i].sem);
    }
    *eof = 1;
    return len;
}

#ifdef USE_PROC_REGISTER

static int scull_get_info(char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof = 0;
    return scull_read_procmem (buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry scull_proc_entry = {
        namelen:    8,
        name:       "scullmem",
        mode:       S_IFREG | S_IRUGO,
        nlink:      1,
        get_info:   scull_get_info,
};

static void scull_create_proc()
{
    proc_register_dynamic(&proc_root, &scull_proc_entry);
}

static void scull_remove_proc()
{
    proc_unregister(&proc_root, scull_proc_entry.low_ino);
}

#else  /* no USE_PROC_REGISTER - modern world */

static void scull_create_proc()
{
    create_proc_read_entry("scullmem", 0 /* default mode */,
                           NULL /* parent dir */, scull_read_procmem,
                           NULL /* client data */);
}

static void scull_remove_proc()
{
    /* no problem if it was not registered */
    remove_proc_entry("scullmem", NULL /* parent dir */);
}


#endif /* USE_PROC_REGISTER */





#endif /* SCULL_DEBUG */





/*
 * Open and close
 */


/* In scull_open, the fop_array is used according to TYPE(dev) */
int scull_open(struct inode *inode, struct file *filp)
{
    Scull_Dev *dev; /* device information */
    int num = NUM(inode->i_rdev);
    int type = TYPE(inode->i_rdev);

    /*
     * the type and num values are only valid if we are not using devfs.
     * However, since we use them to retrieve the device pointer, we
     * don't need them with devfs as filp->private_data is already
     * initialized
     */

    /*
     * If private data is not valid, we are not using devfs
     * so use the type (from minor nr.) to select a new f_op
     */
    if (!filp->private_data && type) {
        if (type > SCULL_MAX_TYPE) return -ENODEV;
        filp->f_op = scull_fop_array[type];
        return filp->f_op->open(inode, filp); /* dispatch to specific open */
    }

    /* type 0, check the device number (unless private_data valid) */
    dev = (Scull_Dev *)filp->private_data;
    if (!dev) {
        if (num >= scull_nr_devs) return -ENODEV;
        dev = &scull_devices[num];
        filp->private_data = dev; /* for other methods */
    }

    MOD_INC_USE_COUNT;  /* Before we maybe sleep */
    /* now trim to 0 the length of the device if open was write-only */
    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
        if (down_interruptible(&dev->sem)) {
            MOD_DEC_USE_COUNT;
            return -ERESTARTSYS;
        }
        scull_trim(dev); /* ignore errors */
        up(&dev->sem);
    }

    return 0;          /* success */
}

int scull_release(struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
    return 0;
}
/*
 * Follow the list 
 */
Scull_Dev *scull_follow(Scull_Dev *dev, int n)
{
    while (n--) {
        if (!dev->next) {
            dev->next = kmalloc(sizeof(Scull_Dev), GFP_KERNEL);
            memset(dev->next, 0, sizeof(Scull_Dev));
        }
        dev = dev->next;
        continue;
    }
    return dev;
}

/*
 * Data management: read and write
 */

ssize_t scull_read(struct file *filp, char *buf, size_t count,
                loff_t *f_pos)
{
    Scull_Dev *dev = filp->private_data; /* the first listitem */
    Scull_Dev *dptr;
    int quantum = dev->quantum;
    int qset = dev->qset;
    int itemsize = quantum * qset; /* how many bytes in the listitem */
    int item, s_pos, q_pos, rest;
    ssize_t ret = 0;

    if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;
    if (*f_pos >= dev->size)
        goto out;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;
    /* find listitem, qset index, and offset in the quantum */
    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    s_pos = rest / quantum; q_pos = rest % quantum;

    /* follow the list up to the right position (defined elsewhere) */
    dptr = scull_follow(dev, item);

    if (!dptr->data)
        goto out; /* don't fill holes */
    if (!dptr->data[s_pos])
        goto out;
    /* read only up to the end of this quantum */
    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_to_user(buf, dptr->data[s_pos]+q_pos, count)) {
        ret = -EFAULT;
	goto out;
    }
    *f_pos += count;
    ret = count;

 out:
    up(&dev->sem);
    return ret;
}

ssize_t scull_write(struct file *filp, const char *buf, size_t count,
                loff_t *f_pos)
{
    Scull_Dev *dev = filp->private_data;
    Scull_Dev *dptr;
    int quantum = dev->quantum;
    int qset = dev->qset;
    int itemsize = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t ret = -ENOMEM; /* value used in "goto out" statements */

    if (down_interruptible(&dev->sem))
            return -ERESTARTSYS;

    /* find listitem, qset index and offset in the quantum */
    item = (long)*f_pos / itemsize;
    rest = (long)*f_pos % itemsize;
    s_pos = rest / quantum; q_pos = rest % quantum;

    /* follow the list up to the right position */
    dptr = scull_follow(dev, item);
    if (!dptr->data) {
        dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
        if (!dptr->data)
            goto out;
        memset(dptr->data, 0, qset * sizeof(char *));
    }
    if (!dptr->data[s_pos]) {
        dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
        if (!dptr->data[s_pos])
            goto out;
    }
    /* write only up to the end of this quantum */
    if (count > quantum - q_pos)
        count = quantum - q_pos;

    if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
        ret = -EFAULT;
	goto out;
    }
    *f_pos += count;
    ret = count;

    /* update the size */
    if (dev->size < *f_pos)
        dev-> size = *f_pos;

  out:
    up(&dev->sem);
    return ret;
}

/*
 * The ioctl() implementation
 *
 * This is done twice, once the 2.2 way, followed by the 2.0 way.  One
 * would not normally do things in this manner, but we wanted to illustrate
 * both ways...
 */

#ifndef LINUX_20

int scull_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

    int err = 0, tmp;
    int ret = 0;
    
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
     */
    if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * access_ok is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if (err) return -EFAULT;

    switch(cmd) {

#ifdef SCULL_DEBUG
      case SCULL_IOCHARDRESET:
         /*
          * reset the counter to 1, to allow unloading in case
          * of problems. Use 1, not 0, because the invoking
          * process has the device open.
          */
         while (MOD_IN_USE)
             MOD_DEC_USE_COUNT;
         MOD_INC_USE_COUNT;
         /* don't break: fall through and reset things */
#endif /* SCULL_DEBUG */

      case SCULL_IOCRESET:
        scull_quantum = SCULL_QUANTUM;
        scull_qset = SCULL_QSET;
        break;
        
      case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        ret = __get_user(scull_quantum, (int *)arg);
        break;

      case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        scull_quantum = arg;
        break;

      case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
        ret = __put_user(scull_quantum, (int *)arg);
        break;

      case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
        return scull_quantum;

      case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        tmp = scull_quantum;
        ret = __get_user(scull_quantum, (int *)arg);
        if (ret == 0)
            ret = __put_user(tmp, (int *)arg);
        break;

      case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        tmp = scull_quantum;
        scull_quantum = arg;
        return tmp;
        
      case SCULL_IOCSQSET:
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        ret = __get_user(scull_qset, (int *)arg);
        break;

      case SCULL_IOCTQSET:
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        scull_qset = arg;
        break;

      case SCULL_IOCGQSET:
        ret = __put_user(scull_qset, (int *)arg);
        break;

      case SCULL_IOCQQSET:
        return scull_qset;

      case SCULL_IOCXQSET:
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        tmp = scull_qset;
        ret = __get_user(scull_qset, (int *)arg);
        if (ret == 0)
            ret = put_user(tmp, (int *)arg);
        break;

      case SCULL_IOCHQSET:
        if (! capable (CAP_SYS_ADMIN))
            return -EPERM;
        tmp = scull_qset;
        scull_qset = arg;
        return tmp;

        /*
         * The following two change the buffer size for scullpipe.
         * The scullpipe device uses this same ioctl method, just to
         * write less code. Actually, it's the same driver, isn't it?
         */

      case SCULL_P_IOCTSIZE:
        scull_p_buffer = arg;
        break;

      case SCULL_P_IOCQSIZE:
        return scull_p_buffer;


      default:  /* redundant, as cmd was checked against MAXNR */
        return -ENOTTY;
    }
    return ret;

}

#else  /* LINUX_20 */

int scull_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

    int err = 0, tmp;
    
    /*
     * extract the type and number bitfields, and don't decode
     * wrong cmds: return ENOTTY before verify_area()
     */
    if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

    /*
     * the direction is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. `Type' is user-oriented, while
     * verify_area is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = verify_area(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  verify_area(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if (err) return err;

    switch(cmd) {

      case SCULL_IOCRESET:
        scull_quantum = SCULL_QUANTUM;
        scull_qset = SCULL_QSET;
        break;
        
      case SCULL_IOCSQUANTUM: /* Set: arg points to the value */
        scull_quantum = get_user((int *)arg);
        break;

      case SCULL_IOCTQUANTUM: /* Tell: arg is the value */
        scull_quantum = arg;
        break;

      case SCULL_IOCGQUANTUM: /* Get: arg is pointer to result */
        put_user(scull_quantum, (int *)arg);
        break;

      case SCULL_IOCQQUANTUM: /* Query: return it (it's positive) */
        return scull_quantum;

      case SCULL_IOCXQUANTUM: /* eXchange: use arg as pointer */
        tmp = scull_quantum;
        scull_quantum = get_user((int *)arg);
        put_user(tmp, (int *)arg);
        break;

      case SCULL_IOCHQUANTUM: /* sHift: like Tell + Query */
        tmp = scull_quantum;
        scull_quantum = arg;
        return tmp;
        
      case SCULL_IOCSQSET:
        scull_qset = get_user((int *)arg);
        break;

      case SCULL_IOCTQSET:
        scull_qset = arg;
        break;

      case SCULL_IOCGQSET:
        put_user(scull_qset, (int *)arg);
        break;

      case SCULL_IOCQQSET:
        return scull_qset;

      case SCULL_IOCXQSET:
        tmp = scull_qset;
        scull_qset = get_user((int *)arg);
        put_user(tmp, (int *)arg);
        break;

      case SCULL_IOCHQSET:
        tmp = scull_qset;
        scull_quantum = arg;
        return tmp;

        /*
         * The following two change the buffer size for scullpipe.
         * The scullpipe device uses this same ioctl method, just to
         * write less code. Actually, it's the same driver, isn't it?
         */

      case SCULL_P_IOCTSIZE:
        scull_p_buffer = arg;
        break;

      case SCULL_P_IOCQSIZE:
        return scull_p_buffer;


      default:  /* redundant, as cmd was checked against MAXNR */
        return -ENOTTY;
    }
    return 0;

}


#endif /* LINUX_20 */


/*
 * The "extended" operations -- only seek
 */

loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
    Scull_Dev *dev = filp->private_data;
    loff_t newpos;

    switch(whence) {
      case 0: /* SEEK_SET */
        newpos = off;
        break;

      case 1: /* SEEK_CUR */
        newpos = filp->f_pos + off;
        break;

      case 2: /* SEEK_END */
        newpos = dev->size + off;
        break;

      default: /* can't happen */
        return -EINVAL;
    }
    if (newpos<0) return -EINVAL;
    filp->f_pos = newpos;
    return newpos;
}


/*
 * The following wrappers are meant to make things work with 2.0 kernels
 */
#ifdef LINUX_20
int scull_lseek_20(struct inode *ino, struct file *f,
                off_t offset, int whence)
{
    return (int)scull_llseek(f, offset, whence);
}

int scull_read_20(struct inode *ino, struct file *f, char *buf, int count)
{
    return (int)scull_read(f, buf, count, &f->f_pos);
}

int scull_write_20(struct inode *ino, struct file *f, const char *b, int c)
{
    return (int)scull_write(f, b, c, &f->f_pos);
}

void scull_release_20(struct inode *ino, struct file *f)
{
    scull_release(ino, f);
}

/* Redefine "real" names to the 2.0 ones */
#define scull_llseek scull_lseek_20
#define scull_read scull_read_20
#define scull_write scull_write_20
#define scull_release scull_release_20
#define llseek lseek
#endif  /* LINUX_20 */

struct file_operations scull_fops = {
    llseek:     scull_llseek,
    read:       scull_read,
    write:      scull_write,
    ioctl:      scull_ioctl,
    open:       scull_open,
    release:    scull_release,
};

/*
 * Finally, the module stuff
 */

#ifdef CONFIG_DEVFS_FS
devfs_handle_t scull_devfs_dir;
static char devname[4];
#endif

/*
 * The cleanup function is used to handle initialization failures as well.
 * Thefore, it must be careful to work correctly even if some of the items
 * have not been initialized
 */
void scull_cleanup_module(void)
{
    int i;

#ifndef CONFIG_DEVFS_FS
    /* cleanup_module is never called if registering failed */
    unregister_chrdev(scull_major, "scull");
#endif

#ifdef SCULL_DEBUG /* use proc only if debugging */
    scull_remove_proc();
#endif
    if (scull_devices) {
        for (i=0; i<scull_nr_devs; i++) {
            scull_trim(scull_devices+i);
            /* the following line is only used for devfs */
            devfs_unregister(scull_devices[i].handle);
        }
        kfree(scull_devices);
    }

    /* and call the cleanup functions for friend devices */
    scull_p_cleanup();
    scull_access_cleanup();

    /* once again, only for devfs */
    devfs_unregister(scull_devfs_dir);

}


int scull_init_module(void)
{
    int result, i;

    SET_MODULE_OWNER(&scull_fops);
#ifdef CONFIG_DEVFS_FS
    /* If we have devfs, create /dev/scull to put files in there */
    scull_devfs_dir = devfs_mk_dir(NULL, "scull", NULL);
    if (!scull_devfs_dir) return -EBUSY; /* problem */

#else /* no devfs, do it the "classic" way  */    

    /*
     * Register your major, and accept a dynamic number. This is the
     * first thing to do, in order to avoid releasing other module's
     * fops in scull_cleanup_module()
     */
    result = register_chrdev(scull_major, "scull", &scull_fops);
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n",scull_major);
        return result;
    }
    if (scull_major == 0) scull_major = result; /* dynamic */

#endif /* CONFIG_DEVFS_FS */
    /* 
     * allocate the devices -- we can't have them static, as the number
     * can be specified at load time
     */
    scull_devices = kmalloc(scull_nr_devs * sizeof(Scull_Dev), GFP_KERNEL);
    if (!scull_devices) {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(Scull_Dev));
    for (i=0; i < scull_nr_devs; i++) {
        scull_devices[i].quantum = scull_quantum;
        scull_devices[i].qset = scull_qset;
        sema_init(&scull_devices[i].sem, 1);
#ifdef CONFIG_DEVFS_FS
        sprintf(devname, "%i", i);
        devfs_register(scull_devfs_dir, devname,
                       DEVFS_FL_AUTO_DEVNUM,
                       0, 0, S_IFCHR | S_IRUGO | S_IWUGO,
                       &scull_fops,
                       scull_devices+i);
#endif  
    }

    /* At this point call the init function for any friend device */
    if ( (result = scull_p_init()) )
        goto fail;
    if ( (result = scull_access_init()) )
        goto fail;
    /* ... */

#ifndef SCULL_DEBUG
    EXPORT_NO_SYMBOLS; /* otherwise, leave global symbols visible */
#endif

#ifdef SCULL_DEBUG /* only when debugging */
    scull_create_proc();
#endif

    return 0; /* succeed */

  fail:
    scull_cleanup_module();
    return result;
}



module_init(scull_init_module);
module_exit(scull_cleanup_module);
