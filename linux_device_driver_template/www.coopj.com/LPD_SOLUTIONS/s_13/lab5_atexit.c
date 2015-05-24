/* **************** LPD:1.0 s_13/lab5_atexit.c **************** */
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
 * Exiting (atexit demonstration)
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

#define DEATH(mess) { perror(mess); exit(errno); }

void my_atexit_fun1 (void)
{
    printf ("I am in my_atexit_fun1\n");
}

void my_atexit_fun2 (void)
{
    printf ("I am in my_atexit_fun2\n");
}

int main (int argc, char *argv[])
{
    printf ("beginning the atexit program .... \n");

    if (atexit (my_atexit_fun1) < 0)
        DEATH ("atexit");
    printf ("Installed my_atexit_fun1\n");

    if (atexit (my_atexit_fun2) < 0)
        DEATH ("atexit");
    printf ("Installed my_atexit_fun2\n");

    exit (EXIT_SUCCESS);
}
