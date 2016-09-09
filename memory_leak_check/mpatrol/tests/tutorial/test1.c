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
 * Reads the standard input file stream, converts all lowercase
 * characters to uppercase, and displays all non-empty lines to the
 * standard output file stream.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char *strtoupper(char *s)
{
    char *t;
    size_t i, l;

    l = strlen(s);
    t = (char *) malloc(l);
    for (i = 0; i < l; i++)
        t[i] = toupper(s[i]);
    t[i] = '\0';
    return t;
}


int main(void)
{
    char *b, *s;

    b = (char *) malloc(BUFSIZ);
    while (gets(b))
    {
        s = strtoupper(b);
        if (*s != '\0')
        {
            puts(s);
            free(s);
        }
    }
    free(b);
    return EXIT_SUCCESS;
}
