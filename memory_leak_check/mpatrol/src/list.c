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
 * Doubly-linked lists.  This implementation is based upon that described
 * in the Amiga ROM Kernel Reference Manual: Libraries, Third Edition by
 * Commodore-Amiga, Inc. (Addison-Wesley, 1992, ISBN 0-201-56774-1).
 */


#include "list.h"


#if MP_IDENT_SUPPORT
#ident "$Id: list.c,v 1.7 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *list_id = "$Id: list.c,v 1.7 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a list head so that the list becomes empty.
 * Note the two sentinel list nodes occupying the head and tail fields
 * and the tail and tlpr fields, which make list manipulation a lot
 * easier.
 */

MP_GLOBAL
void
__mp_newlist(listhead *l)
{
    l->head = (listnode *) &l->tail;
    l->tail = NULL;
    l->tlpr = (listnode *) &l->head;
    l->size = 0;
}


/* Add a new node to the beginning of a list.
 */

MP_GLOBAL
void
__mp_addhead(listhead *l, listnode *n)
{
    listnode *t;

    t = l->head;
    l->head = n;
    n->next = t;
    t->prev = n;
    n->prev = (listnode *) &l->head;
    l->size++;
}


/* Add a new node to the end of a list.
 */

MP_GLOBAL
void
__mp_addtail(listhead *l, listnode *n)
{
    listnode *t;

    t = l->tlpr;
    l->tlpr = n;
    n->next = (listnode *) &l->tail;
    t->next = n;
    n->prev = t;
    l->size++;
}


/* Insert a new node before an existing node in a list.
 */

MP_GLOBAL
void
__mp_prepend(listhead *l, listnode *p, listnode *n)
{
    listnode *t;

    t = p->prev;
    p->prev = n;
    n->next = p;
    n->prev = t;
    t->next = n;
    l->size++;
}


/* Insert a new node after an existing node in a list.
 */

MP_GLOBAL
void
__mp_insert(listhead *l, listnode *p, listnode *n)
{
    listnode *t;

    t = p->next;
    p->next = n;
    n->prev = p;
    n->next = t;
    t->prev = n;
    l->size++;
}


/* Remove an existing node from the list it belongs to.
 */

MP_GLOBAL
void
__mp_remove(listhead *l, listnode *n)
{
    n->prev->next = n->next;
    n->next->prev = n->prev;
    l->size--;
}


/* Remove an existing node from the beginning of a list and return it,
 * or NULL if the list is empty.
 */

MP_GLOBAL
listnode *
__mp_remhead(listhead *l)
{
    listnode *t;

    if (l->size == 0)
        return NULL;
    t = l->head;
    l->head = t->next;
    l->head->prev = (listnode *) &l->head;
    l->size--;
    return t;
}


/* Remove an existing node from the end of a list and return it,
 * or NULL if the list is empty.
 */

MP_GLOBAL
listnode *
__mp_remtail(listhead *l)
{
    listnode *t;

    if (l->size == 0)
        return NULL;
    t = l->tlpr;
    l->tlpr = t->prev;
    l->tlpr->next = (listnode *) &l->tail;
    l->size--;
    return t;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
