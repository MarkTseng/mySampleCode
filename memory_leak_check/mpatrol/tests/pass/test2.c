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
 * Demonstrates and tests the facility for specifying user-defined prologue
 * and epilogue functions.
 */


#include "mpatrol.h"
#include <stdio.h>


__mp_prologuehandler old_prologue;
__mp_epiloguehandler old_epilogue;


void prologue(MP_CONST void *p, size_t l, size_t m, MP_CONST char *s,
              MP_CONST char *t, unsigned long u, MP_CONST void *a)
{
    if (old_prologue != NULL)
        old_prologue(p, l, m, s, t, u, a);
    if (p == (void *) -1)
        fprintf(stderr, "allocating %lu bytes\n", l);
    else if (l == (size_t) -1)
        fprintf(stderr, "freeing allocation 0x%0*lX\n", sizeof(void *) * 2, p);
    else if (l == (size_t) -2)
        fprintf(stderr, "duplicating string `%s'\n", p);
    else
        fprintf(stderr, "reallocating allocation 0x%0*lX to %lu bytes\n",
                sizeof(void *) * 2, p, l);
}


void epilogue(MP_CONST void *p, MP_CONST char *s, MP_CONST char *t,
              unsigned long u, MP_CONST void *a)
{
    if (p != (void *) -1)
        fprintf(stderr, "allocation returns 0x%0*lX\n", sizeof(void *) * 2, p);
    if (old_epilogue != NULL)
        old_epilogue(p, s, t, u, a);
}


int main(void)
{
    void *p, *q;

    old_prologue = __mp_prologue(prologue);
    old_epilogue = __mp_epilogue(epilogue);
    if (p = malloc(16))
        if (q = realloc(p, 32))
            free(q);
        else
            free(p);
    if (p = (char *) strdup("test"))
        free(p);
    __mp_prologue(old_prologue);
    __mp_epilogue(old_epilogue);
    return EXIT_SUCCESS;
}
