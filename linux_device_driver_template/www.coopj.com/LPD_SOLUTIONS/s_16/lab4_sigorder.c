/* **************** LPD:1.0 s_16/lab4_sigorder.c **************** */
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
 * Examining Signal Priorities.
 *
 * In the below, do not send or handle either of the signals SIGKILL
 * or SIGSTOP.
 *
 * Write a C program that includes a signal handler that can handle
 * any signal.  The handler should avoid making any system calls (such
 * as those that might occur doing I/O).
 *
 * The handler should simply store the sequence of signals as they
 * come in, and update a counter array for each signal that indicates
 * how many times the signal has been handled.
 *
 * The program should begin by suspending processing of all signals
 * (using sigprocmask().
 *
 * It should then install the new set of signal handlers (which can be
 * the same for all signals, registering them with the sigaction()
 * interface.
 *
 * The the program should send every possible signal to itself multiple
 * times, using the raise() function.
 *
 * Signal processing should be resumed, once again using sigprocmask).
 *
 * Before completing, the program should print out statistics
 * including:
 *
 *    The total number of times each signal was received.
 *
 *    The order in which the signals were received, noting each time the total
 *
 *    number of times that signal had been received up to that point.
 *
 * Note the following items:
 *
 *   If more than one of a given signal is raised while the process
 *   has blocked it, does the process receive it multiple times?
 *
 *   Are all signals received by the process, or are some handled
 *   before they reach it?
 *
 *   What order are the signals received in?
 *
 * If you are running KDE as your desktop environment, you may find
 * signal 32 blocked.  One signal, SIGCONT (18 on x86) may not get
 * through; can you figure out why?
 *
 * It appears that in the 2.6 kernel signals 32 and 33 can not be
 * blocked and will cause your program to fail.  Even though header
 * files indicate SIGRTMIN=32, the command kill -l indicates
 * SIGRTMIN=34.
 *
 * Avoid sending these signals.
 *
 @*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUMSIGS 64

/* prototypes of locally-defined signal handlers */

void (sig_handler) (int);

int sig_count[NUMSIGS + 1];     /* counter for signals received */
volatile static int line = 0;
volatile int signumbuf[6400], sigcountbuf[6400];

int main (int argc, char *argv[])
{
    sigset_t sigmask_new, sigmask_old;
    struct sigaction sigact, oldact;
    int signum, rc, i;
    pid_t pid;

    pid = getpid ();

    /* block all possible signals */
    rc = sigfillset (&sigmask_new);
    rc = sigprocmask (SIG_SETMASK, &sigmask_new, &sigmask_old);

    /* Assign values to members of sigaction structures */
    memset (&sigact, 0, sizeof (struct sigaction));
    sigact.sa_handler = sig_handler;    /* we use a pointer to a handler */
    sigact.sa_flags = 0;        /* no flags */
    /* VERY IMPORTANT */
    sigact.sa_mask = sigmask_new;   /* block signals in the handler itself  */

    /*
     * Now, use sigaction to create references to local signal
     * handlers * and raise the signal to myself
     */

    printf
        ("\nInstalling signal handler and Raising signal for signal number:\n\n");
    for (signum = 1; signum <= NUMSIGS; signum++) {
        if (signum == SIGKILL || signum == SIGSTOP || signum == 32
            || signum == 33) {
            printf ("  --");
            continue;
        }
        sigaction (signum, &sigact, &oldact);
        /* send the signal 3 times! */
        rc = raise (signum);
        rc = raise (signum);
        rc = raise (signum);
        if (rc) {
            printf ("Failed on Signal %d\n", signum);
        } else {
            printf ("%4d", signum);
            if (signum % 16 == 0)
                printf ("\n");
        }
    }
    fflush (stdout);

    /* restore original mask */
    rc = sigprocmask (SIG_SETMASK, &sigmask_old, NULL);

    printf ("\nSignal  Number(Times Processed)\n");
    printf ("--------------------------------------------\n");
    for (i = 1; i <= NUMSIGS; i++) {
        printf ("%4d:%3d  ", i, sig_count[i]);
        if (i % 8 == 0)
            printf ("\n");
    }
    printf ("\n");

    printf ("\nHistory: Signal  Number(Count Processed)\n");
    printf ("--------------------------------------------\n");
    for (i = 0; i < line; i++) {
        if (i % 8 == 0)
            printf ("\n");
        printf ("%4d(%1d)", signumbuf[i], sigcountbuf[i]);
    }
    printf ("\n");
    exit (0);
}

void sig_handler (int sig)
{
    sig_count[sig]++;
    signumbuf[line] = sig;
    sigcountbuf[line] = sig_count[sig];
    line++;
}
