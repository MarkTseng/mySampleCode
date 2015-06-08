/* **************** LPD:1.0 s_17/lab1_siginfo.c **************** */
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
 * Using siginfo and sigqueue().
 *
 * Take the simple example using sigqueue() and expand it to pass a
 * structure into the enhanced signal handler.
 *
 * Do this a number of times and print out values of interest.
 @*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define DEATH(mess) { perror(mess); exit(errno); }

struct my_s
{
    int x;
    char s[32];
};

/* our signal handlers */

void sig_act (int sig, siginfo_t * si, void *a)
{
    struct my_s *s = si->si_ptr;
    printf ("in handler pid=%d, SIGNAL = %d\n", getpid (), sig);

    printf ("si_signo = \t%d\n", si->si_signo);
    printf ("si_code = \t%d\n", si->si_code);
    printf ("si_pid = \t%d\n", si->si_pid);
    printf ("si_uid = \t%d\n", si->si_uid);
    printf ("si_ptr.x = \t%d\n", s->x);
    printf ("si_ptr.s = \t%s\n", s->s);
}

int main (int argc, char *argv[])
{
    struct sigaction act;
    struct my_s s;
    int j, sig = SIGINT;
    union sigval sv;

    if (argc > 1)
        sig = atoi (argv[1]);

    memset (&act, 0, sizeof (act));
    act.sa_sigaction = sig_act;
    act.sa_flags = SA_SIGINFO;
    if (sigaction (sig, &act, NULL) < 0)    /* for CTRL-C */
        DEATH ("sigaction");

    printf ("pid=%d Successfully installed signal handler for signal=%d\n",
            getpid (), sig);

    for (j = 0; j < 3; j++) {
        printf ("This is a pointless message\n");
        s.x = j * 100;
        strcpy (s.s, "hello buddy");
        sv.sival_ptr = &s;
        printf ("sigqueue returns %d\n", sigqueue (getpid (), sig, sv));
        sleep (1);
    }
    exit (0);
}
