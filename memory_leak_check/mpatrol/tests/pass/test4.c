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
 * Demonstrates and tests the facility for obtaining information about
 * the allocation a specific address belongs to.
 */


#include "mpatrol.h"
#include <stdio.h>


void display(void *p)
{
    __mp_allocstack *s;
    __mp_allocinfo d;
    __mp_symbolinfo i;

    if (!__mp_info(p, &d) || !d.allocated)
    {
        fprintf(stderr, "nothing known about address 0x%0*lX\n",
                sizeof(void *) * 2, p);
        return;
    }
    fprintf(stderr, "block:    0x%0*lX\n", sizeof(void *) * 2, d.block);
    fprintf(stderr, "size:     %lu\n", d.size);
    fprintf(stderr, "type:     %s\n", __mp_function(d.type));
    fprintf(stderr, "alloc:    %lu\n", d.alloc);
    fprintf(stderr, "realloc:  %lu\n", d.realloc);
    fprintf(stderr, "thread:   %lu\n", d.thread);
    fprintf(stderr, "event:    %lu\n", d.event);
    fprintf(stderr, "func:     %s\n", d.func ? d.func : "<unknown>");
    fprintf(stderr, "file:     %s\n", d.file ? d.file : "<unknown>");
    fprintf(stderr, "line:     %lu\n", d.line);
    for (s = d.stack; s != NULL; s = s->next)
    {
        fprintf(stderr, "\t0x%0*lX", sizeof(void *) * 2, s->addr);
        if (__mp_syminfo(s->addr, &i))
        {
            if (i.name != NULL)
                fprintf(stderr, " %s", i.name);
            if ((i.addr != NULL) && (i.addr != s->addr))
                fprintf(stderr, "%+ld",
                        (char *) s->addr - (char *) i.addr);
            if (i.object != NULL)
                fprintf(stderr, " [%s]", i.object);
        }
        else if (s->name != NULL)
            fprintf(stderr, " %s", s->name);
        fputc('\n', stderr);
    }
    fprintf(stderr, "typestr:  %s\n",
            d.typestr ? d.typestr : "<unknown>");
    fprintf(stderr, "typesize: %lu\n", d.typesize);
    fprintf(stderr, "userdata: 0x%0*lX\n", sizeof(void *) * 2, d.userdata);
    fputs("flags:   ", stderr);
    if (!d.freed && !d.marked && !d.profiled && !d.traced && !d.internal)
        fputs(" none\n", stderr);
    else
    {
        if (d.freed)
            fputs(" freed", stderr);
        if (d.marked)
            fputs(" marked", stderr);
        if (d.profiled)
            fputs(" profiled", stderr);
        if (d.traced)
            fputs(" traced", stderr);
        if (d.internal)
            fputs(" internal", stderr);
        fputc('\n', stderr);
    }
}


void func2(void)
{
    void *p;

    if (p = malloc(16))
    {
        display(p);
        free(p);
    }
    display(p);
}


void func1(void)
{
    func2();
}


int main(void)
{
    func1();
    return EXIT_SUCCESS;
}
