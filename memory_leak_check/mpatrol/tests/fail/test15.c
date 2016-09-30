/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
