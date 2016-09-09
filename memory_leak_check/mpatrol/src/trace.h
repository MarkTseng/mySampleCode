#ifndef MP_TRACE_H
#define MP_TRACE_H


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
 * Memory allocation tracing.  This interface is used by the mpatrol
 * library to enable or disable the tracing of dynamic memory allocations
 * to a file for later processing.
 */


/*
 * $Id: trace.h,v 1.13 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include "memory.h"


/* A tracehead contains all the tracing information including the
 * filename of the output file and the current tracing state.
 */

typedef struct tracehead
{
    char *file;   /* tracing filename */
    char tracing; /* tracing status */
}
tracehead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newtrace(tracehead *, meminfo *);
MP_EXPORT int __mp_changetrace(tracehead *, char *, int);
MP_EXPORT int __mp_endtrace(tracehead *);
MP_EXPORT void __mp_traceheap(void *, size_t, int);
MP_EXPORT void __mp_tracealloc(tracehead *, unsigned long, void *, size_t,
                               unsigned long, char *, char *, unsigned long);
MP_EXPORT void __mp_tracerealloc(tracehead *, unsigned long, void *, size_t,
                                 unsigned long, char *, char *, unsigned long);
MP_EXPORT void __mp_tracefree(tracehead *, unsigned long, unsigned long, char *,
                              char *, unsigned long);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_TRACE_H */
