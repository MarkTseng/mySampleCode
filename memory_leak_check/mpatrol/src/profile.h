#ifndef MP_PROFILE_H
#define MP_PROFILE_H


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
 * Memory allocation profiling.  This interface is used by the mpatrol
 * library to enable or disable the profiling of dynamic memory allocations
 * to a file for later processing.
 */


/*
 * $Id: profile.h,v 1.20 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include "symbol.h"


/* A profdata structure belongs to a list of profdata structures and contains
 * statistics about the counts and totals of all of the small, medium, large
 * and extra large allocations and deallocations for a particular call site.
 * An internal profdata structure stores details of a single memory block
 * allocated for profdata structure slots.
 */

typedef union profdata
{
    struct
    {
        listnode node;       /* internal list node */
        void *block;         /* pointer to block of memory */
        size_t size;         /* size of block of memory */
    }
    index;
    struct
    {
        listnode node;       /* list node */
        unsigned long index; /* data index */
        size_t acount[4];    /* total numbers of allocations */
        size_t dcount[4];    /* total numbers of deallocations */
        size_t atotal[4];    /* total numbers of allocated bytes */
        size_t dtotal[4];    /* total numbers of deallocated bytes */
    }
    data;
}
profdata;


/* A profnode belongs to a tree of profnodes and contains profiling
 * details for a function in a call stack.  An internal profnode stores
 * details of a single memory block allocated for profnode slots.
 */

typedef union profnode
{
    struct
    {
        listnode node;          /* internal list node */
        void *block;            /* pointer to block of memory */
        size_t size;            /* size of block of memory */
    }
    index;
    struct
    {
        treenode node;          /* tree node */
        union profnode *parent; /* parent node */
        unsigned long index;    /* node index */
        void *addr;             /* return address */
        symnode *symbol;        /* associated symbol */
        profdata *data;         /* profiling data */
    }
    data;
}
profnode;


/* A profhead contains all the profiling information including the
 * filename of the output file and the current profiling state.
 */

typedef struct profhead
{
    heaphead *heap;              /* pointer to heap */
    symhead *syms;               /* pointer to symbol table */
    slottable dtable;            /* table of profdata structures */
    slottable ntable;            /* table of profnodes */
    listhead ilist;              /* internal list of memory blocks */
    listhead list;               /* list of profdata structures */
    treeroot tree;               /* tree of profnodes */
    size_t size;                 /* memory used by internal blocks */
    size_t acounts[MP_BIN_SIZE]; /* allocation bins */
    size_t dcounts[MP_BIN_SIZE]; /* deallocation bins */
    size_t atotals;              /* total bytes of large allocations */
    size_t dtotals;              /* total bytes of large deallocations */
    size_t sbound;               /* small allocation boundary */
    size_t mbound;               /* medium allocation boundary */
    size_t lbound;               /* large allocation boundary */
    unsigned long autosave;      /* autosave frequency */
    unsigned long autocount;     /* autosave count */
    char *file;                  /* profiling filename */
    memaccess prot;              /* protection status */
    size_t protrecur;            /* protection recursion count */
    char profiling;              /* profiling status */
}
profhead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newprofile(profhead *, heaphead *, symhead *);
MP_EXPORT void __mp_deleteprofile(profhead *);
MP_EXPORT int __mp_profilealloc(profhead *, size_t, void *, int);
MP_EXPORT int __mp_profilefree(profhead *, size_t, void *, int);
MP_EXPORT int __mp_writeprofile(profhead *, int);
MP_EXPORT int __mp_protectprofile(profhead *, memaccess);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_PROFILE_H */
