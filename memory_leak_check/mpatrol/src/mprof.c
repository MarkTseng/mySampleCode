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
 * A tool designed to read a profiling output file produced by the mpatrol
 * library and display the profiling information that was obtained.
 */


#include "tree.h"
#include "graph.h"
#include "getopt.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mprof_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#define PROGVERSION "1.3" /* the current version of this program */


/* The flags used to parse the command line options.
 */

typedef enum options_flags
{
    OF_ADDRESSES  = 'a',
    OF_COUNTS     = 'c',
    OF_GRAPHFILE  = 'g',
    OF_HELP       = 'h',
    OF_LEAKS      = 'l',
    OF_STACKDEPTH = 'n',
    OF_VERSION    = 'V',
    OF_CALLGRAPH  = 'v'
}
options_flags;


/* Structure containing statistics about the counts and totals of all of the
 * small, medium, large and extra large allocations and deallocations for a
 * particular call site.
 */

typedef struct profiledata
{
    size_t acount[4]; /* total numbers of allocations */
    size_t dcount[4]; /* total numbers of deallocations */
    size_t atotal[4]; /* total numbers of allocated bytes */
    size_t dtotal[4]; /* total numbers of deallocated bytes */
}
profiledata;


/* Structure containing profiling details for a function in a call stack.
 */

typedef struct profilenode
{
    treenode node;        /* tree node */
    treenode tnode;       /* temporary tree node */
    unsigned long parent; /* parent node */
    void *addr;           /* return address */
    unsigned long symbol; /* associated symbol */
    unsigned long name;   /* associated symbol name */
    unsigned long data;   /* profiling data */
    profiledata tdata;    /* temporary profiling data */
    unsigned char flags;  /* temporary flags */
}
profilenode;


/* Structure representing a vertex in the allocation call graph.
 */

typedef struct vertex
{
    treenode node;       /* tree node */
    graphnode gnode;     /* graph node */
    profilenode *pnode;  /* profiling node */
    unsigned long index; /* vertex index */
}
vertex;


/* Structure representing an edge in the allocation call graph.
 */

typedef struct edge
{
    listnode node;       /* list node */
    graphedge gnode;     /* edge node */
    profiledata data;    /* profiling data */
    unsigned char flags; /* temporary flags */
}
edge;


/* The version of the mpatrol library which produced the profiling output file.
 */

static unsigned long version;


/* The total number of allocations and deallocations.
 */

static size_t acount, dcount;


/* The total bytes of allocations and deallocations.
 */

static size_t atotal, dtotal;


/* The allocation and deallocation bins.
 */

static size_t *acounts, *dcounts;


/* The total bytes of large allocations and deallocations.
 */

static size_t atotals, dtotals;


/* The number of allocation bins.
 */

static size_t binsize;


/* The allocations and deallocations for all call sites.
 */

static profiledata *data;


/* The number of profiledata structures for all call sites.
 */

static size_t datasize;


/* The profiling details for all call sites.
 */

static profilenode *nodes;


/* The number of profilenode structures for all call sites.
 */

static size_t nodesize;


/* The array containing the symbol addresses.
 */

static void **addrs;


/* The string table containing the symbol names.
 */

static char *symbols;


/* The small, medium and large allocation boundaries.
 */

static size_t sbound, mbound, lbound;


/* The tree containing profiling details for all call sites.
 */

static treeroot proftree;


/* The tree containing temporary profiling details for all call sites.
 */

static treeroot temptree;


/* The list of edges in the allocation call graph.
 */

static listhead edgelist;


/* The allocation call graph.
 */

static graphhead graph;


/* The profiling output file produced by mpatrol.
 */

static FILE *proffile;


/* The graph specification file optionally produced by mprof.
 */

static FILE *graphfile;


/* The filename used to invoke this tool.
 */

static char *progname;


/* Indicates if different allocation points within single functions should
 * be noted when displaying the profiling tables.
 */

static int useaddresses;


/* Indicates if the emphasis will be on allocation counts rather than
 * allocated bytes when displaying the profiling tables.
 */

static int showcounts;


/* Indicates if memory leaks rather than memory allocations should be written
 * to the graph specification file.
 */

static int showleaks;


/* Indicates the maximum stack depth to use when comparing function call
 * stacks for the memory leak table.
 */

static unsigned long maxstack;


/* Indicates if the allocation call graph should be displayed.
 */

static int showgraph;


/* The table describing all recognised options.
 */

static option options_table[] =
{
    {"addresses", OF_ADDRESSES, NULL,
     "\tSpecifies that different call sites from within the same function\n"
     "\tare to be differentiated and that the names of all functions should\n"
     "\tbe displayed with their call site offset in bytes.\n"},
    {"call-graph", OF_CALLGRAPH, NULL,
     "\tSpecifies that the allocation call graph should be displayed.\n"},
    {"counts", OF_COUNTS, NULL,
     "\tSpecifies that certain tables should be sorted by the number of\n"
     "\tallocations or deallocations rather than the total number of bytes\n"
     "\tallocated or deallocated.\n"},
    {"graph-file", OF_GRAPHFILE, "file",
     "\tSpecifies that the allocation call graph should also be written to a\n"
     "\tgraph specification file for later visualisation with dot.\n"},
    {"help", OF_HELP, NULL,
     "\tDisplays this quick-reference option summary.\n"},
    {"leaks", OF_LEAKS, NULL,
     "\tSpecifies that memory leaks rather than memory allocations are to be\n"
     "\twritten to the graph specification file.\n"},
    {"stack-depth", OF_STACKDEPTH, "depth",
     "\tSpecifies the maximum stack depth to display and also use when\n"
     "\tcalculating if one call site has the same call stack as another call\n"
     "\tsite.\n"},
    {"version", OF_VERSION, NULL,
     "\tDisplays the version number of this program.\n"},
    NULL
};


/* Clear the statistics for a set of profiling data.
 */

static
void
cleardata(profiledata *a)
{
    size_t i;

    for (i = 0; i < 4; i++)
    {
        a->acount[i] = 0;
        a->dcount[i] = 0;
        a->atotal[i] = 0;
        a->dtotal[i] = 0;
    }
}


/* Sum the statistics from two sets of profiling data.
 */

static
void
sumdata(profiledata *a, profiledata *b)
{
    size_t i;

    for (i = 0; i < 4; i++)
    {
        a->acount[i] += b->acount[i];
        a->dcount[i] += b->dcount[i];
        a->atotal[i] += b->atotal[i];
        a->dtotal[i] += b->dtotal[i];
    }
}


/* Compare two function call stacks.
 */

static
int
comparestack(profilenode *n, profilenode *p, int c)
{
    size_t i;

    for (i = 1; (c != 0) || (maxstack == 0) || (i < maxstack); i++)
    {
        if ((n->parent == 0) || (p->parent == 0))
            return ((n->parent == 0) && (p->parent == 0));
        n = &nodes[n->parent - 1];
        p = &nodes[p->parent - 1];
        if ((n->addr != p->addr) && (useaddresses || (n->symbol == 0) ||
             (n->symbol != p->symbol)))
            return 0;
    }
    return 1;
}


/* Byte-swap a block of memory.
 */

static
void
byteswap(void *b, size_t n)
{
    char *s, *t;
    char c;

    s = (char *) b;
    t = (char *) b + n - 1;
    while (s < t)
    {
        c = *s;
        *s++ = *t;
        *t-- = c;
    }
}


/* Read an entry from the profiling output file.
 */

static
void
getentry(void *d, size_t l, size_t n, int b)
{
    size_t i;

    if (fread(d, l, n, proffile) != n)
    {
        fprintf(stderr, "%s: Error reading file\n", progname);
        exit(EXIT_FAILURE);
    }
    /* Byte-swap all of the elements if necessary.
     */
    if (b != 0)
        for (i = 0; i < n; i++)
        {
            byteswap(d, l);
            d = (char *) d + l;
        }
}


/* Read all of the data from the profiling output file.
 */

static
void
readfile(void)
{
    char s[4];
    profiledata *d;
    profilenode *p;
    size_t i;
    unsigned long n;
    int b;

    /* When reading the profiling output file, we assume that if it begins and
     * ends with the magic sequence of characters then it is a valid profiling
     * output file from the mpatrol library.  There are probably an infinite
     * number of checks we could do to ensure that the rest of the data in the
     * file is valid, but that would be overcomplicated and probably slow this
     * program down.  However, if the file is only partially written then the
     * getentry() function will catch the error before we do something silly.
     */
    getentry(s, sizeof(char), 4, 0);
    if (memcmp(s, MP_PROFMAGIC, 4) != 0)
    {
        fprintf(stderr, "%s: Invalid file format\n", progname);
        exit(EXIT_FAILURE);
    }
    /* The following test allows us to read profiling output files that were
     * produced on a different processor architecture.  If the next word in the
     * file does not contain the value 1 then we have to byte-swap any further
     * data that we read from the file.  Note that this test only works if the
     * word size is the same on both machines.
     */
    getentry(&i, sizeof(size_t), 1, 0);
    b = (i != 1);
    /* Get the version number of the mpatrol library which produced the
     * profiling output file.  The profiling file format changed to include the
     * version number at mpatrol 1.3.0 so we can't reliably read files produced
     * before then.  We also assume that we can't read files produced by later
     * versions of mpatrol.
     */
    getentry(&version, sizeof(unsigned long), 1, b);
    if (version < 10300)
    {
        fprintf(stderr, "%s: Profiling file version too old\n", progname);
        exit(EXIT_FAILURE);
    }
    else if (version / 100 > MP_VERNUM / 100)
    {
        fprintf(stderr, "%s: Profiling file version too new\n", progname);
        exit(EXIT_FAILURE);
    }
    getentry(&sbound, sizeof(size_t), 1, b);
    getentry(&mbound, sizeof(size_t), 1, b);
    getentry(&lbound, sizeof(size_t), 1, b);
    /* Read the allocation and deallocation bins.
     */
    getentry(&binsize, sizeof(size_t), 1, b);
    if (binsize > 0)
    {
        if (((acounts = (size_t *) malloc(binsize * sizeof(size_t))) == NULL) ||
            ((dcounts = (size_t *) malloc(binsize * sizeof(size_t))) == NULL))
        {
            fprintf(stderr, "%s: Out of memory\n", progname);
            exit(EXIT_FAILURE);
        }
        getentry(acounts, sizeof(size_t), binsize, b);
        getentry(&atotals, sizeof(size_t), 1, b);
        getentry(dcounts, sizeof(size_t), binsize, b);
        getentry(&dtotals, sizeof(size_t), 1, b);
        for (i = 0; i < binsize; i++)
        {
            acount += acounts[i];
            dcount += dcounts[i];
            if (i == binsize - 1)
            {
                atotal += atotals;
                dtotal += dtotals;
            }
            else
            {
                atotal += acounts[i] * (i + 1);
                dtotal += dcounts[i] * (i + 1);
            }
        }
    }
    /* Read the profiling data structures.
     */
    getentry(&datasize, sizeof(size_t), 1, b);
    if (datasize > 0)
    {
        if ((data = (profiledata *) malloc(datasize * sizeof(profiledata))) ==
            NULL)
        {
            fprintf(stderr, "%s: Out of memory\n", progname);
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < datasize; i++)
        {
            getentry(&n, sizeof(unsigned long), 1, b);
            d = &data[n - 1];
            getentry(d->acount, sizeof(size_t), 4, b);
            getentry(d->atotal, sizeof(size_t), 4, b);
            getentry(d->dcount, sizeof(size_t), 4, b);
            getentry(d->dtotal, sizeof(size_t), 4, b);
        }
    }
    /* Read the statistics for every call site.
     */
    getentry(&nodesize, sizeof(size_t), 1, b);
    if (nodesize > 0)
    {
        if ((nodes = (profilenode *) malloc(nodesize * sizeof(profilenode))) ==
            NULL)
        {
            fprintf(stderr, "%s: Out of memory\n", progname);
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < nodesize; i++)
        {
            getentry(&n, sizeof(unsigned long), 1, b);
            p = &nodes[n - 1];
            getentry(&p->parent, sizeof(unsigned long), 1, b);
            getentry(&p->addr, sizeof(void *), 1, b);
            getentry(&p->symbol, sizeof(unsigned long), 1, b);
            getentry(&p->name, sizeof(unsigned long), 1, b);
            getentry(&p->data, sizeof(unsigned long), 1, b);
            __mp_treeinsert(&proftree, &p->node, (unsigned long) p->addr);
            cleardata(&p->tdata);
            p->flags = 0;
        }
    }
    /* Read the table containing the symbol addresses.
     */
    getentry(&i, sizeof(size_t), 1, b);
    if (i > 0)
    {
        if ((addrs = (void **) malloc(i * sizeof(void *))) == NULL)
        {
            fprintf(stderr, "%s: Out of memory\n", progname);
            exit(EXIT_FAILURE);
        }
        getentry(addrs, sizeof(void *), i, b);
    }
    /* Read the string table containing the symbol names.
     */
    getentry(&i, sizeof(size_t), 1, b);
    if (i > 0)
    {
        if ((symbols = (char *) malloc(i * sizeof(char))) == NULL)
        {
            fprintf(stderr, "%s: Out of memory\n", progname);
            exit(EXIT_FAILURE);
        }
        getentry(symbols, sizeof(char), i, 0);
    }
    getentry(s, sizeof(char), 4, 0);
    if (memcmp(s, MP_PROFMAGIC, 4) != 0)
    {
        fprintf(stderr, "%s: Invalid file format\n", progname);
        exit(EXIT_FAILURE);
    }
}


/* Count the number of decimal digits in a number.
 */

static
unsigned char
countdigits(unsigned long n)
{
    unsigned char d;

    for (d = 1; n /= 10; d++);
    return d;
}


/* Display a character a specified number of times.
 */

static
void
printchar(char c, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        fputc(c, stdout);
}


/* Display a set of profiling data.
 */

static
void
printdata(size_t *d, size_t t)
{
    size_t i;
    double n;

    for (i = 0; i < 4; i++)
        if ((d[i] == 0) || (t == 0))
            fputs("   ", stdout);
        else
        {
            n = ((double) d[i] / (double) t) * 100.0;
            if (n <= 0.5)
                fputs("  .", stdout);
            else if (n >= 99.5)
                fputs(" %%", stdout);
            else
                fprintf(stdout, " %2.0f", n);
        }
}


/* Display the symbol associated with a particular call site.
 */

static
void
printsymbol(FILE *f, profilenode *n)
{
    ptrdiff_t o;

    if (n->name == 0)
        fprintf(f, MP_POINTER, n->addr);
    else
    {
        fputs(symbols + n->name, f);
        if (useaddresses && (n->symbol != 0) &&
            ((o = (char *) n->addr - (char *) addrs[n->symbol - 1]) != 0))
            fprintf(f, "%+ld", o);
    }
}


/* Add a new vertex to the allocation call graph.
 */

static
vertex *
addvertex(profilenode *n)
{
    vertex *v;

    if ((v = (vertex *) malloc(sizeof(vertex))) == NULL)
    {
        fprintf(stderr, "%s: Out of memory\n", progname);
        exit(EXIT_FAILURE);
    }
    if (useaddresses || (n->symbol == 0))
        __mp_treeinsert(&temptree, &v->node, (unsigned long) n->addr);
    else
        __mp_treeinsert(&temptree, &v->node, n->symbol);
    __mp_addnode(&graph, &v->gnode);
    v->pnode = n;
    v->index = 0;
    return v;
}


/* Add a new edge to the allocation call graph.
 */

static
edge *
addedge(graphnode *p, graphnode *c)
{
    edge *e;

    if ((e = (edge *) malloc(sizeof(edge))) == NULL)
    {
        fprintf(stderr, "%s: Out of memory\n", progname);
        exit(EXIT_FAILURE);
    }
    __mp_addtail(&edgelist, &e->node);
    __mp_addedge(&graph, &e->gnode, p, c);
    cleardata(&e->data);
    e->flags = 0;
    return e;
}


/* Build the allocation call graph.
 */

static
void
buildgraph(void)
{
    profilenode *n, *p;
    vertex *u, *v;
    edge *e;
    graphedge *g;
    profiledata d;

    for (n = (profilenode *) __mp_minimum(proftree.root); n != NULL;
         n = (profilenode *) __mp_successor(&n->node))
        if ((n->data != 0) && !(n->flags & 1))
        {
            cleardata(&d);
            sumdata(&d, &data[n->data - 1]);
            p = (profilenode *) __mp_successor(&n->node);
            while ((p != NULL) && ((p->addr == n->addr) || (!useaddresses &&
                     (p->symbol != 0) && (p->symbol == n->symbol))))
            {
                if ((p->data != 0) && !(p->flags & 1) && comparestack(n, p, 1))
                {
                    sumdata(&d, &data[p->data - 1]);
                    p->flags |= 1;
                }
                p = (profilenode *) __mp_successor(&p->node);
            }
            p = n;
            if (useaddresses || (p->symbol == 0))
                u = (vertex *) __mp_search(temptree.root,
                                           (unsigned long) p->addr);
            else
                u = (vertex *) __mp_search(temptree.root, p->symbol);
            if (u == NULL)
                u = addvertex(p);
            if ((g = __mp_findedge(&graph, &u->gnode, &graph.end)) == NULL)
                e = addedge(&u->gnode, &graph.end);
            else
                e = (edge *) ((char *) g - offsetof(edge, gnode));
            sumdata(&e->data, &d);
            u->pnode->flags |= 2;
            for (v = u; p->parent != 0; u = v)
            {
                p = &nodes[p->parent - 1];
                if (useaddresses || (p->symbol == 0))
                    v = (vertex *) __mp_search(temptree.root,
                                               (unsigned long) p->addr);
                else
                    v = (vertex *) __mp_search(temptree.root, p->symbol);
                if (v == NULL)
                    v = addvertex(p);
                if ((g = __mp_findedge(&graph, &v->gnode, &u->gnode)) == NULL)
                    e = addedge(&v->gnode, &u->gnode);
                else
                    e = (edge *) ((char *) g - offsetof(edge, gnode));
                /* Find out if we've been here before.  If we have then we have
                 * detected a cycle in the call graph and we should not
                 * contribute anything to the current edge since we have done so
                 * already.  However, we mark the edge as being part of a cycle
                 * since it is useful to know this later on.
                 */
                if (v->pnode->flags & 2)
                    e->flags |= 2;
                else
                {
                    sumdata(&e->data, &d);
                    v->pnode->flags |= 2;
                }
            }
            if ((g = __mp_findedge(&graph, &graph.start, &v->gnode)) == NULL)
                e = addedge(&graph.start, &v->gnode);
            else
                e = (edge *) ((char *) g - offsetof(edge, gnode));
            sumdata(&e->data, &d);
            /* Clear all of the flags from the current call stack that we used
             * to determine cycles in the call graph.
             */
            p = n;
            do
            {
                if (useaddresses || (p->symbol == 0))
                    v = (vertex *) __mp_search(temptree.root,
                                               (unsigned long) p->addr);
                else
                    v = (vertex *) __mp_search(temptree.root, p->symbol);
                v->pnode->flags &= ~2;
                if (p->parent != 0)
                    p = &nodes[p->parent - 1];
                else
                    p = NULL;
            }
            while (p != NULL);
        }
    for (n = (profilenode *) __mp_minimum(proftree.root); n != NULL;
         n = (profilenode *) __mp_successor(&n->node))
        n->flags = 0;
}


/* Delete the allocation call graph.
 */

static
void
deletegraph(void)
{
    vertex *n, *v;
    edge *e, *m;

    /* Remove the nodes from the graph and free up the memory that they
     * inhabit.  This is done by traversing the tree rather than the graph
     * in order to avoid deleting nodes that result in other nodes being
     * inaccessible.
     */
    for (v = (vertex *) __mp_minimum(temptree.root); v != NULL; v = n)
    {
        n = (vertex *) __mp_successor(&v->node);
        __mp_treeremove(&temptree, &v->node);
        __mp_removenode(&graph, &v->gnode);
        free(v);
    }
    /* Remove the edges from the graph.  The graph is now empty following
     * the removal of the nodes in the previous loop, so we just need to
     * free the memory that each edge uses.
     */
    for (e = (edge *) edgelist.head; m = (edge *) e->node.next; e = m)
    {
        __mp_remove(&edgelist, &e->node);
        free(e);
    }
}


/* Display the allocation bin table.
 */

static
void
bintable(void)
{
    size_t i;
    unsigned long a, b, c, d;
    double e, f, g, h;
    int p;

    p = 0;
    printchar(' ', 32);
    fputs("ALLOCATION BINS\n\n", stdout);
    printchar(' ', 29);
    fprintf(stdout, "(number of bins: %lu)\n\n", binsize);
    printchar(' ', 21);
    fputs("allocated", stdout);
    printchar(' ', 26);
    fputs("unfreed\n", stdout);
    printchar(' ', 10);
    printchar('-', 32);
    fputs("  ", stdout);
    printchar('-', 32);
    fputs("\n    size   count       %     bytes       %   "
          "count       %     bytes       %\n\n", stdout);
    for (i = 0; i < binsize; i++)
        if (acounts[i] != 0)
        {
            a = acounts[i];
            b = a - dcounts[i];
            if (i == binsize - 1)
            {
                c = atotals;
                d = c - dtotals;
            }
            else
            {
                c = a * (i + 1);
                d = b * (i + 1);
            }
            e = ((double) a / (double) acount) * 100.0;
            if (acount != dcount)
                f = ((double) b / (double) (acount - dcount)) * 100.0;
            else
                f = 0.0;
            g = ((double) c / (double) atotal) * 100.0;
            if (atotal != dtotal)
                h = ((double) d / (double) (atotal - dtotal)) * 100.0;
            else
                h = 0.0;
            fprintf(stdout, " %s %4lu  %6lu  %6.2f  %8lu  %6.2f  "
                    "%6lu  %6.2f  %8lu  %6.2f\n",
                    (i == binsize - 1) ? ">=" : "  ",
                    i + 1, a, e, c, g, b, f, d, h);
            p = 1;
        }
    if (p == 1)
        fputc('\n', stdout);
    fprintf(stdout, "   total  %6lu          %8lu          "
            "%6lu          %8lu\n", acount, atotal,
            acount - dcount, atotal - dtotal);
}


/* Display the direct allocation table.
 */

static
void
directtable(void)
{
    profiledata *d;
    profilenode *n, *p;
    treenode *t;
    profiledata m;
    size_t i;
    unsigned long a, b, c;
    double e, f;

    cleardata(&m);
    printchar(' ', 31);
    fputs("DIRECT ALLOCATIONS\n\n", stdout);
    printchar(' ', 20);
    fprintf(stdout, "(0 < s <= %lu < m <= %lu < l <= %lu < x)\n\n",
            sbound, mbound, lbound);
    if (showcounts)
    {
        printchar(' ', 9);
        fputs("allocated", stdout);
        printchar(' ', 21);
        fputs("unfreed\n", stdout);
        printchar('-', 27);
        fputs("  ", stdout);
        printchar('-', 27);
        fputs("\n count       %   s  m  l  x   "
              "count       %   s  m  l  x     bytes  function\n\n", stdout);
    }
    else
    {
        printchar(' ', 10);
        fputs("allocated", stdout);
        printchar(' ', 23);
        fputs("unfreed\n", stdout);
        printchar('-', 29);
        fputs("  ", stdout);
        printchar('-', 29);
        fputs("\n   bytes       %   s  m  l  x     "
              "bytes       %   s  m  l  x   count  function\n\n", stdout);
    }
    for (n = (profilenode *) __mp_minimum(proftree.root); n != NULL; n = p)
    {
        p = (profilenode *) __mp_successor(&n->node);
        if (n->data != 0)
        {
            d = &n->tdata;
            sumdata(d, &data[n->data - 1]);
            while ((p != NULL) && ((p->addr == n->addr) || (!useaddresses &&
                     (p->symbol != 0) && (p->symbol == n->symbol))))
            {
                if (p->data != 0)
                    sumdata(d, &data[p->data - 1]);
                p = (profilenode *) __mp_successor(&p->node);
            }
            a = 0;
            for (i = 0; i < 4; i++)
                if (showcounts)
                    a += d->acount[i];
                else
                    a += d->atotal[i];
            __mp_treeinsert(&temptree, &n->tnode, a);
            sumdata(&m, d);
        }
    }
    for (t = __mp_maximum(temptree.root); t != NULL; t = __mp_predecessor(t))
    {
        n = (profilenode *) ((char *) t - offsetof(profilenode, tnode));
        d = &n->tdata;
        a = t->key;
        b = c = 0;
        for (i = 0; i < 4; i++)
        {
            if (showcounts)
            {
                b += d->dcount[i];
                c += d->atotal[i];
            }
            else
            {
                b += d->dtotal[i];
                c += d->acount[i];
            }
            d->dcount[i] = d->acount[i] - d->dcount[i];
            d->dtotal[i] = d->atotal[i] - d->dtotal[i];
        }
        b = a - b;
        if (showcounts)
        {
            e = ((double) a / (double) acount) * 100.0;
            if (acount != dcount)
                f = ((double) b / (double) (acount - dcount)) * 100.0;
            else
                f = 0.0;
            fprintf(stdout, "%6lu  %6.2f ", a, e);
            printdata(d->acount, acount);
            fprintf(stdout, "  %6lu  %6.2f ", b, f);
            printdata(d->dcount, acount - dcount);
            fprintf(stdout, "  %8lu  ", c);
        }
        else
        {
            e = ((double) a / (double) atotal) * 100.0;
            if (atotal != dtotal)
                f = ((double) b / (double) (atotal - dtotal)) * 100.0;
            else
                f = 0.0;
            fprintf(stdout, "%8lu  %6.2f ", a, e);
            printdata(d->atotal, atotal);
            fprintf(stdout, "  %8lu  %6.2f ", b, f);
            printdata(d->dtotal, atotal - dtotal);
            fprintf(stdout, "  %6lu  ", c);
        }
        printsymbol(stdout, n);
        fputc('\n', stdout);
        cleardata(d);
    }
    for (i = 0; i < 4; i++)
    {
        m.dcount[i] = m.acount[i] - m.dcount[i];
        m.dtotal[i] = m.atotal[i] - m.dtotal[i];
    }
    if (temptree.size != 0)
        fputc('\n', stdout);
    if (showcounts)
    {
        fprintf(stdout, "%6lu         ", acount);
        printdata(m.acount, acount);
        fprintf(stdout, "  %6lu         ", acount - dcount);
        printdata(m.dcount, acount - dcount);
        fprintf(stdout, "  %8lu  total\n", atotal);
    }
    else
    {
        fprintf(stdout, "%8lu         ", atotal);
        printdata(m.atotal, atotal);
        fprintf(stdout, "  %8lu         ", atotal - dtotal);
        printdata(m.dtotal, atotal - dtotal);
        fprintf(stdout, "  %6lu  total\n", acount);
    }
    __mp_newtree(&temptree);
}


/* Display the memory leak table.
 */

static
void
leaktable(void)
{
    profiledata *d;
    profilenode *n, *p;
    treenode *t;
    size_t i;
    unsigned long a, b, j, k;
    double e, f, g;

    printchar(' ', 34);
    fputs("MEMORY LEAKS\n\n", stdout);
    printchar(' ', 28);
    fprintf(stdout, "(maximum stack depth: %lu)\n\n", maxstack);
    printchar(' ', 16);
    fputs("unfreed", stdout);
    printchar(' ', 22);
    fputs("allocated\n", stdout);
    printchar('-', 40);
    fputs("  ", stdout);
    printchar('-', 16);
    if (showcounts)
        fputs("\n     %   count       %     bytes       %   "
              "count     bytes  function\n\n", stdout);
    else
        fputs("\n     %     bytes       %   count       %     "
              "bytes   count  function\n\n", stdout);
    for (n = (profilenode *) __mp_minimum(proftree.root); n != NULL; n = p)
    {
        p = (profilenode *) __mp_successor(&n->node);
        if ((n->data != 0) && !n->flags)
        {
            d = &n->tdata;
            sumdata(d, &data[n->data - 1]);
            while ((p != NULL) && ((p->addr == n->addr) || (!useaddresses &&
                     (p->symbol != 0) && (p->symbol == n->symbol))))
            {
                if ((p->data != 0) && !p->flags && comparestack(n, p, 0))
                {
                    sumdata(d, &data[p->data - 1]);
                    p->flags = 1;
                }
                p = (profilenode *) __mp_successor(&p->node);
            }
            p = (profilenode *) __mp_successor(&n->node);
            a = 0;
            for (i = 0; i < 4; i++)
                if (showcounts)
                    a += d->acount[i] - d->dcount[i];
                else
                    a += d->atotal[i] - d->dtotal[i];
            if (a > 0)
                __mp_treeinsert(&temptree, &n->tnode, a);
        }
    }
    for (n = (profilenode *) __mp_minimum(proftree.root); n != NULL;
         n = (profilenode *) __mp_successor(&n->node))
        n->flags = 0;
    for (t = __mp_maximum(temptree.root); t != NULL; t = __mp_predecessor(t))
    {
        n = (profilenode *) ((char *) t - offsetof(profilenode, tnode));
        d = &n->tdata;
        a = t->key;
        b = j = k = 0;
        for (i = 0; i < 4; i++)
            if (showcounts)
            {
                b += d->dtotal[i];
                j += d->acount[i];
                k += d->atotal[i];
            }
            else
            {
                b += d->dcount[i];
                j += d->atotal[i];
                k += d->acount[i];
            }
        b = k - b;
        e = ((double) a / (double) j) * 100.0;
        f = ((double) b / (double) k) * 100.0;
        if (showcounts)
        {
            g = ((double) a / (double) (acount - dcount)) * 100.0;
            fprintf(stdout, "%6.2f  %6lu  %6.2f  %8lu  %6.2f  %6lu  %8lu  ",
                    g, a, e, b, f, j, k);
        }
        else
        {
            g = ((double) a / (double) (atotal - dtotal)) * 100.0;
            fprintf(stdout, "%6.2f  %8lu  %6.2f  %6lu  %6.2f  %8lu  %6lu  ",
                    g, a, e, b, f, j, k);
        }
        printsymbol(stdout, n);
        fputc('\n', stdout);
        p = n;
        for (i = 1; (maxstack == 0) || (i < maxstack); i++)
        {
            if (p->parent == 0)
                break;
            p = &nodes[p->parent - 1];
            printchar(' ', 60);
            printsymbol(stdout, p);
            fputc('\n', stdout);
        }
        cleardata(d);
    }
    if (acount != 0)
        e = ((double) (acount - dcount) / (double) acount) * 100.0;
    else
        e = 0.0;
    if (atotal != 0)
        f = ((double) (atotal - dtotal) / (double) atotal) * 100.0;
    else
        f = 0.0;
    if (temptree.size != 0)
        fputc('\n', stdout);
    if (showcounts)
        fprintf(stdout, "        %6lu  %6.2f  %8lu  %6.2f  %6lu  %8lu  total\n",
                acount - dcount, e, atotal - dtotal, f, acount, atotal);
    else
        fprintf(stdout, "        %8lu  %6.2f  %6lu  %6.2f  %8lu  %6lu  total\n",
                atotal - dtotal, f, acount - dcount, e, atotal, acount);
    __mp_newtree(&temptree);
}


/* Display the allocation call graph.
 */

static
void
callgraph(void)
{
    size_t d[4];
    listnode *l;
    vertex *u, *v;
    edge *e;
    graphedge *g;
    size_t i, s, t;

    i = 1;
    printchar(' ', 29);
    fputs("ALLOCATION CALL GRAPH\n\n", stdout);
    printchar(' ', 28);
    fprintf(stdout, "(number of vertices: %lu)\n\n", temptree.size);
    if (showcounts)
    {
        printchar(' ', 10);
        fputs("allocated", stdout);
        printchar(' ', 13);
        fputs("unfreed\n", stdout);
        printchar(' ', 5);
        printchar('-', 19);
        fputs("  ", stdout);
        printchar('-', 19);
        fputs("\nindex count   s  m  l  x   count   s  m  l  x  function\n",
              stdout);
    }
    else
    {
        printchar(' ', 11);
        fputs("allocated", stdout);
        printchar(' ', 15);
        fputs("unfreed\n", stdout);
        printchar(' ', 5);
        printchar('-', 21);
        fputs("  ", stdout);
        printchar('-', 21);
        fputs("\nindex   bytes   s  m  l  x     bytes   s  m  l  x  function\n",
              stdout);
    }
    /* Compute the index for each vertex in the graph.  This is currently
     * done after the graph has been completely constructed so that the
     * ordering can be done by code address.  It has to be done before we
     * start displaying the graph since each vertex will contain forward
     * references to its parents and children.
     */
    for (v = (vertex *) __mp_minimum(temptree.root); v != NULL;
         v = (vertex *) __mp_successor(&v->node))
        v->index = i++;
    for (v = (vertex *) __mp_minimum(temptree.root); v != NULL;
         v = (vertex *) __mp_successor(&v->node))
    {
        if (showcounts)
            printchar('-', 45);
        else
            printchar('-', 49);
        fputc('\n', stdout);
        /* Calculate the details of the parents.
         */
        for (l = v->gnode.parents.head; l->next != NULL; l = l->next)
        {
            /* The following three lines are a bit excessive and result from
             * using class-based programming in C.  These conversions would be
             * automatically calculated by the compiler if we were using C++
             * classes.
             */
            g = (graphedge *) ((char *) l - offsetof(graphedge, pnode));
            e = (edge *) ((char *) g - offsetof(edge, gnode));
            u = (vertex *) ((char *) e->gnode.parent - offsetof(vertex, gnode));
            if (&u->gnode != &graph.start)
            {
                for (i = s = t = 0; i < 4; i++)
                {
                    if (showcounts)
                    {
                        t += e->data.acount[i];
                        d[i] = e->data.acount[i] - e->data.dcount[i];
                    }
                    else
                    {
                        t += e->data.atotal[i];
                        d[i] = e->data.atotal[i] - e->data.dtotal[i];
                    }
                    s += d[i];
                }
                if (e->flags & 2)
                    fputs(" (*) ", stdout);
                else
                    printchar(' ', 5);
                if (showcounts)
                {
                    fprintf(stdout, "%6lu ", t);
                    printdata(e->data.acount, t);
                    fprintf(stdout, "  %6lu ", s);
                }
                else
                {
                    fprintf(stdout, "%8lu ", t);
                    printdata(e->data.atotal, t);
                    fprintf(stdout, "  %8lu ", s);
                }
                printdata(d, s);
                printchar(' ', 6);
                printsymbol(stdout, u->pnode);
                fprintf(stdout, " [%lu]\n", u->index);
            }
        }
        fprintf(stdout, "[%lu] ", v->index);
        if (showcounts)
            printchar(' ', 44 - countdigits(v->index));
        else
            printchar(' ', 48 - countdigits(v->index));
        printsymbol(stdout, v->pnode);
        fprintf(stdout, " [%lu]\n", v->index);
        /* Calculate the details of the children.
         */
        for (l = v->gnode.children.head; l->next != NULL; l = l->next)
        {
            /* The following three lines are a bit excessive and result from
             * using class-based programming in C.  These conversions would be
             * automatically calculated by the compiler if we were using C++
             * classes.
             */
            g = (graphedge *) ((char *) l - offsetof(graphedge, cnode));
            e = (edge *) ((char *) g - offsetof(edge, gnode));
            u = (vertex *) ((char *) e->gnode.child - offsetof(vertex, gnode));
            if (&u->gnode != &graph.end)
            {
                for (i = s = t = 0; i < 4; i++)
                {
                    if (showcounts)
                    {
                        t += e->data.acount[i];
                        d[i] = e->data.acount[i] - e->data.dcount[i];
                    }
                    else
                    {
                        t += e->data.atotal[i];
                        d[i] = e->data.atotal[i] - e->data.dtotal[i];
                    }
                    s += d[i];
                }
                if (e->flags & 2)
                    fputs(" (*) ", stdout);
                else
                    printchar(' ', 5);
                if (showcounts)
                {
                    fprintf(stdout, "%6lu ", t);
                    printdata(e->data.acount, t);
                    fprintf(stdout, "  %6lu ", s);
                }
                else
                {
                    fprintf(stdout, "%8lu ", t);
                    printdata(e->data.atotal, t);
                    fprintf(stdout, "  %8lu ", s);
                }
                printdata(d, s);
                printchar(' ', 6);
                printsymbol(stdout, u->pnode);
                fprintf(stdout, " [%lu]\n", u->index);
            }
        }
    }
}


/* Write out the graph specification file in dot format.
 */

static
void
writegraph(profilenode *p, graphnode *n)
{
    listnode *l;
    vertex *v;
    edge *e;
    graphedge *g;
    size_t i, t;

    for (l = n->children.head; l->next != NULL; l = l->next)
    {
        /* The following three lines are a bit excessive and result from
         * using class-based programming in C.  These conversions would be
         * automatically calculated by the compiler if we were using C++
         * classes.
         */
        g = (graphedge *) ((char *) l - offsetof(graphedge, cnode));
        e = (edge *) ((char *) g - offsetof(edge, gnode));
        v = (vertex *) ((char *) e->gnode.child - offsetof(vertex, gnode));
        if (!(e->flags & 1))
        {
            e->flags |= 1;
            for (i = t = 0; i < 4; i++)
                if (showcounts)
                {
                    t += e->data.acount[i];
                    if (showleaks)
                        t -= e->data.dcount[i];
                }
                else
                {
                    t += e->data.atotal[i];
                    if (showleaks)
                        t -= e->data.dtotal[i];
                }
            if ((t > 0) || !showleaks)
            {
                fputs("    \"", graphfile);
                if (p == NULL)
                    fputs("START", graphfile);
                else
                    printsymbol(graphfile, p);
                fputs("\" -> \"", graphfile);
                if (&v->gnode == &graph.end)
                    fputs("ALLOC", graphfile);
                else
                    printsymbol(graphfile, v->pnode);
                fprintf(graphfile, "\" [label = \"%lu\"", t);
                if (e->flags & 2)
                    fputs(", style = dotted", graphfile);
                fputs("];\n", graphfile);
            }
            writegraph(v->pnode, &v->gnode);
        }
    }
    if (p == NULL)
        for (e = (edge *) edgelist.head; e->node.next != NULL;
             e = (edge *) e->node.next)
            e->flags &= ~1;
}


/* Read the profiling output file and display all specified information.
 */

int
main(int argc, char **argv)
{
    char b[256];
    char *f, *g;
    int c, e, h, r, v;

    g = NULL;
    e = h = v = 0;
    r = EXIT_SUCCESS;
    maxstack = 1;
    progname = __mp_basename(argv[0]);
    while ((c = __mp_getopt(argc, argv, __mp_shortopts(b, options_table),
             options_table)) != EOF)
        switch (c)
        {
          case OF_ADDRESSES:
            useaddresses = 1;
            break;
          case OF_CALLGRAPH:
            showgraph = 1;
            break;
          case OF_COUNTS:
            showcounts = 1;
            break;
          case OF_GRAPHFILE:
            g = __mp_optarg;
            break;
          case OF_HELP:
            h = 1;
            break;
          case OF_LEAKS:
            showleaks = 1;
            break;
          case OF_STACKDEPTH:
            if (!__mp_getnum(progname, __mp_optarg, (long *) &maxstack, 1))
                e = 1;
            break;
          case OF_VERSION:
            v = 1;
            break;
          default:
            e = 1;
            break;
        }
    argc -= __mp_optindex;
    argv += __mp_optindex;
    if (v == 1)
    {
        fprintf(stdout, "%s %s\n%s %s\n\n", progname, PROGVERSION,
                __mp_copyright, __mp_author);
        fputs("This is free software, and you are welcome to redistribute it "
              "under certain\n", stdout);
        fputs("conditions; see the GNU Lesser General Public License for "
              "details.\n\n", stdout);
        fputs("For the latest mpatrol release and documentation,\n", stdout);
        fprintf(stdout, "visit %s.\n\n", __mp_homepage);
    }
    if (argc > 1)
        e = 1;
    if ((e == 1) || (h == 1))
    {
        fprintf(stdout, "Usage: %s [options] [file]\n\n", progname);
        if (h == 0)
            fprintf(stdout, "Type `%s --help' for a complete list of "
                    "options.\n", progname);
        else
            __mp_showopts(options_table);
        if (e == 1)
            exit(EXIT_FAILURE);
        exit(EXIT_SUCCESS);
    }
    if (argc == 1)
        f = argv[0];
    else
        f = MP_PROFFILE;
    acount = dcount = 0;
    atotal = dtotal = 0;
    acounts = dcounts = NULL;
    atotals = dtotals = 0;
    binsize = 0;
    data = NULL;
    datasize = 0;
    nodes = NULL;
    nodesize = 0;
    addrs = NULL;
    symbols = NULL;
    sbound = mbound = lbound = 0;
    __mp_newtree(&proftree);
    __mp_newtree(&temptree);
    __mp_newlist(&edgelist);
    __mp_newgraph(&graph);
    if (strcmp(f, "-") == 0)
        proffile = stdin;
    else if ((proffile = fopen(f, "rb")) == NULL)
    {
        fprintf(stderr, "%s: Cannot open file `%s'\n", progname, f);
        exit(EXIT_FAILURE);
    }
    readfile();
    fclose(proffile);
    bintable();
    fputs("\n\n", stdout);
    directtable();
    fputs("\n\n", stdout);
    leaktable();
    /* The reason that the allocation call graph is not used for the direct
     * allocation and memory leak tables is that the code to build and display
     * the allocation call graph was added much later.  Rather than convert
     * these tables to use the new call graph, I decided to keep the code that
     * already worked and only use the call graph for any new tables.
     */
    buildgraph();
    if (showgraph)
    {
        fputs("\n\n", stdout);
        callgraph();
    }
    if (g != NULL)
    {
        if (strcmp(g, "stdout") == 0)
            graphfile = stdout;
        else if (strcmp(g, "stderr") == 0)
            graphfile = stderr;
        else if ((graphfile = fopen(g, "w")) == NULL)
        {
            fprintf(stderr, "%s: Cannot open file `%s'\n", progname, g);
            r = EXIT_FAILURE;
        }
        if (r == EXIT_SUCCESS)
        {
            fprintf(graphfile, "/* produced by %s %s from %s */\n\n", progname,
                    PROGVERSION, f);
            if (showleaks)
                fputs("digraph \"memory leak call graph\"\n{\n", graphfile);
            else
                fputs("digraph \"allocation call graph\"\n{\n", graphfile);
            writegraph(NULL, &graph.start);
            fputs("}\n", graphfile);
            if ((graphfile != stdout) && (graphfile != stderr))
                fclose(graphfile);
        }
    }
    deletegraph();
    if (acounts != NULL)
        free(acounts);
    if (dcounts != NULL)
        free(dcounts);
    if (data != NULL)
        free(data);
    if (nodes != NULL)
        free(nodes);
    if (addrs != NULL)
        free(addrs);
    if (symbols != NULL)
        free(symbols);
    return r;
}
