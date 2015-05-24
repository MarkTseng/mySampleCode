/* **************** LPD:1.0 s_28/lab2_server_pthread.c **************** */
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
 * Concurrent Servers (pthread solution)
 @*/

#include "lab_server.h"

#include <pthread.h>

#define STACK_SIZE 128          /* in KB */

pthread_t *thread_id;
int *thread_data;
pthread_mutex_t nopen_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int nopen = 0;         /* note the volatile ! */
int *isopen, *cd;
int ncons = MAX_NUM_CONNECTIONS;    /* note this may be modified */

void terminate_client (int ci)
{
    isopen[ci] = 0;
    pthread_mutex_lock (&nopen_lock);
    nopen--;
    close (cd[ci]);
    /* this guy is finished */
    printf ("connection closed   (cd[%2d] = %2d), nopen = %2d\n",
            ci, cd[ci], nopen);
    pthread_mutex_unlock (&nopen_lock);

    fflush (stdout);
}
void *handle_clientp (void *arg)
{
    int *j, ci;
    j = arg;
    ci = *j;
    while (!handle_client (cd[ci])) ;
    terminate_client (ci);
    return NULL;
}

int main (int argc, char *argv[])
{
    int rc, sd, ci, backlog = MAX_BACKLOG;
    short port = PORTNUM;
    pthread_attr_t thread_attr;

    if (argc > 1)
        ncons = atoi (argv[1]);

    /* decrease stack size, non-privilged operation */

    rc = pthread_attr_init (&thread_attr);
    rc = pthread_attr_setstacksize (&thread_attr, STACK_SIZE * 1024);

    /*  could do this instead of through pthread library */
    /*    shrink_stack (STACK_SIZE); */

    /* increase maximum number of file descriptors, must be root! */
    /* a few extra, for 0, 1, 2 etc. */
    check_and_set_max_fd (ncons + 8);

    cd = malloc (ncons * sizeof (int));
    isopen = malloc (ncons * sizeof (int));

    thread_id = malloc (ncons * sizeof (pthread_t));
    thread_data = malloc (ncons * sizeof (int));

    gethostname (hostname, 128);

    /* open an internet tcp stream socket */
    /* socket, setsockopt for reuse, bind, listen; */

    sd = get_socket (port, backlog);

    for (ci = 0; ci < ncons; ci++)
        isopen[ci] = 0;

    for (;;) {

        /* accept new connection only if less than the maximum is there */
        while (nopen == ncons) {
            /* wait for a thread to complete */
        }

        /*  find the first open one */

        ci = accept_one (sd, isopen, cd, ncons);
        isopen[ci] = 1;
        pthread_mutex_lock (&nopen_lock);
        nopen++;
        printf ("connection accepted (cd[%2d] = %2d), nopen = %2d\n",
                ci, cd[ci], nopen);
        pthread_mutex_unlock (&nopen_lock);
        fflush (stdout);

        /* spin off a new thread to handle the connection */
        thread_data[ci] = ci;

        if (pthread_create
            (thread_id + ci, &thread_attr, handle_clientp, thread_data + ci))
            DEATH ("Thread Creation");

        if (pthread_detach (thread_id[ci]))
            DEATH ("Thread Detaching");

        printf (" For ci=%d, new thread_pid=%ld\n", ci, thread_id[ci]);
        fflush (stdout);
    }

    free (isopen);
    free (cd);
    close (sd);
    exit (EXIT_SUCCESS);
}
