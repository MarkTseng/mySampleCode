/* -*- mode: C; c-file-style: "linux" -*- */

/* MemProf -- memory profiler and leak detector
 * Copyright 1999, 2000, 2001, Red Hat, Inc.
 * Copyright 2002, Kristian Rietveld
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/*====*/

#ifndef __MEMINTERCEPT_UTILS__
#define __MEMINTERCEPT_UTILS__

#include <stdint.h>

/*  __attribute__ ((visibility ("hidden"))) could be useful here to eliminate
 * the chance of these names colliding with application symbols.
 */

/* simple printf to a fd, without using malloc() or stdio */
void mi_printf (int fd, const char *format, ...);

/* mi_printf (2, ...) */
void mi_debug (const char *format, ...);

/* Like perror(), but with the same restrictions as mi_printf */
void mi_perror (const char *message);

/* Like write, but retry until all of buf is written, and use send
 * to avoid SIGPIPE. (Only works for sockets
 */
int mi_write (int fd, const void *buf, int total);

/* Atomic increment and decrement functions
 */
uint32_t mi_atomic_decrement (uint32_t *addr);
uint32_t mi_atomic_increment (uint32_t *addr);


#endif /* __MEMINTERCEPT_UTILS__ */
