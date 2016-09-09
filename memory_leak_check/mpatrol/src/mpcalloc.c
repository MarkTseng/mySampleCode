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
 * Release version of zero-initialised memory allocation function.  This is
 * used by the version of MP_CALLOC() that is defined in mpalloc.h and will
 * call a failure handler function instead of returning NULL.
 */


#include "config.h"
#include "mpalloc.h"
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id: mpcalloc.c,v 1.6 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mpcalloc_id = "$Id: mpcalloc.c,v 1.6 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


extern __mp_failhandler __mp_failure;


/* Allocate a zero-initialised memory block of a given size and abort on
 * failure.
 */

MP_API
void *
__mp_xcalloc(size_t l, MP_CONST char *s, MP_CONST char *t, unsigned long u,
             MP_CONST char *g, size_t h)
{
    void *p;

    if (l == 0)
        if (h != 0)
            l = h;
        else
            l = 1;
    if ((p = malloc(l)) == NULL)
    {
        if (__mp_failure != NULL)
            __mp_failure(l, s, t, u, g, h);
        abort();
    }
    memset(p, 0, l);
    return p;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
