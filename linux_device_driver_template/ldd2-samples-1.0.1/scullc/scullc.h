/*
 * scullc.h -- definitions for the scullc char module
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
 */

#include <linux/ioctl.h>

/* version dependencies have been confined to a separate file */

#include "sysdep.h"


/*
 * Macros to help debugging
 */

#undef PDEBUG             /* undef it, just in case */
#ifdef SCULLC_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "scullc: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#define SCULLC_MAJOR 0   /* dynamic major by default */

#define SCULLC_DEVS 4    /* scullc0 through scullc3 */

/*
 * The bare device is a variable-length region of memory.
 * Use a linked list of indirect blocks.
 *
 * "ScullC_Dev->data" points to an array of pointers, each
 * pointer refers to a memory page.
 *
 * The array (quantum-set) is SCULLC_QSET long.
 */
#define SCULLC_QUANTUM  4000 /* use a quantum size like scull */
#define SCULLC_QSET     500

typedef struct ScullC_Dev {
   void **data;
   struct ScullC_Dev *next;  /* next listitem */
   int vmas;                 /* active mappings */
   int quantum;              /* the current allocation size */
   int qset;                 /* the current array size */
   size_t size;              /* 32-bit will suffice */
   struct semaphore sem;     /* Mutual exclusion */
} ScullC_Dev;

extern ScullC_Dev *scullc_devices;

extern struct file_operations scullc_fops;

/*
 * The different configurable parameters
 */
extern int scullc_major;     /* main.c */
extern int scullc_devs;
extern int scullc_order;
extern int scullc_qset;

/*
 * Prototypes for shared functions
 */
int scullc_trim(ScullC_Dev *dev);
ScullC_Dev *scullc_follow(ScullC_Dev *dev, int n);


#ifdef SCULLC_DEBUG
#  define SCULLC_USE_PROC
#endif

#ifndef min
#  define min(a,b) ((a)<(b) ? (a) : (b))
#endif

/*
 * Ioctl definitions
 */

/* Use 'K' as magic number */
#define SCULLC_IOC_MAGIC  'K'

#define SCULLC_IOCRESET    _IO(SCULLC_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly
 * G means "Get" (to a pointed var)
 * Q means "Query", response is on the return value
 * X means "eXchange": G and S atomically
 * H means "sHift": T and Q atomically
 */
#define SCULLC_IOCSQUANTUM _IOW(SCULLC_IOC_MAGIC,  1, scullc_quantum)
#define SCULLC_IOCTQUANTUM _IO(SCULLC_IOC_MAGIC,   2)
#define SCULLC_IOCGQUANTUM _IOR(SCULLC_IOC_MAGIC,  3, scullc_quantum)
#define SCULLC_IOCQQUANTUM _IO(SCULLC_IOC_MAGIC,   4)
#define SCULLC_IOCXQUANTUM _IOWR(SCULLC_IOC_MAGIC, 5, scullc_quantum)
#define SCULLC_IOCHQUANTUM _IO(SCULLC_IOC_MAGIC,   6)
#define SCULLC_IOCSQSET    _IOW(SCULLC_IOC_MAGIC,  7, scullc_qset)
#define SCULLC_IOCTQSET    _IO(SCULLC_IOC_MAGIC,   8)
#define SCULLC_IOCGQSET    _IOR(SCULLC_IOC_MAGIC,  9, scullc_qset)
#define SCULLC_IOCQQSET    _IO(SCULLC_IOC_MAGIC,  10)
#define SCULLC_IOCXQSET    _IOWR(SCULLC_IOC_MAGIC,11, scullc_qset)
#define SCULLC_IOCHQSET    _IO(SCULLC_IOC_MAGIC,  12)

#define SCULLC_IOCHARDRESET _IO(SCULLC_IOC_MAGIC, 13) /* debugging tool */

#define SCULLC_IOC_MAXNR 13



