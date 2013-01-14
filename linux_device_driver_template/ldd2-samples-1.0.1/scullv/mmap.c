/*  -*- C -*-
 * mmap.c -- memory mapping for the scullv char module
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

#include "scullv.h"        /* local definitions */

#ifdef LINUX_20
static struct mm_struct *init_mm_ptr;
#define init_mm (*init_mm_ptr) /* to avoid ifdefs later */

static void retrieve_init_mm_ptr(void)
{
    struct task_struct *p;

    for (p = current ; (p = p->next_task) != current ; )
        if (p->pid == 0)
            break;

    init_mm_ptr = p->mm;
}
#endif



/*
 * Given a VMA, get our device pointer from it.
 */
static inline ScullV_Dev *scullv_vma_to_dev (struct vm_area_struct *vma)
{
#if defined(LINUX_24)
    return (ScullV_Dev *) vma->vm_private_data;
#elif defined(LINUX_22)
    struct inode *inode = INODE_FROM_F(vma->vm_file); 
    return scullv_devices + MINOR(inode->i_rdev);
#else /* LINUX_20 */
    return scullv_devices + MINOR(vma->vm_inode->i_rdev);
#endif
}

/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void scullv_vma_open(struct vm_area_struct *vma)
{
    ScullV_Dev *dev = scullv_vma_to_dev(vma);

    dev->vmas++;
    MOD_INC_USE_COUNT;
}

void scullv_vma_close(struct vm_area_struct *vma)
{
    ScullV_Dev *dev = scullv_vma_to_dev(vma);

    dev->vmas--;
    MOD_DEC_USE_COUNT;
}

/*
 * The nopage method: the core of the file. It retrieves the
 * page required from the scullv device and returns it to the
 * user. The count for the page must be incremented, because
 * it is automatically decremented at page unmap.
 *
 * For this reason, "order" must be zero. Otherwise, only the first
 * page has its count incremented, and the allocating module must
 * release it as a whole block. Therefore, it isn't possible to map
 * pages from a multipage block: when they are unmapped, their count
 * is individually decreased, and would drop to 0.
 */

struct page *scullv_vma_nopage(struct vm_area_struct *vma,
                                unsigned long address, int write)
{
    unsigned long offset;
    ScullV_Dev *ptr, *dev = scullv_vma_to_dev(vma);
    struct page *page = NOPAGE_SIGBUS;
    void *pageptr = NULL; /* default to "missing" */
    pgd_t *pgd; pmd_t *pmd; pte_t *pte;
    unsigned long lpage;

    down(&dev->sem);
    offset = (address - vma->vm_start) + VMA_OFFSET(vma);
    if (offset >= dev->size) goto out; /* out of range */

    /*
     * Now retrieve the scullv device from the list,then the page.
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

    /*
     * After scullv lookup, "page" is now the address of the page
     * needed by the current process. Since it's a vmalloc address,
     * first retrieve the unsigned long value to be looked up
     * in page tables.
     */
    lpage = VMALLOC_VMADDR(pageptr);
#ifdef LINUX_24						
    spin_lock(&init_mm.page_table_lock);
#endif  						
    pgd = pgd_offset(&init_mm, lpage);
    pmd = pmd_offset(pgd, lpage);
    pte = pte_offset(pmd, lpage);
#ifdef LINUX_24  					
    page = pte_page(*pte);
    spin_unlock(&init_mm.page_table_lock);
#else  							
    page = mem_map + MAP_NR(pte_page(*pte));  		
#endif  						
    
    /* got it, now increment the count */
    get_page(page);
out:
    up(&dev->sem);
    return page;
}


#ifndef LINUX_24
unsigned long scullv_vma_nopage_old(struct vm_area_struct *vma,
                unsigned long address, int write)
{
    struct page *page = scullv_vma_nopage(vma, address, write);
    if (page)
#ifdef LINUX_20
        return (page->map_nr << PAGE_SHIFT);
#else  /* 2.2 */
        return (unsigned long) __va ((page - mem_map) << PAGE_SHIFT);
#endif
    return 0;
}

#define scullv_vma_nopage scullv_vma_nopage_old /* for the ops table */

#endif /* not 2.4 */


struct vm_operations_struct scullv_vm_ops = {
    open:     scullv_vma_open,
    close:  scullv_vma_close,
    nopage:   scullv_vma_nopage,
};


#ifndef LINUX_20
int scullv_mmap(struct file *filp, struct vm_area_struct *vma)
{
    struct inode *inode = INODE_FROM_F(filp);

    if (VMA_OFFSET(vma) & (PAGE_SIZE-1))                
        return -ENXIO; /* need aligned offsets */	

    /* don't do anything here: "nopage" will fill the holes */
    vma->vm_ops = &scullv_vm_ops;
    vma->vm_flags |= VM_RESERVED;
#ifdef LINUX_24					
    vma->vm_private_data = scullv_devices + MINOR(inode->i_rdev);
#else						
    vma->vm_file = filp;			
#endif						
    scullv_vma_open(vma);
    return 0;
}

#else /* LINUX_20 */
int scullv_mmap(struct inode *inode, struct file *filp,
                struct vm_area_struct *vma)
{

    if (VMA_OFFSET(vma) & (PAGE_SIZE-1))
        return -ENXIO; /* need aligned offsets */

    /*
     * A pointer to init_mm is needed to access page tables
     */
    if (!init_mm_ptr)
        retrieve_init_mm_ptr();

    /* don't do anything here: "nopage" will fill the holes */
    vma->vm_ops = &scullv_vm_ops;
    vma->vm_inode = inode;
    inode->i_count++;
    scullv_vma_open(vma);
    return 0;
}

#endif /* LINUX_20 */
