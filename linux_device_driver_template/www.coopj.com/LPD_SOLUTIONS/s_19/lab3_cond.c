/* **************** LPD:1.0 s_19/lab3_cond.c **************** */
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
 * Condition Variables
 *
 * Write a program that creates four threads and a counter.
 *
 * Have one thread wait for input, with scanf("%d", &val). After
 * getting the value it should (safely) add that value to the counter.
 *
 * Meanwhile, the other three threads should be be in a loop. In each
 * loop, they should:
 *
 *      -- Wait for the counter to be greater than zero.
 *      -- Decrement the counter; print a message with their thread ID
 *      (get it from pthread_self()) and the counter value.
 *      -- Sleep for one second.
 *
 * You should hold a lock when looking at or changing the counter, but
 * not at any other time - especially not when sleeping or waiting for
 * user input!
 *
 * If you want to be really good, try not even holding the lock when
 * you are printing. Make sure that you don't refer to the counter
 * outside of the lock though.
 *
 @*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_WORKERS 3

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

/*
 * Both the counter and the "exiting" flag are protected by the lock.
 */
static volatile int counter = 0;
static volatile int exiting = 0;

/*
 * Wait for some data to show up in the counter. Then "process" the data.
 * To process the data I just decrement the counter and sleep for a second.
 *
 * Parameters:
 *   arg - Not used.
 * Return value:
 *   Not used; always returns NULL.
 */
void *decrementer (void *arg)
{
    int counterVal;

    for (;;) {
        /*
         * Lock the mutex so we can safely examine the counter and the
         * "exiting" flag.
         */
        pthread_mutex_lock (&lock);
        while (!exiting && (counter <= 0)) {
            /*
             * "cond" should be signalled when the counter
             * is incremented or the "exiting" flag is set, so we will wake up as
             * soon as we have something to do.
             */
            pthread_cond_wait (&cond, &lock);
        }
        if (exiting) {
            pthread_mutex_unlock (&lock);
            pthread_exit (NULL);
        }
        /*
         * We copy "counter" into the local "counterVal" so that we can print it
         * out after releasing the lock.
         */
        counterVal = --counter;
        pthread_mutex_unlock (&lock);

        printf ("Thread %ld decrementing to %d.\n",
                pthread_self (), counterVal);
        sleep (1);
    }
}

int main (int argc, char *argv[])
{
    pthread_t threads[NUM_WORKERS];
    int i, val;

    for (i = 0; i < NUM_WORKERS; ++i) {
        pthread_create (&threads[i], NULL, decrementer, NULL);
    }
    for (;;) {
        switch (scanf ("%d", &val)) {
        case 1:
            /*
             * We got a value to add to the counter. Add it, then broadcast to
             * cond to wake up any threads waiting for something to do.
             */
            pthread_mutex_lock (&lock);
            counter += val;
            pthread_cond_broadcast (&cond);
            pthread_mutex_unlock (&lock);
            break;

        case EOF:
            /*
             * We got an EOF, so we set the "exiting" flag, broadcast to the workers,
             * then wait for them all to exit cleanly before we close up the
             * application.
             */
            pthread_mutex_lock (&lock);
            exiting = 1;
            pthread_cond_broadcast (&cond);
            pthread_mutex_unlock (&lock);
            for (i = 0; i < NUM_WORKERS; ++i) {
                pthread_join (threads[i], NULL);
            }
            exit (0);

        default:
            fprintf (stderr,
                     "You must enter numbers or EOF to end the program.\n");
            exit (1);
        }
    }
}
