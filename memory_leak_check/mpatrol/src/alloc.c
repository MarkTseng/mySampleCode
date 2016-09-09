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
 * Memory allocations.  The associated information that will be stored
 * with each memory allocation is dealt with by the allocation information
 * module, and a void pointer is passed in from there so as to hide the
 * details from this module.  Only the efficient implementation of memory
 * allocation, deallocation and reuse is performed here.
 */


#include "alloc.h"
#include "utils.h"


#if MP_IDENT_SUPPORT
#ident "$Id: alloc.c,v 1.18 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *alloc_id = "$Id: alloc.c,v 1.18 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of an allochead so that there are no allocated,
 * freed or free blocks.
 */

MP_GLOBAL
void
__mp_newallocs(allochead *h, size_t m, size_t s, unsigned char o,
               unsigned char a, unsigned char f, unsigned long u)
{
    struct { char x; allocnode y; } z;
    long n;

    __mp_newheap(&h->heap);
    /* Determine the minimum alignment for an allocation node on this
     * system and force the alignment to be a power of two.  This
     * information is used when initialising the slot table.
     */
    n = (char *) &z.y - &z.x;
    __mp_newslots(&h->table, sizeof(allocnode), __mp_poweroftwo(n));
    __mp_newlist(&h->list);
    __mp_newlist(&h->flist);
    __mp_newtree(&h->itree);
    __mp_newtree(&h->atree);
    __mp_newtree(&h->gtree);
    __mp_newtree(&h->ftree);
    h->isize = h->asize = h->gsize = h->fsize = 0;
    h->fmax = m;
    h->oflow = __mp_poweroftwo(s);
    h->obyte = o;
    h->abyte = a;
    h->fbyte = f;
    h->flags = u;
    if (h->flags & FLG_PAGEALLOC)
    {
        if (h->oflow == 0)
            h->oflow = 1;
        h->oflow = __mp_roundup(h->oflow, h->heap.memory.page);
    }
    h->prot = MA_NOACCESS;
    h->protrecur = 0;
}


/* Forget all allocated, freed or free blocks.
 */

MP_GLOBAL
void
__mp_deleteallocs(allochead *h)
{
    /* We don't need to explicitly free any memory as this is dealt with
     * at a lower level by the heap manager.
     */
    __mp_deleteheap(&h->heap);
    h->table.free = NULL;
    h->table.size = 0;
    __mp_newlist(&h->list);
    __mp_newlist(&h->flist);
    __mp_newtree(&h->itree);
    __mp_newtree(&h->atree);
    __mp_newtree(&h->gtree);
    __mp_newtree(&h->ftree);
    h->isize = h->asize = h->gsize = h->fsize = 0;
    h->prot = MA_NOACCESS;
    h->protrecur = 0;
}


/* Allocate a new allocation node.
 */

static
allocnode *
getnode(allochead *h)
{
    allocnode *n;
    heapnode *p;

    /* If we have no more allocation node slots left then we must allocate
     * some more memory for them.  An extra MP_ALLOCFACTOR pages of memory
     * should suffice.
     */
    if ((n = (allocnode *) __mp_getslot(&h->table)) == NULL)
    {
        if ((p = __mp_heapalloc(&h->heap, h->heap.memory.page * MP_ALLOCFACTOR,
              h->table.entalign, 1)) == NULL)
            return NULL;
        __mp_initslots(&h->table, p->block, p->size);
        n = (allocnode *) __mp_getslot(&h->table);
        n->lnode.next = n->lnode.prev = NULL;
        __mp_treeinsert(&h->itree, &n->tnode, (unsigned long) p->block);
        n->block = p->block;
        n->size = p->size;
        n->info = NULL;
        h->isize += p->size;
        n = (allocnode *) __mp_getslot(&h->table);
    }
    return n;
}


/* Split a free node into an allocated node of a certain size and alignment
 * and up to two new free nodes.
 */

static
allocnode *
splitnode(allochead *h, allocnode *n, size_t l, size_t a, void *i)
{
    allocnode *p, *q;
    size_t m, s;

    /* We choose the worst case scenario here and allocate new nodes for
     * both the left and right nodes.  This is so that we can easily recover
     * from lack of system memory at this point rather than rebuild the
     * original free node if we discover that we are out of memory later.
     */
    if (((p = getnode(h)) == NULL) || ((q = getnode(h)) == NULL))
    {
        if (p != NULL)
            __mp_freeslot(&h->table, p);
        return NULL;
    }
    /* Remove the free node from the free tree.
     */
    __mp_treeremove(&h->ftree, &n->tnode);
    h->fsize -= n->size;
    n->block = (char *) n->block + h->oflow;
    n->size -= h->oflow << 1;
    /* Check to see if we have space left over to create a free node to the
     * left of the new node.  This is never done if all allocations are pages.
     */
    if (!(h->flags & FLG_PAGEALLOC) &&
        ((m = __mp_roundup((unsigned long) n->block, a) -
          (unsigned long) n->block) > 0))
    {
        __mp_prepend(&h->list, &n->lnode, &p->lnode);
        __mp_treeinsert(&h->ftree, &p->tnode, m);
        p->block = (char *) n->block - h->oflow;
        p->size = m;
        p->info = NULL;
        n->block = (char *) n->block + m;
        n->size -= m;
        h->fsize += m;
    }
    else
        __mp_freeslot(&h->table, p);
    /* If we are allocating pages then the effective block size is the
     * original size rounded up to a multiple of the system page size.
     */
    if (h->flags & FLG_PAGEALLOC)
        s = __mp_roundup(l, h->heap.memory.page);
    else
        s = l;
    /* Check to see if we have space left over to create a free node to the
     * right of the new node.  This free node will always have a size which is
     * a multiple of the system page size if all allocations are pages.
     */
    if ((m = n->size - s) > 0)
    {
        __mp_insert(&h->list, &n->lnode, &q->lnode);
        __mp_treeinsert(&h->ftree, &q->tnode, m);
        q->block = (char *) n->block + s + h->oflow;
        q->size = m;
        q->info = NULL;
        n->size = s;
        h->fsize += m;
    }
    else
        __mp_freeslot(&h->table, q);
    /* Initialise the details of the newly allocated node and insert it in
     * the allocation tree.
     */
    n->info = i;
    if (h->flags & FLG_PAGEALLOC)
    {
        __mp_memprotect(&h->heap.memory, n->block, n->size, MA_READWRITE);
        /* If we are aligning the end of allocations to the upper end of pages
         * then we may have to shift the start of the block up by a certain
         * number of bytes.  This will then also lead to us having to prefill
         * the unused space with the overflow byte or place a watch point area
         * there.
         */
        if ((h->flags & FLG_ALLOCUPPER) &&
            ((m = __mp_rounddown(n->size - l, a)) > 0))
        {
            if (h->flags & FLG_OFLOWWATCH)
                __mp_memwatch(&h->heap.memory, n->block, m, MA_NOACCESS);
            else
                __mp_memset(n->block, h->obyte, m);
            n->block = (char *) n->block + m;
            n->size -= m;
        }
        /* We may need to prefill any unused space at the end of the block with
         * the overflow byte, or place a watch point area there.
         */
        if ((m = n->size - l) > 0)
            if (h->flags & FLG_OFLOWWATCH)
                __mp_memwatch(&h->heap.memory, (char *) n->block + l, m,
                              MA_NOACCESS);
            else
                __mp_memset((char *) n->block + l, h->obyte, m);
        n->size = l;
    }
    else if (h->flags & FLG_OFLOWWATCH)
    {
        __mp_memwatch(&h->heap.memory, (char *) n->block - h->oflow, h->oflow,
                      MA_NOACCESS);
        __mp_memwatch(&h->heap.memory, (char *) n->block + n->size, h->oflow,
                      MA_NOACCESS);
    }
    else
    {
        __mp_memset((char *) n->block - h->oflow, h->obyte, h->oflow);
        __mp_memset((char *) n->block + n->size, h->obyte, h->oflow);
    }
    __mp_treeinsert(&h->atree, &n->tnode, (unsigned long) n->block);
    h->asize += n->size;
    return n;
}


/* Attempt to merge a free node with any bordering free nodes.
 */

static
allocnode *
mergenode(allochead *h, allocnode *n)
{
    allocnode *l, *r;

    /* See if the left node is free and borders on this node.
     */
    l = (allocnode *) n->lnode.prev;
    if ((l->lnode.prev == NULL) || (l->info != NULL) ||
        ((char *) l->block + l->size < (char *) n->block))
        l = NULL;
    /* See if the right node is free and borders on this node.
     */
    r = (allocnode *) n->lnode.next;
    if ((r->lnode.next == NULL) || (r->info != NULL) ||
        ((char *) n->block + n->size < (char *) r->block))
        r = NULL;
    /* If either or both of the left or right node is suitable for
     * merging then perform the merge.
     */
    if ((l != NULL) || (r != NULL))
    {
        __mp_treeremove(&h->ftree, &n->tnode);
        if (l != NULL)
        {
            __mp_remove(&h->list, &l->lnode);
            __mp_treeremove(&h->ftree, &l->tnode);
            n->block = l->block;
            n->size += l->size;
            __mp_freeslot(&h->table, l);
        }
        if (r != NULL)
        {
            __mp_remove(&h->list, &r->lnode);
            __mp_treeremove(&h->ftree, &r->tnode);
            n->size += r->size;
            __mp_freeslot(&h->table, r);
        }
        __mp_treeinsert(&h->ftree, &n->tnode, n->size);
    }
    return n;
}


/* Create a new allocation node of a specified size and alignment.
 */

MP_GLOBAL
allocnode *
__mp_getalloc(allochead *h, size_t l, size_t a, void *i)
{
    allocnode *n, *r, *s;
    heapnode *p;
    treenode *t;
    size_t b, m;

    b = h->oflow << 1;
    if (l == 0)
        l = 1;
    if (a == 0)
        a = h->heap.memory.align;
    else if (a > h->heap.memory.page)
        a = h->heap.memory.page;
    else
        a = __mp_poweroftwo(a);
    /* If all allocations are not pages then we must add more bytes to the
     * allocation request to account for alignment.
     */
    if (h->flags & FLG_PAGEALLOC)
        m = 0;
    else
        m = a - 1;
    /* If we have no suitable space for this allocation then we must allocate
     * memory via the heap manager.
     */
    if ((t = __mp_searchhigher(h->ftree.root, l + b + m)) == NULL)
    {
        if ((n = getnode(h)) == NULL)
            return NULL;
        /* If all allocations are pages then we must specify that we want our
         * heap allocation to be page-aligned.
         */
        if (h->flags & FLG_PAGEALLOC)
            m = h->heap.memory.page;
        else
            m = a;
        if ((p = __mp_heapalloc(&h->heap,
              __mp_roundup(l + b, h->heap.memory.page), m, 0)) == NULL)
        {
            __mp_freeslot(&h->table, n);
            return NULL;
        }
        /* Initialise the free memory.  If all allocations are pages then we
         * prevent any free memory from being both read from and written to.
         */
        if (h->flags & FLG_PAGEALLOC)
            __mp_memprotect(&h->heap.memory, p->block, p->size, MA_NOACCESS);
        else
            __mp_memset(p->block, h->fbyte, p->size);
        /* Insert the new memory block into the correct position in the
         * memory block list.  This is vital for merging free nodes.
         */
        if ((t = __mp_searchlower(h->atree.root, (unsigned long) p->block)) ||
            (t = __mp_searchlower(h->gtree.root, (unsigned long) p->block)))
            r = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
        else
            r = (allocnode *) &h->list;
        while (((s = (allocnode *) r->lnode.next)->lnode.next != NULL) &&
               (s->block < p->block))
            r = s;
        __mp_insert(&h->list, &r->lnode, &n->lnode);
        __mp_treeinsert(&h->ftree, &n->tnode, p->size);
        n->block = p->block;
        n->size = p->size;
        n->info = NULL;
        h->fsize += p->size;
        /* Merge the memory block with any bordering free nodes.  This
         * is also vital to maintain the property that the memory block
         * list does not ever contain two bordering free nodes.
         */
        n = mergenode(h, n);
    }
    else
        n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
    /* Split the free node as requested.
     */
    return splitnode(h, n, l, a, i);
}


/* Attempt to resize an existing allocation node.
 */

MP_GLOBAL
int
__mp_resizealloc(allochead *h, allocnode *n, size_t l)
{
    allocnode *p;
    size_t m, s;
    long d;

    /* If all allocations are pages and the allocations are to be aligned
     * to the end of a page then the easiest solution is to fail here since
     * the majority of cases would require relocation of the original memory
     * allocation.
     */
    if ((h->flags & FLG_PAGEALLOC) && (h->flags & FLG_ALLOCUPPER))
        return 0;
    if (l == 0)
        l = 1;
    d = l - n->size;
    /* If we are allocating pages then the effective block size is the
     * original size rounded up to a multiple of the system page size.
     */
    if (h->flags & FLG_PAGEALLOC)
        m = __mp_roundup(n->size, h->heap.memory.page);
    else
        m = n->size;
    /* Obtain the bordering free node to the right of this node, if one
     * exists.  There is no need to look any further right as it is
     * guaranteed that it will not be another bordering free node.
     */
    p = (allocnode *) n->lnode.next;
    if ((p->lnode.next == NULL) || (p->info != NULL) ||
        ((char *) n->block + m + h->oflow < (char *) p->block))
        p = NULL;
    if ((h->flags & FLG_PAGEALLOC) && (l <= m) && (l > m - h->heap.memory.page))
    {
        /* There is space in the existing allocated pages to perform the
         * resize without requiring the modification or creation of a
         * neighbouring free node so we remove the watch point area if it
         * exists.
         */
        if (h->flags & FLG_OFLOWWATCH)
            __mp_memwatch(&h->heap.memory, (char *) n->block + n->size,
                          m - n->size, MA_READWRITE);
    }
    else if (d > 0)
    {
        /* If the request was to increase the size of the node and we have no
         * suitable node to merge with or the total size of both nodes is still
         * too small then we just fail.  The relocation to a larger memory
         * allocation is done by the calling function.
         */
        if ((p == NULL) || (m + p->size < l))
            return 0;
        __mp_treeremove(&h->ftree, &p->tnode);
        if (h->flags & FLG_PAGEALLOC)
        {
            s = __mp_roundup(l, h->heap.memory.page) - m;
            /* Remove any memory protection and the watch point area if it
             * exists.
             */
            __mp_memprotect(&h->heap.memory, (char *) p->block - h->oflow, s,
                            MA_READWRITE);
            if (h->flags & FLG_OFLOWWATCH)
                __mp_memwatch(&h->heap.memory, (char *) n->block + n->size,
                              m - n->size, MA_READWRITE);
        }
        else
        {
            s = d;
            /* Remove the right-most watch point area if it exists.
             */
            if (h->flags & FLG_OFLOWWATCH)
                __mp_memwatch(&h->heap.memory, (char *) n->block + m, h->oflow,
                              MA_READWRITE);
        }
        p->block = (char *) p->block + s;
        p->size -= s;
        /* If the resulting size of the free block we merged with is zero then
         * we can just delete it, otherwise we must insert it back into the
         * free tree.
         */
        if (p->size == 0)
        {
            __mp_remove(&h->list, &p->lnode);
            __mp_freeslot(&h->table, p);
        }
        else
            __mp_treeinsert(&h->ftree, &p->tnode, p->size);
        h->fsize -= s;
    }
    else if (d < 0)
    {
        /* If the request was to decrease the size of the node then we
         * must either increase the size of the bordering node, or create
         * a new free node.
         */
        if (p == NULL)
        {
            if ((p = getnode(h)) == NULL)
                return 0;
            __mp_insert(&h->list, &n->lnode, &p->lnode);
            p->block = (char *) n->block + m + h->oflow;
            p->size = 0;
            p->info = NULL;
        }
        else
            __mp_treeremove(&h->ftree, &p->tnode);
        if (h->flags & FLG_PAGEALLOC)
        {
            s = m - __mp_roundup(l, h->heap.memory.page);
            /* Remove the watch point area if it exists.
             */
            if (h->flags & FLG_OFLOWWATCH)
                __mp_memwatch(&h->heap.memory, (char *) n->block + n->size,
                              m - n->size, MA_READWRITE);
        }
        else
        {
            s = -d;
            /* Remove the right-most watch point area if it exists.
             */
            if (h->flags & FLG_OFLOWWATCH)
                __mp_memwatch(&h->heap.memory, (char *) n->block + m, h->oflow,
                              MA_READWRITE);
        }
        p->block = (char *) p->block - s;
        p->size += s;
        if (h->flags & FLG_PAGEALLOC)
            __mp_memprotect(&h->heap.memory, p->block, s, MA_NOACCESS);
        else
            __mp_memset(p->block, h->fbyte, s);
        __mp_treeinsert(&h->ftree, &p->tnode, p->size);
        h->fsize += s;
    }
    if (h->flags & FLG_PAGEALLOC)
    {
        s = __mp_roundup(l, h->heap.memory.page) - l;
        if (h->flags & FLG_OFLOWWATCH)
            __mp_memwatch(&h->heap.memory, (char *) n->block + l, s,
                          MA_NOACCESS);
        else
            __mp_memset((char *) n->block + l, h->obyte, s);
    }
    else if (h->flags & FLG_OFLOWWATCH)
        __mp_memwatch(&h->heap.memory, (char *) n->block + l, h->oflow,
                      MA_NOACCESS);
    else
        __mp_memset((char *) n->block + l, h->obyte, h->oflow);
    n->size = l;
    h->asize += d;
    return 1;
}


/* Free an existing allocation node.
 */

MP_GLOBAL
void
__mp_freealloc(allochead *h, allocnode *n, void *i)
{
    void *p;
    size_t l, s;

    /* If we are keeping the details (and possibly the contents) of a specified
     * number of recently freed memory allocations then we may have to recycle
     * the oldest freed allocation if the length of the queue would extend past
     * the user-specified limit.
     */
    if ((i != NULL) && (h->flist.size != 0) && (h->flist.size == h->fmax))
        __mp_recyclefreed(h);
    /* Remove the allocated node from the allocation tree.
     */
    __mp_treeremove(&h->atree, &n->tnode);
    h->asize -= n->size;
    if (h->flags & FLG_PAGEALLOC)
    {
        p = (void *) __mp_rounddown((unsigned long) n->block,
                                    h->heap.memory.page);
        s = __mp_roundup(n->size + ((char *) n->block - (char *) p),
                         h->heap.memory.page);
        if (h->flags & FLG_OFLOWWATCH)
        {
            /* Remove any watch points within the allocated pages.
             */
            if ((l = (char *) n->block - (char *) p) > 0)
                __mp_memwatch(&h->heap.memory, p, l, MA_READWRITE);
            if ((l = s - n->size - l) > 0)
                __mp_memwatch(&h->heap.memory, (char *) n->block + n->size, l,
                              MA_READWRITE);
        }
    }
    if (i != NULL)
    {
        /* We are keeping this node and so place it on the freed tree.
         * If all allocations are pages then we either prevent the original
         * contents from being both read or written to, or prevent the
         * allocation from being written to.  If not then we may optionally
         * preserve its contents, otherwise it will be filled with the free
         * byte.
         */
        n->info = i;
        if (h->flags & FLG_PAGEALLOC)
            if (h->flags & FLG_PRESERVE)
            {
                __mp_memprotect(&h->heap.memory, n->block, n->size,
                                MA_READONLY);
                if (h->flags & FLG_OFLOWWATCH)
                {
                    /* Replace any watch points within the allocated pages.
                     * We have to do this here because when we change the
                     * memory protection we may trigger a watch point on some
                     * systems.
                     */
                    if ((l = (char *) n->block - (char *) p) > 0)
                        __mp_memwatch(&h->heap.memory, p, l, MA_NOACCESS);
                    if ((l = s - n->size - l) > 0)
                        __mp_memwatch(&h->heap.memory, (char *) n->block +
                                      n->size, l, MA_NOACCESS);
                }
            }
            else
                __mp_memprotect(&h->heap.memory, n->block, n->size,
                                MA_NOACCESS);
        else if (!(h->flags & FLG_PRESERVE))
            __mp_memset(n->block, h->fbyte, n->size);
        __mp_addtail(&h->flist, &n->fnode);
        __mp_treeinsert(&h->gtree, &n->tnode, (unsigned long) n->block);
        h->gsize += n->size;
    }
    else
    {
        /* We are placing this node on the free tree and so it will become
         * available for reuse.  If all allocations are pages then we prevent
         * the contents from being read or written to, otherwise the contents
         * will be filled with the free byte.
         */
        if (h->flags & FLG_PAGEALLOC)
        {
            /* Any watch points will have already been removed, and the
             * surrounding overflow buffers will already be protected with
             * the MA_NOACCESS flag.
             */
            __mp_memprotect(&h->heap.memory, n->block, n->size, MA_NOACCESS);
            n->block = p;
            n->size = s;
        }
        else if (h->flags & FLG_OFLOWWATCH)
        {
            /* Remove any watch points that were made to monitor the overflow
             * buffers.
             */
            __mp_memwatch(&h->heap.memory, (char *) n->block - h->oflow,
                          h->oflow, MA_READWRITE);
            __mp_memwatch(&h->heap.memory, (char *) n->block + n->size,
                          h->oflow, MA_READWRITE);
        }
        n->block = (char *) n->block - h->oflow;
        n->size += h->oflow << 1;
        n->info = NULL;
        if (!(h->flags & FLG_PAGEALLOC))
            __mp_memset(n->block, h->fbyte, n->size);
        __mp_treeinsert(&h->ftree, &n->tnode, n->size);
        h->fsize += n->size;
        mergenode(h, n);
    }
}


/* Recycle a freed allocation node.
 */

MP_GLOBAL
void
__mp_recyclefreed(allochead *h)
{
    allocnode *n;
    void *p;
    size_t l, s;

    n = (allocnode *) ((char *) h->flist.head - offsetof(allocnode, fnode));
    /* Remove the freed node from the freed list and the freed tree.
     */
    __mp_remove(&h->flist, &n->fnode);
    __mp_treeremove(&h->gtree, &n->tnode);
    h->gsize -= n->size;
    if (h->flags & FLG_PAGEALLOC)
    {
        p = (void *) __mp_rounddown((unsigned long) n->block,
                                    h->heap.memory.page);
        s = __mp_roundup(n->size + ((char *) n->block - (char *) p),
                         h->heap.memory.page);
        if (h->flags & FLG_OFLOWWATCH)
        {
            /* Remove any watch points within the allocated pages.
             */
            if ((l = (char *) n->block - (char *) p) > 0)
                __mp_memwatch(&h->heap.memory, p, l, MA_READWRITE);
            if ((l = s - n->size - l) > 0)
                __mp_memwatch(&h->heap.memory, (char *) n->block + n->size, l,
                              MA_READWRITE);
        }
    }
    /* We are placing this node on the free tree and so it will become
     * available for reuse.  If all allocations are pages then we prevent
     * the contents from being read or written to, otherwise the contents
     * will be filled with the free byte.
     */
    if (h->flags & FLG_PAGEALLOC)
    {
        /* Any watch points will have already been removed, and the
         * surrounding overflow buffers will already be protected with
         * the MA_NOACCESS flag.
         */
        __mp_memprotect(&h->heap.memory, n->block, n->size, MA_NOACCESS);
        n->block = p;
        n->size = s;
    }
    else if (h->flags & FLG_OFLOWWATCH)
    {
        /* Remove any watch points that were made to monitor the overflow
         * buffers.
         */
        __mp_memwatch(&h->heap.memory, (char *) n->block - h->oflow, h->oflow,
                      MA_READWRITE);
        __mp_memwatch(&h->heap.memory, (char *) n->block + n->size, h->oflow,
                      MA_READWRITE);
    }
    n->block = (char *) n->block - h->oflow;
    n->size += h->oflow << 1;
    n->info = NULL;
    if (!(h->flags & FLG_PAGEALLOC))
        __mp_memset(n->block, h->fbyte, n->size);
    __mp_treeinsert(&h->ftree, &n->tnode, n->size);
    h->fsize += n->size;
    mergenode(h, n);
}


/* Protect the internal memory blocks used by the allocation manager with the
 * supplied access permission.
 */

MP_GLOBAL
int
__mp_protectalloc(allochead *h, memaccess a)
{
    allocnode *n;
    treenode *t;

    if (!__mp_heapprotect(&h->heap, a))
        return 0;
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
    for (t = __mp_minimum(h->itree.root); t != NULL; t = __mp_successor(t))
    {
        n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
        if (!__mp_memprotect(&h->heap.memory, n->block, n->size, a))
            return 0;
    }
    return 1;
}


/* Search for an allocated node which contains a given address.
 */

MP_GLOBAL
allocnode *
__mp_findalloc(allochead *h, void *p)
{
    allocnode *n;
    treenode *t;

    if (t = __mp_searchlower(h->atree.root, (unsigned long) p))
    {
        n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
        if ((char *) n->block + n->size > (char *) p)
            return n;
    }
    return NULL;
}


/* Search for a freed node which contains a given address.
 */

MP_GLOBAL
allocnode *
__mp_findfreed(allochead *h, void *p)
{
    allocnode *n;
    treenode *t;

    if (t = __mp_searchlower(h->gtree.root, (unsigned long) p))
    {
        n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
        if ((char *) n->block + n->size > (char *) p)
            return n;
    }
    return NULL;
}


/* Search for a node which contains a given address, either within
 * its allocation or as part of an overflow buffer.
 */

MP_GLOBAL
allocnode *
__mp_findnode(allochead *h, void *p, size_t s)
{
    allocnode *n;
    treenode *t;
    void *b;
    size_t l;

    /* Search for the lowest node that is closest to the given address.
     */
    if ((t = __mp_searchlower(h->atree.root, (unsigned long) p)) ||
        (t = __mp_searchlower(h->gtree.root, (unsigned long) p)))
        n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
    else
        n = (allocnode *) h->list.head;
    /* Loop through the list of suitable nodes looking for a likely
     * candidate.
     */
    while (n->lnode.next != NULL)
    {
        if ((h->flags & FLG_PAGEALLOC) && (n->info != NULL))
        {
            b = (void *) __mp_rounddown((unsigned long) n->block,
                                        h->heap.memory.page);
            l = __mp_roundup(n->size + ((char *) n->block - (char *) b),
                             h->heap.memory.page);
        }
        else
        {
            b = n->block;
            l = n->size;
        }
        if (n->info != NULL)
        {
            b = (char *) b - h->oflow;
            l += h->oflow << 1;
        }
        if (p < b)
            if ((char *) p + s > (char *) b)
                return n;
            else
                break;
        else if ((char *) b + l > (char *) p)
            return n;
        n = (allocnode *) n->lnode.next;
    }
    return NULL;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
