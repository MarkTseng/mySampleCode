/* bps - Breakpoint Shenanigans - K Sheldrake
** bps.h - header file for bps.c
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

#if !defined _BPS_H
#define _BPS_H 1


#include "bps_error.h"
#include "bps_util.h"
#include "bps_data.h"
#include "bps_cmdline.h"
#include "bps_ptrace.h"
#include "bps_elf.h"

int main(int argc, char *argv[], char *envp[]);


#endif

