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
 * Stack addresses.  Simply handles the storage of call stack details
 * which may include function names if symbols could be read from the
 * executable file.
 */


#include "addr.h"
#include "utils.h"


#if MP_IDENT_SUPPORT
#ident "$Id: addr.c,v 1.10 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *addr_id = "$Id: addr.c,v 1.10 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of an addrhead so that the stack address table
 * becomes empty.
 */

MP_GLOBAL
void
__mp_newaddrs(addrhead *h, heaphead *e)
{
    struct { char x; addrnode y; } z;
    long n;

    h->heap = e;
    /* Determine the minimum alignment for an address node on this system
     * and force the alignment to be a power of two.  This information
     * is used when initialising the slot table.
     */
    n = (char *) &z.y - &z.x;
    __mp_newslots(&h->table, sizeof(addrnode), __mp_poweroftwo(n));
    __mp_newlist(&h->list);
    h->size = 0;
    h->prot = MA_NOACCESS;
    h->protrecur = 0;
}


/* Forget all data currently in the stack address table.
 */

MP_GLOBAL
void
__mp_deleteaddrs(addrhead *h)
{
    /* We don't need to explicitly free any memory as this is dealt with
     * at a lower level by the heap manager.
     */
    h->heap = NULL;
    h->table.free = NULL;
    h->table.size = 0;
    __mp_newlist(&h->list);
    h->size = 0;
    h->prot = MA_NOACCESS;
    h->protrecur = 0;
}


/* Allocate a new address node.
 */

static
addrnode *
getaddrnode(addrhead *h)
{
    addrnode *n;
    heapnode *p;

    /* If we have no more address node slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((n = (addrnode *) __mp_getslot(&h->table)) == NULL)
    {
        if ((p = __mp_heapalloc(h->heap, h->heap->memory.page * MP_ALLOCFACTOR,
              h->table.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&h->table, p->block, p->size);
        n = (addrnode *) __mp_getslot(&h->table);
        __mp_addtail(&h->list, &n->index.node);
        n->index.block = p->block;
        n->index.size = p->size;
        h->size += p->size;
        n = (addrnode *) __mp_getslot(&h->table);
    }
    return n;
}


/* Allocate new address nodes for the supplied call stack.
 */

MP_GLOBAL
addrnode *
__mp_getaddrs(addrhead *h, stackinfo *p)
{
    addrnode *m, *n;
    stackinfo s;

    s = *p;
    if ((p->frame == NULL) || (p->addr == NULL))
        m = NULL;
    else if (m = getaddrnode(h))
    {
        m->data.next = NULL;
        /* We don't fill in the symbol name field at the moment as that might
         * take a while to locate in the symbol table.  Instead we'll do it
         * when the name really needs to be printed out or stored.
         */
        m->data.name = NULL;
        m->data.addr = p->addr;
        n = m;
        /* Traverse the call stack, allocating a new address node
         * for each entry.
         */
        while (__mp_getframe(p) && (p->addr != NULL))
        {
            if ((n->data.next = getaddrnode(h)) == NULL)
            {
                __mp_freeaddrs(h, m);
                m = NULL;
                break;
            }
            n = n->data.next;
            n->data.next = NULL;
            n->data.name = NULL;
            n->data.addr = p->addr;
        }
    }
    *p = s;
    return m;
}


/* Free up an entire call stack of address nodes.
 */

MP_GLOBAL
void
__mp_freeaddrs(addrhead *h, addrnode *n)
{
    addrnode *m;

    while (n != NULL)
    {
        m = n->data.next;
        __mp_freeslot(&h->table, n);
        n = m;
    }
}


/* Protect the memory blocks used by the stack address table with the
 * supplied access permission.
 */

MP_GLOBAL
int
__mp_protectaddrs(addrhead *h, memaccess a)
{
    addrnode *n;

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
    for (n = (addrnode *) h->list.head; n->index.node.next != NULL;
         n = (addrnode *) n->index.node.next)
        if (!__mp_memprotect(&h->heap->memory, n->index.block, n->index.size,
             a))
            return 0;
    return 1;
}


/* Recursively compare the return addresses of two call stacks.
 */

MP_GLOBAL
stackcompare
__mp_compareaddrs(addrnode *n, addrnode *p)
{
    stackcompare r;

    if ((n == NULL) || (p == NULL))
        if (n != NULL)
            r = SC_SHALLOWER;
        else if (p != NULL)
            r = SC_DEEPER;
        else
            r = SC_SAME;
    else if (((r = __mp_compareaddrs(n->data.next, p->data.next)) == SC_SAME) &&
             (n->data.addr != p->data.addr))
        r = SC_DIFFERENT;
    return r;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
