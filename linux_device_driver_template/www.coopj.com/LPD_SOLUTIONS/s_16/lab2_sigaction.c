/* **************** LPD:1.0 s_16/lab2_sigaction.c **************** */
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
 * sigaction()
 *
 * Adapt the previous exercise (or start fresh) to use the sigaction
 * interface.  Set up a signal set and mask to do this.

 * Have the SIGINT handler sleep for a couple of seconds; what happens
 * if you send a SIGQUIT while it is sleeping?
 @*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define DEATH(mess) { perror(mess); exit(errno); }

/* our signal handlers */

void sig_int (int what)
{
    printf ("We have received SIGINT: Will sleep for 2 seconds and continue\n");
    sleep (2);
    printf (" done sleeping 2 seconds\n");
}

void sig_quit (int what)
{
    printf ("We have received SIGQUIT: Aborting.\n");
    abort ();
}

int main (int argc, char *argv[])
{
    struct sigaction act;

    memset (&act, 0, sizeof (act));

    /* Install signal handlers */
    act.sa_handler = sig_int;
    if (sigaction (SIGINT, &act, NULL) < 0) /* for CTRL-C */
        DEATH ("sigaction");

    printf ("Successfully installed signal handler for SIGINT\n");

    act.sa_handler = sig_quit;
    if (sigaction (SIGQUIT, &act, NULL) < 0)    /* for CTRL-\ */
        DEATH ("sigaction");

    printf ("Successfully installed signal handler for SIGQUIT\n");

    /* Do something pointless, forever */
    for (;;) {
        printf ("This is a pointless message.\n");
        sleep (1);
    }

    exit (0);
}
