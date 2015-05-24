/* **************** LPD:1.0 s_18/lab3_sighandler.c **************** */
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
 * Signals, Threads and Handlers
 *
 * Instead of dedicating one thread to signals, install a signal
 * handler.
 *
 * After launching a number of threads, block handling the signal in
 * the master thread.
 *
 * Send one or more signals to the process either from the command
 * line (you can use SIGINT which is Control-C), or using kill().
 *
 * Verify which thread deals with the signals using pthread_self() to
 * print out the thread ID's.

 *
@*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#define NUM_THREADS 10

void sighand (int sig)
{
    printf ("Im executing signal handler in thread %ld\n", pthread_self ());
}

void *tfun (void *arg)
{
    printf ("            Thread %ld starting\n", pthread_self ());
    pause ();                   /* will return when any signal is received */
    printf ("            Thread %ld exiting\n", pthread_self ());
    pthread_exit (NULL);
}

int main (int argc, char *argv[])
{
    int i;

    pthread_t threads[NUM_THREADS];
    sigset_t sigmask;
    struct sigaction act;

    act.sa_handler = sighand;
    if (sigaction (SIGINT, &act, NULL) < 0) {
        printf ("Failed to install signal handler\n");
        exit (-1);
    }

    printf ("Master thread id = %ld\n", (long)pthread_self ());

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create (&threads[i], NULL, tfun, NULL);
        printf ("I created thread id=%ld\n", (long)threads[i]);
    }

    /* block the signal in the master thread  only */

    sigemptyset (&sigmask);
    sigaddset (&sigmask, SIGINT);
    pthread_sigmask (SIG_BLOCK, &sigmask, NULL);

    /* for fun send the signals to myself
       disable this to send Ctl-C from command line */

    for (i = 0; i < NUM_THREADS; i++) {
        kill (getpid (), SIGINT);
        sched_yield ();         /* Could also do a  sleep(1); */
    }

    /* wait for the threads to terminate */

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join (threads[i], NULL);
        printf ("Master thread joined with thread %d\n", i);
    }

    exit (0);

}
