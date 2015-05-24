/* **************** LPD:1.0 s_03/lab2_dll_lib.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2009
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/*
 * Dyanmical Loading (library)
 *
 * Compile with:
 *
 * gcc -O2 -Wall -pedantic -fpic -shared -o libhello_dl.so lab2_dll_lib.c
 *
 * Make a simple shared library (libhello_dl.so) that
 * has a "hello world" print function, and one or more constructor and
 * destructor routines.
 *
 * Make a test program that dynamically loads the library and executes
 * the function in it.
 @*/

#include <stdio.h>
void hello_dl ()
{
    printf ("\n%s\n\n", "hello world from a dynamic library\n");
}

void my_init1 (void) __attribute__ ((constructor));
void my_init2 (void) __attribute__ ((constructor));
void my_init1 (void)
{
    fprintf (stderr, "I entered the first constructor function \n");
}
void my_init2 (void)
{
    fprintf (stderr, "I entered the second constructor function \n");
}

void my_exit1 (void) __attribute__ ((destructor));
void my_exit2 (void) __attribute__ ((destructor));
void my_exit1 (void)
{
    fprintf (stderr, "I entered the first destructor function \n");
}
void my_exit2 (void)
{
    fprintf (stderr, "I entered the second destructor function \n");
}
