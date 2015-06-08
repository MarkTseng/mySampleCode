/* **************** LPD:1.0 s_19/lab4_prodcons.c **************** */
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
 * Producer/Consumer
 *
 * Write a multi-threaded program that has one or more producers and
 * one or more consumers, each producer and consumer being represented
 * by a thread.
 *
 * The producers create a data event, which can be as simple as
 * storing an index in a structure, while the consumers use the event.
 *
 * There should be a maximum number of events that can be buffered
 * by producers before consumers eliminate them.
 *
 * The solution is implemented in terms of condition variables; you
 * can probably can find other methods, such as using semaphores.
@*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NMAX 200
#define NBUF 24
#define NPROD 4
#define NCONS 7

static pthread_cond_t cond_not_empty = PTHREAD_COND_INITIALIZER;
static pthread_cond_t cond_not_full = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int j_produced = 0, j_consumed = 0, nbuf = NBUF, nmax = NMAX;
volatile int j_active = 0;

struct event
{
    int location;
};

struct event **buf_struct;

void produce (int j)
{                               /* j runs from 1 to nbuf */
    int location = (j - 1) % nbuf;
    buf_struct[location]->location = location;
    printf ("          PRODUCE: loc = %3d, id=%3d\n", location,
            (int)pthread_self () % 1000);
}

void consume (int j)
{                               /* j runs from 1 to nbuf */
    int location = (j - 1) % nbuf;
    buf_struct[location]->location = location;
    printf ("          CONSUME: loc = %3d, id=%3d\n", location,
            (int)pthread_self () % 1000);
}

#define PRO_QUIT { \
        printf ("Exiting the producer thread, id=%3d\n", (int)pthread_self()%1000); \
        pthread_exit (0); }
#define PRO_CHECK { \
    if ( j_produced == nmax){ pthread_mutex_unlock(&lock); PRO_QUIT; }}

void *producer (void *arg)
{
    printf ("Starting the producer thread, id=%3d\n",
            (int)pthread_self () % 1000);
    while (j_produced < nmax) {
        pthread_mutex_lock (&lock);
        while (j_active >= nbuf) {
            PRO_CHECK;
            pthread_cond_wait (&cond_not_full, &lock);
        }
        PRO_CHECK;
        j_produced++;
        j_active++;
        printf ("prod %3d, active %3d, id=%3d", j_produced, j_active,
                (int)pthread_self () % 1000);
        produce (j_produced);
        pthread_cond_broadcast (&cond_not_empty);
        pthread_mutex_unlock (&lock);
    }
    PRO_QUIT;

}

#define CON_QUIT { \
        printf ("Exiting the consumer thread, id=%3d\n", (int)pthread_self()%1000); \
        pthread_exit (0); }
#define CON_CHECK { \
    if ( j_consumed == nmax ) { pthread_mutex_unlock (&lock); CON_QUIT; } }

void *consumer (void *arg)
{
    printf ("Starting the consumer thread, id=%3d\n",
            (int)pthread_self () % 1000);
    while (j_consumed < nmax) {
        pthread_mutex_lock (&lock);
        while (j_active == 0) {
            CON_CHECK;
            pthread_cond_wait (&cond_not_empty, &lock);
        }
        CON_CHECK;
        j_consumed++;
        printf ("cons %3d, active %3d, id=%3d", j_consumed, j_active,
                (int)pthread_self () % 1000);
        consume (j_consumed);
        j_active--;
        pthread_cond_broadcast (&cond_not_full);
        pthread_mutex_unlock (&lock);
    }
    CON_QUIT;
}

int main (int argc, char *argv[])
{
    pthread_t t_p[NPROD], t_c[NCONS];
    void *rc;
    int j;
    if (argc > 2) {
        nmax = atoi (argv[1]);
        nbuf = atoi (argv[2]);
    }

    buf_struct = malloc (nbuf * sizeof (unsigned long));

    for (j = 0; j < nbuf; j++)
        buf_struct[j] = malloc (sizeof (struct event));

    for (j = 0; j < NPROD; j++)
        pthread_create (&t_p[j], NULL, producer, NULL);
    for (j = 0; j < NCONS; j++)
        pthread_create (&t_c[j], NULL, consumer, NULL);

    for (j = 0; j < NPROD; j++)
        pthread_join (t_p[j], &rc);
    for (j = 0; j < NCONS; j++)
        pthread_join (t_c[j], &rc);

    exit (0);
}
