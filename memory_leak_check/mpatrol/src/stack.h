#ifndef MP_STACK_H
#define MP_STACK_H


/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2002 Graeme S. Roy <graeme.roy@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/*
 * Call stacks.  This module attempts to provide a common interface for
 * traversing the function call stack at a specific point during execution.
 */


/*
 * $Id: stack.h,v 1.15 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"
#include <stddef.h>
#include <signal.h>
#if !MP_BUILTINSTACK_SUPPORT
#if MP_LIBRARYSTACK_SUPPORT && TARGET == TARGET_WINDOWS
#include <windows.h>
#include <imagehlp.h>
#endif /* MP_LIBRARYSTACK_SUPPORT && TARGET */
#endif /* MP_BUILTINSTACK_SUPPORT */


#if !MP_BUILTINSTACK_SUPPORT
#if MP_LIBRARYSTACK_SUPPORT
#if SYSTEM == SYSTEM_HPUX
/* HP/UX provides functions to traverse the PA/RISC stack frames.  This
 * structure only makes visible the stack frame entries that we need to
 * use - all the rest are simply reserved.
 */

typedef struct frameinfo
{
    unsigned int size;   /* frame size */
    unsigned int sp;     /* stack pointer */
    unsigned int ps;     /* program counter space */
    unsigned int pc;     /* program counter */
    unsigned int dp;     /* data pointer */
    unsigned int res[6]; /* reserved entries */
}
frameinfo;
#endif /* SYSTEM */
#else /* MP_LIBRARYSTACK_SUPPORT */
#if TARGET == TARGET_UNIX && ARCH == ARCH_MIPS
/* The MIPS architecture does not save and restore a frame pointer on the
 * stack so the stack pointer and program counter must be used to obtain
 * this information.
 */

typedef struct frameinfo
{
    unsigned int sp; /* stack pointer */
    unsigned int ra; /* return address */
}
frameinfo;
#endif /* TARGET && ARCH */
#endif /* MP_LIBRARYSTACK_SUPPORT */
#endif /* MP_BUILTINSTACK_SUPPORT */


/* A stackinfo structure provides information about the currently selected
 * stack frame.
 */

typedef struct stackinfo
{
    void *frame;               /* current frame handle */
    void *addr;                /* current return address */
#if MP_BUILTINSTACK_SUPPORT
    void *frames[MP_MAXSTACK]; /* array of frame pointers */
    void *addrs[MP_MAXSTACK];  /* array of return addresses */
    size_t index;              /* current stack index */
#elif MP_LIBRARYSTACK_SUPPORT
#if TARGET == TARGET_UNIX
#if SYSTEM == SYSTEM_HPUX
    struct frameinfo next;     /* next frame handle */
#elif SYSTEM == SYSTEM_IRIX || SYSTEM == SYSTEM_TRU64
    struct sigcontext next;    /* next frame handle */
#endif /* SYSTEM */
#elif TARGET == TARGET_WINDOWS
    STACKFRAME next;           /* next frame handle */
#endif /* TARGET */
#else /* MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
#if TARGET == TARGET_UNIX && ARCH == ARCH_MIPS
    struct frameinfo next;     /* next frame handle */
#else /* TARGET && ARCH */
    void *next;                /* next frame handle */
#endif /* TARGET && ARCH */
#endif /* MP_BUILTINSTACK_SUPPORT && MP_LIBRARYSTACK_SUPPORT */
    void *first;               /* first frame information */
}
stackinfo;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newframe(stackinfo *, void *);
MP_EXPORT int __mp_getframe(stackinfo *);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_STACK_H */
