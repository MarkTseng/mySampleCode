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
 * This file includes all of the mpatrol library source files in order
 * to build one large object file instead of a library of object files.
 */


#define MP_EXPORT static /* limit visibility of symbols */
#define MP_GLOBAL static /* limit visibility of symbols */


#include "config.h"


#if MP_IDENT_SUPPORT
#ident "$Id$"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *library_id = "$Id$";
#endif /* MP_IDENT_SUPPORT */


#include "list.c"
#include "tree.c"
#include "slots.c"
#include "utils.c"
#include "memory.c"
#include "heap.c"
#include "alloc.c"
#include "info.c"
#include "stack.c"
#include "addr.c"
#include "strtab.c"
#include "symbol.c"
#include "signals.c"
#if MP_THREADS_SUPPORT
#include "mutex.c"
#endif /* MP_THREADS_SUPPORT */
#include "diag.c"
#include "option.c"
#include "leaktab.c"
#include "profile.c"
#include "trace.c"
#include "inter.c"
#if TARGET == TARGET_UNIX || (TARGET == TARGET_AMIGA && defined(__GNUC__)) || \
    TARGET == TARGET_WINDOWS
#include "malloc.c"
#include "cplus.c"
#if TARGET != TARGET_UNIX
#include "sbrk.c"
#endif /* TARGET */
#endif /* TARGET && __GNUC__ */
#include "version.c"
