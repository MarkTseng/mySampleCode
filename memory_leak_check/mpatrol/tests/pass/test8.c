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
 * Tests alloca() and related functions via nested function calls.
 * The final output should be a horizontal pyramid of plus signs
 * followed by a horizontal pyramid of minus signs.
 */


#include "mpatrol.h"
#include <stdio.h>


char *f1(char *s)
{
    char *t;
    size_t l;

    l = strlen(s) + 1;
    if ((t = (char *) alloca(l + 1)) == NULL)
        return NULL;
    memcpy(t, s, l);
    t[l - 1] = t[l - 2];
    t[l] = '\0';
    return strdup(t);
}


char *f2(char *s)
{
    char *t;
    size_t l;

    l = strlen(s) - 1;
    if ((t = (char *) alloca(l + 1)) == NULL)
        return NULL;
    memcpy(t, s, l + 1);
    t[l] = '\0';
    return strdup(t);
}


int f(char *s, size_t l)
{
    char *t;
    size_t i;

    puts(s);
    for (i = 0; i < l; i++)
    {
        if (((t = f1(s)) == NULL) ||
            ((s = (char *) alloca(strlen(t) + 1)) == NULL))
            return 0;
        strcpy(s, t);
        free(t);
        puts(s);
    }
    for (i = 0; i < l; i++)
    {
        if (((t = f2(s)) == NULL) ||
            ((s = (char *) alloca(strlen(t) + 1)) == NULL))
            return 0;
        strcpy(s, t);
        free(t);
        puts(s);
    }
    return 1;
}


int main(void)
{
    char *s;

    s = strdupa("+");
    if (!f(s, 4))
        exit(EXIT_FAILURE);
    dealloca(s);
    s = strdupa("-");
    if (!f(s, 4))
        exit(EXIT_FAILURE);
    dealloca(s);
    return EXIT_SUCCESS;
}
