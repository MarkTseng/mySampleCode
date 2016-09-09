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
 * Library interface.  This module defines the visible interface for the
 * mpatrol library.  It also defines replacement functions for the GNU
 * Checker tool, which adds code to check all reads from and writes to
 * memory when the user's code is compiled with the -fcheck-memory-usage
 * option in the GNU compiler.
 */


#include "inter.h"
#include "diag.h"
#include "machine.h"
#if MP_THREADS_SUPPORT
#include "mutex.h"
#endif /* MP_THREADS_SUPPORT */
#if (TARGET == TARGET_AMIGA && defined(__GNUC__)) || \
    (TARGET == TARGET_WINDOWS && !defined(__GNUC__))
#include "sbrk.h"
#endif /* TARGET && __GNUC__ */
#include "option.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#if TARGET == TARGET_UNIX
#include <unistd.h>
#endif /* TARGET */


#if MP_IDENT_SUPPORT
#ident "$Id: inter.c,v 1.156 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *inter_id = "$Id: inter.c,v 1.156 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if MP_INUSE_SUPPORT
int _Inuse_init(unsigned long, int);
void _Inuse_close(void);
#endif /* MP_INUSE_SUPPORT */


/* The memory header structure used by the library to hold all of its data
 * and settings.
 */

static infohead memhead;


#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_LINUX
/* This contains a pointer to the environment variables for a process.  If
 * it is not set up yet then we must use sbrk() to allocate all memory since
 * we can't initialise mpatrol until the environment variable can be read.
 */

extern char **__environ;
#elif SYSTEM == SYSTEM_TRU64
/* The exception support library on Tru64 always allocates some memory from
 * the heap in order to initialise the code address range tables.  We need
 * to ensure that we don't initialise mpatrol before this otherwise stack
 * unwinding will fail, so we check to ensure that the table has been set up
 * before we proceed.  We also need to ensure that we don't try to unwind the
 * stack before the mpatrol library is initialised so we use a flag for this.
 */

static int init_flag;
extern void *__exc_crd_list_head;
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS && !defined(__GNUC__)
/* These are global variables used by the Microsoft C run-time library to
 * indicate initialisation of the environment variables, the exit function
 * table and the streams buffers respectively.  The run-time library calls
 * malloc() and related functions to set these up so we cannot initialise
 * the mpatrol library until all of these have been set up.  Instead, we
 * will call sbrk() to allocate enough memory for these as they appear, but
 * we cannot record anything about these allocations.
 */

extern int __env_initialized;
extern void *__onexitbegin;
extern void **__piob;
#endif /* TARGET && __GNUC__ */


/* Determine if the C run-time library is initialised.
 */

#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_LINUX
#define crt_initialised() (__environ)
#elif SYSTEM == SYSTEM_TRU64
#define crt_initialised() (__exc_crd_list_head && init_flag)
#else /* SYSTEM */
#define crt_initialised() (1)
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
#ifndef __GNUC__
#define crt_initialised() (__env_initialized && __onexitbegin && __piob)
#else /* __GNUC__ */
#define crt_initialised() (1)
#endif /* __GNUC__ */
#endif /* TARGET */


#if MP_INIT_SUPPORT
/* On systems with .init and .fini sections we can plant calls to initialise
 * the mpatrol mutexes and data structures before main() is called and also
 * to terminate the mpatrol library after exit() is called.  However, we need
 * to refer to a symbol within the machine module so that we can drag in the
 * contents of the .init and .fini sections when the mpatrol library is built
 * as an archive library.
 */

static int *initsection = &__mp_initsection;
#elif SYSTEM == SYSTEM_TRU64
/* Tru64 has an interesting feature in that any global functions whose names
 * begin with __init_* and __fini_* will be called before and after main()
 * respectively.  Note that we don't define __INIT_* and __FINI_* functions
 * since they are reserved for the system.
 */

void
__init_mpatrol(void)
{
#if MP_THREADS_SUPPORT
    __mp_initmutexes();
#endif /* MP_THREADS_SUPPORT */
    __mp_init();
    init_flag = 1;
}

#if !MP_USE_ATEXIT
void
__fini_mpatrol(void)
{
    __mp_fini();
}
#endif /* MP_USE_ATEXIT */
#elif defined(__GNUC__)
/* The GNU C compiler allows us to indicate that a function is a constructor
 * which should be called before main() or that a function is a destructor
 * that should be called at or after exit().  However, these get entered into
 * the .ctors and .dtors sections which means that the final link must also
 * be performed by the GNU C compiler.
 */

static void initlibrary(void) __attribute__((__constructor__));

static
void
initlibrary(void)
{
#if MP_THREADS_SUPPORT
    __mp_initmutexes();
#endif /* MP_THREADS_SUPPORT */
    __mp_init();
}

#if !MP_USE_ATEXIT
static void finilibrary(void) __attribute__((__destructor__));

static
void
finilibrary(void)
{
    __mp_fini();
}
#endif /* MP_USE_ATEXIT */
#elif defined(__cplusplus)
/* C++ provides a way to initialise the mpatrol library before main() is
 * called and terminate it after exit() is called.  Note that if the C++
 * compiler uses a special section for calling functions before main() that
 * is not recognised by the system tools then the C++ compiler must also be
 * used to perform the final link.
 */

static struct initlibrary
{
    initlibrary()
    {
#if MP_THREADS_SUPPORT
        __mp_initmutexes();
#endif /* MP_THREADS_SUPPORT */
        __mp_init();
    }
#if !MP_USE_ATEXIT
    ~initlibrary()
    {
        __mp_fini();
    }
#endif /* MP_USE_ATEXIT */
}
meminit;
#endif /* __cplusplus */


/* Save the current signal handlers and set them to ignore.  Also lock the
 * library data structures if we are thread-safe.
 */

static
void
savesignals(void)
{
#if MP_THREADS_SUPPORT
    __mp_lockmutex(MT_MAIN);
#endif /* MP_THREADS_SUPPORT */
    /* Only perform this step if we are not doing a recursive call.
     */
    if (memhead.recur++ == 0)
    {
        if (!memhead.init)
            __mp_initsignals(&memhead.signals);
        if (memhead.flags & FLG_SAFESIGNALS)
            __mp_savesignals(&memhead.signals);
    }
}


/* Restore the previous signal handlers.  Also unlock the library data
 * structures if we are thread-safe.
 */

static
void
restoresignals(void)
{
    /* Only perform this step if we are not doing a recursive call.
     */
    if (--memhead.recur == 0)
        __mp_restoresignals(&memhead.signals);
#if MP_THREADS_SUPPORT
    __mp_unlockmutex(MT_MAIN);
#endif /* MP_THREADS_SUPPORT */
}


/* Check the validity of all memory blocks, but only if the allocation count
 * is within range and the event count is a multiple of the heap checking
 * frequency.
 */

static
void
checkheap(loginfo *v, unsigned long n)
{
    unsigned long l;

    /* If the lower and upper bounds are zero then we never need to check
     * the heap.
     */
    if ((l = memhead.lrange) || (memhead.urange != 0))
    {
        if (l == (unsigned long) -1)
            l = 0;
        if ((l <= n) && (n <= memhead.urange) &&
            ((memhead.check == 1) || (memhead.event % memhead.check == 0)))
            __mp_checkinfo(&memhead, v);
    }
}


/* Check the alloca allocation stack for any allocations that should be freed.
 */

static
void
checkalloca(loginfo *i, int f)
{
    allocanode *n, *p;
#if MP_FULLSTACK
    addrnode *a, *b;
    stackcompare r;
#endif /* MP_FULLSTACK */
    alloctype t;
    int c;

    if (memhead.fini || (memhead.astack.size == 0))
        return;
#if MP_FULLSTACK
    /* Create the address nodes for the current call.  This is not necessarily
     * an efficient way of doing call stack comparisons if the alloca allocation
     * stack does not contain many outstanding entries, but on some platforms
     * call stack traversal is an expensive business.
     */
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectaddrs(&memhead.addr, MA_READWRITE);
    a = __mp_getaddrs(&memhead.addr, i->stack);
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectaddrs(&memhead.addr, MA_READONLY);
#endif /* MP_FULLSTACK */
    for (n = (allocanode *) memhead.astack.head;
         p = (allocanode *) n->node.next; n = p)
    {
        c = 0;
        if (f == 1)
            c = 1;
#if MP_FULLSTACK
        /* We need to compare the call stacks of the two calling functions in
         * order to see if we can free the allocation.  Currently, this just
         * involves ensuring that the call stacks are different or that the
         * current call stack is shallower than that which made the original
         * allocation.  In theory we could also free the allocation if the
         * current call stack was deeper and the initial addresses differ, but
         * that would involve a lot more calculations.
         */
        else
        {
            b = (addrnode *) n->data.frame;
            if ((b->data.next != NULL) && (a->data.next != NULL) &&
                (((r = __mp_compareaddrs(b->data.next, a->data.next)) ==
                  SC_DIFFERENT) || (r == SC_SHALLOWER)))
                c = 1;
        }
#else /* MP_FULLSTACK */
        /* We need to add an arbitrary number of bytes to the address of the
         * local variable in order to be completely safe, since the current
         * call may differ by one stack frame if the entry to the mpatrol
         * library was via a macro defined in mpatrol.h rather than a function
         * defined in malloc.c, or vice versa.  Unfortunately, this means that
         * we are going to be unable to free the allocation at the first
         * available point when it goes out of scope.
         */
        else if (memhead.alloc.heap.memory.stackdir < 0)
        {
            /* The stack grows down so we must ensure that the current local
             * variable pointer occupies a higher address than that which
             * made the original allocation if we are to free the allocation.
             */
            if ((char *) n->data.frame + 256 < (char *) &i->stack->frame)
                c = 1;
        }
        else
        {
            /* The stack grows up so we must ensure that the current local
             * variable pointer occupies a lower address than that which
             * made the original allocation if we are to free the allocation.
             */
            if ((char *) n->data.frame > (char *) &i->stack->frame + 256)
                c = 1;
        }
#endif /* MP_FULLSTACK */
        if (c == 1)
        {
            if (memhead.prologue && (memhead.recur == 1))
                memhead.prologue(n->block, (size_t) -1, 0, i->func, i->file,
                                 i->line, i->stack->addr);
            memhead.event++;
            t = i->type;
            i->type = AT_ALLOCA;
            __mp_freememory(&memhead, n->block, i);
            i->type = t;
            if (memhead.epilogue && (memhead.recur == 1))
                memhead.epilogue((void *) -1, i->func, i->file, i->line,
                                 i->stack->addr);
        }
    }
#if MP_FULLSTACK
    /* Free the address nodes for the current call.  They may well have to be
     * generated again but that can't be guaranteed.
     */
    if (a != NULL)
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectaddrs(&memhead.addr, MA_READWRITE);
        __mp_freeaddrs(&memhead.addr, a);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectaddrs(&memhead.addr, MA_READONLY);
    }
#endif /* MP_FULLSTACK */
}


/* Deal with anything that needs to be done with marked memory allocations
 * at program termination.
 */

static
void
finishmarked(void)
{
    allocnode *n;
    infonode *m;
    treenode *t;
    unsigned long d;
    int p;

#if MP_THREADS_SUPPORT
    d = __mp_threadid();
#else /* MP_THREADS_SUPPORT */
    d = 0;
#endif /* MP_THREADS_SUPPORT */
    if (memhead.flags & FLG_NOPROTECT)
        p = -1;
    else
        p = 0;
    for (t = __mp_minimum(memhead.alloc.atree.root); t != NULL;
         t = __mp_successor(t))
    {
        n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
        m = (infonode *) n->info;
        if (m->data.flags & FLG_MARKED)
        {
            if ((p == 0) && (m->data.flags & (FLG_PROFILED | FLG_TRACED)))
            {
                __mp_protectinfo(&memhead, MA_READWRITE);
                p = 1;
            }
            if (m->data.flags & FLG_PROFILED)
                __mp_profilefree(&memhead.prof, n->size, m,
                                 !(memhead.flags & FLG_NOPROTECT));
            if (m->data.flags & FLG_TRACED)
                __mp_tracefree(&memhead.trace, m->data.alloc, d, NULL, NULL, 0);
        }
    }
    if (p == 1)
        __mp_protectinfo(&memhead, MA_READONLY);
}


/* The last error encountered by the mpatrol library.
 */

MP_API errortype __mp_errno;


/* Initialise the mpatrol library.
 */

MP_API
void
__mp_init(void)
{
    size_t i;

    savesignals();
    if (memhead.fini)
        /* We currently don't allow the library to be reinitialised after it
         * has been terminated.  To reinitialise the library before it has been
         * terminated use __mp_reinit().
         */
        __mp_abort();
    if (!memhead.init)
    {
        __mp_newinfo(&memhead);
#if (TARGET == TARGET_AMIGA && defined(__GNUC__))
        /* Initialise the simulated UNIX heap.  This gets done anyway, but it
         * forces a reference to the sbrk module which is necessary since
         * we need it for libiberty, which gets linked after libmpatrol.
         */
        sbrk(0);
#endif /* TARGET && __GNUC__ */
#if MP_INUSE_SUPPORT
        _Inuse_init(0, 0);
#endif /* MP_INUSE_SUPPORT */
#if MP_USE_ATEXIT
        /* We will have to terminate the library using atexit() since either
         * this has been explicitly specified or there is no support for
         * .init/.fini functions, constructor/destructor functions or C++.
         * This is usually OK to do but there may be problems if the mpatrol
         * library is terminated before all memory allocations are freed.
         */
        atexit(__mp_fini);
#endif /* MP_USE_ATEXIT */
        /* Read any options from the specified environment variable.
         */
        __mp_parseoptions(&memhead);
        /* Set up the random number generator for the FAILFREQ option.
         */
        if (memhead.fseed == 0)
            memhead.fseed = (unsigned long) time(NULL);
        srand((unsigned int) memhead.fseed);
        /* Attempt to open the log file.
         */
        if (!__mp_openlogfile(memhead.log))
            memhead.log = NULL;
        /* Output the header and version information.
         */
        __mp_printversion();
        /* Attempt to read symbols from any shared libraries and possibly
         * determine the program name if it has not already been set.
         */
        __mp_addextsymbols(&memhead.syms, &memhead.alloc.heap.memory);
        /* Obtain the program filename and attempt to read any symbols from
         * that file.
         */
        if (memhead.alloc.heap.memory.prog != NULL)
            __mp_addsymbols(&memhead.syms, memhead.alloc.heap.memory.prog, NULL,
                            0);
        __mp_fixsymbols(&memhead.syms);
        if (__mp_diagflags & FLG_HTML)
            __mp_diagtag("<HR>");
        __mp_diag("\n");
        if (!(memhead.flags & FLG_NOPROTECT))
        {
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
            __mp_protectsymbols(&memhead.syms, MA_READONLY);
            __mp_protectinfo(&memhead, MA_READONLY);
        }
        /* Start the leak table if necessary.
         */
        if (memhead.flags & FLG_LEAKTABLE)
            memhead.ltable.tracing = 1;
        /* Finally, call any initialisation functions in the order in which
         * they were registered.
         */
        for (i = 0; i < memhead.initcount; i++)
        {
            memhead.inits[i]();
            memhead.inits[i] = NULL;
        }
        memhead.initcount = 0;
    }
    restoresignals();
}


/* Reinitialise the mpatrol library.
 */

MP_API
void
__mp_reinit(void)
{
    char l[256];
    allocnode *n;
    treenode *t;
    unsigned long i;

    savesignals();
    if (!memhead.init)
        __mp_init();
    else if (((memhead.recur == 1) || (memhead.flags & FLG_CHECKFORK)) &&
             ((i = __mp_processid()) != memhead.pid))
    {
        memhead.pid = i;
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READWRITE);
        if (memhead.log == NULL)
            strcpy(l, "stderr");
        else
            strcpy(l, memhead.log);
        memhead.log = __mp_logfile(&memhead.alloc.heap.memory, "%n.%p.log");
        __mp_diag("Log file split to %s\n\n", memhead.log);
        __mp_closelogfile();
        /* Attempt to open the log file.
         */
        if (!__mp_openlogfile(memhead.log))
            memhead.log = NULL;
        /* Output the header and version information.
         */
        __mp_printversion();
        __mp_diag("Log file continued from %s\n\n", l);
        /* Write out any profiling information to the profiling output file.
         */
        if (memhead.prof.autocount > 0)
            __mp_writeprofile(&memhead.prof, !(memhead.flags & FLG_NOPROTECT));
        memhead.prof.file = __mp_proffile(&memhead.alloc.heap.memory,
                                          "%n.%p.out");
        /* Remove the traced flag from any existing memory allocations and then
         * change the tracing output file.
         */
        for (t = __mp_minimum(memhead.alloc.atree.root); t != NULL;
             t = __mp_successor(t))
        {
            n = (allocnode *) ((char *) t - offsetof(allocnode, tnode));
            ((infonode *) n->info)->data.flags &= ~FLG_TRACED;
        }
        __mp_changetrace(&memhead.trace,
                         __mp_tracefile(&memhead.alloc.heap.memory,
                                        "%n.%p.trace"), 0);
        if ((memhead.recur == 1) && !(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READONLY);
    }
    restoresignals();
}


/* Finalise the mpatrol library.
 */

MP_API
void
__mp_fini(void)
{
    stackinfo i;
    loginfo v;

    savesignals();
    if (memhead.init)
    {
        if (__mp_processid() != memhead.pid)
            __mp_reinit();
        if (!memhead.fini)
        {
            /* First, determine the call stack details in case we need to
             * free any remaining allocations that were made by alloca().
             */
            __mp_newframe(&i, NULL);
            if (__mp_getframe(&i))
                __mp_getframe(&i);
            v.ltype = LT_MAX;
            v.type = AT_MAX;
            v.func = NULL;
            v.file = NULL;
            v.line = 0;
            v.stack = &i;
            v.typestr = NULL;
            v.typesize = 0;
            v.logged = 0;
            __mp_checkinfo(&memhead, &v);
            checkalloca(&v, 1);
            /* Then call any finalisation functions in the reverse order in
             * which they were registered.
             */
            while (memhead.finicount > 0)
            {
                memhead.finis[--memhead.finicount]();
                memhead.finis[memhead.finicount] = NULL;
            }
            /* Then close any access library handles that might still be open.
             */
            __mp_closesymbols(&memhead.syms);
            /* Then print a summary of library statistics and settings.
             */
            __mp_printsummary(&memhead);
            /* Display the leak table if necessary.
             */
            if (memhead.flags & FLG_LEAKTABLE)
            {
                if (!(memhead.flags & FLG_NOPROTECT))
                    __mp_protectinfo(&memhead, MA_READWRITE);
                __mp_diag("\n");
                __mp_printleaktab(&memhead, 0, 2, 0);
                if (!(memhead.flags & FLG_NOPROTECT))
                    __mp_protectinfo(&memhead, MA_READONLY);
            }
            /* Then deal with any SHOW options that may have been requested.
             */
            if ((memhead.flags & FLG_SHOWMAP) && (memhead.alloc.list.size > 0))
            {
                __mp_diag("\n");
                __mp_printmap(&memhead);
            }
            if ((memhead.flags & FLG_SHOWSYMBOLS) &&
                (memhead.syms.dtree.size > 0))
                __mp_printsymbols(&memhead.syms);
            if ((memhead.flags & FLG_SHOWFREE) && (memhead.alloc.fsize > 0))
                __mp_printfree(&memhead);
            if ((memhead.flags & FLG_SHOWFREED) && (memhead.alloc.gsize > 0))
                __mp_printfreed(&memhead);
            if ((memhead.alloc.asize - memhead.mtotal) > 0)
            {
                if (memhead.flags & FLG_SHOWUNFREED)
                    __mp_printallocs(&memhead, 0);
                if ((memhead.uabort > 0) &&
                    ((memhead.alloc.asize - memhead.mtotal) >= memhead.uabort))
                    __mp_printallocs(&memhead, 1);
            }
            /* Deal with anything that needs to be done with marked allocations.
             */
            finishmarked();
            /* Next, close the tracing output file.  This will do nothing if
             * tracing has not been enabled.
             */
            __mp_endtrace(&memhead.trace);
            /* Now write out any profiling information to the profiling output
             * file.
             */
            if (memhead.prof.autocount > 0)
                __mp_writeprofile(&memhead.prof,
                                  !(memhead.flags & FLG_NOPROTECT));
            /* Finally, attempt to close the log file.
             */
            __mp_closelogfile();
            memhead.fini = 1;
#if MP_INUSE_SUPPORT
            _Inuse_close();
#endif /* MP_INUSE_SUPPORT */
        }
#if MP_DELETEHEAP
        /* We only need to perform this step if the operating system does not
         * reclaim memory from a terminated process.  We must not perform this
         * step if the operating system needs to deal with dynamically
         * allocated memory after the library has terminated.
         */
        if (!(memhead.flags & FLG_NOPROTECT))
        {
            __mp_protectinfo(&memhead, MA_READWRITE);
            __mp_protectsymbols(&memhead.syms, MA_READWRITE);
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        }
        __mp_deleteinfo(&memhead);
#if MP_THREADS_SUPPORT
        __mp_finimutexes();
#endif /* MP_THREADS_SUPPORT */
#endif /* MP_DELETEHEAP */
        memhead.init = 0;
    }
    restoresignals();
}


/* Provide a function which can be used as a breakpoint target in a debugger.
 */

MP_API
void
__mp_trap(void)
{
}


/* Register a finalisation function to be called when __mp_fini() is called.
 */

MP_API
int
__mp_atexit(void (*f)(void))
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (memhead.finicount == MP_MAXFINIS)
        r = 0;
    else
    {
        memhead.finis[memhead.finicount++] = f;
        r = 1;
    }
    restoresignals();
    return r;
}


/* Set an mpatrol option after the library has been initialised.
 */

MP_API
unsigned long
__mp_setoption(long o, unsigned long v)
{
    unsigned long r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (o > 0)
        r = 1;
    else
    {
        o = -o;
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READWRITE);
        if ((r = __mp_set(&memhead, (unsigned long) o, v)) &&
            (o != OPT_SETFLAGS) && (o != OPT_UNSETFLAGS))
            r = 1;
        if ((memhead.recur == 1) && !(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READONLY);
    }
    restoresignals();
    return r;
}


/* Get an mpatrol option after the library has been initialised.
 */

MP_API
int
__mp_getoption(long o, unsigned long *v)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (o > 0)
        r = 0;
    else
    {
        o = -o;
        r = __mp_get(&memhead, (unsigned long) o, v);
    }
    restoresignals();
    return r;
}


/* Return the memory header structure.
 */

MP_API
infohead *
__mp_memhead(void)
{
    return &memhead;
}


/* Allocate a new block of memory of a specified size and alignment.
 */

MP_API
void *
__mp_alloc(size_t l, size_t a, alloctype f, char *s, char *t, unsigned long u,
           char *g, size_t h, size_t k)
{
    void *p;
    stackinfo i;
    loginfo v;
    int j, z;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we must
     * allocate new memory with sbrk().  We don't even attempt to do anything
     * with calls to memalign(), valloc() and pvalloc() but these shouldn't
     * be coming through anyway.
     */
    if (!crt_initialised())
    {
        if (l == 0)
            l = 1;
        if ((p = sbrk(l)) == (void *) -1)
            if ((f == AT_ALLOCA) || (f == AT_XMALLOC) || (f == AT_XCALLOC))
                abort();
            else
                p = NULL;
        else if ((f == AT_CALLOC) || (f == AT_XCALLOC))
            __mp_memset(p, 0, l);
        return p;
    }
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if (memhead.prologue && (memhead.recur == 1))
        memhead.prologue((void *) -1, l, __mp_fixalign(&memhead, f, a), s, t, u,
                         i.addr);
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = g;
    v.typesize = h;
    v.logged = 0;
    checkheap(&v, memhead.count + 1);
    checkalloca(&v, 0);
    memhead.event++;
    if (((f == AT_XMALLOC) || (f == AT_XCALLOC)) && (l == 0) && (h != 0))
        l = h;
    z = 0;
  retry:
    p = __mp_getmemory(&memhead, l, a, &v);
    if (memhead.epilogue && (memhead.recur == 1))
        memhead.epilogue(p, s, t, u, i.addr);
    if (p == NULL)
    {
        if ((z == 0) && (memhead.nomemory))
        {
            /* Call the low-memory handler if no memory block was allocated.
             */
            memhead.nomemory(s, t, u, i.addr);
            if (memhead.prologue && (memhead.recur == 1))
                memhead.prologue((void *) -1, l, __mp_fixalign(&memhead, f, a),
                                 s, t, u, i.addr);
            if ((f != AT_NEW) && (f != AT_NEWVEC))
                z = 1;
            goto retry;
        }
        if ((f == AT_ALLOCA) || (f == AT_XMALLOC) || (f == AT_XCALLOC))
        {
            /* The alloca(), xmalloc() and xcalloc() functions should always
             * return non-NULL pointers, which means we must abort here.
             */
            __mp_printsummary(&memhead);
            __mp_diag("\n");
            __mp_error(ET_OUTMEM, f, t, u, NULL);
            memhead.fini = 1;
            __mp_abort();
        }
    }
    restoresignals();
    return p;
}


/* Allocate a new block of memory to duplicate a string.
 */

MP_API
char *
__mp_strdup(char *p, size_t l, alloctype f, char *s, char *t, unsigned long u,
            size_t k)
{
    char *o;
    stackinfo i;
    loginfo v;
    size_t n;
    int j, z;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we must
     * allocate new memory with sbrk() and copy the string to the new
     * allocation.
     */
    if (!crt_initialised())
    {
        if (p == NULL)
            o = NULL;
        else
        {
            n = strlen(p);
            if (((f == AT_STRNDUP) || (f == AT_STRNSAVE) ||
                 (f == AT_STRNDUPA)) && (n > l))
                n = l;
            if ((o = (char *) sbrk(n + 1)) == (void *) -1)
                o = NULL;
            else
            {
                __mp_memcopy(o, p, n);
                o[n] = '\0';
            }
        }
        if ((o == NULL) && ((f == AT_STRDUPA) || (f == AT_STRNDUPA) ||
             (f == AT_XSTRDUP)))
            abort();
        return o;
    }
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if (memhead.prologue && (memhead.recur == 1))
        memhead.prologue(p, (size_t) -2, 1, s, t, u, i.addr);
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = "char";
    v.typesize = sizeof(char);
    v.logged = 0;
    checkheap(&v, memhead.count + 1);
    checkalloca(&v, 0);
    memhead.event++;
    o = p;
    z = 0;
  retry:
    if ((f == AT_STRNDUP) || (f == AT_STRNSAVE) || (f == AT_STRNDUPA))
        j = 1;
    else
        j = 0;
    n = l;
    /* If the string is not NULL and does not overflow any memory blocks then
     * allocate the memory and copy the string to the new allocation.
     */
    if (__mp_checkstring(&memhead, o, &n, &v, j))
    {
        if (p = (char *) __mp_getmemory(&memhead, n + 1, 1, &v))
        {
            __mp_memcopy(p, o, n);
            p[n] = '\0';
        }
    }
    else
        p = NULL;
    if (memhead.epilogue && (memhead.recur == 1))
        memhead.epilogue(p, s, t, u, i.addr);
    if (p == NULL)
    {
        if ((z == 0) && memhead.nomemory)
        {
            /* Call the low-memory handler if no memory block was allocated.
             */
            memhead.nomemory(s, t, u, i.addr);
            if (memhead.prologue && (memhead.recur == 1))
                memhead.prologue(o, (size_t) -2, 1, s, t, u, i.addr);
            z = 1;
            goto retry;
        }
        if ((f == AT_STRDUPA) || (f == AT_STRNDUPA) || (f == AT_XSTRDUP))
        {
            /* The strdupa(), strndupa() and xstrdup() functions should always
             * return a non-NULL pointer, which means we must abort here.
             */
            __mp_printsummary(&memhead);
            __mp_diag("\n");
            __mp_error(ET_OUTMEM, f, t, u, NULL);
            memhead.fini = 1;
            __mp_abort();
        }
    }
    restoresignals();
    return p;
}


/* Resize an existing block of memory to a new size and alignment.
 */

MP_API
void *
__mp_realloc(void *p, size_t l, size_t a, alloctype f, char *s, char *t,
             unsigned long u, char *g, size_t h, size_t k)
{
    void *q;
    stackinfo i;
    loginfo v;
    int j, z;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we must
     * allocate new memory with sbrk() and copy the old contents to the new
     * allocation.  We can't free the old allocation as we know nothing
     * about it.
     */
    if (!crt_initialised())
    {
        if (p == NULL)
        {
            if (l == 0)
                l = 1;
            if ((q = sbrk(l)) == (void *) -1)
                q = NULL;
            else if (f == AT_RECALLOC)
                __mp_memset(q, 0, l);
        }
        else if ((l == 0) || (f == AT_EXPAND) || ((q = sbrk(l)) == (void *) -1))
            q = NULL;
        else
            __mp_memcopy(q, p, l);
        if (q == NULL)
            if (f == AT_REALLOCF)
                free(p);
            else if (f == AT_XREALLOC)
                abort();
        return q;
    }
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if (memhead.prologue && (memhead.recur == 1))
        memhead.prologue(p, l, __mp_fixalign(&memhead, f, a), s, t, u, i.addr);
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = g;
    v.typesize = h;
    v.logged = 0;
    checkheap(&v, memhead.count);
    checkalloca(&v, 0);
    memhead.event++;
    if ((f == AT_XREALLOC) && (l == 0) && (h != 0))
        l = h;
    q = p;
    z = 0;
  retry:
    p = __mp_resizememory(&memhead, q, l, a, &v);
    if (memhead.epilogue && (memhead.recur == 1))
        memhead.epilogue(p, s, t, u, i.addr);
    if (p == NULL)
    {
        if ((z == 0) && memhead.nomemory)
        {
            /* Call the low-memory handler if no memory block was allocated.
             */
            memhead.nomemory(s, t, u, i.addr);
            if (memhead.prologue && (memhead.recur == 1))
                memhead.prologue(q, l, __mp_fixalign(&memhead, f, a), s, t, u,
                                 i.addr);
            z = 1;
            goto retry;
        }
        if (f == AT_XREALLOC)
        {
            /* The xrealloc() function should always return a non-NULL pointer,
             * which means we must abort here.
             */
            __mp_printsummary(&memhead);
            __mp_diag("\n");
            __mp_error(ET_OUTMEM, f, t, u, NULL);
            memhead.fini = 1;
            __mp_abort();
        }
    }
    restoresignals();
    return p;
}


/* Free an existing block of memory.
 */

MP_API
void
__mp_free(void *p, alloctype f, char *s, char *t, unsigned long u, size_t k)
{
    stackinfo i;
    loginfo v;
    int j;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then just
     * return since we know nothing about any of the prior allocations anyway.
     */
    if (!crt_initialised() || memhead.fini)
#else /* TARGET */
    if (memhead.fini)
#endif /* TARGET */
        return;
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if (memhead.prologue && (memhead.recur == 1))
        memhead.prologue(p, (size_t) -1, 0, s, t, u, i.addr);
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    checkheap(&v, memhead.count);
    checkalloca(&v, 0);
    memhead.event++;
    __mp_freememory(&memhead, p, &v);
    if (memhead.epilogue && (memhead.recur == 1))
        memhead.epilogue((void *) -1, s, t, u, i.addr);
    restoresignals();
}


/* Set a block of memory to contain a specific byte.
 */

MP_API
void *
__mp_setmem(void *p, size_t l, unsigned char c, alloctype f, char *s, char *t,
            unsigned long u, size_t k)
{
    stackinfo i;
    loginfo v;
    int j;

    if (!memhead.init || memhead.fini)
    {
        __mp_memset(p, c, l);
        return p;
    }
    savesignals();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    checkalloca(&v, 0);
    __mp_setmemory(&memhead, p, l, c, &v);
    restoresignals();
    return p;
}


/* Copy a block of memory from one address to another.
 */

MP_API
void *
__mp_copymem(void *p, void *q, size_t l, unsigned char c, alloctype f, char *s,
             char *t, unsigned long u, size_t k)
{
    void *r;
    stackinfo i;
    loginfo v;
    int j;

    if (!memhead.init || memhead.fini)
        if (f == AT_MEMCCPY)
        {
            if (r = __mp_memfind(p, l, &c, 1))
                l = (size_t) ((char *) r - (char *) p) + 1;
            __mp_memcopy(q, p, l);
            if (r != NULL)
                return (char *) q + l;
            else
                return NULL;
        }
        else
        {
            __mp_memcopy(q, p, l);
            return q;
        }
    savesignals();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    checkalloca(&v, 0);
    q = __mp_copymemory(&memhead, p, q, l, c, &v);
    restoresignals();
    return q;
}


/* Attempt to locate the position of one block of memory in another block.
 */

MP_API
void *
__mp_locatemem(void *p, size_t l, void *q, size_t m, alloctype f, char *s,
               char *t, unsigned long u, size_t k)
{
    void *r;
    stackinfo i;
    loginfo v;
    int j;
    unsigned char b;

    if (f == AT_MEMCHR)
    {
        /* If the function that called us was memchr() then we must convert the
         * second length to a character and set up the new pointer and length.
         */
        b = (unsigned char) (m & 0xFF);
        q = (void *) &b;
        m = 1;
    }
    if (!memhead.init || memhead.fini)
        return __mp_memfind(p, l, q, m);
    savesignals();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    checkalloca(&v, 0);
    r = __mp_locatememory(&memhead, p, l, q, m, &v);
    restoresignals();
    return r;
}


/* Compare two blocks of memory.
 */

MP_API
int
__mp_comparemem(void *p, void *q, size_t l, alloctype f, char *s, char *t,
                unsigned long u, size_t k)
{
    void *m;
    stackinfo i;
    loginfo v;
    int j, r;

    if (!memhead.init || memhead.fini)
        if (m = __mp_memcompare(p, q, l))
        {
            l = (char *) m - (char *) p;
            return (int) ((unsigned char *) p)[l] -
                   (int) ((unsigned char *) q)[l];
        }
        else
            return 0;
    savesignals();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
    {
        j = __mp_getframe(&i);
        while ((k > 0) && (j != 0))
        {
            j = __mp_getframe(&i);
            k--;
        }
    }
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = f;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    checkalloca(&v, 0);
    r = __mp_comparememory(&memhead, p, q, l, &v);
    restoresignals();
    return r;
}


/* Return the full version number of the mpatrol library.
 */

MP_API
unsigned long
__mp_libversion(void)
{
    return MP_VERNUM;
}


/* Return an error message corresponding to a given error type.
 */

MP_API
char *
__mp_strerror(errortype e)
{
    if ((e >= (errortype) 0) && (e <= ET_MAX))
        return __mp_errordetails[e].string;
    return NULL;
}


/* Return the name of the function corresponding to a given allocation type.
 */

MP_API
char *
__mp_function(alloctype f)
{
    if ((f >= (alloctype) 0) && (f < AT_MAX))
        return __mp_functionnames[f];
    return NULL;
}


/* Set the user data for a given memory allocation.
 */

MP_API
int
__mp_setuser(void *p, void *d)
{
    allocnode *n;
    infonode *m;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if (((n = __mp_findalloc(&memhead.alloc, p)) == NULL) ||
        ((m = (infonode *) n->info) == NULL))
        r = 0;
    else
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READWRITE);
        m->data.userdata = d;
        if ((memhead.recur == 1) && !(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READONLY);
        r = 1;
    }
    restoresignals();
    return r;
}


/* Set the marked flag for a given memory allocation.
 */

MP_API
int
__mp_setmark(void *p)
{
    allocnode *n;
    infonode *m;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if (((n = __mp_findalloc(&memhead.alloc, p)) == NULL) ||
        ((m = (infonode *) n->info) == NULL) || (m->data.flags & FLG_MARKED) ||
        (m->data.type == AT_ALLOCA) || (m->data.type == AT_STRDUPA) ||
        (m->data.type == AT_STRNDUPA))
        r = 0;
    else
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READWRITE);
        m->data.flags |= FLG_MARKED;
        if ((memhead.recur == 1) && !(memhead.flags & FLG_NOPROTECT))
            __mp_protectinfo(&memhead, MA_READONLY);
        memhead.mcount++;
        memhead.mtotal += n->size;
        r = 1;
    }
    restoresignals();
    return r;
}


/* Obtain any details about the memory block that contains a given address.
 */

MP_API
int
__mp_info(void *p, allocinfo *d)
{
    addrnode *a;
    symnode *s;
    allocnode *n;
    infonode *m;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if ((n = __mp_findnode(&memhead.alloc, p, 1)) == NULL)
    {
        restoresignals();
        return 0;
    }
    /* Return mostly empty fields if the pointer is in free memory.
     */
    if ((m = (infonode *) n->info) == NULL)
    {
        d->block = n->block;
        d->size = n->size;
        d->type = AT_MAX;
        d->alloc = 0;
        d->realloc = 0;
        d->thread = 0;
        d->event = 0;
        d->func = NULL;
        d->file = NULL;
        d->line = 0;
        d->stack = NULL;
        d->typestr = NULL;
        d->typesize = 0;
        d->userdata = NULL;
        d->allocated = 0;
        d->freed = 0;
        d->marked = 0;
        d->profiled = 0;
        d->traced = 0;
        d->internal = 0;
        restoresignals();
        return 1;
    }
    /* We now fill in the details for the supplied structure.
     */
    d->block = n->block;
    d->size = n->size;
    d->type = m->data.type;
    d->alloc = m->data.alloc;
    d->realloc = m->data.realloc;
#if MP_THREADS_SUPPORT
    d->thread = m->data.thread;
#else /* MP_THREADS_SUPPORT */
    d->thread = 0;
#endif /* MP_THREADS_SUPPORT */
    d->event = m->data.event;
    d->func = m->data.func;
    d->file = m->data.file;
    d->line = m->data.line;
    d->stack = m->data.stack;
    d->typestr = m->data.typestr;
    d->typesize = m->data.typesize;
    d->userdata = m->data.userdata;
    d->allocated = 1;
    d->freed = ((m->data.flags & FLG_FREED) != 0);
    d->marked = ((m->data.flags & FLG_MARKED) != 0);
    d->profiled = ((m->data.flags & FLG_PROFILED) != 0);
    d->traced = ((m->data.flags & FLG_TRACED) != 0);
    d->internal = ((m->data.flags & FLG_INTERNAL) != 0);
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectinfo(&memhead, MA_READWRITE);
    /* The names of the symbols in the call stack may not have been determined
     * yet, so we traverse the stack, filling in any known symbol names as we
     * go.
     */
    for (a = m->data.stack; a != NULL; a = a->data.next)
        if ((a->data.name == NULL) &&
            (s = __mp_findsymbol(&memhead.syms, a->data.addr)))
            a->data.name = s->data.name;
    if ((memhead.recur == 1) && !(memhead.flags & FLG_NOPROTECT))
        __mp_protectinfo(&memhead, MA_READONLY);
    restoresignals();
    return 1;
}


/* Obtain any details about the function symbol that contains a given address.
 */

MP_API
int
__mp_syminfo(void *p, symbolinfo *d)
{
    symnode *n;
    char *s, *t;
    unsigned long u;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    n = __mp_findsymbol(&memhead.syms, p);
    r = __mp_findsource(&memhead.syms, p, &s, &t, &u);
    if (((n == NULL) && (s != NULL)) || (t != NULL))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if ((n == NULL) && (s != NULL))
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if ((n != NULL) || (r != 0))
    {
        if (n != NULL)
        {
            d->name = n->data.name;
            d->object = n->data.file;
            d->addr = n->data.addr;
            d->size = n->data.size;
        }
        else
        {
            d->name = s;
            d->object = NULL;
            d->addr = NULL;
            d->size = 0;
        }
        d->file = t;
        d->line = u;
        r = 1;
    }
    restoresignals();
    return r;
}


/* Obtain the name of the function symbol that contains a given address.
 */

MP_API
char *
__mp_symbol(void *p)
{
    symnode *n;
    char *s, *t;
    unsigned long u;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (n = __mp_findsymbol(&memhead.syms, p))
        t = n->data.name;
    else if (__mp_findsource(&memhead.syms, p, &s, &t, &u) && (s != NULL))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        t = __mp_addstring(&memhead.syms.strings, s);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    else
        t = NULL;
    restoresignals();
    return t;
}


/* Display any details about the memory block that contains a given address.
 * This is for calling within a symbolic debugger and will result in output to
 * the standard error file stream instead of the log file.
 */

MP_API
int
__mp_printinfo(void *p)
{
    addrnode *a;
    symnode *s;
    allocnode *n;
    infonode *m;

    savesignals();
    if (memhead.init && (__mp_processid() != memhead.pid))
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    n = NULL;
    if (!memhead.init || memhead.fini ||
        ((n = __mp_findnode(&memhead.alloc, p, 1)) == NULL) ||
        ((m = (infonode *) n->info) == NULL))
    {
        fprintf(stderr, "address " MP_POINTER, p);
        if (n == NULL)
            fputs(" not in heap\n", stderr);
        else
        {
            fputs(" located in free memory:\n", stderr);
            fprintf(stderr, "    start of block:     " MP_POINTER "\n",
                    n->block);
            fprintf(stderr, "    size of block:      %lu byte%s\n", n->size,
                    (n->size == 1) ? "" : "s");
        }
        restoresignals();
        return 0;
    }
    /* We now display the details of the associated memory block.
     */
    fprintf(stderr, "address " MP_POINTER " located in %s block:\n", p,
            (m->data.flags & FLG_FREED) ? "freed" : "allocated");
    fprintf(stderr, "    start of block:     " MP_POINTER "\n", n->block);
    fprintf(stderr, "    size of block:      %lu byte%s\n", n->size,
            (n->size == 1) ? "" : "s");
    fprintf(stderr, "    stored type:        %s\n",
            m->data.typestr ? m->data.typestr : "<unknown>");
    fputs("    stored type size:   ", stderr);
    if (m->data.typesize)
        fprintf(stderr, "%lu byte%s\n", m->data.typesize,
                (m->data.typesize == 1) ? "" : "s");
    else
        fputs("<unknown>\n", stderr);
    fprintf(stderr, "    user data:          " MP_POINTER "\n",
            m->data.userdata);
    if (m->data.flags & FLG_FREED)
        fputs("    freed by:           ", stderr);
    else
        fputs("    allocated by:       ", stderr);
    fprintf(stderr, "%s\n", __mp_functionnames[m->data.type]);
    fprintf(stderr, "    allocation index:   %lu\n", m->data.alloc);
    fprintf(stderr, "    reallocation index: %lu\n", m->data.realloc);
#if MP_THREADS_SUPPORT
    fprintf(stderr, "    thread identifier:  %lu\n", m->data.thread);
#endif /* MP_THREADS_SUPPORT */
    fprintf(stderr, "    modification event: %lu\n", m->data.event);
    fputs("    flags:             ", stderr);
    if (m->data.flags == 0)
        fputs(" none\n", stderr);
    else
    {
        if (m->data.flags & FLG_FREED)
            fputs(" freed", stderr);
        if (m->data.flags & FLG_MARKED)
            fputs(" marked", stderr);
        if (m->data.flags & FLG_PROFILED)
            fputs(" profiled", stderr);
        if (m->data.flags & FLG_TRACED)
            fputs(" traced", stderr);
        if (m->data.flags & FLG_INTERNAL)
            fputs(" internal", stderr);
        fputc('\n', stderr);
    }
    fprintf(stderr, "    calling function:   %s\n",
            m->data.func ? m->data.func : "<unknown>");
    fprintf(stderr, "    called from file:   %s\n",
            m->data.file ? m->data.file : "<unknown>");
    fputs("    called at line:     ", stderr);
    if (m->data.line)
        fprintf(stderr, "%lu\n", m->data.line);
    else
        fputs("<unknown>\n", stderr);
    /* Traverse the function call stack, displaying as much information as
     * possible.
     */
    if (a = m->data.stack)
    {
        fputs("    function call stack:\n", stderr);
        do
        {
            fprintf(stderr, "\t" MP_POINTER " ", a->data.addr);
            if (a->data.name)
                fputs(a->data.name, stderr);
            else if (s = __mp_findsymbol(&memhead.syms, a->data.addr))
                fputs(s->data.name, stderr);
            else
                fputs("???", stderr);
            fputc('\n', stderr);
            a = a->data.next;
        }
        while (a != NULL);
    }
    restoresignals();
    return 1;
}


/* Return the current allocation event count for later use when examining
 * the difference in the list of allocations between now and a future point.
 */

MP_API
unsigned long
__mp_snapshot(void)
{
    unsigned long i;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    i = memhead.event;
    restoresignals();
    return i;
}


/* Iterate over all of the allocated and freed memory blocks, calling a
 * user-supplied function for each one encountered, selecting only those
 * memory blocks that have been modified since a given allocation event.
 */

MP_API
size_t
__mp_iterate(int (*f)(void *, void *), void *d, unsigned long s)
{
    allocnode *n, *p;
    infonode *m;
    size_t i;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    i = 0;
    for (n = (allocnode *) memhead.alloc.list.head;
         p = (allocnode *) n->lnode.next; n = p)
        if ((m = (infonode *) n->info) && !(m->data.flags & FLG_INTERNAL) &&
            (m->data.event > s))
        {
            if (f == NULL)
                r = __mp_printinfo(n->block);
            else
                r = f(n->block, d);
            if (r > 0)
                i++;
            else if (r < 0)
                break;
        }
    restoresignals();
    return i;
}


/* Iterate over all of the allocated, freed and free memory blocks, calling
 * a user-supplied function for each one encountered.
 */

MP_API
size_t
__mp_iterateall(int (*f)(void *, void *), void *d)
{
    allocnode *n, *p;
    size_t i;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    i = 0;
    for (n = (allocnode *) memhead.alloc.list.head;
         p = (allocnode *) n->lnode.next; n = p)
    {
        if (f == NULL)
            r = __mp_printinfo(n->block);
        else
            r = f(n->block, d);
        if (r > 0)
            i++;
        else if (r < 0)
            break;
    }
    restoresignals();
    return i;
}


/* Add a memory allocation to the leak table.
 */

MP_API
int
__mp_addallocentry(char *f, unsigned long l, size_t c)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectinfo(&memhead, MA_READWRITE);
    r = __mp_allocentry(&memhead.ltable, f, l, c);
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectinfo(&memhead, MA_READONLY);
    restoresignals();
    return r;
}


/* Remove a memory allocation from the leak table.
 */

MP_API
int
__mp_addfreeentry(char *f, unsigned long l, size_t c)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectleaktab(&memhead.ltable, MA_READWRITE);
    r = __mp_freeentry(&memhead.ltable, f, l, c);
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectleaktab(&memhead.ltable, MA_READONLY);
    restoresignals();
    return r;
}


/* Clear the leak table.
 */

MP_API
void
__mp_clearleaktable(void)
{
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectleaktab(&memhead.ltable, MA_READWRITE);
    __mp_clearleaktab(&memhead.ltable);
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectleaktab(&memhead.ltable, MA_READONLY);
    restoresignals();
}


/* Start recording memory allocation events in the leak table.
 */

MP_API
int
__mp_startleaktable(void)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    r = memhead.ltable.tracing;
    memhead.ltable.tracing = 1;
    restoresignals();
    return r;
}


/* Stop recording memory allocation events in the leak table.
 */

MP_API
int
__mp_stopleaktable(void)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    r = memhead.ltable.tracing;
    memhead.ltable.tracing = 0;
    restoresignals();
    return r;
}


/* Display the leak table.
 */

MP_API
void
__mp_leaktable(size_t l, int o, unsigned char f)
{
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectleaktab(&memhead.ltable, MA_READWRITE);
    __mp_printleaktab(&memhead, l, o, f);
    if (!(memhead.flags & FLG_NOPROTECT))
        __mp_protectleaktab(&memhead.ltable, MA_READONLY);
    restoresignals();
}


/* Display a complete memory map of the heap and (optionally) a summary of
 * all mpatrol library settings and statistics.
 */

MP_API
void
__mp_memorymap(int s)
{
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (s != 0)
        __mp_printsummary(&memhead);
    if (memhead.alloc.list.size > 0)
    {
        if (s != 0)
            __mp_diag("\n");
        __mp_printmap(&memhead);
    }
    restoresignals();
}


/* Display a summary of all mpatrol library settings and statistics.
 */

MP_API
void
__mp_summary(void)
{
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    __mp_printsummary(&memhead);
    restoresignals();
}


/* Return statistics about the current state of the heap.
 */

MP_API
int
__mp_stats(heapinfo *d)
{
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    d->acount = memhead.alloc.atree.size;
    d->atotal = memhead.alloc.asize;
    d->fcount = memhead.alloc.ftree.size;
    d->ftotal = memhead.alloc.fsize;
    d->gcount = memhead.alloc.gtree.size;
    d->gtotal = memhead.alloc.gsize;
    d->icount = memhead.alloc.heap.itree.size + memhead.alloc.itree.size +
                memhead.addr.list.size + memhead.syms.strings.list.size +
                memhead.syms.strings.tree.size + memhead.syms.itree.size +
                memhead.ltable.list.size + memhead.prof.ilist.size +
                memhead.list.size + memhead.alist.size;
    d->itotal = memhead.alloc.heap.isize + memhead.alloc.isize +
                memhead.addr.size + memhead.syms.strings.size +
                memhead.syms.size + memhead.ltable.isize + memhead.prof.size +
                memhead.size;
    d->mcount = memhead.mcount;
    d->mtotal = memhead.mtotal;
    restoresignals();
    return 1;
}


/* Check the validity of all memory blocks that have been filled with
 * a predefined pattern.
 */

MP_API
void
__mp_checkheap(char *s, char *t, unsigned long u)
{
    stackinfo i;
    loginfo v;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details in case we need to free any
     * allocations that were made by alloca().
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = AT_MAX;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    __mp_checkinfo(&memhead, &v);
    checkalloca(&v, 0);
    restoresignals();
}


/* Check the validity of all memory blocks that have been filled with
 * a predefined pattern.
 */

MP_API
void
__mp_check(void)
{
    __mp_checkheap(NULL, NULL, 0);
}


/* Set the prologue function and return the previous setting.
 */

MP_API
prologuehandler
__mp_prologue(prologuehandler h)
{
    prologuehandler p;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    p = memhead.prologue;
    memhead.prologue = h;
    restoresignals();
    return p;
}


/* Set the epilogue function and return the previous setting.
 */

MP_API
epiloguehandler
__mp_epilogue(epiloguehandler h)
{
    epiloguehandler p;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    p = memhead.epilogue;
    memhead.epilogue = h;
    restoresignals();
    return p;
}


/* Set the low-memory handler and return the previous setting.
 */

MP_API
nomemoryhandler
__mp_nomemory(nomemoryhandler h)
{
    nomemoryhandler p;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    p = memhead.nomemory;
    memhead.nomemory = h;
    restoresignals();
    return p;
}


/* Push source level information onto the top of the delete stack.
 */

MP_API
void
__mp_pushdelstack(char *s, char *t, unsigned long u)
{
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if ((memhead.delpos >= 0) && (memhead.delpos < MP_MAXDELSTACK))
    {
        memhead.dels[memhead.delpos].func = s;
        memhead.dels[memhead.delpos].file = t;
        memhead.dels[memhead.delpos].line = u;
    }
    memhead.delpos++;
}


/* Pop source level information off the top of the delete stack.
 */

MP_API
void
__mp_popdelstack(char **s, char **t, unsigned long *u)
{
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if ((--memhead.delpos >= 0) && (memhead.delpos < MP_MAXDELSTACK))
    {
        *s = memhead.dels[memhead.delpos].func;
        *t = memhead.dels[memhead.delpos].file;
        *u = memhead.dels[memhead.delpos].line;
    }
    else
    {
        *s = *t = NULL;
        *u = 0;
    }
}


/* Write user data to the mpatrol log file.
 */

MP_API
int
__mp_printf(char *s, ...)
{
    char b[1024];
    char *p, *t;
    va_list v;
    size_t l;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    va_start(v, s);
    r = vsprintf(b, s, v);
    va_end(v);
    if (r >= 0)
    {
        l = strlen(MP_PRINTPREFIX);
        for (t = b; p = strchr(t, '\n'); t = p + 1)
        {
            *p = '\0';
            if (*t != '\0')
            {
                __mp_diag("%s%s", MP_PRINTPREFIX, t);
                r += l;
            }
            __mp_diag("\n");
        }
        if (*t != '\0')
        {
            __mp_diag("%s%s\n", MP_PRINTPREFIX, t);
            r += l + 1;
        }
    }
    restoresignals();
    return r;
}


/* Write user data to the mpatrol log file.
 */

MP_API
int
__mp_vprintf(char *s, va_list v)
{
    char b[1024];
    char *p, *t;
    size_t l;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    r = vsprintf(b, s, v);
    if (r >= 0)
    {
        l = strlen(MP_PRINTPREFIX);
        for (t = b; p = strchr(t, '\n'); t = p + 1)
        {
            *p = '\0';
            if (*t != '\0')
            {
                __mp_diag("%s%s", MP_PRINTPREFIX, t);
                r += l;
            }
            __mp_diag("\n");
        }
        if (*t != '\0')
        {
            __mp_diag("%s%s\n", MP_PRINTPREFIX, t);
            r += l + 1;
        }
    }
    restoresignals();
    return r;
}


/* Write user data to the mpatrol log file along with location information.
 */

MP_API
void
__mp_printfwithloc(char *s, char *t, unsigned long u, char *m, ...)
{
    char b[1024];
    char *p, *r;
    stackinfo i;
    va_list v;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    va_start(v, m);
    vsprintf(b, m, v);
    va_end(v);
    for (r = b; p = strchr(r, '\n'); r = p + 1)
    {
        *p = '\0';
        if (*r != '\0')
            __mp_diag("%s%s", MP_PRINTPREFIX, r);
        __mp_diag("\n");
    }
    if (*r != '\0')
        __mp_diag("%s%s\n", MP_PRINTPREFIX, r);
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if ((s != NULL) || (t != NULL))
    {
        __mp_diag("   ");
        if (s != NULL)
            __mp_diag(" in function `%s'", s);
        if (t != NULL)
            __mp_diag(" in file `%s' at line %lu", t, u);
        __mp_diag("\n");
    }
    if (i.addr != NULL)
    {
        __mp_printstack(&memhead.syms, &i);
        __mp_diag("\n");
    }
    restoresignals();
}


/* Write user data to the mpatrol log file along with location information.
 */

MP_API
void
__mp_vprintfwithloc(char *s, char *t, unsigned long u, char *m, va_list v)
{
    char b[1024];
    char *p, *r;
    stackinfo i;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    vsprintf(b, m, v);
    for (r = b; p = strchr(r, '\n'); r = p + 1)
    {
        *p = '\0';
        if (*r != '\0')
            __mp_diag("%s%s", MP_PRINTPREFIX, r);
        __mp_diag("\n");
    }
    if (*r != '\0')
        __mp_diag("%s%s\n", MP_PRINTPREFIX, r);
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* If no filename was passed through then attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (t == NULL) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    if ((s != NULL) || (t != NULL))
    {
        __mp_diag("   ");
        if (s != NULL)
            __mp_diag(" in function `%s'", s);
        if (t != NULL)
            __mp_diag(" in file `%s' at line %lu", t, u);
        __mp_diag("\n");
    }
    if (i.addr != NULL)
    {
        __mp_printstack(&memhead.syms, &i);
        __mp_diag("\n");
    }
    restoresignals();
}


/* Write a hex dump for a specified memory location to the mpatrol log file.
 */

MP_API
void
__mp_logmemory(void *p, size_t l)
{
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    __mp_printmemory(p, l);
    __mp_diag("\n");
    restoresignals();
}


/* Write the current call stack to the mpatrol log file.
 */

MP_API
int
__mp_logstack(size_t k)
{
    stackinfo i;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    __mp_newframe(&i, NULL);
    if (r = __mp_getframe(&i))
    {
        r = __mp_getframe(&i);
        while ((k > 0) && (r != 0))
        {
            r = __mp_getframe(&i);
            k--;
        }
    }
    if (r != 0)
    {
        __mp_printstack(&memhead.syms, &i);
        __mp_diag("\n");
    }
    restoresignals();
    return r;
}


/* Write the details about the memory block that contains a given address to
 * the mpatrol log file.
 */

MP_API
int
__mp_logaddr(void *p)
{
    allocnode *n;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (((n = __mp_findnode(&memhead.alloc, p, 1)) == NULL) ||
        (n->info == NULL))
        r = 0;
    else
    {
        __mp_printalloc(&memhead.syms, n);
        __mp_diag("\n");
        r = 1;
    }
    restoresignals();
    return r;
}


/* Invoke a text editor on a given source file at a specific line.
 */

MP_API
int
__mp_edit(char *f, unsigned long l)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    r = __mp_editfile(f, l, 0);
    restoresignals();
    return r;
}


/* List a given source file at a specific line.
 */

MP_API
int
__mp_list(char *f, unsigned long l)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    r = __mp_editfile(f, l, 1);
    restoresignals();
    return r;
}


/* Edit or list a given source file at a specific line, but only if the EDIT
 * or LIST options are in effect.
 */

MP_API
int
__mp_view(char *f, unsigned long l)
{
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    if (__mp_diagflags & FLG_EDIT)
        r = __mp_editfile(f, l, 0);
    else if (__mp_diagflags & FLG_LIST)
        r = __mp_editfile(f, l, 1);
    else
        r = 0;
    restoresignals();
    return r;
}


/* Read in an allocation contents file.
 */

MP_API
int
__mp_readcontents(char *s, void *p)
{
    allocnode *n;
    infonode *m;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if (((n = __mp_findalloc(&memhead.alloc, p)) == NULL) ||
        ((m = (infonode *) n->info) == NULL))
        r = 0;
    else
        r = __mp_readalloc(s, m->data.alloc, n->block, n->size);
    restoresignals();
    return r;
}


/* Write out an allocation contents file.
 */

MP_API
int
__mp_writecontents(char *s, void *p)
{
    allocnode *n;
    infonode *m;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if (((n = __mp_findalloc(&memhead.alloc, p)) == NULL) ||
        ((m = (infonode *) n->info) == NULL))
        r = 0;
    else
        r = __mp_writealloc(s, m->data.alloc, n->block, n->size);
    restoresignals();
    return r;
}


/* Compare an allocation contents file with the contents currently in memory.
 */

MP_API
long
__mp_cmpcontents(char *s, void *p)
{
    allocnode *n;
    infonode *m;
    long r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if (((n = __mp_findalloc(&memhead.alloc, p)) == NULL) ||
        ((m = (infonode *) n->info) == NULL))
        r = -1;
    else
        r = __mp_cmpalloc(s, m->data.alloc, n->block, n->size);
    restoresignals();
    return r;
}


/* Remove an allocation contents file.
 */

MP_API
int
__mp_remcontents(char *s, void *p)
{
    allocnode *n;
    infonode *m;
    int r;

    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Check that we know something about the address that was supplied.
     */
    if (((n = __mp_findnode(&memhead.alloc, p, 1)) == NULL) ||
        ((m = (infonode *) n->info) == NULL))
        r = 0;
    else
        r = __mp_remalloc(s, m->data.alloc);
    restoresignals();
    return r;
}


/* The following function definitions are used by GNU autoconf to determine
 * which libraries must be linked in with mpatrol on a specific system.
 * They must be globally visible and have to be function symbols rather than
 * data symbols so that AC_CHECK_LIB() works reliably.
 */

#if TARGET == TARGET_WINDOWS || SYSTEM == SYSTEM_CYGWIN
#if FORMAT == FORMAT_PE || DYNLINK == DYNLINK_WINDOWS || MP_LIBRARYSTACK_SUPPORT
MP_API void __mp_libimagehlp(void) {}
#endif /* FORMAT && DYNLINK && MP_LIBRARYSTACK_SUPPORT */
#elif SYSTEM == SYSTEM_HPUX
#if MP_LIBRARYSTACK_SUPPORT
MP_API void __mp_libcl(void) {}
#endif /* MP_LIBRARYSTACK_SUPPORT */
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_TRU64
#if MP_LIBRARYSTACK_SUPPORT
MP_API void __mp_libexc(void) {}
#endif /* MP_LIBRARYSTACK_SUPPORT */
#endif /* TARGET && SYSTEM */
#if (FORMAT == FORMAT_COFF || FORMAT == FORMAT_XCOFF) && SYSTEM != SYSTEM_LYNXOS
MP_API void __mp_libld(void) {}
#elif FORMAT == FORMAT_ELF32 || FORMAT == FORMAT_ELF64
MP_API void __mp_libelf(void) {}
#elif FORMAT == FORMAT_BFD
MP_API void __mp_libbfd(void) {}
MP_API void __mp_libiberty(void) {}
#endif /* FORMAT */


/* The function that is called at every function prologue.
 */

MP_API
void
__cyg_profile_func_enter(void *a, void *p)
{
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    restoresignals();
}


/* The function that is called at every function epilogue.
 */

MP_API
void
__cyg_profile_func_exit(void *a, void *p)
{
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    restoresignals();
}


/* Set the access rights for a block of memory using the checker interface.
 */

MP_API
void
chkr_set_right(void *p, size_t l, unsigned char a)
{
    char *s, *t;
    stackinfo i;
    loginfo v;
    unsigned long u;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details in case we need to report any errors.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* No filename was passed through so attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = AT_MAX;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    if (!__mp_checkrange(&memhead, p, l, &v))
    {
        memhead.fini = 1;
        __mp_abort();
    }
    restoresignals();
}


/* Copy the access rights for a block of memory to another block using the
 * checker interface.
 */

MP_API
void
chkr_copy_bitmap(void *p, void *q, size_t l)
{
    char *s, *t;
    stackinfo i;
    loginfo v;
    unsigned long u;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details in case we need to report any errors.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* No filename was passed through so attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = AT_MAX;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    if (!__mp_checkrange(&memhead, p, l, &v) ||
        !__mp_checkrange(&memhead, q, l, &v))
    {
        memhead.fini = 1;
        __mp_abort();
    }
    restoresignals();
}


/* Check a block of memory using the checker interface.
 */

MP_API
void
chkr_check_addr(void *p, size_t l, unsigned char a)
{
    char *s, *t;
    stackinfo i;
    loginfo v;
    unsigned long u;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details in case we need to report any errors.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* No filename was passed through so attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = AT_MAX;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    if (!__mp_checkrange(&memhead, p, l, &v))
    {
        memhead.fini = 1;
        __mp_abort();
    }
    restoresignals();
}


/* Check a string using the checker interface.
 */

MP_API
void
chkr_check_str(char *p, unsigned char a)
{
    char *s, *t;
    stackinfo i;
    loginfo v;
    size_t l;
    unsigned long u;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details in case we need to report any errors.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* No filename was passed through so attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = AT_MAX;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    if (!__mp_checkstring(&memhead, p, &l, &v, 0))
    {
        memhead.fini = 1;
        __mp_abort();
    }
    restoresignals();
}


/* Check a function pointer using the checker interface.
 */

MP_API
void
chkr_check_exec(void *p)
{
    char *s, *t;
    stackinfo i;
    loginfo v;
    unsigned long u;

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* If the C run-time library has not finished initialising then we cannot
     * initialise the mpatrol library and so we just return.
     */
    if (!crt_initialised())
        return;
#endif /* TARGET */
    savesignals();
    if (!memhead.init)
        __mp_init();
    if (__mp_processid() != memhead.pid)
        __mp_reinit();
    /* Determine the call stack details in case we need to report any errors.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i))
        __mp_getframe(&i);
    /* No filename was passed through so attempt to read any debugging
     * information to determine the source location of the call.
     */
    if ((memhead.recur == 1) && (i.addr != NULL) &&
        __mp_findsource(&memhead.syms, (char *) i.addr - 1, &s, &t, &u))
    {
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READWRITE);
        if (s != NULL)
            s = __mp_addstring(&memhead.syms.strings, s);
        if (t != NULL)
            t = __mp_addstring(&memhead.syms.strings, t);
        if (!(memhead.flags & FLG_NOPROTECT))
            __mp_protectstrtab(&memhead.syms.strings, MA_READONLY);
    }
    v.ltype = LT_MAX;
    v.type = AT_MAX;
    v.func = s;
    v.file = t;
    v.line = u;
    v.stack = &i;
    v.typestr = NULL;
    v.typesize = 0;
    v.logged = 0;
    restoresignals();
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
