/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2002 Graeme S. Roy <graeme.roy@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/*
 * Allocates a block of 8 bytes, fills these bytes with spaces and then
 * writes the word "test" into the first four bytes before printing out
 * the resulting string.  However, the string is not null-terminated and
 * the call to printf() will result in bytes being read past the end of
 * the allocation.  This test must be run on a system with memory
 * protection and the failure will be detected if PAGEALLOC=UPPER is used.
 */


#include "mpatrol.h"
#include <stdio.h>


int main(void)
{
    char *p;

    if (p = (char *) malloc(8))
    {
        memset(p, ' ', 8);
        p[0] = 't';
        p[1] = 'e';
        p[2] = 's';
        p[3] = 't';
        printf("%s\n", p);
        free(p);
    }
    return EXIT_SUCCESS;
}
