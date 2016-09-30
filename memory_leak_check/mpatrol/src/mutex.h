#ifndef MP_MUTEX_H
#define MP_MUTEX_H


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
 * Threads interface.  This module provides a common interface for the
 * mpatrol library when making calls for thread-safe facilities.
 */


/*
 * $Id$
 */


#include "config.h"


/* The different types of mutex that can be locked.
 */

typedef enum mutextype
{
    MT_MAIN, /* main mpatrol library mutex */
    MT_MAX   /* total number of mutex types */
}
mutextype;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_initmutexes(void);
MP_EXPORT void __mp_finimutexes(void);
MP_EXPORT void __mp_lockmutex(mutextype);
MP_EXPORT void __mp_unlockmutex(mutextype);
MP_EXPORT unsigned long __mp_threadid(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_MUTEX_H */
