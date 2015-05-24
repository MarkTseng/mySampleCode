/* **************** LPD:1.0 s_13/lab5_onexit.c **************** */
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
 * Exiting (on_exit demonstration)
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

void my_on_exit_fun (int rc_exit, int *arg)
{
    printf ("I am in my_on_exit_fun, rc_exit = %d, arg = %d\n", rc_exit, *arg);
}

int main (int argc, char *argv[])
{
    int exit_code, arg1 = 1;

    if (argc > 1) {
        exit_code = atoi (argv[1]);
    } else {
        printf (" USAGE: %s exit_code\n", argv[0]);
        exit (-1);
    }

    printf ("beginning the on_exit program: \n");
    if (on_exit ((void *)my_on_exit_fun, &arg1) < 0)
        DEATH ("on_exit");
    printf ("Installed  my_on_exit_fun\n");

    exit (exit_code);
}
