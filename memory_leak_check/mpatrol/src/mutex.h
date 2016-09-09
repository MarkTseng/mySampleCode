#ifndef MP_MUTEX_H
#define MP_MUTEX_H


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
 * Threads interface.  This module provides a common interface for the
 * mpatrol library when making calls for thread-safe facilities.
 */


/*
 * $Id: mutex.h,v 1.8 2002/01/08 20:13:59 graeme Exp $
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
