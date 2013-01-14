/* -*- C -*-
 * main.c -- the bare scullp char module
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
 * $Id: _main.c.in,v 1.19 2001/07/18 22:28:18 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

/* modversion stuff: no #ifdef needed if 2.0 support is not needed */
#ifdef CONFIG_MODVERSIONS
#  include <linux/modversions.h>
#endif

#include <linux/kernel.h> /* printk() */
#include <linux/malloc.h> /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /* O_ACCMODE */

#include <asm/system.h>   /* cli(), *_flags */

#include "scullp.h"        /* local definitions */



/*
 * I don't use static symbols here, because register_symtab is called
 */

int scullp_major =   SCULLP_MAJOR;
int scullp_devs =    SCULLP_DEVS;    /* number of bare scullp devices */
int scullp_qset =    SCULLP_QSET;
int scullp_order =   SCULLP_ORDER;

MODULE_PARM(scullp_major, "i");
MODULE_PARM(scullp_devs, "i");
MODULE_PARM(scullp_qset, "i");
MODULE_PARM(scullp_order, "i");
MODULE_AUTHOR("Alessandro Rubini");


ScullP_Dev *scullp_devices; /* allocated in scullp_init */

int scullp_trim(ScullP_Dev *dev);


#ifdef SCULLP_USE_PROC /* don't waste space if unused */
/*
 * The proc filesystem: function to read and entry
 */

void scullp_proc_offset(char *buf, char **start, off_t *offset, int *len)
{
    if (*offset == 0)
        return;
    if (*offset >= *len) {      /* Not there yet */
        *offset -= *len;
        *len = 0;
    }
    else {                      /* We're into the interesting stuff now */
        *start = buf + *offset;
        *offset = 0;
    }
}

int scullp_read_procmem(char *buf, char **start, off_t offset,
                   int count, int *eof, void *data)
{
    int i, j, order, qset, len = 0;
    int limit = count - 80; /* Don't print more than this */
    ScullP_Dev *d;

    *start = buf;
    for(i=0; i<scullp_devs; i++) {
        d=&scullp_devices[i];
        if (down_interruptible (&d->sem))
                return -ERESTARTSYS;
        qset=d->qset;  /* retrieve the features of each device */
        order=d->order;
        len += sprintf(buf+len,"\nDevice %i: qset %i, order %i, sz %li\n",
                       i, qset, order, (long)(d->size));
        for (; d; d=d->next) { /* scan the list */
            len += sprintf(buf+len,"  item at %p, qset at %p\n",d,d->data);
            scullp_proc_offset (buf, start, &offset, &len);
            if (len > limit)
                goto out;
            if (d->data && !d->next) /* dump only the last item - save space */
                for (j=0; j<qset; j++) {
                    if (d->data[j])
                        len += sprintf(buf+len,"    % 4i:%8p\n",j,d->data[j]);
                    scullp_proc_offset (buf, start, &offset, &len);
                    if (len > limit)
                        goto out;
		}
        }
    out:
        up (&scullp_devices[i].sem);
	if (len > limit)
	    break;
    }
    *eof = 1;
    return len;
}

#ifdef USE_PROC_REGISTER

static int scullp_get_info (char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof = 0;
    return scullp_read_procmem(buf, start, offset, len, &eof, NULL);
}


struct proc_dir_entry scullp_proc_entry = {
        0,                 /* low_ino: the inode -- dynamic */
        9, "scullpmem",     /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        scullp_get_info,   /* function used to read data */
        /* nothing more */
    };


static inline void create_proc_read_entry (const char *name, mode_t mode,
                struct proc_dir_entry *base, void *read_func, void *data)
{
    proc_register_dynamic (&proc_root, &scullp_proc_entry);
}

static inline void remove_proc_entry (char *name, void *parent)
{
    proc_unregister (&proc_root, scullp_proc_entry.low_ino);
}

#endif /* USE_PROC_REGISTER */
#endif /* SCULLP_USE_PROC */

/*
 * Open and close
 */

int scullp_open (struct inode *inode, struct file *filp)
{
    int num = MINOR(inode->i_rdev);
    ScullP_Dev *dev; /* device information */

    /*  check the device number */
    if (num >= scullp_devs) return -ENODEV;
    dev = &scullp_devices[num];

    /* now trim to 0 the length of the device if open was write-only */
     if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
        if (down_interruptible (&dev->sem))
            return -ERESTARTSYS;
        scullp_trim(dev); /* ignore errors */
        up (&dev->sem);
    }

    /* and use filp->private_data to point to the device data */
    filp->private_data = dev;

    MOD_INC_USE_COUNT;
    return 0;          /* success */
}

int scullp_release (struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
    return 0;
}

/*
 * Follow the list 
 */
ScullP_Dev *scullp_follow(ScullP_Dev *dev, int n)
{
    while (n--) {
        if (!dev->next) {
            dev->next = kmalloc(sizeof(ScullP_Dev), GFP_KERNEL);
            memset(dev->next, 0, sizeof(ScullP_Dev));
        }
        dev = dev->next;
        continue;
    }
    return dev;
}

/*
 * Data management: read and write
 */

ssize_t scullp_read (struct file *filp, char *buf, size_t count,
                loff_t *f_pos)
{
    ScullP_Dev *dev = filp->private_data; /* the first listitem */
    ScullP_Dev *dptr;
    int quantum = PAGE_SIZE << dev->order;
    int qset = dev->qset;
    int itemsize = quantum * qset; /* how many bytes in the listitem */
    int item, s_pos, q_pos, rest;
    ssize_t retval = 0;

    if (down_interruptible (&dev->sem))
            return -ERESTARTSYS;
    if (*f_pos > dev->size) 
        goto nothing;
    if (*f_pos + count > dev->size)
        count = dev->size - *f_pos;
    /* find listitem, qset index, and offset in the quantum */
    item = ((long) *f_pos) / itemsize;
    rest = ((long) *f_pos) % itemsize;
    s_pos = rest / quantum; q_pos = rest % quantum;

    /* follow the list up to the right position (defined elsewhere) */
    dptr = scullp_follow(dev, item);

    if (!dptr->data)
        goto nothing; /* don't fill holes */
    if (!dptr->data[s_pos])
        goto nothing;
    if (count > quantum - q_pos)
        count = quantum - q_pos; /* read only up to the end of this quantum */

    if (copy_to_user (buf, dptr->data[s_pos]+q_pos, count)) {
        retval = -EFAULT;
        goto nothing;
    }
    up (&dev->sem);

    *f_pos += count;
    return count;

  nothing:
    up (&dev->sem);
    return retval;
}



ssize_t scullp_write (struct file *filp, const char *buf, size_t count,
                loff_t *f_pos)
{
    ScullP_Dev *dev = filp->private_data;
    ScullP_Dev *dptr;
    int quantum = PAGE_SIZE << dev->order;
    int qset = dev->qset;
    int itemsize = quantum * qset;
    int item, s_pos, q_pos, rest;
    ssize_t retval = -ENOMEM; /* our most likely error */

    if (down_interruptible (&dev->sem))
            return -ERESTARTSYS;

    /* find listitem, qset index and offset in the quantum */
    item = ((long) *f_pos) / itemsize;
    rest = ((long) *f_pos) % itemsize;
    s_pos = rest / quantum; q_pos = rest % quantum;

    /* follow the list up to the right position */
    dptr = scullp_follow(dev, item);
    if (!dptr->data) {
        dptr->data = kmalloc(qset * sizeof(void *), GFP_KERNEL);
        if (!dptr->data)
            goto nomem;
        memset(dptr->data, 0, qset * sizeof(char *));
    }
    /* Here's the allocation of a single quantum */
    if (!dptr->data[s_pos]) {
        dptr->data[s_pos] =
	  (void *)__get_free_pages(GFP_KERNEL, dptr->order);
        if (!dptr->data[s_pos])
            goto nomem;
        memset(dptr->data[s_pos], 0, PAGE_SIZE << dptr->order);
    }
    if (count > quantum - q_pos)
        count = quantum - q_pos; /* write only up to the end of this quantum */
    if (copy_from_user (dptr->data[s_pos]+q_pos, buf, count)) {
        retval = -EFAULT;
        goto nomem;
    }
    *f_pos += count;
 
    /* update the size */
    if (dev->size < *f_pos)
        dev->size = *f_pos;
    up (&dev->sem);
    return count;

  nomem:
    up (&dev->sem);
    return retval;
}

/*
 * The ioctl() implementation
 */

int scullp_ioctl (struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

    int err= 0, ret = 0, tmp;

    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (_IOC_TYPE(cmd) != SCULLP_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > SCULLP_IOC_MAXNR) return -ENOTTY;

    /*
     * the type is a bitmask, and VERIFY_WRITE catches R/W
     * transfers. Note that the type is user-oriented, while
     * verify_area is kernel-oriented, so the concept of "read" and
     * "write" is reversed
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err =  !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if (err) return -EFAULT;

    switch(cmd) {

      case SCULLP_IOCRESET:
        scullp_qset = SCULLP_QSET;
        scullp_order = SCULLP_ORDER;
        break;
        
      case SCULLP_IOCSORDER: /* Set: arg points to the value */
        ret = __GET_USER(scullp_order, (int *) arg);
        break;

      case SCULLP_IOCTORDER: /* Tell: arg is the value */
        scullp_order = arg;
        break;

      case SCULLP_IOCGORDER: /* Get: arg is pointer to result */
        ret = __PUT_USER (scullp_order, (int *) arg);
        break;

      case SCULLP_IOCQORDER: /* Query: return it (it's positive) */
        return scullp_order;

      case SCULLP_IOCXORDER: /* eXchange: use arg as pointer */
        tmp = scullp_order;
        ret = __GET_USER(scullp_order, (int *) arg);
        if (ret == 0)
            ret = __PUT_USER(tmp, (int *) arg);
        break;

      case SCULLP_IOCHORDER: /* sHift: like Tell + Query */
        tmp = scullp_order;
        scullp_order = arg;
        return tmp;
        
      case SCULLP_IOCSQSET:
        ret = __GET_USER(scullp_qset, (int *) arg);
        break;

      case SCULLP_IOCTQSET:
        scullp_qset = arg;
        break;

      case SCULLP_IOCGQSET:
        ret = __PUT_USER(scullp_qset, (int *)arg);
        break;

      case SCULLP_IOCQQSET:
        return scullp_qset;

      case SCULLP_IOCXQSET:
        tmp = scullp_qset;
        ret = __GET_USER(scullp_qset, (int *) arg);
        if (ret == 0)
            ret = __PUT_USER(tmp, (int *)arg);
        break;

      case SCULLP_IOCHQSET:
        tmp = scullp_qset;
        scullp_qset = arg;
        return tmp;

      default:  /* redundant, as cmd was checked against MAXNR */
        return -ENOTTY;
    }

    return ret;

}

/*
 * The "extended" operations
 */

loff_t scullp_llseek (struct file *filp, loff_t off, int whence)
{
    ScullP_Dev *dev = filp->private_data;
    long newpos;

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
 * Mmap *is* available, but confined in a different file
 */
#ifndef LINUX_20
extern int scullp_mmap(struct file *filp, struct vm_area_struct *vma);
#else
extern int scullp_mmap(struct inode *inode, struct file *filp,
		struct vm_area_struct *vma);
#endif

/*
 * The 2.0 wrappers
 */
#ifdef LINUX_20

int scullp_lseek_20 (struct inode *ino, struct file *f,
                off_t offset, int whence)
{
    return (int)scullp_llseek(f, offset, whence);
}

int scullp_read_20 (struct inode *ino, struct file *f, char *buf, int count)
{
    return (int)scullp_read(f, buf, count, &f->f_pos);
}

int scullp_write_20 (struct inode *ino, struct file *f, const char *b, int c)
{
    return (int)scullp_write(f, b, c, &f->f_pos);
}

void scullp_release_20 (struct inode *ino, struct file *f)
{
    scullp_release(ino, f);
}

#define scullp_llseek scullp_lseek_20
#define scullp_read scullp_read_20
#define scullp_write scullp_write_20
#define scullp_release scullp_release_20
#define llseek lseek

#endif /* LINUX_20 */

/*
 * The fops
 */

struct file_operations scullp_fops = {
    llseek: scullp_llseek,
    read: scullp_read,
    write: scullp_write,
    ioctl: scullp_ioctl,
    mmap: scullp_mmap,
    open: scullp_open,
    release: scullp_release,
};

int scullp_trim(ScullP_Dev *dev)
{
    ScullP_Dev *next, *dptr;
    int qset = dev->qset;   /* "dev" is not-null */
    int i;

    if (dev->vmas) /* don't trim: there are active mappings */
        return -EBUSY;

    for (dptr = dev; dptr; dptr = next) { /* all the list items */
        if (dptr->data) {
            /* This code frees a whole quantum-set */
            for (i = 0; i < qset; i++)
                if (dptr->data[i])
                    free_pages((unsigned long)(dptr->data[i]),
                               dptr->order);

            kfree(dptr->data);
            dptr->data=NULL;
        }
        next=dptr->next;
        if (dptr != dev) kfree(dptr); /* all of them but the first */
    }
    dev->size = 0;
    dev->qset = scullp_qset;
    dev->order = scullp_order;
    dev->next = NULL;
    return 0;
}




/*
 * Finally, the module stuff
 */

int scullp_init(void)
{
    int result, i;

    SET_MODULE_OWNER(&scullp_fops);
    /*
     * Register your major, and accept a dynamic number
     */
    result = register_chrdev(scullp_major, "scullp", &scullp_fops);
    if (result < 0) return result;
    if (scullp_major == 0) scullp_major = result; /* dynamic */

    /* 
     * allocate the devices -- we can't have them static, as the number
     * can be specified at load time
     */
    scullp_devices = kmalloc(scullp_devs * sizeof (ScullP_Dev), GFP_KERNEL);
    if (!scullp_devices) {
        result = -ENOMEM;
        goto fail_malloc;
    }
    memset(scullp_devices, 0, scullp_devs * sizeof (ScullP_Dev));
    for (i=0; i < scullp_devs; i++) {
        scullp_devices[i].order = scullp_order;
        scullp_devices[i].qset = scullp_qset;
        sema_init (&scullp_devices[i].sem, 1);
    }


#ifdef SCULLP_USE_PROC /* only when available */
    create_proc_read_entry("scullpmem", 0, NULL, scullp_read_procmem, NULL);
#endif
    return 0; /* succeed */

  fail_malloc:
    unregister_chrdev(scullp_major, "scullp");
    return result;
}

void scullp_cleanup(void)
{
    int i;
    unregister_chrdev(scullp_major, "scullp");

#ifdef SCULLP_USE_PROC
    remove_proc_entry("scullpmem", 0);
#endif

    for (i=0; i<scullp_devs; i++)
        scullp_trim(scullp_devices+i);
    kfree(scullp_devices);
}


module_init(scullp_init);
module_exit(scullp_cleanup);
