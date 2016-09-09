#ifndef MP_MPALLOC_H
#define MP_MPALLOC_H


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
 * $Id: mpalloc.h,v 1.7 2002/01/08 20:13:59 graeme Exp $
 */


#include <stdlib.h>


/* A macro for representing constant function parameters.
 */

#ifndef MP_CONST
#if defined(__STDC__) || defined(__cplusplus)
#define MP_CONST const
#else /* __STDC__ && __cplusplus */
#define MP_CONST
#endif /* __STDC__ && __cplusplus */
#endif /* MP_CONST */


/* A macro for determining the current function name.
 */

#ifndef MP_FUNCNAME
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ == 199901L)
#define MP_FUNCNAME __func__
#elif defined(__GNUC__)
#define MP_FUNCNAME __PRETTY_FUNCTION__
#else /* __STDC_VERSION__ && __GNUC__ */
#define MP_FUNCNAME NULL
#endif /* __STDC_VERSION__ && __GNUC__ */
#endif /* MP_FUNCNAME */


#if !defined(MP_MPATROL_H) || defined(NDEBUG)

typedef void (*__mp_failhandler)(size_t, MP_CONST char *, MP_CONST char *,
                                 unsigned long, MP_CONST char *, size_t);

#define MP_MALLOC(p, l, t) \
    (p = (t *) __mp_xmalloc((l) * sizeof(t), MP_FUNCNAME, __FILE__, __LINE__, \
                            #t, sizeof(t)))
#define MP_CALLOC(p, l, t) \
    (p = (t *) __mp_xcalloc((l) * sizeof(t), MP_FUNCNAME, __FILE__, __LINE__, \
                            #t, sizeof(t)))
#define MP_STRDUP(p, s) \
    (p = __mp_xstrdup((s), MP_FUNCNAME, __FILE__, __LINE__))
#define MP_REALLOC(p, l, t) \
    (p = (t *) __mp_xrealloc((p), (l) * sizeof(t), MP_FUNCNAME, __FILE__, \
                             __LINE__, #t, sizeof(t)))
#define MP_FREE(p) \
    do { if (p) { free(p); p = NULL; } } while (0)
#define MP_FAILURE(f) __mp_setfailhandler(f)

#endif /* MP_MPATROL_H && NDEBUG */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


void *__mp_xmalloc(size_t, MP_CONST char *, MP_CONST char *, unsigned long,
                   MP_CONST char *, size_t);
void *__mp_xcalloc(size_t, MP_CONST char *, MP_CONST char *, unsigned long,
                   MP_CONST char *, size_t);
char *__mp_xstrdup(MP_CONST char *, MP_CONST char *, MP_CONST char *,
                   unsigned long);
void *__mp_xrealloc(void *, size_t, MP_CONST char *, MP_CONST char *,
                    unsigned long, MP_CONST char *, size_t);
__mp_failhandler __mp_setfailhandler(__mp_failhandler);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_MPALLOC_H */
