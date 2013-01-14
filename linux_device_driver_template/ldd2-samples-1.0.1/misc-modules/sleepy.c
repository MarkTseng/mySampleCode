/*
 * sleepy.c -- the writers awake the readers
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
 * $Id: sleepy.c,v 1.13 2001/07/18 22:28:16 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>

#include <linux/sched.h>  /* current and everything */
#include <linux/kernel.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/types.h>  /* size_t */

#include "sysdep.h"

int sleepy_major=0;

DECLARE_WAIT_QUEUE_HEAD(wq);

ssize_t sleepy_read (struct file *filp, char *buf, size_t count, loff_t *pos)
{
    printk(KERN_DEBUG "process %i (%s) going to sleep\n",
           current->pid, current->comm);
    interruptible_sleep_on(&wq);
    printk(KERN_DEBUG "awoken %i (%s)\n", current->pid, current->comm);
    return 0; /* EOF */
}

ssize_t sleepy_write (struct file *filp, const char *buf, size_t count,
		loff_t *pos)
{
    printk(KERN_DEBUG "process %i (%s) awakening the readers...\n",
           current->pid, current->comm);
    wake_up_interruptible(&wq);
    return count; /* succeed, to avoid retrial */
}


#ifdef LINUX_20 /* wrappers for 2.0 */
int sleepy_read_20 (struct inode *ino, struct file *f, char *buf, int count)
{
    return (int)sleepy_read(f, buf, count, &f->f_pos);
}

int sleepy_write_20 (struct inode *ino, struct file *f, const char *b, int c)
{
    return (int)sleepy_write(f, b, c, &f->f_pos);
}

#define sleepy_read sleepy_read_20
#define sleepy_write sleepy_write_20
#endif  /* LINUX_20 */


struct file_operations sleepy_fops = {
    read:  sleepy_read,
    write: sleepy_write,
};


int sleepy_init(void)
{
    int result;

    SET_MODULE_OWNER(&sleepy_fops);
    /*
     * Register your major, and accept a dynamic number
     */
    result = register_chrdev(sleepy_major, "sleepy", &sleepy_fops);
    if (result < 0) return result;
    if (sleepy_major == 0) sleepy_major = result; /* dynamic */

    return 0;
}

void sleepy_cleanup(void)
{
    unregister_chrdev(sleepy_major, "sleepy");
}

module_init(sleepy_init);
module_exit(sleepy_cleanup);

