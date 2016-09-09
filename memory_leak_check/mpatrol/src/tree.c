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
 * Binary search trees.  This implementation is based upon the red-black
 * tree data structures and algorithms described in Introduction to
 * Algorithms, First Edition by Cormen, Leiserson and Rivest (The MIT Press,
 * 1990, ISBN 0-262-03141-8).
 */


#include "tree.h"


#if MP_IDENT_SUPPORT
#ident "$Id: tree.c,v 1.7 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *tree_id = "$Id: tree.c,v 1.7 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a tree root so that the tree becomes empty.
 * Note the sentinel leaf node which is used to make tree manipulation
 * easier and is pointed to by all real leaf nodes.
 */

MP_GLOBAL
void
__mp_newtree(treeroot *t)
{
    t->root = &t->null;
    t->null.parent = t->null.left = t->null.right = NULL;
    t->null.key = t->null.flag = 0;
    t->size = 0;
}


/* Perform a left rotation of a subtree in order to preserve inorder key
 * ordering.
 */

static
void
rotateleft(treeroot *t, treenode *l)
{
    treenode *r;

    if ((r = l->right) == NULL)
        return;
    if ((l->right = r->left)->left)
        r->left->parent = l;
    if ((r->parent = l->parent) == NULL)
        t->root = r;
    else if (l == l->parent->left)
        l->parent->left = r;
    else
        l->parent->right = r;
    r->left = l;
    l->parent = r;
}


/* Perform a right rotation of a subtree in order to preserve inorder key
 * ordering.
 */

static
void
rotateright(treeroot *t, treenode *r)
{
    treenode *l;

    if ((l = r->left) == NULL)
        return;
    if ((r->left = l->right)->right)
        l->right->parent = r;
    if ((l->parent = r->parent) == NULL)
        t->root = l;
    else if (r == r->parent->left)
        r->parent->left = l;
    else
        r->parent->right = l;
    l->right = r;
    r->parent = l;
}


/* Insert a new tree node with a specific key into a tree, possibly
 * restructuring the tree in order to preserve the red-black property and
 * keep it properly balanced.
 */

MP_GLOBAL
void
__mp_treeinsert(treeroot *t, treenode *n, unsigned long k)
{
    treenode *a, *b;

    if (n == &t->null)
        return;
    a = t->root;
    b = NULL;
    while (a->left)
    {
        b = a;
        if (k < a->key)
            a = a->left;
        else
            a = a->right;
    }
    n->parent = b;
    n->left = &t->null;
    n->right = &t->null;
    n->key = k;
    n->flag = 1;
    if (b == NULL)
        t->root = n;
    else if (k < b->key)
        b->left = n;
    else
        b->right = n;
    while ((n != t->root) && n->parent->flag)
        if (n->parent == n->parent->parent->left)
            if ((a = n->parent->parent->right)->flag)
            {
                a->flag = 0;
                n->parent->flag = 0;
                n = n->parent->parent;
                n->flag = 1;
            }
            else
            {
                if (n == n->parent->right)
                {
                    n = n->parent;
                    rotateleft(t, n);
                }
                n->parent->flag = 0;
                n->parent->parent->flag = 1;
                rotateright(t, n->parent->parent);
            }
        else
            if ((a = n->parent->parent->left)->flag)
            {
                a->flag = 0;
                n->parent->flag = 0;
                n = n->parent->parent;
                n->flag = 1;
            }
            else
            {
                if (n == n->parent->left)
                {
                    n = n->parent;
                    rotateright(t, n);
                }
                n->parent->flag = 0;
                n->parent->parent->flag = 1;
                rotateleft(t, n->parent->parent);
            }
    t->root->flag = 0;
    t->size++;
}


/* Remove an existing tree node from a tree, possibly restructuring the
 * tree in order to preserve the red-black property and keep it properly
 * balanced.
 */

MP_GLOBAL
void
__mp_treeremove(treeroot *t, treenode *n)
{
    treenode *a, *b;
    char f;

    if (n == &t->null)
        return;
    if ((n->left->left == NULL) || (n->right->right == NULL))
        b = n;
    else
        b = __mp_successor(n);
    if (b->left->left)
        a = b->left;
    else
        a = b->right;
    if ((a->parent = b->parent) == NULL)
        t->root = a;
    else if (b == b->parent->left)
        b->parent->left = a;
    else
        b->parent->right = a;
    f = b->flag;
    if (b != n)
    {
        if ((b->parent = n->parent) == NULL)
            t->root = b;
        else if (n == n->parent->left)
            n->parent->left = b;
        else
            n->parent->right = b;
        if ((b->left = n->left)->left)
            n->left->parent = b;
        if ((b->right = n->right)->right)
            n->right->parent = b;
        if (a->parent == n)
            a->parent = b;
        b->flag = n->flag;
    }
    if (f == 0)
    {
        while ((a != t->root) && !a->flag)
            if (a == a->parent->left)
            {
                if ((b = a->parent->right)->flag)
                {
                    b->flag = 0;
                    a->parent->flag = 1;
                    rotateleft(t, a->parent);
                    b = a->parent->right;
                }
                if (!b->left->flag && !b->right->flag)
                {
                    b->flag = 1;
                    a = a->parent;
                }
                else
                {
                    if (!b->right->flag)
                    {
                        b->flag = 1;
                        b->left->flag = 0;
                        rotateright(t, b);
                        b = a->parent->right;
                    }
                    b->flag = a->parent->flag;
                    a->parent->flag = 0;
                    b->right->flag = 0;
                    rotateleft(t, a->parent);
                    a = t->root;
                }
            }
            else
            {
                if ((b = a->parent->left)->flag)
                {
                    b->flag = 0;
                    a->parent->flag = 1;
                    rotateright(t, a->parent);
                    b = a->parent->left;
                }
                if (!b->left->flag && !b->right->flag)
                {
                    b->flag = 1;
                    a = a->parent;
                }
                else
                {
                    if (!b->left->flag)
                    {
                        b->flag = 1;
                        b->right->flag = 0;
                        rotateleft(t, b);
                        b = a->parent->left;
                    }
                    b->flag = a->parent->flag;
                    a->parent->flag = 0;
                    b->left->flag = 0;
                    rotateright(t, a->parent);
                    a = t->root;
                }
            }
        a->flag = 0;
    }
    t->null.parent = NULL;
    t->size--;
}


/* Search a subtree for a node with an exact match for a given key,
 * or return NULL if no such node exists.
 */

MP_GLOBAL
treenode *
__mp_search(treenode *n, unsigned long k)
{
    while (n->left && (k != n->key))
        if (k < n->key)
            n = n->left;
        else
            n = n->right;
    if (n->left)
        return n;
    return NULL;
}


/* Search a subtree for a node with the highest key not greater than the
 * given key, or return NULL if no such node exists.
 */

MP_GLOBAL
treenode *
__mp_searchlower(treenode *n, unsigned long k)
{
    treenode *a;

    a = n;
    while (n->left && (k != n->key))
    {
        a = n;
        if (k < n->key)
            n = n->left;
        else
            n = n->right;
    }
    if (n->left)
        return n;
    if (a->left && (k > a->key))
        return a;
    return __mp_predecessor(a);
}


/* Search a subtree for a node with the lowest key not less than the
 * given key, or return NULL if no such node exists.
 */

MP_GLOBAL
treenode *
__mp_searchhigher(treenode *n, unsigned long k)
{
    treenode *a;

    a = n;
    while (n->right && (k != n->key))
    {
        a = n;
        if (k < n->key)
            n = n->left;
        else
            n = n->right;
    }
    if (n->right)
        return n;
    if (a->right && (k < a->key))
        return a;
    return __mp_successor(a);
}


/* Return the leftmost node of a subtree.
 */

MP_GLOBAL
treenode *
__mp_minimum(treenode *n)
{
    treenode *a;

    if (n->left == NULL)
        return NULL;
    while ((a = n->left)->left)
        n = a;
    return n;
}


/* Return the rightmost node of a subtree.
 */

MP_GLOBAL
treenode *
__mp_maximum(treenode *n)
{
    treenode *a;

    if (n->right == NULL)
        return NULL;
    while ((a = n->right)->right)
        n = a;
    return n;
}


/* Return the predecessor node of a given node, or NULL if no such
 * node exists.
 */

MP_GLOBAL
treenode *
__mp_predecessor(treenode *n)
{
    treenode *a;

    if (n->left == NULL)
        return NULL;
    if (n->left->left)
        return __mp_maximum(n->left);
    a = n->parent;
    while ((a != NULL) && (n == a->left))
    {
        n = a;
        a = a->parent;
    }
    return a;
}


/* Return the successor node of a given node, or NULL if no such
 * node exists.
 */

MP_GLOBAL
treenode *
__mp_successor(treenode *n)
{
    treenode *a;

    if (n->right == NULL)
        return NULL;
    if (n->right->right)
        return __mp_minimum(n->right);
    a = n->parent;
    while ((a != NULL) && (n == a->right))
    {
        n = a;
        a = a->parent;
    }
    return a;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
