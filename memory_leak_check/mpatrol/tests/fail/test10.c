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
 * Allocates a block of 16 bytes and then attempts to copy the contents of
 * the block to a local character array.  However, the source of the copy
 * also spans 1 byte before and 1 byte after the allocated block, resulting
 * in an error in the log file.
 */


#include "mpatrol.h"


int main(void)
{
    char b[16];
    char *p;

    if (p = (char *) malloc(16))
    {
        memcpy(b, p - 1, 18);
        free(p);
    }
    return EXIT_SUCCESS;
}
