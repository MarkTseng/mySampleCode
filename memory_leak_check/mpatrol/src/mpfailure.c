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
 * Default allocation failure handler.  This is used to report the location
 * and size of allocation failure when the versions of MP_MALLOC() and related
 * functions that are defined in mpalloc.h run out of memory.  It can be
 * overridden with MP_FAILURE().
 */


#include "config.h"
#include "mpalloc.h"
#include <stdio.h>


#if MP_IDENT_SUPPORT
#ident "$Id: mpfailure.c,v 1.4 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mpfailure_id = "$Id: mpfailure.c,v 1.4 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* The default allocation failure handler.
 */

static
void
defaultfailure(size_t l, MP_CONST char *s, MP_CONST char *t, unsigned long u,
               MP_CONST char *g, size_t h)
{
    fputs("out of memory", stderr);
    if (s != NULL)
        fprintf(stderr, " in %s", s);
    if ((t != NULL) && (u != 0))
        fprintf(stderr, " in %s line %lu", t, u);
    fputc('\n', stderr);
    fprintf(stderr, "while allocating %lu byte%s", l, (l == 1) ? "" : "s");
    if ((g != NULL) && (h != 0))
    {
        l /= h;
        fprintf(stderr, " for %lu item%s of type %s", l, (l == 1) ? "" : "s",
                g);
    }
    fputc('\n', stderr);
    fflush(NULL);
    exit(EXIT_FAILURE);
}


/* The current allocation failure handler.
 */

__mp_failhandler __mp_failure = defaultfailure;


#ifdef __cplusplus
}
#endif /* __cplusplus */
