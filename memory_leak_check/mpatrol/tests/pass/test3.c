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
 * Demonstrates and tests the facility for specifying a user-defined
 * low memory handler.
 */


#include "mpatrol.h"
#include <stdio.h>


void *buffer = NULL;
int freed = 0;


void handler(MP_CONST char *s, MP_CONST char *t, unsigned long u,
             MP_CONST void *a)
{
    fputs("low memory handler called", stderr);
    if (s != NULL)
        fprintf(stderr, " from %s", s);
    if ((t != NULL) && (u != 0))
        fprintf(stderr, " at %s line %lu", t, u);
    fputc('\n', stderr);
    if (buffer == NULL)
    {
        fputs("no buffer to free\n", stderr);
        abort();
    }
    free(buffer);
    fputs("successfully freed buffer\n", stderr);
    buffer = NULL;
    freed = 1;
}


int main(void)
{
    void *p;
    int r;

    r = EXIT_SUCCESS;
    __mp_setoption(MP_OPT_LIMIT, 1572864);
    __mp_nomemory(handler);
    buffer = malloc(1048576);
    p = malloc(1048576);
    if ((p != NULL) && freed)
        fputs("test passed\n", stderr);
    else
    {
        fputs("test failed\n", stderr);
        r = EXIT_FAILURE;
    }
    if (p != NULL)
        free(p);
    if (buffer != NULL)
        free(buffer);
    return r;
}
