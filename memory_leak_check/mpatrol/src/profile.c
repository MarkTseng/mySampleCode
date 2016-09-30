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
 * Memory allocation profiling.  The call graphs for every memory allocation
 * and deallocation are recorded here along with their memory usage statistics
 * and are written to a file for later processing by a profiling tool.
 */


#include "profile.h"
#include "info.h"
#include "diag.h"
#include "utils.h"
#include "version.h"
#include <stdio.h>
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *profile_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a profhead so that the mpatrol library
 * is ready to profile memory allocations.
 */

MP_GLOBAL
void
__mp_newprofile(profhead *p, heaphead *h, symhead *s)
{
    struct { char x; profdata y; } w;
    struct { char x; profnode y; } z;
    size_t i;
    long n;

    p->heap = h;
    p->syms = s;
    /* Determine the minimum alignment for a profdata structure and a
     * profnode on this system and force the alignments to be a power
     * of two.  This information is used when initialising the slot
     * tables.
     */
    n = (char *) &w.y - &w.x;
    __mp_newslots(&p->dtable, sizeof(profdata), __mp_poweroftwo(n));
    n = (char *) &z.y - &z.x;
    __mp_newslots(&p->ntable, sizeof(profnode), __mp_poweroftwo(n));
    __mp_newlist(&p->ilist);
    __mp_newlist(&p->list);
    __mp_newtree(&p->tree);
    p->size = 0;
    for (i = 0; i < MP_BIN_SIZE; i++)
        p->acounts[i] = p->dcounts[i] = 0;
    p->atotals = p->dtotals = 0;
    p->sbound = MP_SMALLBOUND;
    p->mbound = MP_MEDIUMBOUND;
    p->lbound = MP_LARGEBOUND;
    p->autosave = p->autocount = 0;
    p->file = __mp_proffile(&h->memory, NULL);
    p->prot = MA_NOACCESS;
    p->protrecur = 0;
    p->profiling = 0;
}


/* Forget all existing profiling information.
 */

MP_GLOBAL
void
__mp_deleteprofile(profhead *p)
{
    size_t i;

    /* We don't need to explicitly free any memory as this is dealt with
     * at a lower level by the heap manager.
     */
    p->heap = NULL;
    p->syms = NULL;
    p->dtable.free = NULL;
    p->dtable.size = 0;
    p->ntable.free = NULL;
    p->ntable.size = 0;
    __mp_newlist(&p->ilist);
    __mp_newlist(&p->list);
    __mp_newtree(&p->tree);
    p->size = 0;
    for (i = 0; i < MP_BIN_SIZE; i++)
        p->acounts[i] = p->dcounts[i] = 0;
    p->atotals = p->dtotals = 0;
    p->autocount = 0;
    p->file = NULL;
    p->prot = MA_NOACCESS;
    p->protrecur = 0;
    p->profiling = 0;
}


/* Allocate a new profiling data structure.
 */

static
profdata *
getprofdata(profhead *p)
{
    profdata *d;
    heapnode *h;
    size_t i;

    /* If we have no more profiling data structure slots left then we must
     * allocate some more memory for them.  An extra MP_ALLOCFACTOR pages
     * of memory should suffice.
     */
    if ((d = (profdata *) __mp_getslot(&p->dtable)) == NULL)
    {
        if ((h = __mp_heapalloc(p->heap, p->heap->memory.page * MP_ALLOCFACTOR,
              p->dtable.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&p->dtable, h->block, h->size);
        d = (profdata *) __mp_getslot(&p->dtable);
        __mp_addtail(&p->ilist, &d->index.node);
        d->index.block = h->block;
        d->index.size = h->size;
        p->size += h->size;
        d = (profdata *) __mp_getslot(&p->dtable);
    }
    __mp_addtail(&p->list, &d->data.node);
    d->data.index = p->list.size;
    for (i = 0; i < 4; i++)
    {
        d->data.acount[i] = d->data.dcount[i] = 0;
        d->data.atotal[i] = d->data.dtotal[i] = 0;
    }
    return d;
}


/* Allocate a new profiling node.
 */

static
profnode *
getprofnode(profhead *p)
{
    profnode *n;
    heapnode *h;

    /* If we have no more profnode slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((n = (profnode *) __mp_getslot(&p->ntable)) == NULL)
    {
        if ((h = __mp_heapalloc(p->heap, p->heap->memory.page * MP_ALLOCFACTOR,
              p->ntable.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&p->ntable, h->block, h->size);
        n = (profnode *) __mp_getslot(&p->ntable);
        __mp_addtail(&p->ilist, &n->index.node);
        n->index.block = h->block;
        n->index.size = h->size;
        p->size += h->size;
        n = (profnode *) __mp_getslot(&p->ntable);
    }
    return n;
}


/* Locate or create a call site associated with a specified return address.
 */

static
profnode *
getcallsite(profhead *p, addrnode *a)
{
    profnode *n, *t;
    addrnode *d;

    if (n = (profnode *) __mp_search(p->tree.root,
        (unsigned long) a->data.addr))
    {
        while ((t = (profnode *) __mp_predecessor(&n->data.node)) &&
               (t->data.addr == a->data.addr))
            n = t;
        while ((n != NULL) && (n->data.addr == a->data.addr))
        {
            for (t = n->data.parent, d = a->data.next;
                 (t != NULL) && (d != NULL);
                 t = t->data.parent, d = d->data.next)
                if (t->data.addr != d->data.addr)
                    break;
            if ((t == NULL) && (d == NULL))
                return n;
            n = (profnode *) __mp_successor(&n->data.node);
        }
    }
    t = NULL;
    if (((n = getprofnode(p)) == NULL) || ((a->data.next != NULL) &&
         ((t = getcallsite(p, a->data.next)) == NULL)))
    {
        if (n != NULL)
            __mp_freeslot(&p->ntable, n);
        return NULL;
    }
    __mp_treeinsert(&p->tree, &n->data.node, (unsigned long) a->data.addr);
    n->data.parent = t;
    n->data.index = p->tree.size;
    n->data.addr = a->data.addr;
    n->data.symbol = __mp_findsymbol(p->syms, a->data.addr);
    n->data.data = NULL;
    return n;
}


/* Record a memory allocation for profiling.
 */

MP_GLOBAL
int
__mp_profilealloc(profhead *p, size_t l, void *d, int w)
{
    profnode *n;
    infonode *m;
    size_t i;

    /* Try to associate the allocation with a previous call site, or create
     * a new call site if no such site exists.  This information is not
     * recorded if the return address could not be determined.
     */
    m = (infonode *) d;
    if ((m->data.stack != NULL) && (m->data.stack->data.addr != NULL))
    {
        if (((n = getcallsite(p, m->data.stack)) == NULL) ||
            ((n->data.data == NULL) &&
             ((n->data.data = getprofdata(p)) == NULL)))
            return 0;
        if (l <= p->sbound)
            i = 0;
        else if (l <= p->mbound)
            i = 1;
        else if (l <= p->lbound)
            i = 2;
        else
            i = 3;
        n->data.data->data.acount[i]++;
        n->data.data->data.atotal[i] += l;
    }
    /* Note the size of the allocation in one of the allocation bins.
     * The highest allocation bin stores a count of all the allocations
     * that are larger than the largest bin.
     */
    if (l < MP_BIN_SIZE)
        p->acounts[l - 1]++;
    else
    {
        p->acounts[MP_BIN_SIZE - 1]++;
        p->atotals += l;
    }
    /* If the autosave feature is enabled then we may need to write out
     * all of the current profiling information to the output file before
     * we can return.
     */
    p->autocount++;
    if ((p->autosave != 0) && (p->autocount % p->autosave == 0))
        __mp_writeprofile(p, w);
    return 1;
}


/* Record a memory deallocation for profiling.
 */

MP_GLOBAL
int
__mp_profilefree(profhead *p, size_t l, void *d, int w)
{
    profnode *n;
    infonode *m;
    size_t i;

    /* Try to associate the deallocation with a previous call site, or create
     * a new call site if no such site exists.  This information is not
     * recorded if the return address could not be determined.
     */
    m = (infonode *) d;
    if ((m->data.stack != NULL) && (m->data.stack->data.addr != NULL))
    {
        if (((n = getcallsite(p, m->data.stack)) == NULL) ||
            ((n->data.data == NULL) &&
             ((n->data.data = getprofdata(p)) == NULL)))
            return 0;
        if (l <= p->sbound)
            i = 0;
        else if (l <= p->mbound)
            i = 1;
        else if (l <= p->lbound)
            i = 2;
        else
            i = 3;
        n->data.data->data.dcount[i]++;
        n->data.data->data.dtotal[i] += l;
    }
    /* Note the size of the deallocation in one of the deallocation bins.
     * The highest deallocation bin stores a count of all the deallocations
     * that are larger than the largest bin.
     */
    if (l < MP_BIN_SIZE)
        p->dcounts[l - 1]++;
    else
    {
        p->dcounts[MP_BIN_SIZE - 1]++;
        p->dtotals += l;
    }
    /* If the autosave feature is enabled then we may need to write out
     * all of the current profiling information to the output file before
     * we can return.
     */
    p->autocount++;
    if ((p->autosave != 0) && (p->autocount % p->autosave == 0))
        __mp_writeprofile(p, w);
    return 1;
}


/* Write the profiling information to the output file.
 */

MP_GLOBAL
int
__mp_writeprofile(profhead *p, int w)
{
    char s[4];
    profdata *d;
    profnode *n;
    FILE *f;
    size_t i, j, l;
    unsigned long v;

    p->autocount = 0;
    /* The profiling file name can also be named as stderr and stdout which
     * will go to the standard error and standard output streams respectively.
     */
    if (p->file == NULL)
        return 0;
    else if (strcmp(p->file, "stderr") == 0)
        f = stderr;
    else if (strcmp(p->file, "stdout") == 0)
        f = stdout;
    else if ((f = fopen(p->file, "wb")) == NULL)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot open file\n", p->file);
        p->file = NULL;
        return 0;
    }
    if (w != 0)
        __mp_protectsymbols(p->syms, MA_READWRITE);
    /* Technically, we should check the return values from each of the calls
     * to fwrite().  However, that would increase the complexity of this
     * function and would make the code extremely hard to follow.  Instead,
     * we just assume that each write to the output file succeeds and hope
     * that if an error does occur then it will not be too drastic if we
     * continue writing the rest of the file.
     */
    i = 1;
    v = MP_VERNUM;
    __mp_memcopy(s, (char *) MP_PROFMAGIC, 4);
    fwrite(s, sizeof(char), 4, f);
    fwrite(&i, sizeof(size_t), 1, f);
    fwrite(&v, sizeof(unsigned long), 1, f);
    fwrite(&p->sbound, sizeof(size_t), 1, f);
    fwrite(&p->mbound, sizeof(size_t), 1, f);
    fwrite(&p->lbound, sizeof(size_t), 1, f);
    /* Write out the allocation and deallocation bins.
     */
    i = MP_BIN_SIZE;
    fwrite(&i, sizeof(size_t), 1, f);
    fwrite(p->acounts, sizeof(size_t), MP_BIN_SIZE, f);
    fwrite(&p->atotals, sizeof(size_t), 1, f);
    fwrite(p->dcounts, sizeof(size_t), MP_BIN_SIZE, f);
    fwrite(&p->dtotals, sizeof(size_t), 1, f);
    /* Write out the profiling data structures.
     */
    fwrite(&p->list.size, sizeof(size_t), 1, f);
    for (d = (profdata *) p->list.head; d->data.node.next != NULL;
         d = (profdata *) d->data.node.next)
    {
        fwrite(&d->data.index, sizeof(unsigned long), 1, f);
        fwrite(d->data.acount, sizeof(size_t), 4, f);
        fwrite(d->data.atotal, sizeof(size_t), 4, f);
        fwrite(d->data.dcount, sizeof(size_t), 4, f);
        fwrite(d->data.dtotal, sizeof(size_t), 4, f);
    }
    /* Write out the statistics from every call site.
     */
    i = j = 0;
    l = 1;
    fwrite(&p->tree.size, sizeof(size_t), 1, f);
    for (n = (profnode *) __mp_minimum(p->tree.root); n != NULL;
         n = (profnode *) __mp_successor(&n->data.node))
    {
        fwrite(&n->data.index, sizeof(unsigned long), 1, f);
        if (n->data.parent != NULL)
            fwrite(&n->data.parent->data.index, sizeof(unsigned long), 1, f);
        else
            fwrite(&i, sizeof(unsigned long), 1, f);
        fwrite(&n->data.addr, sizeof(void *), 1, f);
        if (n->data.symbol != NULL)
        {
            if (n->data.symbol->data.index == 0)
            {
                n->data.symbol->data.index = ++j;
                n->data.symbol->data.offset = l;
                l += strlen(n->data.symbol->data.name) + 1;
            }
            fwrite(&n->data.symbol->data.index, sizeof(unsigned long), 1, f);
            fwrite(&n->data.symbol->data.offset, sizeof(unsigned long), 1, f);
        }
        else
        {
            fwrite(&i, sizeof(unsigned long), 1, f);
            fwrite(&i, sizeof(unsigned long), 1, f);
        }
        if (n->data.data != NULL)
            fwrite(&n->data.data->data.index, sizeof(unsigned long), 1, f);
        else
            fwrite(&i, sizeof(unsigned long), 1, f);
    }
    /* Write out the address of every symbol that has been referenced.
     */
    fwrite(&j, sizeof(size_t), 1, f);
    if (j > 0)
        for (n = (profnode *) __mp_minimum(p->tree.root); n != NULL;
             n = (profnode *) __mp_successor(&n->data.node))
            if ((n->data.symbol != NULL) && (n->data.symbol->data.index != 0))
            {
                n->data.symbol->data.index = 0;
                fwrite(&n->data.symbol->data.addr, sizeof(void *), 1, f);
            }
    /* Write out the name of every symbol that has been referenced.
     */
    fwrite(&l, sizeof(size_t), 1, f);
    fputc('\0', f);
    if (l > 1)
        for (n = (profnode *) __mp_minimum(p->tree.root); n != NULL;
             n = (profnode *) __mp_successor(&n->data.node))
            if ((n->data.symbol != NULL) && (n->data.symbol->data.offset != 0))
            {
                n->data.symbol->data.offset = 0;
                fputs(n->data.symbol->data.name, f);
                fputc('\0', f);
            }
    fwrite(s, sizeof(char), 4, f);
    if (w != 0)
        __mp_protectsymbols(p->syms, MA_READONLY);
    if (ferror(f))
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: problem writing profiling "
                   "file\n", p->file);
        p->file = NULL;
        if ((f != stderr) && (f != stdout))
            fclose(f);
        return 0;
    }
    if ((f != stderr) && (f != stdout) && fclose(f))
        return 0;
    return 1;
}


/* Protect the memory blocks used by the profiling table with the supplied
 * access permission.
 */

MP_GLOBAL
int
__mp_protectprofile(profhead *p, memaccess a)
{
    profnode *n;

    /* The library already knows what its protection status is so we don't
     * need to do anything if the request has already been done.
     */
    if (p->prot == a)
    {
        p->protrecur++;
        return 1;
    }
    else if (p->protrecur > 0)
    {
        p->protrecur--;
        return 1;
    }
    p->prot = a;
    /* Even though there are likely to be profdata and profnode data structures
     * on the list of internal data blocks, it is safe to assume one or the
     * other since they both share the same internal data members.
     */
    for (n = (profnode *) p->ilist.head; n->index.node.next != NULL;
         n = (profnode *) n->index.node.next)
        if (!__mp_memprotect(&p->heap->memory, n->index.block, n->index.size,
             a))
            return 0;
    return 1;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
