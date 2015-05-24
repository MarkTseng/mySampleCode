/* **************** LPD:1.0 s_16/lab1_signal.c **************** */
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
 * Signals:
 *
 * Using the signal() interface for installing signal handlers, write
 * a simple program to do something pointless in an infinite
 * loop. (Like printing out the same message after every second.)
 *
 * Upon getting a CTRL-C (SIGINT) the program should print a message,
 * but not die.
 *
 * Upon getting a CTRL-\ (SIGQUIT) it should call abort() to dump
 * core.
 *
 * Note: Some Linux distributions set the maximum core dump size to 0.
 * Type ulimit -c to find out what you actually have.  If the maximum
 * core dump size has to be set, do it with ulimit -c {nbytes}.
 *
@*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

/* our signal handlers */

void sig_int (int what)
{
    printf ("We have received SIGINT: Continuing Anyway, Boss.\n");
}
void sig_quit (int what)
{
    printf ("We have received SIGQUIT: Aborting.\n");
    abort ();
}

int main (int argc, char *argv[])
{
    /* Install signal handlers */
    signal (SIGINT, sig_int);   /* for CTRL-C */
    signal (SIGQUIT, sig_quit); /* for CTRL-\ */

    /* Do something pointless, forever */
    for (;;) {
        printf ("This is a pointless message.\n");
        sleep (1);
    }
    exit (0);
}
