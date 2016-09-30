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
 * C++ dynamic memory allocation functions.  Overrides the standard library
 * definitions of these functions if they haven't already been overridden by
 * the mpatrol.h header file.  This source file should be compiled with a
 * C++ compiler but best-guess definitions of the functions will be made if
 * it is compiled with a C compiler.
 */


#include "inter.h"
#include "diag.h"
#if MP_THREADS_SUPPORT
#include "mutex.h"
#endif /* MP_THREADS_SUPPORT */
#ifdef __cplusplus
#ifdef __GNUC__
#pragma implementation "new"
#endif /* __GNUC__ */
#include <new>
#endif /* __cplusplus */


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *cplus_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
#if TARGET != TARGET_WINDOWS || defined(__GNUC__)
namespace std
{
#endif /* TARGET && __GNUC__ */
#endif /* __cplusplus */


/* Set the low-memory handler and return the previous setting.
 */

MP_API
#ifdef __cplusplus
new_handler
set_new_handler(new_handler h) throw()
#elif defined(__GNUC__)
void
(*_ZSt15set_new_handlerPFvvE(void (*h)(void)))(void)
#else /* __cplusplus && __GNUC__ */
void
(*set_new_handler__3stdFPFv_v(void (*h)(void)))(void)
#endif /* __cplusplus && __GNUC__ */
{
#ifdef __cplusplus
    return (new_handler) __mp_nomemory((nomemoryhandler) h);
#else /* __cplusplus */
    return (void (*)(void)) __mp_nomemory((nomemoryhandler) h);
#endif /* __cplusplus */
}


#ifdef __cplusplus
#if TARGET != TARGET_WINDOWS || defined(__GNUC__)
}
#endif /* TARGET && __GNUC__ */
#endif /* __cplusplus */


/* Allocate an uninitialised memory block of a given size, throwing an
 * exception if the allocation cannot be made.
 */

MP_API
#ifdef __cplusplus
void *
operator new(size_t l) throw(std::bad_alloc)
#elif defined(__GNUC__)
void *
_Znwj(size_t l)
#else /* __cplusplus && __GNUC__ */
void *
__nw__FUi(size_t l)
#endif /* __cplusplus && __GNUC__ */
{
#ifndef __cplusplus
    infohead *h;
#endif /* __cplusplus */
    void *p;

    if ((p = __mp_alloc(l, 0, AT_NEW, NULL, NULL, 0, NULL, 0, 1)) == NULL)
    {
#ifdef __cplusplus
        throw std::bad_alloc();
#else /* __cplusplus */
        /* The C++ standard specifies that operator new should always return
         * a non-NULL pointer (unless it is the nothrow version).  Since this
         * is being compiled by a C compiler we cannot throw an exception and
         * so we have to abort.
         */
#if MP_THREADS_SUPPORT
        __mp_lockmutex(MT_MAIN);
#endif /* MP_THREADS_SUPPORT */
        h = __mp_memhead();
        h->recur++;
        __mp_printsummary(h);
        __mp_diag("\n");
        __mp_error(ET_OUTMEM, AT_NEW, NULL, 0, NULL);
        h->fini = 1;
        __mp_abort();
#endif /* __cplusplus */
    }
    return p;
}


/* Allocate an uninitialised memory block of a given size, returning NULL
 * if the allocation cannot be made.
 */

MP_API
#ifdef __cplusplus
void *
operator new(size_t l, MP_CONST std::nothrow_t&) throw()
#elif defined(__GNUC__)
void *
_ZnwjRKSt9nothrow_t(size_t l)
#else /* __cplusplus && __GNUC__ */
void *
__nw__FUiRCQ2_3std9nothrow_t(size_t l)
#endif /* __cplusplus && __GNUC__ */
{
    return __mp_alloc(l, 0, AT_NEW, NULL, NULL, 0, NULL, 0, 1);
}


/* Allocate an uninitialised memory block of a given size for use by an array,
 * throwing an exception if the allocation cannot be made.
 */

MP_API
#ifdef __cplusplus
void *
operator new[](size_t l) throw(std::bad_alloc)
#elif defined(__GNUC__)
void *
_Znaj(size_t l)
#else /* __cplusplus && __GNUC__ */
void *
__nwa__FUi(size_t l)
#endif /* __cplusplus && __GNUC__ */
{
#ifndef __cplusplus
    infohead *h;
#endif /* __cplusplus */
    void *p;

    if ((p = __mp_alloc(l, 0, AT_NEWVEC, NULL, NULL, 0, NULL, 0, 1)) == NULL)
    {
#ifdef __cplusplus
        throw std::bad_alloc();
#else /* __cplusplus */
        /* The C++ standard specifies that operator new[] should always return
         * a non-NULL pointer (unless it is the nothrow version).  Since this
         * is being compiled by a C compiler we cannot throw an exception and
         * so we have to abort.
         */
#if MP_THREADS_SUPPORT
        __mp_lockmutex(MT_MAIN);
#endif /* MP_THREADS_SUPPORT */
        h = __mp_memhead();
        h->recur++;
        __mp_printsummary(h);
        __mp_diag("\n");
        __mp_error(ET_OUTMEM, AT_NEWVEC, NULL, 0, NULL);
        h->fini = 1;
        __mp_abort();
#endif /* __cplusplus */
    }
    return p;
}


/* Allocate an uninitialised memory block of a given size for use by an array,
 * returning NULL if the allocation cannot be made.
 */

MP_API
#ifdef __cplusplus
void *
operator new[](size_t l, MP_CONST std::nothrow_t&) throw()
#elif defined(__GNUC__)
void *
_ZnajRKSt9nothrow_t(size_t l)
#else /* __cplusplus && __GNUC__ */
void *
__nwa__FUiRCQ2_3std9nothrow_t(size_t l)
#endif /* __cplusplus && __GNUC__ */
{
    return __mp_alloc(l, 0, AT_NEWVEC, NULL, NULL, 0, NULL, 0, 1);
}


/* Free an existing block of memory that was allocated by operator new.
 */

MP_API
#ifdef __cplusplus
void
operator delete(void *p) throw()
#elif defined(__GNUC__)
void
_ZdlPv(void *p)
#else /* __cplusplus && __GNUC__ */
void
__dl__FPv(void *p)
#endif /* __cplusplus && __GNUC__ */
{
    __mp_free(p, AT_DELETE, NULL, NULL, 0, 1);
}


/* Free an existing block of memory that was allocated by operator new.
 */

MP_API
#ifdef __cplusplus
void
operator delete(void *p, MP_CONST std::nothrow_t&) throw()
#elif defined(__GNUC__)
void
_ZdlPvRKSt9nothrow_t(void *p)
#else /* __cplusplus && __GNUC__ */
void
__dl__FPvRCQ2_3std9nothrow_t(void *p)
#endif /* __cplusplus && __GNUC__ */
{
    __mp_free(p, AT_DELETE, NULL, NULL, 0, 1);
}


/* Free an existing block of memory that was allocated by operator new[].
 */

MP_API
#ifdef __cplusplus
void
operator delete[](void *p) throw()
#elif defined(__GNUC__)
void
_ZdaPv(void *p)
#else /* __cplusplus && __GNUC__ */
void
__dla__FPv(void *p)
#endif /* __cplusplus && __GNUC__ */
{
    __mp_free(p, AT_DELETEVEC, NULL, NULL, 0, 1);
}


/* Free an existing block of memory that was allocated by operator new[].
 */

MP_API
#ifdef __cplusplus
void
operator delete[](void *p, MP_CONST std::nothrow_t&) throw()
#elif defined(__GNUC__)
void
_ZdaPvRKSt9nothrow_t(void *p)
#else /* __cplusplus && __GNUC__ */
void
__dla__FPvRCQ2_3std9nothrow_t(void *p)
#endif /* __cplusplus && __GNUC__ */
{
    __mp_free(p, AT_DELETEVEC, NULL, NULL, 0, 1);
}
