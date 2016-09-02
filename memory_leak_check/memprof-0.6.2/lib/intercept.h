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

#ifndef __INTERCEPT_H__
#define __INTERCEPT_H__

/* Functions provided by core to speedintercept/memintercept */

typedef int MIBool;

#define MI_TRUE 1
#define MI_FALSE 0

void mi_write_stack (int      n_frames,
		     void   **frames,
		     void    *data);
MIBool  mi_tracing     (void);
MIBool  mi_check_init  (void);

/* Hooks provided by speedintercept/memintercpet to core */

void mi_init ();
void mi_start ();
void mi_stop ();

#endif /* __INTERCEPT_H__ */
