/* **************** LPD:1.0 s_02/lab1_hello.c **************** */
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
/* LAB: Compiling
 *
 * Make a simple hello world program.  Compile it and execute it.  The
 * purpose of this exercise is just to make sure your compilation
 * environment is working.
 *
 @*/

/* Compile with:  gcc -O2 -Wall -pedantic -o lab1_hello  lab1_hello.c */

#include <stdio.h>
#include <stdlib.h>

int main ()
{
    printf ("hello world\n");
    exit (0);
}
