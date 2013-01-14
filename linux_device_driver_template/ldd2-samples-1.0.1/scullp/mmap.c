/*  -*- C -*-
 * mmap.c -- memory mapping for the scullp char module
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
 * $Id: _mmap.c.in,v 1.21 2001/07/18 22:28:18 rubini Exp $
 */

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#define __NO_VERSION__
#include <linux/module.h>

#include <linux/mm.h>     /* everything */
#include <linux/errno.h>  /* error codes */
#include <asm/pgtable.h>

#include "scullp.h"        /* local definitions */




/*
 * Given a VMA, get our device pointer from it.
 */
static inline ScullP_Dev *scullp_vma_to_dev (struct vm_area_struct *vma)
{
#if defined(LINUX_24)
    return (ScullP_Dev *) vma->vm_private_data;
#elif defined(LINUX_22)
    struct inode *inode = INODE_FROM_F(vma->vm_file); 
    return scullp_devices + MINOR(inode->i_rdev);
#else /* LINUX_20 */
    return scullp_devices + MINOR(vma->vm_inode->i_rdev);
#endif
}

/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void scullp_vma_open(struct vm_area_struct *vma)
{
    ScullP_Dev *dev = scullp_vma_to_dev(vma);

    dev->vmas++;
    MOD_INC_USE_COUNT;
}

void scullp_vma_close(struct vm_area_struct *vma)
{
    ScullP_Dev *dev = scullp_vma_to_dev(vma);

    dev->vmas--;
    MOD_DEC_USE_COUNT;
}

/*
 * The nopage method: the core of the file. It retrieves the
 * page required from the scullp device and returns it to the
 * user. The count for the page must be incremented, because
 * it is automatically decremented at page unmap.
 *
 * For this reason, "order" must be zero. Otherwise, only the first
 * page has its count incremented, and the allocating module must
 * release it as a whole block. Therefore, it isn't possible to map
 * pages from a multipage block: when they are unmapped, their count
 * is individually decreased, and would drop to 0.
 */

struct page *scullp_vma_nopage(struct vm_area_struct *vma,
                                unsigned long address, int write)
{
    unsigned long offset;
    ScullP_Dev *ptr, *dev = scullp_vma_to_dev(vma);
    struct page *page = NOPAGE_SIGBUS;
    void *pageptr = NULL; /* default to "missing" */

    down(&dev->sem);
    offset = (address - vma->vm_start) + VMA_OFFSET(vma);
    if (offset >= dev->size) goto out; /* out of range */

    /*
     * Now retrieve the scullp device from the list,then the page.
     * If the device has holes, the process receives a SIGBUS when
     * accessing the hole.
     */
    offset >>= PAGE_SHIFT; /* offset is a number of pages */
    for (ptr = dev; ptr && offset >= dev->qset;) {
        ptr = ptr->next;
        offset -= dev->qset;
    }
    if (ptr && ptr->data) pageptr = ptr->data[offset];
    if (!pageptr) goto out; /* hole or end-of-file */
    page = virt_to_page(pageptr);
    
    /* got it, now increment the count */
    get_page(page);
out:
    up(&dev->sem);
    return page;
}


#ifndef LINUX_24
unsigned long scullp_vma_nopage_old(struct vm_area_struct *vma,
                unsigned long address, int write)
{
    struct page *page = scullp_vma_nopage(vma, address, write);
    if (page)
#ifdef LINUX_20
        return (page->map_nr << PAGE_SHIFT);
#else  /* 2.2 */
        return (unsigned long) __va ((page - mem_map) << PAGE_SHIFT);
#endif
    return 0;
}

#define scullp_vma_nopage scullp_vma_nopage_old /* for the ops table */

#endif /* not 2.4 */


struct vm_operations_struct scullp_vm_ops = {
    open:     scullp_vma_open,
    close:  scullp_vma_close,
    nopage:   scullp_vma_nopage,
};


#ifndef LINUX_20
int scullp_mmap(struct file *filp, struct vm_area_struct *vma)
{
    struct inode *inode = INODE_FROM_F(filp);

    /* refuse to map if order is not 0 */
    if (scullp_devices[MINOR(inode->i_rdev)].order)
        return -ENODEV;
    if (VMA_OFFSET(vma) & (PAGE_SIZE-1))                
        return -ENXIO; /* need aligned offsets */	

    /* don't do anything here: "nopage" will fill the holes */
    vma->vm_ops = &scullp_vm_ops;
    vma->vm_flags |= VM_RESERVED;
#ifdef LINUX_24					
    vma->vm_private_data = scullp_devices + MINOR(inode->i_rdev);
#else						
    vma->vm_file = filp;			
#endif						
    scullp_vma_open(vma);
    return 0;
}

#else /* LINUX_20 */
int scullp_mmap(struct inode *inode, struct file *filp,
                struct vm_area_struct *vma)
{
    /* refuse to map if order is not 0 */
    if (scullp_devices[MINOR(inode->i_rdev)].order)
        return -ENODEV;

    if (VMA_OFFSET(vma) & (PAGE_SIZE-1))
        return -ENXIO; /* need aligned offsets */


    /* don't do anything here: "nopage" will fill the holes */
    vma->vm_ops = &scullp_vm_ops;
    vma->vm_inode = inode;
    inode->i_count++;
    scullp_vma_open(vma);
    return 0;
}

#endif /* LINUX_20 */
