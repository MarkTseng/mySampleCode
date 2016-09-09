#ifndef MP_ALLOC_H
#define MP_ALLOC_H


/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2002 Graeme S. Roy <graeme.roy@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/*
 * Memory allocations.  The allocation, deallocation and reuse of blocks
 * of heap memory is performed here by storing details of each memory
 * allocation in a binary search tree for the purpose of locating them
 * quickly and providing efficient reuse of blocks.
 */


/*
 * $Id: alloc.h,v 1.10 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include "heap.h"
#include "list.h"


#define FLG_NOFREE     1  /* do not reuse freed allocations */
#define FLG_PRESERVE   2  /* preserve freed contents */
#define FLG_OFLOWWATCH 4  /* watch overflow buffers */
#define FLG_PAGEALLOC  8  /* all allocations are pages */
#define FLG_ALLOCUPPER 16 /* allocations aligned to ends of pages */


/* An allocation node belongs to one of three binary search trees of
 * allocation nodes.  The allocation tree stores details of allocated
 * blocks, while the freed tree stores details of freed blocks (if an option
 * is given to preserve all freed blocks).  Both trees are ordered by
 * the start address of the memory allocation.  The free tree stores
 * details of free blocks and is ordered by the size of the free block.
 */

typedef struct allocnode
{
    listnode lnode; /* list node */
    listnode fnode; /* freed list node */
    treenode tnode; /* tree node */
    void *block;    /* pointer to block of memory */
    size_t size;    /* size of block of memory */
    void *info;     /* associated information */
}
allocnode;


/* An allochead contains the slot table of allocation nodes, as well as
 * the three trees (including a fourth tree for storing details of internal
 * blocks), a list for storing all memory blocks in ascending order of
 * start address, and associated settings controlling how the memory
 * allocations are filled and if they are to have overflow buffers.
 */

typedef struct allochead
{
    heaphead heap;       /* pointer to heap */
    slottable table;     /* table of allocation nodes */
    listhead list;       /* list of allocation nodes */
    listhead flist;      /* list of freed allocations */
    treeroot itree;      /* internal allocation tree */
    treeroot atree;      /* allocation tree */
    treeroot gtree;      /* freed tree */
    treeroot ftree;      /* free tree */
    size_t isize;        /* internal allocation total */
    size_t asize;        /* allocation total */
    size_t gsize;        /* freed total */
    size_t fsize;        /* free total */
    size_t fmax;         /* maximum number of freed allocations */
    size_t oflow;        /* overflow buffer size */
    unsigned char obyte; /* overflow byte */
    unsigned char abyte; /* allocation byte */
    unsigned char fbyte; /* free byte */
    unsigned long flags; /* allocation flags */
    memaccess prot;      /* protection status */
    size_t protrecur;    /* protection recursion count */
}
allochead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newallocs(allochead *, size_t, size_t, unsigned char,
                              unsigned char, unsigned char, unsigned long);
MP_EXPORT void __mp_deleteallocs(allochead *);
MP_EXPORT allocnode *__mp_getalloc(allochead *, size_t, size_t, void *);
MP_EXPORT int __mp_resizealloc(allochead *, allocnode *, size_t);
MP_EXPORT void __mp_freealloc(allochead *, allocnode *, void *);
MP_EXPORT void __mp_recyclefreed(allochead *);
MP_EXPORT int __mp_protectalloc(allochead *, memaccess);
MP_EXPORT allocnode *__mp_findalloc(allochead *, void *);
MP_EXPORT allocnode *__mp_findfreed(allochead *, void *);
MP_EXPORT allocnode *__mp_findnode(allochead *, void *, size_t);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_ALLOC_H */
