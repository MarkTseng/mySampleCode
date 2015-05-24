/* **************** LPD:1.0 s_03/lab1_hello_main.c **************** */
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
/* LAB: Compiling with headers
 *
 * Using the previous hello program as a starting point, put some
 * functions in a separate source file. Also use a header file.
 * Compile and execute it.
 *
 @*/

/*  Compile with: gcc -Wall -pedantic -o hello_main hello_main.c hello_sub.c
 *
 * (If you use #include <hello.h>, you need the -I. option.)
 */

#include "lab1_hello.h"

int main ()
{
    hello_sub ();
    exit (0);
}
