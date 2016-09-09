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
 * Ensures that source level information can be determined for C++ operator
 * new and operator delete even when called via compiler-generated constructors
 * and destructors.  This is a C++-only test.
 */


#include "mpatrol.h"
#include <stdio.h>


class A
{
  public:
    A() : p(NULL), v(0)
    {
        p = new char;
        p[0] = '\0';
    }

    A(char *s) : p(NULL), v(1)
    {
        p = new char[strlen(s) + 1];
        strcpy(p, s);
    }

    ~A()
    {
        if (v == 0)
            delete p;
        else
            delete[] p;
    }

  private:
    char *p;
    int v;
};


A a;
A b("test1");


int main(void)
{
    A c;
    A d("test2");
    A *e = new A;
    A *f = new A("test3");
    char *g = new char;
    char *h = new char[8];
    delete[] h;
    delete g;
    delete f;
    delete e;
    return EXIT_SUCCESS;
}
