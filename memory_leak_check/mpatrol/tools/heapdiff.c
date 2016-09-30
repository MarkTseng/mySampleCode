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
 * Heap difference logger.  Uses mpatrol to log the differences in the
 * heap between two points in a program's execution, which can then be
 * used to help detect localised memory leaks.
 */


#include <stdio.h>
#include "config.h"
#include "heapdiff.h"


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *heapdiff_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


/* The filename template for allocation contents files produced when the
 * HD_CONTENTS flag is used.
 */

#define CONTENTS_FILENAME ".heapdiff"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The number of calls that have been made to __mpt_heapdiffstart(), which is
 * also used to uniquely identify each new heapdiff object.  Reading and
 * modifying this variable is not currently threadsafe but this should not
 * really be an issue unless the ids of two heapdiff objects clash.
 */

static unsigned long count;


/* The number of heapdiff objects currently registered with the HD_CONTENTS
 * flag.  This is incremented at each call to __mpt_heapdiffstart() and is
 * decremented after each call to __mpt_heapdiffend().
 */

static unsigned long total;


/* The previous mpatrol prologue handler.
 */

static __mp_prologuehandler old_prologue;


/* Check to see if a memory allocation has been freed, and if so remove all
 * of its allocation contents files and possibly also call the old prologue
 * function if one was installed.
 */

static
void
prologue(MP_CONST void *p, size_t l, size_t m, MP_CONST char *s,
         MP_CONST char *t, unsigned long u, MP_CONST void *a)
{
    char b[64];
    size_t i;

    if (old_prologue != NULL)
        old_prologue(p, l, m, s, t, u, a);
    if (l == (size_t) -1)
        for (i = 1; i <= count; i++)
        {
            sprintf(b, "%s.%lu", CONTENTS_FILENAME, i);
            __mp_remcontents(b, p);
        }
}


/* The callback function that is called by cleanup() for every unfreed heap
 * allocation that remains at the end of the program.
 */

static
int
cleanupcallback(MP_CONST void *p, void *t)
{
    char b[64];
    size_t i;

    for (i = 1; i <= count; i++)
    {
        sprintf(b, "%s.%lu", CONTENTS_FILENAME, i);
        __mp_remcontents(b, p);
    }
    return 1;
}


/* The callback function that is called by __mpt_heapdiffstart() to write out
 * an allocation contents file for every memory allocation.
 */

static
int
writecallback(MP_CONST void *p, void *t)
{
    char b[64];
    heapdiff *h;
    __mp_allocinfo d;
    int r;

    if (!__mp_info(p, &d))
        return 0;
    h = (heapdiff *) t;
    r = 0;
    if (!d.freed)
    {
        sprintf(b, "%s.%lu", CONTENTS_FILENAME, h->id);
        if (__mp_writecontents(b, p))
            r = 1;
        else
            __mp_printf("error writing contents of allocation " MP_POINTER "\n",
                        p);
    }
    return r;
}


/* The callback function that is called by __mpt_heapdiffend() to compare each
 * allocation's contents with those written to the allocation contents files.
 */

static
int
cmpcallback(MP_CONST void *p, void *t)
{
    char b[64];
    heapdiff *h;
    __mp_allocinfo d;
    long n;

    if (!__mp_info(p, &d))
        return 0;
    h = (heapdiff *) t;
    if (!d.freed)
    {
        sprintf(b, "%s.%lu", CONTENTS_FILENAME, h->id);
        if ((n = __mp_cmpcontents(b, p)) != -1)
        {
            if (n > 0)
            {
                if (h->flags & HD_FULL)
                    __mp_logaddr(d.block);
                if ((h->flags & HD_VIEW) && (d.file != NULL) && (d.line != 0))
                    __mp_view(d.file, d.line);
                h->count++;
                h->total += n;
            }
            __mp_remcontents(b, p);
            return 1;
        }
    }
    return 0;
}


/* The callback function that is called by __mp_iterate() for every heap
 * allocation that has changed since a specified heap event.
 */

static
int
callback(MP_CONST void *p, void *t)
{
    heapdiff *h;
    char *s;
    __mp_allocinfo d;

    if (!__mp_info(p, &d))
        return 0;
    h = (heapdiff *) t;
    if (((d.freed && (h->flags & HD_FREED)) ||
         (!d.freed && (h->flags & HD_UNFREED))) &&
        (!d.marked || (h->flags & HD_MARKED)))
    {
        if (h->flags & HD_FULL)
            __mp_logaddr(d.block);
        else
        {
            if (d.func != NULL)
                s = d.func;
            else if ((d.stack != NULL) && (d.stack->name != NULL))
                s = d.stack->name;
            else
                s = NULL;
            if (s != NULL)
                __mp_printf("%lu byte%s %s in %s at %s line %lu\n", d.size,
                            (d.size == 1) ? "" : "s",
                            d.freed ? "freed" : "allocated", s,
                            d.file ? d.file : "<unknown>", d.line);
            else
                __mp_printf("%lu byte%s %s at %s line %lu\n", d.size,
                            (d.size == 1) ? "" : "s",
                            d.freed ? "freed" : "allocated",
                            d.file ? d.file : "<unknown>", d.line);
        }
        if ((h->flags & HD_VIEW) && (d.file != NULL) && (d.line != 0))
            __mp_view(d.file, d.line);
        h->count++;
        h->total += d.size;
        return 1;
    }
    return 0;
}


/* Iterate through all of the current memory allocations and remove all of
 * their allocation contents files.
 */

static
void
cleanup(void)
{
    if (total != 0)
        __mp_iterate(cleanupcallback, NULL, 0);
}


/* Initialise a heapdiff object and take a snapshot of the current heap state.
 */

void
__mpt_heapdiffstart(heapdiff *h, unsigned long f, MP_CONST char *t,
                    unsigned long u)
{
    h->id = ++count;
    h->event = __mp_snapshot();
    h->flags = f;
    __mp_getoption(MP_OPT_NOFREE, &h->nofree);
    if (f & HD_FREED)
        __mp_setoption(MP_OPT_NOFREE, ~0);
    h->count = h->total = 0;
    if ((t != NULL) && (u != 0))
        __mp_printf("HEAPDIFF %lu STARTING at %s line %lu {\n", h->id, t, u);
    else
        __mp_printf("HEAPDIFF %lu STARTING {\n", h->id);
    __mp_logstack(1);
    if (f & HD_CONTENTS)
    {
        __mp_iterate(writecallback, h, 0);
        if (total++ == 0)
            old_prologue = __mp_prologue(prologue);
    }
    if (count == 1)
        __mp_atexit(cleanup);
}


/* Iterate over the all of the heap allocations, logging any that have changed
 * since the given heapdiff object was initialised, and also logging any useful
 * statistics that can be gathered before reinitialising the heapdiff object.
 */

void
__mpt_heapdiffend(heapdiff *h, MP_CONST char *t, unsigned long u)
{
    unsigned long f;

    if (h->id == 0)
        return;
    if ((h->flags & HD_CONTENTS) && (--total == 0))
        __mp_prologue(old_prologue);
    if ((t != NULL) && (u != 0))
        __mp_printf("} HEAPDIFF %lu ENDING at %s line %lu\n", h->id, t, u);
    else
        __mp_printf("} HEAPDIFF %lu ENDING\n", h->id);
    __mp_logstack(1);
    f = h->flags;
    if (f & HD_CONTENTS)
    {
        h->count = h->total = 0;
        __mp_printf("allocation differences:\n\n");
        __mp_iterate(cmpcallback, h, 0);
        __mp_printf("total differences: %lu (%lu allocation%s)\n\n", h->total,
                    h->count, (h->count == 1) ? "" : "s");
    }
    if (f & HD_FREED)
    {
        h->flags &= ~HD_UNFREED;
        h->count = h->total = 0;
        __mp_printf("freed allocations:\n\n");
        if (__mp_iterate(callback, h, h->event) && !(f & HD_FULL))
            __mp_printf("\n");
        __mp_printf("total freed: %lu (%lu byte%s)\n\n", h->count, h->total,
                    (h->total == 1) ? "" : "s");
        h->flags = f;
    }
    if (f & HD_UNFREED)
    {
        h->flags &= ~HD_FREED;
        h->count = h->total = 0;
        __mp_printf("unfreed allocations:\n\n");
        if (__mp_iterate(callback, h, h->event) && !(f & HD_FULL))
            __mp_printf("\n");
        __mp_printf("total unfreed: %lu (%lu byte%s)\n\n", h->count, h->total,
                    (h->total == 1) ? "" : "s");
        h->flags = f;
    }
    __mp_setoption(MP_OPT_NOFREE, h->nofree);
    h->id = h->event = 0;
    h->flags = h->nofree = 0;
    h->count = h->total = 0;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
