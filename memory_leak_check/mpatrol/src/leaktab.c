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
 * leaks occurred can be made.  The hash function comes from P. J. Weinberger's
 * C compiler and was published in Compilers: Principles, Techniques and Tools,
 * First Edition by Aho, Sethi and Ullman (Addison-Wesley, 1986, ISBN
 * 0-201-10194-7).
 */


#include "leaktab.h"
#include "utils.h"
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *leaktab_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a leaktab so that the leak table becomes empty.
 */

MP_GLOBAL
void
__mp_newleaktab(leaktab *t, heaphead *h)
{
    struct { char x; tablenode y; } z;
    size_t i;
    long n;

    t->heap = h;
    /* Determine the minimum alignment for a table node on this system and
     * force the alignment to be a power of two.  This information is used
     * when initialising the slot table.
     */
    n = (char *) &z.y - &z.x;
    __mp_newslots(&t->table, sizeof(tablenode), __mp_poweroftwo(n));
    for (i = 0; i < MP_LEAKTAB_SIZE; i++)
        __mp_newlist(&t->slots[i]);
    __mp_newlist(&t->list);
    __mp_newtree(&t->tree);
    t->isize = t->size = 0;
    t->prot = MA_NOACCESS;
    t->protrecur = 0;
    t->tracing = 0;
}


/* Forget all data currently in the leak table.
 */

MP_GLOBAL
void
__mp_deleteleaktab(leaktab *t)
{
    size_t i;

    /* We don't need to explicitly free any memory as this is dealt with
     * at a lower level by the heap manager.
     */
    t->heap = NULL;
    t->table.free = NULL;
    t->table.size = 0;
    for (i = 0; i < MP_LEAKTAB_SIZE; i++)
        __mp_newlist(&t->slots[i]);
    __mp_newlist(&t->list);
    __mp_newtree(&t->tree);
    t->isize = t->size = 0;
    t->prot = MA_NOACCESS;
    t->protrecur = 0;
    t->tracing = 0;
}


/* Clear all data currently in the leak table.
 */

MP_GLOBAL
void
__mp_clearleaktab(leaktab *t)
{
    tablenode *n;
    size_t i;

    for (i = 0; i < MP_LEAKTAB_SIZE; i++)
        while (n = (tablenode *) __mp_remhead(&t->slots[i]))
            __mp_freeslot(&t->table, n);
    __mp_newtree(&t->tree);
    t->size = 0;
}


/* Sort the leak table.
 */

MP_GLOBAL
void
__mp_sortleaktab(leaktab *t, int o, int c)
{
    tablenode *n;
    size_t i;
    unsigned long k;

    __mp_newtree(&t->tree);
    for (i = 0; i < MP_LEAKTAB_SIZE; i++)
        for (n = (tablenode *) t->slots[i].head; n->data.node.next != NULL;
             n = (tablenode *) n->data.node.next)
        {
            if (o == SOPT_ALLOCATED)
                if (c != 0)
                    k = n->data.acount;
                else
                    k = n->data.atotal;
            else if (o == SOPT_FREED)
                if (c != 0)
                    k = n->data.dcount;
                else
                    k = n->data.dtotal;
            else if (c != 0)
                k = n->data.acount - n->data.dcount;
            else
                k = n->data.atotal - n->data.dtotal;
            if (k > 0)
                __mp_treeinsert(&t->tree, &n->data.tnode, k);
        }
}


/* Calculate the hash bucket an entry should be placed in.
 */

static
unsigned long
hashloc(char *f, unsigned long l)
{
    unsigned long g, h;

    if (f != NULL)
        for (h = 0; *f != '\0'; f++)
        {
            h = (h << 4) + *f;
            if (g = h & (0xFUL << ((sizeof(unsigned long) << 3) - 4)))
            {
                h ^= g >> ((sizeof(unsigned long) << 3) - 8);
                h ^= g;
            }
        }
    else
        h = 0;
    return (h + l) % MP_LEAKTAB_SIZE;
}


/* Compare the file names and line numbers of two entries.
 */

static
int
hashcmp(char *f1, unsigned long l1, char *f2, unsigned long l2)
{
    if ((l1 != l2) || ((f1 == NULL) && (f2 != NULL)) ||
        ((f1 != NULL) && (f2 == NULL)))
        return 0;
    if ((f1 == f2) || (strcmp(f1, f2) == 0))
        return 1;
    return 0;
}


/* Allocate a new table node.
 */

static
tablenode *
gettablenode(leaktab *t)
{
    tablenode *n;
    heapnode *p;

    /* If we have no more table node slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((n = (tablenode *) __mp_getslot(&t->table)) == NULL)
    {
        if ((p = __mp_heapalloc(t->heap, t->heap->memory.page * MP_ALLOCFACTOR,
              t->table.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&t->table, p->block, p->size);
        n = (tablenode *) __mp_getslot(&t->table);
        __mp_addtail(&t->list, &n->index.node);
        n->index.block = p->block;
        n->index.size = p->size;
        t->isize += p->size;
        n = (tablenode *) __mp_getslot(&t->table);
    }
    return n;
}


/* Add a memory allocation to the leak table.
 */

MP_GLOBAL
int
__mp_allocentry(leaktab *t, char *f, unsigned long l, size_t c)
{
    tablenode *n;
    unsigned long k;

    k = hashloc(f, l);
    for (n = (tablenode *) t->slots[k].head; n->data.node.next != NULL;
         n = (tablenode *) n->data.node.next)
        if (hashcmp(n->data.file, n->data.line, f, l))
        {
            n->data.acount++;
            n->data.atotal += c;
            return 1;
        }
    if ((n = gettablenode(t)) == NULL)
        return 0;
    __mp_addhead(&t->slots[k], &n->data.node);
    n->data.file = f;
    n->data.line = l;
    n->data.acount = 1;
    n->data.atotal = c;
    n->data.dcount = 0;
    n->data.dtotal = 0;
    t->size++;
    return 1;
}


/* Remove a memory allocation from the leak table.
 */

MP_GLOBAL
int
__mp_freeentry(leaktab *t, char *f, unsigned long l, size_t c)
{
    tablenode *n;
    unsigned long k;

    k = hashloc(f, l);
    for (n = (tablenode *) t->slots[k].head; n->data.node.next != NULL;
         n = (tablenode *) n->data.node.next)
        if (hashcmp(n->data.file, n->data.line, f, l))
        {
            n->data.dcount++;
            if (n->data.dcount > n->data.acount)
                n->data.dcount = n->data.acount;
            n->data.dtotal += c;
            if (n->data.dtotal > n->data.atotal)
                n->data.dtotal = n->data.atotal;
            return 1;
        }
    return 0;
}


/* Protect the memory blocks used by the leak table with the supplied access
 * permission.
 */

MP_GLOBAL
int
__mp_protectleaktab(leaktab *t, memaccess a)
{
    tablenode *n;

    /* The library already knows what its protection status is so we don't
     * need to do anything if the request has already been done.
     */
    if (t->prot == a)
    {
        t->protrecur++;
        return 1;
    }
    else if (t->protrecur > 0)
    {
        t->protrecur--;
        return 1;
    }
    t->prot = a;
    for (n = (tablenode *) t->list.head; n->index.node.next != NULL;
         n = (tablenode *) n->index.node.next)
        if (!__mp_memprotect(&t->heap->memory, n->index.block, n->index.size,
             a))
            return 0;
    return 1;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
