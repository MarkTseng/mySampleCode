/* **************** LPD:1.0 s_18/lab2_signal.c **************** */
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
 * Signals and Threads
 *
 * Write a program that launches multiple threads, which has one
 * thread reserved for dealing with signals, using sigwait(),
 * pthread_sigmask().
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

void *sigfun (void *arg)
{
    int sigreceived;

    sigset_t sigmask;
    sigfillset (&sigmask);
    printf (" Signal Handling Thread %ld starting\n", pthread_self ());
    while (1) {
        sigwait (&sigmask, &sigreceived);
        printf (" Signal Handling Thread %ld received signal %d\n",
                pthread_self (), sigreceived);
    }
    printf (" Signal Handling Thread %ld exiting\n", pthread_self ());
    pthread_exit (NULL);
}

void *fun (void *arg)
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

    sigemptyset (&sigmask);
    sigaddset (&sigmask, SIGINT);
    pthread_sigmask (SIG_BLOCK, &sigmask, NULL);

    printf ("Master thread id = %ld\n", (long)pthread_self ());

    pthread_create (&threads[0], NULL, sigfun, NULL);
    printf ("I created signal handling thread id=%ld\n", (long)threads[0]);

    for (i = 1; i < NUM_THREADS; i++) {
        pthread_create (&threads[i], NULL, fun, NULL);
        printf ("I created thread id=%ld\n", (long)threads[i]);
    }

    sleep (1);
    printf ("Sending SIGINT (Ctl-C)\n");

    kill (getpid (), SIGINT);

    sleep (3);

    exit (0);

}
