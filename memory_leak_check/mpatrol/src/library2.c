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
 * This file includes all of the mpalloc library source files in order
 * to build one large object file instead of a library of object files.
 */


#define MP_EXPORT static /* limit visibility of symbols */
#define MP_GLOBAL static /* limit visibility of symbols */


#include "config.h"


#if MP_IDENT_SUPPORT
#ident "$Id: library2.c,v 1.4 2002/01/08 20:13:59 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *library2_id = "$Id: library2.c,v 1.4 2002/01/08 20:13:59 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#include "mpalloc.c"
#include "mpfailure.c"
#include "mpsetfail.c"
#include "mpmalloc.c"
#include "mpcalloc.c"
#include "mpstrdup.c"
#include "mprealloc.c"
