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
 * Allocates a block of 16 bytes and stores both a pointer to the
 * block and the size of the block in a union.  This test would work
 * if it used a structure instead of a union.
 */


#include "mpatrol.h"


typedef union block
{
    void *ptr;
    unsigned long size;
}
block;


int main(void)
{
    block b;

    if (b.ptr = malloc(16))
    {
        b.size = 16;
        free(b.ptr);
    }
    return EXIT_SUCCESS;
}
