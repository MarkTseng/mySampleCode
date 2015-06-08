/* **************** LPD:1.0 s_13/lab5_decon.c **************** */
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
 * Exiting (deconstructor/demonstration)
 *
 * Have a program use atexit() (or on_exit()), or the
 * constructor/destructor methods for exit handling..
 *
 * Register at least two handlers to be processed during exiting.
 *
 * Can you make what happens depend on the exit code from your main
 * routine?
 @*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void exit_fun1 (void) __attribute__ ((destructor));
void exit_fun2 (void) __attribute__ ((destructor));

void exit_fun1 (void)
{
    printf ("I am in exit_fun1\n");
}

void exit_fun2 (void)
{
    printf ("I am in exit_fun2\n");
}

int main (int argc, char *argv[])
{
    printf ("About to call exit from %s program .... \n", argv[0]);
    exit (0);
}
