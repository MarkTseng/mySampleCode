/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


/* mod2.c */

#include <stdio.h>
#include <unistd.h>
int test2[100000] = { 1, 2, 3 };

#ifndef VERSION
#define VERSION ""
#endif

void
x2(void) {
    printf("Called mod2-x2 " VERSION "\n");
}
