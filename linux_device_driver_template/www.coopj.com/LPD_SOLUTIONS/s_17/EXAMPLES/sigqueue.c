/* **************** LPD:1.0 s_17/sigqueue.c **************** */
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
#include <errno.h>
#include <string.h>

#define DEATH(mess) { perror(mess); exit(errno); }

/* our signal handlers */

void sig_act (int sig, siginfo_t * si, void *a)
{
    printf ("in handler pid=%d, SIGNAL = %d\n", getpid (), sig);

    printf ("si_signo = \t%d\n", si->si_signo);
    printf ("si_code = \t%d\n", si->si_code);
    printf ("si_pid = \t%d\n", si->si_pid);
    printf ("si_uid = \t%d\n", si->si_uid);
    printf ("si_value = \t%d\n", si->si_value.sival_int);
    printf ("si_int = \t%d\n", si->si_int);
}

int main (int argc, char *argv[])
{
    struct sigaction act;;
    int j;
    union sigval sv;

    memset (&act, 0, sizeof (act));
    act.sa_sigaction = sig_act;
    act.sa_flags = SA_SIGINFO;
    if (sigaction (SIGINT, &act, NULL) < 0) /* for CTRL-C */
        DEATH ("sigaction");

    printf ("pid=%d Successfully installed signal handler for SIGINT\n",
            getpid ());

    for (j = 0; j < 3; j++) {
        printf ("This is a pointless message\n");
        sv.sival_int = j * 100;
        printf ("sigqueue returns %d\n", sigqueue (getpid (), SIGINT, sv));
        sleep (1);
    }
    exit (0);
}
