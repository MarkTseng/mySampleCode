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
 * This file includes all of the mpalloc library source files in order
 * to build one large object file instead of a library of object files.
 */


#define MP_EXPORT static /* limit visibility of symbols */
#define MP_GLOBAL static /* limit visibility of symbols */


#include "config.h"


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *library2_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#include "mpalloc.c"
#include "mpfailure.c"
#include "mpsetfail.c"
#include "mpmalloc.c"
#include "mpcalloc.c"
#include "mpstrdup.c"
#include "mprealloc.c"
