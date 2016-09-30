#ifndef MP_ADDR_H
#define MP_ADDR_H


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
 * Stack addresses.  This module stores details of the call stack for
 * each memory allocation if applicable.  The actual traversal of the
 * call stack takes place elsewhere.
 */


/*
 * $Id$
 */


#include "config.h"
#include "heap.h"
#include "list.h"
#include "stack.h"


/* The different types of call stack comparison results.
 */

typedef enum stackcompare
{
    SC_SAME,      /* same depth, same addresses */
    SC_DIFFERENT, /* same depth, different addresses */
    SC_SHALLOWER, /* shallower call stack */
    SC_DEEPER     /* deeper call stack */
}
stackcompare;


/* An address node belongs to a stack of address nodes and contains
 * details of a single function in the call stack.  An internal address
 * node stores details of a single memory block allocated for address
 * node slots.
 */

typedef union addrnode
{
    struct
    {
        listnode node;        /* internal list node */
        void *block;          /* pointer to block of memory */
        size_t size;          /* size of block of memory */
    }
    index;
    struct
    {
        union addrnode *next; /* next address node in call stack */
        char *name;           /* name of function */
        void *addr;           /* return address in function */
    }
    data;
}
addrnode;


/* An addrhead holds the table of address node slots as well as the
 * internal list of memory blocks allocated for address node slots.
 */

typedef struct addrhead
{
    heaphead *heap;   /* pointer to heap */
    slottable table;  /* table of address nodes */
    listhead list;    /* internal list of memory blocks */
    size_t size;      /* number of nodes allocated */
    memaccess prot;   /* protection status */
    size_t protrecur; /* protection recursion count */
}
addrhead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newaddrs(addrhead *, heaphead *);
MP_EXPORT void __mp_deleteaddrs(addrhead *);
MP_EXPORT addrnode *__mp_getaddrs(addrhead *, stackinfo *);
MP_EXPORT void __mp_freeaddrs(addrhead *, addrnode *);
MP_EXPORT int __mp_protectaddrs(addrhead *, memaccess);
MP_EXPORT stackcompare __mp_compareaddrs(addrnode *, addrnode *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_ADDR_H */
