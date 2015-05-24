/* **************** LPD:1.0 s_16/blocksig.c **************** */
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
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int rc;
    sigset_t sigsus, oldset;

    /* block all possible signals */
    rc = sigfillset (&sigsus);
    rc = sigprocmask (SIG_SETMASK, &sigsus, &oldset);

    printf (" going to sleep 5 seconds, try control-C!\n");
    sleep (5);
    printf (" going ahead\n");

    /* restore original mask */
    rc = sigprocmask (SIG_SETMASK, &oldset, NULL);

    /* Do something pointless, forever */
    for (;;) {
        printf ("This is a pointless message.\n");
        sleep (1);
    }
    return 0;
}
