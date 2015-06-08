/* **************** LPD:1.0 s_19/lab1_mutex.c **************** */
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
 * Threads with Mutexes
 *
 * Extend the counter exercise from the previous section previous
 * exercise to protect the counter variable with a mutex.
 *
 * What happens if the counter thread exits while holding the mutex?
 * Does it release the lock?
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

static char running = 1;        /* note this can be static now! */
static long long counter = 0;

pthread_mutex_t c_mutex;

/* This is the "computational" thread */
void *process (void *arg)
{
    while (running) {
        pthread_mutex_lock (&c_mutex);
        counter++;
        pthread_mutex_unlock (&c_mutex);
    }
    pthread_exit (NULL);
}

/* This is the "user interface" thread */
int main (int argc, char **argv)
{
    int i;
    pthread_t threadId;
    void *retval;

    /* Initialize a mutex with default attributes */
    pthread_mutex_init (&c_mutex, NULL);

    /* Start up the processing thread. */
    if (pthread_create (&threadId, NULL, process, "0"))
        DEATH ("pthread_create");

    /* Every so often, look at the counter and print it out. */
    for (i = 0; i < 10; i++) {
        sleep (1);
        pthread_mutex_lock (&c_mutex);
        printf ("%lld\n", counter);
        pthread_mutex_unlock (&c_mutex);
    }

    /* Tell the processing thread to quit. */
    running = 0;

    /* Wait for the processing thread to quit. */
    if (pthread_join (threadId, &retval))
        DEATH ("pthread_join");

    return 0;
}
