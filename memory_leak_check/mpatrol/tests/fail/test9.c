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
 * Allocates a block of 16 bytes and then attempts to zero the contents of
 * the block.  However, a zero byte is also written 1 byte before and 1
 * byte after the allocated block, resulting in an error in the log file.
 */


#include "mpatrol.h"


int main(void)
{
    char *p;

    if (p = (char *) malloc(16))
    {
        memset(p - 1, 0, 18);
        free(p);
    }
    return EXIT_SUCCESS;
}
