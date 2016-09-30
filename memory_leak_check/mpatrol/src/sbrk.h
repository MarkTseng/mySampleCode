#ifndef MP_SBRK_H
#define MP_SBRK_H


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
 * Simulated UNIX heap.  These functions allow manipulation of the size of a
 * simulated heap and as a result can allocate or free memory from that heap.
 */


/*
 * $Id$
 */


#include "config.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_API int brk(void *);
MP_API void *sbrk(long);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_SBRK_H */
