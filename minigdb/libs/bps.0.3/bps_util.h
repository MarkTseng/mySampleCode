/* bps - Breakpoint Shenanigans - K Sheldrake
** bps_util.h - utility functions for bps
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

#if !defined _BPS_UTIL_H
#define _BPS_UTIL_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include "bps_error.h"

#define HEX_PER_ROW 16
#define S_MYPRINTF 256


/* sad but true; verbosity is global */
extern int verbose;

/* output is the output stream for messages */
extern int output;


void usage(char *msg);


/* generic support functions */
unsigned char printablechar(unsigned char c);
void hexdump(unsigned char *data, unsigned char format, int data_len);
void shexdump(unsigned char *data, int data_len);
int myprintf(unsigned int vlevel, const char *fmt, ... );
int openpipe(char *pipename);


#endif

