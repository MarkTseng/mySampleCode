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
 * Directed graphs.  Not only are the edges directed, but the overall
 * graph has a beginning and end for the purposes of representing a call
 * graph.
 */


#include "graph.h"


#if MP_IDENT_SUPPORT
#ident "$Id: graph.c,v 1.6 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *graph_id = "$Id: graph.c,v 1.6 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Initialise the fields of a graph head so that the graph becomes empty.
 * Note the two sentinel graph nodes representing the start and end nodes
 * of the graph, which make manipulation and traversal a lot easier.
 */

MP_GLOBAL
void
__mp_newgraph(graphhead *g)
{
    __mp_newlist(&g->start.parents);
    __mp_newlist(&g->start.children);
    __mp_newlist(&g->end.parents);
    __mp_newlist(&g->end.children);
    g->nodes = g->edges = 0;
}


/* Initialise the fields of a graph node and associate it with a given graph.
 */

MP_GLOBAL
void
__mp_addnode(graphhead *g, graphnode *n)
{
    __mp_newlist(&n->parents);
    __mp_newlist(&n->children);
    g->nodes++;
}


/* Remove a graph node from a given graph.
 */

MP_GLOBAL
void
__mp_removenode(graphhead *g, graphnode *n)
{
    graphedge *e;
    listnode *l, *p;

    for (l = n->parents.head; p = l->next; l = p)
    {
        e = (graphedge *) ((char *) l - offsetof(graphedge, pnode));
        __mp_removeedge(g, e);
    }
    for (l = n->children.head; p = l->next; l = p)
    {
        e = (graphedge *) ((char *) l - offsetof(graphedge, cnode));
        __mp_removeedge(g, e);
    }
    g->nodes--;
}


/* Initialise the fields of a graph edge and associate it with parent and
 * child nodes in a given graph.
 */

MP_GLOBAL
void
__mp_addedge(graphhead *g, graphedge *e, graphnode *p, graphnode *c)
{
    __mp_addtail(&c->parents, &e->pnode);
    __mp_addtail(&p->children, &e->cnode);
    e->parent = p;
    e->child = c;
    g->edges++;
}


/* Remove a graph edge from a given graph.
 */

MP_GLOBAL
void
__mp_removeedge(graphhead *g, graphedge *e)
{
    __mp_remove(&e->child->parents, &e->pnode);
    __mp_remove(&e->parent->children, &e->cnode);
    e->parent = e->child = NULL;
    g->edges--;
}


/* Search for a graph edge which joins one graph node to another.
 */

MP_GLOBAL
graphedge *
__mp_findedge(graphhead *g, graphnode *p, graphnode *c)
{
    graphedge *e;
    listnode *n;

    for (n = c->parents.head; n->next != NULL; n = n->next)
    {
        e = (graphedge *) ((char *) n - offsetof(graphedge, pnode));
        if (e->parent == p)
            return e;
    }
    return NULL;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
