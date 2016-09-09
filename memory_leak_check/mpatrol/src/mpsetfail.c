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
 * Allocation failure handler.  This is used to set the allocation failure
 * handler that is called by the versions of MP_MALLOC() and related functions
 * that are defined in mpalloc.h instead of returning NULL.
 */


#include "config.h"
#include "mpalloc.h"


#if MP_IDENT_SUPPORT
#ident "$Id: mpsetfail.c,v 1.5 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mpsetfail_id = "$Id: mpsetfail.c,v 1.5 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


extern __mp_failhandler __mp_failure;


/* Set the allocation failure handler and return the previous setting.
 */

MP_API
__mp_failhandler
__mp_setfailhandler(__mp_failhandler h)
{
    __mp_failhandler p;

    p = __mp_failure;
    __mp_failure = h;
    return p;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
