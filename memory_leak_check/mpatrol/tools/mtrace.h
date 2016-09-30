#ifndef MPT_MTRACE_H
#define MPT_MTRACE_H


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
 * GNU-compatible memory allocation tracing.  Implements mtrace() and
 * muntrace() using mpatrol and produces trace files compatible with
 * those produced by the GNU C library.
 */


/*
 * $Id$
 */


/*
 * This file defines mtrace() and muntrace(), two functions which enable
 * and disable memory allocation tracing respectively.  These should be
 * called in matching pairs but will have no effect unless the MALLOC_TRACE
 * environment variable is set to the filename of the trace file to use.
 * The resulting trace files can be processed by the mtrace perl script
 * which is distributed with the GNU C library.
 */


#include <mpatrol.h>


#ifndef NDEBUG

#define mtrace() __mpt_mtrace()
#define muntrace() __mpt_muntrace()


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


void __mpt_mtrace(void);
void __mpt_muntrace(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#else /* NDEBUG */

#define mtrace() ((void) 0)
#define muntrace() ((void) 0)

#endif /* NDEBUG */


#endif /* MPT_MTRACE_H */
