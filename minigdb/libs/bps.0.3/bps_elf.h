/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_elf.h - elf functions for bps
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

#if !defined _BPS_ELF_H
#define _BPS_ELF_H 1

#include <elf.h>
#include <libelf.h>
#include <gelf.h>
#include <sys/stat.h>
#include "bps_error.h"
#include "bps_util.h"

#define S_SYMNAME 128

/* struct to hold symbols */
struct symbol
{
	char name[S_SYMNAME];
	unsigned long address;
};


/* elf functions */
int readsyms(struct symbol **symbols, char *exe, int display);
void display_symbols(struct symbol *symbols, int total);
unsigned long symaddr(struct symbol *symbols, int total, char *name);


#endif

