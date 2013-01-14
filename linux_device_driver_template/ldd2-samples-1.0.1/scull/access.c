/*
 * access.c -- the files with access control on open
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
 * $Id: access.c,v 1.21 2001/07/18 22:28:16 rubini Exp $
 */

 
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#define __NO_VERSION__
#include <linux/module.h> /* get MOD_DEC_USE_COUNT, not the version string */
#include <linux/version.h> /* need it for conditionals in scull.h */

#include <linux/kernel.h> /* printk() */
#include <linux/malloc.h> /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/fcntl.h>
#include <linux/tty.h>    /* current->tty */

#include "scull.h"        /* local definitions */

/*
 * These devices fall back on the main scull operations. They only
 * differ in the implementation of open() and close()
 */

/*
 * The following deals with some of the 2.2 API changes.
 */
#ifdef LINUX_20

extern int scull_lseek_20(struct inode *ino, struct file *f, off_t offset,
                int whence);
extern int scull_read_20(struct inode *ino, struct file *f, char *buf,
                int count);
extern int scull_write_20(struct inode *ino, struct file *f, const char *buf,
                int count);

#define scull_llseek scull_lseek_20
#define scull_read   scull_read_20
#define scull_write  scull_write_20
#endif



/************************************************************************
 *
 * The first device is the single-open one,
 *  it has an hw structure and an open count
 */

Scull_Dev scull_s_device;
int scull_s_count = 0;
spinlock_t scull_s_lock;

int scull_s_open(struct inode *inode, struct file *filp)
{
    Scull_Dev *dev = &scull_s_device; /* device information */
    int num = NUM(inode->i_rdev);

    if (!filp->private_data && num > 0)
        return -ENODEV; /* not devfs: allow 1 device only */
    spin_lock(&scull_s_lock);
    if (scull_s_count) {
        spin_unlock(&scull_s_lock);
        return -EBUSY; /* already open */
    }
    scull_s_count++;
    spin_unlock(&scull_s_lock);
    /* then, everything else is copied from the bare scull device */

    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
        scull_trim(dev);
    if (!filp->private_data)
        filp->private_data = dev;
    MOD_INC_USE_COUNT;
    return 0;          /* success */
}

int scull_s_release(struct inode *inode, struct file *filp)
{
    scull_s_count--; /* release the device */
    MOD_DEC_USE_COUNT;
    return 0;
}

#ifdef LINUX_20

void scull_s_release_20(struct inode *ino, struct file *f)
{
    scull_s_release(ino, f);
}
#define scull_s_release scull_s_release_20
#define llseek lseek
#endif

/*
 * The other operations for the single-open device come from the bare device
 */
struct file_operations scull_sngl_fops = {
    llseek:     scull_llseek,
    read:       scull_read,
    write:      scull_write,
    ioctl:      scull_ioctl,
    open:       scull_s_open,
    release:    scull_s_release,
};


/************************************************************************
 *
 * Next, the "uid" device. It can be opened multiple times by the
 * same user, but access is denied to other users if the device is open
 */

Scull_Dev scull_u_device;
int scull_u_count = 0;
uid_t scull_u_owner = 0;
spinlock_t scull_u_lock;

int scull_u_open(struct inode *inode, struct file *filp)
{
    Scull_Dev *dev = &scull_u_device; /* device information */
    int num = NUM(inode->i_rdev);

    if (!filp->private_data && num > 0)
        return -ENODEV; /* not devfs: allow 1 device only */
    spin_lock(&scull_u_lock);
    if (scull_u_count && 
        (scull_u_owner != current->uid) &&  /* allow user */
        (scull_u_owner != current->euid) && /* allow whoever did su */
                    !capable(CAP_DAC_OVERRIDE)) { /* still allow root */
            spin_unlock(&scull_u_lock);
            return -EBUSY;   /* -EPERM would confuse the user */
    }

    if (scull_u_count == 0)
        scull_u_owner = current->uid; /* grab it */

    scull_u_count++;
    spin_unlock(&scull_u_lock);

    /* then, everything else is copied from the bare scull device */

    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
        scull_trim(dev);
    if (!filp->private_data)
        filp->private_data = dev;
    MOD_INC_USE_COUNT;
    return 0;          /* success */
}

int scull_u_release(struct inode *inode, struct file *filp)
{
    scull_u_count--; /* nothing else */
    MOD_DEC_USE_COUNT;
    return 0;
}

#ifdef LINUX_20

void scull_u_release_20(struct inode *ino, struct file *f)
{
    scull_u_release(ino, f);
}
#define scull_u_release scull_u_release_20

#endif



/*
 * The other operations for the device come from the bare device
 */
struct file_operations scull_user_fops = {
    llseek:     scull_llseek,
    read:       scull_read,
    write:      scull_write,
    ioctl:      scull_ioctl,
    open:       scull_u_open,
    release:    scull_u_release,
};


/************************************************************************
 *
 * Next, the device with blocking-open based on uid
 */

Scull_Dev scull_w_device;
int scull_w_count = 0;
uid_t scull_w_owner = 0;
static DECLARE_WAIT_QUEUE_HEAD(scull_w_wait);
spinlock_t scull_w_lock;

int scull_w_open(struct inode *inode, struct file *filp)
{
    Scull_Dev *dev = &scull_w_device; /* device information */
    int num = NUM(inode->i_rdev);

    if (!filp->private_data && num > 0)
        return -ENODEV; /* not devfs: allow 1 device only */
    spin_lock(&scull_w_lock);
    while (scull_w_count && 
      (scull_w_owner != current->uid) &&  /* allow user */
      (scull_w_owner != current->euid) && /* allow whoever did su */
      !capable(CAP_DAC_OVERRIDE)) {
        spin_unlock(&scull_w_lock);
        if (filp->f_flags & O_NONBLOCK) return -EAGAIN; 
        interruptible_sleep_on(&scull_w_wait);
        if (signal_pending(current)) /* a signal arrived */
          return -ERESTARTSYS; /* tell the fs layer to handle it */
        /* else, loop */
        spin_lock(&scull_w_lock);
    }
    if (scull_w_count == 0)
        scull_w_owner = current->uid; /* grab it */
    scull_w_count++;
    spin_unlock(&scull_w_lock);

    /* then, everything else is copied from the bare scull device */

    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
        scull_trim(dev);
    if (!filp->private_data)
        filp->private_data = dev;
    MOD_INC_USE_COUNT;
    return 0;          /* success */
}

int scull_w_release(struct inode *inode, struct file *filp)
{
    scull_w_count--;
    if (scull_w_count == 0)
        wake_up_interruptible(&scull_w_wait); /* awake other uid's */
    MOD_DEC_USE_COUNT;
    return 0;
}


#ifdef LINUX_20

void scull_w_release_20(struct inode *ino, struct file *f)
{
    scull_w_release(ino, f);
}
#define scull_w_release scull_w_release_20

#endif



/*
 * The other operations for the device come from the bare device
 */
struct file_operations scull_wusr_fops = {
    llseek:     scull_llseek,
    read:       scull_read,
    write:      scull_write,
    ioctl:      scull_ioctl,
    open:       scull_w_open,
    release:    scull_w_release,
};

/************************************************************************
 *
 * Finally the `cloned' private device. This is trickier because it
 * involves list management, and dynamic allocation.
 */
devfs_handle_t scull_priv_handle;    /* only used if devfs is there */

/* The clone-specific data structure includes a key field */
struct scull_listitem {
    Scull_Dev device;
    int key;
    struct scull_listitem *next;
    
};

/* The list of devices, and a lock to protect it */
struct scull_listitem *scull_c_head;
spinlock_t scull_c_lock;

/* Look for a device or create one if missing */
static Scull_Dev *scull_c_lookfor_device(int key)
{
    struct scull_listitem *lptr, *prev = NULL;

    for (lptr = scull_c_head; lptr && (lptr->key != key); lptr = lptr->next)
        prev=lptr;
    if (lptr) return &(lptr->device);

    /* not found */
    lptr = kmalloc(sizeof(struct scull_listitem), GFP_ATOMIC);
    if (!lptr) return NULL;

    /* initialize the device */
    memset(lptr, 0, sizeof(struct scull_listitem));
    lptr->key = key;
    scull_trim(&(lptr->device)); /* initialize it */
    sema_init(&(lptr->device.sem), 1);

    /* place it in the list */
    if (prev)  prev->next = lptr;
    else       scull_c_head = lptr;

    return &(lptr->device);
}

int scull_c_open(struct inode *inode, struct file *filp)
{
    Scull_Dev *dev;
    int key, num = NUM(inode->i_rdev);
 
    if (!filp->private_data && num > 0)
        return -ENODEV; /* not devfs: allow 1 device only */

    if (!current->tty) { 
        PDEBUG("Process \"%s\" has no ctl tty\n",current->comm);
        return -EINVAL;
    }
    key = MINOR(current->tty->device);

    /* look for a scullc device in the list */
    spin_lock(&scull_c_lock);
    dev = scull_c_lookfor_device(key);
    spin_unlock(&scull_c_lock);

    if (!dev) return -ENOMEM;

    /* then, everything else is copied from the bare scull device */
    if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
        scull_trim(dev);
    filp->private_data = dev;
    MOD_INC_USE_COUNT;
    return 0;          /* success */
}

int scull_c_release(struct inode *inode, struct file *filp)
{
    /*
     * Nothing to do, because the device is persistent.
     * A `real' cloned device should be freed on last close
     */
    MOD_DEC_USE_COUNT;
    return 0;
}


#ifdef LINUX_20

void scull_c_release_20(struct inode *ino, struct file *f)
{
    scull_c_release(ino, f);
}
#define scull_c_release scull_c_release_20

#endif



/*
 * The other operations for the device come from the bare device
 */
struct file_operations scull_priv_fops = {
    llseek:   scull_llseek,
    read:     scull_read,
    write:    scull_write,
    ioctl:    scull_ioctl,
    open:     scull_c_open,
    release:  scull_c_release,
};

/************************************************************************
 *
 * And the init and cleanup functions come last
 */

int scull_access_init(void)
{
    /* assign quantum and quantumset */
    scull_s_device.quantum = scull_quantum;
    scull_s_device.qset    = scull_qset;
    scull_u_device.quantum = scull_quantum;
    scull_u_device.qset    = scull_qset;
    scull_w_device.quantum = scull_quantum;
    scull_w_device.qset    = scull_qset;

    /* Initialize spinlocks */
    spin_lock_init(&scull_s_lock);
    spin_lock_init(&scull_u_lock);
    spin_lock_init(&scull_w_lock);
    spin_lock_init(&scull_c_lock);

    /* and semaphores (used by read and write) */
    sema_init(&scull_s_device.sem, 1);
    sema_init(&scull_u_device.sem, 1);
    sema_init(&scull_w_device.sem, 1);

    /* and file operations owners */
    SET_MODULE_OWNER(&scull_sngl_fops);
    SET_MODULE_OWNER(&scull_user_fops);
    SET_MODULE_OWNER(&scull_wusr_fops);
    SET_MODULE_OWNER(&scull_priv_fops);

#ifdef CONFIG_DEVFS_FS
    /* finally, create the devfs entry points */
    scull_s_device.handle =
        devfs_register(scull_devfs_dir, "single",
                       DEVFS_FL_AUTO_DEVNUM,
                       0, 0, S_IFCHR | S_IRUGO | S_IWUGO,
                       &scull_sngl_fops,
                       &scull_s_device);
    scull_u_device.handle =
        devfs_register(scull_devfs_dir, "user",
                       DEVFS_FL_AUTO_DEVNUM,
                       0, 0, S_IFCHR | S_IRUGO | S_IWUGO,
                       &scull_user_fops,
                       &scull_u_device);

    scull_w_device.handle =
        devfs_register(scull_devfs_dir, "wuser",
                       DEVFS_FL_AUTO_DEVNUM,
                       0, 0, S_IFCHR | S_IRUGO | S_IWUGO,
                       &scull_wusr_fops,
                       &scull_w_device);
    scull_priv_handle =
        devfs_register(scull_devfs_dir, "priv",
                       DEVFS_FL_AUTO_DEVNUM,
                       0, 0, S_IFCHR | S_IRUGO | S_IWUGO,
                       &scull_priv_fops,
                       &scull_priv_fops); /* any non-null value */
#endif    
    return 0;
}

/*
 * This is called by cleanup_module or on failure.
 * It is required to never fail, even if nothing was initialized first
 */
void scull_access_cleanup(void)
{
    struct scull_listitem *lptr, *prev;

    scull_trim(&scull_s_device); /* disallocate it */
    scull_trim(&scull_u_device); /* disallocate it */
    scull_trim(&scull_w_device); /* disallocate it */

    /* all the cloned devices */
    prev=NULL;
    for (lptr = scull_c_head; lptr; lptr = lptr->next) {
        scull_trim(&(lptr->device));
        if (prev) kfree(prev);
        prev=lptr;
    }
    if (prev) kfree(prev);
    scull_c_head = NULL; /* overkill: we're unloading anyways */

    /* remove devfs entry points */
    devfs_unregister(scull_s_device.handle);
    devfs_unregister(scull_u_device.handle);
    devfs_unregister(scull_w_device.handle);
    devfs_unregister(scull_priv_handle);

    return;
}




