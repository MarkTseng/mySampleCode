/*
 * jiq.c -- the just-in-queue module
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
 * $Id: jiq.c,v 1.15 2001/07/18 22:28:16 rubini Exp $
 */
 
/* BUGS: the module is not reentrant: only one file can be read at a time
 *       the module usage count is not used: you could remove the module
 *           while reading it, thus asking for troubles.
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
#include <linux/kernel.h>
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>  /* error codes */
#include <linux/tqueue.h>
#include <linux/interrupt.h> /* intr_count */

#include "sysdep.h"

/*
 * This module is a silly one: it only embeds short code fragments
 * that show how enqueued tasks `feel' theit environment
 */

#define LIMIT (PAGE_SIZE-128) /* don't print any more after this size */

/*
 * Print information about the current environment. This is called from
 * within the task queues. If the limit is reched, awake the reading
 * process.
 */
DECLARE_WAIT_QUEUE_HEAD (jiq_wait);

struct tq_struct jiq_task; /* global: initialized to zero */




/*
 * Keep track of info we need between task queue runs.
 */
struct clientdata {
    int len;
    char *buf;
    unsigned long jiffies;
    task_queue *queue;
} jiq_data;

#define SCHEDULER_QUEUE ((task_queue *) 1)



#ifdef HAVE_TASKLETS
void jiq_print_tasklet (unsigned long);
DECLARE_TASKLET (jiq_tasklet, jiq_print_tasklet, (unsigned long) &jiq_data);
#endif /* HAVE_TASKLETS */


/*
 * Do the printing; return non-zero if the task should be rescheduled.
 */
int jiq_print(void *ptr)
{
  struct clientdata *data = (struct clientdata *)ptr;
  int len = data->len;
  char *buf = data->buf;
  unsigned long j = jiffies;

  if (len > LIMIT) { 
      wake_up_interruptible(&jiq_wait);
      return 0;
  }

  if (len == 0)
      len = sprintf(buf,"    time  delta interrupt  pid cpu command\n");
  else
      len =0;

  /* intr_count is only exported since 1.3.5, but 1.99.4 is needed anyways */
  len += sprintf(buf+len,"%9li %3li        %i   %5i %3i %s\n",
                 j, j - data->jiffies,
                 in_interrupt (), current->pid, smp_processor_id (), current->comm);

  data->len += len;
  data->buf += len;
  data->jiffies = j;
  return 1;
}


/*
 * Call jiq_print from a task queue
 */
void jiq_print_tq(void *ptr)
{
    if (jiq_print (ptr)) {
        struct clientdata *data = (struct clientdata *)ptr;
        if (data->queue == SCHEDULER_QUEUE)
	    schedule_task(&jiq_task);
        else if (data->queue)
            queue_task(&jiq_task, data->queue);
        if (data->queue == &tq_immediate)
            mark_bh(IMMEDIATE_BH); /* this one needs to be marked */
    }
}



/*
 * Use the scheduler queue  -- /proc/jiqsched
 */

int jiq_read_sched(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

    jiq_data.len = 0;               /* nothing printed, yet */
    jiq_data.buf = buf;             /* print in this place */
    jiq_data.jiffies = jiffies;     /* initial time */

    /* jiq_print will queue_task() again in jiq_data.queue */
    jiq_data.queue = SCHEDULER_QUEUE;

    schedule_task(&jiq_task);             /* ready to run */
    interruptible_sleep_on(&jiq_wait);    /* sleep till completion */

    *eof = 1;
    return jiq_data.len;
}



#ifdef USE_PROC_REGISTER

static int jiq_old_read_sched(char *buf, char **start, off_t offset, int len,
                int unused)
{
    int eof;
    return jiq_read_sched(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jiq_proc_sched = {
        0,                 /* low_ino: the inode -- dynamic */
        8, "jiqsched",     /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jiq_old_read_sched,   /* function used to read data */
        /* nothing more */
    };

#endif


int jiq_read_timer(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

    jiq_data.len = 0;            /* nothing printed, yet */
    jiq_data.buf = buf;          /* print in this place */
    jiq_data.jiffies = jiffies;  /* initial time */
    jiq_data.queue = &tq_timer;  /* re-register yourself here */

    queue_task(&jiq_task, &tq_timer);     /* ready to run */
    interruptible_sleep_on(&jiq_wait);    /* sleep till completion */

    *eof = 1;
    return jiq_data.len;
}

#ifdef USE_PROC_REGISTER

static int jiq_old_read_timer(char *buf, char **start, off_t offset, int len,
                int unused)
{
    int eof;
    return jiq_read_timer(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jiq_proc_timer = {
        0,                 /* low_ino: the inode -- dynamic */
        8, "jiqtimer",     /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jiq_old_read_timer,   /* function used to read data */
        /* nothing more */
    };
#endif



int jiq_read_immed(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    jiq_data.len = 0;                /* nothing printed, yet */
    jiq_data.buf = buf;              /* print in this place */
    jiq_data.jiffies = jiffies;      /* initial time */
    jiq_data.queue = &tq_immediate;  /* re-register yourself here */

    queue_task(&jiq_task, &tq_immediate); /* ready to run */
    mark_bh(IMMEDIATE_BH);
    interruptible_sleep_on(&jiq_wait);    /* sleep till completion */

    *eof = 1;
    return jiq_data.len;
}

#ifdef USE_PROC_REGISTER

static int jiq_old_read_immed(char *buf, char **start, off_t offset, int len,
                int unused)
{
    int eof;
    return jiq_read_immed(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jiq_proc_immed = {
        0,                 /* low_ino: the inode -- dynamic */
        8, "jiqimmed",     /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jiq_old_read_immed,   /* function used to read data */
        /* nothing more */
    };
#endif



#ifdef HAVE_TASKLETS
/*
 * Call jiq_print from a tasklet
 */
void jiq_print_tasklet(unsigned long ptr)
{
    if (jiq_print ((void *) ptr))
        tasklet_schedule (&jiq_tasklet);
}



int jiq_read_tasklet(char *buf, char **start, off_t offset, int len,
                int *eof, void *data)
{
    jiq_data.len = 0;                /* nothing printed, yet */
    jiq_data.buf = buf;              /* print in this place */
    jiq_data.jiffies = jiffies;      /* initial time */

    tasklet_schedule(&jiq_tasklet);
    interruptible_sleep_on(&jiq_wait);    /* sleep till completion */

    *eof = 1;
    return jiq_data.len;
}

/* No PROC_REGISTER junk since tasklets postdate all that */

#endif /* HAVE_TASKLETS */



/*
 * This one, instead, tests out the timers.
 */

struct timer_list jiq_timer;

void jiq_timedout(unsigned long ptr)
{
    jiq_print((void *)ptr);            /* print a line */
    wake_up_interruptible(&jiq_wait);  /* awake the process */
}


int jiq_read_run_timer(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

    jiq_data.len = 0;           /* prepare the argument for jiq_print() */
    jiq_data.buf = buf;
    jiq_data.jiffies = jiffies;
    jiq_data.queue = NULL;      /* don't requeue */

    init_timer(&jiq_timer);              /* init the timer structure */
    jiq_timer.function = jiq_timedout;
    jiq_timer.data = (unsigned long)&jiq_data;
    jiq_timer.expires = jiffies + HZ; /* one second */

    jiq_print(&jiq_data);   /* print and go to sleep */
    add_timer(&jiq_timer);
    interruptible_sleep_on(&jiq_wait);
    del_timer_sync(&jiq_timer);  /* in case a signal woke us up */
    
    *eof = 1;
    return jiq_data.len;
}

#ifdef USE_PROC_REGISTER

static int jiq_old_read_run_timer(char *buf, char **start, off_t offset, int len,
                int unused)
{
    int eof;
    return jiq_read_run_timer(buf, start, offset, len, &eof, NULL);
}

struct proc_dir_entry jiq_proc_run_timer = {
        0,                 /* low_ino: the inode -- dynamic */
        7, "jitimer",      /* len of name and name */
        S_IFREG | S_IRUGO, /* mode */
        1, 0, 0,           /* nlinks, owner, group */
        0, NULL,           /* size - unused; operations -- use default */
        &jiq_old_read_run_timer, /* function used to read data */
        /* nothing more */
    };
#endif


/*
 * the init/clean material
 */

int jiq_init(void)
{
    /* these lines are in jiq_init() */
    jiq_task.routine = jiq_print_tq;
    jiq_task.data = (void *)&jiq_data;

#ifdef USE_PROC_REGISTER
    proc_register_dynamic(&proc_root, &jiq_proc_sched);
    proc_register_dynamic(&proc_root, &jiq_proc_timer);
    proc_register_dynamic(&proc_root, &jiq_proc_immed);
    proc_register_dynamic(&proc_root, &jiq_proc_run_timer);
#else
    create_proc_read_entry("jiqsched", 0, NULL, jiq_read_sched, NULL);
    create_proc_read_entry("jiqtimer", 0, NULL, jiq_read_timer, NULL);
    create_proc_read_entry("jiqimmed", 0, NULL, jiq_read_immed, NULL);
    create_proc_read_entry("jitimer", 0, NULL, jiq_read_run_timer, NULL);
#ifdef HAVE_TASKLETS
    create_proc_read_entry("jiqtasklet", 0, NULL, jiq_read_tasklet, NULL);
#endif
#endif



#ifndef JIT_DEBUG
    EXPORT_NO_SYMBOLS;
#endif
    return 0; /* succeed */
}

void jiq_cleanup(void)
{
#ifdef USE_PROC_REGISTER
    proc_unregister(&proc_root, jiq_proc_sched.low_ino);
    proc_unregister(&proc_root, jiq_proc_timer.low_ino);
    proc_unregister(&proc_root, jiq_proc_immed.low_ino);
    proc_unregister(&proc_root, jiq_proc_run_timer.low_ino);
#else
    remove_proc_entry("jiqsched", 0);
    remove_proc_entry("jiqtimer", 0);
    remove_proc_entry("jiqimmed", 0);
    remove_proc_entry("jitimer", 0);
#ifdef HAVE_TASKLETS
    remove_proc_entry("jiqtasklet", 0);
#endif
#endif
}


module_init(jiq_init);
module_exit(jiq_cleanup);
