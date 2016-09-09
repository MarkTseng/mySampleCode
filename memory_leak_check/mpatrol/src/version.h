#ifndef MP_VERSION_H
#define MP_VERSION_H


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
 * Library version and copyright definitions.
 */


/*
 * $Id: version.h,v 1.22 2002/01/08 22:30:00 graeme Exp $
 */


#include "config.h"


/* The version of the mpatrol library.  The version is of the format vrrff,
 * where v represents the version number, rr represents the revision number
 * and ff represents the bug fix count.
 */

#define MP_VERNUM 10408


/* The version string for the mpatrol library.
 */

#define MP_VERSION "mpatrol 1.4.8"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT char *__mp_version;
MP_EXPORT char *__mp_copyright;
MP_EXPORT char *__mp_author;
MP_EXPORT char *__mp_email;
MP_EXPORT char *__mp_homepage;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_VERSION_H */
