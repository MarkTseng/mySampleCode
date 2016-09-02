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

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>

#include "intercept.h"
#include "memintercept.h"
#include "memintercept-utils.h"
#include "stack-frame.h"

static void *(*old_malloc) (size_t size);
static void *(*old_calloc) (size_t nmemb, size_t size);
static void *(*old_memalign) (size_t boundary, size_t size);
static void *(*old_realloc) (void *ptr, size_t size);
static void (*old_free) (void *ptr);

static void
abort_unitialized (const char *call)
{
	mi_debug ("MemProf: unexpected library call during initialization: %s\n", call);
	abort();
}

static void *
do_malloc (size_t size, int to_skip)
{
	void *result;
	MIInfo info;

	/* It's possible to get recursion here, since dlsym() can trigger
	 * memory allocation. To deal with this, we flag the initialization
	 * condition specially, then use the special knowledge that it's
	 * OK for malloc to fail during initialization (libc degrades
	 * gracefully), so we just return NULL from malloc(), realloc().
	 *
	 * This trick is borrowed from from libc's memusage.
	 */
	if (!mi_check_init ())
		return NULL;

	result = (*old_malloc) (size);

	if (mi_tracing ()) {
		info.alloc.operation = MI_MALLOC;
		info.alloc.old_ptr = NULL;
		info.alloc.new_ptr = result;
		info.alloc.size = size;

		mi_call_with_backtrace (to_skip + 1, mi_write_stack, &info);
	}
		
	return result;
}

void *
__libc_malloc (size_t size)
{
	return do_malloc (size, 1);
}

void *
malloc (size_t size)
{
	return do_malloc (size, 1);
}

static void *
do_calloc (size_t nmemb, size_t size)
{
	int total = nmemb * size;
	void *result = do_malloc (total, 2);

	if (result)
		memset (result, 0, total);
	
	return result;
}


void *
__libc_calloc (size_t nmemb, size_t size)
{
	return do_calloc (nmemb, size);
}

void *
calloc (size_t nmemb, size_t size)
{
	return do_calloc (nmemb, size);
}

static void *
do_memalign (size_t boundary, size_t size)
{
	void *result;
	MIInfo info;

	if (!mi_check_init ())
		abort_unitialized ("memalign");

	result = (*old_memalign) (boundary, size);

	if (mi_tracing ()) {
		info.alloc.operation = MI_MALLOC;
		info.alloc.old_ptr = NULL;
		info.alloc.new_ptr = result;
		info.alloc.size = size;
	
		mi_call_with_backtrace (2, mi_write_stack, &info);
	}

	return result;
}

void *
__libc_memalign (size_t boundary, size_t size)
{
	return do_memalign (boundary, size);
}

void *
memalign (size_t boundary, size_t size)
{
	return do_memalign (boundary, size);
}

static void *
do_realloc (void *ptr, size_t size)
{
	void *result;
	MIInfo info;

	if (!mi_check_init ())
		return NULL;/* See comment in initialize() */

	result = (*old_realloc) (ptr, size);

	if (mi_tracing ()) {
		info.alloc.operation = MI_REALLOC;
		info.alloc.old_ptr = ptr;
		info.alloc.new_ptr = result;
		info.alloc.size = size;
	
		mi_call_with_backtrace (2, mi_write_stack, &info);
	}

	return result;
}

void *
__libc_realloc (void *ptr, size_t size)
{
	return do_realloc (ptr, size);
}

void *
realloc (void *ptr, size_t size)
{
	return do_realloc (ptr, size);
}

static void
do_free (void  *ptr)
{
	MIInfo info;

	if (!mi_check_init ())
		return;

	(*old_free) (ptr);

	if (mi_tracing ()) {
		info.alloc.operation = MI_FREE;
		info.alloc.old_ptr = ptr;
		info.alloc.new_ptr = NULL;
		info.alloc.size = 0;

		mi_call_with_backtrace (2, mi_write_stack, &info);
	}
}

void
__libc_free (void *ptr)
{
	do_free (ptr);
}

void
free (void *ptr)
{
	do_free (ptr);
}

void
mi_init (void)
{
	old_malloc = dlsym(RTLD_NEXT, "__libc_malloc");
	old_realloc = dlsym(RTLD_NEXT, "__libc_realloc");
	old_free = dlsym(RTLD_NEXT, "__libc_free");
	old_calloc = dlsym(RTLD_NEXT, "__libc_calloc");
	old_memalign = dlsym(RTLD_NEXT, "__libc_memalign");
}

void
mi_start (void)
{
}

void
mi_stop (void)
{
}
