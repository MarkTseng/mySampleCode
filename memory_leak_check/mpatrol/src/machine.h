#ifndef MP_MACHINE_H
#define MP_MACHINE_H


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
 * Processor-dependent assembler routines.  These routines are written in
 * assembler, so there may be name linkage or calling convention issues on
 * some platforms.  In addition, this header file should not be included by
 * machine.c as it contains the C interfaces and not valid assembler syntax.
 */


/*
 * $Id: machine.h,v 1.8 2002/01/08 20:13:59 graeme Exp $
 */


#include "config.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if MP_INIT_SUPPORT
extern int __mp_initsection;
#endif /* MP_INIT_SUPPORT */


#if !MP_BUILTINSTACK_SUPPORT
#if MP_LIBRARYSTACK_SUPPORT
#if SYSTEM == SYSTEM_HPUX
void __mp_frameinfo(void *);
#endif /* SYSTEM */
#else /* MP_LIBRARYSTACK_SUPPORT */
#if ARCH == ARCH_MIPS
unsigned int __mp_stackpointer(void);
unsigned int __mp_returnaddress(void);
#elif ARCH == ARCH_SPARC
#if SYSTEM == SYSTEM_SUNOS
unsigned int __mp_stackpointer(void);
#endif /* SYSTEM */
#endif /* ARCH */
#endif /* MP_LIBRARYSTACK_SUPPORT */
#endif /* MP_BUILTINSTACK_SUPPORT */


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_MACHINE_H */
