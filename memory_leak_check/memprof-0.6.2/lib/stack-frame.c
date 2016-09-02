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
#include <config.h>

#include <alloca.h>
#include <string.h>

#ifdef HAVE_EXECINFO_H
# include <execinfo.h>
#endif

#include "stack-frame.h"

#if defined (HAVE_BACKTRACE)
void
mi_call_with_backtrace (int to_skip, void callback (int, void **, void *), void *data)
{
    int bufsiz = 128, count = 0;
    void **buf;
	static __thread int in_backtrace = 0;

again:
    buf = alloca (bufsiz * sizeof (void *));

	if (!(in_backtrace++))
			count = backtrace (buf, bufsiz);
	in_backtrace--;
    
    if (count == bufsiz)
    {
	bufsiz = bufsiz * 2;
	goto again;
    }

    callback (count - to_skip, buf + to_skip, data);
}
#endif

/* Magic code to chain along the stack frames

   This may even work on non-386 gcc targets, but I haven't tested it.. */

#define HAVE_FRAME_ACCESSORS

#if defined (__GNUC__)
typedef struct stack_frame_struct stack_frame;
struct stack_frame_struct {
    stack_frame *next;
    void *return_address;
};

static inline stack_frame *
first_frame (void)
{
    return __builtin_frame_address (0);
}

static inline stack_frame *
next_frame (stack_frame *fp)
{
    return (fp != 0) ? fp->next : 0;
}

static inline void *
frame_return_address (stack_frame *fp)
{
    return (fp != 0) ? fp->return_address : 0;
}

#endif

#ifndef HAVE_BACKTRACE

#ifndef HAVE_FRAME_ACCESSORS
# error "Don't have stack-frame accessors for your compiler/architecture."
#endif

void
mi_call_with_backtrace (int to_skip, void callback (int, void **, void *), void *data)
{
    int nframes = 64;
    void **frames = alloca (sizeof (void *) * nframes);
    int depth;
    stack_frame *frame;

    for (frame = first_frame (), depth = 0;
	 frame != 0;
	 frame = next_frame (frame), depth++)
    {                                      
	void *ret = frame_return_address (frame);
	if (depth == nframes)
	{
	    void *new = alloca (sizeof (void *) * (nframes * 2));
	    memcpy (new, frames, nframes * sizeof (void *));
	    frames = new;
	    nframes *= 2;
	}
	frames[depth] = ret;
    }

    callback (depth - to_skip, frames + to_skip, data);
}

#endif /* !HAVE_BACKTRACE */

/**
 * mi_call_with_signal_backtrace:
 * @first_address: Instruction pointer from signal context
 * @second_frame: Frame pointer from signal context.
 * @stack_bound: Stack pointer from signal context; used as
 *               a rough way of detecting invalid frame pointers.
 * @callback: callback function
 * @data: User data to pass to @callback
 * 
 * Calls @callback with a stack trace from the the pointer where the
 * signal interupted normap execution.
 **/
void
mi_call_with_signal_backtrace (void *first_address, void *second_frame, void *stack_bound,
			       void callback (int, void **, void *), void *data)
{
    int nframes = 64;
    void **frames = alloca (sizeof (void *) * nframes);
    int depth;
    stack_frame *frame;

    frames[0] = first_address;

    for (frame = second_frame, depth = 1;
	 (void *)frame > stack_bound;
	 frame = next_frame (frame), depth++)
    {                                      
	void *ret = frame_return_address (frame);
	if (depth == nframes)
	{
	    void *new = alloca (sizeof (void *) * (nframes * 2));
	    memcpy (new, frames, nframes * sizeof (void *));
	    frames = new;
	    nframes *= 2;
	}
	frames[depth] = ret;
    }

    callback (depth, frames, data);
}

