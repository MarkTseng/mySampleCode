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
 * Memory allocation tracing.  If tracing is enabled then all memory
 * allocations and deallocations are written to a file in a compact
 * format for later processing by a tracing tool.
 */


#include "trace.h"
#include "diag.h"
#include "utils.h"
#include "version.h"
#include <stdio.h>
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *trace_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


/* A name cache is used to record the most recently-used strings that have been
 * written to the tracing output file.  Such strings can be replaced with
 * indices into the name cache.  When the name cache is full, the cache entry
 * that was accessed least recently will be replaced by a new name.
 */

typedef struct namecache
{
    listnode node;        /* list node */
    char *name;           /* pointer to name */
    unsigned char index;  /* cache index */
}
namecache;


/* A rescache structure stores information about memory reservations on the
 * heap before the tracing output file has been opened.
 */

typedef struct rescache
{
    void *block;   /* pointer to block of memory */
    size_t size;   /* size of block of memory */
    char internal; /* allocation is internal */
}
rescache;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The arrays of cache entries for the function and file name caches.
 */

static namecache funcnames[MP_NAMECACHE_SIZE];
static namecache filenames[MP_NAMECACHE_SIZE];


/* The lists of used and free cache entries for the function and file name
 * caches.
 */

static listhead usedfuncs, freefuncs;
static listhead usedfiles, freefiles;


/* The cache that stores information about memory reservations on the heap
 * before the tracing output file has been opened.  The cachecounter variable
 * keeps a count of the number of entries stored in the cache.
 */

static rescache cache[MP_RESCACHE_SIZE];
static size_t cachecounter;


/* The file pointer to the tracing output file.  This should not really be
 * a file scope variable as it prevents this module from being re-entrant.
 * The traceready variable indicates when the tracing output file is ready
 * to be used.
 */

static FILE *tracefile;
static int traceready;


/* Initialise the fields of a tracehead so that the mpatrol library
 * is ready to trace memory allocations.
 */

MP_GLOBAL
void
__mp_newtrace(tracehead *t, memoryinfo *m)
{
    size_t i;

    t->file = __mp_tracefile(m, NULL);
    t->tracing = 0;
    __mp_newlist(&usedfuncs);
    __mp_newlist(&freefuncs);
    __mp_newlist(&usedfiles);
    __mp_newlist(&freefiles);
    for (i = 0; i < MP_NAMECACHE_SIZE; i++)
    {
        __mp_addtail(&freefuncs, &funcnames[i].node);
        __mp_addtail(&freefiles, &filenames[i].node);
        funcnames[i].name = filenames[i].name = NULL;
        funcnames[i].index = filenames[i].index = i + 1;
    }
    tracefile = NULL;
    traceready = 0;
}


/* Change the tracing output file and optionally terminate the old file.
 */

MP_GLOBAL
int
__mp_changetrace(tracehead *t, char *f, int e)
{
    size_t i;
    int r;
    char s;

    r = 1;
    s = t->tracing;
    if (e == 1)
        r = __mp_endtrace(t);
    else if ((tracefile == NULL) || (tracefile == stderr) ||
             (tracefile == stdout))
    {
        /* We don't want to close the stderr or stdout file streams so
         * we just flush them instead.  If the tracing file hasn't been set,
         * this will just flush all open output files.
         */
        if (fflush(tracefile))
            r = 0;
    }
    else if (fclose(tracefile))
        r = 0;
    t->file = f;
    t->tracing = s;
    __mp_newlist(&usedfuncs);
    __mp_newlist(&freefuncs);
    __mp_newlist(&usedfiles);
    __mp_newlist(&freefiles);
    for (i = 0; i < MP_NAMECACHE_SIZE; i++)
    {
        __mp_addtail(&freefuncs, &funcnames[i].node);
        __mp_addtail(&freefiles, &filenames[i].node);
        funcnames[i].name = filenames[i].name = NULL;
    }
    tracefile = NULL;
    traceready = 0;
    return r;
}


/* Finish tracing and attempt to close the tracing output file.
 */

MP_GLOBAL
int
__mp_endtrace(tracehead *t)
{
    char s[4];
    size_t i;
    int r;

    r = 1;
    traceready = 0;
    if ((t->tracing) && (tracefile != NULL))
    {
        __mp_memcopy(s, (char *) MP_TRACEMAGIC, 4);
        fwrite(s, sizeof(char), 4, tracefile);
    }
    if ((tracefile == NULL) || (tracefile == stderr) || (tracefile == stdout))
    {
        /* We don't want to close the stderr or stdout file streams so
         * we just flush them instead.  If the tracing file hasn't been set,
         * this will just flush all open output files.
         */
        if (fflush(tracefile))
            r = 0;
    }
    else if (fclose(tracefile))
        r = 0;
    tracefile = NULL;
    __mp_newlist(&usedfuncs);
    __mp_newlist(&freefuncs);
    __mp_newlist(&usedfiles);
    __mp_newlist(&freefiles);
    for (i = 0; i < MP_NAMECACHE_SIZE; i++)
    {
        __mp_addtail(&freefuncs, &funcnames[i].node);
        __mp_addtail(&freefiles, &filenames[i].node);
        funcnames[i].name = filenames[i].name = NULL;
    }
    t->file = NULL;
    t->tracing = 0;
    return r;
}


/* Check that there were no errors writing to the tracing output file.
 */

static
int
checktracefile(tracehead *t)
{
    if (ferror(tracefile))
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: problem writing tracing "
                   "file\n", t->file);
        __mp_endtrace(t);
        return 0;
    }
    return 1;
}


/* Attempt to open the tracing output file.
 */

static
int
opentracefile(tracehead *t)
{
    char s[4];
    void *b;
    size_t i, l;
    unsigned long v;

    /* The tracing file name can also be named as stderr and stdout which
     * will go to the standard error and standard output streams respectively.
     */
    if (t->file == NULL)
        return 0;
    else if (strcmp(t->file, "stderr") == 0)
        tracefile = stderr;
    else if (strcmp(t->file, "stdout") == 0)
        tracefile = stdout;
    else if ((tracefile = fopen(t->file, "wb")) == NULL)
    {
        __mp_error(ET_MAX, AT_MAX, NULL, 0, "%s: cannot open file\n", t->file);
        t->file = NULL;
        return 0;
    }
    i = 1;
    v = MP_VERNUM;
    __mp_memcopy(s, (char *) MP_TRACEMAGIC, 4);
    fwrite(s, sizeof(char), 4, tracefile);
    fwrite(&i, sizeof(size_t), 1, tracefile);
    fwrite(&v, sizeof(unsigned long), 1, tracefile);
    traceready = 1;
    /* Write out all of the entries in the memory reservation cache.  This
     * only needs to be done when the tracing output file is opened since all
     * subsequent tracing events will be written out directly.
     */
    for (i = 0; i < cachecounter; i++)
    {
        if (cache[i].internal)
            fputc('I', tracefile);
        else
            fputc('H', tracefile);
        b = __mp_encodeuleb128((unsigned long) cache[i].block, &l);
        fwrite(b, l, 1, tracefile);
        b = __mp_encodeuleb128(cache[i].size, &l);
        fwrite(b, l, 1, tracefile);
    }
    cachecounter = 0;
    return checktracefile(t);
}


/* Record a heap memory reservation for tracing.
 */

MP_GLOBAL
void
__mp_traceheap(void *a, size_t l, int i)
{
    void *b;
    size_t s;

    if (!traceready)
    {
        /* If the tracing output file has not yet been opened then it is
         * likely that the mpatrol library is still being initialised, in
         * which case it is unsafe to open the file due to the possibility
         * of recursion.  As a precautionary measure, we store the current
         * information in a cache that will be written out when the file is
         * finally opened.  If the cache is full, simply discard the current
         * information.
         */
        if (cachecounter < MP_RESCACHE_SIZE)
        {
            cache[cachecounter].block = a;
            cache[cachecounter].size = l;
            cache[cachecounter].internal = i;
            cachecounter++;
        }
        return;
    }
    if (i != 0)
        fputc('I', tracefile);
    else
        fputc('H', tracefile);
    /* Some of the following values are written as LEB128 numbers.  This is so
     * that the size of the tracing output file can be kept to a minimum.
     */
    b = __mp_encodeuleb128((unsigned long) a, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128(l, &s);
    fwrite(b, s, 1, tracefile);
}


/* Add a string to a specified name cache or return the index of an existing
 * string.
 */

static
unsigned char
addname(listhead *u, listhead *f, char *s)
{
    namecache *n;

    /* First check to see if the name already exists in the cache.
     */
    for (n = (namecache *) u->head; n->node.next != NULL;
         n = (namecache *) n->node.next)
        if ((*n->name == *s) && (strcmp(n->name + 1, s + 1) == 0))
        {
            /* If the entry is not at the front of the cache then move it there.
             */
            if (n != (namecache *) u->head)
            {
                __mp_remove(u, &n->node);
                __mp_addhead(u, &n->node);
            }
            return n->index;
        }
    /* If there are free entries in the cache then use the first one, otherwise
     * use the least-recently-used entry.
     */
    if (f->size > 0)
        n = (namecache *) __mp_remhead(f);
    else
        n = (namecache *) __mp_remtail(u);
    __mp_addhead(u, &n->node);
    n->name = s;
    return (unsigned char) (n->index | 0x80);
}


/* Write out a function name to the tracing output file.
 */

static
void
writefuncname(char *s)
{
    unsigned char n;

    if ((s != NULL) && (*s != '\0'))
    {
        n = addname(&usedfuncs, &freefuncs, s);
        fputc(n, tracefile);
        if (n & 0x80)
        {
            fputs(s, tracefile);
            fputc('\0', tracefile);
        }
    }
    else
        fputc('\0', tracefile);
}


/* Write out a file name to the tracing output file.
 */

static
void
writefilename(char *s)
{
    unsigned char n;

    if ((s != NULL) && (*s != '\0'))
    {
        n = addname(&usedfiles, &freefiles, s);
        fputc(n, tracefile);
        if (n & 0x80)
        {
            fputs(s, tracefile);
            fputc('\0', tracefile);
        }
    }
    else
        fputc('\0', tracefile);
}


/* Record a memory allocation for tracing.
 */

MP_GLOBAL
void
__mp_tracealloc(tracehead *t, unsigned long n, void *a, size_t l,
                unsigned long d, char *f, char *g, unsigned long u)
{
    void *b;
    size_t s;

    if ((tracefile == NULL) && !opentracefile(t))
        return;
    fputc('A', tracefile);
    /* Some of the following values are written as LEB128 numbers.  This is so
     * that the size of the tracing output file can be kept to a minimum.
     */
    b = __mp_encodeuleb128(n, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128((unsigned long) a, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128(l, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128(d, &s);
    fwrite(b, s, 1, tracefile);
    writefuncname(f);
    writefilename(g);
    b = __mp_encodeuleb128(u, &s);
    fwrite(b, s, 1, tracefile);
    checktracefile(t);
}


/* Record a memory reallocation for tracing.
 */

MP_GLOBAL
void
__mp_tracerealloc(tracehead *t, unsigned long n, void *a, size_t l,
                  unsigned long d, char *f, char *g, unsigned long u)
{
    void *b;
    size_t s;

    if ((tracefile == NULL) && !opentracefile(t))
        return;
    fputc('R', tracefile);
    /* Some of the following values are written as LEB128 numbers.  This is so
     * that the size of the tracing output file can be kept to a minimum.
     */
    b = __mp_encodeuleb128(n, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128((unsigned long) a, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128(l, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128(d, &s);
    fwrite(b, s, 1, tracefile);
    writefuncname(f);
    writefilename(g);
    b = __mp_encodeuleb128(u, &s);
    fwrite(b, s, 1, tracefile);
    checktracefile(t);
}


/* Record a memory deallocation for tracing.
 */

MP_GLOBAL
void
__mp_tracefree(tracehead *t, unsigned long n, unsigned long d, char *f, char *g,
               unsigned long u)
{
    void *b;
    size_t s;

    if ((tracefile == NULL) && !opentracefile(t))
        return;
    fputc('F', tracefile);
    /* Some of the following values are written as LEB128 numbers.  This is so
     * that the size of the tracing output file can be kept to a minimum.
     */
    b = __mp_encodeuleb128(n, &s);
    fwrite(b, s, 1, tracefile);
    b = __mp_encodeuleb128(d, &s);
    fwrite(b, s, 1, tracefile);
    writefuncname(f);
    writefilename(g);
    b = __mp_encodeuleb128(u, &s);
    fwrite(b, s, 1, tracefile);
    checktracefile(t);
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
