#ifndef MPT_DMALLOC_H
#define MPT_DMALLOC_H


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


/*
 * $Id: dmalloc.h,v 1.12 2002/01/08 20:05:10 graeme Exp $
 */


/*
 * This file provides Dmalloc-compatible functions which are built on top
 * of the mpatrol library.  They are compatible with the 4.8.2 release of
 * Dmalloc, but only the Dmalloc-specific functions are defined here,
 * leaving the overriding of standard functions up to the mpatrol library.
 * As the mpatrol library does not currently override the C library
 * string functions, neither does this file.  In addition, the Dmalloc
 * distribution comes with definitions for xmemalign(), xvalloc() and
 * xrecalloc(), neither of which are defined by this file or by the
 * mpatrol library.
 *
 * This module is intended to work with the existing dmalloc command,
 * which sets the contents of the DMALLOC_OPTIONS environment variable
 * according to any specified command line options.  The four documented
 * Dmalloc global variables are also defined, although the two address
 * variables are not acted upon and changing the dmalloc_logpath variable
 * has no effect yet.  The dmalloc_errno variable is mapped onto the
 * __mp_errno variable and so the dmalloc_strerror() function always
 * returns strings that are specific to the mpatrol library.  Note that
 * unlike the actual Dmalloc library, this file is not threadsafe, and the
 * lockon option has no effect.  In addition, the start option ignores the
 * file:line syntax and uses allocation indices rather than events.
 *
 * The dmalloc_debug() function does not support the setting of all of the
 * Dmalloc flags, although this file defines preprocessor macros for each
 * of them, something which is not currently done in the dmalloc.h file in
 * the Dmalloc distribution.  In fact, many of them do not make sense when
 * applied to the mpatrol library.  Some of them have slightly changed
 * behaviour due to the mapping process and some of them cannot be
 * implemented due to the mpatrol library having been initialised
 * beforehand.
 *
 * The dmalloc_verify() and dmalloc_examine() functions do not give an
 * error message if the pointer passed in does not correspond to a heap
 * allocation, and the latter function does not automatically perform an
 * integrity check of the heap.  The malloc_verify() function has not been
 * included in this implementation since it is functionally identical to
 * dmalloc_verify().  Note that the dmalloc_verify() function will only
 * ever return DMALLOC_VERIFY_ERROR if the pointer to be checked is not
 * null and is invalid - it always terminates with an error message in the
 * mpatrol log file whenever the pointer to be checked is null and it has
 * detected heap corruption.
 *
 * The dmalloc_log_heap_map() and dmalloc_log_stats() functions map on to
 * the __mp_memorymap() and __mp_summary() functions and so have entirely
 * different display formats.  The dmalloc_log_unfreed() and
 * dmalloc_log_changed() functions have similar display formats to the
 * original Dmalloc library, but the summary tables are displayed
 * differently and will display symbol names if they are available and
 * filename and line number information isn't.  The dmalloc_message() and
 * dmalloc_vmessage() functions write tracing to the mpatrol log file
 * prefixed by three fields of optional information, which can be
 * controlled by the LOG_* macros when building this module.
 *
 * This file is initialised via the mpatrol library's initialiser function
 * feature, which means that if the __mp_init_dmalloc() function is noted
 * by the mpatrol symbol manager then it will be called when the mpatrol
 * library is being initialised.  If this feature is not supported then
 * the dmalloc_init() function must be called as early on as possible,
 * otherwise this file will not be initialised until one of its functions
 * are called.
 */


#include <mpatrol.h>


/* The version of Dmalloc that this file is simulating and is compatible with.
 */

#define DMALLOC_VERSION_MAJOR 4
#define DMALLOC_VERSION_MINOR 8
#define DMALLOC_VERSION_PATCH 2
#define DMALLOC_VERSION_BETA  0


/* The return codes from dmalloc_examine() and dmalloc_verify().
 */

#define DMALLOC_ERROR          0
#define DMALLOC_NOERROR        1
#define DMALLOC_VERIFY_ERROR   0
#define DMALLOC_VERIFY_NOERROR 1


/* The library flags that can be set with dmalloc_debug().  They are not
 * currently defined in dmalloc.h in the original Dmalloc distribution but
 * dmalloc_debug() is pretty much useless without them.
 */

#define DMALLOC_LOG_STATS         0x00000001
#define DMALLOC_LOG_NONFREE       0x00000002
#define DMALLOC_LOG_KNOWN         0x00000004
#define DMALLOC_LOG_TRANS         0x00000008
#define DMALLOC_LOG_ADMIN         0x00000020
#define DMALLOC_LOG_BLOCKS        0x00000040
#define DMALLOC_LOG_BAD_SPACE     0x00000100
#define DMALLOC_LOG_NONFREE_SPACE 0x00000200
#define DMALLOC_CHECK_FENCE       0x00000400
#define DMALLOC_CHECK_HEAP        0x00000800
#define DMALLOC_CHECK_LISTS       0x00001000
#define DMALLOC_CHECK_BLANK       0x00002000
#define DMALLOC_CHECK_FUNCS       0x00004000
#define DMALLOC_FORCE_LINEAR      0x00010000
#define DMALLOC_CATCH_SIGNALS     0x00020000
#define DMALLOC_LOG_ELAPSED_TIME  0x00040000
#define DMALLOC_LOG_CURRENT_TIME  0x00080000
#define DMALLOC_REALLOC_COPY      0x00100000
#define DMALLOC_FREE_BLANK        0x00200000
#define DMALLOC_ERROR_ABORT       0x00400000
#define DMALLOC_ALLOC_BLANK       0x00800000
#define DMALLOC_HEAP_CHECK_MAP    0x01000000
#define DMALLOC_PRINT_MESSAGES    0x02000000
#define DMALLOC_CATCH_NULL        0x04000000
#define DMALLOC_NEVER_REUSE       0x08000000
#define DMALLOC_ALLOW_FREE_NULL   0x20000000
#define DMALLOC_ERROR_DUMP        0x40000000


/* The different function types that can be passed to the callback function
 * registered with dmalloc_track().
 */

#define DMALLOC_FUNC_MALLOC   MP_AT_MALLOC
#define DMALLOC_FUNC_CALLOC   MP_AT_CALLOC
#define DMALLOC_FUNC_MEMALIGN MP_AT_MEMALIGN
#define DMALLOC_FUNC_VALLOC   MP_AT_VALLOC
#define DMALLOC_FUNC_STRDUP   MP_AT_STRDUP
#define DMALLOC_FUNC_REALLOC  MP_AT_REALLOC
#define DMALLOC_FUNC_RECALLOC MP_AT_RECALLOC
#define DMALLOC_FUNC_FREE     MP_AT_FREE


/* The type for any callback function that is registered with dmalloc_track().
 */

typedef void (*dmalloc_track_t)(MP_CONST char *, unsigned long, int, size_t,
                                size_t, MP_CONST void *, MP_CONST void *);


#ifndef NDEBUG

#define dmalloc_errno __mp_errno

#define dmalloc_init() __mp_init_dmalloc()
#define dmalloc_shutdown() __mpt_dmallocshutdown()
#define dmalloc_log_heap_map() __mp_memorymap(0)
#define dmalloc_log_stats() __mp_summary()
#define dmalloc_log_unfreed() __mpt_dmalloclogchanged(0, 1, 0, 1)
#define dmalloc_verify(p) __mpt_dmallocverify((p), MP_FUNCNAME, __FILE__, \
                                              __LINE__)
#define dmalloc_debug(f) __mpt_dmallocdebug(f)
#define dmalloc_debug_current() __mpt_dmallocdebugcurrent()
#define dmalloc_examine(p, l, t, u, a) __mpt_dmallocexamine((p), (l), (t), \
                                                            (u), (a))
#define dmalloc_message __mpt_dmallocmessage
#define dmalloc_vmessage(s, v) __mpt_dmallocvmessage((s), (v))
#define dmalloc_track(h) __mpt_dmalloctrack(h)
#define dmalloc_mark() __mp_snapshot()
#define dmalloc_log_changed(m, u, f, d) __mpt_dmalloclogchanged((m), (u), (f), \
                                                                (d))
#define dmalloc_strerror(e) __mpt_dmallocstrerror(e)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


extern char *dmalloc_logpath;
extern void *dmalloc_address;
extern unsigned long dmalloc_address_count;


void __mpt_dmallocshutdown(void);
int __mpt_dmallocverify(MP_CONST void *, MP_CONST char *, MP_CONST char *,
                        unsigned long);
unsigned long __mpt_dmallocdebug(unsigned long);
unsigned long __mpt_dmallocdebugcurrent(void);
int __mpt_dmallocexamine(MP_CONST void *, size_t *, char **, unsigned long *,
                         void **);
void __mpt_dmallocmessage(MP_CONST char *, ...);
void __mpt_dmallocvmessage(MP_CONST char *, va_list);
void __mpt_dmalloctrack(dmalloc_track_t);
void __mpt_dmalloclogchanged(unsigned long, int, int, int);
MP_CONST char *__mpt_dmallocstrerror(__mp_errortype);
void __mp_init_dmalloc(void);


static MP_VOLATILE void *__mpt_init_dmalloc = (void *) __mp_init_dmalloc;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#else /* NDEBUG */

#define dmalloc_errno __mp_errno

#define dmalloc_init() ((void) 0)
#define dmalloc_shutdown() ((void) 0)
#define dmalloc_log_heap_map() ((void) 0)
#define dmalloc_log_stats() ((void) 0)
#define dmalloc_log_unfreed() ((void) 0)
#define dmalloc_verify(p) ((int) 1)
#define dmalloc_debug(f) ((unsigned long) 0)
#define dmalloc_debug_current() ((unsigned long) 0)
#define dmalloc_examine(p, l, t, u, a) ((int) 0)
#define dmalloc_vmessage(s, v) ((void) 0)
#define dmalloc_track(h) ((void) 0)
#define dmalloc_mark() ((unsigned long) 0)
#define dmalloc_log_changed(m, u, f, d) ((void) 0)
#define dmalloc_strerror(e) "errno value is not valid"

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ == 199901L)
#define dmalloc_message(...) ((void) 0)
#elif defined(__GNUC__)
#define dmalloc_message(a...) ((void) 0)
#else /* __STDC_VERSION__ && __GNUC__ */
static
void
dmalloc_message(MP_CONST char *s, ...)
{
}
#endif /* __STDC_VERSION__ && __GNUC__ */

#endif /* NDEBUG */


#endif /* MPT_DMALLOC_H */
