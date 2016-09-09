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
 * Dmalloc-compatible interface.  Implements Dmalloc functions using
 * mpatrol.  Dmalloc is copyright (C) 1992-2001 Gray Watson.
 */


#include "config.h"
#include "dmalloc.h"
#include <stdio.h>
#include <ctype.h>
#include <time.h>


#if MP_IDENT_SUPPORT
#ident "$Id: dmalloc.c,v 1.16 2002/01/08 20:05:10 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *dmalloc_id = "$Id: dmalloc.c,v 1.16 2002/01/08 20:05:10 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


/* Specify whether to prefix every log message produced by
 * __mpt_dmallocmessage() and __mpt_dmallocvmessage() with the current
 * time in numerical form and/or string form.  The current event number
 * can also be logged as well.
 */

#define LOG_TIME_NUMBER     1
#define LOG_CTIME_STRING    0
#define LOG_ITERATION_COUNT 1


/* The escape characters that should be given special consideration in
 * bytestring().
 */

#define ESCAPE_CHARS   "\\\a\b\f\n\r\t\v"
#define ESCAPE_REPLACE "\\abfnrtv"


/* The structure used to store information about each debugging token
 * recognised by the Dmalloc library.
 */

typedef struct tokeninfo
{
    char *longname;     /* long token name */
    char *shortname;    /* short token name */
    unsigned long flag; /* associated flag */
}
tokeninfo;


/* The structure used to pass information to the callback function from
 * __mp_iterate() when __mpt_dmalloclogchanged() is called.
 */

typedef struct listinfo
{
    unsigned long kcount; /* known count */
    unsigned long ktotal; /* known total */
    unsigned long ucount; /* unknown count */
    unsigned long utotal; /* unknown total */
    int unfreed : 1;      /* log unfreed allocations */
    int freed : 1;        /* log freed allocations */
    int details : 1;      /* log pointer details */
}
listinfo;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The table containing all of the recognised debugging tokens with their
 * associated flags.
 */

static tokeninfo tokens[] =
{
    {"none", "nil", 0},
    {"log-stats", "lst", DMALLOC_LOG_STATS},
    {"log-non-free", "lnf", DMALLOC_LOG_NONFREE},
    {"log-known", "lkn", DMALLOC_LOG_KNOWN},
    {"log-trans", "ltr", DMALLOC_LOG_TRANS},
    {"log-admin", "lad", DMALLOC_LOG_ADMIN},
    {"log-blocks", "lbl", DMALLOC_LOG_BLOCKS},
    {"log-bad-space", "lbs", DMALLOC_LOG_BAD_SPACE},
    {"log-nonfree-space", "lns", DMALLOC_LOG_NONFREE_SPACE},
    {"check-fence", "cfe", DMALLOC_CHECK_FENCE},
    {"check-heap", "che", DMALLOC_CHECK_HEAP},
    {"check-lists", "cli", DMALLOC_CHECK_LISTS},
    {"check-blank", "cbl", DMALLOC_CHECK_BLANK},
    {"check-funcs", "cfu", DMALLOC_CHECK_FUNCS},
    {"force-linear", "fli", DMALLOC_FORCE_LINEAR},
    {"catch-signals", "csi", DMALLOC_CATCH_SIGNALS},
    {"log-elapsed-time", "let", DMALLOC_LOG_ELAPSED_TIME},
    {"log-current-time", "lct", DMALLOC_LOG_CURRENT_TIME},
    {"realloc-copy", "rco", DMALLOC_REALLOC_COPY},
    {"free-blank", "fbl", DMALLOC_FREE_BLANK},
    {"error-abort", "eab", DMALLOC_ERROR_ABORT},
    {"alloc-blank", "abl", DMALLOC_ALLOC_BLANK},
    {"heap-check-map", "hcm", DMALLOC_HEAP_CHECK_MAP},
    {"print-messages", "pme", DMALLOC_PRINT_MESSAGES},
    {"catch-null", "cnu", DMALLOC_CATCH_NULL},
    {"never-reuse", "nre", DMALLOC_NEVER_REUSE},
    {"allow-free-null", "afn", DMALLOC_ALLOW_FREE_NULL},
    {"error-dump", "edu", DMALLOC_ERROR_DUMP},
    {NULL, NULL, 0}
};


/* Indicates if this module has been initialised.
 */

static int malloc_initialised;


/* The time at which this module was initialised.
 */

static time_t malloc_time;


/* The library debug flags.
 */

static unsigned long malloc_flags;


/* The point at which to start checking the heap and the frequency at which
 * to check it.
 */

static unsigned long malloc_start, malloc_interval;


/* The pointer to the callback function registered with __mpt_dmalloctrack().
 */

static dmalloc_track_t malloc_tracker;


/* The previous mpatrol prologue and epilogue handlers.
 */

static __mp_prologuehandler old_prologue;
static __mp_epiloguehandler old_epilogue;


/* The pointer, size and alignment obtained each time our prologue function is
 * called.  This is then used by our epilogue function, but we don't need to
 * worry about nested calls to the prologue function since the mpatrol library
 * guarantees that it will never occur, even when there are multiple threads.
 */

static void *malloc_pointer;
static size_t malloc_size;
static size_t malloc_align;


/* The global variables which control the behaviour of the library and are
 * part of the Dmalloc library interface.  The last two are intended for use
 * within a debugger.
 */

char *dmalloc_logpath;
void *dmalloc_address;
unsigned long dmalloc_address_count;


/* Read any library options from the DMALLOC_OPTIONS environment variable.
 */

static
void
readoptions(void)
{
    static char b[1024];
    tokeninfo *i;
    char *p, *s, *t;

    if (((s = getenv("DMALLOC_OPTIONS")) == NULL) || (*s == '\0'))
        return;
    strncpy(b, s, sizeof(b) - 1);
    b[sizeof(b) - 1] = '\0';
    for (s = t = b; t != NULL; s = t + 1)
    {
        if (t = strchr(s, ','))
            *t = '\0';
        if (*s == '\0')
            continue;
        if ((strncmp(s, "addr", 4) == 0) && (s[4] == '='))
        {
            if (p = strchr(s + 5, ':'))
                *p = '\0';
            dmalloc_address = (void *) strtoul(s + 5, NULL, 16);
            if (p != NULL)
                dmalloc_address_count = strtoul(p + 1, NULL, 10);
        }
        else if ((strncmp(s, "debug", 5) == 0) && (s[5] == '='))
            malloc_flags = strtoul(s + 6, NULL, 16);
        else if ((strncmp(s, "inter", 5) == 0) && (s[5] == '='))
        {
            if ((malloc_interval = strtoul(s + 6, NULL, 10)) == 0)
                malloc_interval = 1;
        }
        else if ((strncmp(s, "log", 3) == 0) && (s[3] == '='))
            dmalloc_logpath = s + 4;
        else if ((strncmp(s, "start", 5) == 0) && (s[5] == '='))
            if (strchr(s + 6, ':') == NULL)
                malloc_start = strtoul(s + 6, NULL, 10);
            else
                malloc_start = 0;
        else
            for (i = tokens; i->longname != NULL; i++)
                if ((strcmp(s, i->longname) == 0) ||
                    (strcmp(s, i->shortname) == 0))
                {
                    malloc_flags |= i->flag;
                    break;
                }
    }
}


/* Map the Dmalloc options to mpatrol options.
 */

static
void
setoptions(void)
{
    unsigned long v;

    if (dmalloc_logpath != NULL)
    {
        __mp_setoption(MP_OPT_LOGFILE, (unsigned long) dmalloc_logpath);
        dmalloc_logpath = NULL;
    }
    v = MP_FLG_SHOWFREE;
    if (malloc_flags & DMALLOC_LOG_STATS)
        __mp_setoption(MP_OPT_SETFLAGS, v);
    else
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    v = MP_FLG_SHOWUNFREED;
    if (malloc_flags & DMALLOC_LOG_NONFREE)
        __mp_setoption(MP_OPT_SETFLAGS, v);
    else
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    v = MP_FLG_LOGALLOCS | MP_FLG_LOGREALLOCS | MP_FLG_LOGFREES;
    if (malloc_flags & DMALLOC_LOG_TRANS)
        __mp_setoption(MP_OPT_SETFLAGS, v);
    else
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    if (malloc_flags & DMALLOC_CHECK_FENCE)
    {
        if (!__mp_getoption(MP_OPT_OFLOWSIZE, &v) || (v == 0))
            v = sizeof(void *);
    }
    else
        v = 0;
    __mp_setoption(MP_OPT_OFLOWSIZE, v);
    if (malloc_flags & DMALLOC_CHECK_HEAP)
    {
        v = (unsigned long) -1;
        __mp_setoption(MP_OPT_CHECKLOWER, malloc_start);
    }
    else
    {
        v = 0;
        __mp_setoption(MP_OPT_CHECKLOWER, 0);
    }
    __mp_setoption(MP_OPT_CHECKUPPER, v);
    __mp_setoption(MP_OPT_CHECKFREQ, malloc_interval);
    v = MP_FLG_SAFESIGNALS;
    if (malloc_flags & DMALLOC_CATCH_SIGNALS)
        __mp_setoption(MP_OPT_SETFLAGS, v);
    else
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    v = MP_FLG_PRESERVE;
    if (malloc_flags & DMALLOC_FREE_BLANK)
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    else
        __mp_setoption(MP_OPT_SETFLAGS, v);
    v = MP_FLG_SHOWMAP;
    if (malloc_flags & DMALLOC_HEAP_CHECK_MAP)
        __mp_setoption(MP_OPT_SETFLAGS, v);
    else
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    if (malloc_flags & DMALLOC_NEVER_REUSE)
        v = ~0L;
    else
        v = 0;
    __mp_setoption(MP_OPT_NOFREE, v);
    v = MP_FLG_CHECKFREES;
    if (malloc_flags & DMALLOC_ALLOW_FREE_NULL)
        __mp_setoption(MP_OPT_UNSETFLAGS, v);
    else
        __mp_setoption(MP_OPT_SETFLAGS, v);
}


/* Record the pointer, size and alignment for later processing by the epilogue
 * function and possibly also call the old prologue function if one was
 * installed.
 */

static
void
prologue(MP_CONST void *p, size_t l, size_t m, MP_CONST char *s,
         MP_CONST char *t, unsigned long u, MP_CONST void *a)
{
    if (old_prologue != NULL)
        old_prologue(p, l, m, s, t, u, a);
    malloc_pointer = (void *) p;
    malloc_size = l;
    malloc_align = m;
}


/* Call the tracker function with any relevant details if one has been
 * registered and possibly also call the old epilogue function if one was
 * installed.
 */

static
void
epilogue(MP_CONST void *p, MP_CONST char *s, MP_CONST char *t, unsigned long u,
         MP_CONST void *a)
{
    size_t l;

    if (dmalloc_logpath != NULL)
    {
        __mp_setoption(MP_OPT_LOGFILE, (unsigned long) dmalloc_logpath);
        dmalloc_logpath = NULL;
    }
    if (malloc_tracker != NULL)
    {
        if (malloc_pointer == (void *) -1)
            malloc_tracker(t, u, DMALLOC_FUNC_MALLOC, malloc_size, malloc_align,
                           NULL, p);
        else if (malloc_size == (size_t) -1)
            malloc_tracker(t, u, DMALLOC_FUNC_FREE, 0, 0, malloc_pointer, NULL);
        else if (malloc_size == (size_t) -2)
        {
            if (malloc_pointer == NULL)
                l = 0;
            else
                l = strlen((char *) malloc_pointer) + 1;
            malloc_tracker(t, u, DMALLOC_FUNC_STRDUP, l, malloc_align, NULL, p);
        }
        else if (malloc_pointer == NULL)
            malloc_tracker(t, u, DMALLOC_FUNC_MALLOC, malloc_size, malloc_align,
                           NULL, p);
        else if (malloc_size == 0)
            malloc_tracker(t, u, DMALLOC_FUNC_FREE, 0, 0, malloc_pointer, NULL);
        else
            malloc_tracker(t, u, DMALLOC_FUNC_REALLOC, malloc_size,
                           malloc_align, malloc_pointer, p);
    }
    if (old_epilogue != NULL)
        old_epilogue(p, s, t, u, a);
}


/* Convert the bytes in a memory allocation to human-readable form.
 */

static
char *
bytestring(char *b, size_t s, char *p, size_t l)
{
    char *t;
    size_t i, n;

    for (i = n = 0; (i < s) && (i < l); i++)
        if (p[i] == '\0')
        {
            b[n++] = '\\';
            b[n++] = '0';
            if ((i < s - 1) && (i < l - 1))
            {
                b[n++] = '0';
                b[n++] = '0';
            }
        }
        else if (t = strchr(ESCAPE_CHARS, p[i]))
        {
            b[n++] = '\\';
            b[n++] = ESCAPE_REPLACE[t - ESCAPE_CHARS];
        }
        else if (!isprint(p[i]))
        {
            sprintf(b + n, "\\%03o", p[i]);
            n += 4;
        }
        else
            b[n++] = p[i];
    b[n] = '\0';
    return b;
}


/* The callback function that is called by __mp_iterate() for every heap
 * allocation that has changed since a specified heap event.
 */

static
int
callback(MP_CONST void *p, void *t)
{
    char b[1024];
    char s[81];
    listinfo *i;
    __mp_allocinfo d;

    if (!__mp_info(p, &d))
        return 0;
    i = (listinfo *) t;
    if ((d.freed && i->freed) || (!d.freed && i->unfreed))
    {
        if (d.file != NULL)
        {
            i->kcount++;
            i->ktotal += d.size;
        }
        else
        {
            i->ucount++;
            i->utotal += d.size;
        }
        if ((d.file != NULL) || !(malloc_flags & DMALLOC_LOG_KNOWN))
        {
            if (d.file != NULL)
            {
                if (i->details)
                    sprintf(b, "%s:%lu", d.file, d.line);
                __mp_addallocentry(d.file, d.line, d.size);
                if (d.freed)
                    __mp_addfreeentry(d.file, d.line, d.size);
            }
            else if (d.func != NULL)
            {
                if (i->details)
                    strcpy(b, d.func);
                __mp_addallocentry(d.func, 0, d.size);
                if (d.freed)
                    __mp_addfreeentry(d.func, 0, d.size);
            }
            else if (d.stack == NULL)
            {
                if (i->details)
                    strcpy(b, "unknown");
                __mp_addallocentry(NULL, 0, d.size);
                if (d.freed)
                    __mp_addfreeentry(NULL, 0, d.size);
            }
            else if (d.stack->name != NULL)
            {
                if (i->details)
                    strcpy(b, d.stack->name);
                __mp_addallocentry(d.stack->name, 0, d.size);
                if (d.freed)
                    __mp_addfreeentry(d.stack->name, 0, d.size);
            }
            else
            {
                if (i->details)
                    sprintf(b, "ra=%#lx", d.stack->addr);
                __mp_addallocentry(NULL, (unsigned long) d.stack->addr, d.size);
                if (d.freed)
                    __mp_addfreeentry(NULL, (unsigned long) d.stack->addr,
                                      d.size);
            }
            if (i->details)
            {
                __mpt_dmallocmessage(" %s: '%#lx' (%lu byte%s) from '%s'\n",
                                     d.freed ? "freed" : "not freed", d.block,
                                     d.size, (d.size == 1) ? "" : "s", b);
                if (malloc_flags & DMALLOC_LOG_NONFREE_SPACE)
                    __mpt_dmallocmessage("  dump of '%#lx': '%s'\n", d.block,
                                         bytestring(s, 20, (char *) d.block,
                                                    d.size));
            }
        }
        return 1;
    }
    return 0;
}


/* Terminate the dmalloc module.
 */

void
__mpt_dmallocshutdown(void)
{
    time_t t;
    unsigned long h, m, s;

    if (!malloc_initialised)
        return;
    __mp_prologue(old_prologue);
    __mp_epilogue(old_epilogue);
    if ((t = time(NULL)) != (time_t) -1)
    {
        s = (unsigned long) t - (unsigned long) malloc_time;
        h = s / 3600;
        m = (s / 60) % 60;
        s %= 60;
    }
    else
    {
        t = (time_t) 0;
        h = m = s = 0;
    }
    __mpt_dmallocmessage("ending time = %lu, elapsed since start = "
                         "%lu:%02lu:%02lu\n\n", (unsigned long) t, h, m, s);
    malloc_initialised = 0;
}


/* Check the integrity of the memory allocation containing a given pointer,
 * or the entire heap if the pointer is NULL.
 */

int
__mpt_dmallocverify(MP_CONST void *p, MP_CONST char *s, MP_CONST char *t,
                    unsigned long u)
{
    __mp_allocinfo i;
    int r;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    if ((p == NULL) || (__mp_info(p, &i) && (p == i.block) && i.allocated &&
         !i.freed))
        r = 1;
    else
        r = 0;
    __mp_checkheap(s, t, u);
    return r;
}


/* Set the library debug flags and return the previous setting.
 */

unsigned long
__mpt_dmallocdebug(unsigned long f)
{
    unsigned long r;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    r = malloc_flags;
    malloc_flags = f;
    setoptions();
    return r;
}


/* Return the current library debug flags.
 */

unsigned long
__mpt_dmallocdebugcurrent(void)
{
    if (!malloc_initialised)
        __mp_init_dmalloc();
    return malloc_flags;
}


/* Examine a pointer in the heap and return information about the memory
 * allocation it belongs to.
 */

int
__mpt_dmallocexamine(MP_CONST void *p, size_t *l, char **t, unsigned long *u,
                     void **a)
{
    __mp_allocinfo i;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    if (__mp_info(p, &i) && i.allocated)
    {
        if (l != NULL)
            *l = i.size;
        if (t != NULL)
            *t = i.file;
        if (u != NULL)
            *u = i.line;
        if (a != NULL)
            if (i.stack != NULL)
                *a = i.stack->addr;
            else
                *a = NULL;
        return 1;
    }
    return 0;
}


/* Write a message to the log file.
 */

void
__mpt_dmallocmessage(MP_CONST char *s, ...)
{
    va_list v;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    va_start(v, s);
    __mpt_dmallocvmessage(s, v);
    va_end(v);
}


/* Write a message to the log file.
 */

void
__mpt_dmallocvmessage(MP_CONST char *s, va_list v)
{
    char b[1024];
    char m[64];
    char *c, *p;
#if LOG_TIME_NUMBER || LOG_CTIME_STRING
    time_t t;
#endif /* LOG_TIME_NUMBER && LOG_CTIME_STRING */
    size_t l;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    l = 0;
#if LOG_TIME_NUMBER || LOG_CTIME_STRING
    if ((t = time(NULL)) == (time_t) -1)
        t = (time_t) 0;
#endif /* LOG_TIME_NUMBER && LOG_CTIME_STRING */
#if LOG_TIME_NUMBER
    sprintf(m + l, "%lu: ", (unsigned long) t);
    l += strlen(m + l);
#endif /* LOG_TIME_NUMBER */
#if LOG_CTIME_STRING
    sprintf(m + l, "%24.24s: ", ctime(&t));
    l += strlen(m + l);
#endif /* LOG_CTIME_STRING */
#if LOG_ITERATION_COUNT
    sprintf(m + l, "%lu: ", __mp_snapshot());
    l += strlen(m + l);
#endif /* LOG_ITERATION_COUNT */
    m[l] = '\0';
    vsprintf(b, s, v);
    for (c = b; p = strchr(c, '\n'); c = p + 1)
    {
        *p = '\0';
        if (*c != '\0')
        {
            __mp_printf("%s%s\n", m, c);
            if (malloc_flags & DMALLOC_PRINT_MESSAGES)
                fprintf(stderr, "%s%s\n", m, c);
        }
        else
        {
            __mp_printf("\n");
            if (malloc_flags & DMALLOC_PRINT_MESSAGES)
                fputc('\n', stderr);
        }
    }
    if (*c != '\0')
    {
        __mp_printf("%s%s\n", m, c);
        if (malloc_flags & DMALLOC_PRINT_MESSAGES)
            fprintf(stderr, "%s%s\n", m, c);
    }
}


/* Register a callback function that will be called for each memory
 * allocation event.
 */

void
__mpt_dmalloctrack(dmalloc_track_t h)
{
    if (!malloc_initialised)
        __mp_init_dmalloc();
    malloc_tracker = h;
}


/* Log the details of any changes to the heap since a certain point.
 */

void
__mpt_dmalloclogchanged(unsigned long m, int u, int f, int d)
{
    char *s;
    listinfo i;
    int t;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    if ((u != 0) && (f != 0))
        s = "not-freed and freed";
    else if (u != 0)
        s = "not-freed";
    else if (f != 0)
        s = "freed";
    else
        return;
    __mpt_dmallocmessage("dumping %s pointers changed since %lu:\n", s, m);
    i.kcount = i.ktotal = 0;
    i.ucount = i.utotal = 0;
    i.unfreed = (u != 0) ? 1 : 0;
    i.freed = (f != 0) ? 1 : 0;
    i.details = (d != 0) ? 1 : 0;
    t = __mp_stopleaktable();
    __mp_clearleaktable();
    __mp_iterate(callback, &i, m);
    if (i.unfreed)
    {
        __mp_printf("\n");
        __mp_leaktable(0, MP_LT_UNFREED, 0);
    }
    if (i.freed)
    {
        __mp_printf("\n");
        __mp_leaktable(0, MP_LT_FREED, 0);
    }
    __mp_clearleaktable();
    if (t != 0)
        __mp_startleaktable();
    if ((i.kcount != 0) || (i.ucount != 0))
    {
        __mpt_dmallocmessage("\n");
        if (i.kcount != 0)
            __mpt_dmallocmessage(" known memory: %lu pointer%s, %lu byte%s\n",
                                 i.kcount, (i.kcount == 1) ? "" : "s", i.ktotal,
                                 (i.ktotal == 1) ? "" : "s");
        if (i.ucount != 0)
            __mpt_dmallocmessage(" unknown memory: %lu pointer%s, %lu byte%s\n",
                                 i.ucount, (i.ucount == 1) ? "" : "s", i.utotal,
                                 (i.utotal == 1) ? "" : "s");
    }
    __mpt_dmallocmessage("\n");
}


/* Return the string associated with a certain error value.
 */

MP_CONST char *
__mpt_dmallocstrerror(__mp_errortype e)
{
    MP_CONST char *s;

    if (!malloc_initialised)
        __mp_init_dmalloc();
    if ((s = __mp_strerror(e)) == NULL)
        s = "errno value is not valid";
    return s;
}


/* Initialise the dmalloc module.
 */

void
__mp_init_dmalloc(void)
{
    char *t;

    if (malloc_initialised)
        return;
    malloc_initialised = 1;
    if ((malloc_time = time(NULL)) == (time_t) -1)
        malloc_time = (time_t) 0;
    malloc_flags = 0;
    malloc_start = 0;
    malloc_interval = 1;
    malloc_tracker = NULL;
    dmalloc_logpath = NULL;
    dmalloc_address = NULL;
    dmalloc_address_count = 0;
    readoptions();
    setoptions();
    __mp_atexit(__mpt_dmallocshutdown);
    if (!__mp_getoption(MP_OPT_LOGFILE, (unsigned long *) &t) || (t == NULL))
        t = "stderr";
    __mpt_dmallocmessage("Dmalloc version '%lu.%lu.%lu' (mpatrol)\n",
                         DMALLOC_VERSION_MAJOR, DMALLOC_VERSION_MINOR,
                         DMALLOC_VERSION_PATCH);
    __mpt_dmallocmessage("flags = %#lx, logfile '%s'\n", malloc_flags, t);
    __mpt_dmallocmessage("interval = %lu, addr = %#lx, seen # = %lu\n",
                         malloc_interval, dmalloc_address,
                         dmalloc_address_count);
    __mpt_dmallocmessage("starting time = %lu\n\n",
                         (unsigned long) malloc_time);
    old_prologue = __mp_prologue(prologue);
    old_epilogue = __mp_epilogue(epilogue);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
