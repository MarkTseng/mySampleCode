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
 * String tables.  All memory allocations are performed by the heap
 * manager and all strnodes are stored at the beginning of their
 * respective blocks of memory.  The hash function comes from P. J.
 * Weinberger's C compiler and was published in Compilers: Principles,
 * Techniques and Tools, First Edition by Aho, Sethi and Ullman
 * (Addison-Wesley, 1986, ISBN 0-201-10194-7).
 */


#include "strtab.h"
#include "utils.h"
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *strtab_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a strtab so that the string table becomes empty.
 */

MP_GLOBAL
void
__mp_newstrtab(strtab *t, heaphead *h)
{
    struct { char x; hashentry y; } w;
    struct { char x; strnode y; } z;
    size_t i;
    long n;

    t->heap = h;
    n = (char *) &w.y - &w.x;
    __mp_newslots(&t->table, sizeof(hashentry), __mp_poweroftwo(n));
    for (i = 0; i < MP_HASHTAB_SIZE; i++)
        __mp_newlist(&t->slots[i]);
    __mp_newlist(&t->list);
    __mp_newtree(&t->tree);
    t->size = 0;
    /* Determine the minimum alignment for a strnode on this system
     * and force the alignment to be a power of two.
     */
    n = (char *) &z.y - &z.x;
    t->align = __mp_poweroftwo(n);
    t->prot = MA_NOACCESS;
    t->protrecur = 0;
}


/* Forget all data currently in the string table.
 */

MP_GLOBAL
void
__mp_deletestrtab(strtab *t)
{
    size_t i;

    /* We don't need to explicitly free any memory as this is dealt with
     * at a lower level by the heap manager.
     */
    t->heap = NULL;
    t->table.free = NULL;
    t->table.size = 0;
    for (i = 0; i < MP_HASHTAB_SIZE; i++)
        __mp_newlist(&t->slots[i]);
    __mp_newlist(&t->list);
    __mp_newtree(&t->tree);
    t->size = 0;
    t->prot = MA_NOACCESS;
    t->protrecur = 0;
}


/* Calculate the hash bucket a string should be placed in.
 */

static
unsigned long
hash(char *s)
{
    unsigned long g, h;

    for (h = 0; *s != '\0'; s++)
    {
        h = (h << 4) + *s;
        if (g = h & (0xFUL << ((sizeof(unsigned long) << 3) - 4)))
        {
            h ^= g >> ((sizeof(unsigned long) << 3) - 8);
            h ^= g;
        }
    }
    return h % MP_HASHTAB_SIZE;
}


/* Allocate a new hash entry.
 */

static
hashentry *
gethashentry(strtab *t)
{
    hashentry *e;
    heapnode *p;

    /* If we have no more hash entry slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((e = (hashentry *) __mp_getslot(&t->table)) == NULL)
    {
        if ((p = __mp_heapalloc(t->heap, t->heap->memory.page * MP_ALLOCFACTOR,
              t->table.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&t->table, p->block, p->size);
        e = (hashentry *) __mp_getslot(&t->table);
        __mp_addtail(&t->list, &e->node);
        e->data.block = p->block;
        e->size = p->size;
        t->size += p->size;
        e = (hashentry *) __mp_getslot(&t->table);
    }
    return e;
}


/* Add a new string to the string table.
 */

MP_GLOBAL
char *
__mp_addstring(strtab *t, char *s)
{
    hashentry *e;
    strnode *n;
    heapnode *p;
    char *r;
    size_t k, l, m;

    k = hash(s);
    l = strlen(s) + 1;
    /* Search to see if the string already exists in the hash table and
     * return it if it does.
     */
    for (e = (hashentry *) t->slots[k].head; e->node.next != NULL;
         e = (hashentry *) e->node.next)
        if ((e->size == l) && (strcmp(e->data.key, s) == 0))
            return e->data.key;
    if ((e = gethashentry(t)) == NULL)
        return NULL;
    /* If we have no suitable space left then we must allocate some more
     * memory for the string table in order for it to be able to hold the
     * new string.  The size of the new string is rounded up to a multiple
     * of the system page size and is then multiplied by MP_ALLOCFACTOR.
     */
    if ((n = (strnode *) __mp_searchhigher(t->tree.root, l)) == NULL)
    {
        m = __mp_roundup(sizeof(strnode) + l, t->heap->memory.page);
        if ((p = __mp_heapalloc(t->heap, m * MP_ALLOCFACTOR, t->align, 1)) ==
            NULL)
        {
            __mp_freeslot(&t->table, e);
            return NULL;
        }
        n = (strnode *) p->block;
        n->block = p->block;
        n->next = (char *) p->block + sizeof(strnode);
        n->avail = p->size - sizeof(strnode);
        n->size = p->size;
        t->size += p->size;
    }
    else
        __mp_treeremove(&t->tree, &n->node);
    r = n->next;
    __mp_memcopy(r, s, l);
    n->next += l;
    n->avail -= l;
    /* We have already removed the strnode from the allocation tree since
     * we have altered the number of available characters within it.  We now
     * insert the strnode back into the tree, reflecting its new status.
     */
    __mp_treeinsert(&t->tree, &n->node, n->avail);
    /* We now add the string to the hash table.
     */
    __mp_addhead(&t->slots[k], &e->node);
    e->data.key = r;
    e->size = l;
    return r;
}


/* Protect the memory blocks used by the string table with the
 * supplied access permission.
 */

MP_GLOBAL
int
__mp_protectstrtab(strtab *t, memaccess a)
{
    hashentry *e;
    strnode *n;

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
    for (n = (strnode *) __mp_minimum(t->tree.root); n != NULL;
         n = (strnode *) __mp_successor(&n->node))
        if (!__mp_memprotect(&t->heap->memory, n->block, n->size, a))
            return 0;
    for (e = (hashentry *) t->list.head; e->node.next != NULL;
         e = (hashentry *) e->node.next)
        if (!__mp_memprotect(&t->heap->memory, e->data.block, e->size, a))
            return 0;
    return 1;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
