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
 * Option handling.  Run-time modifiable flags are set here by parsing
 * an option string read from an environment variable.  Any warnings
 * or errors that occurred during parsing will be reported.
 */


#include "option.h"
#include "diag.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <time.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *option_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The temporary buffer used to parse the configuration environment variable,
 * since if we change the variable directly then we change the environment.
 * This should not really be a file scope variable as it prevents this module
 * from being re-entrant.
 */

static char options[1024];


/* The table describing a summary of all recognised options.
 */

static char *options_help[] =
{
    "ALLOCBYTE", "unsigned integer",
    "", "Specifies an 8-bit byte pattern with which to prefill newly-allocated",
    "", "memory.",
    "ALLOCSTOP", "unsigned integer",
    "", "Specifies an allocation index at which to stop the program when it is",
    "", "being allocated.",
    "ALLOWOFLOW", NULL,
    "", "Specifies that a warning rather than an error should be produced if",
    "", "any memory operation function overflows the boundaries of a memory",
    "", "allocation, and that the operation should still be performed.",
    "AUTOSAVE", "unsigned integer",
    "", "Specifies the frequency at which to periodically write the profiling",
    "", "data to the profiling output file.",
    "CHECK", "unsigned range",
    "", "Specifies a range of allocation indices at which to check the",
    "", "integrity of free memory and overflow buffers.",
    "CHECKALL", NULL,
    "", "Equivalent to the CHECKALLOCS, CHECKREALLOCS, CHECKFREES and",
    "", "CHECKMEMORY options specified together.",
    "CHECKALLOCS", NULL,
    "", "Checks that no attempt is made to allocate a block of memory of size",
    "", "zero.",
    "CHECKFORK", NULL,
    "", "Checks at every call to see if the process has been forked in case",
    "", "new log, profiling and tracing output files need to be started.",
    "CHECKFREES", NULL,
    "", "Checks that no attempt is made to deallocate a NULL pointer.",
    "CHECKMEMORY", NULL,
    "", "Checks that no attempt is made to perform a zero-length memory",
    "", "operation or a memory operation on a NULL pointer.",
    "CHECKREALLOCS", NULL,
    "", "Checks that no attempt is made to reallocate a NULL pointer or resize",
    "", "an existing block of memory to size zero.",
    "DEFALIGN", "unsigned integer",
    "", "Specifies the default alignment for general-purpose memory",
    "", "allocations, which must be a power of two.",
    "EDIT", NULL,
    "", "Specifies that a text editor should be invoked to edit any relevant",
    "", "source files that are associated with any warnings or errors when",
    "", "they occur.",
    "FAILFREQ", "unsigned integer",
    "", "Specifies the frequency at which all memory allocations will randomly",
    "", "fail.",
    "FAILSEED", "unsigned integer",
    "", "Specifies the random number seed which will be used when determining",
    "", "which memory allocations will randomly fail.",
    "FREEBYTE", "unsigned integer",
    "", "Specifies an 8-bit byte pattern with which to prefill newly-freed",
    "", "memory.",
    "FREESTOP", "unsigned integer",
    "", "Specifies an allocation index at which to stop the program when it is",
    "", "being freed.",
    "HELP", NULL,
    "", "Displays this quick-reference option summary.",
    "HTML", NULL,
    "", "Specifies that the log file should be formatted in HTML.",
    "LARGEBOUND", "unsigned integer",
    "", "Specifies the limit in bytes up to which memory allocations should be",
    "", "classified as large allocations for profiling purposes.",
    "LEAKTABLE", NULL,
    "", "Specifies that the leak table should be automatically used and a leak",
    "", "table summary should be displayed at the end of program execution.",
    "LIMIT", "unsigned integer",
    "", "Specifies the limit in bytes at which all memory allocations should",
    "", "fail if the total allocated memory should increase beyond this.",
    "LIST", NULL,
    "", "Specifies that a context listing should be shown for any relevant",
    "", "source files that are associated with any warnings or errors when",
    "", "they occur.",
    "LOGALL", NULL,
    "", "Equivalent to the LOGALLOCS, LOGREALLOCS, LOGFREES and LOGMEMORY",
    "", "options specified together.",
    "LOGALLOCS", NULL,
    "", "Specifies that all memory allocations are to be logged and sent to",
    "", "the log file.",
    "LOGFILE", "string",
    "", "Specifies an alternative file in which to place all diagnostics from",
    "", "the mpatrol library.",
    "LOGFREES", NULL,
    "", "Specifies that all memory deallocations are to be logged and sent to",
    "", "the log file.",
    "LOGMEMORY", NULL,
    "", "Specifies that all memory operations are to be logged and sent to the",
    "", "log file.",
    "LOGREALLOCS", NULL,
    "", "Specifies that all memory reallocations are to be logged and sent to",
    "", "the log file.",
    "MEDIUMBOUND", "unsigned integer",
    "", "Specifies the limit in bytes up to which memory allocations should be",
    "", "classified as medium allocations for profiling purposes.",
    "NOFREE", "unsigned integer",
    "", "Specifies that a number of recently-freed memory allocations should",
    "", "be prevented from being returned to the free memory pool.",
    "NOPROTECT", NULL,
    "", "Specifies that the mpatrol library's internal data structures should",
    "", "not be made read-only after every memory allocation, reallocation or",
    "", "deallocation.",
    "OFLOWBYTE", "unsigned integer",
    "", "Specifies an 8-bit byte pattern with which to fill the overflow",
    "", "buffers of all memory allocations.",
    "OFLOWSIZE", "unsigned integer",
    "", "Specifies the size in bytes to use for all overflow buffers, which",
    "", "must be a power of two.",
    "OFLOWWATCH", NULL,
    "", "Specifies that watch point areas should be used for overflow buffers",
    "", "rather than filling with the overflow byte.",
    "PAGEALLOC", "LOWER|UPPER",
    "", "Specifies that each individual memory allocation should occupy at",
    "", "least one page of virtual memory and should be placed at the lowest",
    "", "or highest point within these pages.",
    "PRESERVE", NULL,
    "", "Specifies that any reallocated or freed memory allocations should",
    "", "preserve their original contents.",
    "PROF", NULL,
    "", "Specifies that all memory allocations are to be profiled and sent to",
    "", "the profiling output file.",
    "PROFFILE", "string",
    "", "Specifies an alternative file in which to place all memory allocation",
    "", "profiling information from the mpatrol library.",
    "PROGFILE", "string",
    "", "Specifies an alternative filename with which to locate the executable",
    "", "file containing the program's symbols.",
    "REALLOCSTOP", "unsigned integer",
    "", "Specifies an allocation index at which to stop the program when a",
    "", "memory allocation is being reallocated.",
    "SAFESIGNALS", NULL,
    "", "Instructs the library to save and replace certain signal handlers",
    "", "during the execution of library code and to restore them afterwards.",
    "SHOWALL", NULL,
    "", "Equivalent to the SHOWFREE, SHOWFREED, SHOWUNFREED, SHOWMAP and",
    "", "SHOWSYMBOLS options specified together.",
    "SHOWFREE", NULL,
    "", "Specifies that a summary of all of the free memory blocks should be",
    "", "displayed at the end of program execution.",
    "SHOWFREED", NULL,
    "", "Specifies that a summary of all of the freed memory allocations",
    "", "should be displayed at the end of program execution.",
    "SHOWMAP", NULL,
    "", "Specifies that a memory map of the entire heap should be displayed at",
    "", "the end of program execution.",
    "SHOWSYMBOLS", NULL,
    "", "Specifies that a summary of all of the function symbols read from the",
    "", "program's executable file should be displayed at the end of program",
    "", "execution.",
    "SHOWUNFREED", NULL,
    "", "Specifies that a summary of all of the unfreed memory allocations",
    "", "should be displayed at the end of program execution.",
    "SMALLBOUND", "unsigned integer",
    "", "Specifies the limit in bytes up to which memory allocations should be",
    "", "classified as small allocations for profiling purposes.",
    "TRACE", NULL,
    "", "Specifies that all memory allocations are to be traced and sent to",
    "", "the tracing output file.",
    "TRACEFILE", "string",
    "", "Specifies an alternative file in which to place all memory allocation",
    "", "tracing information from the mpatrol library.",
    "UNFREEDABORT", "unsigned integer",
    "", "Specifies the minimum number of unfreed allocations at which to abort",
    "", "the program just before program termination.",
    "USEDEBUG", NULL,
    "", "Specifies that any debugging information in the executable file",
    "", "should be used to obtain additional source-level information.",
    "USEMMAP", NULL,
    "", "Specifies that the library should use mmap() instead of sbrk() to",
    "", "allocate user memory on UNIX platforms.",
    NULL
};


/* Perform a case-insensitive comparison between two option keywords.
 */

static
int
matchoption(char *s, char *t)
{
#if TARGET != TARGET_AMIGA && TARGET != TARGET_WINDOWS && \
    TARGET != TARGET_NETWARE
    int d;
#endif /* TARGET */

#if TARGET == TARGET_AMIGA || TARGET == TARGET_WINDOWS || \
    TARGET == TARGET_NETWARE
    return !stricmp(s, t);
#else /* TARGET */
    while (((d = toupper(*s) - toupper(*t)) == 0) && (*s != '\0'))
    {
        s++;
        t++;
    }
    return (d == 0);
#endif /* TARGET */
}


/* Convert a string representation of a number to an integer,
 * reporting any errors that occur during the conversion.
 */

static
size_t
readnumber(char *s, long *n, int u)
{
    char *t;
    int e;

    e = errno;
    errno = 0;
    while (isspace(*s))
        s++;
    if ((u == 1) && (*s == '-'))
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "ignoring negative number `%s'\n",
                   s);
        t = s;
    }
    else if ((u == 0) && (*s == '-') && (s[1] == '0') && ((s[2] == 'b') ||
             (s[2] == 'B')))
        /* This is a negative binary number.
         */
        *n = -strtol(s + 3, &t, 2);
    else if ((*s == '0') && ((s[1] == 'b') || (s[1] == 'B')))
    {
        /* This is a positive binary number.
         */
        if (u == 0)
            *n = strtol(s + 2, &t, 2);
        else
            *n = strtoul(s + 2, &t, 2);
    }
    /* Otherwise let the conversion function work out the number base
     * from the prefix.
     */
    else if (u == 0)
        *n = strtol(s, &t, 0);
    else
        *n = strtoul(s, &t, 0);
    if (errno == ERANGE)
        __mp_warn(ET_MAX, AT_MAX, NULL, 0, "%s number overflow in `%s'\n",
                  ((u == 0) && (*n == LONG_MIN)) ? "negative" : "positive", s);
    errno = e;
    return (size_t) (t - s);
}


/* Convert a string representation of a numeric range to two unsigned integers,
 * ensuring that the first integer is less than or equal to the second.  An
 * open range at either end is represented by -1.
 */

static
int
readrange(char *s, unsigned long *l, unsigned long *u, unsigned long *d)
{
    char *p, *t;
    unsigned long n;
    int w;
    char b, c;

    w = 0;
    *l = *u = (unsigned long) -1;
    *d = 1;
    for (p = s; (*p != '/') && (*p != '\0'); p++);
    b = *p;
    *p = '\0';
    for (t = s; (*t != '-') && (*t != '\0'); t++);
    c = *t;
    *t = '\0';
    /* If there was a number before the minus sign then read it.
     */
    if ((*s != '\0') && (s[readnumber(s, (long *) l, 1)] != '\0'))
    {
        *l = (unsigned long) -1;
        w = 1;
    }
    else if (c == '\0')
        *u = *l;
    else
    {
        s = t + 1;
        /* If there was a number after the minus sign then read it too.
         */
        if ((*s != '\0') && (s[readnumber(s, (long *) u, 1)] != '\0'))
        {
            *u = (unsigned long) -1;
            w = 1;
        }
    }
    if ((w == 0) && (b == '/'))
    {
        s = p + 1;
        /* If there was a forward slash then read the number after it as well.
         */
        if ((*s != '\0') && (s[readnumber(s, (long *) d, 1)] != '\0'))
        {
            *d = 1;
            w = 1;
        }
    }
    if (w != 0)
        return 0;
    /* If one or the other of the integers was zero (but not both) then convert
     * it to an open range.
     */
    if ((*l == 0) && (*u != 0))
        *l = (unsigned long) -1;
    else if ((*l != 0) && (*u == 0))
        *u = (unsigned long) -1;
    /* If the frequency was zero then convert it to one.
     */
    if (*d == 0)
        *d = 1;
    /* Swap the integers if the first number is greater than the second.
     */
    if ((*l != (unsigned long) -1) && (*u != (unsigned long) -1) && (*l > *u))
    {
        n = *l;
        *l = *u;
        *u = n;
    }
    return 1;
}


/* Display the quick-reference help summary.
 */

static
void
showoptions(void)
{
    char **s, **t;

    __mp_diag("Available options:\n\n");
    for (s = options_help, t = s + 1; *s != NULL; s += 2, t += 2)
        if (**s != '\0')
        {
            __mp_diag("    %s", *s);
            if (*t != NULL)
                __mp_diag("=<%s>", *t);
            __mp_diag("\n");
        }
        else
            __mp_diag("\t%s\n", *t);
    __mp_diag("\n");
}


/* The main option parsing routine.
 */

MP_GLOBAL
void
__mp_parseoptions(infohead *h)
{
    char *a, *f, *o, *p, *s, *t;
    unsigned long d, m, n;
    int i, l, q;

    l = 0;
    f = p = t = NULL;
    if (((s = __mp_getenv(MP_OPTIONS)) == NULL) || (*s == '\0'))
        return;
    if (strlen(s) + 1 > sizeof(options))
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: environment variable too "
                   "long\n", MP_OPTIONS);
        return;
    }
    /* We shouldn't modify the original string returned by __mp_getenv() since
     * that would modify the environment, and it may be placed in read-only
     * memory anyway.
     */
    strcpy(options, s);
    s = options;
    while (*s != '\0')
    {
        i = 0;
        while (isspace(*s))
            s++;
        if (*s == '\0')
            break;
        if (*s != '=')
        {
            /* Scan the option keyword.
             */
            for (o = s, q = 0; ((q == 1) || !isspace(*s)) && (*s != '\0') &&
                 (*s != '='); s++)
                if (*s == '"')
                {
                    /* Remove any quotes from the keyword.
                     */
                    __mp_memcopy(s, s + 1, strlen(s));
                    q = (q == 0) ? 1 : 0;
                    s--;
                }
            if ((*s != '\0') && (*s != '='))
            {
                *s++ = '\0';
                i = 1;
            }
        }
        else
            o = "";
        if ((*s == '=') && (i == 0))
        {
            /* Scan the option value.
             */
            *s++ = '\0';
            for (a = s, q = 0; ((q == 1) || !isspace(*s)) && (*s != '\0'); s++)
                if (*s == '"')
                {
                    /* Remove any quotes from the value.
                     */
                    __mp_memcopy(s, s + 1, strlen(s));
                    q = (q == 0) ? 1 : 0;
                    s--;
                }
            if (*s != '\0')
                *s++ = '\0';
        }
        else
            a = "";
        if (*o != '\0')
        {
            /* We now have an option keyword and possibly an associated
             * value, so we can now check for valid keywords.
             */
            i = OE_UNRECOGNISED;
            switch (toupper(*o))
            {
              case 'A':
                if (matchoption(o, "ALLOCBYTE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else if (n > 0xFF)
                        i = OE_BIGNUMBER;
                    else
                    {
                        h->alloc.abyte = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "ALLOCSTOP"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->astop = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "ALLOWOFLOW"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_ALLOWOFLOW;
                }
                else if (matchoption(o, "AUTOSAVE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->prof.autosave = n;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'C':
                if (matchoption(o, "CHECK"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (!readrange(a, &m, &n, &d))
                        i = OE_BADRANGE;
                    else
                    {
                        h->lrange = m;
                        h->urange = n;
                        h->check = d;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "CHECKALL"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_CHECKALLOCS | FLG_CHECKREALLOCS |
                                FLG_CHECKFREES | FLG_CHECKMEMORY;
                }
                else if (matchoption(o, "CHECKALLOCS"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_CHECKALLOCS;
                }
                else if (matchoption(o, "CHECKFORK"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_CHECKFORK;
                }
                else if (matchoption(o, "CHECKFREES"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_CHECKFREES;
                }
                else if (matchoption(o, "CHECKMEMORY"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_CHECKMEMORY;
                }
                else if (matchoption(o, "CHECKREALLOCS"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_CHECKREALLOCS;
                }
                break;
              case 'D':
                if (matchoption(o, "DEFALIGN"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if ((a[readnumber(a, (long *) &n, 1)] != '\0') ||
                             (n == 0))
                        i = OE_BADNUMBER;
                    else if (n > h->alloc.heap.memory.page)
                        i = OE_BIGNUMBER;
                    else
                    {
                        h->alloc.heap.memory.align = __mp_poweroftwo(n);
                        i = OE_RECOGNISED;
                    }
                break;
              case 'E':
                if (matchoption(o, "EDIT"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
#if TARGET == TARGET_UNIX
                    __mp_diagflags &= ~FLG_LIST;
                    __mp_diagflags |= FLG_EDIT;
#endif /* TARGET */
                }
                break;
              case 'F':
                if (matchoption(o, "FAILFREQ"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->ffreq = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "FAILSEED"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->fseed = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "FREEBYTE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else if (n > 0xFF)
                        i = OE_BIGNUMBER;
                    else
                    {
                        h->alloc.fbyte = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "FREESTOP"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->fstop = n;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'H':
                if (matchoption(o, "HELP"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    l = 1;
                }
                else if (matchoption(o, "HTML"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    __mp_diagflags |= FLG_HTMLNEXT;
                }
                break;
              case 'L':
                if (matchoption(o, "LARGEBOUND"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        if (n == 0)
                            h->prof.lbound = MP_LARGEBOUND;
                        else
                            h->prof.lbound = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "LEAKTABLE"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_LEAKTABLE;
                }
                else if (matchoption(o, "LIMIT"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->limit = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "LIST"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
#if TARGET == TARGET_UNIX
                    __mp_diagflags &= ~FLG_EDIT;
                    __mp_diagflags |= FLG_LIST;
#endif /* TARGET */
                }
                else if (matchoption(o, "LOGALL"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_LOGALLOCS | FLG_LOGREALLOCS | FLG_LOGFREES |
                                FLG_LOGMEMORY;
                }
                else if (matchoption(o, "LOGALLOCS"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_LOGALLOCS;
                }
                else if (matchoption(o, "LOGFILE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else
                    {
                        f = a;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "LOGFREES"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_LOGFREES;
                }
                else if (matchoption(o, "LOGMEMORY"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_LOGMEMORY;
                }
                else if (matchoption(o, "LOGREALLOCS"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_LOGREALLOCS;
                }
                break;
              case 'M':
                if (matchoption(o, "MEDIUMBOUND"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        if (n == 0)
                            h->prof.mbound = MP_MEDIUMBOUND;
                        else
                            h->prof.mbound = n;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'N':
                if (matchoption(o, "NOFREE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        if (h->alloc.fmax = n)
                            h->alloc.flags |= FLG_NOFREE;
                        else
                            h->alloc.flags &= ~FLG_NOFREE;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "NOPROTECT"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_NOPROTECT;
                }
                break;
              case 'O':
                if (matchoption(o, "OFLOWBYTE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else if (n > 0xFF)
                        i = OE_BIGNUMBER;
                    else
                    {
                        h->alloc.obyte = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "OFLOWSIZE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if ((a[readnumber(a, (long *) &n, 1)] != '\0') ||
                             ((n == 0) && (h->alloc.flags & FLG_PAGEALLOC)))
                        i = OE_BADNUMBER;
                    else
                    {
                        h->alloc.oflow = __mp_poweroftwo(n);
                        if (h->alloc.flags & FLG_PAGEALLOC)
                            h->alloc.oflow = __mp_roundup(h->alloc.oflow,
                                             h->alloc.heap.memory.page);
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "OFLOWWATCH"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
#if MP_WATCH_SUPPORT
                    h->alloc.flags |= FLG_OFLOWWATCH;
#endif /* MP_WATCH_SUPPORT */
                }
                break;
              case 'P':
                if (matchoption(o, "PAGEALLOC"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (matchoption(a, "LOWER"))
                    {
#if MP_PROTECT_SUPPORT
                        h->alloc.flags |= FLG_PAGEALLOC;
                        if (h->alloc.oflow == 0)
                            h->alloc.oflow = 1;
                        h->alloc.oflow = __mp_roundup(h->alloc.oflow,
                                         h->alloc.heap.memory.page);
#endif /* MP_PROTECT_SUPPORT */
                        i = OE_RECOGNISED;
                    }
                    else if (matchoption(a, "UPPER"))
                    {
#if MP_PROTECT_SUPPORT
                        h->alloc.flags |= FLG_PAGEALLOC | FLG_ALLOCUPPER;
                        if (h->alloc.oflow == 0)
                            h->alloc.oflow = 1;
                        h->alloc.oflow = __mp_roundup(h->alloc.oflow,
                                         h->alloc.heap.memory.page);
#endif /* MP_PROTECT_SUPPORT */
                        i = OE_RECOGNISED;
                    }
                    else
                        i = OE_LOWERORUPPER;
                else if (matchoption(o, "PRESERVE"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->alloc.flags |= FLG_PRESERVE;
                }
                else if (matchoption(o, "PROF"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->prof.profiling = 1;
                }
                else if (matchoption(o, "PROFFILE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else
                    {
                        p = a;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "PROGFILE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else
                    {
                        h->alloc.heap.memory.prog = a;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'R':
                if (matchoption(o, "REALLOCSTOP"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->rstop = n;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'S':
                if (matchoption(o, "SAFESIGNALS"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SAFESIGNALS;
                }
                else if (matchoption(o, "SHOWALL"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SHOWFREE | FLG_SHOWFREED | FLG_SHOWUNFREED |
                                FLG_SHOWMAP | FLG_SHOWSYMBOLS;
                }
                else if (matchoption(o, "SHOWFREE"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SHOWFREE;
                }
                else if (matchoption(o, "SHOWFREED"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SHOWFREED;
                }
                else if (matchoption(o, "SHOWMAP"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SHOWMAP;
                }
                else if (matchoption(o, "SHOWSYMBOLS"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SHOWSYMBOLS;
                }
                else if (matchoption(o, "SHOWUNFREED"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->flags |= FLG_SHOWUNFREED;
                }
                else if (matchoption(o, "SMALLBOUND"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        if (n == 0)
                            h->prof.sbound = MP_SMALLBOUND;
                        else
                            h->prof.sbound = n;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'T':
                if (matchoption(o, "TRACE"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->trace.tracing = 1;
                    h->alloc.heap.tracing = 1;
                }
                else if (matchoption(o, "TRACEFILE"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else
                    {
                        t = a;
                        i = OE_RECOGNISED;
                    }
                break;
              case 'U':
                if (matchoption(o, "UNFREEDABORT"))
                    if (*a == '\0')
                        i = OE_NOARGUMENT;
                    else if (a[readnumber(a, (long *) &n, 1)] != '\0')
                        i = OE_BADNUMBER;
                    else
                    {
                        h->uabort = n;
                        i = OE_RECOGNISED;
                    }
                else if (matchoption(o, "USEDEBUG"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
                    h->syms.lineinfo = 1;
                }
                else if (matchoption(o, "USEMMAP"))
                {
                    if (*a != '\0')
                        i = OE_IGNARGUMENT;
                    else
                        i = OE_RECOGNISED;
#if MP_MMAP_SUPPORT
                    if (h->alloc.list.size == 0)
                        h->alloc.heap.memory.flags |= FLG_USEMMAP;
#endif /* MP_MMAP_SUPPORT */
                }
                break;
              default:
                break;
            }
            /* Now check the error code returned from attempting to match
             * the keyword and report if anything went wrong.
             */
            switch (i)
            {
              case OE_UNRECOGNISED:
                if (*a == '\0')
                    __mp_error(ET_MAX, AT_MAX, NULL, 0, "unrecognised option "
                               "`%s'\n", o);
                else
                    __mp_error(ET_MAX, AT_MAX, NULL, 0, "unrecognised option "
                               "`%s=%s'\n", o, a);
                break;
              case OE_NOARGUMENT:
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "missing argument for "
                           "option `%s'\n", o);
                break;
              case OE_BADNUMBER:
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "bad numeric argument `%s' "
                           "for option `%s'\n", a, o);
                break;
              case OE_BADRANGE:
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "bad numeric range `%s' "
                           "for option `%s'\n", a, o);
                break;
              case OE_BIGNUMBER:
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "numeric argument `%s' is "
                           "too large for option `%s'\n", a, o);
                break;
              case OE_LOWERORUPPER:
                __mp_error(ET_MAX, AT_MAX, NULL, 0, "must specify `LOWER' or "
                           "`UPPER' for option `%s'\n", o);
                break;
              case OE_IGNARGUMENT:
                __mp_warn(ET_MAX, AT_MAX, NULL, 0, "ignoring argument `%s' for "
                          "option `%s'\n", a, o);
                break;
              default:
                break;
            }
        }
        else if (*a != '\0')
            __mp_warn(ET_MAX, AT_MAX, NULL, 0, "missing option for argument "
                      "`%s'\n", a);
    }
    /* Check the validity of the profiling allocation boundaries.  There is
     * potential for error if either of the small or large bounds overlap the
     * medium bound and the medium bound is either 1 or the maximum sized
     * integer, but it will just result in wrong profiling and nothing more.
     */
    if (h->prof.sbound >= h->prof.mbound)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "small allocation boundary `%lu' "
                   "overlaps medium allocation boundary `%lu'\n",
                   h->prof.sbound, h->prof.mbound);
        h->prof.sbound = h->prof.mbound - 1;
    }
    if (h->prof.lbound <= h->prof.mbound)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "large allocation boundary `%lu' "
                   "overlaps medium allocation boundary `%lu'\n",
                   h->prof.lbound, h->prof.mbound);
        h->prof.lbound = h->prof.mbound + 1;
    }
    /* Show the quick-reference option summary if it was requested.
     */
    if (l != 0)
        showoptions();
    /* Set up the filenames of the log, profiling and tracing files if they
     * were overridden.
     */
    if (f != NULL)
        h->log = __mp_logfile(&h->alloc.heap.memory, f);
    if (p != NULL)
        h->prof.file = __mp_proffile(&h->alloc.heap.memory, p);
    if (t != NULL)
        h->trace.file = __mp_tracefile(&h->alloc.heap.memory, t);
}


/* Set mpatrol flags after the library has been initialised.
 */

static
unsigned long
setflags(infohead *h, unsigned long f, int u)
{
    unsigned long i;

    if (f == 0)
        return 0;
    for (i = 1; i != 0; i <<= 1)
        if (f & i)
        {
            f &= ~i;
            switch (i)
            {
              case OPT_CHECKALLOCS:
                if (u == 0)
                    h->flags |= FLG_CHECKALLOCS;
                else
                    h->flags &= ~FLG_CHECKALLOCS;
                break;
              case OPT_CHECKREALLOCS:
                if (u == 0)
                    h->flags |= FLG_CHECKREALLOCS;
                else
                    h->flags &= ~FLG_CHECKREALLOCS;
                break;
              case OPT_CHECKFREES:
                if (u == 0)
                    h->flags |= FLG_CHECKFREES;
                else
                    h->flags &= ~FLG_CHECKFREES;
                break;
              case OPT_CHECKMEMORY:
                if (u == 0)
                    h->flags |= FLG_CHECKMEMORY;
                else
                    h->flags &= ~FLG_CHECKMEMORY;
                break;
              case OPT_LOGALLOCS:
                if (u == 0)
                    h->flags |= FLG_LOGALLOCS;
                else
                    h->flags &= ~FLG_LOGALLOCS;
                break;
              case OPT_LOGREALLOCS:
                if (u == 0)
                    h->flags |= FLG_LOGREALLOCS;
                else
                    h->flags &= ~FLG_LOGREALLOCS;
                break;
              case OPT_LOGFREES:
                if (u == 0)
                    h->flags |= FLG_LOGFREES;
                else
                    h->flags &= ~FLG_LOGFREES;
                break;
              case OPT_LOGMEMORY:
                if (u == 0)
                    h->flags |= FLG_LOGMEMORY;
                else
                    h->flags &= ~FLG_LOGMEMORY;
                break;
              case OPT_SHOWMAP:
                if (u == 0)
                    h->flags |= FLG_SHOWMAP;
                else
                    h->flags &= ~FLG_SHOWMAP;
                break;
              case OPT_SHOWSYMBOLS:
                if (u == 0)
                    h->flags |= FLG_SHOWSYMBOLS;
                else
                    h->flags &= ~FLG_SHOWSYMBOLS;
                break;
              case OPT_SHOWFREE:
                if (u == 0)
                    h->flags |= FLG_SHOWFREE;
                else
                    h->flags &= ~FLG_SHOWFREE;
                break;
              case OPT_SHOWFREED:
                if (u == 0)
                    h->flags |= FLG_SHOWFREED;
                else
                    h->flags &= ~FLG_SHOWFREED;
                break;
              case OPT_SHOWUNFREED:
                if (u == 0)
                    h->flags |= FLG_SHOWUNFREED;
                else
                    h->flags &= ~FLG_SHOWUNFREED;
                break;
              case OPT_LEAKTABLE:
                if (u == 0)
                {
                    h->flags |= FLG_LEAKTABLE;
                    h->ltable.tracing = 1;
                }
                else
                {
                    h->flags &= ~FLG_LEAKTABLE;
                    h->ltable.tracing = 0;
                }
                break;
              case OPT_ALLOWOFLOW:
                if (u == 0)
                    h->flags |= FLG_ALLOWOFLOW;
                else
                    h->flags &= ~FLG_ALLOWOFLOW;
                break;
              case OPT_CHECKFORK:
                if (u == 0)
                    h->flags |= FLG_CHECKFORK;
                else
                    h->flags &= ~FLG_CHECKFORK;
                break;
              case OPT_EDIT:
                if (u == 0)
                {
#if TARGET == TARGET_UNIX
                    __mp_diagflags &= ~FLG_LIST;
                    __mp_diagflags |= FLG_EDIT;
#endif /* TARGET */
                }
                else
                {
#if TARGET == TARGET_UNIX
                    __mp_diagflags &= ~FLG_EDIT;
#endif /* TARGET */
                }
                break;
              case OPT_LIST:
                if (u == 0)
                {
#if TARGET == TARGET_UNIX
                    __mp_diagflags &= ~FLG_EDIT;
                    __mp_diagflags |= FLG_LIST;
#endif /* TARGET */
                }
                else
                {
#if TARGET == TARGET_UNIX
                    __mp_diagflags &= ~FLG_LIST;
#endif /* TARGET */
                }
                break;
              default:
                f |= i;
                break;
            }
        }
    return f;
}


/* Set an mpatrol option after the library has been initialised.
 */

MP_GLOBAL
unsigned long
__mp_set(infohead *h, unsigned long o, unsigned long v)
{
    unsigned long r;

    r = 0;
    switch (o)
    {
      case OPT_HELP:
        showoptions();
        break;
      case OPT_SETFLAGS:
        r = setflags(h, v, 0);
        break;
      case OPT_UNSETFLAGS:
        r = setflags(h, v, 1);
        break;
      case OPT_ALLOCSTOP:
        h->astop = v;
        break;
      case OPT_REALLOCSTOP:
        h->rstop = v;
        break;
      case OPT_FREESTOP:
        h->fstop = v;
        break;
      case OPT_ALLOCBYTE:
        if (v > 0xFF)
            v = 0xFF;
        h->alloc.abyte = v;
        break;
      case OPT_DEFALIGN:
        if ((v == 0) || (v > h->alloc.heap.memory.page))
            r = o;
        else
            h->alloc.heap.memory.align = __mp_poweroftwo(v);
        break;
      case OPT_LIMIT:
        h->limit = v;
        break;
      case OPT_FAILFREQ:
        h->ffreq = v;
        break;
      case OPT_FAILSEED:
        if (v == 0)
            v = (unsigned long) time(NULL);
        srand((unsigned int) v);
        h->fseed = v;
        break;
      case OPT_UNFREEDABORT:
        h->uabort = v;
        break;
      case OPT_AUTOSAVE:
        if (h->prof.autocount > 0)
            __mp_writeprofile(&h->prof, !(h->flags & FLG_NOPROTECT));
        h->prof.autosave = v;
        break;
      case OPT_CHECKLOWER:
        h->lrange = v;
        break;
      case OPT_CHECKUPPER:
        h->urange = v;
        break;
      case OPT_CHECKFREQ:
        if (v == 0)
            v = 1;
        h->check = v;
        break;
      case OPT_NOFREE:
        while (h->alloc.flist.size > v)
            __mp_recyclefreed(&h->alloc);
        if (h->alloc.fmax = v)
            h->alloc.flags |= FLG_NOFREE;
        else
            h->alloc.flags &= ~FLG_NOFREE;
        break;
      default:
        r = o;
        break;
    }
    return r;
}


/* Get mpatrol flags after the library has been initialised.
 */

static
unsigned long
getflags(infohead *h)
{
    unsigned long f;

    f = 0;
    if (h->flags & FLG_CHECKALLOCS)
        f |= OPT_CHECKALLOCS;
    if (h->flags & FLG_CHECKREALLOCS)
        f |= OPT_CHECKREALLOCS;
    if (h->flags & FLG_CHECKFREES)
        f |= OPT_CHECKFREES;
    if (h->flags & FLG_CHECKMEMORY)
        f |= OPT_CHECKMEMORY;
    if (h->flags & FLG_LOGALLOCS)
        f |= OPT_LOGALLOCS;
    if (h->flags & FLG_LOGREALLOCS)
        f |= OPT_LOGREALLOCS;
    if (h->flags & FLG_LOGFREES)
        f |= OPT_LOGFREES;
    if (h->flags & FLG_LOGMEMORY)
        f |= OPT_LOGMEMORY;
    if (h->flags & FLG_SHOWMAP)
        f |= OPT_SHOWMAP;
    if (h->flags & FLG_SHOWSYMBOLS)
        f |= OPT_SHOWSYMBOLS;
    if (h->flags & FLG_SHOWFREE)
        f |= OPT_SHOWFREE;
    if (h->flags & FLG_SHOWFREED)
        f |= OPT_SHOWFREED;
    if (h->flags & FLG_SHOWUNFREED)
        f |= OPT_SHOWUNFREED;
    if (h->flags & FLG_LEAKTABLE)
        f |= OPT_LEAKTABLE;
    if (h->flags & FLG_ALLOWOFLOW)
        f |= OPT_ALLOWOFLOW;
    if (h->prof.profiling)
        f |= OPT_PROF;
    if (h->trace.tracing)
        f |= OPT_TRACE;
    if (h->flags & FLG_SAFESIGNALS)
        f |= OPT_SAFESIGNALS;
    if (h->flags & FLG_NOPROTECT)
        f |= OPT_NOPROTECT;
    if (h->flags & FLG_CHECKFORK)
        f |= OPT_CHECKFORK;
    if (h->alloc.flags & FLG_PRESERVE)
        f |= OPT_PRESERVE;
    if (h->alloc.flags & FLG_OFLOWWATCH)
        f |= OPT_OFLOWWATCH;
    if (h->alloc.flags & FLG_PAGEALLOC)
        f |= OPT_PAGEALLOC;
    if (h->alloc.flags & FLG_ALLOCUPPER)
        f |= OPT_ALLOCUPPER;
    if (h->alloc.heap.memory.flags & FLG_USEMMAP)
        f |= OPT_USEMMAP;
    if (h->syms.lineinfo)
        f |= OPT_USEDEBUG;
    if (__mp_diagflags & FLG_EDIT)
        f |= OPT_EDIT;
    if (__mp_diagflags & FLG_LIST)
        f |= OPT_LIST;
    if (__mp_diagflags & FLG_HTML)
        f |= OPT_HTML;
    return f;
}


/* Get an mpatrol option after the library has been initialised.
 */

MP_GLOBAL
int
__mp_get(infohead *h, unsigned long o, unsigned long *v)
{
    int r;

    r = 1;
    switch (o)
    {
      case OPT_HELP:
        *v = 0;
        break;
      case OPT_SETFLAGS:
        *v = getflags(h);
        break;
      case OPT_UNSETFLAGS:
        *v = ~getflags(h);
        break;
      case OPT_ALLOCSTOP:
        *v = h->astop;
        break;
      case OPT_REALLOCSTOP:
        *v = h->rstop;
        break;
      case OPT_FREESTOP:
        *v = h->fstop;
        break;
      case OPT_ALLOCBYTE:
        *v = h->alloc.abyte;
        break;
      case OPT_FREEBYTE:
        *v = h->alloc.fbyte;
        break;
      case OPT_OFLOWBYTE:
        *v = h->alloc.obyte;
        break;
      case OPT_OFLOWSIZE:
        *v = h->alloc.oflow;
        break;
      case OPT_DEFALIGN:
        *v = h->alloc.heap.memory.align;
        break;
      case OPT_LIMIT:
        *v = h->limit;
        break;
      case OPT_FAILFREQ:
        *v = h->ffreq;
        break;
      case OPT_FAILSEED:
        *v = h->fseed;
        break;
      case OPT_UNFREEDABORT:
        *v = h->uabort;
        break;
      case OPT_LOGFILE:
        *v = (unsigned long) h->log;
        break;
      case OPT_PROFFILE:
        *v = (unsigned long) h->prof.file;
        break;
      case OPT_TRACEFILE:
        *v = (unsigned long) h->trace.file;
        break;
      case OPT_PROGFILE:
        *v = (unsigned long) h->alloc.heap.memory.prog;
        break;
      case OPT_AUTOSAVE:
        *v = h->prof.autosave;
        break;
      case OPT_CHECKLOWER:
        *v = h->lrange;
        break;
      case OPT_CHECKUPPER:
        *v = h->urange;
        break;
      case OPT_CHECKFREQ:
        *v = h->check;
        break;
      case OPT_NOFREE:
        *v = h->alloc.fmax;
        break;
      case OPT_SMALLBOUND:
        *v = h->prof.sbound;
        break;
      case OPT_MEDIUMBOUND:
        *v = h->prof.mbound;
        break;
      case OPT_LARGEBOUND:
        *v = h->prof.lbound;
        break;
      default:
        r = 0;
        break;
    }
    return r;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
