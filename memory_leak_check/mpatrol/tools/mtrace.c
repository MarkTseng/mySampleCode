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
 * GNU-compatible memory allocation tracing.  Implements mtrace() and
 * muntrace() using mpatrol and produces trace files compatible with
 * those produced by the GNU C library.
 */


#include "config.h"
#include "mtrace.h"
#include <stdio.h>


#if MP_IDENT_SUPPORT
#ident "$Id: mtrace.c,v 1.10 2002/01/08 20:05:10 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mtrace_id = "$Id: mtrace.c,v 1.10 2002/01/08 20:05:10 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The previous mpatrol prologue and epilogue handlers.
 */

static __mp_prologuehandler old_prologue;
static __mp_epiloguehandler old_epilogue;


/* The file where all tracing output goes.
 */

static FILE *trace_file;


/* Indicates whether the call to __mpt_muntrace() has been planted with
 * __mp_atexit().
 */

static int trace_atexit;


/* The pointer and size obtained each time our prologue function is called.
 * This is then used by our epilogue function, but we don't need to worry
 * about nested calls to the prologue function since the mpatrol library
 * guarantees that it will never occur, even when there are multiple threads.
 */

static void *alloc_pointer;
static size_t alloc_size;


/* Log where an allocation or reallocation was made, or where a deallocation
 * was performed.  We can also interface with the mpatrol symbol manager to
 * obtain the name of the function that made the call if that is not already
 * available, and perhaps even the filename and line number if USEDEBUG is used
 * and supported.
 */

static
void
location(MP_CONST char *s, MP_CONST char *t, unsigned long u, MP_CONST void *a)
{
    __mp_symbolinfo i;
    long o;

    if ((t != NULL) && (u != 0))
    {
        fprintf(trace_file, "@ %s:%lu ", t, u);
        return;
    }
    if (a == NULL)
        return;
    fputs("@ ", trace_file);
    if (__mp_syminfo(a, &i))
        if ((i.file != NULL) && (i.line != 0))
        {
            fprintf(trace_file, "%s:%lu ", i.file, i.line);
            return;
        }
        else if ((s != NULL) || (i.name != NULL))
        {
            if (i.object != NULL)
                fprintf(trace_file, "%s:", i.object);
            fprintf(trace_file, "(%s", (s != NULL) ? s : i.name);
            o = (char *) a - (char *) i.addr;
            if (o > 0)
                fprintf(trace_file, "+%#lx", o);
            else if (o < 0)
                fprintf(trace_file, "-%#lx", o);
            fputc(')', trace_file);
        }
    fprintf(trace_file, "[%p] ", a);
}


/* Record the pointer and size for later logging and possibly also call the
 * old prologue function if one was installed.
 */

static
void
prologue(MP_CONST void *p, size_t l, size_t m, MP_CONST char *s,
         MP_CONST char *t, unsigned long u, MP_CONST void *a)
{
    if (old_prologue != NULL)
        old_prologue(p, l, m, s, t, u, a);
    alloc_pointer = (void *) p;
    alloc_size = l;
}


/* Log the resulting pointer and size and possibly also call the old epilogue
 * function if one was installed.
 */

static
void
epilogue(MP_CONST void *p, MP_CONST char *s, MP_CONST char *t, unsigned long u,
         MP_CONST void *a)
{
    size_t l;

    location(s, t, u, a);
    if (alloc_pointer == (void *) -1)
        fprintf(trace_file, "+ %p %#lx\n", p, alloc_size);
    else if (alloc_size == (size_t) -1)
        fprintf(trace_file, "- %p\n", alloc_pointer);
    else if (alloc_size == (size_t) -2)
    {
        if (alloc_pointer == NULL)
            l = 0;
        else
            l = strlen((char *) alloc_pointer) + 1;
        fprintf(trace_file, "+ %p %#lx\n", p, l);
    }
    else if (alloc_pointer == NULL)
        fprintf(trace_file, "+ %p %#lx\n", p, alloc_size);
    else if (alloc_size == 0)
        fprintf(trace_file, "- %p\n", alloc_pointer);
    else if (p == NULL)
        fprintf(trace_file, "! %p %#lx\n", alloc_pointer, alloc_size);
    else
    {
        fprintf(trace_file, "< %p\n", alloc_pointer);
        location(s, t, u, a);
        fprintf(trace_file, "> %p %#lx\n", p, alloc_size);
    }
    if (old_epilogue != NULL)
        old_epilogue(p, s, t, u, a);
}


/* Open the trace file and start tracing.
 */

void
__mpt_mtrace(void)
{
    char *f;

    if (trace_file != NULL)
        return;
    if ((f = getenv("MALLOC_TRACE")) && (*f != '\0') &&
        (trace_file = fopen(f, "w")))
    {
        if (!trace_atexit)
        {
            __mp_atexit(__mpt_muntrace);
            trace_atexit = 1;
        }
        fputs("= Start\n", trace_file);
        old_prologue = __mp_prologue(prologue);
        old_epilogue = __mp_epilogue(epilogue);
    }
}


/* Finish tracing and close the trace file.
 */

void
__mpt_muntrace(void)
{
    if (trace_file == NULL)
        return;
    __mp_prologue(old_prologue);
    __mp_epilogue(old_epilogue);
    fputs("= End\n", trace_file);
    fclose(trace_file);
    trace_file = NULL;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
