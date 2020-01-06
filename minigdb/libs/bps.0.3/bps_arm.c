/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_arm.c - arm specific functions
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

#include "bps_ptrace.h"


#if defined ARCHARM
/*
** the following arm functions were written using information gleened from
   ARM Instruction Set:
   http://bear.ces.cwru.edu/eecs_382/ARM7-TDMI-manual-pt2.pdf

   The basic approach to instruction slicing is this:
   mask the bits you want to check - instr & 0xMASKPATTERN
   then check if that equals what you expect - == 0xBITPATTERN
   Extract values by shifting the instr right and then masking the bits you want - (instr >> 11) & 0xf
*/

/* arm_shiftrot extracts operand2 from the data processing or load instruction and shifts and
   rotates it accordingly */
unsigned int arm_shiftrot(unsigned int instr, REGS_STRUCT *regs)
{
	int op2regindex = 0;
	int shiftregindex = 0;
	int operand2;
	int shiftamount = 0;
	int i;

	if (!regs) {
		myprintf(0, "arm_shiftrot: invalid arguments\n");
		return 0;
	}
	
	op2regindex = instr & 0xf;
	operand2 = regs->uregs[op2regindex];
	/* if PC, it will be 2 words on by the time it is processed */
	if (op2regindex == 15) {
		operand2 += 8;
	}

	if (instr & 0x10) {
		/* shift by register - this adds an additional word to the PC */
		if (op2regindex == 15) {
			operand2 += 4;
		}
		shiftregindex = (instr >> 8) & 0x0f;
		shiftamount = regs->uregs[shiftregindex] & 0xff;
	} else {
		/* shift by immediate */
		shiftamount = (instr >> 7) & 0x1f;
	}

	switch ((instr >> 5) & 0x3) {
		case 0:
			/* logical left */
			if (shiftamount > 0) {
				operand2 = operand2 << shiftamount;
			}
			break;
		case 1:
			/* logical right */
			if (shiftamount == 1) {
				operand2 = (operand2 >> 1) & 0x7fffffff;
			} else if (shiftamount == 0) {
				operand2 = 0;
			} else if (shiftamount > 1) {
				operand2 = (operand2 >> 1) & 0x7fffffff;
				operand2 = operand2 >> (shiftamount - 1);
			}
			break;
		case 2:
			/* arith right */
			if (shiftamount > 0) {
				operand2 = operand2 >> shiftamount;
			}
			break;
		case 3:
			/* rotate right */
			if (shiftamount > 0) {
				for (i=0; i<(shiftamount % 32); i++) {
					operand2 = ((operand2 >> 1) & 0x7fffffff) | (operand2 << 31);
				}
			} else if (shiftamount == 0) {
				operand2 = ((operand2 >> 1) & 0x7fffffff) | ((regs->REG_CPSR << 2) & 0x80000000);
			}

			break;
	}

	return operand2;
}


/* arm_dataproc processes the opcode and two operands */
unsigned int arm_dataproc(int opcode, unsigned int operand1, unsigned int operand2, REGS_STRUCT *regs)
{
	if (!regs) {
		myprintf(0, "arm_dataproc: invalid arguments\n");
		return 0;
	}
	
	switch (opcode) {
		case 0:
			/* AND */
			myprintf(3, " and\n");
			return (operand1 & operand2);
			break;

		case 1:
			/* EOR */
			myprintf(3, " eor\n");
			return (operand1 ^ operand2);
			break;

		case 2:
			/* SUB */
			myprintf(3, " sub\n");
			return (operand1 - operand2);
			break;

		case 3:
			/* RSB */
			myprintf(3, " rsb\n");
			return (operand2 - operand1);
			break;

		case 4:
			/* ADD */
			myprintf(3, " add\n");
			return (operand1 + operand2);
			break;

		case 5:
			/* ADC */
			myprintf(3, " adc\n");
			return (operand1 + operand2 + ((regs->REG_CPSR >> 29) & 0x1));
			break;

		case 6:
			/* SBC */
			myprintf(3, " sbc\n");
			return (operand1 - operand2 + ((regs->REG_CPSR >> 29) & 0x1) -1);
			break;

		case 7:
			/* RSC */
			myprintf(3, " rsc\n");
			return (operand2 - operand1 + ((regs->REG_CPSR >> 29) & 0x1) -1);
			break;

		case 0xc:
			/* ORR */
			myprintf(3, " orr\n");
			return (operand1 | operand2);
			break;

		case 0xd:
			/* MOV */
			myprintf(3, " mov\n");
			return operand2;
			break;

		case 0xe:
			/* BIC */
			myprintf(3, " bic\n");
			return (operand1 & ~operand2);
			break;

		case 0xf:
			/* MVN */
			myprintf(3, " mvn\n");
			return ~operand2;
			break;
	}

	return 0;
}


/* thumb_branchaddr returns the branch address of the current instruction or 0 if no branch address */
/* this spaghetti function checks the instruction bitmap for branches and instructions that will
   affect the PC, R15. */
unsigned int thumb_branchaddr(pid_t exe, REGS_STRUCT *regs, unsigned int instr)
{
	unsigned long addr = regs->REG_PC;
	int branchoffset = 0;
	int op1regindex = 0;
	int op2regindex = 0;
	int operand1 = 0;
	int operand2 = 0;
	int i;

	if (!regs) {
		myprintf(0, "thumb_branchaddr: invalid arguments\n");
		return 0;
	}
	
	/* check if instruction is branch */
	if ((instr & 0x0000ff80) == 0x00004700) {
		/* branch and exchange */
		myprintf(3, "thumb branch and exchange\n");
		op1regindex = (instr >> 3) & 0xf;
		operand1 = regs->uregs[op1regindex];
		/* preserve the LSB as this can be used to switch thumb->arm */
		return operand1;

	} else if ((instr & 0x0000fc87) == 0x00004487) {
		/* hi register operation on r15 */
		op1regindex = (instr >> 3) & 0xf;
		operand1 = regs->uregs[op1regindex];
		op2regindex = 0xf;
		operand2 = regs->uregs[op2regindex] + 4;

		/* process the opcode */
		switch ((instr >> 8) & 0x3) {
			case 0:
				/* ADD */
				myprintf(3, "thumb add\n");
				return ((operand1 + operand2) | 0x1);
				break;
			case 2:
				/* MOV */
				myprintf(3, "thumb mov\n");
				return (operand1 | 0x1);
				break;
		}

	} else if ((instr & 0x0000ff00) == 0x0000bd00) {
		/* pop registers including r15 */
		myprintf(3, "thumb pop\n");
		/* count number of register to pop before r15 */
		operand1 = instr & 0xff;
		operand2 = 0;
		for (i=0; i<8; i++) {
			if (operand1 & 0x1) {
				operand2++;
			}
			operand1 = operand1 >> 1;
		}
		/* calc address of r15 on stack */
		operand1 = regs->REG_SP + (4 * operand2);
		/* read address and return it, preserving the LSB */
		return readchildword(exe, operand1 & ~0x3);

	} else if (((instr & 0x0000f000) == 0x0000d000) && ((instr & 0x00000f00) != 0x00000f00)) {
		/* conditional branch */
		myprintf(3, "thumb cond branch\n");
		branchoffset = ((instr & 0xff) << 24) >> 23;
		return ((addr + branchoffset + 4) | 0x1);

	} else if ((instr & 0x0000ff00) == 0x0000df00) {
		/* swi */
		myprintf(3, "thumb swi\n");
		return 0;

	} else if ((instr & 0x0000f800) == 0x0000e000) {
		/* unconditional branch */
		myprintf(3, "thumb branch\n");
		branchoffset = ((instr & 0x7ff) << 21) >> 20;
		return ((addr + branchoffset + 4) | 0x1);

	} else if ((instr & 0x0000f800) == 0x0000f800) {
		/* long branch with link */
		myprintf(3, "thumb long branch with link\n");
		branchoffset = ((instr & 0x000007ff) << 21) >> 20;
		return ((regs->uregs[14] + branchoffset) | 0x1);

	}

	return 0;
}


/* arm_branchaddr returns the branch address of the current instruction or 0 if no branch address */
/* this spaghetti function checks the instruction bitmap for branches and instructions that will
   affect the PC, R15. */
unsigned int arm_branchaddr(pid_t exe, REGS_STRUCT *regs, unsigned int instr)
{
	unsigned long addr = regs->REG_PC;
	int branchoffset = 0;
	int op1regindex = 0;
	int op2regindex = 0;
	int opcode = 0;
	int operand1 = 0;
	int operand2 = 0;
	int shiftamount = 0;
	int i,j;

	if (!regs) {
		myprintf(0, "arm_branchaddr: invalid arguments\n");
		return 0;
	}
	
	/* check if instruction is branch */
	if (((instr & 0x0e000000) >> 25) == 0x5) {
		/* branch - extend sign bit and multiply by 4, then add 8 */
		myprintf(3, "branch\n");
		branchoffset = instr;
		branchoffset = (((branchoffset & 0x00ffffff) << 8) >> 6) + 8;
		return ((addr + branchoffset) & ~0x1);

	} else if ((instr & 0x0ffffff0) == 0x012fff10) {
		/* branch and exchange */
		myprintf(3, "branch and exchange\n");
		op1regindex = instr & 0xf;
		/* preserve LSB as this can be used to switch arm->thumb */
		return regs->uregs[op1regindex];

	} else if ((instr & 0x0f000000) == 0x0f000000) {
		/* swi - we can treat this as a non-branch */
		myprintf(3, "swi\n");
		return 0;

	} else if (!(instr & 0x0c000000) && (((instr >> 12) & 0x0f) == 15)) {
		/* data processing instruction and Rd is R15 (pc) */

		/* decode the instruction */
		opcode = (instr >> 21) & 0x0f;
		op1regindex = (instr >> 16) & 0x0f;
		operand1 = regs->uregs[op1regindex];
		/* if PC, it will be 2 words on by the time it is processed */
		if (op1regindex == 15) {
			operand1 += 8;
		}
		operand2 = instr & 0x0fff;

		/* first do the shifting and rotating */
		if (instr & 0x02000000) {
			/* immediate */
			operand2 = instr & 0xff;
			shiftamount = ((instr >> 8) & 0x0f) << 1;
			/* rotate right */
			if (shiftamount > 0) {
				for (i=0; i<shiftamount; i++) {
					operand2 = ((operand2 >> 1) & 0x7fffffff) | (operand2 << 31);
				}
			}

		} else {
			/* register */
			op2regindex = instr & 0xf;
			operand2 = arm_shiftrot(instr, regs);

			if (instr & 0x10) {
				/* shift by register - this adds an additional word to the PC */
				if (op1regindex == 15) {
					operand1 += 4;
				}
			}
		}

		/* now process the opcode */
		return (arm_dataproc(opcode, operand1, operand2, regs) & ~0x1);

	} else if ((instr & 0x0c10f000) == 0x0410f000) {
		/* LDR(B) with R15 (PC) as destination */
		myprintf(3, "ldr\n");
		op1regindex = (instr >> 16) & 0x0f;
		operand1 = regs->uregs[op1regindex];
		/* if base register is PC, then add 8 */
		if (op1regindex == 15) {
			operand1 += 8;
		}

		if (instr & 0x01000000) {
			/* pre - add offset */

			if (instr & 0x02000000) {
				/* operand2 is a shifted register */
				op2regindex = instr & 0xf;
				/* can't be PC */
				if (op2regindex == 15) {
					return 0;
				}
				operand2 = arm_shiftrot(instr, regs);

			} else {
				/* operand 2 is an immediate value */
				operand2 = instr & 0x0fff;
			}

		} else {
			/* post - don't add offset */
			operand2 = 0;
		}

		if (instr & 0x00800000) {
			/* up or add */
			addr = operand1 + operand2;
		} else {
			/* down or sub */
			addr = operand1 - operand2;
		}

		/* check address alignment */
		if (addr & 0x3) {
			/* address is misaligned */
			operand1 = readchildword(exe, addr & ~0x3);
			switch (addr & 0x3) {
				case 1:
					operand2 = (operand1 << 8) | ((operand1 >> 24) & 0xff);
					break;
				case 2:
					operand2 = (operand1 << 16) | ((operand1 >> 16) & 0xffff);
					break;
				case 3:
					operand2 = (operand1 << 24) | ((operand1 >> 8) & 0xffffff);
					break;
			}

		} else {
			operand2 = readchildword(exe, addr);
		}

		/* check size of load; preserve LSB as this can be used to switch to thumb */
		if (instr & 0x00400000) {
			/* byte */
			return (operand2 & 0xff);
		} else {
			/* word */
			return operand2;
		}

	} else if ((instr & 0x0e108000) == 0x08108000) {
		/* block data transfer LDM including R15 */
		myprintf(3, "ldm\n");
		op1regindex = (instr >> 16) & 0xf;
		/* can't be PC */
		if (op1regindex == 15) {
			return 0;
		}
		operand1 = regs->uregs[op1regindex];

		if (instr & 0x00800000) {
			/* up or add */
			operand2 = 0;
			j = instr;
			/* count how many registers are being loaded (excluding r15) */
			for (i=0;i<15;i++) {
				if (j & 0x1) {
					operand2++;
				}
				j = j >> 1;
			}

			/* add one for pre */
			if (instr & 0x01000000) {
				operand2++;
			}

			/* calc address of pc to load */
			addr = operand1 + (operand2 * 4);
			/* read it from memory, aligned */
			operand2 = readchildword(exe, addr & ~0x3);
			/* preserve LSB */
			return operand2;

		} else {
			/* down or sub */

			/* subtract 1 for pre */
			if (instr & 0x01000000) {
				operand1 -= 4;
			}

			/* read it from memory, aligned */
			operand2 = readchildword(exe, operand1 & ~0x3);
			/* preserve LSB */
			return operand2;

		}

	}

	return 0;

}


/* arm_singlestep emulates single stepping on arm */
void arm_singlestep(struct process *proc)
{
	REGS_STRUCT regs;
	int thumb = 0;
	unsigned long next = 0;
	unsigned long branch = 0;
	unsigned int currint = 0;
	unsigned int nextint = 0;
	unsigned int branchint = 0;

	if (!proc) {
		myprintf(0, "arm_singlestep: no process\n");
		return;
	}
	
	/* ARM ptrace() doesn't implement singlestep so write our own */
	readchildregs(proc->pid, &regs);

	/* check if arm or thumb */
	if (regs.REG_CPSR & CPSR_THUMB) {
		thumb = 1;
	}

	/* check pc alignment */
	if ((regs.REG_PC & 0x01) || (!thumb && (regs.REG_PC & 0x02))) {
		myprintf(0, "PC misalignment: thumb mode: %d, addr: %p\n", thumb, (void *)regs.REG_PC);
		exit(E_UNKNOWN);
	}

	/* get current instruction */
	currint = readchildword(proc->pid, regs.REG_PC & ~0x3);


	/* if current instruction is a branch (conditional or not) then trap on the target address */
	if (thumb) {
		/* as we're in thumb mode, set the LSB of the next instruction address so that set/clearbreakpoint
		   know to use thumb traps */
		next = regs.REG_PC + 3;
		/* check alignment */
		if (regs.REG_PC & 0x02) {
			branch = thumb_branchaddr(proc->pid, &regs, (currint >> 16) & 0xffff);
		} else {
			branch = thumb_branchaddr(proc->pid, &regs, currint & 0xffff);
		}
	} else {
		branch = arm_branchaddr(proc->pid, &regs, currint);
		next = regs.REG_PC + 4;
	}

	/* set breakpoints and store in process struct */
	nextint = setbreakpoint(proc->pid, next);
	proc->ssaddr1 = next;
	proc->sstrap1 = nextint;
	if (branch) {
		branchint = setbreakpoint(proc->pid, branch);
		proc->ssaddr2 = branch;
		proc->sstrap2 = branchint;
	} else {
		proc->ssaddr2 = 0;
		proc->sstrap2 = 0;
	}

	/* run the child into one of the breakpoints */
	child_continue(proc->pid, 0);

}
#endif

