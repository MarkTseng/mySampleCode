/*
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
 */
#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/module.h>
#include <linux/errno.h>

#define rdtsc(dest) \
     __asm__ __volatile__("nop; mfc0 %0,$9; nop" : "=r" (dest))

int i;

int init_module()
{
    unsigned int count1,count2,fix;

    rdtsc(count1);
    rdtsc(count2);
    fix = count2-count1;

    rdtsc(count1);
    printk("rdtsc()  took %5u cycles\n",fix);
    rdtsc(count2);

    printk("printk() took %5u cycles\n",count2-count1-fix);
    return 0;
}

void cleanup_module(void)
{}
