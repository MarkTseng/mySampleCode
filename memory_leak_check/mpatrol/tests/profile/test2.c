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
 * Associates an integer value with its negative string equivalent in a
 * structure, and then allocates 256 such pairs randomly, displays them
 * then frees them.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct pair
{
    int value;
    char *string;
}
pair;


pair *new_pair(int n)
{
    static char s[16];
    pair *p;

    if ((p = (pair *) malloc(sizeof(pair))) == NULL)
    {
        fputs("Out of memory\n", stderr);
        exit(EXIT_FAILURE);
    }
    p->value = n;
    sprintf(s, "%d", -n);
    if ((p->string = strdup(s)) == NULL)
    {
        fputs("Out of memory\n", stderr);
        exit(EXIT_FAILURE);
    }
    return p;
}


int main(void)
{
    pair *a[256];
    int i, n;

    for (i = 0; i < 256; i++)
    {
        n = (int) ((rand() * 256.0) / (RAND_MAX + 1.0)) - 128;
        a[i] = new_pair(n);
    }
    for (i = 0; i < 256; i++)
        printf("%3d: %4d -> \"%s\"\n", i, a[i]->value, a[i]->string);
    for (i = 0; i < 256; i++)
    {
        free(a[i]->string);
        free(a[i]);
    }
    return EXIT_SUCCESS;
}
