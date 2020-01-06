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
 * Allocates a single byte of memory and then attempts to write a word
 * to the allocation, resulting in some unallocated bytes being written.
 * This can sometimes be detected with PAGEALLOC=UPPER but can always be
 * detected with OFLOWWATCH or by using the -fcheck-memory-usage option
 * of gcc.  The error will eventually be picked up if you are using
 * overflow buffers, but not immediately.
 */


#include "mpatrol.h"


int main(void)
{
    int *p;

    if (p = (int *) malloc(1))
    {
        p[0] = 0;
        free(p);
    }
    return EXIT_SUCCESS;
}
