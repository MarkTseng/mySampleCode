#ifndef MP_MPDEBUG_H
#define MP_MPDEBUG_H


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
 * $Id: mpdebug.h,v 1.3 2002/01/08 20:13:59 graeme Exp $
 */


#ifndef HAVE_MPATROL
#define HAVE_MPATROL 0
#endif /* HAVE_MPATROL */

#ifndef HAVE_MPALLOC
#define HAVE_MPALLOC 0
#endif /* HAVE_MPALLOC */


#if HAVE_MPATROL
#include <mpatrol.h>
#else /* HAVE_MPATROL */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef __cplusplus
#include <new>
#endif /* __cplusplus */
#if HAVE_MPALLOC
#include <mpalloc.h>
#else /* HAVE_MPALLOC */
#include <stdio.h>

typedef void *__mp_failhandler;

#define MP_MALLOC(p, l, t) \
    (((p = (t *) malloc((l) * sizeof(t))) != NULL) ? (t *) (p) : \
     (t *) (fputs("out of memory\n", stderr), abort(), NULL))
#define MP_CALLOC(p, l, t) \
    (((p = (t *) calloc((l), sizeof(t))) != NULL) ? (t *) (p) : \
     (t *) (fputs("out of memory\n", stderr), abort(), NULL))
#define MP_STRDUP(p, s) \
    (((p = (char *) strdup(s)) != NULL) ? (char *) (p) : \
     (char *) (fputs("out of memory\n", stderr), abort(), NULL))
#define MP_REALLOC(p, l, t) \
    (((p = (t *) realloc((p), (l) * sizeof(t))) != NULL) ? (t *) (p) : \
     (t *) (fputs("out of memory\n", stderr), abort(), NULL))
#define MP_FREE(p) do { if (p) { free(p); p = NULL; } } while (0)
#define MP_FAILURE(f) ((__mp_failhandler) NULL)
#endif /* HAVE_MPALLOC */
#endif /* HAVE_MPATROL */


#endif /* MP_MPDEBUG_H */
