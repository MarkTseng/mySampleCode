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
 * Call stacks.  The method for traversing a function call stack is
 * dependent on both the operating system and processor architecture.
 * The most correct way of doing this would be to perform code-reading
 * in order to ascertain the return address for a function.  However,
 * some operating systems provide support functions for doing this.
 */


#include "stack.h"
#include "memory.h"
#include "machine.h"
#include <string.h>
#if !MP_BUILTINSTACK_SUPPORT && !MP_LIBUNWIND_SUPPORT
#if MP_GLIBCBACKTRACE_SUPPORT
#include <execinfo.h>
#elif MP_LIBRARYSTACK_SUPPORT
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_IRIX
#include <exception.h>
#include <ucontext.h>
#elif SYSTEM == SYSTEM_TRU64
#include <excpt.h>
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
#include <setjmp.h>
#endif /* TARGET */
#else /* MP_GLIBCBACKTRACE_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
#if TARGET == TARGET_UNIX
#include <setjmp.h>
#if MP_SIGINFO_SUPPORT
#include <siginfo.h>
#endif /* MP_SIGINFO_SUPPORT */
#if SYSTEM == SYSTEM_DRSNX || SYSTEM == SYSTEM_SOLARIS
#if ARCH == ARCH_SPARC
#include <ucontext.h>
#ifndef R_SP
#define R_SP REG_SP
#endif /* R_SP */
#endif /* ARCH */
#endif /* SYSTEM */
#endif /* TARGET */
#endif /* MP_GLIBCBACKTRACE_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_LIBUNWIND_SUPPORT */


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *stack_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#if MP_BUILTINSTACK_SUPPORT
/* This method of call stack traversal uses two special builtin functions in
 * gcc called __builtin_frame_address() and __builtin_return_address().  Both
 * of these functions take the number of stack frames to traverse as a parameter
 * but this must currently be a constant, hence the reason for all of the
 * following complicated macros, and for the fact that there must currently
 * be a maximum number of stack frames to traverse that is determined at compile
 * time.  However, it may be the case that this method is slightly better than
 * manually traversing the call stack.  Perhaps in the future gcc might allow
 * these functions to accept non-constant parameters...
 */

#define frameaddress(v, n) (v[n] = __builtin_frame_address(n))
#define frameaddress1(v) frameaddress(v, 0)
#define frameaddress2(v) frameaddress1(v) && frameaddress(v, 1)
#define frameaddress3(v) frameaddress2(v) && frameaddress(v, 2)
#define frameaddress4(v) frameaddress3(v) && frameaddress(v, 3)
#define frameaddress5(v) frameaddress4(v) && frameaddress(v, 4)
#define frameaddress6(v) frameaddress5(v) && frameaddress(v, 5)
#define frameaddress7(v) frameaddress6(v) && frameaddress(v, 6)
#define frameaddress8(v) frameaddress7(v) && frameaddress(v, 7)
#define frameaddressn(v, w, n) if (frameaddress ## n(v)) \
                                   w = __builtin_frame_address(n)
#define frameaddresses(v, w, n) frameaddressn(v, w, n)

#define returnaddress(v, n) (v[n] = __builtin_return_address(n))
#define returnaddress1(v) returnaddress(v, 0)
#define returnaddress2(v) returnaddress1(v) && returnaddress(v, 1)
#define returnaddress3(v) returnaddress2(v) && returnaddress(v, 2)
#define returnaddress4(v) returnaddress3(v) && returnaddress(v, 3)
#define returnaddress5(v) returnaddress4(v) && returnaddress(v, 4)
#define returnaddress6(v) returnaddress5(v) && returnaddress(v, 5)
#define returnaddress7(v) returnaddress6(v) && returnaddress(v, 6)
#define returnaddress8(v) returnaddress7(v) && returnaddress(v, 7)
#define returnaddressn(v, w, n) if (returnaddress ## n(v)) \
                                    w = __builtin_return_address(n)
#define returnaddresses(v, w, n) returnaddressn(v, w, n)

#if MP_MAXSTACK > 8
#error not enough frameaddress() and returnaddress() macros
#endif /* MP_MAXSTACK */
#elif !MP_GLIBCBACKTRACE_SUPPORT && !MP_LIBUNWIND_SUPPORT && \
      !MP_LIBRARYSTACK_SUPPORT
#if TARGET == TARGET_UNIX && ARCH == ARCH_MIPS
/* These macros are used by the unwind() function for setting flags when
 * certain instructions are seen.
 */

#define RA_OFFSET 1 /* return address offset has been set */
#define SP_OFFSET 2 /* stack pointer offset has been set */
#define SP_LOWER  4 /* lower part of stack pointer offset has been set */
#define SP_UPPER  8 /* upper part of stack pointer offset has been set */
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if !MP_BUILTINSTACK_SUPPORT && !MP_GLIBCBACKTRACE_SUPPORT && \
    TARGET == TARGET_UNIX
#if MP_LIBUNWIND_SUPPORT
/* libunwind itself does some memory allocations, which will then cause infinite
 * recursion.  This variable acts as a per-thread lock to prevent that from
 * happening.
 */

static __thread int recursive;
#elif MP_LIBRARYSTACK_SUPPORT
#if SYSTEM == SYSTEM_HPUX
/* The following function is defined in the HP/UX traceback library (libcl).
 */

int U_get_previous_frame(frameinfo *, frameinfo *);
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_TRU64
/* The unwind() function in the IRIX and Tru64 exception-handling libraries
 * (libexc) may call malloc() and several memory operation functions, so we
 * need to guard against this by preventing recursive calls.
 */

static unsigned char recursive;

#if SYSTEM == SYSTEM_TRU64
MP_API char *__mp_symbol(void *);
#endif /* SYSTEM */
#endif /* SYSTEM */
#else /* MP_LIBUNWIND_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
static jmp_buf environment;
#if MP_SIGINFO_SUPPORT
static struct sigaction bushandler;
static struct sigaction segvhandler;
#else /* MP_SIGINFO_SUPPORT */
static void (*bushandler)(int);
static void (*segvhandler)(int);
#endif /* MP_SIGINFO_SUPPORT */
#endif /* MP_LIBUNWIND_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && TARGET */


/* Determine the stack direction on this system.
 */

MP_GLOBAL
int
__mp_stackdirection(void *p)
{
    unsigned long n;

    n = (unsigned long) &p;
    if (p == NULL)
        return __mp_stackdirection(&n);
    else if (&n < (unsigned long *) p)
        return -1;
    else
        return 1;
}


/* Initialise the fields of a stackinfo structure.
 */

MP_GLOBAL
void
__mp_newframe(stackinfo *s, void *f)
{
    s->frame = s->addr = NULL;
#if MP_BUILTINSTACK_SUPPORT || MP_GLIBCBACKTRACE_SUPPORT
    for (s->index = 0; s->index < MP_MAXSTACK; s->index++)
        s->frames[s->index] = s->addrs[s->index] = NULL;
#if MP_GLIBCBACKTRACE_SUPPORT
    s->count = 0;
#endif /* MP_GLIBCBACKTRACE_SUPPORT */
    s->index = 0;
#elif MP_LIBUNWIND_SUPPORT
    __mp_memset(&s->context, 0, sizeof(unw_context_t));
    __mp_memset(&s->cursor, 0, sizeof(unw_cursor_t));
#elif MP_LIBRARYSTACK_SUPPORT
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_HPUX
    __mp_memset(&s->next, 0, sizeof(frameinfo));
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_TRU64
    __mp_memset(&s->next, 0, sizeof(struct sigcontext));
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
    __mp_memset(&s->next, 0, sizeof(STACKFRAME));
#endif /* TARGET */
#else /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */
#if TARGET == TARGET_UNIX && ARCH == ARCH_MIPS
    s->next.sp = s->next.ra = 0;
#else /* TARGET && ARCH */
    s->next = NULL;
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */
    s->first = f;
}


#if !MP_BUILTINSTACK_SUPPORT && !MP_GLIBCBACKTRACE_SUPPORT && \
    !MP_LIBUNWIND_SUPPORT && !MP_LIBRARYSTACK_SUPPORT && TARGET == TARGET_UNIX
/* Handles any signals that result from illegal memory accesses whilst
 * traversing the call stack.
 */

static
void
stackhandler(int s)
{
    longjmp(environment, 1);
}
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */


#if !MP_BUILTINSTACK_SUPPORT && !MP_GLIBCBACKTRACE_SUPPORT && \
    !MP_LIBUNWIND_SUPPORT && !MP_LIBRARYSTACK_SUPPORT
#if (TARGET == TARGET_UNIX && (ARCH == ARCH_IX86 || ARCH == ARCH_M68K || \
      ARCH == ARCH_M88K || ARCH == ARCH_POWER || ARCH == ARCH_POWERPC || \
      ARCH == ARCH_SPARC)) || ((TARGET == TARGET_WINDOWS || \
      TARGET == TARGET_NETWARE) && ARCH == ARCH_IX86)
/* Obtain the return address for the specified stack frame handle.
 */

static
unsigned long *
getaddr(unsigned long *p)
{
    unsigned long *a;

    /* This function relies heavily on the stack frame format of supported
     * OS / processor combinations.  A better way to determine the return
     * address would be to perform code reading, but on CISC processors this
     * could be a nightmare.
     */
#if ARCH == ARCH_IX86 || ARCH == ARCH_M68K || ARCH == ARCH_M88K
    a = (unsigned long *) *(p + 1);
#elif ARCH == ARCH_POWER || ARCH == ARCH_POWERPC
    a = (unsigned long *) *(p + 2);
#elif ARCH == ARCH_SPARC
    if (*p == 0)
        a = NULL;
#if ENVIRON == ENVIRON_64
    else if (a = (unsigned long *) *((unsigned long *) (*p + 0x7FF) + 15))
#else /* ENVIRON */
    else if (a = (unsigned long *) *((unsigned long *) *p + 15))
#endif /* ENVIRON */
        a += 2;
#endif /* ARCH */
    return a;
}
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */


#if !MP_BUILTINSTACK_SUPPORT && !MP_GLIBCBACKTRACE_SUPPORT && \
    !MP_LIBUNWIND_SUPPORT && !MP_LIBRARYSTACK_SUPPORT
#if TARGET == TARGET_UNIX && ARCH == ARCH_MIPS
/* Determine the stack pointer and return address of the previous stack frame
 * by performing code reading.
 */

static
int
unwind(frameinfo *f)
{
    long p, s;
    unsigned long a, i, q;
    unsigned short l, u;

    s = -1;
    p = 0;
    q = 0xFFFFFFFF;
    l = u = 0;
    a = 0;
    /* Determine the current stack pointer and return address if we are
     * initiating call stack traversal.
     */
    if (f->ra == 0)
    {
        f->sp = __mp_stackpointer();
        f->ra = __mp_returnaddress();
    }
    /* Search for the return address offset in the stack frame.
     */
    while (!((a & RA_OFFSET) && (a & SP_OFFSET)) && (f->ra < q))
    {
        i = *((unsigned long *) f->ra);
        if (i == 0x03E00008)
        {
            /* jr ra */
            q = f->ra + 8;
        }
        else if (i == 0x03A1E821)
        {
            /* addu sp,sp,at */
            s = 0;
            a |= SP_OFFSET;
        }
        else
            switch (i >> 16)
            {
              case 0x27BD:
                /* addiu sp,sp,## */
                s = i & 0xFFFF;
                a |= SP_OFFSET;
                break;
              case 0x3401:
                /* ori at,zero,## */
                l = i & 0xFFFF;
                u = 0;
                a |= SP_LOWER;
                break;
              case 0x3421:
                /* ori at,at,## */
                l = i & 0xFFFF;
                a |= SP_LOWER;
                break;
              case 0x3C01:
                /* lui at,## */
                l = 0;
                u = i & 0xFFFF;
                a |= SP_UPPER;
                break;
              case 0x8FBF:
                /* lw ra,##(sp) */
                p = i & 0xFFFF;
                a |= RA_OFFSET;
                break;
            }
        f->ra += 4;
    }
    if ((s == 0) && ((a & SP_LOWER) || (a & SP_UPPER)))
        s = (u << 16) | l;
    if ((s > 0) && (i = ((unsigned long *) f->sp)[p >> 2]) &&
        ((*((unsigned long *) (i - 8)) == 0x0320F809) ||
         (*((unsigned long *) (i - 8)) >> 16 == 0x0C10)))
    {
        /* jalr ra,t9 or jal ## */
        f->sp += s;
        f->ra = i;
        return 1;
    }
    f->sp = f->ra = 0;
    return 0;
}
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */


#if !MP_BUILTINSTACK_SUPPORT && !MP_GLIBCBACKTRACE_SUPPORT && \
    !MP_LIBUNWIND_SUPPORT && !MP_LIBRARYSTACK_SUPPORT && TARGET == TARGET_UNIX
#if ARCH == ARCH_SPARC
/* Return a handle for the frame pointer at the current point in execution.
 */

static
unsigned long *
getframe(void)
{
#if SYSTEM == SYSTEM_DRSNX || SYSTEM == SYSTEM_SOLARIS
    ucontext_t c;
#endif /* SYSTEM */
    unsigned long a;

#if SYSTEM == SYSTEM_DRSNX || SYSTEM == SYSTEM_SOLARIS
    if (getcontext(&c) == -1)
        return NULL;
    a = c.uc_mcontext.gregs[R_SP];
#else /* SYSTEM */
    a = __mp_stackpointer();
#endif /* SYSTEM */
#if ENVIRON == ENVIRON_64
    return (unsigned long *) (a + 0x7FF) + 14;
#else /* ENVIRON */
    return (unsigned long *) a + 14;
#endif /* ENVIRON */
}
#endif /* ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */


/* Return a handle for the stack frame at the current point in execution
 * or the next stack frame in the call stack.
 */

MP_GLOBAL
int
__mp_getframe(stackinfo *p)
{
#if MP_BUILTINSTACK_SUPPORT
    void *f;
#elif MP_GLIBCBACKTRACE_SUPPORT
    int d, i;
#elif MP_LIBUNWIND_SUPPORT
    unw_word_t f;
#elif MP_LIBRARYSTACK_SUPPORT
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_HPUX
    frameinfo f;
#elif SYSTEM == SYSTEM_TRU64
    char *s;
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
    jmp_buf j;
#endif /* TARGET */
#else /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */
#if MP_SIGINFO_SUPPORT
    struct sigaction i;
#endif /* MP_SIGINFO_SUPPORT */
#if (TARGET == TARGET_UNIX && (ARCH == ARCH_IX86 || ARCH == ARCH_M68K || \
      ARCH == ARCH_M88K || ARCH == ARCH_POWER || ARCH == ARCH_POWERPC || \
      ARCH == ARCH_SPARC)) || ((TARGET == TARGET_WINDOWS || \
      TARGET == TARGET_NETWARE) && ARCH == ARCH_IX86)
    unsigned long *f;
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */
    int r;

    r = 0;
#if MP_BUILTINSTACK_SUPPORT
    if (p->index == 0)
    {
        /* We use the macros defined above to fill in the arrays of frame
         * pointers and return addresses.  These macros rely on the fact that
         * if there are no more frames in the call stack then the builtin
         * functions will return NULL.  If this is not the case then the
         * library will crash.
         */
        frameaddresses(p->frames, f, MP_MAXSTACK);
        returnaddresses(p->addrs, f, MP_MAXSTACK);
    }
    if ((p->index++ < MP_MAXSTACK) && (p->frame = p->frames[p->index - 1]))
    {
        p->addr = p->addrs[p->index - 1];
        r = 1;
    }
    else
    {
        p->frame = NULL;
        p->addr = NULL;
        p->index = MP_MAXSTACK;
    }
#elif MP_GLIBCBACKTRACE_SUPPORT
    if (p->index == 0)
    {
        /* Obtain up to MP_MAXSTACK return addresses for the calling stack
         * frames.
         */
        p->count = backtrace(p->addrs, MP_MAXSTACK);
        /* Spoof the frame pointers by taking the address of a local variable
         * in this function and adding a fixed offset to that address for each
         * subsequent stack frame.
         */
        d = __mp_stackdirection(NULL);
        for (i = 0; i < p->count; i++)
            p->frames[i] = (char *) &d - (i * d * 64);
    }
    if (p->index++ < p->count)
    {
        p->frame = p->frames[p->index - 1];
        p->addr = p->addrs[p->index - 1];
        r = 1;
    }
    else
    {
        p->frame = NULL;
        p->addr = NULL;
        p->index = MP_MAXSTACK;
    }
#elif MP_LIBUNWIND_SUPPORT
    /* The libunwind library provides support for traversing function call
     * stack frames in a machine-independent way.  Instead of a frame pointer
     * we just use the stack pointer returned by unw_get_reg().
     */
    if (!recursive)
    {
        recursive = 1;
        if (p->frame == NULL)
        {
            unw_getcontext(&p->context);
            unw_init_local(&p->cursor, &p->context);
        }
        if (unw_step(&p->cursor) > 0)
        {
            unw_get_reg(&p->cursor, UNW_REG_SP, &f);
            p->frame = (void *) f;
            unw_get_reg(&p->cursor, UNW_REG_IP, &f);
            p->addr = (void *) f;
            r = 1;
        }
        else
        {
            p->frame = NULL;
            p->addr = NULL;
        }
        recursive = 0;
    }
#elif MP_LIBRARYSTACK_SUPPORT
    /* HP/UX, IRIX, Tru64 and Windows platforms provide a library for
     * traversing function call stack frames since the stack frame format
     * does not necessarily preserve frame pointers.  On HP/UX this is done
     * via a special section which can be read by debuggers.
     */
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_HPUX
    if (p->frame == NULL)
    {
        __mp_frameinfo(&p->next);
        if (U_get_previous_frame(&p->next, &f) == 0)
        {
            p->next.size = f.size;
            p->next.sp = f.sp;
            p->next.ps = f.ps;
            p->next.pc = f.pc;
            p->next.dp = f.dp;
        }
        else
            __mp_memset(&p->next, 0, sizeof(frameinfo));
    }
    if (p->next.pc && (U_get_previous_frame(&p->next, &f) == 0))
    {
        p->frame = (void *) p->next.sp;
        p->addr = (void *) (p->next.pc & ~3);
        p->next.size = f.size;
        p->next.sp = f.sp;
        p->next.ps = f.ps;
        p->next.pc = f.pc;
        p->next.dp = f.dp;
        r = 1;
    }
    else
    {
        p->frame = NULL;
        p->addr = NULL;
    }
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_TRU64
    /* On IRIX and Tru64, the unwind() function may call malloc(), free() and
     * some memory operation functions every time it is invoked.  Despite the
     * fact that we guard against recursion here, it may slow down execution to
     * an unbearable pace, so it might be an idea to remove malloc.c from the
     * mpatrol library if you have the option of recompiling all of your
     * sources to include mpatrol.h.
     */
    if (!recursive)
    {
        recursive = 1;
        if (p->frame == NULL)
        {
#if SYSTEM == SYSTEM_IRIX
            exc_setjmp(&p->next);
#elif SYSTEM == SYSTEM_TRU64
            exc_capture_context(&p->next);
#endif /* SYSTEM */
            unwind(&p->next, NULL);
        }
        if (p->next.sc_pc != 0)
        {
            /* On IRIX, the sigcontext structure stores registers in 64-bit
             * format so we must be careful when converting them to 32-bit
             * quantities.
             */
#if SYSTEM == SYSTEM_IRIX
            p->frame = (void *) p->next.sc_regs[CXT_SP];
#elif SYSTEM == SYSTEM_TRU64
            p->frame = (void *) p->next.sc_regs[R_SP];
#endif /* SYSTEM */
            p->addr = (void *) p->next.sc_pc;
#if SYSTEM == SYSTEM_TRU64
            /* On Tru64 we cannot reliably unwind the stack from file scope
             * initialisation or finalisation functions, or from exception-
             * handling support functions.  Unfortunately, this means we must
             * look at the names of the calling functions, which is likely to
             * fail if the executable file has been stripped.
             */
            if (((s = __mp_symbol(p->addr)) == NULL) ||
                ((strncmp(s, "__INIT_00_add_", 14) != 0) &&
                 (strncmp(s, "__FINI_00_remove_", 17) != 0) &&
                 (strncmp(s, "__exc_add_", 10) != 0) &&
                 (strncmp(s, "__exc_remove_", 13) != 0)))
            {
#endif /* SYSTEM */
                unwind(&p->next, NULL);
                r = 1;
#if SYSTEM == SYSTEM_TRU64
            }
            else
            {
                p->frame = NULL;
                p->addr = NULL;
            }
#endif /* SYSTEM */
        }
        else
        {
            p->frame = NULL;
            p->addr = NULL;
        }
        recursive = 0;
    }
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
    if (p->frame == NULL)
    {
        if (p->first == NULL)
        {
            setjmp(j);
            p->next.AddrPC.Offset = (unsigned long) j[5];
            p->next.AddrFrame.Offset = (unsigned long) j[0];
            p->next.AddrStack.Offset = (unsigned long) j[4];
        }
        else
        {
            p->next.AddrPC.Offset = ((CONTEXT *) p->first)->Eip;
            p->next.AddrFrame.Offset = ((CONTEXT *) p->first)->Ebp;
            p->next.AddrStack.Offset = ((CONTEXT *) p->first)->Esp;
        }
        p->next.AddrPC.Mode = AddrModeFlat;
        p->next.AddrFrame.Mode = AddrModeFlat;
        p->next.AddrStack.Mode = AddrModeFlat;
    }
    if (StackWalk(IMAGE_FILE_MACHINE_I386, GetCurrentProcess(),
        GetCurrentThread(), &p->next, NULL, NULL, SymFunctionTableAccess,
        SymGetModuleBase, NULL) && p->next.AddrReturn.Offset)
    {
        p->frame = (void *) p->next.AddrFrame.Offset;
        p->addr = (void *) p->next.AddrReturn.Offset;
        r = 1;
    }
    else
    {
        p->frame = NULL;
        p->addr = NULL;
        __mp_memset(&p->next, 0, sizeof(STACKFRAME));
    }
#endif /* TARGET */
#else /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */
#if (TARGET == TARGET_UNIX && (ARCH == ARCH_IX86 || ARCH == ARCH_M68K || \
      ARCH == ARCH_M88K || ARCH == ARCH_POWER || ARCH == ARCH_POWERPC || \
      ARCH == ARCH_SPARC)) || ((TARGET == TARGET_WINDOWS || \
      TARGET == TARGET_NETWARE) && ARCH == ARCH_IX86)
    /* This section is not complete in any way for the OS / processor
     * combinations it supports, as it is intended to be as portable as possible
     * without writing in assembler.  In particular, optimised code is likely
     * to cause major problems for stack traversal on some platforms.
     */
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
    i.sa_flags = 0;
    (void *) i.sa_handler = (void *) stackhandler;
    sigfillset(&i.sa_mask);
    sigaction(SIGBUS, &i, &bushandler);
    sigaction(SIGSEGV, &i, &segvhandler);
#else /* MP_SIGINFO_SUPPORT */
    bushandler = signal(SIGBUS, stackhandler);
    segvhandler = signal(SIGSEGV, stackhandler);
#endif /* MP_SIGINFO_SUPPORT */
    if (setjmp(environment))
        __mp_newframe(p, p->first);
    else
#endif /* TARGET */
    {
        if (p->frame == NULL)
            if (p->first == NULL)
#if ARCH == ARCH_IX86 || ARCH == ARCH_M68K
                f = (unsigned long *) &p - 2;
#elif ARCH == ARCH_M88K
                f = (unsigned long *) &p - 4;
#elif ARCH == ARCH_POWER || ARCH == ARCH_POWERPC
                f = (unsigned long *) &p - 6;
#elif ARCH == ARCH_SPARC
                f = getframe();
#endif /* ARCH */
            else
                f = (unsigned long *) p->first;
        else
            f = (unsigned long *) p->next;
        if (p->frame = f)
        {
            p->addr = getaddr(f);
            /* We cache the next frame pointer in the call stack since on some
             * systems it may be overwritten by another call.
             */
#if ARCH == ARCH_IX86 || ARCH == ARCH_M68K || ARCH == ARCH_M88K || \
    ARCH == ARCH_POWER || ARCH == ARCH_POWERPC
#if SYSTEM == SYSTEM_LYNXOS
            if (!getaddr((unsigned long *) *f))
                p->next = NULL;
            else
#endif /* SYSTEM */
                p->next = (void *) *f;
#elif ARCH == ARCH_SPARC
            if (p->addr == NULL)
                p->next = NULL;
            else
#if ENVIRON == ENVIRON_64
                p->next = (void *) ((unsigned long *) (*f + 0x7FF) + 14);
#else /* ENVIRON */
                p->next = (void *) ((unsigned long *) *f + 14);
#endif /* ENVIRON */
#endif /* ARCH */
            r = 1;
        }
    }
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
    sigaction(SIGBUS, &bushandler, NULL);
    sigaction(SIGSEGV, &segvhandler, NULL);
#else /* MP_SIGINFO_SUPPORT */
    signal(SIGBUS, bushandler);
    signal(SIGSEGV, segvhandler);
#endif /* MP_SIGINFO_SUPPORT */
#endif /* TARGET */
#elif TARGET == TARGET_UNIX && ARCH == ARCH_MIPS
    /* For the MIPS architecture we perform code reading to determine the
     * frame pointers and the return addresses.
     */
#if MP_SIGINFO_SUPPORT
    i.sa_flags = 0;
    (void *) i.sa_handler = (void *) stackhandler;
    sigfillset(&i.sa_mask);
    sigaction(SIGBUS, &i, &bushandler);
    sigaction(SIGSEGV, &i, &segvhandler);
#else /* MP_SIGINFO_SUPPORT */
    bushandler = signal(SIGBUS, stackhandler);
    segvhandler = signal(SIGSEGV, stackhandler);
#endif /* MP_SIGINFO_SUPPORT */
    if (setjmp(environment))
        __mp_newframe(p, p->first);
    else
    {
        if (p->frame == NULL)
            unwind(&p->next);
        if ((p->next.ra) && unwind(&p->next))
        {
            p->frame = (void *) p->next.sp;
            p->addr = (void *) (p->next.ra - 8);
            r = 1;
        }
        else
        {
            p->frame = NULL;
            p->addr = NULL;
        }
    }
#if MP_SIGINFO_SUPPORT
    sigaction(SIGBUS, &bushandler, NULL);
    sigaction(SIGSEGV, &segvhandler, NULL);
#else /* MP_SIGINFO_SUPPORT */
    signal(SIGBUS, bushandler);
    signal(SIGSEGV, segvhandler);
#endif /* MP_SIGINFO_SUPPORT */
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_GLIBCBACKTRACE_SUPPORT && ... */
    return r;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
