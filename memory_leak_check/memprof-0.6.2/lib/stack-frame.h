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

 /* This code is mostly from eazel-tools/prof
  *
  *  stack-frame.c -- hacks to read the backtrace
  *
  *  Authors: John Harper <jsh@eazel.com>
  *
  * With modifications by Owen Taylor <otaylor@redhat.com> to
  * add mi_call_with_signal_backtrace() and to work better within
  * the MemProf framework.
  */
#ifndef __STACK_FRAME_H__
#define __STACK_FRAME_H__

/* STACKPOINTER macros to access the right regs on different ARCHs */
#if defined (__i386__)
#define EIPRIP eip
#define EBPRBP ebp
#define ESPRSP esp
#elif defined (__x86_64__)
#define EIPRIP rip
#define EBPRBP rbp
#define ESPRSP rsp
#else
#error "Unknown architecture and generating backtrace is broken"
#endif

void mi_call_with_backtrace (int   to_skip,
			     void  callback (int, void **, void *),
			     void *data);

void mi_call_with_signal_backtrace (void *first_addresss,
				    void *second_frame,
				    void *stack_bound,
				    void  callback (int, void **, void *),
				    void *data);

#endif /* __STACK_FRAME_H__ */
