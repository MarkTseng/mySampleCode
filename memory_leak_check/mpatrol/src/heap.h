#ifndef MP_HEAP_H
#define MP_HEAP_H


/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Memory heaps.  All dynamic memory allocations made by the mpatrol
 * library are made through this module, which keeps track of each
 * individual block of memory allocated by the low-level memory
 * allocator.  Note that such a heap may not be (and is not guaranteed
 * to be) composed of contiguous blocks of memory.
 */


/*
 * $Id$
 */


#include "config.h"
#include "memory.h"
#include "slots.h"
#include "tree.h"


/* A heap node belongs to a binary search tree of heap nodes, ordered
 * by address, and contains details of a single allocated block of memory.
 */

typedef struct heapnode
{
    treenode node; /* tree node */
    void *block;   /* pointer to block of memory */
    size_t size;   /* size of block of memory */
}
heapnode;


/* A heap head contains the slot table of heap nodes, whose memory blocks
 * are allocated in the internal allocation tree.
 */

typedef struct heaphead
{
    memoryinfo memory; /* memory details */
    slottable table;   /* table of heap nodes */
    treeroot itree;    /* internal allocation tree */
    treeroot dtree;    /* heap node allocation tree */
    size_t isize;      /* internal allocation total */
    size_t dsize;      /* heap node allocation total */
    memaccess prot;    /* protection status */
    size_t protrecur;  /* protection recursion count */
    char tracing;      /* heap reservation tracing status */
}
heaphead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newheap(heaphead *);
MP_EXPORT void __mp_deleteheap(heaphead *);
MP_EXPORT heapnode *__mp_heapalloc(heaphead *, size_t, size_t, int);
MP_EXPORT void __mp_heapfree(heaphead *, heapnode *);
MP_EXPORT int __mp_heapprotect(heaphead *, memaccess);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_HEAP_H */
