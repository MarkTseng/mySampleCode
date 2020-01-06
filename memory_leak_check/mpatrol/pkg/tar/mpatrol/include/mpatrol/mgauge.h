#ifndef MPT_MGAUGE_H
#define MPT_MGAUGE_H


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
 * Memory allocation gauge.  Implements mgaugestart(), mgaugeend(),
 * mgaugeon() and mgaugeoff() using mpatrol to produce a simple gauge
 * showing the total allocated memory.
 */


/*
 * $Id$
 */


/*
 * This file defines mgaugestart(), mgaugeend(), mgaugeon() and mgaugeoff()
 * which produce and control a simple memory allocation gauge in a terminal
 * window.  The gauge is displayed in textual form using the standard I/O
 * library rather than using a graphics library.  Since it is updated in
 * real-time, it makes no sense to send the output of the gauge to a file.
 * Only one gauge can be in use at any one time.
 *
 * The first argument to mgaugestart() is the filename of the file to write
 * the gauge to.  As mentioned before, this should be a terminal file that
 * can be displayed in real-time, such as /dev/pts* on UNIX systems or CON:#?
 * on AmigaOS.  If it is a null pointer then the standard error file stream
 * will be used.
 *
 * The second argument to mgaugestart() specifies the character that will be
 * used to represent allocated memory.  If this is given as whitespace, `|'
 * or `+' then `#' will be used instead.  The third argument specifies the
 * number of bytes that the gauge represents.  If the total allocated memory
 * exceeds this then `+' will be appended to the gauge.  The final argument
 * specifies the frequency of memory allocation events at which the gauge
 * should be updated.  If it is specified as zero then all events will cause
 * the gauge to be updated.
 */


#include <mpatrol.h>


#ifndef NDEBUG

#define mgaugestart(f, c, s, u) __mpt_mgaugestart((f), (c), (s), (u))
#define mgaugeend() __mpt_mgaugeend()
#define mgaugeon() __mpt_mgaugeon()
#define mgaugeoff() __mpt_mgaugeoff()


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


int __mpt_mgaugestart(MP_CONST char *, unsigned char, unsigned long,
                      unsigned long);
void __mpt_mgaugeend(void);
void __mpt_mgaugeon(void);
void __mpt_mgaugeoff(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#else /* NDEBUG */

#define mgaugestart(f, c, s, u) ((int) 0)
#define mgaugeend() ((void) 0)
#define mgaugeon() ((void) 0)
#define mgaugeoff() ((void) 0)

#endif /* NDEBUG */


#endif /* MPT_MGAUGE_H */
