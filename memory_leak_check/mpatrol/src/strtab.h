#ifndef MP_STRTAB_H
#define MP_STRTAB_H


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
 * String tables.  Any strings that require to be preserved by the
 * symbol table module are stored in a string table allocated by this
 * module.
 */


/*
 * $Id$
 */


#include "config.h"
#include "list.h"
#include "heap.h"


/* A hash entry is used to maintain a hash table of strings so that
 * only unique strings are ever stored in the string table.
 */

typedef struct hashentry
{
    listnode node;   /* list node */
    union
    {
        void *block; /* pointer to internal block */
        char *key;   /* pointer to string in table */
    }
    data;
    size_t size;     /* size of internal block or string */
}
hashentry;


/* A strnode belongs to a binary search tree of strnodes, ordered
 * by available space, and contains details of a single allocated block
 * of memory, as well as a pointer to the first available character.
 * A strnode structure actually occupies the first bytes of its
 * block of memory.
 */

typedef struct strnode
{
    treenode node; /* tree node */
    void *block;   /* pointer to block of memory */
    char *next;    /* pointer to first available character */
    size_t avail;  /* available space left in block */
    size_t size;   /* size of block of memory */
}
strnode;


/* A strtab holds the hash table of strings and the tree of strnodes
 * that comprise the string table.  A new strnode is added to the string
 * table if there is not enough space to store a particular string.
 */

typedef struct strtab
{
    heaphead *heap;                  /* pointer to heap */
    slottable table;                 /* table of hash entries */
    listhead slots[MP_HASHTAB_SIZE]; /* array of lists for hash entries */
    listhead list;                   /* internal list of memory blocks */
    treeroot tree;                   /* allocation tree */
    size_t size;                     /* allocation total */
    size_t align;                    /* alignment for each strnode */
    memaccess prot;                  /* protection status */
    size_t protrecur;                /* protection recursion count */
}
strtab;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newstrtab(strtab *, heaphead *);
MP_EXPORT void __mp_deletestrtab(strtab *);
MP_EXPORT char *__mp_addstring(strtab *, char *);
MP_EXPORT int __mp_protectstrtab(strtab *, memaccess);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_STRTAB_H */
