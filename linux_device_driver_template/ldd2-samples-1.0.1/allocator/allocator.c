/*
 * allocator.c -- allocate after high_memory, if available
 *
 * Copyright (C) 1998,2000   rubini@linux.it (Alessandro Rubini)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifdef MODULE
#  define EXPORT_SYMTAB
#endif

#include <linux/version.h>
#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/errno.h>
#include <linux/types.h>

#include <asm/page.h>

#include "sysdep.h"
#include "allocator.h"

MODULE_AUTHOR("Alessandro Rubini");

#ifdef MODULE
/* export symbols, using a different approach for 2.0 */
#  ifdef __USE_OLD_SYMTAB__
static struct symbol_table allocator_syms = {
#include <linux/symtab_begin.h>
        X(allocator_allocate_dma),
        X(allocator_free_dma),
#include <linux/symtab_end.h>
};

#    define allocator_register_symtab()  register_symtab(&allocator_syms)
#  else /* new way (2.2, 2.4) */
EXPORT_SYMBOL(allocator_allocate_dma);
EXPORT_SYMBOL(allocator_free_dma);
#    define  allocator_register_symtab()  /* nothing */
#  endif /* symtab way */
#else
#define allocator_register_symtab() /* nothing */
#endif /* module */



#define ALL_MSG "allocator: "

#undef PDEBUG             /* undef it, just in case */
#ifdef ALL_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG ALL_MSG fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

/* This is meant to be a parameter: 0 = probe, pos. = megs, neg. = disable */
int allocator_himem = 0;

static unsigned long allocator_buffer      = 0;  /* physical address */
static unsigned long allocator_buffer_size = 0;  /* bytes */

/*
 * The allocator keeps a list of DMA areas, so multiple devices
 * can coexist. The list is kept sorted by address
 */

struct allocator_struct {
    unsigned long address;
    unsigned long size;
    struct allocator_struct *next;
};

struct allocator_struct *allocator_list = NULL;


#if 0
static int dump_list(void)
{
    struct allocator_struct *ptr;

    PDEBUG("Current list:\n");
    for (ptr = allocator_list; ptr; ptr = ptr->next) {
        PDEBUG("0x%08lx (size %likB)\n",ptr->address,ptr->size>>10);
    }
    return 0;
}
#endif

/* ========================================================================
 * This function is the actual allocator.
 *
 * If space is available in high memory (as detected at load time), that
 * one is returned. The return value is a physical address (i.e., it can
 * be used straight ahead for DMA, but needs remapping for program use).
 */

unsigned long allocator_allocate_dma (unsigned long bytes, int prio)
{
    struct allocator_struct *ptr = allocator_list, *newptr;

    /* check if high memory is available */
    if (!allocator_buffer)
        return 0;

     /* Round it to a multiple of the pagesize */
     bytes = PAGE_ALIGN(bytes);
    PDEBUG("request for %li bytes\n", bytes);

    while (ptr && ptr->next) {
        if (ptr->next->address - (ptr->address + ptr->size) >= bytes)
            break; /* enough space */
        ptr = ptr->next;
    }
    if (!ptr->next) {
        /* dump_list(); */
        PDEBUG("alloc failed\n");
        return 0; /* end of list */
    }
    newptr = kmalloc(sizeof(struct allocator_struct),prio);
    if (!newptr)
        return 0;

    /* ok, now stick it after ptr */
    newptr->address = ptr->address + ptr->size;
    newptr->size = bytes;
    newptr->next = ptr->next;
    ptr->next = newptr;

    /* dump_list(); */
    PDEBUG("returning 0x%08lx\n",newptr->address);
    return newptr->address;
}

int allocator_free_dma (unsigned long address)
{
    struct allocator_struct *ptr = allocator_list, *prev;

    while (ptr && ptr->next) {
        if (ptr->next->address == address)
            break;
	ptr = ptr->next;
	}
    /* the one being freed is ptr->next */
    prev = ptr; ptr = ptr->next;

    if (!ptr) {
        printk(KERN_ERR ALL_MSG "free_dma(0x%08lx) but add. not allocated\n",
               ptr->address);
        return -EINVAL;
    }
    PDEBUGG("freeing: %08lx (%li) next %08lx\n",ptr->address,ptr->size,
	   ptr->next->address);
    prev->next = ptr->next;
    kfree(ptr);

    /* dump_list(); */
    return 0;
}

/* ========================================================================
 * Init and cleanup
 *
 * On cleanup everything is released. If the list is not empty, that a
 * problem of our clients
 */
int allocator_init(void)
{
    /* check how much free memory is there */

    volatile void *remapped;
    unsigned long trial_size = allocator_himem<<20;
    unsigned long last_trial = 0;
    int step = !(allocator_himem); /* no step if size known */
    unsigned long i=0;
    struct allocator_struct *head, *tail;
    char test_string[]="0123456789abcde"; /* 16 bytes */

    PDEBUGG("himem = %i\n",allocator_himem);
    if (allocator_himem < 0) /* don't even try */
        return -EINVAL;

    if (!trial_size) trial_size = 1<<20; /* not specified: try one meg */

    while (1) {
        remapped = ioremap(__pa(high_memory), trial_size);
        if (!remapped)
            break;
        PDEBUGG("Trying %li megs (at %p, %p)\n",trial_size>>20,
	       (void *)__pa(high_memory), remapped);
        for (i=last_trial; i<trial_size; i+=16) {
            strcpy((char *)(remapped)+i, test_string);
            if (strcmp((char *)remapped+i, test_string))
                break;
        }
        iounmap((void *)remapped);
        schedule();
        last_trial = trial_size;
        if (i==trial_size)
            trial_size <<= step; /* double, if all went well */
        else
            break;
        if (!step) break;
    }
    PDEBUG("%li megs (%li k, %li b)\n",i>>20,i>>10,i);
    allocator_buffer_size = i;
    allocator_buffer = __pa(high_memory);
    if (!allocator_buffer_size) {
        printk(KERN_WARNING ALL_MSG "no free high memory to use\n");
        return -ENOMEM;
    }

    /*
     * to simplify things, always have two cells in the list:
     * the first and the last. This avoids some conditionals and
     * extra code when allocating and deallocating: we only play
     * in the middle of the list
     */
    head = kmalloc(sizeof(struct allocator_struct),GFP_KERNEL);
    if (!head)
        return -ENOMEM;
    tail = kmalloc(sizeof(struct allocator_struct),GFP_KERNEL);
    if (!tail) {
        kfree(head);
        return -ENOMEM;
    }

    head->size = tail->size = 0;
    head->address = allocator_buffer;
    tail->address = allocator_buffer + allocator_buffer_size;
    head->next = tail;
    tail->next = NULL;
    allocator_list = head;
    allocator_register_symtab(); /* only used for 2.0 if this is a module */
    return 0; /* ok, ready */
}

void allocator_cleanup(void)
{
    struct allocator_struct *ptr, *next;

    for (ptr = allocator_list; ptr; ptr = next) {
        next = ptr->next;
        PDEBUG("freeing list: 0x%08lx\n",ptr->address);
        kfree(ptr);
    }

    allocator_buffer      = 0;
    allocator_buffer_size = 0;
    allocator_list = NULL;
}


module_init(allocator_init);
module_exit(allocator_cleanup);
