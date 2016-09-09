#ifndef MPT_DBMALLOC_H
#define MPT_DBMALLOC_H


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
 * Dbmalloc-compatible interface.  Implements Dbmalloc functions using
 * mpatrol.  Dbmalloc is copyright (C) 1990-1992 Conor P. Cahill.
 */


/*
 * $Id: dbmalloc.h,v 1.15 2002/01/08 20:05:10 graeme Exp $
 */


/*
 * This file provides Dbmalloc-compatible functions which are built on top
 * of the mpatrol library.  They are compatible with the last known public
 * release of Dbmalloc (patch level 14), but only the Dbmalloc-specific
 * functions are defined here, leaving the overriding of standard functions
 * up to the mpatrol library.  As the mpatrol library does not currently
 * override the C library string functions and the X toolkit heap allocation
 * functions, neither does this file.
 *
 * The dbmallopt() function does not support the setting of all of the
 * Dbmalloc options.  In fact, most of them do not make sense when applied
 * to the mpatrol library.  Some of them have slightly changed behaviour
 * due to the mapping process and some of them cannot be implemented due to
 * the mpatrol library having been initialised beforehand.
 *
 * The malloc_dump() function does not support the full recognition of the
 * MALLOC_DETAIL option in that it does not display the additional columns
 * and summary that the Dbmalloc library does.  This is because this would
 * make no sense when applied to the mpatrol library, but it does still
 * affect whether freed allocations are shown in the listing (although the
 * details for such allocations are slightly different, and there are no
 * entries displayed for free memory blocks).
 *
 * The output for the malloc_dump() and malloc_list() functions is almost
 * identical to that of the Dbmalloc library except for a slight change in
 * the pointer format when displaying the address of each memory allocation.
 * The stack information is obtained differently as well, since the mpatrol
 * library records symbolic stack tracebacks for each memory allocation.
 * As a result, malloc_enter() and malloc_leave() do nothing and the return
 * address in a stack frame is displayed if no associated symbol name, file
 * and line number could be determined.  Parentheses are not printed at the
 * end of symbol names so that they can be processed properly by a C++
 * demangler if necessary.  Passing a file descriptor of 0 to malloc_dump()
 * or malloc_list() results in the output being sent to the mpatrol log
 * file.
 *
 * The malloc_size() and malloc_mark() functions do not give an error
 * message if the pointer passed in does not correspond to a heap
 * allocation.  Neither of these functions automatically perform an
 * integrity check of the heap.  Note that the malloc_chain_check()
 * function will never return a non-zero value - it always terminates with
 * an error message in the mpatrol log file whenever it detects heap
 * corruption.  As a result, the malloc_abort() function is not used.
 *
 * This file is initialised via the mpatrol library's initialiser function
 * feature, which means that if the __mp_init_dbmalloc() function is noted
 * by the mpatrol symbol manager then it will be called when the mpatrol
 * library is being initialised.  If this feature is not supported then the
 * dbmallinit() function must be called as early on as possible, otherwise
 * this file will not be initialised until one of its functions are called.
 */


#include <mpatrol.h>


/* Commands for dbmallopt().  Some of them are ignored as they have no
 * meaning when used with the mpatrol library.
 */

#define MALLOC_WARN      100
#define MALLOC_FATAL     101
#define MALLOC_ERRFILE   102
#define MALLOC_CKCHAIN   103
#define MALLOC_FILLAREA  104
#define MALLOC_LOWFRAG   105
#define MALLOC_CKDATA    106
#define MALLOC_REUSE     107
#define MALLOC_SHOWLINKS 108
#define MALLOC_DETAIL    109
#define MALLOC_FREEMARK  110
#define MALLOC_ZERO      111


/* The settings for the MALLOC_WARN and MALLOC_FATAL options.
 */

#define M_HANDLE_IGNORE 0
#define M_HANDLE_ABORT  1
#define M_HANDLE_EXIT   2
#define M_HANDLE_CORE   3
#define M_HANDLE_DUMP   128


/* The errors reported by the library.  None of these are actually used
 * in this implementation and are here in case user code relies on them.
 */

#define M_CODE_CHAIN_BROKE  MP_ET_MAX
#define M_CODE_NO_END       MP_ET_MAX
#define M_CODE_BAD_PTR      MP_ET_NOTALL
#define M_CODE_BAD_MAGIC    MP_ET_NOTALL
#define M_CODE_BAD_CONNECT  MP_ET_MAX
#define M_CODE_OVERRUN      MP_ET_ALLOVF
#define M_CODE_REUSE        MP_ET_FRDCOR
#define M_CODE_NOT_INUSE    MP_ET_PRVFRD
#define M_CODE_NOMORE_MEM   MP_ET_OUTMEM
#define M_CODE_OUTOF_BOUNDS MP_ET_RNGOVF
#define M_CODE_FREELIST_BAD MP_ET_MAX
#define M_CODE_NOBOUND      MP_ET_MAX
#define M_CODE_STK_NOCUR    MP_ET_MAX
#define M_CODE_STK_BADFUNC  MP_ET_MAX
#define M_CODE_UNDERRUN     MP_ET_ALLOVF
#define M_CODE_FREEMARK     MP_ET_FREMRK
#define M_CODE_ZERO_ALLOC   MP_ET_ALLZER


/* The union used to supply a command argument to dbmallopt().
 */

union dbmalloptarg
{
    long i;    /* integer value */
    char *str; /* string value */
};


#ifndef NDEBUG

#define malloc_errno __mp_errno

#define dbmallinit() __mp_init_dbmalloc()
#define dbmallopt(c, v) __mpt_dbmallocoption((c), (v))
#define malloc_perror(s) __mpt_dbmallocperror(s)
#define malloc_chain_check(f) __mpt_dbmallocchaincheck((f), MP_FUNCNAME, \
                                                       __FILE__, __LINE__)
#define malloc_dump(f) __mpt_dbmallocdump(f)
#define malloc_list(f, l, u) __mpt_dbmalloclist((f), (l), (u))
#define malloc_inuse(h) __mpt_dbmallocinuse(h)
#define malloc_size(p) __mpt_dbmallocsize(p)
#define malloc_mark(p) (void) __mp_setmark(p)
#define malloc_enter(f) ((void) 0)
#define malloc_leave(f) ((void) 0)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


int __mpt_dbmallocoption(int, union dbmalloptarg *);
void __mpt_dbmallocperror(MP_CONST char *);
int __mpt_dbmallocchaincheck(int, MP_CONST char *, MP_CONST char *,
                             unsigned long);
void __mpt_dbmallocdump(int);
void __mpt_dbmalloclist(int, unsigned long, unsigned long);
unsigned long __mpt_dbmallocinuse(unsigned long *);
size_t __mpt_dbmallocsize(MP_CONST void *);
void __mp_init_dbmalloc(void);


static MP_VOLATILE void *__mpt_init_dbmalloc = (void *) __mp_init_dbmalloc;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#else /* NDEBUG */

#define malloc_errno __mp_errno

#define dbmallinit() ((void) 0)
#define dbmallopt(c, v) ((int) 1)
#define malloc_perror(s) ((void) 0)
#define malloc_chain_check(f) ((int) 0)
#define malloc_dump(f) ((void) 0)
#define malloc_list(f, l, u) ((void) 0)
#define malloc_inuse(h) (*(h) = 0, (unsigned long) 0)
#define malloc_size(p) ((size_t) -1)
#define malloc_mark(p) ((void) 0)
#define malloc_enter(f) ((void) 0)
#define malloc_leave(f) ((void) 0)

#endif /* NDEBUG */


#endif /* MPT_DBMALLOC_H */
