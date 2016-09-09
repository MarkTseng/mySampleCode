#ifndef MP_TREE_H
#define MP_TREE_H


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
 * Binary search trees.  All tree structures used within the mpatrol
 * library are based on the following data structures and use the same
 * interface for building and traversing them.  Only the linkage between
 * tree nodes is dealt with by this module - dynamically allocating
 * memory for nodes is done elsewhere.
 */


/*
 * $Id: tree.h,v 1.6 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include <stddef.h>


/* A tree node simply contains linkage information and is intended to be
 * used as a member for any datatypes that need to belong to a tree.
 */

typedef struct treenode
{
    struct treenode *parent; /* parent node in tree */
    struct treenode *left;   /* left child node in tree */
    struct treenode *right;  /* right child node in tree */
    unsigned long key;       /* search key */
    char flag;               /* node flags */
}
treenode;


/* A tree root contains a pointer to the topmost node of the tree and
 * also contains a sentinel leaf node in order to make tree manipulation
 * simpler.
 */

typedef struct treeroot
{
    struct treenode *root; /* topmost node of tree */
    struct treenode null;  /* leaf node for tree */
    size_t size;           /* number of nodes in tree */
}
treeroot;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newtree(treeroot *);
MP_EXPORT void __mp_treeinsert(treeroot *, treenode *, unsigned long);
MP_EXPORT void __mp_treeremove(treeroot *, treenode *);
MP_EXPORT treenode *__mp_search(treenode *, unsigned long);
MP_EXPORT treenode *__mp_searchlower(treenode *, unsigned long);
MP_EXPORT treenode *__mp_searchhigher(treenode *, unsigned long);
MP_EXPORT treenode *__mp_minimum(treenode *);
MP_EXPORT treenode *__mp_maximum(treenode *);
MP_EXPORT treenode *__mp_predecessor(treenode *);
MP_EXPORT treenode *__mp_successor(treenode *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_TREE_H */
