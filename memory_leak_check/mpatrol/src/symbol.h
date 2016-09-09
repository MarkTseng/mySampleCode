#ifndef MP_SYMBOL_H
#define MP_SYMBOL_H


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
 * Symbol tables.  Symbol table reading from executable files is handled
 * from this module, which provides a common interface across multiple
 * executable file formats.
 */


/*
 * $Id: symbol.h,v 1.18 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include "heap.h"
#include "strtab.h"


/* A symbol node belongs to a binary search tree of symbol nodes, ordered by
 * start address, and contains details of a single symbol read from an
 * executable file.  An internal symbol node stores details of a single memory
 * block allocated for symbol node slots.
 */

typedef union symnode
{
    struct
    {
        treenode node;        /* internal tree node */
        void *block;          /* pointer to block of memory */
        size_t size;          /* size of block of memory */
    }
    index;
    struct
    {
        treenode node;        /* tree node */
        char *file;           /* file name */
        char *name;           /* symbol name */
        void *addr;           /* start address */
        size_t size;          /* size of symbol */
        unsigned long index;  /* profiling table index */
        unsigned long offset; /* profiling table offset */
        unsigned long flags;  /* format-dependent flags */
    }
    data;
}
symnode;


/* A symhead holds the table of symbol node slots as well as the
 * internal tree of memory blocks allocated for symbol node slots and
 * the string table used for holding symbol names.
 */

typedef struct symhead
{
    heaphead *heap;   /* pointer to heap */
    strtab strings;   /* string table */
    slottable table;  /* table of symbol nodes */
    treeroot itree;   /* internal allocation tree */
    treeroot dtree;   /* symbol node allocation tree */
    size_t size;      /* number of symbol nodes in table */
    void *hhead;      /* access library handle list head */
    void *htail;      /* access library handle list tail */
    void *inits;      /* initialisation functions handle */
    memaccess prot;   /* protection status */
    size_t protrecur; /* protection recursion count */
    char lineinfo;    /* line number information flag */
}
symhead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newsymbols(symhead *, heaphead *, void *);
MP_EXPORT void __mp_closesymbols(symhead *);
MP_EXPORT void __mp_deletesymbols(symhead *);
MP_EXPORT int __mp_addsymbols(symhead *, char *, char *, size_t);
MP_EXPORT int __mp_addextsymbols(symhead *, meminfo *);
MP_EXPORT void __mp_fixsymbols(symhead *);
MP_EXPORT int __mp_protectsymbols(symhead *, memaccess);
MP_EXPORT symnode *__mp_findsymbol(symhead *, void *);
MP_EXPORT int __mp_findsource(symhead *, void *, char **, char **,
                              unsigned long *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_SYMBOL_H */
