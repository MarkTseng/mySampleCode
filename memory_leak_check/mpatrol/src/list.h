#ifndef MP_LIST_H
#define MP_LIST_H


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
 * Doubly-linked lists.  All lists used within the mpatrol library are
 * based on the following data structures and use the same interface for
 * building them.  Only the linkage between list nodes is dealt with by this
 * module - dynamically allocating memory for nodes is done elsewhere.
 */


/*
 * $Id$
 */


#include "config.h"
#include <stddef.h>


/* A list node simply contains linkage information and is intended to be
 * used as a member for any datatypes that need to belong to a list.
 */

typedef struct listnode
{
    struct listnode *next; /* next node in list */
    struct listnode *prev; /* previous node in list */
}
listnode;


/* A list head is effectively two merged list nodes which act as sentinels
 * in order to make list manipulation simpler.
 */

typedef struct listhead
{
    struct listnode *head; /* first node of list */
    struct listnode *tail; /* always NULL */
    struct listnode *tlpr; /* last node of list */
    size_t size;           /* number of nodes in list */
}
listhead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newlist(listhead *);
MP_EXPORT void __mp_addhead(listhead *, listnode *);
MP_EXPORT void __mp_addtail(listhead *, listnode *);
MP_EXPORT void __mp_prepend(listhead *, listnode *, listnode *);
MP_EXPORT void __mp_insert(listhead *, listnode *, listnode *);
MP_EXPORT void __mp_remove(listhead *, listnode *);
MP_EXPORT listnode *__mp_remhead(listhead *);
MP_EXPORT listnode *__mp_remtail(listhead *);
MP_EXPORT void __mp_joinlist(listhead *, listhead *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_LIST_H */
