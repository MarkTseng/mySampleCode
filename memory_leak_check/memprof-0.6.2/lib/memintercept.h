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

typedef union _MIInfo MIInfo;
typedef struct _MIInfoAny MIInfoAny;
typedef struct _MIInfoAlloc MIInfoAlloc;
typedef struct _MIInfoFork MIInfoFork;
typedef struct _MIInfoExec MIInfoExec;

typedef enum {
	MI_MALLOC,
	MI_REALLOC,
	MI_FREE,
	MI_TIME,
	MI_EXEC,
	MI_NEW,
	MI_FORK,
	MI_CLONE,
	MI_EXIT
} MIOperation;

struct _MIInfoAny {
	MIOperation operation;
	pid_t pid;
	unsigned int seqno;
};

struct _MIInfoAlloc {
	MIOperation operation;
	pid_t  pid;
	unsigned int seqno;
	void  *old_ptr;
	void  *new_ptr;
	size_t size;
	unsigned int stack_size;
};

struct _MIInfoFork {
	MIOperation operation;
	pid_t pid;
	unsigned int seqno;
	pid_t new_pid;
	pid_t new_fd;
};

struct _MIInfoExec {
	MIOperation operation;
	pid_t pid;
	unsigned int seqno;
};

union _MIInfo {
	MIOperation operation;
	MIInfoAny any;
	MIInfoAlloc alloc;
	MIInfoFork fork;
	MIInfoExec exec;
};

