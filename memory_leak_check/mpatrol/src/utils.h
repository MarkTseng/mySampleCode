#ifndef MP_UTILS_H
#define MP_UTILS_H


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
 * Mathematical support routines.
 */


/*
 * $Id$
 */


#include "config.h"
#include <stddef.h>


#if MP_MACROROUTINES
#define __mp_ispoweroftwo(n) \
    (((unsigned long) (n) > 0) && (((unsigned long) (n) & \
       ((unsigned long) (n) - 1)) == 0))
#define __mp_rounddown(n, a) \
    ((unsigned long) (n) & ~((unsigned long) (a) - 1))
#define __mp_roundup(n, a) \
    ((((unsigned long) (n) - 1) & ~((unsigned long) (a) - 1)) + \
     (unsigned long) (a))
#endif /* MP_MACROROUTINES */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT unsigned char __mp_logtwo(unsigned long);
MP_EXPORT unsigned long __mp_squareroot(unsigned long);
#if !MP_MACROROUTINES
MP_EXPORT int __mp_ispoweroftwo(unsigned long);
#endif /* MP_MACROROUTINES */
MP_EXPORT unsigned long __mp_poweroftwo(unsigned long);
#if !MP_MACROROUTINES
MP_EXPORT unsigned long __mp_rounddown(unsigned long, unsigned long);
MP_EXPORT unsigned long __mp_roundup(unsigned long, unsigned long);
#endif /* MP_MACROROUTINES */
MP_EXPORT void *__mp_encodesleb128(long, size_t *);
MP_EXPORT void *__mp_encodeuleb128(unsigned long, size_t *);
MP_EXPORT long __mp_decodesleb128(void *, size_t *);
MP_EXPORT unsigned long __mp_decodeuleb128(void *, size_t *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_UTILS_H */
