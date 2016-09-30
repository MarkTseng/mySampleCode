#ifndef MP_LEAKTAB_H
#define MP_LEAKTAB_H


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
 * Memory leak tables.  Such tables are used by the mpatrol library to record
 * where heap memory is allocated and freed so that a summary of where memory
 * leaks occurred can be made.
 */


/*
 * $Id$
 */


#include "config.h"
#include "list.h"
#include "heap.h"


#define SOPT_ALLOCATED 0 /* sort by allocated memory */
#define SOPT_FREED     1 /* sort by freed memory */
#define SOPT_UNFREED   2 /* sort by unfreed memory */


/* A tablenode belongs to a hash table of table nodes and contains an
 * allocation summary for a particular file name and line number, or
 * alternatively for a particular function name or return address.
 */

typedef union tablenode
{
    struct
    {
        listnode node;      /* internal list node */
        void *block;        /* pointer to block of memory */
        size_t size;        /* size of block of memory */
    }
    index;
    struct
    {
        listnode node;      /* list node */
        treenode tnode;     /* tree node */
        char *file;         /* file name, or function name if line is 0 */
        unsigned long line; /* line number, or return address if file is NULL */
        size_t acount;      /* allocation count */
        size_t atotal;      /* allocation total */
        size_t dcount;      /* deallocation count */
        size_t dtotal;      /* deallocation total */
    }
    data;
}
tablenode;


/* A leaktab holds the hash table of table nodes.
 */

typedef struct leaktab
{
    heaphead *heap;                  /* pointer to heap */
    slottable table;                 /* table of table nodes */
    listhead slots[MP_LEAKTAB_SIZE]; /* array of lists for table nodes */
    listhead list;                   /* internal list of memory blocks */
    treeroot tree;                   /* tree for sorting */
    size_t isize;                    /* memory used by internal blocks */
    size_t size;                     /* number of nodes in table */
    memaccess prot;                  /* protection status */
    size_t protrecur;                /* protection recursion count */
    char tracing;                    /* leak tracing status */
}
leaktab;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newleaktab(leaktab *, heaphead *);
MP_EXPORT void __mp_deleteleaktab(leaktab *);
MP_EXPORT void __mp_clearleaktab(leaktab *);
MP_EXPORT void __mp_sortleaktab(leaktab *, int, int);
MP_EXPORT int __mp_allocentry(leaktab *, char *, unsigned long, size_t);
MP_EXPORT int __mp_freeentry(leaktab *, char *, unsigned long, size_t);
MP_EXPORT int __mp_protectleaktab(leaktab *, memaccess);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_LEAKTAB_H */
