/* **************** LPD:1.0 s_04/lab1_main.c **************** */
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
/* Librairies
 *
 *
 * Use the simple "hello world" program from the previous lesson.  Make
 * a static library out of the functions, and compile and link to it.
 *
 * Do the same with a shared library.
 *
 * See "nomake.sh" for compilation instructions
 *
 */

#include <stdio.h>
#include <stdlib.h>

void hello_sub ();

int main ()
{
    hello_sub ();
    exit (0);
}
