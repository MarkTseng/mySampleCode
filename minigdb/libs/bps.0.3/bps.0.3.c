/* bps - Breakpoint Shenanigans - K Sheldrake */

#define BPSVER "0.3"

/*
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
** The idea is to automate some of the things I use gdb for, namely repeatedly breaking on certain addresses and
** displaying the contents of registers and memory locations, and automatically continuing.
**
** Breakpoints can be specified as either addresses or function names; display information can be contents of
** registers, or the contents of buffers pointed to by addresses or registers, with offsets.  Buffers need to
** have the format specified - b(ytes), h(alfwords), w(ords), s(tring); size (not for string); and an optional offset.
** For each breakpoint you can specify the maximum number of times to break on it.  You can also specify a function
** or address to hit before enabling the breakpoints.
**
** edit ARCH in Makefile then run make.
**
** Supported platforms:
** * 32bit x86 (Arch) linux
** * 32bit ARM (Arch) linux, arm and thumb (but not thumb2)
** Expecting it to work on any other platforms is very optimistic of you.
**
** To do:
**
** * Representative ARM testing.
** * Port to MIPS.
** * Tainting of values and buffers - the idea is to be able to predictably change data (overwrite or XOR) such that
**   the data will be identifiable when it is observed later.
** * Use of register (and derefed offset register) values as buffer parameters, e.g. size.
** * Set UID for child.
** * Conditional breakpoints.
** * Read options from file rather than command line.
** * Code tidy and audit.
** * Convert functions to library.
** * Produce example code that uses library.
**
** Version numbering:
** Whole version numbers, such as 1.0, are public releases which should be stable and working.
** Version numbers with 1 decimal place, such as 0.2, are interim releases which should also be stable and working, but lacking functionality.
** Version numbers with 2 decimal places, such as 0.2.3, are development stages building towards the next interim release.
**
** History:
**
** v0.3 23/2/2015
**
** Port to Thumb. This appears to work for standard thumb, such as that found on ARMv4-ARMv6 (ARM7-ARM11).  It does
** not currently support thumb2 found on later ARM processors.  It supports switching from arm to thumb and vice
** versa via B, BX, LDR, LDM and POP.  It does not support switching state as a result of a data processing operation,
** such as MOV or ADD (this appears to be only in thumb2).  See the arm_branchaddr() and thumb_branchaddr() functions
** for details.  Note that the addresses returned from these functions have the LSB reset if branch points to an ARM
** instruction, and set if branch points to a thumb instruction.
** Divided source into separate files and added Makefile with configurable ARCH variable.
** Added multi-process (fork) and multi-thread (pthread_create) support.
** Now supports pass-through of the program's own signals.
** Improved output function for auto-indent.
**
** v0.2
**
** Ported to ARM 32bit.
** Wrote arm_singlestep() to simulate hardware single stepping with software breakpoints.
** Tested majority of arm_singlestep components.
** Moved decpc() to child_continue() as that's the only placed it should be called.
** Updated usage().
**
** v0.1
**
** First attempt appears to work on x86.
*/

#if !defined ARCHX86 && !defined ARCHARM
	#error "You must define ARCH with -DARCH[X86|ARM]"
#endif

#include "bps.h"

/* usage function displays args and optional message */
void usage(char *msg) {
	printf("\n");
	printf("bps - Breakpoint Shenanigans - v%s K Sheldrake\n\n", BPSVER);
	printf("Bps comes with ABSOLUTELY NO WARRANTY.\n");
	printf("This is free software, and you are welcome\n");
	printf("to redistribute it under certain conditions.\n");
	printf("Bps is provided under GPLv2.\n");
	printf("See http://www.gnu.org/licenses/gpl-2.0.html\n");
	printf("for details.\n\n");
	printf("Automatically display interesting info on breakpoints.\n\n");
	printf("Use 'bps -l exe' to list the functions in an elf binary.\n\n");
	printf("Use bps with either -f or -F followed by items to display (-r, -R and -A), and a command line after\n");
	printf("a '--' argument, to run the command line with the specified breakpoint enabled.\n\n");
	printf("You can specify any number of breakpoints, one after another; each -r, -R, -A, -c, -k, -e and -d\n");
	printf("option only affects the most recent -f/-F breakpoint that it follows. You can also specify multiple\n");
	printf("display and enable/disable options per breakpoint.\n\n");
	printf("If unspecified, count defaults to infinite (-1).  Setting it to 0 disables the breakpoint.\n");
	printf("Disabled breakpoints can be enabled by other breakpoints with -e, triggered after count traps.\n");
	printf("Breakpoints can also reenable themselves.\n\n");
	printf("Display registers with -r, buffers pointed to by registers with -R and buffers in memory with -A.\n");
	printf("x86 regs are usual names; ARM regs are r0 - r17.\n");
	printf("Use formats b/B for bytes, h/H for halfwords, w/W for words or s/S for strings.\n");
	printf("Buffer size must be specified for b/B, h/H and w/W formats.\n");
	printf("Capital letter formats cause the calculated address to be dereferenced before displaying.\n");
	printf("Numbers can be specified in decimal (no prefix), hex (0x prefix) or octal (0 prefix).\n\n");
	printf("To use a named pipe, first create it with 'mkfifo -m 0600 </path/to/pipe>' and then listen on it\n");
	printf("in a separate shell with 'cat </path/to/pipe>' prior to running bps.\n\n");
	printf("Bps traces the launched process and all its children. The default configuration is that breakpoints\n");
	printf("are not copied to the children, although this can be enabled globally with -z before breakpoints are\n");
	printf("specified, or enabled on processes upon flagged breakpoints, also with -z but after a breakpoint.\n");
	printf("Bps also defaults to breakpoint enable/disable options only affecting the process in which the\n");
	printf("breakpoint was triggered, but this can be made global with -G. Individual breakpoints can cause\n");
	printf("breakpoints to be enabled/disabled globally or locally (the opposite of -G) with -g after a\n");
	printf("breakpoint.\n\n");
	printf("Separate the bps options from the command line with -- .\n\n");
	printf("\n");
	printf("bps - Breakpoint Shenanigans - v%s K Sheldrake\n\n", BPSVER);
	printf("Automatically display interesting info on breakpoints.\n\n");
	printf("bps -l exe                                       - list functions in exe\n");
	printf("bps <options> -- exe [arg1 [arg2 ...]]           - run exe with breakpoints\n\n");
	printf("    -z/-Z                                        - enable/disable copy of breakpoints on fork\n");
	printf("                                                   (initial configuration)\n");
	printf("    -y/-Y                                        - ditto for new threads\n");
	printf("    -G                                           - breakpoint enable/disable affects all processes\n");
	printf("                                                   (default is to only affect process trapped on)\n");
	printf("    -b init break fn | -B init break address     - optionally specify an initial breakpoint\n");
	printf("                                                   at which to enable the real breakpoints\n");
	printf("    -T                                           - initial breakpoint is thumb (arm only)\n");
	printf("    -I                                           - establish breakpoints on all processes on initial\n");
	printf("                                                   breakpoint\n");
	printf("    -f function name | -F address                - specify a breakpoint\n");
	printf("     -t                                           - specify thumb rather than arm (arm only)\n");
	printf("     -r register                                  - register to display\n");
	printf("     -R register:format[:size][:offset]           - register buffer to display\n");
	printf("     -A address:format[:size][:offset]            - address buffer to display\n");
	printf("     -c count                                     - number of times to trap\n");
	printf("     -D                                           - disable breakpoint on launch\n");
	printf("     -k                                           - kill exe after count traps\n");
	printf("     -e breakpoint number                         - breakpoint to enable after count traps\n");
	printf("     -d breakpoint number                         - breakpoint to disable after count traps\n");
	printf("     -z/-Z                                        - enable/disable copy of breakpoints on\n");
	printf("                                                    following forks\n");
	printf("     -y/-Y                                        - ditto for new threads\n");
	printf("     -g                                           - reverse effect of -G for following enable/disable\n");
	printf("                                                    options for this breakpoint - if -G specified, then\n");
	printf("                                                    -g makes following options local, and vice versa\n");
	printf("    -o                                           - send output to stderr rather than stdout\n");
	printf("    -p pipe for output                           - send output to named pipe rather than stdout,\n");
	printf("                                                   for times when stderr isn't far enough away\n");
	printf("    -v                                           - verbose; use multiple times for extra info\n");
	printf("    -- exe [arg1 [arg2 ...]]                     - command line to run\n");
	printf("\n");
	if (msg != NULL) {
		printf("Error: %s\n", msg);
	}
	exit(E_ARGS);
}




/* main - here we go */

int main(int argc, char *argv[], char *envp[])
{
	int c;
	int i;
	char **cmdline = NULL;
	int numargs = 0;

	struct symbol *symbols = NULL;
	int symtot = 0;

	unsigned long breakaddr = 0;
	char *breakfn = NULL;
	int breakthumb = 0;
	int bpmodglobal = 0;
	int bpactionglobal = 0;
	int copyonfork = 0;
	int copyonthread = 0;
	int initbreakglobal = 0;

	struct breakpoint *bphead = NULL;
	struct breakpoint *currbp = NULL;

	struct reglist *currreg;
	struct ptrlist *currptr;
	struct addrlist *curraddr;
	struct bpmodlist *currbpmod;


	/* sort options */
	opterr = 0;

	while ((c = getopt(argc, (char **)argv, "l:GIb:B:p:of:F:zZyYTtgr:R:A:c:Dke:d:vh")) != -1) {
		switch(c) {
			case 'h':
				usage(NULL);
				break;
			case 'l':
				if (strlen(optarg) > 0) {
					symtot = readsyms(&symbols, optarg, 1);
					exit(0);
				}
				break;
			case 'G':
				bpmodglobal = 1;
				break;
			case 'I':
				initbreakglobal = 1;
				break;
			case 'b':
				if (strlen(optarg) > 0) {
					breakfn = optarg;
				}
				break;
			case 'B':
				if (strlen(optarg) > 0) {
					breakaddr = (unsigned long) strtoll(optarg, NULL, 0);
				}
				break;
			case 'T':
				breakthumb = 1;
				break;
			case 'f':
			case 'F':
				if (strlen(optarg) > 0) {
					currbp = addbp(&bphead);
					if (c == 'f') {
						currbp->fnname = optarg;
					} else {
						currbp->address = (unsigned long) strtoll(optarg, NULL, 0);
					}
				}
				bpactionglobal = bpmodglobal;
				currbp->copyonfork = copyonfork;
				break;
			case 't':
				if (currbp) {
					currbp->thumb = 1;
				} else {
					usage("need to specify -f or -F before -t");
				}
				break;
			case 'g':
				if (currbp) {
					if (bpmodglobal) {
						bpactionglobal = 0;
					} else {
						bpactionglobal = 1;
					}
				} else {
					usage("need to specify -f or -F before -g");
				}
				break;
			case 'z':
				if (currbp) {
					currbp->copyonfork = 1;
				} else {
					copyonfork = 1;
				}
				break;
			case 'Z':
				if (currbp) {
					currbp->copyonfork = 0;
				} else {
					copyonfork = 0;
				}
				break;
			case 'y':
				if (currbp) {
					currbp->copyonthread = 1;
				} else {
					copyonthread = 1;
				}
				break;
			case 'Y':
				if (currbp) {
					currbp->copyonthread = 0;
				} else {
					copyonthread = 0;
				}
				break;
			case 'c':
				if (strlen(optarg) > 0) {
					if (currbp) {
						currbp->defcount = strtol(optarg, NULL, 0);
						if (currbp->count == -1) {
							currbp->count = currbp->defcount;
						}
					} else {
						usage("need to specify -f or -F before -c");
					}
				}
				break;
			case 'D':
				if (currbp) {
					currbp->count = 0;
				} else {
					usage("need to specify -f or -F before -D");
				}
				break;
			case 'k':
				if (currbp) {
					currbp->kill = 1;
				} else {
					usage("need to specify -f or -F before -k");
				}
				break;
			case 'r':
				if (strlen(optarg) > 0) {
					if (currbp) {
						if (isvalidreg(optarg)) {
							currreg = addreg(&(currbp->printregs));
							currreg->reg = optarg;
						} else {
							usage("invalid register");
						}
					} else {
						usage("need to specify -f or -F before -r");
					}
				}
				break;
			case 'R':
				if (strlen(optarg) > 0) {
					if (currbp) {
						currptr = addptr(&(currbp->printptrs));
						if (!processptrarg(optarg, currptr)) {
							usage("error processing register ptr argument");
						}
					} else {
						usage("need to specify -f or -F before -R");
					}
				}
				break;
			case 'A':
				if (strlen(optarg) > 0) {
					if (currbp) {
						curraddr = addaddr(&(currbp->printaddrs));
						if (!processaddrarg(optarg, curraddr)) {
							usage("error processing address ptr argument");
						}
					} else {
						usage("need to specify -f or -F before -A");
					}
				}
				break;
			case 'e':
				if (strlen(optarg) > 0) {
					if (currbp) {
						currbpmod = addbpmod(&(currbp->bpmods));
						currbpmod->bpnum = strtol(optarg, NULL, 0);
						currbpmod->action = BPACTION_ENABLE;
						currbpmod->global = bpactionglobal;
					} else {
						usage("need to specify -f or -F before -e");
					}
				}
				break;
			case 'd':
				if (strlen(optarg) > 0) {
					if (currbp) {
						currbpmod = addbpmod(&(currbp->bpmods));
						currbpmod->bpnum = strtol(optarg, NULL, 0);
						currbpmod->action = BPACTION_DISABLE;
						currbpmod->global = bpactionglobal;
					} else {
						usage("need to specify -f or -F before -d");
					}
				}
				break;
			case 'v':
				verbose++;
				break;
			case 'o':
				output = 2;
				break;
			case 'p':
				if (strlen(optarg) > 0) {
					output = openpipe(optarg);
				}
				break;
			default:
				usage("unknown switch");
				break;
		}

	}

	cmdline = argv + optind;
	numargs = argc - optind;

	if (numargs <= 0) {
		usage("no command specified");
	}

	myprintf(1, "cmdline: ");
	for (i=0; i<numargs; i++) {
		myprintf(1, "%s ", cmdline[i]);
	}
	myprintf(1, "\n\n");


	/* check we have some breakpoints */
	if (bphead == NULL) {
		usage("no breakpoints");
	}

	symtot = readsyms(&symbols, cmdline[0], 0);

	if (symtot <= 0) {
		usage("cannot read symbols");
	}

	/* resolve function names */

	/* initial break address */
	if (!breakaddr) {
		if ((breakfn) && (strlen(breakfn) > 0)) {
			breakaddr = symaddr(symbols, symtot, breakfn);
			if (!breakaddr) {
				usage("cannot resolve init break function");
			}
		}
	}
	/* if thumb, set the LSB */
	if ((breakaddr) && (breakthumb)) {
		breakaddr = breakaddr | 0x1;
	}

	/* breakpoints */
	currbp = bphead;
	while (currbp) {
		if (!currbp->address) {
			if (strlen(currbp->fnname) > 0) {
				currbp->address = symaddr(symbols, symtot, currbp->fnname);
				if (!currbp->address) {
					usage("cannot resolve breakpoint address");
				}
			} else {
				usage("empty breakpoint function name");
			}
		}
		/* if thumb, set the LSB */
		if (currbp->thumb) {
			currbp->address = currbp->address | 0x1;
		}
		currbp = currbp->next;
	}

	printf("Bps comes with ABSOLUTELY NO WARRANTY.\n");
	printf("This is free software, and you are welcome\n");
	printf("to redistribute it under certain conditions.\n");
	printf("Bps is provided under GPLv2.\n");
	printf("See http://www.gnu.org/licenses/gpl-2.0.html\n");
	printf("for details.\n\n");


	/* display init break function */
	if ((breakfn) && (strlen(breakfn) > 0)) {
		myprintf(1, "init break fn: %s %p\n\n", breakfn, (void *)breakaddr);
	} else if (breakaddr) {
		myprintf(1, "init break addr: %p\n\n", (void *)breakaddr);
	}

	/* display breakpoints */
	if (verbose >= 1) {
		display_breakpoints(bphead);
		myprintf(1, "\n");
	}

	run_debugger(cmdline, envp, breakaddr, bphead, copyonfork, copyonthread, initbreakglobal);

	return 0;
}

