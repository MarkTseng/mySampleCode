/* **************** LPD:1.0 s_19/lab2_sem.c **************** */
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
 * POSIX Semaphores and Threads
 *
 * Modify the pthreads exercise (which may have already been enhanced
 * to use mutexes), to use POSIX semaphores to protect the counter
 * variable.
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <semaphore.h>

#define DEATH(mess) { perror(mess); exit(errno); }

/* volatile char running = 1; */
static char running = 1;        /* note this can be static now! */
static long long counter = 0;

sem_t c_sem;

/* This is the "computational" thread */
void *process (void *arg)
{
    while (running) {
        sem_wait (&c_sem);
        counter++;
        sem_post (&c_sem);
    }
    pthread_exit (NULL);
}

/* This is the "user interface" thread */
int main (int argc, char **argv)
{
    int i;
    pthread_t threadId;
    void *retval;

    /* Initialize a semaphore up */
    sem_init (&c_sem, 0, 1);

    /* Start up the processing thread. */
    if (pthread_create (&threadId, NULL, process, "0"))
        DEATH ("pthread_create");

    /* Every so often, look at the counter and print it out. */
    for (i = 0; i < 10; i++) {
        sleep (1);
        sem_wait (&c_sem);
        printf ("%lld\n", counter);
        sem_post (&c_sem);
    }

    /* Tell the processing thread to quit. */
    running = 0;

    /* Wait for the processing thread to quit. */
    if (pthread_join (threadId, &retval))
        DEATH ("pthread_join");

    return 0;
}
