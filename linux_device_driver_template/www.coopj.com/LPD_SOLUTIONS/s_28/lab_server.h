/* **************** LPD:1.0 s_28/lab_server.h **************** */
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
 * Concurrent Servers (header file)
 *
 * Take your code for a simple Internet TCP server, and adapt it to
 * handle multiple simultaneous connections using select().
 *
 * Set a maximum number of simultaneous connections to permit.,
 *
 * Make sure you examine the listening socket descriptor so you can
 * accept new connections, as well as listen for incoming data on
 * existing connections.
 *
 * To test the server, adapt your simple Internet TCP client to open
 * multiple sockets, and then send data to them in random order.  You
 * can do this with something like:
 *
 *
 * srand(seed);
 * ....
 * sd_to_writeto = rand()%number_of_connections;
 *
 *
 * What happens if you try to open more connections than your
 * server will permit?
 *  *
 * Implement the server using:
 *
 * select().
 *
 * poll().
 *
 * The epoll set of functions.
 *
 * fork().
 *
 * You'll need to create a child for each new incoming connection.
 *
 * You'll have to be careful to reap children when they terminate.
 * Otherwise they will become zombies and you will be limited in
 * the number of connections you can open.
 *
 * To do this you'll most likely want to use some combination of
 * waitpid() and installing a signal handling routine for
 * SIGCHLD.
 *
 * The pthread library.
 *
 * Create  multiple threads, using pthread_create() to create a
 * light weight process, or thread, for each new incoming connection.
 *
 * Make sure you detach the thread when you create it,
 * either by using pthread_detach() or setting the
 * attributes with pthread_attr_init(),
 * pthread_attr_setdetachstate().  Alternatively, you can wait
 * for the thread to complete with pthread_join().
 *
 * Be careful about any global variables that can be affected in
 * multiple threads, using pthread_mutex_lock() when
 * necessary.
 *
 * Make sure you compile with -pthread.
 *
 * To compare results you may wish to insert some timing functions in
 * the client program.  If you do, start the timers when all clients
 * have been opened, and stop them before they close, as otherwise
 * you'll just be timing how long it takes to open and close the
 * connections, which is a different metric.
 */

#ifndef _LAB_SERVER_H
#define _LAB_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/resource.h>

#define PORTNUM 7177
#define MAX_BACKLOG         1000
#define MAX_NUM_CONNECTIONS 10000
#define MSG_MAXSIZE 1024

#define DEATH(mess) { perror(mess); exit(errno); };

char hostname[128];

int accept_new_one (int sd, int *isopen, int *cd, int ncons)
{
    static int ci_last = 0;
    int ci, ci_loop;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof (struct sockaddr);

    /*  find the first open one */

    for (ci_loop = 0, ci = ci_last; ci_loop < ncons; ci_loop++) {
        if (!isopen[ci])
            break;
        ci++;
        if (ci == ncons)
            ci = 0;
    }
    ci_last = ci;

    if ((cd[ci] = accept (sd, (struct sockaddr *)&client_addr,
                          &client_addrlen)) < 0)
        DEATH ("accept");

    return ci;
}
int accept_one (int sd, int *isopen, int *cd, int ncons)
{
    int ci;
    struct sockaddr_in client_addr;
    socklen_t client_addrlen = sizeof (struct sockaddr);

    /*  find the first open one */

    for (ci = 0; ci < ncons; ci++)
        if (!isopen[ci])
            break;

    if ((cd[ci] = accept (sd, (struct sockaddr *)&client_addr,
                          &client_addrlen)) < 0)
        DEATH ("accept");

    return ci;
}

int get_socket (short port, int backlog)
{
    int sd, yes = 1;
    struct sockaddr_in my_addr;

    /* open an internet tcp stream socket */

    if ((sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
        DEATH ("socket");

    printf ("socket created on %s, (sd=%d)\n", hostname, sd);

    /* make socket re-useable */

    if (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)))
        DEATH ("setsockopt");

    /* bind the socket to a port */

    memset (&my_addr, 0, sizeof (struct sockaddr_in));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons (PORTNUM);

    if (bind (sd, (struct sockaddr *)&my_addr, sizeof (struct sockaddr_in)))
        DEATH ("bind");

    printf ("socket bound\n");

    /* set up a listening queue */

    if (listen (sd, backlog))
        DEATH ("listen");

    return sd;
}

int handle_client (int sd)
{
    int count;
    char message[MSG_MAXSIZE], response[80];

    sprintf (response, "Hi from server at: %s. PID=%d\n", hostname, getpid ());

    if ((count = recv (sd, message, MSG_MAXSIZE, 0)) > 0) {
        write (STDOUT_FILENO, message, count);
        send (sd, response, strlen (response), 0);
        /* better read only one line; only one process running! */
        return 0;
    }
    if (count < 0)
        printf ("handle_client recv error");

    /* note: recv() returns 0 if there is no message and there is an
       orderly shutdown.  this leads to a -1 here used to flag shutdown */

    return -1;
}

/* find the maximum file descriptor needed
   this should be optimized, but since in the
   current setting messages don't come in until
   all are open, this is ok */
int get_max_fd (int fdmax_p, int *isopen, int *cd, int ncons)
{
    int j, fdmax = fdmax_p;
    for (j = 0; j < ncons; j++)
        if (isopen[j] && (cd[j] > fdmax))
            fdmax = cd[j];
    return fdmax;
}

/* set the maximum number of file descriptors only if needed */
void check_and_set_max_fd (int n)
{
    struct rlimit rlp;

    (void)getrlimit (RLIMIT_NOFILE, &rlp);
    if (rlp.rlim_cur < n) {
        rlp.rlim_cur = rlp.rlim_max = n;
        if (setrlimit (RLIMIT_NOFILE, &rlp) < 0)
            DEATH (" Failed to set maxfile descriptors" ", run as root\n");
    }
    fprintf (stderr, "Max file descriptors set to: %d\n", (int)rlp.rlim_cur);
}

/* set the maximum number of file descriptors */
void set_max_fd (int n)
{
    struct rlimit rlp;
    rlp.rlim_cur = rlp.rlim_max = n;
    if (setrlimit (RLIMIT_NOFILE, &rlp) < 0)
        DEATH (" Failed to set maxfile descriptors" ", run as root\n");
}

/* change the per process stack size in user-space */
void shrink_stack (int stack_size)
{
    struct rlimit rlp;
    rlp.rlim_cur = rlp.rlim_max = stack_size;
    if (setrlimit (RLIMIT_STACK, &rlp) < 0)
        DEATH (" Failed to reset stacksize\n");
}

#endif
