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
 * Slot tables.  New free entries for a slot table are initialised by
 * supplying a block of contiguous memory which is to be used for slot
 * entries.  This memory is divided into slots and all of these slots are
 * then added to the free chain.  Other blocks of memory can be added at a
 * later time in order to expand the table further, and these memory blocks
 * need not be adjacent to the existing blocks.
 */


#include "slots.h"
#include "utils.h"


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *slots_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a slot table so that the table becomes empty
 * and there are no free slots.
 */

MP_GLOBAL
void
__mp_newslots(slottable *t, size_t s, size_t a)
{
    struct { char x; slotentry y; } z;
    long n;

    t->free = NULL;
    /* Determine the minimum alignment for a slot entry on this system.
     * If the supplied alignment is less than this then force it to be the
     * minimum.  In any case, force the alignment to be a power of two.
     */
    n = (char *) &z.y - &z.x;
    if (a < (unsigned long) n)
        a = n;
    t->entalign = __mp_poweroftwo(a);
    /* If the supplied size is less than the size of a slot entry then
     * force it to be the size of a slot entry.
     */
    if (s < sizeof(slotentry))
        s = sizeof(slotentry);
    t->entsize = s;
    t->size = 0;
}


/* Add a new block of memory to be used for free slots to a slot table.
 */

MP_GLOBAL
size_t
__mp_initslots(slottable *t, void *p, size_t s)
{
    slotentry *e;
    char *b;
    size_t l, n;

    /* Obtain a pointer for the first slot entry, given that the pointer
     * to the supplied block may not be suitably aligned.
     */
    e = (slotentry *) __mp_roundup((unsigned long) p, t->entalign);
    s -= (char *) e - (char *) p;
    b = (char *) e + t->entsize;
    l = __mp_roundup(t->entsize, t->entalign) - t->entsize;
    for (n = 0, p = (char *) p + s; b <= (char *) p; n++)
    {
        e->next = t->free;
        t->free = e;
        e = (slotentry *) (b + l);
        b = (char *) e + t->entsize;
    }
    return n;
}


/* Return a free slot entry from a slot table, or NULL if the table is full.
 */

MP_GLOBAL
void *
__mp_getslot(slottable *t)
{
    slotentry *e;

    if ((e = t->free) != NULL)
    {
        t->free = e->next;
        t->size++;
    }
    return (void *) e;
}


/* Free up an allocated slot entry and return it to the free chain in the
 * slot table.
 */

MP_GLOBAL
void
__mp_freeslot(slottable *t, void *p)
{
    slotentry *e;

    e = (slotentry *) p;
    e->next = t->free;
    t->free = e;
    t->size--;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
