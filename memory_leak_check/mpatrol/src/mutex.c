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
 * Threads interface.  Provides thread-safe facilities for locking
 * data structures used by the mpatrol library.  An extremely informative
 * book on POSIX threads and multithreaded programming in general is
 * Programming with POSIX Threads, First Edition by David R. Butenhof
 * (Addison-Wesley, 1997, ISBN 0-201-63392-2).
 */


#include "mutex.h"
#include <stddef.h>
#if TARGET == TARGET_UNIX
#include <pthread.h>
#if SYSTEM == SYSTEM_TRU64
#include <tis.h>
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
#include <proto/exec.h>
#include <exec/semaphores.h>
#elif TARGET == TARGET_WINDOWS
#include <windows.h>
#include <winbase.h>
#elif TARGET == TARGET_NETWARE
#include <nwsemaph.h>
#include <nwthread.h>
#endif /* TARGET */


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mutex_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


/* A mutex provides a way to lock a data structure in an atomic way.
 */

#if TARGET == TARGET_UNIX
typedef pthread_mutex_t mutex;        /* POSIX threads mutex */
#elif TARGET == TARGET_AMIGA
typedef struct SignalSemaphore mutex; /* Amiga semaphore */
#elif TARGET == TARGET_WINDOWS
typedef HANDLE mutex;                 /* Windows handle */
#elif TARGET == TARGET_NETWARE
typedef LONG mutex;                   /* Netware handle */
#else /* TARGET */
typedef int mutex;                    /* No threads support */
#endif /* TARGET */


/* A recursive mutex allows a thread to relock a mutex that is currently
 * locked by that thread.
 */

typedef struct recmutex
{
    mutex guard;         /* guard mutex */
    mutex real;          /* actual mutex */
    unsigned long owner; /* owning thread */
    unsigned long count; /* recursion count */
    char init;           /* initialisation flag */
}
recmutex;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* We will have to initialise the array of mutex locks at run-time.  This
 * means that we must find a way of initialising the mutexes before the
 * mpatrol library is initialised.
 */

static recmutex locks[MT_MAX];


#if TARGET == TARGET_UNIX && SYSTEM != SYSTEM_LYNXOS
/* We can make use of the POSIX threads function pthread_once() in
 * order to prevent the mutexes being initialised more than once at
 * the same time.
 */

static pthread_once_t lockflag = PTHREAD_ONCE_INIT;
#endif /* TARGET && SYSTEM */


/* Initialise the mpatrol library mutexes.  We're up a brown smelly creek if
 * any of these functions dynamically allocate memory.
 */

MP_GLOBAL
void
__mp_initmutexes(void)
{
    unsigned long i;

    for (i = 0; i < MT_MAX; i++)
        if (!locks[i].init)
        {
            locks[i].init = 1;
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_TRU64
            tis_mutex_init(&locks[i].guard);
            tis_mutex_init(&locks[i].real);
#else /* SYSTEM */
            pthread_mutex_init(&locks[i].guard, NULL);
            pthread_mutex_init(&locks[i].real, NULL);
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
            InitSemaphore(&locks[i].guard);
            InitSemaphore(&locks[i].real);
#elif TARGET == TARGET_WINDOWS
            locks[i].guard = CreateMutex(NULL, 0, NULL);
            locks[i].real = CreateMutex(NULL, 0, NULL);
#elif TARGET == TARGET_NETWARE
            locks[i].guard = OpenLocalSemaphore(1);
            locks[i].real = OpenLocalSemaphore(1);
#else /* TARGET */
            locks[i].guard = 0;
            locks[i].real = 0;
#endif /* TARGET */
            locks[i].owner = 0;
            locks[i].count = 0;
        }
}


/* Remove the mpatrol library mutexes.  We're up another one of those creeks
 * if the mutexes are still locked or are likely to be used in the future.
 */

MP_GLOBAL
void
__mp_finimutexes(void)
{
    unsigned long i;

    for (i = 0; i < MT_MAX; i++)
        if (locks[i].init)
        {
            locks[i].init = 0;
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_TRU64
            tis_mutex_destroy(&locks[i].guard);
            tis_mutex_destroy(&locks[i].real);
#else /* SYSTEM */
            pthread_mutex_destroy(&locks[i].guard);
            pthread_mutex_destroy(&locks[i].real);
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
            CloseHandle(locks[i].guard);
            CloseHandle(locks[i].real);
#elif TARGET == TARGET_NETWARE
            CloseLocalSemaphore(locks[i].guard);
            CloseLocalSemaphore(locks[i].real);
#else /* TARGET */
            locks[i].guard = 0;
            locks[i].real = 0;
#endif /* TARGET */
            locks[i].owner = 0;
            locks[i].count = 0;
        }
}


/* Lock an mpatrol library mutex.
 */

static
void
lockmutex(mutex *m)
{
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_TRU64
    tis_mutex_lock(m);
#else /* SYSTEM */
    pthread_mutex_lock(m);
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
    ObtainSemaphore(m);
#elif TARGET == TARGET_WINDOWS
    WaitForSingleObject(*m, INFINITE);
#elif TARGET == TARGET_NETWARE
    WaitOnLocalSemaphore(*m);
#endif /* TARGET */
}


/* Unlock an mpatrol library mutex.
 */

static
void
unlockmutex(mutex *m)
{
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_TRU64
    tis_mutex_unlock(m);
#else /* SYSTEM */
    pthread_mutex_unlock(m);
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
    ReleaseSemaphore(m);
#elif TARGET == TARGET_WINDOWS
    ReleaseMutex(*m);
#elif TARGET == TARGET_NETWARE
    SignalLocalSemaphore(*m);
#endif /* TARGET */
}


/* Lock an mpatrol library recursive mutex.
 */

MP_GLOBAL
void
__mp_lockmutex(mutextype m)
{
    recmutex *l;
    unsigned long i;

    l = &locks[m];
    i = __mp_threadid();
#if TARGET == TARGET_UNIX && SYSTEM != SYSTEM_LYNXOS
#if SYSTEM == SYSTEM_TRU64
    tis_once(&lockflag, __mp_initmutexes);
#else /* SYSTEM */
    pthread_once(&lockflag, __mp_initmutexes);
#endif /* SYSTEM */
#else /* TARGET && SYSTEM */
    if (!l->init)
        __mp_initmutexes();
#endif /* TARGET && SYSTEM */
    lockmutex(&l->guard);
    if ((l->owner == i) && (l->count > 0))
        l->count++;
    else
    {
        unlockmutex(&l->guard);
        lockmutex(&l->real);
        lockmutex(&l->guard);
        l->owner = i;
        l->count = 1;
    }
    unlockmutex(&l->guard);
}


/* Unlock an mpatrol library recursive mutex.
 */

MP_GLOBAL
void
__mp_unlockmutex(mutextype m)
{
    recmutex *l;
    unsigned long i;

    l = &locks[m];
    i = __mp_threadid();
    if (l->init)
    {
        lockmutex(&l->guard);
        if ((l->owner == i) && (l->count > 0))
        {
            l->count--;
            if (l->count == 0)
            {
                unlockmutex(&l->real);
                l->owner = 0;
            }
        }
        unlockmutex(&l->guard);
    }
}


/* Return the identifier of the currently running thread.
 */

MP_GLOBAL
unsigned long
__mp_threadid(void)
{
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_TRU64
    return (unsigned long) tis_self();
#else /* SYSTEM */
    return (unsigned long) pthread_self();
#endif /* SYSTEM */
#elif TARGET == TARGET_AMIGA
    return (unsigned long) FindTask(NULL);
#elif TARGET == TARGET_WINDOWS
    return (unsigned long) GetCurrentThreadId();
#elif TARGET == TARGET_NETWARE
    return (unsigned long) GetThreadId();
#else /* TARGET */
    return 0;
#endif /* TARGET */
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
