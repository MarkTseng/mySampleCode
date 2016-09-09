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
 * Memory heaps.  All memory allocations are performed by the low-level
 * memory allocator.  All heap nodes are stored in a slot table whose
 * memory blocks are stored in an internal allocation tree.  All other
 * memory blocks allocated are stored in the heap node allocation tree.
 */


#include "heap.h"
#include "trace.h"
#include "utils.h"


#if MP_IDENT_SUPPORT
#ident "$Id: heap.c,v 1.14 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *heap_id = "$Id: heap.c,v 1.14 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if MP_INUSE_SUPPORT
void _Inuse_heapalloc(void *, unsigned long);
#endif /* MP_INUSE_SUPPORT */


/* Initialise the fields of a heap head so that the heap becomes empty.
 */

MP_GLOBAL
void
__mp_newheap(heaphead *h)
{
    struct { char x; heapnode y; } z;
    long n;

    __mp_newmemory(&h->memory);
    /* Determine the minimum alignment for a heap node on this system
     * and force the alignment to be a power of two.  This information
     * is used when initialising the slot table.
     */
    n = (char *) &z.y - &z.x;
    __mp_newslots(&h->table, sizeof(heapnode), __mp_poweroftwo(n));
    __mp_newtree(&h->itree);
    __mp_newtree(&h->dtree);
    h->isize = h->dsize = 0;
    h->prot = MA_NOACCESS;
    h->protrecur = 0;
    h->tracing = 0;
}


/* Return all of the memory allocated by a heap back to the system.
 */

MP_GLOBAL
void
__mp_deleteheap(heaphead *h)
{
    heapnode *n, *p;

    /* Free up the memory pointed to by the heap nodes first, since the
     * heap nodes themselves are freed in the second loop.
     */
    for (n = (heapnode *) __mp_minimum(h->dtree.root); n != NULL; n = p)
    {
        p = (heapnode *) __mp_successor(&n->node);
        __mp_heapfree(h, n);
    }
    for (n = (heapnode *) __mp_minimum(h->itree.root); n != NULL; n = p)
    {
        p = (heapnode *) __mp_successor(&n->node);
        __mp_treeremove(&h->itree, &n->node);
        __mp_memfree(&h->memory, n->block, n->size);
    }
    __mp_endmemory(&h->memory);
    h->table.free = NULL;
    h->table.size = 0;
    h->isize = 0;
    h->prot = MA_NOACCESS;
    h->protrecur = 0;
    h->tracing = 0;
}


/* Allocate a new block of memory for the heap, or return NULL if no
 * memory block could be allocated.
 */

MP_GLOBAL
heapnode *
__mp_heapalloc(heaphead *h, size_t l, size_t a, int i)
{
    heapnode *n;
    void *p;
    size_t s;

    /* If we have no more heap node slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((n = (heapnode *) __mp_getslot(&h->table)) == NULL)
    {
        s = h->memory.page * MP_ALLOCFACTOR;
        if ((p = __mp_memalloc(&h->memory, &s, h->table.entalign, 0)) == NULL)
            return NULL;
        __mp_initslots(&h->table, p, s);
        n = (heapnode *) __mp_getslot(&h->table);
        __mp_treeinsert(&h->itree, &n->node, (unsigned long) p);
        n->block = p;
        n->size = s;
        h->isize += s;
        if (h->tracing)
            __mp_traceheap(p, s, 1);
#if MP_INUSE_SUPPORT
        _Inuse_heapalloc(p, s);
#endif /* MP_INUSE_SUPPORT */
        n = (heapnode *) __mp_getslot(&h->table);
    }
    /* Allocate the requested block of memory and add it to the heap.
     */
    if ((p = __mp_memalloc(&h->memory, &l, a, !i)) == NULL)
    {
        __mp_freeslot(&h->table, n);
        return NULL;
    }
    __mp_treeinsert(&h->dtree, &n->node, (unsigned long) p);
    n->block = p;
    n->size = l;
    h->dsize += l;
    if (h->tracing)
        __mp_traceheap(p, l, i);
#if MP_INUSE_SUPPORT
    _Inuse_heapalloc(p, l);
#endif /* MP_INUSE_SUPPORT */
    return n;
}


/* Return a block of memory belonging to the heap back to the system.
 */

MP_GLOBAL
void
__mp_heapfree(heaphead *h, heapnode *n)
{
    h->dsize -= n->size;
    __mp_memfree(&h->memory, n->block, n->size);
    __mp_treeremove(&h->dtree, &n->node);
    __mp_freeslot(&h->table, n);
}


/* Protect the internal memory blocks used by the heap with the
 * supplied access permission.
 */

MP_GLOBAL
int
__mp_heapprotect(heaphead *h, memaccess a)
{
    heapnode *n;

    /* The library already knows what its protection status is so we don't
     * need to do anything if the request has already been done.
     */
    if (h->prot == a)
    {
        h->protrecur++;
        return 1;
    }
    else if (h->protrecur > 0)
    {
        h->protrecur--;
        return 1;
    }
    h->prot = a;
    for (n = (heapnode *) __mp_minimum(h->itree.root); n != NULL;
         n = (heapnode *) __mp_successor(&n->node))
        if (!__mp_memprotect(&h->memory, n->block, n->size, a))
            return 0;
    return 1;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
