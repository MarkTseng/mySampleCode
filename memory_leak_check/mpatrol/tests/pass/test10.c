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
 * Demonstrates and tests the facility for obtaining information on
 * local memory leaks.  Will also edit or list the location of each
 * leak if the EDIT or LIST option is in effect.
 */


#include "mpatrol.h"
#include <stdio.h>


int callback(MP_CONST void *p, void *t)
{
    __mp_allocstack *s;
    __mp_allocinfo d;

    if (!__mp_info(p, &d) || !d.allocated)
    {
        fprintf(stderr, "nothing known about address 0x%0*lX\n",
                sizeof(void *) * 2, p);
        return -1;
    }
    if (!d.freed)
    {
        fprintf(stderr, "0x%0*lX", sizeof(void *) * 2, d.block);
        fprintf(stderr, " %s", d.func ? d.func : "<unknown>");
        fprintf(stderr, " %s", d.file ? d.file : "<unknown>");
        fprintf(stderr, " %lu", d.line);
        for (s = d.stack; s != NULL; s = s->next)
        {
            if (s == d.stack)
                fputs(" (", stderr);
            else
                fputs("->", stderr);
            if (s->name != NULL)
                fprintf(stderr, "%s", s->name);
            else
                fprintf(stderr, "0x%0*lX", sizeof(void *) * 2, s->addr);
            if (s->next == NULL)
                fputc(')', stderr);
        }
        fputc('\n', stderr);
        if ((d.file != NULL) && (d.line != 0))
            __mp_view(d.file, d.line);
        *((unsigned long *) t) = *((unsigned long *) t) + d.size;
        return 1;
    }
    return 0;
}


void func2(unsigned long n)
{
    void *p;

    p = malloc((n * 10) + 1);
    if (n % 13)
        free(p);
}


void func1(void)
{
    void *p;
    size_t i, n;
    unsigned long s, t;

    p = malloc(16);
    s = __mp_snapshot();
    for (i = 0; i < 128; i++)
        func2(i);
    free(p);
    t = 0;
    if (n = __mp_iterate(callback, &t, s))
        fprintf(stderr, "Detected %lu memory leaks (%lu bytes)\n", n, t);
    if ((n != 10) || (t != 5860))
        fputs("Expected 10 memory leaks (5860 bytes)\n", stderr);
}


int main(void)
{
    void *p;

    p = malloc(16);
    func1();
    free(p);
    return EXIT_SUCCESS;
}
