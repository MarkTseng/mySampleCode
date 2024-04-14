/* -*- C -*-
 * main.c -- the bare scullc char module
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
 * $Id: _main.c.in,v 1.21 2004/10/14 20:11:39 corbet Exp $
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/aio.h>
#include <asm/uaccess.h>
#include "scullc.h"		/* local definitions */


int scullc_major =   SCULLC_MAJOR;
int scullc_devs =    SCULLC_DEVS;	/* number of bare scullc devices */
int scullc_qset =    SCULLC_QSET;
int scullc_quantum = SCULLC_QUANTUM;

module_param(scullc_major, int, 0);
module_param(scullc_devs, int, 0);
module_param(scullc_qset, int, 0);
module_param(scullc_quantum, int, 0);
MODULE_AUTHOR("Alessandro Rubini");
MODULE_LICENSE("Dual BSD/GPL");

struct scullc_dev *scullc_devices; /* allocated in scullc_init */

int scullc_trim(struct scullc_dev *dev);
void scullc_cleanup(void);

/* declare one cache pointer: use it for all devices */
kmem_cache_t *scullc_cache;





#ifdef SCULLC_USE_PROC /* don't waste space if unused */
/*
 * The proc filesystem: function to read and entry
 */

void scullc_proc_offset(char *buf, char **start, off_t *offset, int *len)
{
	if (*offset == 0)
		return;
	if (*offset >= *len) {
		/* Not there yet */
		*offset -= *len;
		*len = 0;
	} else {
		/* We're into the interesting stuff now */
		*start = buf + *offset;
		*offset = 0;
	}
}

/* FIXME: Do we need this here??  It be ugly  */
int scullc_read_procmem(char *buf, char **start, off_t offset,
                   int count, int *eof, void *data)
{
	int i, j, quantum, qset, len = 0;
	int limit = count - 80; /* Don't print more than this */
	struct scullc_dev *d;

	*start = buf;
	for(i = 0; i < scullc_devs; i++) {
		d = &scullc_devices[i];
		if (down_interruptible (&d->sem))
			return -ERESTARTSYS;
		qset = d->qset;  /* retrieve the features of each device */
		quantum=d->quantum;
		len += sprintf(buf+len,"\nDevice %i: qset %i, quantum %i, sz %li\n",
				i, qset, quantum, (long)(d->size));
		for (; d; d = d->next) { /* scan the list */
			len += sprintf(buf+len,"  item at %p, qset at %p\n",d,d->data);
			scullc_proc_offset (buf, start, &offset, &len);
			if (len > limit)
				goto out;
			if (d->data && !d->next) /* dump only the last item - save space */
				for (j = 0; j < qset; j++) {
					if (d->data[j])
						len += sprintf(buf+len,"    % 4i:%8p\n",j,d->data[j]);
					scullc_proc_offset (buf, start, &offset, &len);
					if (len > limit)
						goto out;
				}
		}
	  out:
		up (&scullc_devices[i].sem);
		if (len > limit)
			break;
	}
	*eof = 1;
	return len;
}

#endif /* SCULLC_USE_PROC */

/*
 * Open and close
 */

int scullc_open (struct inode *inode, struct file *filp)
{
	struct scullc_dev *dev; /* device information */

	/*  Find the device */
	dev = container_of(inode->i_cdev, struct scullc_dev, cdev);

    	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (down_interruptible (&dev->sem))
			return -ERESTARTSYS;
		scullc_trim(dev); /* ignore errors */
		up (&dev->sem);
	}

	/* and use filp->private_data to point to the device data */
	filp->private_data = dev;

	return 0;          /* success */
}

int scullc_release (struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * Follow the list 
 */
struct scullc_dev *scullc_follow(struct scullc_dev *dev, int n)
{
	while (n--) {
		if (!dev->next) {
			dev->next = kmalloc(sizeof(struct scullc_dev), GFP_KERNEL);
			memset(dev->next, 0, sizeof(struct scullc_dev));
		}
		dev = dev->next;
		continue;
	}
	return dev;
}

/*
 * Data management: read and write
 */

ssize_t scullc_read (struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scullc_dev *dev = filp->private_data; /* the first listitem */
	struct scullc_dev *dptr;
	int quantum = dev->quantum;
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
	dptr = scullc_follow(dev, item);

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



ssize_t scullc_write (struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scullc_dev *dev = filp->private_data;
	struct scullc_dev *dptr;
	int quantum = dev->quantum;
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
	dptr = scullc_follow(dev, item);
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(void *), GFP_KERNEL);
		if (!dptr->data)
			goto nomem;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	/* Allocate a quantum using the memory cache */
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmem_cache_alloc(scullc_cache, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto nomem;
		memset(dptr->data[s_pos], 0, scullc_quantum);
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

int scullc_ioctl (struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

	int err = 0, ret = 0, tmp;

	/* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
	if (_IOC_TYPE(cmd) != SCULLC_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULLC_IOC_MAXNR) return -ENOTTY;

	/*
	 * the type is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. Note that the type is user-oriented, while
	 * verify_area is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err)
		return -EFAULT;

	switch(cmd) {

	case SCULLC_IOCRESET:
		scullc_qset = SCULLC_QSET;
		scullc_quantum = SCULLC_QUANTUM;
		break;

	case SCULLC_IOCSQUANTUM: /* Set: arg points to the value */
		ret = __get_user(scullc_quantum, (int __user *) arg);
		break;

	case SCULLC_IOCTQUANTUM: /* Tell: arg is the value */
		scullc_quantum = arg;
		break;

	case SCULLC_IOCGQUANTUM: /* Get: arg is pointer to result */
		ret = __put_user (scullc_quantum, (int __user *) arg);
		break;

	case SCULLC_IOCQQUANTUM: /* Query: return it (it's positive) */
		return scullc_quantum;

	case SCULLC_IOCXQUANTUM: /* eXchange: use arg as pointer */
		tmp = scullc_quantum;
		ret = __get_user(scullc_quantum, (int __user *) arg);
		if (ret == 0)
			ret = __put_user(tmp, (int __user *) arg);
		break;

	case SCULLC_IOCHQUANTUM: /* sHift: like Tell + Query */
		tmp = scullc_quantum;
		scullc_quantum = arg;
		return tmp;

	case SCULLC_IOCSQSET:
		ret = __get_user(scullc_qset, (int __user *) arg);
		break;

	case SCULLC_IOCTQSET:
		scullc_qset = arg;
		break;

	case SCULLC_IOCGQSET:
		ret = __put_user(scullc_qset, (int __user *)arg);
		break;

	case SCULLC_IOCQQSET:
		return scullc_qset;

	case SCULLC_IOCXQSET:
		tmp = scullc_qset;
		ret = __get_user(scullc_qset, (int __user *)arg);
		if (ret == 0)
			ret = __put_user(tmp, (int __user *)arg);
		break;

	case SCULLC_IOCHQSET:
		tmp = scullc_qset;
		scullc_qset = arg;
		return tmp;

	default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}

	return ret;
}

/*
 * The "extended" operations
 */

loff_t scullc_llseek (struct file *filp, loff_t off, int whence)
{
	struct scullc_dev *dev = filp->private_data;
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
 * A simple asynchronous I/O implementation.
 */

struct async_work {
	struct kiocb *iocb;
	int result;
	struct work_struct work;
};

/*
 * "Complete" an asynchronous operation.
 */
static void scullc_do_deferred_op(void *p)
{
	struct async_work *stuff = (struct async_work *) p;
	aio_complete(stuff->iocb, stuff->result, 0);
	kfree(stuff);
}


static int scullc_defer_op(int write, struct kiocb *iocb, char __user *buf,
		size_t count, loff_t pos)
{
	struct async_work *stuff;
	int result;

	/* Copy now while we can access the buffer */
	if (write)
		result = scullc_write(iocb->ki_filp, buf, count, &pos);
	else
		result = scullc_read(iocb->ki_filp, buf, count, &pos);

	/* If this is a synchronous IOCB, we return our status now. */
	if (is_sync_kiocb(iocb))
		return result;

	/* Otherwise defer the completion for a few milliseconds. */
	stuff = kmalloc (sizeof (*stuff), GFP_KERNEL);
	if (stuff == NULL)
		return result; /* No memory, just complete now */
	stuff->iocb = iocb;
	stuff->result = result;
	INIT_WORK(&stuff->work, scullc_do_deferred_op, stuff);
	schedule_delayed_work(&stuff->work, HZ/100);
	return -EIOCBQUEUED;
}


static ssize_t scullc_aio_read(struct kiocb *iocb, char __user *buf, size_t count,
		loff_t pos)
{
	return scullc_defer_op(0, iocb, buf, count, pos);
}

static ssize_t scullc_aio_write(struct kiocb *iocb, const char __user *buf,
		size_t count, loff_t pos)
{
	return scullc_defer_op(1, iocb, (char __user *) buf, count, pos);
}


 

/*
 * The fops
 */

struct file_operations scullc_fops = {
	.owner =     THIS_MODULE,
	.llseek =    scullc_llseek,
	.read =	     scullc_read,
	.write =     scullc_write,
	.ioctl =     scullc_ioctl,
	.open =	     scullc_open,
	.release =   scullc_release,
	.aio_read =  scullc_aio_read,
	.aio_write = scullc_aio_write,
};

int scullc_trim(struct scullc_dev *dev)
{
	struct scullc_dev *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	if (dev->vmas) /* don't trim: there are active mappings */
		return -EBUSY;

	for (dptr = dev; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				if (dptr->data[i])
					kmem_cache_free(scullc_cache, dptr->data[i]);

			kfree(dptr->data);
			dptr->data=NULL;
		}
		next=dptr->next;
		if (dptr != dev) kfree(dptr); /* all of them but the first */
	}
	dev->size = 0;
	dev->qset = scullc_qset;
	dev->quantum = scullc_quantum;
	dev->next = NULL;
	return 0;
}


static void scullc_setup_cdev(struct scullc_dev *dev, int index)
{
	int err, devno = MKDEV(scullc_major, index);
    
	cdev_init(&dev->cdev, &scullc_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scullc_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}



/*
 * Finally, the module stuff
 */

int scullc_init(void)
{
	int result, i;
	dev_t dev = MKDEV(scullc_major, 0);
	
	/*
	 * Register your major, and accept a dynamic number.
	 */
	if (scullc_major)
		result = register_chrdev_region(dev, scullc_devs, "scullc");
	else {
		result = alloc_chrdev_region(&dev, 0, scullc_devs, "scullc");
		scullc_major = MAJOR(dev);
	}
	if (result < 0)
		return result;

	
	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	scullc_devices = kmalloc(scullc_devs*sizeof (struct scullc_dev), GFP_KERNEL);
	if (!scullc_devices) {
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(scullc_devices, 0, scullc_devs*sizeof (struct scullc_dev));
	for (i = 0; i < scullc_devs; i++) {
		scullc_devices[i].quantum = scullc_quantum;
		scullc_devices[i].qset = scullc_qset;
		sema_init (&scullc_devices[i].sem, 1);
		scullc_setup_cdev(scullc_devices + i, i);
	}

	scullc_cache = kmem_cache_create("scullc", scullc_quantum,
			0, SLAB_HWCACHE_ALIGN, NULL, NULL); /* no ctor/dtor */
	if (!scullc_cache) {
		scullc_cleanup();
		return -ENOMEM;
	}

#ifdef SCULLC_USE_PROC /* only when available */
	create_proc_read_entry("scullcmem", 0, NULL, scullc_read_procmem, NULL);
#endif
	return 0; /* succeed */

  fail_malloc:
	unregister_chrdev_region(dev, scullc_devs);
	return result;
}



void scullc_cleanup(void)
{
	int i;

#ifdef SCULLC_USE_PROC
	remove_proc_entry("scullcmem", NULL);
#endif

	for (i = 0; i < scullc_devs; i++) {
		cdev_del(&scullc_devices[i].cdev);
		scullc_trim(scullc_devices + i);
	}
	kfree(scullc_devices);

	if (scullc_cache)
		kmem_cache_destroy(scullc_cache);
	unregister_chrdev_region(MKDEV (scullc_major, 0), scullc_devs);
}


module_init(scullc_init);
module_exit(scullc_cleanup);
