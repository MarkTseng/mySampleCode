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
 * Allocates 16 floats and then resizes the allocation to 8 floats and
 * frees them.  Then allocates 16 integers and resizes the allocation
 * to 32 integers before freeing them.  Finally, duplicates a string
 * and then frees it.
 */


#include "mpatrol.h"


int main(void)
{
    float *f;
    int *i;
    char *s;

    MP_MALLOC(f, 16, float);
    MP_REALLOC(f, 8, float);
    MP_FREE(f);
    MP_CALLOC(i, 16, int);
    MP_REALLOC(i, 32, int);
    MP_FREE(i);
    MP_STRDUP(s, "test");
    MP_FREE(s);
    return EXIT_SUCCESS;
}
