/*
 * jit.c -- the just-in-time module
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
 * $Id: jit.c,v 1.11 2001/07/18 22:28:16 rubini Exp $
 */

/* BUGS: this module is not reentrant: only one file can be read at a time
 *       the module usage count is not used: you could remove the module
 *           while reading it, thus asking for troubles
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
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */

#include "sysdep.h"
/*
 * This module is a silly one: it only embeds short code fragments
 * that show how time delays can be handled in the kernel.
 */

int jit_delay = 1; /* the default delay in read() */
char *jit_spoke = "This is a meaningless string, fiftysix characters long.\n";

MODULE_PARM(jit_delay, "i");
MODULE_PARM(jit_spoke, "s");

MODULE_AUTHOR("Alessandro Rubini");


#define LIMIT (PAGE_SIZE-128) /* don't print any more after this size */

static int jit_print(char *buf)
{
    int len = 0;
    while(len < LIMIT)
        len += sprintf(buf+len,"%s",jit_spoke);
    return len;
}

int jit_read_busy(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    /* delay one second (or the chosen value), before printing */
    unsigned long j = jiffies + jit_delay * HZ;

    while (jiffies < j)
        /* nothing */;

    *eof = 1;
    return jit_print(buf);
}

int jit_read_sched(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    /* delay one second (or the chosen value), before printing */
    unsigned long j = jiffies + jit_delay * HZ;

    while (jiffies < j)
        schedule();

    *eof = 1;
    return jit_print(buf);
}

int jit_read_queue(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    /* delay one second (or the chosen value), before printing */
/*    unsigned long j = jiffies + jit_delay * HZ; */
    
    wait_queue_head_t wait;

    init_waitqueue_head (&wait);
    interruptible_sleep_on_timeout(&wait, jit_delay*HZ);

    *eof = 1;
    return jit_print(buf);
}



int jit_read_self(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    /* delay one second (or the chosen value), before printing */

    set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout (jit_delay*HZ);

    *eof = 1;
    return jit_print(buf);
}



#ifdef USE_PROC_REGISTER

static int jit_old_read_busy(char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof;
    return jit_read_busy(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jit_proc_busy = {
        0,                 /* low_ino: the inode -- dynamic */
        7, "jitbusy",      /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jit_old_read_busy,    /* function used to read data */
        /* nothing more */
    };

static int jit_old_read_sched(char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof;
    return jit_read_sched(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jit_proc_sched = {
        0,                 /* low_ino: the inode -- dynamic */
        8, "jitsched",     /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jit_old_read_sched,   /* function used to read data */
        /* nothing more */
    };

static int jit_old_read_queue(char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof;
    return jit_read_queue(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jit_proc_queue = {
        0,                 /* low_ino: the inode -- dynamic */
        8, "jitqueue",     /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jit_old_read_queue,    /* function used to read data */
        /* nothing more */
    };

static int jit_old_read_self(char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof;
    return jit_read_self (buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jit_proc_self = {
        0,                 /* low_ino: the inode -- dynamic */
        7, "jitself",      /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jit_old_read_self,    /* function used to read data */
        /* nothing more */
    };
#endif /* USE_PROC_REGISTER */

/*
 * There is also a silly file returning the current time.
 */

int jit_read_current(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    struct timeval tv1, tv2;
    unsigned long flags;

    do_gettimeofday(&tv1);
    save_flags(flags);
    cli();
    tv2 = xtime;
    restore_flags(flags);

    len=0;
    len += sprintf(buf,"gettime: %9i.%06i\nxtime:   %9i.%06i\njiffies: %li\n",
                   (int) tv1.tv_sec, (int) tv1.tv_usec,
                   (int) tv2.tv_sec, (int) tv2.tv_usec, jiffies);
    *eof = 1;
    return len;
}

#ifdef USE_PROC_REGISTER

static int jit_old_read_current(char *buf, char **start, off_t offset,
                int len, int unused)
{
    int eof;
    return jit_read_current (buf, start, offset, len, &eof, NULL);
}
                
struct proc_dir_entry jit_proc_current = {
        0,                 /* low_ino: the inode -- dynamic */
        10, "currentime",  /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jit_old_read_current, /* function used to read data */
        /* nothing more */
    };

#endif


int jit_init(void)
{
#ifdef USE_PROC_REGISTER
    proc_register_dynamic(&proc_root, &jit_proc_busy);
    proc_register_dynamic(&proc_root, &jit_proc_sched);
    proc_register_dynamic(&proc_root, &jit_proc_queue);
    proc_register_dynamic(&proc_root, &jit_proc_self);
    proc_register_dynamic(&proc_root, &jit_proc_current);
#else
    create_proc_read_entry("currentime", 0, NULL, jit_read_current, NULL);
    create_proc_read_entry("jitbusy", 0, NULL, jit_read_busy, NULL);
    create_proc_read_entry("jitsched", 0, NULL, jit_read_sched, NULL);
    create_proc_read_entry("jitqueue", 0, NULL, jit_read_queue, NULL);
    create_proc_read_entry("jitself", 0, NULL, jit_read_self, NULL);
#endif

#ifndef JIT_DEBUG
    EXPORT_NO_SYMBOLS;
#endif
    return 0; /* succeed */
}

void jit_cleanup(void)
{
#ifdef USE_PROC_REGISTER
    proc_unregister(&proc_root, jit_proc_busy.low_ino);
    proc_unregister(&proc_root, jit_proc_sched.low_ino);
    proc_unregister(&proc_root, jit_proc_queue.low_ino);
    proc_unregister(&proc_root, jit_proc_self.low_ino);
    proc_unregister(&proc_root, jit_proc_current.low_ino);
#else
    remove_proc_entry ("currentime", 0);
    remove_proc_entry ("jitbusy", 0);
    remove_proc_entry ("jitsched", 0);
    remove_proc_entry ("jitqueue", 0);
    remove_proc_entry ("jitself", 0);
#endif
}

module_init(jit_init);
module_exit(jit_cleanup);
