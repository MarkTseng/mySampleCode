/*
 * short.c -- Simple Hardware Operations and Raw Tests
 * short.c -- also a brief example of interrupt handling ("short int")
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
 * $Id: short.c,v 1.22 2001/07/18 22:28:18 rubini Exp $
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
#include <linux/errno.h>  /* error codes */
#include <linux/delay.h>  /* udelay */
#include <linux/malloc.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/tqueue.h>

#include <asm/io.h>

#include "sysdep.h"

#define SHORT_NR_PORTS 8 /* use 8 ports by default */

/*
 * all of the parameters have no "short_" prefix, to save typing when
 * specifying them at load time
 */
static int major = 0; /* dynamic by default */
MODULE_PARM(major, "i");

static int use_mem = 0; /* default is I/O-mapped */
MODULE_PARM(use_mem, "i");

/* default is the first printer port on PC's. "short_base" is there too
   because it's what we want to use in the code */
static unsigned long base = 0x378;
unsigned long short_base = 0;
MODULE_PARM(base, "l");

/* Since short_base is vremapped in case use_mem==1, remember the phys addr. */
unsigned long short_phys;

/* The interrupt line is undefined by default. "short_irq" is as above */
static int irq = -1;
volatile int short_irq = -1;
MODULE_PARM(irq, "i");

static int probe = 0; /* select at load time how to probe irq line */
MODULE_PARM(probe, "i");

static int bh = 0; /* select at load time whether a bottom-half is used */
MODULE_PARM(bh, "i");

static int tasklet = 0; /* select whether a tasklet is used */
MODULE_PARM(tasklet, "i");

static int share = 0; /* select at load time whether install a shared irq */
MODULE_PARM(share, "i");

MODULE_AUTHOR ("Alessandro Rubini");




unsigned long short_buffer = 0;
unsigned long volatile short_head;
volatile unsigned long short_tail;
DECLARE_WAIT_QUEUE_HEAD(short_queue);

/* Set up our tasklet if we're doing that. */
#ifdef HAVE_TASKLETS
void short_do_tasklet (unsigned long);
DECLARE_TASKLET (short_tasklet, short_do_tasklet, 0);
#endif

/*
 * Atomicly increment an index into short_buffer
 */
static inline void short_incr_bp(volatile unsigned long *index, int delta)
{
    unsigned long new = *index + delta;
    barrier ();  /* Don't optimize these two together */
    *index = (new >= (short_buffer + PAGE_SIZE)) ? short_buffer : new;
}


/*
 * The devices with low minor numbers write/read burst of data to/from
 * specific I/O ports (by default the parallel ones).
 * 
 * The device with 128 as minor number returns ascii strings telling
 * when interrupts have been received. Writing to the device toggles
 * 00/FF on the parallel data lines. If there is a loopback wire, this
 * generates interrupts.  
 */

int short_open (struct inode *inode, struct file *filp)
{
    extern struct file_operations short_i_fops;
    MOD_INC_USE_COUNT;
    if (MINOR(inode->i_rdev) & 0x80) {
        filp->f_op = &short_i_fops; /* the interrupt-driven node */
    }
    return 0;
}


#ifdef LINUX_20
void short_release (struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
}
#else
int short_release (struct inode *inode, struct file *filp)
{
    MOD_DEC_USE_COUNT;
    return 0;
}
#endif


/* first, the port-oriented device */

enum short_modes {SHORT_DEFAULT=0, SHORT_PAUSE, SHORT_STRING, SHORT_MEMORY};

ssize_t do_short_read (struct inode *inode, struct file *filp, char *buf,
                size_t count, loff_t *f_pos)
{
    int retval = count;
    unsigned long address = short_base + (MINOR(inode->i_rdev)&0x0f);
    int mode = (MINOR(inode->i_rdev)&0x70) >> 4;
    unsigned char *kbuf=kmalloc(count, GFP_KERNEL), *ptr;
    
    if (!kbuf) return -ENOMEM;
    ptr=kbuf;

    if (use_mem)
	mode = SHORT_MEMORY;
	
    switch(mode) {
      case SHORT_STRING:
        insb(address, ptr, count);
	rmb();
        break;

      case SHORT_DEFAULT:
        while (count--) {
            *(ptr++) = inb(address);
	    rmb();
	}
        break;

      case SHORT_MEMORY:
        while (count--) {
            *(ptr++) = readb(address);
	    rmb();
	}
        break;
      case SHORT_PAUSE:
        while (count--) {
            *(ptr++) = inb_p(address);
	    rmb();
	}
        break;

      default: /* no more modes defined by now */
        retval = -EINVAL;
        break;
    }
    if ( (retval > 0) && copy_to_user(buf, kbuf, retval))
	retval = -EFAULT;
    kfree(kbuf);
    return retval;
}


/*
 * Version-specific methods for the fops structure.
 */
#ifdef LINUX_20
int short_read(struct inode *inode, struct file *filp, char *buf, int count)
{
    return do_short_read(inode, filp, buf, count, &filp->f_pos);
}

#else
ssize_t short_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    return do_short_read(filp->f_dentry->d_inode, filp, buf, count, f_pos);
}
#endif



ssize_t do_short_write (struct inode *inode, struct file *filp, const char *buf,
                size_t count, loff_t *f_pos)
{
    int retval = count;
    unsigned long address = short_base + (MINOR(inode->i_rdev)&0x0f);
    int mode = (MINOR(inode->i_rdev)&0x70) >> 4;
    unsigned char *kbuf=kmalloc(count, GFP_KERNEL), *ptr;

    if (!kbuf) return -ENOMEM;
    if (copy_from_user(kbuf, buf, count))
	return -EFAULT;
    ptr=kbuf;

    if (use_mem)
	mode = SHORT_MEMORY;

    switch(mode) {
      case SHORT_PAUSE:
        while (count--) {
            outb_p(*(ptr++), address);
	    wmb();
	}
        break;

      case SHORT_STRING:
        outsb(address, ptr, count);
	wmb();
        break;

      case SHORT_DEFAULT:
        while (count--) {
            outb(*(ptr++), address);
	    wmb();
	}
        break;

      case SHORT_MEMORY:
        while (count--) {
            writeb(*(ptr++), address);
	    wmb();
	}
        break;

      default: /* no more modes defined by now */
        retval = -EINVAL;
        break;
    }
    kfree(kbuf);
    return retval;
}


#ifdef LINUX_20
int short_write(struct inode *inode, struct file *filp, const char *buf,
                int count)
{
    return do_short_write(inode, filp, buf, count, &filp->f_pos);
}

#else
ssize_t short_write(struct file *filp, const char *buf, size_t count,
                loff_t *f_pos)
{
    return do_short_write(filp->f_dentry->d_inode, filp, buf, count, f_pos);
}
#endif



#ifdef __USE_OLD_SELECT__
int short_poll (struct inode *inode, struct file *filp,
                  int mode, select_table *table)
{
    return mode==SEL_EX ? 0 : 1; /* readable, writable, not-exceptionable */
}
#define poll select
#else /* Use poll */

unsigned int short_poll(struct file *filp, poll_table *wait)
{
    return POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM;
}

#endif /* __USE_OLD_SELECT__ */





struct file_operations short_fops = {
    read: short_read,
    write: short_write,
    poll: short_poll,
    open: short_open,
    release: short_release,
};

/* then,  the interrupt-related device */

ssize_t short_i_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    int count0;

    while (short_head == short_tail) {
        interruptible_sleep_on(&short_queue);
        if (signal_pending (current))  /* a signal arrived */
          return -ERESTARTSYS; /* tell the fs layer to handle it */
        /* else, loop */
    }
    /* count0 is the number of readable data bytes */
    count0 = short_head - short_tail;
    if (count0 < 0) /* wrapped */
        count0 = short_buffer + PAGE_SIZE - short_tail;
    if (count0 < count) count = count0;

    if (copy_to_user(buf, (char *)short_tail, count))
	return -EFAULT;
    short_incr_bp (&short_tail, count);
    return count;
}

ssize_t short_i_write (struct file *filp, const char *buf, size_t count,
                loff_t *f_pos)
{
    int written = 0, odd = *f_pos & 1;
    unsigned long address = short_base; /* output to the parallel data latch */

    if (use_mem) {
	while (written < count)
	    writeb(0xff * ((++written + odd) & 1), address);
    } else {
	while (written < count)
	    outb(0xff * ((++written + odd) & 1), address);
    }

    *f_pos += count;
    return written;
}


/*
 * 2.0 wrappers.
 */
#ifdef LINUX_20

int short_i_read_20 (struct inode *inode, struct file *filp, char *buf,
                int count)
{
    return short_i_read (filp, buf, count, &filp->f_pos);
}

int short_i_write_20 (struct inode *inode, struct file *filp, const char *buf,
                int count)
{
    return short_i_write (filp, buf, count, &filp->f_pos);
}

#define short_i_read  short_i_read_20
#define short_i_write short_i_write_20

#endif /* LINUX_20 */


struct file_operations short_i_fops = {
    read: short_i_read,
    write: short_i_write,
    open: short_open,
    release: short_release,
};

void short_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    struct timeval tv;
    int written;

    do_gettimeofday(&tv);

    /* Write a 16 byte record. Assume PAGE_SIZE is a multiple of 16 */
    written = sprintf((char *)short_head,"%08u.%06u\n",
                          (int)(tv.tv_sec % 100000000), (int)(tv.tv_usec));
    short_incr_bp(&short_head, written);
    wake_up_interruptible(&short_queue); /* awake any reading process */
}

/*
 * The following two functions are equivalent to the previous one,
 * but split in top and bottom half. First, a few needed variables
 */

#define NR_TIMEVAL 512 /* length of the array of time values */

struct timeval tv_data[NR_TIMEVAL]; /* too lazy to allocate it */
volatile struct timeval *tv_head=tv_data;
volatile struct timeval *tv_tail=tv_data;

static struct tq_struct short_task; /* 0 by now, filled by short_init code */

int short_bh_count = 0;

/*
 * Increment a circular buffer pointer in a way that nobody sees
 * an intermediate value.
 */
static inline void short_incr_tv(volatile struct timeval **tvp)
{
    if (*tvp == (tv_data + NR_TIMEVAL - 1))
        *tvp = tv_data;  /* Wrap */
    else
        (*tvp)++;
}



void short_do_tasklet (unsigned long unused)
{
    int savecount = short_bh_count, written;
    short_bh_count = 0; /* we have already been removed from the queue */
    /*
     * The bottom half reads the tv array, filled by the top half,
     * and prints it to the circular text buffer, which is then consumed
     * by reading processes
     */

    /* First write the number of interrupts that occurred before this bh */

    written = sprintf((char *)short_head,"bh after %6i\n",savecount);
    short_incr_bp(&short_head, written);

    /*
     * Then, write the time values. Write exactly 16 bytes at a time,
     * so it aligns with PAGE_SIZE
     */

    do {
        written = sprintf((char *)short_head,"%08u.%06u\n",
			(int)(tv_tail->tv_sec % 100000000),
			(int)(tv_tail->tv_usec));
	short_incr_bp(&short_head, written);
	short_incr_tv(&tv_tail);
    } while (tv_tail != tv_head);

    wake_up_interruptible(&short_queue); /* awake any reading process */
}


void short_bh_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    /* cast to stop 'volatile' warning */
    do_gettimeofday((struct timeval *) tv_head);
    short_incr_tv(&tv_head);

    /* Queue the bh. Don't care for multiple enqueueing */
    queue_task(&short_task, &tq_immediate);
    mark_bh(IMMEDIATE_BH);

    short_bh_count++; /* record that an interrupt arrived */
}


#ifdef HAVE_TASKLETS
/*
 * Tasklet top half
 */

void short_tl_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    do_gettimeofday((struct timeval *) tv_head); /* cast to stop 'volatile' warning */
    short_incr_tv(&tv_head);
    tasklet_schedule(&short_tasklet);
    short_bh_count++; /* record that an interrupt arrived */
}

#endif /* HAVE_TASKLETS */



void short_sh_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    int value, written;
    struct timeval tv;

    /* If it wasn't short, return immediately */
    value = inb(short_base);
    if (!(value & 0x80)) return;
        
    /* clear the interrupting bit */
    outb(value & 0x7F, short_base);

    /* the rest is unchanged */

    do_gettimeofday(&tv);
    written = sprintf((char *)short_head,"%08u.%06u\n",
                          (int)(tv.tv_sec % 100000000), (int)(tv.tv_usec));
    short_incr_bp(&short_head, written);
    wake_up_interruptible(&short_queue); /* awake any reading process */
}

void short_kernelprobe(void)
{
    int count = 0;
    do {
        unsigned long mask;

        mask = probe_irq_on();
        outb_p(0x10,short_base+2); /* enable reporting */
        outb_p(0x00,short_base);   /* clear the bit */
        outb_p(0xFF,short_base);   /* set the bit: interrupt! */
        outb_p(0x00,short_base+2); /* disable reporting */
        udelay(5);  /* give it some time */
        short_irq = probe_irq_off(mask);

        if (short_irq == 0) { /* none of them? */
            printk(KERN_INFO "short: no irq reported by probe\n");
            short_irq = -1;
        }
        /*
         * if more than one line has been activated, the result is
         * negative. We should service the interrupt (no need for lpt port)
         * and loop over again. Loop at most five times, then give up
         */
    } while (short_irq < 0 && count++ < 5);
    if (short_irq < 0)
        printk("short: probe failed %i times, giving up\n", count);
}

void short_probing(int irq, void *dev_id, struct pt_regs *regs)
{
    if (short_irq == 0) short_irq = irq;    /* found */
    if (short_irq != irq) short_irq = -irq; /* ambiguous */
}

void short_selfprobe(void)
{
    int trials[] = {3, 5, 7, 9, 0};
    int tried[]  = {0, 0, 0, 0, 0};
    int i, count = 0;

     /*
      * install the probing handler for all possible lines. Remember
      * the result (0 for success, or -EBUSY) in order to only free
      * what has been acquired
      */
    for (i=0; trials[i]; i++)
        tried[i] = request_irq(trials[i], short_probing,
                               SA_INTERRUPT, "short probe", NULL);

    do {
        short_irq = 0; /* none got, yet */
        outb_p(0x10,short_base+2); /* enable */
        outb_p(0x00,short_base);
        outb_p(0xFF,short_base); /* toggle the bit */
        outb_p(0x00,short_base+2); /* disable */
        udelay(5);  /* give it some time */

        /* the value has been set by the handler */
        if (short_irq == 0) { /* none of them? */
            printk(KERN_INFO "short: no irq reported by probe\n");
        }
        /*
         * If more than one line has been activated, the result is
         * negative. We should service the interrupt (but the lpt port
         * doesn't need it) and loop over again. Do it at most 5 times
         */
    } while (short_irq <=0 && count++ < 5);

    /* end of loop, uninstall the handler */
    for (i=0; trials[i]; i++)
        if (tried[i] == 0)
            free_irq(trials[i], NULL);

    if (short_irq < 0)
        printk("short: probe failed %i times, giving up\n", count);
}

/* Two wrappers, to use non-page-aligned ioremap() on 2.0 */

/* Remap a not (necessarily) aligned port region */
void *short_remap(unsigned long phys_addr)
{
    /* The code comes mainly from arch/any/mm/ioremap.c */
    unsigned long offset, last_addr, size;

    last_addr = phys_addr + SHORT_NR_PORTS - 1;
    offset = phys_addr & ~PAGE_MASK;
    
    /* Adjust the begin and end to remap a full page */
    phys_addr &= PAGE_MASK;
    size = PAGE_ALIGN(last_addr) - phys_addr;
    return ioremap(phys_addr, size) + offset;
}

/* Unmap a region obtained with short_remap */
void short_unmap(void *virt_add)
{
    iounmap((void *)((unsigned long)virt_add & PAGE_MASK));
}



/* Finally, init and cleanup */

int short_init(void)
{
    int result;

    /*
     * first, sort out the base/short_base ambiguity: we'd better
     * use short_base in the code, for clarity, but allow setting
     * just "base" at load time. Same for "irq".
     */
    short_base = base;
    short_irq = irq;

    /* Set up owner pointers.*/
    SET_MODULE_OWNER(&short_fops);
    SET_MODULE_OWNER(&short_i_fops);

    /* Get our needed resources. */
    if (!use_mem) {
	result = check_region(short_base, SHORT_NR_PORTS);
	if (result) {
	    printk(KERN_INFO "short: can't get I/O port address 0x%lx\n",
		   short_base);
	    return result;
	}
	request_region(short_base, SHORT_NR_PORTS, "short");
    } else {
	result = check_mem_region(short_base, SHORT_NR_PORTS);
	if (result) {
	    printk(KERN_INFO "short: can't get I/O mem address 0x%lx\n",
		   short_base);
	    return result;
	}
	request_mem_region(short_base, SHORT_NR_PORTS, "short");

	/* also, ioremap it */
	short_phys = short_base;
	short_base = (unsigned long)short_remap(short_base);
	/* Hmm... we should check the return value */
    }
    result = register_chrdev(major, "short", &short_fops);
    if (result < 0) {
        printk(KERN_INFO "short: can't get major number\n");
        release_region(short_base,SHORT_NR_PORTS);
        return result;
    }
    if (major == 0) major = result; /* dynamic */

    short_buffer = __get_free_pages(GFP_KERNEL,0); /* never fails */
    short_head = short_tail = short_buffer;

    /*
     * Fill the short_task structure, used for the bottom half handler.
     * The cast is there to prevent warnings about the type of the
     * (unused) argument.
     */
    short_task.routine = (void (*)(void *)) short_do_tasklet;
    short_task.data = NULL; /* unused */

    /*
     * Now we deal with the interrupt: either kernel-based
     * autodetection, DIY detection or default number
     */

    if (short_irq < 0 && probe == 1)
        short_kernelprobe();

    if (short_irq < 0 && probe == 2)
        short_selfprobe();

    if (short_irq < 0) /* not yet specified: force the default on */
        switch(short_base) {
          case 0x378: short_irq = 7; break;
          case 0x278: short_irq = 2; break;
          case 0x3bc: short_irq = 5; break;
        }

    /*
     * If shared has been specified, installed the shared handler
     * instead of the normal one. Do it first, before a -EBUSY will
     * force short_irq to -1.
     */
    if (short_irq >= 0 && share > 0) {
        result = request_irq(short_irq, short_sh_interrupt,
                             SA_SHIRQ | SA_INTERRUPT,"short",
                             short_sh_interrupt);
        if (result) {
            printk(KERN_INFO "short: can't get assigned irq %i\n", short_irq);
            short_irq = -1;
        }
        else { /* actually enable it -- assume this *is* a parallel port */
            outb(0x10,short_base+2);
        }
        return 0; /* the rest of the function only installs handlers */
    }

    if (short_irq >= 0) {
        result = request_irq(short_irq, short_interrupt,
                             SA_INTERRUPT, "short", NULL);
        if (result) {
            printk(KERN_INFO "short: can't get assigned irq %i\n",
                   short_irq);
            short_irq = -1;
        }
        else { /* actually enable it -- assume this *is* a parallel port */
            outb(0x10,short_base+2);
        }
    }

    /*
     * Ok, now change the interrupt handler if using top/bottom halves
     * has been requested
     */
    if (short_irq >= 0 && (bh + tasklet) > 0) {
        free_irq(short_irq,NULL);
        result = request_irq(short_irq,
#ifdef HAVE_TASKLETS
                        tasklet ? short_tl_interrupt :
#endif
                        short_bh_interrupt,
                        SA_INTERRUPT,"short-bh", NULL);
        if (result) {
            printk(KERN_INFO "short-bh: can't get assigned irq %i\n",
                   short_irq);
            short_irq = -1;
        }
    }

    return 0;
}

void short_cleanup(void)
{
    if (short_irq >= 0) {
        outb(0x0, short_base + 2);   /* disable the interrupt */
        if (!share) free_irq(short_irq, NULL);
        else free_irq(short_irq, short_sh_interrupt);
    }
        
    unregister_chrdev(major, "short");
    if (use_mem) {
	short_unmap((void *)short_base);
	release_mem_region(short_base, SHORT_NR_PORTS);
    } else {
	release_region(short_base,SHORT_NR_PORTS);
    }
    if (short_buffer) free_page(short_buffer);
}

module_init(short_init);
module_exit(short_cleanup);
