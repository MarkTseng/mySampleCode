/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_data.c - functions that manipualte bps data
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

#if !defined _BPS_DATA_H
#define _BPS_DATA_H 1

#include <stdlib.h>
#include "bps_error.h"
#include "bps_util.h"
#include "bps_elf.h"

#define S_REG 10
#define BPACTION_NONE 0
#define BPACTION_ENABLE 1
#define BPACTION_DISABLE 2
#define PROCESS_PREINIT 0
#define PROCESS_RUNNING 1
#define PROCESS_NEW 2
#define PROCESS_SINGLESTEP 3

/*
** structs
*/

/* list of registers to display */
struct reglist;
struct reglist
{
	char *reg;
	struct reglist *next;
};

/* list of address buffers to display */
struct addrlist;
struct addrlist
{
	unsigned long address;
	unsigned char format;
	int size;
	int offset;
	int deref;
	struct addrlist *next;
};

/* list of register pointed buffers */
struct ptrlist;
struct ptrlist
{
	char *reg;
	unsigned char format;
	int size;
	int offset;
	int deref;
	struct ptrlist *next;
};

/* list of breakpoints to enable or disable when count completes */
struct bpmodlist;
struct bpmodlist
{
	int bpnum;
	int action;
	int global;
	struct bpmodlist *next;
};

/* list of breakpoints */
struct breakpoint;
struct breakpoint
{
	char *fnname;
	unsigned long address;
	int thumb;
	int count;
	int defcount;
	int disabled;
	int kill;
	int copyonfork;
	int copyonthread;
	unsigned int trapint;
	struct reglist *printregs;
	struct addrlist *printaddrs;
	struct ptrlist *printptrs;
	struct bpmodlist *bpmods;
	struct breakpoint *next;
};

/* list of processes */
struct process;
struct process
{
	pid_t pid;
	struct breakpoint *bphead;
	int state;
	int preinit;
	int copyonfork;
	int copyonthread;
	int thread;
	unsigned long ssaddr1;
	unsigned long ssaddr2;
	unsigned int sstrap1;
	unsigned int sstrap2;
	int modbps;
	struct breakpoint *ssbp;
	struct bpmodlist *bpmods;
	struct process *next;
};


/* list functions */
void display_breakpoints(struct breakpoint *bphead);
struct breakpoint *addbp(struct breakpoint **head);
void copybp(struct breakpoint *dst, struct breakpoint *src);
struct breakpoint *copybps(struct breakpoint *head);
void deletebps(struct breakpoint *breakpoints);
struct reglist *addreg(struct reglist **head);
void copyreg(struct reglist *dst, struct reglist *src);
struct reglist *copyregs(struct reglist *head);
struct addrlist *addaddr(struct addrlist **head);
void copyaddr(struct addrlist *dst, struct addrlist *src);
struct addrlist *copyaddrs(struct addrlist *head);
struct ptrlist *addptr(struct ptrlist **head);
void copyptr(struct ptrlist *dst, struct ptrlist *src);
struct ptrlist *copyptrs(struct ptrlist *head);
struct bpmodlist *addbpmod(struct bpmodlist **head);
void copybpmod(struct bpmodlist *dst, struct bpmodlist *src);
struct bpmodlist *copybpmods(struct bpmodlist *head);
void extendbpmods(struct bpmodlist **dst, struct bpmodlist *src);
void deletebpmods(struct bpmodlist *head);
struct process *addprocess(struct process **head);
struct process *getprocess(struct process *head, pid_t pid);
struct process *copyprocess(struct process **head, pid_t pid);
void deleteprocess(struct process **head, pid_t pid);

#endif

