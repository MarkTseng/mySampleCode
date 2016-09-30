#ifndef MP_INTER_H
#define MP_INTER_H


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
 * Library interface.  This module defines the visible interface for the
 * mpatrol library.
 */


/*
 * $Id$
 */


#include "config.h"
#include "info.h"
#include "diag.h"
#include <stdarg.h>


/* An allocinfo structure provides information about a particular memory
 * allocation.  This must be kept up to date with the definition of
 * __mp_allocinfo in mpatrol.h.
 */

typedef struct allocinfo
{
    void *block;           /* pointer to block of memory */
    size_t size;           /* size of block of memory */
    alloctype type;        /* type of memory allocation */
    unsigned long alloc;   /* allocation index */
    unsigned long realloc; /* reallocation index */
    unsigned long thread;  /* thread identifier */
    unsigned long event;   /* event of last modification */
    char *func;            /* calling function name */
    char *file;            /* file name in which call took place */
    unsigned long line;    /* line number at which call took place */
    addrnode *stack;       /* call stack details */
    char *typestr;         /* type stored in allocation */
    size_t typesize;       /* size of type stored in allocation */
    void *userdata;        /* user data associated with allocation */
    int allocated : 1;     /* allocation was allocated */
    int freed : 1;         /* allocation has been freed */
    int marked : 1;        /* allocation has been marked */
    int profiled : 1;      /* allocation has been profiled */
    int traced : 1;        /* allocation has been traced */
    int internal : 1;      /* allocation is internal */
}
allocinfo;


/* A symbolinfo structure provides information about a particular symbol.
 * This must be kept up to date with the definition of __mp_symbolinfo in
 * mpatrol.h.
 */

typedef struct symbolinfo
{
    char *name;         /* symbol name */
    char *object;       /* module symbol located in */
    void *addr;         /* start address */
    size_t size;        /* size of symbol */
    char *file;         /* file name corresponding to address */
    unsigned long line; /* line number corresponding to address */
}
symbolinfo;


/* A heapinfo structure provides statistics about the current state of the
 * heap.  This must be kept up to date with the definition of __mp_heapinfo
 * in mpatrol.h
 */

typedef struct heapinfo
{
    size_t acount; /* total number of allocated blocks */
    size_t atotal; /* total size of allocated blocks */
    size_t fcount; /* total number of free blocks */
    size_t ftotal; /* total size of free blocks */
    size_t gcount; /* total number of freed blocks */
    size_t gtotal; /* total size of freed blocks */
    size_t icount; /* total number of internal blocks */
    size_t itotal; /* total size of internal blocks */
    size_t mcount; /* total number of marked blocks */
    size_t mtotal; /* total size of marked blocks */
}
heapinfo;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_API extern errortype __mp_errno;


MP_API void __mp_init(void);
MP_API void __mp_reinit(void);
MP_API void __mp_fini(void);
MP_API void __mp_trap(void);
MP_API int __mp_atexit(void (*)(void));
MP_API unsigned long __mp_setoption(long, unsigned long);
MP_API int __mp_getoption(long, unsigned long *);
MP_API infohead *__mp_memhead(void);
MP_API void *__mp_alloc(size_t, size_t, alloctype, char *, char *,
                        unsigned long, char *, size_t, size_t);
MP_API char *__mp_strdup(char *, size_t, alloctype, char *, char *,
                         unsigned long, size_t);
MP_API void *__mp_realloc(void *, size_t, size_t, alloctype, char *, char *,
                          unsigned long, char *, size_t, size_t);
MP_API void __mp_free(void *, alloctype, char *, char *, unsigned long, size_t);
MP_API void *__mp_setmem(void *, size_t, unsigned char, alloctype, char *,
                         char *, unsigned long, size_t);
MP_API void *__mp_copymem(void *, void *, size_t, unsigned char, alloctype,
                          char *, char *, unsigned long, size_t);
MP_API void *__mp_locatemem(void *, size_t, void *, size_t, alloctype, char *,
                            char *, unsigned long, size_t);
MP_API int __mp_comparemem(void *, void *, size_t, alloctype, char *, char *,
                           unsigned long, size_t);
MP_API unsigned long __mp_libversion(void);
MP_API char *__mp_strerror(errortype);
MP_API char *__mp_function(alloctype);
MP_API int __mp_setuser(void *, void *);
MP_API int __mp_setmark(void *);
MP_API int __mp_info(void *, allocinfo *);
MP_API int __mp_syminfo(void *, symbolinfo *);
MP_API char *__mp_symbol(void *);
MP_API int __mp_printinfo(void *);
MP_API unsigned long __mp_snapshot(void);
MP_API size_t __mp_iterate(int (*)(void *, void *), void *, unsigned long);
MP_API size_t __mp_iterateall(int (*)(void *, void *), void *);
MP_API int __mp_addallocentry(char *, unsigned long, size_t);
MP_API int __mp_addfreeentry(char *, unsigned long, size_t);
MP_API void __mp_clearleaktable(void);
MP_API int __mp_startleaktable(void);
MP_API int __mp_stopleaktable(void);
MP_API void __mp_leaktable(size_t, int, unsigned char);
MP_API void __mp_memorymap(int);
MP_API void __mp_summary(void);
MP_API int __mp_stats(heapinfo *);
MP_API void __mp_checkheap(char *, char *, unsigned long);
MP_API void __mp_check(void);
MP_API prologuehandler __mp_prologue(prologuehandler);
MP_API epiloguehandler __mp_epilogue(epiloguehandler);
MP_API nomemoryhandler __mp_nomemory(nomemoryhandler);
MP_API void __mp_pushdelstack(char *, char *, unsigned long);
MP_API void __mp_popdelstack(char **, char **, unsigned long *);
MP_API int __mp_printf(char *, ...);
MP_API int __mp_vprintf(char *, va_list);
MP_API void __mp_printfwithloc(char *, char *, unsigned long, char *, ...);
MP_API void __mp_vprintfwithloc(char *, char *, unsigned long, char *, va_list);
MP_API void __mp_logmemory(void *, size_t);
MP_API int __mp_logstack(size_t);
MP_API int __mp_logaddr(void *);
MP_API int __mp_edit(char *, unsigned long);
MP_API int __mp_list(char *, unsigned long);
MP_API int __mp_view(char *, unsigned long);
MP_API int __mp_readcontents(char *, void *);
MP_API int __mp_writecontents(char *, void *);
MP_API long __mp_cmpcontents(char *, void *);
MP_API int __mp_remcontents(char *, void *);
MP_API void __cyg_profile_func_enter(void *, void *);
MP_API void __cyg_profile_func_exit(void *, void *);
MP_API void chkr_set_right(void *, size_t, unsigned char);
MP_API void chkr_copy_bitmap(void *, void *, size_t);
MP_API void chkr_check_addr(void *, size_t, unsigned char);
MP_API void chkr_check_str(char *, unsigned char);
MP_API void chkr_check_exec(void *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_INTER_H */
