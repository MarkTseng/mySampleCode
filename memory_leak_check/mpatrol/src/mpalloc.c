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
 * Release version of library interface.  This module defines the visible
 * interface for the mpatrol library in release mode.  These functions
 * should not normally be required if all of the source files that use
 * mpatrol.h were recompiled with NDEBUG, but they are defined here just
 * in case.
 */


#include "inter.h"
#include "mpalloc.h"
#include "version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#if TARGET == TARGET_UNIX
#include <unistd.h>
#if SYSTEM == SYSTEM_LINUX
#include <malloc.h>
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
#include <windows.h>
#include <winbase.h>
#elif TARGET == TARGET_NETWARE
#include <nwthread.h>
#include <nks/memory.h>
#endif /* TARGET */


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mpalloc_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


/* An allocaheader belongs to a stack of memory allocations that were made
 * by the alloca() and related functions.  Some memory allocations at the
 * top of the stack may be automatically freed when the next call to allocate
 * memory is made and the stack has been unwound.
 */

typedef union allocaheader
{
#if MP_LONGLONG_SUPPORT
  long long integer;            /* type with most restrictive alignment */
#else /* MP_LONGLONG_SUPPORT */
  long integer;                 /* type with most restrictive alignment */
#endif /* MP_LONGLONG_SUPPORT */
  long double real;             /* type with most restrictive alignment */
  struct
  {
      union allocaheader *next; /* pointer to next allocaheader */
      void *frame;              /* pointer to stack frame */
  }
  data;
}
allocaheader;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The stack of allocaheaders that is used to keep track of the details of
 * (and when to free) memory allocated with alloca() and related functions.
 */

static allocaheader *allocastack;


/* Terminate the program after an mpatrol function has been called with an
 * illegal function type.
 */

static
void
illegalfunction(char *f, char *s, char *t, unsigned long u)
{
    fputs("fatal error", stderr);
    if (s != NULL)
        fprintf(stderr, " in %s", s);
    if ((t != NULL) && (u != 0))
        fprintf(stderr, " in %s line %lu", t, u);
    fputc('\n', stderr);
    fprintf(stderr, "mpatrol function %s called with illegal function type\n",
            f);
    fflush(NULL);
    exit(EXIT_FAILURE);
}


/* Round an unsigned integer up to the nearest power of two.
 */

static
unsigned long
poweroftwo(unsigned long n)
{
    unsigned char l;

    if ((n == 0) || ((n & (n - 1)) == 0))
        return n;
    for (l = 0; n > 0; l++, n >>= 1);
    return (unsigned long) (2 << (l - 1));
}


/* Return the system page size.
 */

static
size_t
pagesize(void)
{
    static size_t s;
#if TARGET == TARGET_WINDOWS
    SYSTEM_INFO i;
#endif /* TARGET */

    if (s == 0)
    {
#if TARGET == TARGET_UNIX
        /* This call could also be getpagesize() but it is more POSIX-conforming
         * to call sysconf().  Unfortunately, SunOS and the BSD systems only
         * have getpagesize().
         */
#if SYSTEM == SYSTEM_FREEBSD || SYSTEM == SYSTEM_NETBSD || \
    SYSTEM == SYSTEM_OPENBSD || SYSTEM == SYSTEM_SUNOS
        s = getpagesize();
#else /* SYSTEM */
        s = sysconf(_SC_PAGESIZE);
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
        /* The Amiga has no virtual memory system (at least not supplied with
         * AmigaOS), so we return a fixed value here because it doesn't really
         * matter what the page size is.
         */
        s = 4096;
#elif TARGET == TARGET_WINDOWS
        GetSystemInfo(&i);
        s = i.dwPageSize;
#elif TARGET == TARGET_NETWARE
        s = NXGetPageSize();
#else /* TARGET */
    /* We just assume that any other operating systems have no virtual
     * memory support and so anything we return here is irrelevant.
     */
    return 1024;
#endif /* TARGET */
    }
    return s;
}


/* Determine the stack direction on this system.
 */

static
int
stackdirection(void *p)
{
    static int d;
    unsigned long n;

    if (d == 0)
    {
        n = (unsigned long) &p;
        if (p == NULL)
            return stackdirection(&n);
        else if (&n < (unsigned long *) p)
            d = -1;
        else
            d = 1;
    }
    return d;
}


/* Check the alloca allocation stack for any allocations that should be freed.
 */

static
void
checkalloca(void *p)
{
    allocaheader *m, *n;
    int d;

    d = stackdirection(NULL);
    for (n = allocastack; n != NULL; n = m)
    {
        m = n->data.next;
        if (((d > 0) && ((char *) n->data.frame > (char *) p + 32)) ||
            ((d < 0) && ((char *) n->data.frame + 32 < (char *) p)))
        {
            free(n);
            allocastack = m;
        }
        else
            break;
    }
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
}


/* Reinitialise the mpatrol library.
 */

MP_API
void
__mp_reinit(void)
{
}


/* Finalise the mpatrol library.
 */

MP_API
void
__mp_fini(void)
{
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
    if (atexit(f) == -1)
        return 0;
    return 1;
}


/* Set an mpatrol option after the library has been initialised.
 */

MP_API
unsigned long
__mp_setoption(long o, unsigned long v)
{
    return ~0L;
}


/* Get an mpatrol option after the library has been initialised.
 */

MP_API
int
__mp_getoption(long o, unsigned long *v)
{
    return 0;
}


/* Return the memory header structure.
 */

MP_API
infohead *
__mp_memhead(void)
{
    return NULL;
}


/* Allocate a new block of memory of a specified size and alignment.
 */

MP_API
void *
__mp_alloc(size_t l, size_t a, alloctype f, char *s, char *t, unsigned long u,
           char *g, size_t h, size_t k)
{
    void *p;
    size_t n;

    checkalloca(&l);
    if (l == 0)
        l = 1;
    switch (f)
    {
      case AT_MALLOC:
        p = malloc(l);
        break;
      case AT_CALLOC:
        if (p = malloc(l))
            memset(p, 0, l);
        break;
      case AT_MEMALIGN:
      case AT_VALLOC:
      case AT_PVALLOC:
        /* We cannot rely on any system having implementations of memalign(),
         * valloc() or pvalloc() and so we must either implement them with
         * malloc() or with memalign() if it exists.  For the former
         * implementation, this is done by allocating extra space and then
         * rounding up the start address of the new allocation to the specified
         * alignment.  This means that there is likely to be some space wasted
         * for each allocation and the memory allocated by such functions
         * cannot be freed with free().  The latter point is also likely to be
         * true even if we allocated the memory with memalign().
         */
        n = pagesize();
        if (f == AT_PVALLOC)
            l = ((l - 1) & ~(n - 1)) + n;
        if ((f == AT_VALLOC) || (f == AT_PVALLOC) || (a > n))
            a = n;
        else if (a < sizeof(long))
            a = sizeof(long);
        else
            a = poweroftwo(a);
#if MP_MEMALIGN_SUPPORT
        p = memalign(a, l);
#else /* MP_MEMALIGN_SUPPORT */
        if (p = malloc(l + a - 1))
            p = (void *) ((((unsigned long) p - 1) & ~(a - 1)) + a);
#endif /* MP_MEMALIGN_SUPPORT */
        break;
      case AT_ALLOCA:
        p = __mp_xmalloc(l + sizeof(allocaheader), s, t, u, g, h);
        ((allocaheader *) p)->data.next = allocastack;
        ((allocaheader *) p)->data.frame = (void *) &l;
        allocastack = (allocaheader *) p;
        p = (char *) p + sizeof(allocaheader);
        break;
      case AT_XMALLOC:
        p = __mp_xmalloc(l, s, t, u, g, h);
        break;
      case AT_XCALLOC:
        p = __mp_xcalloc(l, s, t, u, g, h);
        break;
      case AT_NEW:
      case AT_NEWVEC:
        /* This implementation should really call the new handler if it
         * has been installed, but for now just abort if no memory can be
         * allocated.
         */
        p = __mp_xmalloc(l, s, t, u, g, h);
        break;
      default:
        illegalfunction("__mp_alloc", s, t, u);
        break;
    }
    return p;
}


/* Allocate a new block of memory to duplicate a string.
 */

MP_API
char *
__mp_strdup(char *p, size_t l, alloctype f, char *s, char *t, unsigned long u,
            size_t k)
{
    char *r;
    size_t i;

    checkalloca(&p);
    if (f == AT_XSTRDUP)
        return __mp_xstrdup(p, s, t, u);
    i = strlen(p);
    if ((f == AT_STRNDUP) || (f == AT_STRNSAVE) || (f == AT_STRNDUPA))
    {
        if (i > l)
            i = l;
    }
    else if ((f != AT_STRDUP) && (f != AT_STRSAVE) && (f != AT_STRDUPA))
        illegalfunction("__mp_strdup", s, t, u);
    if ((f == AT_STRDUPA) || (f == AT_STRNDUPA))
    {
        r = (char *) __mp_xmalloc(i + sizeof(allocaheader) + 1, s, t, u, "char",
                                  sizeof(char));
        ((allocaheader *) r)->data.next = allocastack;
        ((allocaheader *) r)->data.frame = (void *) &p;
        allocastack = (allocaheader *) r;
        r += sizeof(allocaheader);
    }
    else
        r = (char *) malloc(i + 1);
    if (r != NULL)
    {
        memcpy(r, p, i);
        r[i] = '\0';
    }
    return r;
}


/* Resize an existing block of memory to a new size and alignment.
 */

MP_API
void *
__mp_realloc(void *p, size_t l, size_t a, alloctype f, char *s, char *t,
             unsigned long u, char *g, size_t h, size_t k)
{
    void *q;

    checkalloca(&p);
    if (f == AT_XREALLOC)
        return __mp_xrealloc(p, l, s, t, u, g, h);
    else if ((f != AT_REALLOC) && (f != AT_REALLOCF) && (f != AT_RECALLOC) &&
             (f != AT_EXPAND))
        illegalfunction("__mp_realloc", s, t, u);
    /* There is a major limitation here in that we don't know the size of
     * the existing memory allocation.  This means that we can't implement
     * recalloc() or expand() properly, and in the latter case means that
     * we must always return NULL.  If you have existing implementations
     * of these functions on your system then you could make calls to them
     * here.
     */
    if (p == NULL)
    {
        if (l == 0)
            l = 1;
        if ((p = malloc(l)) && (f == AT_RECALLOC))
            memset(p, 0, l);
    }
    else if (l == 0)
    {
        free(p);
        p = NULL;
    }
    else if (f == AT_REALLOCF)
    {
        if ((q = realloc(p, l)) == NULL)
            free(p);
        p = q;
    }
    else if (f != AT_EXPAND)
        p = realloc(p, l);
    else
        p = NULL;
    return p;
}


/* Free an existing block of memory.
 */

MP_API
void
__mp_free(void *p, alloctype f, char *s, char *t, unsigned long u, size_t k)
{
    checkalloca(&p);
    if ((f != AT_FREE) && (f != AT_CFREE) && (f != AT_DEALLOCA) &&
        (f != AT_XFREE) && (f != AT_DELETE) && (f != AT_DELETEVEC))
        illegalfunction("__mp_free", s, t, u);
    if (f != AT_DEALLOCA)
        free(p);
}


/* Set a block of memory to contain a specific byte.
 */

MP_API
void *
__mp_setmem(void *p, size_t l, unsigned char c, alloctype f, char *s, char *t,
            unsigned long u, size_t k)
{
    if ((f != AT_MEMSET) && (f != AT_BZERO))
        illegalfunction("__mp_setmem", s, t, u);
    return memset(p, (int) c, l);
}


/* Copy a block of memory from one address to another.
 */

MP_API
void *
__mp_copymem(void *p, void *q, size_t l, unsigned char c, alloctype f, char *s,
             char *t, unsigned long u, size_t k)
{
    void *r;

    switch (f)
    {
      case AT_MEMCCPY:
        if (r = memchr(p, (int) c, l))
            l = (size_t) ((char *) r - (char *) p) + 1;
        memcpy(q, p, l);
        if (r != NULL)
            q = (char *) q + l;
        else
            q = NULL;
        break;
      case AT_MEMCPY:
        memcpy(q, p, l);
        break;
      case AT_MEMMOVE:
      case AT_BCOPY:
        memmove(q, p, l);
        break;
      default:
        illegalfunction("__mp_copymem", s, t, u);
        break;
    }
    return q;
}


/* Attempt to locate the position of one block of memory in another block.
 */

MP_API
void *
__mp_locatemem(void *p, size_t l, void *q, size_t m, alloctype f, char *s,
               char *t, unsigned long u, size_t k)
{
    if (f == AT_MEMCHR)
        return memchr(p, (int) (m & 0xFF), l);
    else if (f == AT_MEMMEM)
    {
        if (m > 0)
            while (l >= m)
            {
                if ((*((char *) p) == *((char *) q)) && ((m == 1) ||
                     !memcmp((char *) p + 1, (char *) q + 1, m - 1)))
                    return p;
                p = (char *) p + 1;
                l--;
            }
        return NULL;
    }
    illegalfunction("__mp_locatemem", s, t, u);
    return NULL;
}


/* Compare two blocks of memory.
 */

MP_API
int
__mp_comparemem(void *p, void *q, size_t l, alloctype f, char *s, char *t,
                unsigned long u, size_t k)
{
    if ((f != AT_MEMCMP) && (f != AT_BCMP))
        illegalfunction("__mp_comparemem", s, t, u);
    return memcmp(p, q, l);
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
    return NULL;
}


/* Return the name of the function corresponding to a given allocation type.
 */

MP_API
char *
__mp_function(alloctype f)
{
    return NULL;
}


/* Set the user data for a given memory allocation.
 */

MP_API
int
__mp_setuser(void *p, void *d)
{
    return 0;
}


/* Set the marked flag for a given memory allocation.
 */

MP_API
int
__mp_setmark(void *p)
{
    return 0;
}


/* Obtain any details about the memory block that contains a given address.
 */

MP_API
int
__mp_info(void *p, allocinfo *d)
{
    return 0;
}


/* Obtain any details about the function symbol that contains a given address.
 */

MP_API
int
__mp_syminfo(void *p, symbolinfo *d)
{
    return 0;
}


/* Obtain the name of the function symbol that contains a given address.
 */

MP_API
char *
__mp_symbol(void *p)
{
    return NULL;
}


/* Display any details about the memory block that contains a given address.
 * This is for calling within a symbolic debugger and will result in output to
 * the standard error file stream instead of the log file.
 */

MP_API
int
__mp_printinfo(void *p)
{
    return 0;
}


/* Return the current allocation event count for later use when examining
 * the difference in the list of allocations between now and a future point.
 */

MP_API
unsigned long
__mp_snapshot(void)
{
    return 0;
}


/* Iterate over all of the allocated and freed memory blocks, calling a
 * user-supplied function for each one encountered.
 */

MP_API
size_t
__mp_iterate(int (*f)(void *, void *), void *d, unsigned long s)
{
    return 0;
}


/* Iterate over all of the allocated, freed and free memory blocks, calling
 * a user-supplied function for each one encountered.
 */

MP_API
size_t
__mp_iterateall(int (*f)(void *, void *), void *d)
{
    return 0;
}


/* Add a memory allocation to the leak table.
 */

MP_API
int
__mp_addallocentry(char *f, unsigned long l, size_t c)
{
    return 0;
}


/* Remove a memory allocation from the leak table.
 */

MP_API
int
__mp_addfreeentry(char *f, unsigned long l, size_t c)
{
    return 0;
}


/* Clear the leak table.
 */

MP_API
void
__mp_clearleaktable(void)
{
}


/* Start recording memory allocation events in the leak table.
 */

MP_API
int
__mp_startleaktable(void)
{
    return 0;
}


/* Stop recording memory allocation events in the leak table.
 */

MP_API
int
__mp_stopleaktable(void)
{
    return 0;
}


/* Display the leak table.
 */

MP_API
void
__mp_leaktable(size_t l, int o, unsigned char f)
{
}


/* Display a complete memory map of the heap and (optionally) a summary of
 * all mpatrol library settings and statistics.
 */

MP_API
void
__mp_memorymap(int s)
{
}


/* Display a summary of all mpatrol library settings and statistics.
 */

MP_API
void
__mp_summary(void)
{
}


/* Return statistics about the current state of the heap.
 */

MP_API
int
__mp_stats(heapinfo *d)
{
    return 0;
}


/* Check the validity of all memory blocks that have been filled with
 * a predefined pattern.
 */

MP_API
void
__mp_checkheap(char *s, char *t, unsigned long u)
{
}


/* Check the validity of all memory blocks that have been filled with
 * a predefined pattern.
 */

MP_API
void
__mp_check(void)
{
}


/* Set the prologue function and return the previous setting.
 */

MP_API
prologuehandler
__mp_prologue(prologuehandler h)
{
    return NULL;
}


/* Set the epilogue function and return the previous setting.
 */

MP_API
epiloguehandler
__mp_epilogue(epiloguehandler h)
{
    return NULL;
}


/* Set the low-memory handler and return the previous setting.
 */

MP_API
nomemoryhandler
__mp_nomemory(nomemoryhandler h)
{
    return NULL;
}


/* Push source level information onto the top of the delete stack.
 */

MP_API
void
__mp_pushdelstack(char *s, char *t, unsigned long u)
{
}


/* Pop source level information off the top of the delete stack.
 */

MP_API
void
__mp_popdelstack(char **s, char **t, unsigned long *u)
{
    *s = *t = NULL;
    *u = 0;
}


/* Write user data to the mpatrol log file.
 */

MP_API
int
__mp_printf(char *s, ...)
{
    return 0;
}


/* Write user data to the mpatrol log file.
 */

MP_API
int
__mp_vprintf(char *s, va_list v)
{
    return 0;
}


/* Write user data to the mpatrol log file along with location information.
 */

MP_API
void
__mp_printfwithloc(char *s, char *t, unsigned long u, char *m, ...)
{
}


/* Write user data to the mpatrol log file along with location information.
 */

MP_API
void
__mp_vprintfwithloc(char *s, char *t, unsigned long u, char *m, va_list v)
{
}


/* Write a hex dump for a specified memory location to the mpatrol log file.
 */

MP_API
void
__mp_logmemory(void *p, size_t l)
{
}


/* Write the current call stack to the mpatrol log file.
 */

MP_API
int
__mp_logstack(size_t k)
{
    return 0;
}


/* Write the details about the memory block that contains a given address to
 * the mpatrol log file.
 */

MP_API
int
__mp_logaddr(void *p)
{
    return 0;
}


/* Invoke a text editor on a given source file at a specific line.
 */

MP_API
int
__mp_edit(char *f, unsigned long l)
{
    return 0;
}


/* List a given source file at a specific line.
 */

MP_API
int
__mp_list(char *f, unsigned long l)
{
    return 0;
}


/* Edit or list a given source file at a specific line, but only if the EDIT
 * or LIST options are in effect.
 */

MP_API
int
__mp_view(char *f, unsigned long l)
{
    return 0;
}


/* Read in an allocation contents file.
 */

MP_API
int
__mp_readcontents(char *s, void *p)
{
    return 0;
}


/* Write out an allocation contents file.
 */

MP_API
int
__mp_writecontents(char *s, void *p)
{
    return 0;
}


/* Compare an allocation contents file with the contents currently in memory.
 */

MP_API
long
__mp_cmpcontents(char *s, void *p)
{
    return -1;
}


/* Remove an allocation contents file.
 */

MP_API
int
__mp_remcontents(char *s, void *p)
{
    return 0;
}


/* The function that is called at every function prologue.
 */

MP_API
void
__cyg_profile_func_enter(void *a, void *p)
{
}


/* The function that is called at every function epilogue.
 */

MP_API
void
__cyg_profile_func_exit(void *a, void *p)
{
}


/* Set the access rights for a block of memory using the checker interface.
 */

MP_API
void
chkr_set_right(void *p, size_t l, unsigned char a)
{
}


/* Copy the access rights for a block of memory to another block using the
 * checker interface.
 */

MP_API
void
chkr_copy_bitmap(void *p, void *q, size_t l)
{
}


/* Check a block of memory using the checker interface.
 */

MP_API
void
chkr_check_addr(void *p, size_t l, unsigned char a)
{
}


/* Check a string using the checker interface.
 */

MP_API
void
chkr_check_str(char *p, unsigned char a)
{
}


/* Check a function pointer using the checker interface.
 */

MP_API
void
chkr_check_exec(void *p)
{
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
