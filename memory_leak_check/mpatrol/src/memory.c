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
 * Memory handling.  All memory access or handling routines that directly
 * access the system memory are interfaced from this module.  For UNIX SVR4
 * systems, a useful reference book on operating system memory management is
 * The Magic Garden Explained, First Edition by Goodheart and Cox (Prentice
 * Hall, 1994, ISBN 0-130-98138-9).
 */


#include "memory.h"
#include "stack.h"
#include "utils.h"
#include <stdio.h>
#include <errno.h>
#include <limits.h>
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_LYNXOS
#ifndef POSIX4_D14_MEMCTL
#define POSIX4_D14_MEMCTL 1
#endif /* POSIX4_D14_MEMCTL */
#elif SYSTEM == SYSTEM_SUNOS
#ifndef _POSIX_ARG_MAX
#define _POSIX_ARG_MAX 4096
#endif /* _POSIX_ARG_MAX */
#endif /* SYSTEM */
#include <setjmp.h>
#include <signal.h>
#if MP_SIGINFO_SUPPORT
#include <siginfo.h>
#endif /* MP_SIGINFO_SUPPORT */
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#if SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
    SYSTEM == SYSTEM_OPENBSD
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif /* MAP_ANONYMOUS */
#endif /* SYSTEM */
#if MP_WATCH_SUPPORT
#if SYSTEM == SYSTEM_SOLARIS
#include <procfs.h>
#else /* SYSTEM */
#include <sys/procfs.h>
#endif /* SYSTEM */
#endif /* MP_WATCH_SUPPORT */
#elif TARGET == TARGET_AMIGA
#include <proto/dos.h>
#include <proto/exec.h>
#include <exec/memory.h>
#elif TARGET == TARGET_WINDOWS
#include <windows.h>
#include <winbase.h>
#include <process.h>
#elif TARGET == TARGET_NETWARE
#include <nwthread.h>
#include <nks/memory.h>
#endif /* TARGET */


#if MP_IDENT_SUPPORT
#ident "$Id: memory.c,v 1.59 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *memory_id = "$Id: memory.c,v 1.59 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#if MP_WATCH_SUPPORT
/* This structure is used to simplify the building of the watch command before
 * it is written to the control file of the /proc filesystem.
 */

typedef struct watchcmd
{
    long cmd;       /* the command to set a watch point */
    prwatch_t data; /* details of addresses to watch */
}
watchcmd;
#endif /* MP_WATCH_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if MP_ARRAY_SUPPORT
/* The static memory array used to implement a simulated heap.  This is
 * most likely to be zero-initialised at the beginning of program execution,
 * but this should not be relied upon as the memory it contains will be reused.
 */

static char memoryarray[MP_ARRAY_SIZE];


/* The current number of bytes that have been allocated from the static memory
 * array.  This will never be allowed to increase beyond MP_ARRAY_SIZE.
 */

static size_t memorysize;
#endif /* MP_ARRAY_SUPPORT */


#if TARGET == TARGET_UNIX
static jmp_buf memorystate;
#if MP_SIGINFO_SUPPORT
static struct sigaction membushandler;
static struct sigaction memsegvhandler;
#else /* MP_SIGINFO_SUPPORT */
static void (*membushandler)(int);
static void (*memsegvhandler)(int);
#endif /* MP_SIGINFO_SUPPORT */
#endif /* TARGET */


/* Determine the minimum alignment for a general-purpose memory allocation
 * on this system.
 */

static
size_t
minalign(void)
{
    size_t a;
    long n;

    {
        /* Hopefully the largest integral type.  If the compiler supports
         * long long, it doesn't necessarily mean that it will have a more
         * restrictive alignment than a long integer, but we allow that
         * check anyway.
         */
#if MP_LONGLONG_SUPPORT
        struct { char x; long long y; } z;
#else /* MP_LONGLONG_SUPPORT */
        struct { char x; long y; } z;
#endif /* MP_LONGLONG_SUPPORT */
        n = (char *) &z.y - &z.x;
    }
    a = n;
    {
        /* Hopefully the largest floating point type.  The long double
         * type appeared with the ANSI standard and this code is written
         * in ANSI C so we shouldn't need to worry about not supporting it.
         */
        struct { char x; long double y; } z;
        n = (char *) &z.y - &z.x;
    }
    if (a < (unsigned long) n)
        a = n;
    {
        /* A generic pointer type.  The assumption in this case is that
         * a pointer to void is the most restrictive pointer type on this
         * system.
         */
        struct { char x; void *y; } z;
        n = (char *) &z.y - &z.x;
    }
    if (a < (unsigned long) n)
        a = n;
    return __mp_poweroftwo(a);
}


/* Return the system page size.
 */

static
size_t
pagesize(void)
{
#if TARGET == TARGET_WINDOWS
    SYSTEM_INFO i;
#endif /* TARGET */

#if TARGET == TARGET_UNIX
    /* This call could also be getpagesize() but it is more POSIX-conforming
     * to call sysconf().  Unfortunately, SunOS and the BSD systems only have
     * getpagesize().
     */
#if SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
    SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
    return getpagesize();
#else /* SYSTEM */
    return sysconf(_SC_PAGESIZE);
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
    /* The Amiga has no virtual memory system (at least not supplied with
     * AmigaOS), so we return a fixed value here because it doesn't really
     * matter what the page size is.
     */
    return 4096;
#elif TARGET == TARGET_WINDOWS
    GetSystemInfo(&i);
    return i.dwPageSize;
#elif TARGET == TARGET_NETWARE
    return NXGetPageSize();
#else /* TARGET */
    /* We just assume that any other operating systems have no virtual
     * memory support and so anything we return here is irrelevant.
     */
    return 1024;
#endif /* TARGET */
}


/* Determine the stack direction on this system.
 */

static
int
stackdirection(void *p)
{
    unsigned long n;

    n = (unsigned long) &p;
    if (p == NULL)
        return stackdirection(&n);
    else if (&n < (unsigned long *) p)
        return -1;
    else
        return 1;
}


/* Return the executable file name that the program was invoked with.
 * Note that this function will not be reentrant if the return value is
 * a pointer to a local static string buffer.
 */

static
char *
progname(void)
{
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_AIX
    extern char **p_xargv;
#elif SYSTEM == SYSTEM_HPUX
    extern char **__argv_value;
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_SINIX || SYSTEM == SYSTEM_TRU64
    extern char **__Argv;
#elif SYSTEM == SYSTEM_UNIXWARE
    extern char **___Argv;
#elif SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_LINUX || \
      SYSTEM == SYSTEM_NETBSD || SYSTEM == SYSTEM_OPENBSD
    static char c[256];
    ssize_t l;
    int f;
#elif SYSTEM == SYSTEM_DGUX || SYSTEM == SYSTEM_DRSNX || \
      SYSTEM == SYSTEM_DYNIX || SYSTEM == SYSTEM_LYNXOS || \
      SYSTEM == SYSTEM_SOLARIS || SYSTEM == SYSTEM_SUNOS
    extern char **environ;
    char **e;
    char *t;
#endif /* SYSTEM */
#if !MP_BUILTINSTACK_SUPPORT && !MP_LIBRARYSTACK_SUPPORT && \
    (ARCH == ARCH_IX86 || ARCH == ARCH_M68K || ARCH == ARCH_MIPS || \
     ARCH == ARCH_POWER || ARCH == ARCH_POWERPC || ARCH == ARCH_SPARC)
    unsigned long *p;
    stackinfo s;
#endif /* MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT && ARCH */
#ifdef MP_PROCFS_EXENAME
    static char b[64];
#endif /* MP_PROCFS_EXENAME */
#elif TARGET == TARGET_AMIGA || TARGET == TARGET_WINDOWS
    static char p[256];
#elif TARGET == TARGET_NETWARE
    char *p, *t;
#endif /* TARGET */

#if TARGET == TARGET_UNIX
    /* AIX, HP/UX, IRIX, SINIX, Tru64 and UnixWare have global variables
     * containing argc and argv which we can use to determine the filename
     * that the program was invoked with.
     */
#if SYSTEM == SYSTEM_AIX
    if (p_xargv[0] != NULL)
        return p_xargv[0];
#elif SYSTEM == SYSTEM_HPUX
    if (__argv_value[0] != NULL)
        return __argv_value[0];
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_SINIX || SYSTEM == SYSTEM_TRU64
    if (__Argv[0] != NULL)
        return __Argv[0];
#elif SYSTEM == SYSTEM_UNIXWARE
    if (___Argv[0] != NULL)
        return ___Argv[0];
#elif SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_LINUX || \
      SYSTEM == SYSTEM_NETBSD || SYSTEM == SYSTEM_OPENBSD
    /* The BSD variants and Linux have a file in the /proc filesystem which
     * contains the argument vector that a process was invoked with.
     */
    l = 0;
    sprintf(b, MP_PROCFS_CMDNAME, __mp_processid());
    if ((f = open(b, O_RDONLY)) != -1)
    {
        if ((l = read(f, c, sizeof(c) - 1)) == -1)
            l = 0;
        close(f);
    }
    if (l > 0)
    {
        c[l] = '\0';
        return c;
    }
#elif SYSTEM == SYSTEM_DGUX || SYSTEM == SYSTEM_DRSNX || \
      SYSTEM == SYSTEM_DYNIX || SYSTEM == SYSTEM_LYNXOS || \
      SYSTEM == SYSTEM_SOLARIS || SYSTEM == SYSTEM_SUNOS
    /* We can access the argument vector from the pointer to the environment
     * array on all other UNIX systems.  On DG/UX Intel, DRS/NX, DYNIX/ptx,
     * Solaris and SunOS we stop scanning backwards along the array when we
     * reach argc.  On DG/UX M88K and LynxOS we stop scanning forwards along
     * the array when we reach a NULL pointer.  The contents of the argument
     * vector then follow.
     */
#if (SYSTEM == SYSTEM_DGUX && ARCH == ARCH_M88K) || SYSTEM == SYSTEM_LYNXOS
    for (e = environ; *e != NULL; e++);
    t = (char *) (e + 1);
#else /* SYSTEM && ARCH */
    for (t = NULL, e = environ - 2; *e > (char *) _POSIX_ARG_MAX; t = *e--);
#endif /* SYSTEM && ARCH */
    if (t != NULL)
        return t;
#endif /* SYSTEM */
#if !MP_BUILTINSTACK_SUPPORT && !MP_LIBRARYSTACK_SUPPORT && \
    (ARCH == ARCH_IX86 || ARCH == ARCH_M68K || ARCH == ARCH_MIPS || \
     ARCH == ARCH_POWER || ARCH == ARCH_POWERPC || ARCH == ARCH_SPARC)
    /* Because there is no function to return the executable filename
     * of a process on UNIX, we need to cheat and rely on the ABI by walking
     * up the process stack till we reach the startup code and then find
     * argv[0].  This is very OS-specific and is not my first choice for
     * doing this, but unfortunately it seemed to be the only way.
     */
    __mp_newframe(&s, NULL);
    for (p = NULL; __mp_getframe(&s); p = (unsigned long *) s.frame);
    if (p != NULL)
    {
#if ARCH == ARCH_IX86
#if SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_LINUX || \
    SYSTEM == SYSTEM_NETBSD || SYSTEM == SYSTEM_OPENBSD
        if (p = (unsigned long *) p[4])
            return (char *) *p;
#elif SYSTEM == SYSTEM_LYNXOS
        if (p = (unsigned long *) p[3])
            return (char *) *p;
#else /* SYSTEM */
        if (p = (unsigned long *) p[3])
            return (char *) p;
#endif /* SYSTEM */
#elif ARCH == ARCH_M68K
        if (p = (unsigned long *) p[3])
            return (char *) *p;
#elif ARCH == ARCH_MIPS
        if (p = (unsigned long *) p[7])
            return (char *) p;
#elif ARCH == ARCH_POWER || ARCH == ARCH_POWERPC
#if SYSTEM == SYSTEM_AIX
        if (p = (unsigned long *) p[7])
            return (char *) p;
#else /* SYSTEM */
        if (p = (unsigned long *) p[23])
            return (char *) *p;
#endif /* SYSTEM */
#elif ARCH == ARCH_SPARC
#if ENVIRON == ENVIRON_64
        if (p = (unsigned long *) *(((unsigned long *) (*p + 0x7FF)) + 1))
#else /* ENVIRON */
        if (p = (unsigned long *) *(((unsigned long *) *p) + 1))
#endif /* ENVIRON */
            return (char *) *p;
#endif /* ARCH */
    }
#endif /* MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT && ARCH */
#ifdef MP_PROCFS_EXENAME
    /* If the /proc filesystem is supported then we can usually access the
     * actual executable file that contains the current program through a
     * special file in the current /proc entry.
     */
    sprintf(b, MP_PROCFS_EXENAME, __mp_processid());
    return b;
#endif /* MP_PROCFS_EXENAME */
#elif TARGET == TARGET_AMIGA
    if (GetProgramName(p, sizeof(p)))
        return p;
#elif TARGET == TARGET_WINDOWS
    if (GetModuleFileName(NULL, p, sizeof(p)))
        return p;
#elif TARGET == TARGET_NETWARE
    if (GetNLMNameFromNLMID(GetNLMID(), &p, &t) == 0)
        return p;
#endif /* TARGET */
    return NULL;
}


/* Initialise the fields of a meminfo structure to describe the details
 * of the underlying memory architecture.
 */

MP_GLOBAL
void
__mp_newmemory(meminfo *i)
{
#if MP_WATCH_SUPPORT
    char b[64];
#endif /* MP_WATCH_SUPPORT */

#if MP_ARRAY_SUPPORT
    memorysize = 0;
#endif /* MP_ARRAY_SUPPORT */
    i->align = minalign();
    i->page = pagesize();
    i->stackdir = stackdirection(NULL);
    i->prog = progname();
#if MP_MMAP_SUPPORT
    /* On UNIX systems that support the mmap() function call, we default to
     * using sbrk() for user memory and mmap() for internal memory.  If the
     * MP_MMAP_ANONYMOUS macro is set then we don't need to open a file for
     * mapping.
     */
#if MP_MMAP_ANONYMOUS
    i->mfile = 0;
#else /* MP_MMAP_ANONYMOUS */
    i->mfile = open(MP_MMAP_FILENAME, O_RDWR);
#endif /* MP_MMAP_ANONYMOUS */
#else /* MP_MMAP_SUPPORT */
    i->mfile = -1;
#endif /* MP_MMAP_SUPPORT */
#if MP_WATCH_SUPPORT
    sprintf(b, MP_PROCFS_CTLNAME, __mp_processid());
    i->wfile = open(b, O_WRONLY);
#else /* MP_WATCH_SUPPORT */
    i->wfile = -1;
#endif /* MP_WATCH_SUPPORT */
    i->flags = 0;
}


/* Free up any resources used by the meminfo structure.
 */

MP_GLOBAL
void
__mp_endmemory(meminfo *i)
{
#if MP_MMAP_SUPPORT
    if (i->mfile != -1)
    {
#if !MP_MMAP_ANONYMOUS
        close(i->mfile);
#endif /* MP_MMAP_ANONYMOUS */
        i->mfile = -1;
    }
#endif /* MP_MMAP_SUPPORT */
#if MP_WATCH_SUPPORT
    if (i->wfile != -1)
    {
        close(i->wfile);
        i->wfile = -1;
    }
#endif /* MP_WATCH_SUPPORT */
}


/* Return the process identifier.
 */

MP_GLOBAL
unsigned long
__mp_processid(void)
{
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    return (unsigned long) getpid();
#elif TARGET == TARGET_AMIGA
    return (unsigned long) FindTask(NULL);
#elif TARGET == TARGET_NETWARE
    return (unsigned long) GetThreadId();
#else /* TARGET */
    /* We just assume that any other operating systems have no support for
     * multiple processes and so anything we return here is irrelevant.
     */
    return 1;
#endif /* TARGET */
}


#if MP_ARRAY_SUPPORT
/* Provide sbrk()-like functionality for systems that have no system functions
 * for allocating heap memory.  The simulated heap grows upwards in this
 * implementation.
 */

static
void *
getmemory(long l)
{
    void *p;

    p = memoryarray + memorysize;
    if (l > 0)
        if (memorysize + l > MP_ARRAY_SIZE)
            p = (void *) -1;
        else
            memorysize += l;
    else if (l < 0)
        if (memorysize < -l)
            p = (void *) -1;
        else
            memorysize += l;
    return p;
}
#elif TARGET == TARGET_UNIX
#define getmemory(l) sbrk(l)
#endif /* MP_ARRAY_SUPPORT && TARGET */


/* Allocate a specified size of general-purpose memory from the system
 * with a required alignment.
 */

MP_GLOBAL
void *
__mp_memalloc(meminfo *i, size_t *l, size_t a, int u)
{
    void *p;
#if MP_ARRAY_SUPPORT || TARGET == TARGET_UNIX
    void *t;
    unsigned long n;
#endif /* MP_ARRAY_SUPPORT && TARGET */

    if (*l == 0)
        *l = 1;
#if MP_ARRAY_SUPPORT || TARGET == TARGET_UNIX || TARGET == TARGET_NETWARE
    /* Round up the size of the allocation to a multiple of the system page
     * size.
     */
    *l = __mp_roundup(*l, i->page);
#elif TARGET == TARGET_WINDOWS
    /* The VirtualAlloc() function on Windows only seems to allocate memory in
     * blocks of 65536 bytes, so we round up the size of the allocation to this
     * amount since otherwise the space would be wasted.
     */
    *l = __mp_roundup(*l, 0x10000);
#elif TARGET == TARGET_AMIGA
    /* We aren't guaranteed to allocate a block of memory that is page
     * aligned on the Amiga, so we have to assume the worst case scenario
     * and allocate more memory for the specified alignment.
     */
    if (a > i->page)
        a = i->page;
    if (a > MEM_BLOCKSIZE)
        *l += __mp_poweroftwo(a) - MEM_BLOCKSIZE;
#endif /* MP_ARRAY_SUPPORT && TARGET */
#if MP_ARRAY_SUPPORT || TARGET == TARGET_UNIX
    /* UNIX has a contiguous heap for a process, but we are not guaranteed to
     * have full control over it, so we must assume that each separate memory
     * allocation is independent.  If we are using sbrk() to allocate memory
     * then we also try to ensure that all of our memory allocations are blocks
     * of pages.
     */
#if MP_MMAP_SUPPORT
    /* Decide if we are using mmap() or sbrk() to allocate the memory.  Requests
     * for user memory will be allocated in the opposite way to internal memory.
     */
    if ((((i->flags & FLG_USEMMAP) != 0) == (u != 0)) && (i->mfile != -1))
        u = 1;
    else
        u = 0;
    if (u != 0)
    {
#if MP_MMAP_ANONYMOUS
        if ((p = mmap(NULL, *l, PROT_READ | PROT_WRITE,
              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)) == (void *) -1)
#else /* MP_MMAP_ANONYMOUS */
        if ((p = mmap(NULL, *l, PROT_READ | PROT_WRITE, MAP_PRIVATE, i->mfile,
              0)) == (void *) -1)
#endif /* MP_MMAP_ANONYMOUS */
            p = NULL;
    }
    else
#endif /* MP_MMAP_SUPPORT */
    {
        if (((t = getmemory(0)) == (void *) -1) ||
            ((p = getmemory(*l)) == (void *) -1))
            p = NULL;
        else
        {
            if (p < t)
                /* The heap has grown down, which is quite unusual except on
                 * some weird systems where the stack grows up.
                 */
                n = (unsigned long) p - __mp_rounddown((unsigned long) p,
                                                       i->page);
            else
            {
                t = p;
                n = __mp_roundup((unsigned long) p, i->page) -
                    (unsigned long) p;
            }
            if (n > 0)
                /* We need to allocate a little more memory in order to make the
                 * allocation page-aligned.
                 */
                if ((p = getmemory(n)) == (void *) -1)
                {
                    /* We failed to allocate more memory, but we try to be nice
                     * and return our original allocation back to the system.
                     */
                    getmemory(-*l);
                    p = NULL;
                }
                else if (p >= t)
                    p = (char *) t + n;
        }
    }
#elif TARGET == TARGET_AMIGA
    p = AllocMem(*l, MEMF_ANY | MEMF_CLEAR);
#elif TARGET == TARGET_WINDOWS
    /* The VirtualProtect() function won't allow us to protect a range of pages
     * that span the allocation boundaries made by VirtualAlloc().  As mpatrol
     * tries to merge all bordering free memory areas, we must prevent the
     * pages allocated by different calls to VirtualAlloc() from being merged.
     * The easiest way to do this is to reserve a page of virtual memory after
     * each call to VirtualAlloc() since this won't actually take up any
     * physical memory.  It's a bit of a hack, though!
     */
    p = VirtualAlloc(NULL, *l, MEM_COMMIT, PAGE_READWRITE);
    VirtualAlloc(NULL, 0x10000, MEM_RESERVE, PAGE_NOACCESS);
#elif TARGET == TARGET_NETWARE
    p = NXPageAlloc(*l / i->page, 0);
#endif /* MP_ARRAY_SUPPORT && TARGET */
#if MP_ARRAY_SUPPORT || TARGET == TARGET_UNIX || TARGET == TARGET_NETWARE
    /* UNIX's sbrk() and Netware's NXPageAlloc() do not zero the allocated
     * memory, so we do this here for predictable behaviour.  This is also the
     * case if we are using a simulated heap.
     */
#if MP_MMAP_SUPPORT
    if ((p != NULL) && (u == 0))
#else /* MP_MMAP_SUPPORT */
    if (p != NULL)
#endif /* MP_MMAP_SUPPORT */
        __mp_memset(p, 0, *l);
#endif /* MP_ARRAY_SUPPORT && TARGET */
    if (p == NULL)
        errno = ENOMEM;
    return p;
}


/* Return a block of allocated memory back to the system.
 */

MP_GLOBAL
void
__mp_memfree(meminfo *i, void *p, size_t l)
{
#if !MP_ARRAY_SUPPORT
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS || \
    TARGET == TARGET_NETWARE
    void *t;
#endif /* TARGET */
#endif /* MP_ARRAY_SUPPORT */

    /* This function is hardly ever called except when the process is
     * terminating as the heap manager will take care of reusing unused
     * memory.  There is also no point in doing anything when we are using
     * a simulated heap as it will automatically be returned to the system.
     */
#if !MP_ARRAY_SUPPORT
    if (l == 0)
        return;
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS || \
    TARGET == TARGET_NETWARE
    t = (void *) __mp_rounddown((unsigned long) p, i->page);
#endif /* TARGET */
#if TARGET == TARGET_UNIX
    /* If we used sbrk() to allocate this memory then we can't shrink the
     * break point since someone else might have allocated memory in between
     * our allocations.  The next best thing is to unmap our freed allocations
     * so that they no longer need to be handled by the virtual memory system.
     * If we used mmap() to allocate this memory then we don't need to worry
     * about the above problem.
     */
    l = __mp_roundup(l + ((char *) p - (char *) t), i->page);
    mprotect(t, l, PROT_NONE);
    munmap(t, l);
#elif TARGET == TARGET_AMIGA
    FreeMem(p, l);
#elif TARGET == TARGET_WINDOWS
    VirtualFree(t, 0, MEM_RELEASE);
#elif TARGET == TARGET_NETWARE
    NXPageFree(t);
#endif /* TARGET */
#endif /* MP_ARRAY_SUPPORT */
}


#if TARGET == TARGET_UNIX
/* Handles any signals that result from illegal memory accesses whilst
 * querying the permissions of addresses.
 */

static
void
memoryhandler(int s)
{
    longjmp(memorystate, 1);
}
#endif /* TARGET */


/* Return the access permission of an address.
 */

MP_GLOBAL
memaccess
__mp_memquery(meminfo *i, void *p)
{
#if TARGET == TARGET_UNIX
#if MP_SIGINFO_SUPPORT
    struct sigaction s;
#endif /* MP_SIGINFO_SUPPORT */
    char c;
#elif TARGET == TARGET_WINDOWS
    MEMORY_BASIC_INFORMATION m;
#endif /* TARGET */
    memaccess r;

    r = MA_READWRITE;
#if TARGET == TARGET_UNIX
#if MP_MINCORE_SUPPORT
    /* The mincore() system call allows us to determine if a page is in core,
     * and if it is not and ENOMEM is set then it means that the page is not
     * mapped.  Unfortunately, we can't tell if it's read-only.
     */
    if ((mincore((char *) __mp_rounddown((unsigned long) p, i->page), 1, &c) ==
         -1) && (errno == ENOMEM))
        return MA_NOACCESS;
#endif /* MP_MINCORE_SUPPORT */
    /* One generic way to determine the access permission of an address across
     * all UNIX systems is to attempt to read from and write to the address and
     * check the results using signals.
     */
#if MP_SIGINFO_SUPPORT
    s.sa_flags = 0;
    (void *) s.sa_handler = (void *) memoryhandler;
    sigfillset(&s.sa_mask);
    sigaction(SIGBUS, &s, &membushandler);
    sigaction(SIGSEGV, &s, &memsegvhandler);
#else /* MP_SIGINFO_SUPPORT */
    membushandler = signal(SIGBUS, memoryhandler);
    memsegvhandler = signal(SIGSEGV, memoryhandler);
#endif /* MP_SIGINFO_SUPPORT */
    if (setjmp(memorystate))
        r = MA_NOACCESS;
    else
    {
        c = *((char *) p);
        if (setjmp(memorystate))
            r = MA_READONLY;
        else
            *((char *) p) = c;
    }
#if MP_SIGINFO_SUPPORT
    sigaction(SIGBUS, &membushandler, NULL);
    sigaction(SIGSEGV, &memsegvhandler, NULL);
#else /* MP_SIGINFO_SUPPORT */
    signal(SIGBUS, membushandler);
    signal(SIGSEGV, memsegvhandler);
#endif /* MP_SIGINFO_SUPPORT */
#elif TARGET == TARGET_WINDOWS
    /* On Windows, the VirtualQuery() function allows us to determine the
     * access permission of the page the address belongs to.
     */
    if (VirtualQuery(p, &m, sizeof(m)) >= sizeof(m))
        if (!(m.State & MEM_COMMIT) || (m.Protect & PAGE_NOACCESS) ||
            (m.Protect & PAGE_EXECUTE))
            r = MA_NOACCESS;
        else if ((m.Protect & PAGE_READONLY) || (m.Protect & PAGE_EXECUTE_READ))
            r = MA_READONLY;
#endif /* TARGET */
    return r;
}


/* Protect a block of allocated memory with the supplied access permission.
 */

MP_GLOBAL
int
__mp_memprotect(meminfo *i, void *p, size_t l, memaccess a)
{
#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    void *t;
    int n;
#endif /* TARGET */

#if TARGET == TARGET_UNIX || TARGET == TARGET_WINDOWS
    if (l == 0)
        return 1;
    t = (void *) __mp_rounddown((unsigned long) p, i->page);
    l = __mp_roundup(l + ((char *) p - (char *) t), i->page);
#if TARGET == TARGET_UNIX
    if (a == MA_NOACCESS)
        n = PROT_NONE;
    else if (a == MA_READONLY)
        n = PROT_READ;
    else
        n = PROT_READ | PROT_WRITE;
    if (mprotect(t, l, n) == -1)
        return 0;
#elif TARGET == TARGET_WINDOWS
    if (a == MA_NOACCESS)
        n = PAGE_NOACCESS;
    else if (a == MA_READONLY)
        n = PAGE_READONLY;
    else
        n = PAGE_READWRITE;
    if (!VirtualProtect(t, l, n, (unsigned long *) &n))
        return 0;
#endif /* TARGET */
#endif /* TARGET */
    return 1;
}


/* Notify the operating system to watch a specified group of bytes with the
 * supplied access permission.
 */

MP_GLOBAL
int
__mp_memwatch(meminfo *i, void *p, size_t l, memaccess a)
{
#if MP_WATCH_SUPPORT
    watchcmd w;
#endif /* MP_WATCH_SUPPORT */

#if MP_WATCH_SUPPORT
    if (l == 0)
        return 1;
#if SYSTEM == SYSTEM_SOLARIS
    w.cmd = PCWATCH;
    w.data.pr_vaddr = (uintptr_t) p;
    w.data.pr_size = l;
    if (a == MA_NOACCESS)
        w.data.pr_wflags = WA_READ | WA_WRITE | WA_TRAPAFTER;
    else if (a == MA_READONLY)
        w.data.pr_wflags = WA_WRITE | WA_TRAPAFTER;
    else
        w.data.pr_wflags = 0;
    if ((i->wfile == -1) ||
        (write(i->wfile, (void *) &w, sizeof(watchcmd)) != sizeof(watchcmd)))
        return 0;
#else /* SYSTEM */
    w.cmd = PIOCSWATCH;
    w.data.pr_vaddr = (caddr_t) p;
    if (a == MA_NOACCESS)
    {
        w.data.pr_size = l;
        w.data.pr_wflags = MA_READ | MA_WRITE;
    }
    else if (a == MA_READONLY)
    {
        w.data.pr_size = l;
        w.data.pr_wflags = MA_WRITE;
    }
    else
    {
        w.data.pr_size = 0;
        w.data.pr_wflags = 0;
    }
    if ((i->wfile == -1) || (ioctl(i->wfile, w.cmd, &w.data) == -1))
        return 0;
#endif /* SYSTEM */
#endif /* MP_WATCH_SUPPORT */
    return 1;
}


/* Check that a block of memory only contains a specific byte.
 */

MP_GLOBAL
void *
__mp_memcheck(void *t, char c, size_t l)
{
    long *w;
    char *p;
    size_t i, n;
    long b;

    /* This used to be a simple loop to compare each byte individually, but
     * that is less efficient than attempting to compare words at a time.
     * Therefore, if the number of bytes to compare is larger than a certain
     * number then this routine will attempt to compare as many words as
     * possible.
     */
    if (l > sizeof(long) * sizeof(long))
    {
        /* Check all bytes that occur before the first word.
         */
        if ((n = (unsigned long) t & (sizeof(long) - 1)) > 0)
        {
            if ((n = sizeof(long) - n) > l)
                n = l;
            for (p = (char *) t, t = (char *) t + n; p < (char *) t; p++)
                if (*p != c)
                    return p;
            l -= n;
        }
        if (l == 0)
            return NULL;
        /* Check all words that occur in the memory block.
         */
        if ((n = l / sizeof(long)) > 0)
        {
            /* Build up the word that we will be checking against.
             */
            for (p = (char *) &b, i = 0; i < sizeof(long); p++, i++)
                *p = c;
            for (w = (long *) t, t = (long *) t + n; w < (long *) t; w++)
                if (*w != b)
                {
                    /* Locate the exact byte that caused the test to fail.
                     */
                    for (p = (char *) w, i = 0; i < sizeof(long); p++, i++)
                        if (*p != c)
                            return p;
                    /* The above loop should never exit, but just in case it
                     * does, return the address of the word that caused the test
                     * to fail.
                     */
                    return w;
                }
            l -= n * sizeof(long);
        }
    }
    if (l == 0)
        return NULL;
    /* Check all remaining bytes.
     */
    for (p = (char *) t, t = (char *) t + l; p < (char *) t; p++)
        if (*p != c)
            return p;
    return NULL;
}


/* Compare two blocks of memory.
 */

MP_GLOBAL
void *
__mp_memcompare(void *t, void *s, size_t l)
{
    char *p;
    size_t n;

    /* This used to be a simple loop to compare each byte individually, but
     * that is less efficient than attempting to compare words at a time.
     * Therefore, if the number of bytes to compare is larger than a certain
     * number then this routine will attempt to compare as many words as
     * possible.
     */
    if ((s == t) || (l == 0))
        return NULL;
    n = (unsigned long) s & (sizeof(long) - 1);
    if ((n == ((unsigned long) t & (sizeof(long) - 1))) &&
        (l > sizeof(long) * sizeof(long)))
    {
        /* We can only compare words if the two blocks have the same alignment.
         * This also guarantees that there is at least one word of difference
         * between the two pointers.
         */
        if ((n > 0) && ((n = sizeof(long) - n) > l))
            n = l;
        /* Compare all bytes that occur before the first word.
         */
        while (n > 0)
        {
            if (*((char *) t) != *((char *) s))
                return t;
            s = (char *) s + 1;
            t = (char *) t + 1;
            n--;
            l--;
        }
        /* Compare all words that occur in the memory blocks.
         */
        while (l >= sizeof(long))
        {
            if (*((long *) t) != *((long *) s))
            {
                /* Locate the exact byte that caused the test to fail.
                 */
                for (p = (char *) t, n = 0; n < sizeof(long); p++, n++)
                    if (*p != ((char *) s)[n])
                        return p;
                /* The above loop should never exit, but just in case it
                 * does, return the address of the word that caused the test
                 * to fail.
                 */
                return t;
            }
            s = (long *) s + 1;
            t = (long *) t + 1;
            l -= sizeof(long);
        }
    }
    /* Compare all remaining bytes.
     */
    while (l > 0)
    {
        if (*((char *) t) != *((char *) s))
            return t;
        s = (char *) s + 1;
        t = (char *) t + 1;
        l--;
    }
    return NULL;
}


/* Attempt to locate the position of one block of memory in another block.
 */

MP_GLOBAL
void *
__mp_memfind(void *t, size_t l, void *s, size_t m)
{
    if (m > 0)
        while (l >= m)
        {
            if ((*((char *) t) == *((char *) s)) && ((m == 1) ||
                 (!__mp_memcompare((char *) t + 1, (char *) s + 1, m - 1))))
                return t;
            t = (char *) t + 1;
            l--;
        }
    return NULL;
}


/* Set a block of memory to contain a specific byte.
 */

MP_GLOBAL
void
__mp_memset(void *t, char c, size_t l)
{
    long *w;
    char *p;
    size_t i, n;
    long b;

    /* This used to be a simple loop to set each byte individually, but
     * that is less efficient than attempting to set words at a time.
     * Therefore, if the number of bytes to set is larger than a certain
     * number then this routine will attempt to set as many words as
     * possible.
     */
    if (l > sizeof(long) * sizeof(long))
    {
        /* Set all bytes that occur before the first word.
         */
        if ((n = (unsigned long) t & (sizeof(long) - 1)) > 0)
        {
            if ((n = sizeof(long) - n) > l)
                n = l;
            for (p = (char *) t, t = (char *) t + n; p < (char *) t; *p++ = c);
            l -= n;
        }
        if (l == 0)
            return;
        /* Set all words that occur in the memory block.
         */
        if ((n = l / sizeof(long)) > 0)
        {
            /* Build up the word that we will be writing to memory.
             */
            for (p = (char *) &b, i = 0; i < sizeof(long); *p++ = c, i++);
            for (w = (long *) t, t = (long *) t + n; w < (long *) t; *w++ = b);
            l -= n * sizeof(long);
        }
    }
    if (l == 0)
        return;
    /* Set all remaining bytes.
     */
    for (p = (char *) t, t = (char *) t + l; p < (char *) t; *p++ = c);
}


/* Copy a block of memory from one address to another.
 */

MP_GLOBAL
void
__mp_memcopy(void *t, void *s, size_t l)
{
    size_t n;

    if ((s == t) || (l == 0))
        return;
    /* This used to be a simple loop to copy each byte individually, but
     * that is less efficient than attempting to copy words at a time.
     * Therefore, if the number of bytes to copy is larger than a certain
     * number then this routine will attempt to copy as many words as
     * possible.
     */
    if ((s < t) && ((char *) s + l > (char *) t))
    {
        /* The end of the source block overlaps with the beginning of the
         * destination block, so we have to copy from the end.
         */
        s = (char *) s + l;
        t = (char *) t + l;
        n = (unsigned long) s & (sizeof(long) - 1);
        if ((n == ((unsigned long) t & (sizeof(long) - 1))) &&
            (l > sizeof(long) * sizeof(long)))
        {
            /* We can only copy words if the source and destination have the
             * same alignment.  This also guarantees that there is at least one
             * word of difference between the source and destination pointers.
             */
            if (n > l)
                n = l;
            /* Copy all bytes that occur after the last word.
             */
            while (n > 0)
            {
                s = (char *) s - 1;
                t = (char *) t - 1;
                *((char *) t) = *((char *) s);
                n--;
                l--;
            }
            /* Copy all words that occur in the memory block.
             */
            while (l >= sizeof(long))
            {
                s = (long *) s - 1;
                t = (long *) t - 1;
                *((long *) t) = *((long *) s);
                l -= sizeof(long);
            }
        }
        /* Copy all remaining bytes.
         */
        while (l > 0)
        {
            s = (char *) s - 1;
            t = (char *) t - 1;
            *((char *) t) = *((char *) s);
            l--;
        }
    }
    else
    {
        /* The end of the destination block overlaps with the beginning of the
         * source block, or there is no overlap at all, so we can copy from
         * the beginning.
         */
        n = (unsigned long) s & (sizeof(long) - 1);
        if ((n == ((unsigned long) t & (sizeof(long) - 1))) &&
            (l > sizeof(long) * sizeof(long)))
        {
            /* We can only copy words if the source and destination have the
             * same alignment.  This also guarantees that there is at least one
             * word of difference between the source and destination pointers.
             */
            if ((n > 0) && ((n = sizeof(long) - n) > l))
                n = l;
            /* Copy all bytes that occur before the first word.
             */
            while (n > 0)
            {
                *((char *) t) = *((char *) s);
                s = (char *) s + 1;
                t = (char *) t + 1;
                n--;
                l--;
            }
            /* Copy all words that occur in the memory block.
             */
            while (l >= sizeof(long))
            {
                *((long *) t) = *((long *) s);
                s = (long *) s + 1;
                t = (long *) t + 1;
                l -= sizeof(long);
            }
        }
        /* Copy all remaining bytes.
         */
        while (l > 0)
        {
            *((char *) t) = *((char *) s);
            s = (char *) s + 1;
            t = (char *) t + 1;
            l--;
        }
    }
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
