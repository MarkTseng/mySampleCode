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
 * Duplicates a string using alloca() and then returns the address
 * of the allocation.  This is illegal since the memory allocated
 * by alloca() will be freed when the function returns.  The call
 * to memcpy() will then corrupt free memory and the call to free()
 * will attempt to free an invalid pointer.
 */


#include "mpatrol.h"
#include <stdio.h>


char *f(size_t l)
{
    return (char *) alloca(l);
}


char *g(char *s)
{
    char *t;
    size_t l;

    l = strlen(s) + 1;
    if (t = f(l))
        memcpy(t, s, l);
    return t;
}


int main(void)
{
    char *s;

    s = g("test");
    free(s);
    return EXIT_SUCCESS;
}
