/*
 * scull.h -- definitions for the char module
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
 * $Id: scull.h,v 1.11 2001/07/18 22:28:18 rubini Exp $
 */

#ifndef _SCULL_H_
#define _SCULL_H_

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

/* version dependencies have been confined to a separate file */
#include "sysdep.h"

/*
 * Macros to help debugging
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef SCULL_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "scull: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifndef SCULL_MAJOR
#define SCULL_MAJOR 0   /* dynamic major by default */
#endif

#ifndef SCULL_NR_DEVS
#define SCULL_NR_DEVS 4    /* scull0 through scull3 */
#endif

#ifndef SCULL_P_NR_DEVS
#define SCULL_P_NR_DEVS 4  /* scullpipe0 through scullpipe3 */
#endif

/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "Scull_Dev->data" points to an array of pointers, each
 * pointer refers to a memory area of SCULL_QUANTUM bytes.
 *
 * The array (quantum-set) is SCULL_QSET long.
 */
#ifndef SCULL_QUANTUM
#define SCULL_QUANTUM 4000
#endif

#ifndef SCULL_QSET
#define SCULL_QSET    1000
#endif

/*
 * The pipe device is a simple circular buffer. Here its default size
 */
#ifndef SCULL_P_BUFFER
#define SCULL_P_BUFFER 4000
#endif

#ifdef CONFIG_DEVFS_FS /* only if enabled, to avoid errors in 2.0 */
#include <linux/devfs_fs_kernel.h>
#else
  typedef void * devfs_handle_t;  /* avoid #ifdef inside the structure */
#endif

/*
 * This is somehow a hack: avoid ifdefs in the cleanup code by declaring
 * an empty procedure as a placeholder for devfs_unregister. This is
 * only done *unless* <linux/devfs_fs_kernel.h> was included, as that
 * header already implements placeholder for all the devfs functions
 */
/*............................................... degin-tag devfs-ifdef */
#ifndef DEVFS_FL_DEFAULT
extern inline void devfs_unregister(devfs_handle_t de) {}
#endif

extern devfs_handle_t scull_devfs_dir;


typedef struct Scull_Dev {
   void **data;
   struct Scull_Dev *next;   /* next listitem */
   int quantum;              /* the current quantum size */
   int qset;                 /* the current array size */
   unsigned long size;
   devfs_handle_t handle;    /* only used if devfs is there */
   unsigned int access_key;  /* used by sculluid and scullpriv */
   struct semaphore sem;     /* mutual exclusion semaphore     */
} Scull_Dev;

/*
 * Split minors in two parts
 */
#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

/*
 * Different minors behave differently, so let's use multiple fops
 */

extern struct file_operations scull_fops;        /* simplest: global */
extern struct file_operations scull_priv_fops;   /* private region   */
extern struct file_operations scull_pipe_fops;   /* circular buffer  */
extern struct file_operations scull_sngl_fops;   /* single open      */
extern struct file_operations scull_user_fops;   /* single process   */
extern struct file_operations scull_wusr_fops;   /* single user      */

/*
 * The different configurable parameters
 */
extern int scull_major;     /* main.c */
extern int scull_nr_devs;
extern int scull_quantum;
extern int scull_qset;

extern int scull_p_nr_devs;    /* pipe.c */
extern int scull_p_buffer;


/*
 * Prototypes for shared functions
 */

int     scull_p_init(void);
void    scull_p_cleanup(void);
int     scull_access_init(void);
void    scull_access_cleanup(void);

int     scull_trim(Scull_Dev *dev);

ssize_t scull_read (struct file *filp, char *buf, size_t count,
                    loff_t *f_pos);
ssize_t scull_write (struct file *filp, const char *buf, size_t count,
                     loff_t *f_pos);
loff_t  scull_llseek (struct file *filp, loff_t off, int whence);
int     scull_ioctl (struct inode *inode, struct file *filp,
                     unsigned int cmd, unsigned long arg);


/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define SCULL_IOC_MAGIC  'k'

#define SCULL_IOCRESET    _IO(SCULL_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */
#define SCULL_IOCSQUANTUM _IOW(SCULL_IOC_MAGIC,  1, scull_quantum)
#define SCULL_IOCSQSET    _IOW(SCULL_IOC_MAGIC,  2, scull_qset)
#define SCULL_IOCTQUANTUM _IO(SCULL_IOC_MAGIC,   3)
#define SCULL_IOCTQSET    _IO(SCULL_IOC_MAGIC,   4)
#define SCULL_IOCGQUANTUM _IOR(SCULL_IOC_MAGIC,  5, scull_quantum)
#define SCULL_IOCGQSET    _IOR(SCULL_IOC_MAGIC,  6, scull_qset)
#define SCULL_IOCQQUANTUM _IO(SCULL_IOC_MAGIC,   7)
#define SCULL_IOCQQSET    _IO(SCULL_IOC_MAGIC,   8)
#define SCULL_IOCXQUANTUM _IOWR(SCULL_IOC_MAGIC, 9, scull_quantum)
#define SCULL_IOCXQSET    _IOWR(SCULL_IOC_MAGIC,10, scull_qset)
#define SCULL_IOCHQUANTUM _IO(SCULL_IOC_MAGIC,  11)
#define SCULL_IOCHQSET    _IO(SCULL_IOC_MAGIC,  12)

/*
 * The other entities only have "Tell" and "Query", because they're
 * not printed in the book, and there's no need to have all six.
 * (The previous stuff was only there to show different ways to do it.
 */
#define SCULL_P_IOCTSIZE _IO(SCULL_IOC_MAGIC,   13)
#define SCULL_P_IOCQSIZE _IO(SCULL_IOC_MAGIC,   14)
/* ... more to come */
#define SCULL_IOCHARDRESET _IO(SCULL_IOC_MAGIC, 15) /* debugging tool */

#define SCULL_IOC_MAXNR 15

#endif /* _SCULL_H_ */
