#ifndef MP_SLOTS_H
#define MP_SLOTS_H


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
 * Slot tables.  All fixed-size data structures used within the mpatrol
 * library have chunks of space allocated for them in the form of slot
 * tables, which are essentially dynamically-allocated arrays that contain
 * a chain of all free slots.  Only the linkage between slots is dealt with
 * by this module - dynamically allocating memory for slot tables is done
 * elsewhere.
 */


/*
 * $Id: slots.h,v 1.6 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include <stddef.h>


/* A slot entry can be either allocated or free.  When allocated, it contains
 * the data that is to be stored in the slot table.  When free, it contains
 * a pointer to the next free slot in the chain.
 */

typedef struct slotentry
{
    struct slotentry *next; /* next free slot in the chain */
}
slotentry;


/* A slot table contains information about the minimum alignment and size of
 * each slot entry and also a pointer to the chain of free slots.
 */

typedef struct slottable
{
    struct slotentry *free; /* first free slot in the chain */
    size_t entalign;        /* alignment for each slot entry */
    size_t entsize;         /* size of a single slot entry */
    size_t size;            /* number of slots in table */
}
slottable;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newslots(slottable *, size_t, size_t);
MP_EXPORT size_t __mp_initslots(slottable *, void *, size_t);
MP_EXPORT void *__mp_getslot(slottable *);
MP_EXPORT void __mp_freeslot(slottable *, void *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_SLOTS_H */
