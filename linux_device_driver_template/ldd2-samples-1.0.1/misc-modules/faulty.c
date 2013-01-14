/*
 * faulty.c -- a module which generates an oops when read
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
 * $Id: faulty.c,v 1.15 2001/07/18 22:28:16 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/kernel.h> /* printk() */
#include <linux/sched.h> /* printk() */
#include <linux/fs.h>     /* everything... */
#include <linux/types.h>  /* size_t */

#include "sysdep.h" /* linux 2.0, 2.2, 2.4 compatibility macros */

int faulty_major=0;

char faulty_buf[1024];

ssize_t faulty_read (struct file *filp, char *buf, size_t count, loff_t *pos)
{
    int ret, ret2;
    char stack_buf[4];

    printk(KERN_DEBUG "read: buf %p, count %li\n", buf, (long)count);
    /* the next line oopses with 2.0, but not with 2.2 and later */
    ret = copy_to_user(buf, faulty_buf, count);
    if (!ret) return count; /* we survived */

    printk(KERN_DEBUG "didn't fail: retry\n");
    /* For 2.2 and 2.4, let's try a buffer overflow  */
    sprintf(stack_buf, "1234567\n");
    if (count > 8) count = 8; /* copy 8 bytes to the user */
    ret2 = copy_to_user(buf, stack_buf, count);
    if (!ret2) return count;
    return ret2;
}

ssize_t faulty_write (struct file *filp, const char *buf, size_t count,
		loff_t *pos)
{
    /* make a simple fault by dereferencing a NULL pointer */
    *(int *)0 = 0;
    return 0;
}

#ifdef LINUX_20 /* wrappers for 2.0 */
int faulty_read_20 (struct inode *ino, struct file *f, char *buf, int count)
{
    return (int)faulty_read(f, buf, count, &f->f_pos);
}

int faulty_write_20 (struct inode *ino, struct file *f, const char *b, int c)
{
    return (int)faulty_write(f, b, c, &f->f_pos);
}


#define faulty_read faulty_read_20
#define faulty_write faulty_write_20
#endif  /* LINUX_20 */


struct file_operations faulty_fops = {
    read:  faulty_read,
    write: faulty_write,
};


int faulty_init(void)
{
    int result;

    /*
     * Register your major, and accept a dynamic number
     */
    SET_MODULE_OWNER(&faulty_fops);
    result = register_chrdev(faulty_major, "faulty", &faulty_fops);
    if (result < 0) return result;
    if (faulty_major == 0) faulty_major = result; /* dynamic */

    return 0;
}

void faulty_cleanup(void)
{
    unregister_chrdev(faulty_major, "faulty");
}

module_init(faulty_init);
module_exit(faulty_cleanup);

