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
 * Signal-handling.  The setting of signal-handlers and their actions
 * is controlled from this module.
 */


#include "signals.h"
#include "diag.h"
#if MP_THREADS_SUPPORT
#include "mutex.h"
#endif /* MP_THREADS_SUPPORT */
#include "inter.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
#include <siginfo.h>
#endif /* MP_SIGINFO_SUPPORT */
#elif TARGET == TARGET_WINDOWS
#include <windows.h>
#endif /* TARGET */


#if MP_IDENT_SUPPORT
#ident "$Id: signals.c,v 1.26 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *signals_id = "$Id: signals.c,v 1.26 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The type of the parameter that is passed to the signal() function.
 */

typedef void (*handlerfunction)(int);


#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
/* Handles any signals that result from illegal memory accesses.
 */

#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
static
void
signalhandler(int s, siginfo_t *n, void *p)
#else /* MP_SIGINFO_SUPPORT */
#if SYSTEM == SYSTEM_AIX || SYSTEM == SYSTEM_FREEBSD || \
    SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_LINUX || \
    SYSTEM == SYSTEM_LYNXOS || SYSTEM == SYSTEM_NETBSD || \
    SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
#if SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
    SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
static
void
signalhandler(int s, int c, struct sigcontext *n, void *f)
#elif SYSTEM == SYSTEM_LINUX && ARCH == ARCH_IX86
static
void
signalhandler(int s, struct sigcontext n)
#else /* SYSTEM && ARCH */
static
void
signalhandler(int s, int c, struct sigcontext *n)
#endif /* SYSTEM && ARCH */
#else /* SYSTEM */
static
void
signalhandler(int s)
#endif /* SYSTEM */
#endif /* MP_SIGINFO_SUPPORT */
#elif TARGET == TARGET_WINDOWS
static
long __stdcall
signalhandler(EXCEPTION_POINTERS *e)
#endif /* TARGET */
{
    infohead *h;
    allocnode *t;
    void *a;
#if TARGET == TARGET_WINDOWS
    EXCEPTION_RECORD *r;
#endif /* TARGET */
    stackinfo i;

#if TARGET == TARGET_WINDOWS
    /* This exception handler will catch all kinds of exceptions, but the
     * only one we are interested in is an access violation, so we return
     * control to the system to decide what to do in all other cases.
     */
    r = e->ExceptionRecord;
    if (r->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
        return EXCEPTION_CONTINUE_SEARCH;
#endif /* TARGET */
#if MP_THREADS_SUPPORT
    /* Attempt to lock the library data structures if we are thread-safe.
     * Hopefully this will never result in a deadlock since we should block
     * until all other threads have finished manipulating memory allocations.
     */
    __mp_lockmutex(MT_MAIN);
#endif /* MP_THREADS_SUPPORT */
    h = __mp_memhead();
    h->recur++;
    __mp_printsummary(h);
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT && MP_WATCH_SUPPORT
    /* If we received a TRAP signal then we should only say that it is an
     * illegal memory access if it is a watch point error.  Otherwise we
     * just do the default and abort the process.
     */
    if ((s == SIGTRAP) && (n != NULL) && (n->si_code != TRAP_RWATCH) &&
        (n->si_code != TRAP_WWATCH) && (n->si_code != TRAP_XWATCH))
    {
        h->fini = 1;
        __mp_abort();
    }
#endif /* MP_SIGINFO_SUPPORT && MP_WATCH_SUPPORT */
#endif /* TARGET */
    __mp_diag("\n");
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT || SYSTEM == SYSTEM_AIX || SYSTEM == SYSTEM_FREEBSD || \
    SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_LINUX || \
    SYSTEM == SYSTEM_LYNXOS || SYSTEM == SYSTEM_NETBSD || \
    SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
#if MP_SIGINFO_SUPPORT
    if ((n != NULL) && (n->si_code > 0))
    {
        /* With the siginfo_t structure we can determine which address
         * caused the illegal memory access.  This is not always guaranteed
         * to be the exact byte location, but it is at least guaranteed that
         * it will be on the same page which will suffice for our purposes.
         */
        a = (void *) n->si_addr;
#else /* MP_SIGINFO_SUPPORT */
#if SYSTEM == SYSTEM_LINUX && ARCH == ARCH_IX86
    if (&n != NULL)
#else /* SYSTEM && ARCH */
    if (n != NULL)
#endif /* SYSTEM && ARCH */
    {
        /* With systems that do not contain support for passing the siginfo_t
         * structure to a signal handler we need to use some other form of
         * trickery.  Luckily, on most UNIX systems a sigcontext parameter
         * is passed on the stack to all signal handlers which we can then
         * use to decode the address that caused the illegal memory access.
         */
        a = NULL;
#if SYSTEM == SYSTEM_AIX
        a = (void *) n->sc_jmpbuf.jmp_context.o_vaddr;
#elif SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
      SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
        a = f;
#elif SYSTEM == SYSTEM_IRIX
        a = (void *) ((long) n->sc_badvaddr);
#elif SYSTEM == SYSTEM_LINUX
#if ARCH == ARCH_ALPHA
        a = (char *) n->sc_regs[(*((unsigned int *) n->sc_pc) >> 16) & 0x1F] +
            ((*((unsigned int *) n->sc_pc) << 16) >> 16);
#elif ARCH == ARCH_IX86
        a = (void *) n.cr2;
#elif ARCH == ARCH_M68K
        switch ((n->sc_formatvec >> 12) & 0x0F)
        {
          case 4:
            /* 68060 */
            a = (void *) ((unsigned long *) (n + 1))[0];
            break;
          case 7:
            /* 68040 */
            a = (void *) ((unsigned long *) (n + 1))[3];
            break;
          case 10:
          case 11:
          default:
            /* 68020/68030 */
            a = (void *) ((unsigned long *) (n + 1))[2];
            break;
        }
#endif /* ARCH */
#elif SYSTEM == SYSTEM_LYNXOS
#if ARCH == ARCH_IX86
        a = (void *) n->eax;
#elif ARCH == ARCH_POWER || ARCH == ARCH_POWERPC
        if (n->fpscr < 0x4000)
            a = (void *) n->fpscr;
        else
            a = (void *) (n->fpscr - ((n->fpscr & 7) << 1) + 7);
#endif /* ARCH */
#endif /* SYSTEM */
#endif /* MP_SIGINFO_SUPPORT */
        __mp_error(ET_ILLMEM, AT_MAX, NULL, 0, "illegal memory access at "
                   "address " MP_POINTER, a);
        if (t = __mp_findnode(&h->alloc, a, 1))
            if (t->info != NULL)
                __mp_printalloc(&h->syms, t);
            else
            {
                __mp_diag("    within free block " MP_POINTER " (", t->block);
                __mp_printsize(t->size);
                __mp_diag(")\n");
            }
        else
            __mp_diag("    " MP_POINTER " not in heap\n", a);
    }
    else
#endif /* MP_SIGINFO_SUPPORT && SYSTEM */
        __mp_error(ET_ILLMEM, AT_MAX, NULL, 0, "illegal memory access");
    /* Obtain the call stack so that we can tell where the illegal memory
     * access came from.  This relies on the assumption that the stack area
     * for the signal handler is the same as the stack area for the main
     * process.
     */
    __mp_newframe(&i, NULL);
    if (__mp_getframe(&i) && __mp_getframe(&i) && __mp_getframe(&i))
    {
        __mp_diag("\n    call stack\n");
        __mp_printstack(&h->syms, &i);
    }
#elif TARGET == TARGET_WINDOWS
    /* The exception information provided for access violations allows us to
     * determine the address of the violation and whether an attempt was made
     * to read to or write from that address.
     */
    a = (void *) r->ExceptionInformation[1];
    if (r->ExceptionInformation[0])
        __mp_error(ET_ILLMEM, AT_MAX, NULL, 0, "illegal memory write at "
                   "address " MP_POINTER, a);
    else
        __mp_error(ET_ILLMEM, AT_MAX, NULL, 0, "illegal memory read at "
                   "address " MP_POINTER, a);
    if (t = __mp_findnode(&h->alloc, a, 1))
        if (t->info != NULL)
            __mp_printalloc(&h->syms, t);
        else
        {
            __mp_diag("    within free block " MP_POINTER " (", t->block);
            __mp_printsize(t->size);
            __mp_diag(")\n");
        }
    else
        __mp_diag("    " MP_POINTER " not in heap\n", a);
    /* Unfortunately, the call stack where the access violation came from
     * is not linked to the stack frame of this function.  Therefore, we
     * need to invoke the __mp_newframe() function with the proper frame
     * pointer and explicitly display the first frame as well.
     */
#if !MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT
    __mp_newframe(&i, (void *) e->ContextRecord);
#else /* MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
    __mp_newframe(&i, (void *) e->ContextRecord->Ebp);
#endif /* MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
    if (__mp_getframe(&i))
    {
        a = (void *) e->ContextRecord->Eip;
        __mp_diag("\n    call stack\n");
        __mp_diag("\t" MP_POINTER " ", a);
        __mp_printsymbol(&h->syms, a);
        __mp_diag("\n");
        __mp_printstack(&h->syms, &i);
    }
#endif /* TARGET */
    h->fini = 1;
    __mp_abort();
#if TARGET == TARGET_WINDOWS
    /* This line of code will not be executed due to the call to __mp_abort()
     * on the previous line, but exists to suppress compiler warnings.
     */
    return EXCEPTION_CONTINUE_SEARCH;
#endif /* TARGET */
}
#endif /* TARGET */


/* Initialise a sighead structure.
 */

MP_GLOBAL
void
__mp_initsignals(sighead *s)
{
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
    struct sigaction i;
#endif /* MP_SIGINFO_SUPPORT */
#endif /* TARGET */

#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
    i.sa_flags = SA_SIGINFO;
    (void *) i.sa_handler = (void *) signalhandler;
    sigfillset(&i.sa_mask);
    sigaction(SIGBUS, &i, NULL);
    sigaction(SIGSEGV, &i, NULL);
#if MP_WATCH_SUPPORT
    sigaction(SIGTRAP, &i, NULL);
#endif /* MP_WATCH_SUPPORT */
#else /* MP_SIGINFO_SUPPORT */
    signal(SIGBUS, (handlerfunction) signalhandler);
    signal(SIGSEGV, (handlerfunction) signalhandler);
#if MP_WATCH_SUPPORT
    signal(SIGTRAP, (handlerfunction) signalhandler);
#endif /* MP_WATCH_SUPPORT */
#endif /* MP_SIGINFO_SUPPORT */
#elif TARGET == TARGET_WINDOWS
    SetUnhandledExceptionFilter(signalhandler);
#endif /* TARGET */
    s->sigint = s->sigterm = NULL;
    s->saved = 0;
}


/* Save the current signal handlers and set them to ignore.
 */

MP_GLOBAL
void
__mp_savesignals(sighead *s)
{
    s->sigint = signal(SIGINT, SIG_IGN);
    s->sigterm = signal(SIGTERM, SIG_IGN);
    s->saved = 1;
}


/* Restore the previous signal handlers.
 */

MP_GLOBAL
void
__mp_restoresignals(sighead *s)
{
    if (s->saved)
    {
        signal(SIGINT, s->sigint);
        signal(SIGTERM, s->sigterm);
        s->saved = 0;
    }
}


/* Send the current process an ABORT signal.
 */

MP_GLOBAL
void
__mp_abort(void)
{
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    /* Send the current process an ABORT signal for use in a debugger.
     * Used on systems where this is supported.
     */
    fflush(NULL);
    abort();
#else /* TARGET */
    /* Terminate the current process with a failure exit code.
     * Used on systems where memory will not be automatically returned
     * back to the system on process termination.
     */
    exit(EXIT_FAILURE);
#endif /* TARGET */
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
