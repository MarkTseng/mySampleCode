/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_ptrace.c - functions for manipulating a traced child
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

/* global register table */
/* this should match the struct user_regs_struct in /usr/include/sys/user.h */
/* for ARM, we use struct user_regs instead which has member unsigned long int uregs[18] */
#if defined ARCHX86
char *validregs[] = {"ebx", "ecx", "edx", "esi", "edi", "ebp", "eax", "xds", "xes", "xfs", "xgs", "orig_eax", "eip", "xcs", "eflags", "esp", "xss", NULL};
#endif


/*
** ptrace functions
*/

/* run_debugger starts the child and controller that sets and handles the breakpoints */
void run_debugger(char *cmdline[], char *envp[], unsigned long breakaddr, struct breakpoint *breakpoints, int copyonfork, int copyonthread, int initbreakglobal)
{
	pid_t child;

	/* creating child process */
	child = fork();

	if (child == 0) {
		run_child_exe(cmdline, envp);
		myprintf(1, "run_child_exe ended\n");
	} else if (child > 0) {
		run_controller(child, breakaddr, breakpoints, copyonfork, copyonthread, initbreakglobal);
		myprintf(1, "run_controller ended\n");
	} else {
		myprintf(0, "fork error\n");
		exit(E_FORK);
	}
}


/* run_child_exe starts up the executable for debugging */
void run_child_exe(char *cmdline[], char *envp[])
{
	myprintf(2, "in child process %ld\n", (long)getpid());

	if (cmdline == NULL) {
		usage("no command line");
	}

	if (ptrace(PTRACE_TRACEME, 0, NULL, NULL))
	{
		myprintf(0, "run_child_exe ptrace error: %s\n", strerror(errno));
		exit(E_PTRACE);
	}

	execve(cmdline[0], (char **)cmdline, (char **)envp);
}


/* isintbreakpoint checks the supplied int to see if it contains a trap instruction */
int isintbreakpoint(int trapint, int lsb)
{
#if defined ARCHX86
	if ((trapint & 0xff) == TRAPBYTE) {
#elif defined ARCHARM
	if ( ( ((lsb & 0x1) == 0x0) && (trapint == TRAPINT) ) ||
		 ( ((lsb & 0x3) == 0x1) && ( (trapint & 0xffff) == TRAPHALF) ) ||
		 ( ((lsb & 0x3) == 0x3) && ( ((trapint >> 16) & 0xffff) == TRAPHALF) ) ) {
#endif
		return 1;
	} else {
		return 0;
	}
}


/* set_ptrace_options sets the options for the ptrace.
   PTRACE_O_TRACECLONE - automatically attach processes created by clone()
   PTRACE_O_TRACEFORK  - automatically attach processes created by fork()
   PTRACE_O_TRACESYSGOOD - set bit 7 on generated signals
*/
void set_ptrace_options(pid_t exe)
{
	if (ptrace(PTRACE_SETOPTIONS, exe, 0,
			PTRACE_O_TRACECLONE |
			PTRACE_O_TRACEFORK |
			PTRACE_O_TRACESYSGOOD
			) < 0) {
		myprintf(0, "set_ptrace_options ptrace error: %s\n", strerror(errno));
		exit(E_PTRACE);
	}
}


/* run_controller is the process that controls the children, setting and servicing the traps.
   You might notice that it is mammoth, but it is basically a state machine that handles signals from all
   children sequentially. */
void run_controller(pid_t exe, unsigned long breakaddr, struct breakpoint *breakpoints, int copyonfork, int copyonthread, int initbreakglobal)
{
	int status = 0;
	REGS_STRUCT regs;
	struct breakpoint *currbp;
	int bpnum = 0;
	unsigned long pc = 0;
	unsigned int trapint;
	int sig = 0;
	int ptrace_event = 0;
	long forkpid = 0;
	struct process *prochead = NULL;
	struct process *currproc = NULL;
	struct process *newproc = NULL;
	unsigned int breaktrap = 0;

	if (!breakpoints) {
		usage("no breakpoints");
	}
	
	myprintf(2, "In controller process %ld\n", (long)getpid() );

	/* Wait for exe process to stop */
	exe = wait(&status);

	/* display stop state */
	display_stopstate(exe, status, "Controller start");

	/* set ptrace options */
	set_ptrace_options(exe);

	/* create process node */
	currproc = addprocess(&prochead);
	if (!currproc) {
		myprintf(0, "run_controller: could not create process\n");
		exit(E_MALLOC);
	}
	
	/* set process options */
	currproc->pid = exe;
	currproc->copyonfork = copyonfork;
	currproc->copyonthread = copyonthread;
	currproc->bpmods = NULL;
	currproc->next = NULL;
	
	currproc->state = PROCESS_RUNNING;
	currproc->bphead = copybps(breakpoints);
	if (!(currproc->bphead)) {
		myprintf(0, "run_controller: copybps failed, no breakpoints\n");
		exit(E_MALLOC);
	}
	
	/* is there an initbreak set up? */
	if (breakaddr) {
		/* wait for initbreak before establishing breakpoints */
		currproc->preinit = 1;
		breaktrap = setbreakpoint(exe, breakaddr);
	} else {
		/* no initbreak */
		currproc->preinit = 0;
	}	
	
	/* set up break points */
	//enablebreakpoints(exe, currproc->bphead);
	setupbreakpoints(currproc);
	
	/* set process in motion */
	child_continue(exe, sig);

	/* now run and service the traps */
	while (prochead) {

		/* wait for a process to stop */
		exe = waitpid(-1, &status, __WALL);
		
		/* process stopped */
		myprintf(2, "run_contoller: servicing pid %ld\n", exe);
		
		/* get process details */
		sig = WSTOPSIG(status);
		ptrace_event = (status >> 16) & 0xff;
		currproc = getprocess(prochead, exe);
		
#if defined ARCHX86
		/* if we just executed a trap (not illegal) instruction, then step back to the inserted trap */
		if ( (sig == SIGTRAP) && (ptrace_event != PTRACE_EVENT_FORK) && (ptrace_event != PTRACE_EVENT_CLONE) && (currproc) && (currproc->state != PROCESS_SINGLESTEP) && !(WIFEXITED( status )) ) {
			decpc(exe);
		}
#endif

		/* display stop state */
		display_stopstate(exe, status, "Controller");
		
		/* check if process already exists */
		if (!currproc) {
			myprintf(2, "waitpid returned %ld, which isn't in the process list\n", exe);
			myprintf(2, "status = 0x%x, ptrace_event = 0x%x\n", status, ptrace_event);
			/* 'continue' the child but with sig still set to same signal;
			   this will cause the process to signal again with same signal
			   but hopefully after we have seen it's parent trigger a fork.
			   (If it manages to trigger again before the parent triggers the fork,
			   then it goes back round again and again until it loses the race.) */
			child_continue(exe, sig);
		
		/* check if process has died */
		} else if (WIFEXITED(status)) {
			deleteprocess(&prochead, exe);
		
		/* check for breakpoint */
#if defined ARCHX86
		} else if ((sig == SIGTRAP) && (ptrace_event != PTRACE_EVENT_FORK) && (ptrace_event != PTRACE_EVENT_CLONE)) {
#elif defined ARCHARM
		} else if (((sig == SIGTRAP) && (ptrace_event != PTRACE_EVENT_FORK) && (ptrace_event != PTRACE_EVENT_CLONE)) || (sig == SIGILL)) {
#endif

			/* cancel the signal */
			sig = 0;

			/* get program counter */
			pc = getpc(exe);

			/* check if this is the initial breakpoint */
#if defined ARCHX86
			if ((currproc->preinit) && (!currproc->modbps) && (pc == breakaddr)) {
#elif defined ARCHARM
			if ((currproc->preinit) && (!currproc->modbps) && (pc == (breakaddr & ~0x1))) {
#endif
				/* initial breakpoint */
				myprintf(2, "process %ld hit init breakpoint\n", exe);

				/* clear the init breakpoint */
				clearbreakpoint(exe, breakaddr, breaktrap);
				
				/* set process state */
				currproc->preinit = 0;
				
				if (initbreakglobal) {
					/* set up on all processes */
					initallprocesses(prochead);
				}
				
				child_continue(exe, sig);
				
			} else if (currproc->state == PROCESS_SINGLESTEP) {
				/* process returning from single step operation */
				
#if defined ARCHARM
				/* if manually single stepping then clear ss breakpoints */
				if (currproc->ssaddr1) {
					clearbreakpoint(exe, currproc->ssaddr1, currproc->sstrap1);
					currproc->ssaddr1 = 0;
					currproc->sstrap1 = 0;
				}
				if (currproc->ssaddr2) {
					clearbreakpoint(exe, currproc->ssaddr2, currproc->sstrap2);
					currproc->ssaddr2 = 0;
					currproc->sstrap2 = 0;
				}
#endif

				/* find breakpoint that is single stepping */
				currbp = currproc->ssbp;
				
				if (!currbp) {
					myprintf(0, "run_controller: singlestepping and no stored breakpoint\n");
					exit(E_UNKNOWN);
				}
				
				/* decrement breakpoint count */
				if (!(currbp->disabled) && (currbp->count > 0)) {
					(currbp->count)--;
				}

				/* reestablish breakpoint - we do this always in case it is useful to another thread */
				trapint = setbreakpoint(exe, currbp->address);
				if (!isintbreakpoint(trapint, currbp->address & 0x3)) {
					currbp->trapint = trapint;
				}
				
				if (!(currbp->disabled) && (currbp->count <= 0)) {
					/* count has just hit 0 OR count is infinite - check for stuff to do */
					if (currbp->kill) {
						/* time to die */
						sig = SIGKILL;
					}

					if (currbp->bpmods) {
						/* enable and disable breakpoints */
						modbreakpoints(exe, currbp->bpmods, currproc->bphead, 1);
						modbreakpointsglobal(prochead, currbp->bpmods, exe);
					}
					
					if (currbp->copyonfork) {
						/* enable copyonfork on process */
						currproc->copyonfork = 1;
					} else {
						/* disable copyonfork on process */
						currproc->copyonfork = 0;
					}
					
					/* if count has hit 0, disable it */
					if (!(currbp->count)) {
						currbp->disabled = 1;
					}
				}
		
				/* set process state and continue */
				currproc->state = PROCESS_RUNNING;
				child_continue(exe, sig);
				
			} else {
					
				/* find breakpoint */
				currbp = currproc->bphead;
				bpnum = 1;
				
#if defined ARCHX86
				while ((currbp) && (currbp->address != pc)) {
#elif defined ARCHARM
				while ((currbp) && ((currbp->address & ~0x1) != pc)) {
#endif
					currbp = currbp->next;
					bpnum++;
				}

				if (!currbp) {
#if defined ARCHX86
					if (pc == breakaddr) {
#elif defined ARCHARM
					if (pc == (breakaddr & ~0x1)) {
#endif
						myprintf(3, "process %ld not in preinit but hit initbreak\n", exe);
						currproc->preinit = 0;
					} else {
					
						myprintf(0, "PANIC! process %ld could not find breakpoint 0x%x\n", exe, pc);
					}
				} else {

					/* INTERACT */
					if (!(currproc->preinit) && !(currbp->disabled) && (currbp->count != 0)) {
						
						if (currbp->fnname) {
							myprintf(0, "pid %ld: breakpoint %d: %s (0x%x)\n", exe, bpnum, currbp->fnname, pc);
						} else {
							myprintf(0, "pid %ld: breakpoint %d: 0x%x\n", exe, bpnum, pc);
						}

						/* get registers from child */
						readchildregs(exe, &regs);

						/* display values */
						if (currbp->printregs) {
							myprintf(0, "\tregisters:\n");
							displayregs(currbp->printregs, &regs);
						}

						if (currbp->printptrs) {
							myprintf(0, "\tregister pointers:\n");
							displayptrs(exe, currbp->printptrs, &regs);
						}

						if (currbp->printaddrs) {
							myprintf(0, "\taddresses:\n");
							displayaddrs(exe, currbp->printaddrs, &regs);
						}
					}

					/* clear breakpoint */
					clearbreakpoint(exe, currbp->address, currbp->trapint);

					/* record breakpoint that is single stepping */
					currproc->ssbp = currbp;

					/* set process state */
					currproc->state = PROCESS_SINGLESTEP;
					
					/* execute instruction under breakpoint */
					child_singlestep(currproc);
					
				}
			}
		} else if (ptrace_event == PTRACE_EVENT_FORK) {
			/* process is forking */
			if (ptrace(PTRACE_GETEVENTMSG, exe, NULL, &forkpid) == -1) {
				myprintf(0, "run_controller: PTRACE_GETEVENTMSG failed\n");
				exit(E_PTRACE);
			}

			myprintf(1, "forkpid = %ld\n", forkpid);

			/* create new process */
			newproc = copyprocess(&prochead, exe);
			if (!newproc) {
				myprintf(0, "run_controller: cannot copy process %ld\n", exe);
				exit(E_MALLOC);
			}

						
			/* store the pid */
			newproc->pid = forkpid;
			
			/* mark it as a forked process */
			newproc->thread = 0;
			
			/* cancel the signal and continue */
			sig = 0;
			child_continue(exe, sig);

		} else if (ptrace_event == PTRACE_EVENT_CLONE) {
			/* process is starting a new thread */
			if (ptrace(PTRACE_GETEVENTMSG, exe, NULL, &forkpid) == -1) {
				myprintf(0, "run_controller: PTRACE_GETEVENTMSG failed\n");
				exit(E_PTRACE);
			}

			myprintf(1, "threadpid = %d\n", forkpid);

			/* create new process */
			newproc = copyprocess(&prochead, exe);
			if (!newproc) {
				myprintf(0, "run_controller: cannot copy process %ld\n", exe);
				exit(E_MALLOC);
			}

			
			/* store the pid */
			newproc->pid = forkpid;
			
			/* mark it as a thread (storing the parent pid) to prevent breakpoint enable/disable */
			newproc->thread = threadparent(prochead, exe);
			
			/* cancel the signal and continue */
			sig = 0;
			child_continue(exe, sig);

		} else if (sig == SIGSTOP) {
			myprintf(1, "sig = SIGSTOP (19)\n");

			if (currproc->state == PROCESS_NEW) {
				/* new process starting up */
				
				if (currproc->preinit) {
					/* before initbreak */
					
					currproc->state = PROCESS_RUNNING;
				
				} else {
					/* after initbreak */
					if (currproc->thread) {
						if (!(currproc->copyonthread)) {
							disablebreakpoints(exe, currproc->bphead);
						}
					} else {
						if (!(currproc->copyonfork)) {
							disablebreakpoints(exe, currproc->bphead);
						}
					}
					
					currproc->state = PROCESS_RUNNING;
					
				}
								
				/* cancel the signal */
				sig = SIGCONT;

			} else {
				/* received a SIGSTOP but weren't expecting it - pass through */
				myprintf(1, "exe %ld received external SIGSTOP\n", exe);
			}
			/* continue the exe */
			child_continue(exe, sig);
		} else if (sig == 0) {
			myprintf(0, "process %ld killed\n", exe);
			deleteprocess(&prochead, exe);
		} else {
			myprintf(1, "random signal received: %d\n", sig);
			child_continue(exe, sig);
		}
	}
}


/* initallprocesses resets the init flag on all processes */
void initallprocesses(struct process *head)
{
	if (!head) return;
	
	head->preinit = 0;
	initallprocesses(head->next);
}
		
	
/* threadparent returns the pid of the process that started the specified thread.
   if exe is a process, it returns exe */
pid_t threadparent(struct process *prochead, pid_t exe)
{
	struct process *temp = NULL;
	
	if (!prochead) return 0;
	
	temp = getprocess(prochead, exe);
	if (!temp) return 0;
	
	/* if exe is a process */
	if (!(temp->thread)) {
		/* exe is a process, so it is its own parent process */
		return exe;
	} else {
		/* exe is a thread, so return its parent, recursively */
		return threadparent(prochead, temp->thread);
	}
}


/* setupbreakpoints sets the breakpoints after the initbreak */
void setupbreakpoints(struct process *currproc)
{
	struct breakpoint *ptr = NULL;
	unsigned int trapint;
	
	if (!currproc) {
		myprintf(0, "setupbreakpoints: invalid parameters\n");
		return;
	}
	
	ptr = currproc->bphead;
	while (ptr) {
		if (ptr->address) {
			trapint = setbreakpoint(currproc->pid, ptr->address);
#if defined ARCHX86
			if (!isintbreakpoint(trapint, 0)) {
#elif defined ARCHARM
			if (!isintbreakpoint(trapint, ptr->address & 0x3)) {
#endif
				ptr->trapint = trapint;
			}
		}
		
		ptr = ptr->next;
	}
}


/* set breakpoint */
unsigned int setbreakpoint(pid_t exe, unsigned long breakaddr)
{
	unsigned long addr, origdata, data;

	if (!breakaddr || !exe) {
		myprintf(0, "setbreakpoint: invalid parameters\n");
		return 0;
	}
	
	myprintf(3, "setbreakpoint: %ld, 0x%x\n", exe, breakaddr);

#if defined ARCHX86

	addr = breakaddr; /* break function */

	origdata = readchildword(exe, addr);

	/* check if breakpoint already set */
	if (isintbreakpoint(origdata, 0)) {
		return origdata;
	}

	data = (origdata & ~0xff) | TRAPBYTE;

	writechildword(exe, addr, data);

	return origdata;

#elif defined ARCHARM

	int thumb = 0;

	addr = breakaddr & ~0x1; /* break function with LSB cleared */

	/* arm addresses are 32bit word aligned */
	origdata = readchildword(exe, addr & ~0x3);

	if (breakaddr & 0x1) {
		thumb = 1;
	}

	if ((!thumb) && (addr & 0x2)) {
		myprintf(0, "setbreakpoint: arm, address misalignment, 0x%x\n", addr);
		exit(E_UNKNOWN);
	}

	if (thumb) {
		/* thumb */

		if (addr & 0x2) {
			/* odd half */
			/* check if breakpoint already set */
			if (isintbreakpoint(origdata, breakaddr & 0x3)) {
				return origdata;
			}
			data = (origdata & 0xffff) | (TRAPHALF << 16);
		} else {
			/* even half (e.g. word aligned) */
			/* check if breakpoint already set */
			if (isintbreakpoint(origdata, breakaddr & 0x3)) {
				return origdata;
			}
			data = (origdata & ~0xffff) | TRAPHALF;
		}

	} else {
		/* arm or unsure */

		/* check if breakpoint already set */
		if (isintbreakpoint(origdata, 0)) {
			return origdata;
		}
		/* we need to write addr+2 for arm and addr for thumb */
		data = TRAPINT;
	}

	writechildword(exe, addr & ~0x3, data);

	return origdata;

#endif

}


/* clear breakpoint */
void clearbreakpoint(pid_t exe, unsigned long breakaddr, unsigned int origint)
{
	unsigned long addr, origdata, data;

	if (!breakaddr || !exe) {
		myprintf(0, "clearbreakpoint: invalid parameters\n");
		return;
	}
	
	myprintf(3, "clearbreakpoint: %ld, 0x%x\n", exe, breakaddr);

#if defined ARCHX86

	addr = breakaddr; /* break function */

	origdata = readchildword(exe, addr);

	/* check if breakpoint isn't set */
	if (!isintbreakpoint(origdata, 0)) {
		return;
	}

	data = (origdata & 0xffffff00) | (origint & 0xff);

	writechildword(exe, addr, data);

#elif defined ARCHARM

	int thumb = 0;

	addr = breakaddr & ~0x1; /* break function with LSB cleared */

	/* arm addresses are 32bit word aligned */
	origdata = readchildword(exe, addr & ~0x3);

	if (breakaddr & 0x1) {
		thumb = 1;
	}

	if ((!thumb) && (addr & 0x2)) {
		myprintf(0, "clearbreakpoint: arm, address misalignment, 0x%x\n", addr);
		exit(E_UNKNOWN);
	}

	if (thumb) {
		/* thumb */

		if (addr & 0x2) {
			/* odd half */

			/* check if breakpoint is set */
			if (!isintbreakpoint(origdata, breakaddr & 0x3)) {
				return;
			}
			data = (origdata & 0xffff) | (origint & ~0xffff);
		} else {
			/* even half */

			/* check if breakpoint is set */
			if (!isintbreakpoint(origdata, breakaddr & 0x3)) {
				return;
			}
			data = (origdata & ~0xffff) | (origint & 0xffff);
		}

	} else {
		/* arm or unsure */

		/* check if breakpoint is set */
		if (!isintbreakpoint(origdata, 0)) {
			return;
		}
		data = origint;
	}

	writechildword(exe, addr & ~0x3, data);

#endif
}


/* enablebreakpoint enables a breakpoint by number */
void enablebreakpoint(pid_t exe, int bpnum, struct breakpoint *head)
{
	int i = 1;
	struct breakpoint *ptr = head;

	if ((bpnum <= 0) || (!head)) {
		myprintf(0, "enablebreakpoint invalid arguments\n");
		return;
	}

	while ((i < bpnum) && (ptr->next)) {
		ptr = ptr->next;
		i++;
	}

	if (i == bpnum) {
		/* found the breakpoint */
		ptr->disabled = 0;
		ptr->count = ptr->defcount;
	} else {
		myprintf(1, "enablebreakpoint: breakpoint number %d doesn't exist\n", bpnum);
	}
}


/* disablebreakpoint disables a breakpoint by number */
void disablebreakpoint(pid_t exe, int bpnum, struct breakpoint *head)
{
	int i = 1;
	struct breakpoint *ptr = head;

	if ((bpnum <= 0) || (!head)) {
		myprintf(0, "disablebreakpoint invalid arguments\n");
		return;
	}

	while ((i < bpnum) && (ptr->next)) {
		ptr = ptr->next;
		i++;
	}

	if (i == bpnum) {
		/* found the breakpoint */
		ptr->count = 0;
	} else {
		myprintf(1, "disablebreakpoint: breakpoint number %d doesn't exist\n", bpnum);
	}
}


/* disablebreakpoints disables all breakpoints */
void disablebreakpoints(pid_t exe, struct breakpoint *breakpoints)
{
	struct breakpoint *currbp = NULL;
	
	if (!exe || !breakpoints) return;
	
	currbp = breakpoints;

	while (currbp) {
		if (currbp->address) {
			currbp->disabled = 1;
		}
		currbp = currbp->next;
	}

}


/* modbreakpoints enables and disables breakpoints in the bpmodlist
   local flag overrides the bpmod->global flag; i.e. if local==1 then all bpmods will be actioned,
   otherwise only the ones marked global will be */
void modbreakpoints(pid_t exe, struct bpmodlist *bpmod, struct breakpoint *breakpoints, int local)
{
	if (!breakpoints) {
		myprintf(0, "modbreakpoints no breakpoints\n");
		return;
	}

	if (!bpmod) {
		return;
	}

	if (local || bpmod->global) {
	
		if (bpmod->action == BPACTION_ENABLE) {
			enablebreakpoint(exe, bpmod->bpnum, breakpoints);
		} else if (bpmod->action == BPACTION_DISABLE) {
			disablebreakpoint(exe, bpmod->bpnum, breakpoints);
		}
	}
	
	modbreakpoints(exe, bpmod->next, breakpoints, local);
}


/* modbreakpointsglobal enables and disables breakpoints in all processes */
void modbreakpointsglobal(struct process *prochead, struct bpmodlist *bpmod, pid_t exe)
{
	struct process *currproc = prochead;
	
	if (!prochead || !bpmod) {
		myprintf(0, "modbreakpointsglobal: invalid parameters\n");
		return;
	}
	
	/* for all processes that are not exe and have breakpoints,
	   modify the breakpoints */
	
	while (currproc) {
		if ((currproc->pid != exe) && (currproc->bphead)) {
			modbreakpoints(currproc->pid, bpmod, currproc->bphead, 0);
		}
		currproc = currproc->next;
	}
}


/* getpc returns the process pc */
unsigned long getpc(pid_t exe)
{
	REGS_STRUCT regs;

	/* get registers */
	readchildregs(exe, &regs);

	return regs.REG_PC;
}


/* decpc decrements the program counter of the child process */
unsigned long decpc(pid_t exe)
{
	REGS_STRUCT regs;

	/* get registers */
	readchildregs(exe, &regs);

	/* decrement pc */

#if defined ARCHX86

	regs.REG_PC--;

#elif defined ARCHARM

	if (regs.REG_CPSR & CPSR_THUMB) {
		regs.REG_PC -= 2;
	} else {
		regs.REG_PC -= 4;
	}

#endif

	/* set registers */
	writechildregs(exe, &regs);

	return regs.REG_PC;
}


/* display_stopstate prints a verbose message to indicate the reason the child process stopped */
void display_stopstate(pid_t exe, int status, char *location)
{
	if (!location) {
		myprintf(0, "display_stopstate: invalid parameters\n");
		return;
	}
	
	if (WIFSTOPPED( status )) {
		myprintf(2, "%s: Process %ld stopped %d\n", location, exe, WSTOPSIG( status ) );
		myprintf(2, "%s: raw status = 0x%0x\n", location, status);
		if (!WIFEXITED(status)) {
			myprintf(2, "%s: pc = 0x%0x\n", location, getpc(exe));
		}
	}
	if (WIFEXITED( status )) {
		myprintf(0, "%s: Process %ld exited...\n", location, exe );
	}
}


/* child_continue runs the child process to the next stopping point (trap or segv) */
void child_continue(pid_t exe, int signal)
{
	myprintf(3, "child_continue\n");

	if (ptrace( PTRACE_CONT, exe, NULL, signal ) < 0) {
		myprintf(0, "ptrace_cont error: %s\n", strerror(errno));
		exit(E_PTRACE);
	}
}


/* child_singlestep runs the child process for one instruction */
void child_singlestep(struct process *proc)
{
	myprintf(3, "child_singlestep\n");

	if (!proc) {
		myprintf(0, "child_singlestep: no process\n");
		return;
	}
	
#if defined ARCHX86

	/* x86 ptrace() implements singlestep */
	if (ptrace(PTRACE_SINGLESTEP, proc->pid, NULL, NULL)) {
		myprintf(0, "ptrace_singlestep error: %s\n", strerror(errno));
		exit(E_PTRACE);
	}

#elif defined ARCHARM

	arm_singlestep(proc);

#endif

}


/* read a word from child process */
unsigned long readchildword(pid_t pid, unsigned long addr)
{
	unsigned long word;

	/* read a word from child process */
	word = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
	if (word == -1) {
		if (errno) {
			myprintf(0, "readchildword ptrace_peektext error: %s\n", strerror(errno));
			exit(E_PTRACE);
		}
	}

	myprintf(4, "read 0x%08lx from %ld:0x%x\n", word, pid, addr);

	return word;
}


/* write a word to child process */
void writechildword(pid_t pid, unsigned long addr, unsigned long word)
{
	unsigned long check;

	/* write word to child process */
	if (ptrace(PTRACE_POKETEXT, pid, addr, word)) {
		myprintf(0, "writechildword ptrace_poketext error: %s\n", strerror(errno));
		exit(E_PTRACE);
	}

	check = readchildword(pid, addr);

	if (check != word) {
		myprintf(0, "writechildword word not written error\n");
		exit(E_PTRACE);
	}

	myprintf(4, "wrote 0x%08lx to %ld:0x%x\n", word, pid, addr);
}


/* read a buffer from child process */
unsigned char *readchildbuffer(pid_t pid, unsigned long addr, int size)
{
	int i = 0;
	int count = 0;
	unsigned long word;
	unsigned long *buffer = NULL;
	unsigned long *ptr = NULL;
	int actualsize = size;

	/* increase size to word boundary */
	while ((actualsize % 4) != 0) {
		actualsize++;
	}

	/* create some space for the result */
	buffer = (unsigned long *)malloc(actualsize);
	ptr = buffer;

	/* read the data from child process in 4 byte chunks */
	while (count < actualsize) {
		word = readchildword(pid, addr + count);

		/* update pointers */
		count += 4;
		ptr[i++] = word;
	}

	return (unsigned char *)buffer;
}


/* read a string from child process */
unsigned char *readchildstring(pid_t pid, unsigned long addr)
{
	int len = 0;
	unsigned long word = 0;
	unsigned char *wordptr = (unsigned char *)&word;

	/* read a word at a time and count to end of string */
	word = readchildword(pid, addr);
	len += 4;

	/* any of the four bytes is 0x00 indicates end of string */
	while ((wordptr[0]) && (wordptr[1]) && (wordptr[2]) && (wordptr[3])) {
		word = readchildword(pid, addr + len);
		len += 4;
	}

	/* now we have a length, read the string */
	return readchildbuffer(pid, addr, len);
}


/* readchildregs reads the registers from the child process */
void readchildregs(pid_t pid, REGS_STRUCT *regs)
{
	if (!regs) {
		myprintf(0, "readchildregs: invalid parameters\n");
		return;
	}
	
	/* get registers */
	memset(regs, 0, sizeof(REGS_STRUCT));
	if (ptrace(PTRACE_GETREGS, pid, NULL, regs)) {
		myprintf(0, "readchildregs ptrace_getregs failed: %s\n", strerror(errno));
		exit(E_PTRACE);
	}
}


/* writechildregs writes the registers to the child process */
void writechildregs(pid_t pid, REGS_STRUCT *regs)
{
	if (!regs) {
		myprintf(0, "writechildregs: invalid parameters\n");
		return;
	}
	
	/* set registers */
	if (ptrace(PTRACE_SETREGS, pid, NULL, regs)) {
		myprintf(0, "writechildregs ptrace_setregs failed: %s\n", strerror(errno));
		exit(E_PTRACE);
	}
}


/* regindex returns the index of the specified register in the REGS_STRUCT */
int regindex(char *reg)
{
#if defined ARCHX86
	char **ptr = validregs;
#elif defined ARCHARM
	int regnum = -1;
#endif

	if (!reg) {
		myprintf(0, "regindex empty register string\n");
		return -1;
	}

#if defined ARCHX86
	/* search the validregs list */
	while (*ptr != NULL) {
		if (!strcmp(reg, *ptr)) {
			return (ptr - validregs);
		}
		ptr++;
	}

	/* not found */
	return -1;

#elif defined ARCHARM
	/* check reg is in format rN or rNN and extract N or NN */
	if ((reg[0] != 'r') || (strlen(reg) < 2) || (strlen(reg) > 3)) {
		return -1;
	}

	regnum = strtol(reg+1, NULL, 0);
	if ((regnum >= 0) && (regnum < 18)) {
		return regnum;
	} else {
		return -1;
	}

#endif
}


/* isvalidreg checks if a register string is a valid register */
int isvalidreg(char *reg)
{
	if ((reg == NULL) || (strlen(reg) <= 0)) {
		myprintf(0, "isvalidreg empty argument\n");
		return 0;
	}

	if (regindex(reg) > -1) {
		return 1;
	} else {
		return 0;
	}
}


/* getreg returns the value for a particular register string */
unsigned long getreg(char *reg, REGS_STRUCT *regs)
{
	int index;
	unsigned long *regslong = (unsigned long *)regs;

	if ((reg == NULL) || (strlen(reg) <= 0) || (regs == NULL) || (!isvalidreg(reg))) {
		myprintf(0, "getreg invalid arguments\n");
		exit(E_UNKNOWN);
	}

	index = regindex(reg);

	if (index < 0) {
		myprintf(0, "getreg invalid reg\n");
		exit(E_UNKNOWN);
	}

	return regslong[index];
}


/* displayregs outputs the values of the registers in the reglist */
void displayregs(struct reglist *printregs, REGS_STRUCT *regs)
{
	if (!regs) {
		myprintf(0, "displayregs empty regs struct\n");
		exit(E_UNKNOWN);
	}

	if (printregs) {
		if (isvalidreg(printregs->reg)) {
			myprintf(0, "\t\t%s\t0x%x\n", printregs->reg, getreg(printregs->reg, regs));
			displayregs(printregs->next, regs);
		}
	}
}


/* signof returns the sign of a number as a char */
char signof(int val)
{
	if (val >= 0) return '+';
	else return '-';
}


/* displayaddrs outputs the buffers specified in the addrlist */
void displayaddrs(pid_t pid, struct addrlist *printaddrs, REGS_STRUCT *regs)
{
	unsigned char *buffer = NULL;
	unsigned long address;
	int dsize = 1;

	if (!regs) {
		myprintf(0, "displayaddrs empty regs struct\n");
		exit(E_UNKNOWN);
	}

	if (printaddrs) {
		address = printaddrs->address + printaddrs->offset;
		if ((address) && (address != -1)) {
			/* dereference it if required */
			if (printaddrs->deref) {
				address = readchildword(pid, address);
			}

			if ((address) && (address != -1)) {
				if (printaddrs->format == 's') {
					buffer = readchildstring(pid, address);
					if (printaddrs->deref) {
						myprintf(0, "\t\t(0x%x%c0x%x) %s\n", (printaddrs->address), signof(printaddrs->offset), abs(printaddrs->offset), buffer);
					} else {
						myprintf(0, "\t\t0x%x%c0x%x %s\n", (printaddrs->address), signof(printaddrs->offset), abs(printaddrs->offset), buffer);
					}
					free(buffer);
				} else {
					switch(printaddrs->format) {
						case 'b':
							dsize = 1;
							break;
						case 'h':
							dsize = 2;
							break;
						case 'w':
							dsize = 4;
							break;
						default:
							myprintf(0, "displayaddrs unknown format\n");
							exit(E_UNKNOWN);
					}
					buffer = readchildbuffer(pid, address, printaddrs->size * dsize);
					if (printaddrs->deref) {
						myprintf(0, "\t\t(0x%x%c0x%x):\n", (printaddrs->address), signof(printaddrs->offset), abs(printaddrs->offset));
					} else {
						myprintf(0, "\t\t0x%x%c0x%x:\n", (printaddrs->address), signof(printaddrs->offset), abs(printaddrs->offset));
					}
					hexdump(buffer, printaddrs->format, printaddrs->size);
				}
			} else {
				myprintf(0, "\t\t(0x%x%c0x%x) == 0x%x\n", (printaddrs->address), signof(printaddrs->offset), abs(printaddrs->offset), address);
			}
		} else {
			myprintf(0, "\t\t0x%x%c0x%x == 0x%x\n", (printaddrs->address), signof(printaddrs->offset), abs(printaddrs->offset), address);
		}
		displayaddrs(pid, printaddrs->next, regs);
	}
}


/* displayptrs outputs the buffers pointed to be registers specified in ptrlist */
void displayptrs(pid_t pid, struct ptrlist *printptrs, REGS_STRUCT *regs)
{
	unsigned long address;
	unsigned char *buffer = NULL;
	int dsize = 1;

	if (!regs) {
		myprintf(0, "displayptrs empty regs struct\n");
		exit(E_UNKNOWN);
	}

	if (printptrs) {
		if (isvalidreg(printptrs->reg)) {
			address = getreg(printptrs->reg, regs) + printptrs->offset;
			if ((address) && (address != -1)) {
				/* dereference it if required */
				if (printptrs->deref) {
					address = readchildword(pid, address);
				}

				if ((address) && (address != -1)) {
					if (printptrs->format == 's') {
						buffer = readchildstring(pid, address);
						if (printptrs->deref) {
							myprintf(0, "\t\t(%s%c0x%x) %s\n", printptrs->reg, signof(printptrs->offset), abs(printptrs->offset), buffer);
						} else {
							myprintf(0, "\t\t%s%c0x%x %s\n", printptrs->reg, signof(printptrs->offset), abs(printptrs->offset), buffer);
						}
						free(buffer);
					} else {
						switch(printptrs->format) {
							case 'b':
								dsize = 1;
								break;
							case 'h':
								dsize = 2;
								break;
							case 'w':
								dsize = 4;
								break;
							default:
								myprintf(0, "displayptrs unknown format\n");
								exit(E_UNKNOWN);
						}
						buffer = readchildbuffer(pid, address, printptrs->size * dsize);
						if (printptrs->deref) {
							myprintf(0, "\t\t(%s%c0x%x):\n", printptrs->reg, signof(printptrs->offset), abs(printptrs->offset));
						} else {
							myprintf(0, "\t\t%s%c0x%x:\n", printptrs->reg, signof(printptrs->offset), abs(printptrs->offset));
						}
						hexdump(buffer, printptrs->format, printptrs->size);
						free(buffer);
					}
				} else {
					myprintf(0, "\t\t(%s%c0x%x) == 0x%x\n", printptrs->reg, signof(printptrs->offset), abs(printptrs->offset), address);
				}
			} else {
				myprintf(0, "\t\t%s%c0x%x == 0x%x\n", printptrs->reg, signof(printptrs->offset), abs(printptrs->offset), address);
			}
		}
		displayptrs(pid, printptrs->next, regs);
	}
}





