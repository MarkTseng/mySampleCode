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
 * Release version of string duplication function.  This is used by the
 * version of MP_STRDUP() that is defined in mpalloc.h and will call a
 * failure handler function instead of returning NULL.
 */


#include "config.h"
#include "mpalloc.h"
#include <string.h>


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mpstrdup_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


extern __mp_failhandler __mp_failure;


/* Duplicate an existing string using memory from the heap and abort on failure.
 */

MP_API
char *
__mp_xstrdup(MP_CONST char *r, MP_CONST char *s, MP_CONST char *t,
             unsigned long u)
{
    char *p;
    size_t l;

    l = strlen(r) + 1;
    if ((p = (char *) malloc(l)) == NULL)
    {
        if (__mp_failure != NULL)
            __mp_failure(l, s, t, u, "char", sizeof(char));
        abort();
    }
    memcpy(p, r, l);
    return p;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
