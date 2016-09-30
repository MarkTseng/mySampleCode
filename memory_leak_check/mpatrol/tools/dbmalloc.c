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
 * Dbmalloc-compatible interface.  Implements Dbmalloc functions using
 * mpatrol.  Dbmalloc is copyright (C) 1990-1992 Conor P. Cahill.
 */


#include "config.h"
#include "dbmalloc.h"
#include <stdio.h>
#if TARGET == TARGET_WINDOWS
#include <io.h>
#else /* TARGET */
#include <unistd.h>
#endif /* TARGET */


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *dbmalloc_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


/* The structure used to pass information to the callback function from
 * __mp_iterate() when __mpt_dbmallocdump() and __mpt_dbmalloclist() are
 * called.
 */

typedef struct listinfo
{
    int file;            /* file descriptor */
    unsigned long event; /* upper event bound */
    int header : 1;      /* header output flag */
    int dump : 1;        /* dump output flag */
}
listinfo;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* Indicates if this module has been initialised.
 */

static int malloc_initialised;


/* Specifies that more details should be shown when __mpt_dbmallocdump() is
 * called.
 */

static int malloc_detail;


/* Display the header for __mpt_dbmallocdump() and __mpt_dbmalloclist().
 */

static
void
printheader(int f)
{
    char b[70 + (sizeof(void *) * 2)];
    size_t i;

    for (i = 0; i < 22 + sizeof(void *); i++)
        b[i] = '*';
    strcpy(b + i, " Dump of Malloc Chain ");
    i += strlen(b + i);
    while (i < 68 + (sizeof(void *) * 2))
        b[i++] = '*';
    b[i++] = '\n';
    b[i] = '\0';
    if (f > 0)
        write(f, b, i);
    else
        __mp_printf(b);
    for (i = 0; i < sizeof(void *) - 4; i++)
        b[i] = ' ';
    strcpy(b + i, "POINTER");
    i += strlen(b + i);
    while (i < 4 + (sizeof(void *) * 2))
        b[i++] = ' ';
    strcpy(b + i, "FILE  WHERE         LINE      ALLOC        DATA     "
           "HEX DUMP\n");
    i += strlen(b + i);
    if (f > 0)
        write(f, b, i);
    else
        __mp_printf(b);
    for (i = 0; i < sizeof(void *) - 4; i++)
        b[i] = ' ';
    strcpy(b + i, "TO DATA");
    i += strlen(b + i);
    while (i < 5 + (sizeof(void *) * 2))
        b[i++] = ' ';
    strcpy(b + i, "ALLOCATED         NUMBER     FUNCT       LENGTH  "
           "OF BYTES 1-7\n");
    i += strlen(b + i);
    if (f > 0)
        write(f, b, i);
    else
        __mp_printf(b);
    for (i = 0; i < sizeof(void *) * 2; i++)
        b[i] = '-';
    b[i++] = ' ';
    while (i < 21 + (sizeof(void *) * 2))
        b[i++] = '-';
    b[i++] = ' ';
    while (i < 29 + (sizeof(void *) * 2))
        b[i++] = '-';
    b[i++] = ' ';
    while (i < 44 + (sizeof(void *) * 2))
        b[i++] = '-';
    b[i++] = ' ';
    while (i < 52 + (sizeof(void *) * 2))
        b[i++] = '-';
    b[i++] = ' ';
    while (i < 67 + (sizeof(void *) * 2))
        b[i++] = '-';
    b[i++] = '\n';
    b[i] = '\0';
    if (f > 0)
        write(f, b, i);
    else
        __mp_printf(b);
}


/* The callback function that is called by __mp_iterate() for every heap
 * allocation that has changed since a specified heap event.
 */

static
int
callback(MP_CONST void *p, void *t)
{
    char b[69 + (sizeof(void *) * 2)];
    char m[64];
    listinfo *i;
    __mp_allocstack *a;
    __mp_allocinfo d;
    __mp_symbolinfo s;
    size_t j, n;

    if (!__mp_info(p, &d))
        return 0;
    i = (listinfo *) t;
    if ((d.event <= i->event) && (!d.freed || (i->dump && malloc_detail)) &&
        (!d.marked || i->dump))
    {
        if (!i->header)
        {
            printheader(i->file);
            i->header = 1;
        }
        sprintf(b, "%0*lX ", sizeof(void *) * 2, d.block);
        n = strlen(b);
        if (d.file != NULL)
        {
            sprintf(m, "%7lu", d.line);
            sprintf(b + n, "%-20.20s %7.7s ", d.file, m);
        }
        else
            sprintf(b + n, "%-28s ", "unknown");
        n += strlen(b + n);
        sprintf(m, "%s(%lu)", __mp_function(d.type), d.alloc);
        sprintf(b + n, "%-14.14s ", m);
        n += strlen(b + n);
        sprintf(m, "%7lu", d.size);
        sprintf(b + n, "%7.7s ", m);
        n += strlen(b + n);
        for (j = 0; (j < 7) && (j < d.size); j++)
            sprintf(m + (j << 1), "%02X", ((unsigned char *) d.block)[j]);
        m[j << 1] = '\0';
        sprintf(b + n, "%s\n", m);
        n += strlen(b + n);
        if (i->file > 0)
            write(i->file, b, n);
        else
            __mp_printf(b);
        for (a = d.stack; a != NULL; a = a->next)
        {
            for (n = 0; n <= sizeof(void *) * 2; n++)
                b[n] = ' ';
            strcpy(b + n, "-> ");
            n += 3;
            if (__mp_syminfo(a->addr, &s))
                if (i->file > 0)
                {
                    write(i->file, b, n);
                    write(i->file, s.name, strlen(s.name));
                    if (s.file != NULL)
                    {
                        write(i->file, " in ", 4);
                        write(i->file, s.file, strlen(s.file));
                        sprintf(b, "(%lu)\n", s.line);
                        write(i->file, b, strlen(b));
                    }
                    else
                        write(i->file, "\n", 1);
                }
                else if (s.file != NULL)
                    __mp_printf("%s%s in %s(%lu)\n", b, s.name, s.file, s.line);
                else
                    __mp_printf("%s%s\n", b, s.name);
            else
            {
                sprintf(b + n, "%0*lX\n", sizeof(void *) * 2, a->addr);
                n += strlen(b + n);
                if (i->file > 0)
                    write(i->file, b, n);
                else
                    __mp_printf(b);
            }
        }
        return 1;
    }
    return 0;
}


/* Set a malloc library option.
 */

int
__mpt_dbmallocoption(int c, union dbmalloptarg *v)
{
    char *s;
    unsigned long n;
    int r;

    if (!malloc_initialised)
        __mp_init_dbmalloc();
    r = 0;
    switch (c)
    {
      case MALLOC_ERRFILE:
        if (strcmp(v->str, "-") == 0)
            s = "stderr";
        else
            s = v->str;
        r = __mp_setoption(MP_OPT_LOGFILE, (unsigned long) s);
        break;
      case MALLOC_CKCHAIN:
        if (v->i != 0)
            n = (unsigned long) -1;
        else
            n = 0;
        if (((r = __mp_setoption(MP_OPT_CHECKLOWER, n)) == 0) &&
            ((r = __mp_setoption(MP_OPT_CHECKUPPER, n)) == 0))
            r = __mp_setoption(MP_OPT_CHECKFREQ, 1);
        break;
      case MALLOC_FILLAREA:
        if (v->i == 0)
            n = 0;
        else if (!__mp_getoption(MP_OPT_OFLOWSIZE, &n) || (n == 0))
            n = 1;
        if ((r = __mp_setoption(MP_OPT_OFLOWSIZE, n)) == 0)
        {
            if ((v->i >= 0) && (v->i <= 2))
                c = MP_OPT_SETFLAGS;
            else
                c = MP_OPT_UNSETFLAGS;
            r = __mp_setoption(c, MP_FLG_PRESERVE);
        }
        break;
      case MALLOC_REUSE:
        if (v->i != 0)
            n = 0;
        else
            n = ~0L;
        r = __mp_setoption(MP_OPT_NOFREE, n);
        break;
      case MALLOC_DETAIL:
        malloc_detail = v->i;
        break;
      case MALLOC_ZERO:
        if (v->i != 0)
            c = MP_OPT_SETFLAGS;
        else
            c = MP_OPT_UNSETFLAGS;
        if (__mp_setoption(c, MP_FLG_CHECKALLOCS))
            r = 1;
        break;
      default:
        r = 1;
        break;
    }
    return r;
}


/* Display a malloc library error message.
 */

void
__mpt_dbmallocperror(MP_CONST char *s)
{
    if ((s != NULL) && (*s != '\0'))
        fprintf(stderr, "%s: ", s);
    if ((s = __mp_strerror(__mp_errno)) == NULL)
        s = "unknown error";
    fprintf(stderr, "%s\n", s);
}


/* Verify that the malloc chain is still intact and the heap has not been
 * corrupted.
 */

int
__mpt_dbmallocchaincheck(int f, MP_CONST char *s, MP_CONST char *t,
                         unsigned long u)
{
    if (!malloc_initialised)
        __mp_init_dbmalloc();
    __mp_checkheap(s, t, u);
    return 0;
}


/* Display all of the heap allocations and their associated data.
 */

void
__mpt_dbmallocdump(int f)
{
    listinfo i;

    if (!malloc_initialised)
        __mp_init_dbmalloc();
    i.file = f;
    i.event = __mp_snapshot();
    i.header = 0;
    i.dump = 1;
    __mp_iterate(callback, &i, 0);
    if (f > 0)
        write(f, "\n", 1);
    else if (i.header)
        __mp_printf("\n");
}


/* Display some of the heap allocations and their associated data.
 */

void
__mpt_dbmalloclist(int f, unsigned long l, unsigned long u)
{
    listinfo i;

    if (!malloc_initialised)
        __mp_init_dbmalloc();
    i.file = f;
    if (l <= u)
        i.event = u;
    else
    {
        i.event = l;
        l = u;
        u = i.event;
    }
    i.header = 0;
    i.dump = 0;
    __mp_iterate(callback, &i, l);
    if (f > 0)
        write(f, "\n", 1);
    else if (i.header)
        __mp_printf("\n");
}


/* Return the number of bytes of heap memory currently in use and optionally
 * return the current malloc history id.
 */

unsigned long
__mpt_dbmallocinuse(unsigned long *h)
{
    __mp_heapinfo i;
    unsigned long t;

    if (!malloc_initialised)
        __mp_init_dbmalloc();
    if (__mp_stats(&i))
        t = i.atotal - i.mtotal;
    else
        t = 0;
    if (h != NULL)
        *h = __mp_snapshot();
    return t;
}


/* Return the size in bytes of the memory allocation that contains a specified
 * address.
 */

size_t
__mpt_dbmallocsize(MP_CONST void *p)
{
    __mp_allocinfo i;
    size_t t;

    if (!malloc_initialised)
        __mp_init_dbmalloc();
    if (__mp_info(p, &i) && i.allocated && !i.freed)
        t = i.size;
    else
        t = (size_t) -1;
    return t;
}


/* Initialise the dbmalloc module.
 */

void
__mp_init_dbmalloc(void)
{
    char *v;
    union dbmalloptarg a;
    long n;

    if (!malloc_initialised)
    {
        malloc_initialised = 1;
        malloc_detail = 0;
        if ((v = getenv("MALLOC_BOUNDSIZE")) && (*v != '\0'))
        {
            if ((n = strtol(v, NULL, 10)) < 1)
                n = 1;
            __mp_setoption(MP_OPT_OFLOWSIZE, n);
        }
        if ((v = getenv("MALLOC_FILLBYTE")) && (*v != '\0'))
        {
            if (((n = strtol(v, NULL, 10)) < 0) || (n > 255))
                n = 1;
            __mp_setoption(MP_OPT_ALLOCBYTE, n);
        }
        if ((v = getenv("MALLOC_FREEBYTE")) && (*v != '\0'))
        {
            if (((n = strtol(v, NULL, 10)) < 0) || (n > 255))
                n = 2;
            __mp_setoption(MP_OPT_FREEBYTE, n);
        }
        if ((v = getenv("MALLOC_WARN")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_WARN, &a);
        }
        if ((v = getenv("MALLOC_FATAL")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_FATAL, &a);
        }
        if ((v = getenv("MALLOC_ERRFILE")) && (*v != '\0'))
        {
            a.str = v;
            __mpt_dbmallocoption(MALLOC_ERRFILE, &a);
        }
        if ((v = getenv("MALLOC_CKCHAIN")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_CKCHAIN, &a);
        }
        if ((v = getenv("MALLOC_FILLAREA")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_FILLAREA, &a);
        }
        if ((v = getenv("MALLOC_LOWFRAG")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_LOWFRAG, &a);
        }
        if ((v = getenv("MALLOC_CKDATA")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_CKDATA, &a);
        }
        if ((v = getenv("MALLOC_REUSE")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_REUSE, &a);
        }
        if ((v = getenv("MALLOC_SHOWLINKS")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_SHOWLINKS, &a);
        }
        if ((v = getenv("MALLOC_DETAIL")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_DETAIL, &a);
        }
        if ((v = getenv("MALLOC_FREEMARK")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_FREEMARK, &a);
        }
        if ((v = getenv("MALLOC_ZERO")) && (*v != '\0'))
        {
            a.i = strtol(v, NULL, 10);
            __mpt_dbmallocoption(MALLOC_ZERO, &a);
        }
    }
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
