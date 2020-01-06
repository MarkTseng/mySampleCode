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

#if !defined _BPS_PTRACE_H
#define _BPS_PTRACE_H 1

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "bps_error.h"
#include "bps_data.h"
#include "bps_util.h"

/* trap instructions */
#if defined ARCHX86
#define TRAPBYTE 0xcc
#elif defined ARCHARM
//#define TRAPINT 0xdffedffe
//#define TRAPHALF 0xdffe
//#define TRAPINT 0xdeffdeff
//#define TRAPHALF 0xdeff
#define TRAPINT 0xe7ffffff
#define TRAPHALF 0xdeff
#endif

/* name the user regs struct */
#if defined ARCHX86
#define REGS_STRUCT struct user_regs_struct
#define REG_PC eip
#define REG_SP esp
#elif defined ARCHARM
#define REGS_STRUCT struct user_regs
#define REG_PC uregs[15]
#define REG_SP uregs[13]
#define REG_CPSR uregs[16]
#define CPSR_THUMB 0x20
#endif

/* ptrace functions */
void run_debugger(char *cmdline[], char *envp[], unsigned long breakaddr, struct breakpoint *breakpoints, int copyonfork, int copyonthread, int initbreakglobal);
void run_child_exe(char *cmdline[], char *envp[]);
void run_controller(pid_t exe, unsigned long breakaddr, struct breakpoint *breakpoints, int copyonfork, int copyonthread, int initbreakglobal);

void initallprocesses(struct process *head);
pid_t threadparent(struct process *prochead, pid_t exe);
int isintbreakpoint(int trapint, int lsb);
void setupbreakpoints(struct process *currproc);
void setupbreakpointsglobal(struct process *prochead, pid_t initexe);
void deactivatebreakpoints(struct breakpoint *head);
void deactivatebreakpointsglobal(struct process *prochead, pid_t initexe);
unsigned int setbreakpoint(pid_t exe, unsigned long breakaddr);
void clearbreakpoint(pid_t exe, unsigned long breakaddr, unsigned int origint);
void enablebreakpoint(pid_t exe, int bpnum, struct breakpoint *head);
void disablebreakpoint(pid_t exe, int bpnum, struct breakpoint *head);
void enablebreakpoints(pid_t exe, struct breakpoint *breakpoints);
void disablebreakpoints(pid_t exe, struct breakpoint *breakpoints);
void modbreakpoints(pid_t exe, struct bpmodlist *bpmod, struct breakpoint *breakpoints, int local);
void modbreakpointsglobal(struct process *prochead, struct bpmodlist *bpmod, pid_t exe);

unsigned long decpc(pid_t exe);
unsigned long getpc(pid_t exe);
void display_stopstate(pid_t exe, int status, char *location);
void child_continue(pid_t exe, int signal);
void child_singlestep(struct process *proc);
void set_ptrace_options(pid_t exe);

unsigned long readchildword(pid_t pid, unsigned long addr);
void writechildword(pid_t pid, unsigned long addr, unsigned long word);
unsigned char *readchildbuffer(pid_t pid, unsigned long addr, int size);
unsigned char *readchildstring(pid_t pid, unsigned long addr);
void readchildregs(pid_t pid, REGS_STRUCT *regs);
void writechildregs(pid_t pid, REGS_STRUCT *regs);

int regindex(char *reg);
int isvalidreg(char *reg);
unsigned long getreg(char *reg, REGS_STRUCT *regs);
void displayregs(struct reglist *printregs, REGS_STRUCT *regs);
char signof(int val);
void displayaddrs(pid_t pid, struct addrlist *printaddrs, REGS_STRUCT *regs);
void displayptrs(pid_t pid, struct ptrlist *printptrs, REGS_STRUCT *regs);

/* arm function */
unsigned int arm_shiftrot(unsigned int instr, REGS_STRUCT *regs);
unsigned int arm_dataproc(int opcode, unsigned int operand1, unsigned int operand2, REGS_STRUCT *regs);
unsigned int thumb_branchaddr(pid_t exe, REGS_STRUCT *regs, unsigned int instr);
unsigned int arm_branchaddr(pid_t exe, REGS_STRUCT *regs, unsigned int instr);
void arm_singlestep(struct process *proc);


#endif

