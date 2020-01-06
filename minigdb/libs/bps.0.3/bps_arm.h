/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_ptrace.h - functions for manipulating a traced child
**
** Copyright (C) 2015  Kevin Sheldrake
**
** This file is part of bps.
**
** Bps is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
*/

#if !defined ARCHX86 && !defined ARCHARM
	#error "You must define ARCH with -DARCH[X86|ARM]"
#endif

#if !defined _BPS_ARM_H
#define _BPS_ARM_H 1

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "bps_error.h"
#include "bps_data.h"
#include "bps_util.h"
#include "bps_ptrace.h"


/* arm functions */
unsigned int arm_shiftrot(unsigned int instr, REGS_STRUCT *regs);
unsigned int arm_dataproc(int opcode, unsigned int operand1, unsigned int operand2, REGS_STRUCT *regs);
unsigned int thumb_branchaddr(pid_t exe, REGS_STRUCT *regs, unsigned int instr);
unsigned int arm_branchaddr(pid_t exe, REGS_STRUCT *regs, unsigned int instr);
void arm_singlestep(struct process *proc);


#endif

