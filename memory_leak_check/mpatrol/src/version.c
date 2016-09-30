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
 * Library version and copyright definitions.  The version strings are
 * in the format most common to a specific operating system.  Note that
 * the dates are release dates, not dates of last compilation.
 */


#include "version.h"


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *version_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if TARGET == TARGET_UNIX

/* SCCS identification string and American date format.
 */
MP_GLOBAL char *__mp_version = "@(#) " MP_VERSION " (08/12/16)";

#elif TARGET == TARGET_AMIGA

/* Amiga identification string and European date format.
 */
MP_GLOBAL char *__mp_version = "$VER: " MP_VERSION " (16.12.08)";

#else /* TARGET */

/* No identification string and English date format.
 */
MP_GLOBAL char *__mp_version = MP_VERSION " (16 December 2008)";

#endif /* TARGET */


MP_GLOBAL char *__mp_copyright = "Copyright (C) 1997-2008";
MP_GLOBAL char *__mp_author = "Graeme S. Roy";
MP_GLOBAL char *__mp_email = "graemeroy@users.sourceforge.net";
MP_GLOBAL char *__mp_homepage = "http://sourceforge.net/projects/mpatrol";


#ifdef __cplusplus
}
#endif /* __cplusplus */
