#ifndef MP_VERSION_H
#define MP_VERSION_H


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
 * Library version and copyright definitions.
 */


/*
 * $Id$
 */


#include "config.h"


/* The version of the mpatrol library.  The version is of the format vrrff,
 * where v represents the version number, rr represents the revision number
 * and ff represents the bug fix count.
 */

#define MP_VERNUM 10501


/* The version string for the mpatrol library.
 */

#define MP_VERSION "mpatrol 1.5.1"


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
