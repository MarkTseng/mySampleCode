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

#include "bps_data.h"

/*
** list functions
*/

/* display_breakpoints walks the breakpoint list and prints out the values */
void display_breakpoints(struct breakpoint *bphead)
{
	struct breakpoint *currbp = bphead;
	struct reglist *currreg;
	struct ptrlist *currptr;
	struct addrlist *curraddr;
	struct bpmodlist *currbpmod;
	int i=1;

	while (currbp != NULL) {

		myprintf(1, "breakpoint %d: count = %d\n", i, currbp->defcount);
		if (currbp->count == 0) {
			myprintf(1, "\tdisabled = TRUE\n");
		}
		if (currbp->fnname) {
			myprintf(1, "\tfnname = %s\n", currbp->fnname);
		}
		if (currbp->address) {
			myprintf(1, "\taddress = %p\n", (void *)(currbp->address));
		}

		if (currbp->kill) {
			myprintf(1, "\tkill = TRUE\n");
		}

		if (currbp->printregs) {
			myprintf(1, "\treglist:\n");
			currreg = currbp->printregs;
			while (currreg != NULL) {
				myprintf(1, "\t\t%s\n", currreg->reg);
				currreg = currreg->next;
			}
		}

		if (currbp->printptrs) {
			myprintf(1, "\tptrlist:\n");
			currptr = currbp->printptrs;
			while (currptr != NULL) {
				myprintf(1, "\t\t%s %c %d %d\n", currptr->reg, currptr->format, currptr->size, currptr->offset);
				currptr = currptr->next;
			}
		}

		if (currbp->printaddrs) {
			myprintf(1, "\taddrlist:\n");
			curraddr = currbp->printaddrs;
			while (curraddr != NULL) {
				myprintf(1, "\t\t%p %c %d %d\n", (void *)(curraddr->address), curraddr->format, curraddr->size, curraddr->offset);
				curraddr = curraddr->next;
			}
		}

		if (currbp->bpmods) {
			myprintf(1, "\tbpmodlist:\n");
			currbpmod = currbp->bpmods;
			while (currbpmod != NULL) {
				if (currbpmod->action == BPACTION_ENABLE) {
					myprintf(1, "\t\tenable breakpoint %d\n", currbpmod->bpnum);
				} else if (currbpmod->action == BPACTION_DISABLE) {
                    myprintf(1, "\t\tdisable breakpoint %d\n", currbpmod->bpnum);
				}
				currbpmod = currbpmod->next;
			}
		}

		currbp = currbp->next;
		i++;
	}
}


/* addbp adds a new breakpoint */
struct breakpoint *addbp(struct breakpoint **head)
{
	struct breakpoint *currbp = NULL;

	/* if head is NULL then this is an error */
	if (head == NULL) return NULL;

	if (*head == NULL) {
		/* if *head is NULL then the list is empty */

		*head = (struct breakpoint *)malloc(sizeof(struct breakpoint));
		currbp = *head;
	} else if ((*head)->next != NULL) {
		/* walk to the last node */
		return addbp(&((*head)->next));
	} else {
		(*head)->next = (struct breakpoint *)malloc(sizeof(struct breakpoint));
		currbp = (*head)->next;
	}

	if (currbp == NULL) {
		myprintf(0, "addbp malloc error\n");
		exit(E_MALLOC);
	}

	currbp->fnname = 0;
	currbp->address = 0;
	currbp->thumb = 0;
	currbp->count = -1;
	currbp->defcount = -1;
	currbp->disabled = 0;
	currbp->kill = 0;
	currbp->copyonfork = 0;
	currbp->copyonthread = 0;
	currbp->trapint = 0;
	currbp->printregs = 0;
	currbp->printaddrs = 0;
	currbp->printptrs = 0;
	currbp->bpmods = 0;
	currbp->next = NULL;

	return currbp;
}


/* copybp copies a breakpoint */
void copybp(struct breakpoint *dst, struct breakpoint *src)
{
	if (!dst || !src) return;
	
	if (src->fnname) {
		dst->fnname = strndup(src->fnname, S_SYMNAME);
	} else {
		dst->fnname = NULL;
	}
	dst->address = src->address;
	dst->thumb = src->thumb;
	dst->count = src->count;
	dst->defcount = src->defcount;
	dst->disabled = src->disabled;
	dst->kill = src->kill;
	dst->copyonfork = src->copyonfork;
	dst->copyonthread = src->copyonthread;
	dst->trapint = src->trapint;
	dst->printregs = copyregs(src->printregs);
	dst->printaddrs = copyaddrs(src->printaddrs);
	dst->printptrs = copyptrs(src->printptrs);
	dst->bpmods = copybpmods(src->bpmods);
}


/* copybps copies a list of breakpoints */
struct breakpoint *copybps(struct breakpoint *head)
{
	struct breakpoint *newhead = NULL;
	struct breakpoint *new = NULL;
	struct breakpoint *curr = head;
	
	if (head == NULL) {
		return NULL;
	}
	
	while (curr) {
		new = addbp(&newhead);
		if (new == NULL) {
			return NULL;
		}
		
		copybp(new, curr);
		curr = curr->next;
	}
	
	return newhead;
}


/* deletebps frees the memory associated with breakpoint list */
void deletebps(struct breakpoint *breakpoints)
{
	struct breakpoint *bps = breakpoints;
	struct breakpoint *bptemp = NULL;
	struct reglist *regs = NULL;
	struct addrlist *addrs = NULL;
	struct ptrlist *ptrs = NULL;
	struct bpmodlist *bpmods = NULL;
	void *tempnext = NULL;
	
	while (bps) {
		bptemp = bps->next;
		bps->next = NULL;

		/* first free the lists */
		regs = bps->printregs;
		while (regs) {
			tempnext = (void *)regs->next;
			regs->next = NULL;
			if (regs->reg) {
				free(regs->reg);
			}
			free(regs);
			regs = (struct reglist *)tempnext;
		}
		bps->printregs = NULL;
		
		addrs = bps->printaddrs;
		while (addrs) {
			tempnext = (void *)addrs->next;
			addrs->next = NULL;
			free(addrs);
			addrs = (struct addrlist *)tempnext;
		}
		bps->printaddrs = NULL;
		
		ptrs = bps->printptrs;
		while (ptrs) {
			tempnext = (void *)ptrs->next;
			ptrs->next = NULL;
			if (ptrs->reg) {
				free(ptrs->reg);
			}
			free(ptrs);
			ptrs = (struct ptrlist *)tempnext;
		}
		bps->printptrs = NULL;
		
		bpmods = bps->bpmods;
		while (bpmods) {
			tempnext = (void *)bpmods->next;
			bpmods->next = NULL;
			free(bpmods);
			bpmods = (struct bpmodlist *)tempnext;
		}
		bps->bpmods = NULL;
		
		/* then free the breakpoint */
		free(bps);
		/* and iterate */
		bps = bptemp;
	}
}


/* addreg adds a new register */
struct reglist *addreg(struct reglist **head)
{
	struct reglist *currreg = NULL;

	/* if head is NULL then this is an error */
	if (head == NULL) return NULL;

	if (*head == NULL) {
		/* if *head is NULL then the list is empty */

		*head = (struct reglist *)malloc(sizeof(struct reglist));
		currreg = *head;
	} else if ((*head)->next != NULL) {
		/* walk to the last node */

		return addreg(&((*head)->next));
	} else {
		(*head)->next = (struct reglist *)malloc(sizeof(struct reglist));
		currreg = (*head)->next;
	}

	if (currreg == NULL) {
		myprintf(0, "addreg malloc error\n");
		exit(E_MALLOC);
	}

	currreg->reg = NULL;
	currreg->next = NULL;

	return currreg;
}


/* copyreg copies a register node */
void copyreg(struct reglist *dst, struct reglist *src)
{
	if (!dst || !src) return;
	
	if (src->reg) {
		dst->reg = strndup(src->reg, S_REG);
	} else {
		dst->reg = NULL;
	}
}


/* copyregs copies a list of registers */
struct reglist *copyregs(struct reglist *head)
{
	struct reglist *newhead = NULL;
	struct reglist *new = NULL;
	struct reglist *curr = head;
	
	if (head == NULL) {
		return NULL;
	}
	
	while (curr) {
		new = addreg(&newhead);
		if (new == NULL) {
			return NULL;
		}
		
		copyreg(new, curr);
		curr = curr->next;
	}
	
	return newhead;
}


/* addaddr adds a new address */
struct addrlist *addaddr(struct addrlist **head)
{
	struct addrlist *curraddr = NULL;

	/* if head is NULL then this is an error */
	if (head == NULL) return NULL;

	if (*head == NULL) {
		/* if *head is NULL then the list is empty */

		*head = (struct addrlist *)malloc(sizeof(struct addrlist));
		curraddr = *head;
	} else if ((*head)->next != NULL) {
		/* walk to the last node */

		return addaddr(&((*head)->next));
	} else {
		(*head)->next = (struct addrlist *)malloc(sizeof(struct addrlist));
		curraddr = (*head)->next;
	}

	if (curraddr == NULL) {
		myprintf(0, "addaddr malloc error\n");
		exit(E_MALLOC);
	}

	curraddr->address = 0;
	curraddr->format = 0;
	curraddr->size = 0;
	curraddr->offset = 0;
	curraddr->deref = 0;
	curraddr->next = NULL;

	return curraddr;
}


/* copyaddr copies an address node */
void copyaddr(struct addrlist *dst, struct addrlist *src)
{
	if (!dst || !src) return;

	dst->address = src->address;
	dst->format = src->format;
	dst->size = src->size;
	dst->offset = src->offset;
	dst->deref = src->deref;
}


/* copyaddrs copies a list of addresses */
struct addrlist *copyaddrs(struct addrlist *head)
{
	struct addrlist *newhead = NULL;
	struct addrlist *new = NULL;
	struct addrlist *curr = head;
	
	if (head == NULL) {
		return NULL;
	}
	
	while (curr) {
		new = addaddr(&newhead);
		if (new == NULL) {
			return NULL;
		}
		
		copyaddr(new, curr);
		curr = curr->next;
	}
	
	return newhead;
}


/* addptr adds a new register pointer */
struct ptrlist *addptr(struct ptrlist **head)
{
	struct ptrlist *currptr = NULL;

	/* if head is NULL then this is an error */
	if (head == NULL) return NULL;

	if (*head == NULL) {
		/* if *head is NULL then the list is empty */

		*head = (struct ptrlist *)malloc(sizeof(struct ptrlist));
		currptr = *head;
	} else if ((*head)->next != NULL) {
		/* walk to the last node */

		return addptr(&((*head)->next));
	} else {
		(*head)->next = (struct ptrlist *)malloc(sizeof(struct ptrlist));
		currptr = (*head)->next;
	}

	if (currptr == NULL) {
		myprintf(0, "addptr malloc error\n");
		exit(E_MALLOC);
	}

	currptr->reg = NULL;
	currptr->format = 0;
	currptr->size = 0;
	currptr->offset = 0;
	currptr->deref = 0;
	currptr->next = NULL;

	return currptr;
}


/* copyptr copies a register pointer node */
void copyptr(struct ptrlist *dst, struct ptrlist *src)
{
	if (!dst || !src) return;

	if (src->reg) {
		dst->reg = strndup(src->reg, S_REG);
	} else {
		dst->reg = NULL;
	}
	dst->format = src->format;
	dst->size = src->size;
	dst->offset = src->offset;
	dst->deref = src->deref;
}


/* copyptrs copies a list of register pointers */
struct ptrlist *copyptrs(struct ptrlist *head)
{
	struct ptrlist *newhead = NULL;
	struct ptrlist *new = NULL;
	struct ptrlist *curr = head;
	
	if (head == NULL) {
		return NULL;
	}
	
	while (curr) {
		new = addptr(&newhead);
		if (new == NULL) {
			return NULL;
		}
		
		copyptr(new, curr);
		curr = curr->next;
	}
	
	return newhead;
}


/* addbpmod adds a new breakpoint modification detail */
struct bpmodlist *addbpmod(struct bpmodlist **head)
{
	struct bpmodlist *currbpmod = NULL;

	/* if head is NULL then this is an error */
	if (head == NULL) return NULL;

	if (*head == NULL) {
		/* if *head is NULL then the list is empty */

		*head = (struct bpmodlist *)malloc(sizeof(struct bpmodlist));
		currbpmod = *head;
	} else if ((*head)->next != NULL) {
		/* walk to the last node */

		return addbpmod(&((*head)->next));
	} else {
		(*head)->next = (struct bpmodlist *)malloc(sizeof(struct bpmodlist));
		currbpmod = (*head)->next;
	}

	if (currbpmod == NULL) {
		myprintf(0, "addbpmod malloc error\n");
		exit(E_MALLOC);
	}

	currbpmod->bpnum = 0;
	currbpmod->action = BPACTION_NONE;
	currbpmod->global = 0;
	currbpmod->next = NULL;

	return currbpmod;
}


/* copybpmod copies a breakpoint modifier */
void copybpmod(struct bpmodlist *dst, struct bpmodlist *src)
{
	if (!dst || !src) return;

	dst->bpnum = src->bpnum;
	dst->action = src->action;
	dst->global = src->global;
}


/* copybpmods copies a list of breakpoint modifiers */
struct bpmodlist *copybpmods(struct bpmodlist *head)
{
	struct bpmodlist *newhead = NULL;
	struct bpmodlist *new = NULL;
	struct bpmodlist *curr = head;
	
	if (head == NULL) {
		return NULL;
	}
	
	while (curr) {
		new = addbpmod(&newhead);
		if (new == NULL) {
			return NULL;
		}
		
		copybpmod(new, curr);
		curr = curr->next;
	}
	
	return newhead;
}


/* extendbpmods extends one bpmodlist with another */
void extendbpmods(struct bpmodlist **dst, struct bpmodlist *src)
{
	if (!dst || !src) return;
	
	if (!(*dst)) {
		/* dst list is empty */
		*dst = copybpmods(src);
	} else {
		extendbpmods(&((*dst)->next), src);
	}
}


/* deletebpmods deletes a list of bpmods */
void deletebpmods(struct bpmodlist *head)
{
	struct bpmodlist *curr = NULL;
	
	if (!head) return;
	
	/* unlink head */
	curr = head->next;
	head->next = NULL;
	
	/* free head */
	free(head);
	
	/* free tail */
	deletebpmods(curr);
}


/* addprocess adds a new process to the process list */
struct process *addprocess(struct process **head)
{
	struct process *currprocess = NULL;

	/* if head is NULL then this is an error */
	if (head == NULL) return NULL;

	if (*head == NULL) {
		/* if *head is NULL then the list is empty */

		*head = (struct process *)malloc(sizeof(struct process));
		currprocess = *head;
	} else if ((*head)->next != NULL) {
		/* walk to the last node */

		return addprocess(&((*head)->next));
	} else {
		(*head)->next = (struct process *)malloc(sizeof(struct process));
		currprocess = (*head)->next;
	}

	if (currprocess == NULL) {
		myprintf(0, "addprocess malloc error\n");
		exit(E_MALLOC);
	}

	currprocess->pid = 0;
	currprocess->bphead = NULL;
	currprocess->state = PROCESS_NEW;
	currprocess->preinit = 1;
	currprocess->copyonfork = 0;
	currprocess->copyonthread = 0;
	currprocess->thread = 0;
	currprocess->ssaddr1 = 0;
	currprocess->ssaddr2 = 0;
	currprocess->sstrap1 = 0;
	currprocess->sstrap2 = 0;
	currprocess->modbps = 0;
	currprocess->ssbp = NULL;
	currprocess->bpmods = NULL;
	currprocess->next = NULL;

	return currprocess;
	
}


/* getprocess returns the process with the matching pid */
struct process *getprocess(struct process *head, pid_t pid)
{
	/* if head is NULL then this is an error (or we didn't find the pid) */
	if (head == NULL) return NULL;
	
	if (head->pid == pid) {
		return head;
	} else {
		return getprocess(head->next, pid);
	}
}


/* copyprocess makes a copy of an existing process */
struct process *copyprocess(struct process **head, pid_t pid)
{
	struct process *currprocess = NULL;
	struct process *newprocess = NULL;
	
	if (!head || !*head) return NULL;
	
	currprocess = getprocess(*head, pid);
	if (currprocess == NULL) return NULL;
	
	newprocess = addprocess(head);
	if (newprocess == NULL) return NULL;
	
	newprocess->pid = 0;
	newprocess->bphead = copybps(currprocess->bphead);
	newprocess->state = PROCESS_NEW;
	newprocess->preinit = currprocess->preinit;
	newprocess->copyonfork = currprocess->copyonfork;
	newprocess->copyonthread = currprocess->copyonthread;
	newprocess->thread = currprocess->thread;
	newprocess->ssaddr1 = currprocess->ssaddr1;
	newprocess->ssaddr2 = currprocess->ssaddr2;
	newprocess->sstrap1 = currprocess->sstrap1;
	newprocess->sstrap2 = currprocess->sstrap2;
	newprocess->modbps = 0;
	newprocess->ssbp = currprocess->ssbp;
	newprocess->bpmods = copybpmods(currprocess->bpmods);

	return newprocess;
}


/* deleteprocess removes a process from the list */
void deleteprocess(struct process **head, pid_t pid)
{
	void *tempnext = NULL;
	struct bpmodlist *bpmods = NULL;
	struct process *curr = NULL;
	struct process *target = NULL;
	
	if ((head == NULL) || (*head == NULL)) return;

	/* find process and unlink it */
	
	if ((*head)->pid == pid) {
		/* head is match, unlink it */
		target = *head;
		*head = (*head)->next;
	} else {
		/* walk the list looking for match */
		curr = *head;
		while ((curr) && (curr->next) && (curr->next->pid != pid)) {
			curr = curr->next;
		}
		if ((curr) && (curr->next) && (curr->next->pid == pid)) {
			/* curr->next is match, unlink it */
			target = curr->next;
			curr->next = curr->next->next;
		} else {
			target = NULL;
		}
	}
	
	/* if we found the process */
	if (target) {
		/* walk the breakpoints list, freeing as we go */
		deletebps(target->bphead);
		target->bphead = NULL;

		/* free the process bpmods list */
		bpmods = target->bpmods;
		while (bpmods) {
			tempnext = (void *)bpmods->next;
			bpmods->next = NULL;
			free(bpmods);
			bpmods = (struct bpmodlist *)tempnext;
		}
		target->bpmods = NULL;

		/* free the process */
		free(target);
	}
	
}

