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
 * Allocates 256 blocks of varying sizes and then reallocates and frees
 * them in a different order from which they were allocated.
 */


#include "mpatrol.h"


int main(void)
{
    void *a[256];
    size_t i, n;

    for (i = 0; i < 256; i++)
    {
        n = (size_t) ((rand() * 256.0) / (RAND_MAX + 1.0)) + 1;
        a[i] = malloc(n);
    }
    for (i = 256; i > 0; i--)
    {
        n = (size_t) ((rand() * 256.0) / (RAND_MAX + 1.0)) + 1;
        a[i - 1] = realloc(a[i - 1], n);
    }
    for (i = 0; i < 256; i += 2)
        free(a[i]);
    for (i = 256; i > 0; i -= 2)
        free(a[i - 1]);
    return EXIT_SUCCESS;
}
