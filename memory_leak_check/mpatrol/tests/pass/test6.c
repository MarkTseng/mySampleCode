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
 * Allocates a block of 131072 bytes and partitions the first 65536 bytes
 * into 256 blocks of 256 bytes, each of which is filled with a different
 * byte.  This pattern is also copied identically to the second block of
 * 65536 bytes and the two halves are then compared.
 */


#include "mpatrol.h"
#include <stdio.h>


int main(void)
{
    char *p, *t;
    size_t i;

    if (p = (char *) malloc(131072))
    {
        for (i = 0, t = p; i < 256; i++, t += 256)
        {
            memset(t, i, 256);
            memcpy(t + 65536, t, 256);
        }
        if (memcmp(p, t, 65536))
            puts("test failed");
        else
            puts("test passed");
        free(p);
    }
    return EXIT_SUCCESS;
}
