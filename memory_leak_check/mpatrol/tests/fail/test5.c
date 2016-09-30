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
 * Allocates a block of 16 bytes and then copies a string of 16 bytes
 * into the block.  However, the string is copied to 1 byte before the
 * allocated block which writes before the start of the block.  This test
 * must be run with an OFLOWSIZE greater than zero.
 */


#include "mpatrol.h"


int main(void)
{
    char *p;

    if (p = (char *) malloc(16))
    {
        strcpy(p - 1, "this test fails!");
        free(p);
    }
    return EXIT_SUCCESS;
}
