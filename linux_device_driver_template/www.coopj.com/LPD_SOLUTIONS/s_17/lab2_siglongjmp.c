/* **************** LPD:1.0 s_17/lab2_siglongjmp.c **************** */
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
 * Using sigsetjmp() and siglongjmp().
 *
 * Write a simple program with a signal handler for SIGINT
 * (Control-C).  Have the signal handler return with a call to
 * siglongjmp() to a point in your main program where sigsetjmp() is
 * invoked.
 *    
 * For the first four times the signal handler is hit have the program
 * continue; the fifth time have it terminate.
 *    
 @*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>

#define DEATH(mess) { perror(mess); exit(errno); }

sigjmp_buf env;
int savesigs = 1;

/* our signal handlers */

void sig_int (int what)
{
    static int val = 0;
    val++;
    printf ("We have received SIGINT: val = %d\n", val);
    siglongjmp (env, val);
}

int main (int argc, char *argv[])
{
    int rc;
    struct sigaction act;
    memset (&act, 0, sizeof (act));

    /* Install signal handlers */
    act.sa_handler = sig_int;
    if (sigaction (SIGINT, &act, NULL) < 0) /* for CTRL-C */
        DEATH ("sigaction");

    printf ("Successfully installed signal handler for SIGINT\n");

    rc = sigsetjmp (env, savesigs);
    printf ("rc from sigsetmp = %d\n", rc);

    if (rc == 4) {
        printf ("I'm returning non-locally and then I'm going to quit\n");
        exit (EXIT_SUCCESS);
    }

    for (;;) {
        printf ("Going to wait for a signal with pause()\n");
        pause ();
        printf ("I rejoined inside the sleep loop\n");
    }
    /* should never reach here */
    exit (EXIT_FAILURE);
}
