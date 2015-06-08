/* **************** LPD:1.0 s_18/lab4_constdest.c **************** */
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
 * Threads and Initialization and Exit Handlers
 *
 * Extend the counter exercise to have constructor and destructor
 * functions.
 *
 * Are they run for each thread? Or for only the master thread?
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

void init_fun (void) __attribute__ ((constructor));
void exit_fun (void) __attribute__ ((destructor));

void init_fun (void)
{
    printf ("I am in init_fun\n");
}
void exit_fun (void)
{
    printf ("I am in exit_fun\n");
}

#define DEATH(mess) { perror(mess); exit(errno); }

volatile char running = 1;
volatile static long long counter = 0;  /* if not volatile, won't work when
                                           optimization is turned on */

/* This is the "computational" thread */
void *process (void *arg)
{
    while (running) {
        counter++;
    };
    pthread_exit (NULL);
}

/* This is the "user interface" thread */
int main (int argc, char **argv)
{
    int i;
    pthread_t threadId;
    void *retval;

    /* Start up the processing thread. */
    if (pthread_create (&threadId, NULL, process, "0"))
        DEATH ("pthread_create");

    /* Every so often, look at the counter and print it out. */
    for (i = 0; i < 10; i++) {
        sleep (1);
        printf ("%lld\n", counter);
    }

    /* Tell the processing thread to quit. */
    running = 0;

    /* Wait for the processing thread to quit. */
    if (pthread_join (threadId, &retval))
        DEATH ("pthread_join");

    return 0;
}
