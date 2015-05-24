/* **************** LPD:1.0 s_28/lab2_server_epoll.c **************** */
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
 * Concurrent Servers (epoll solution)
 @*/

#include <sys/epoll.h>
#include "lab_server.h"

/* private data structure describing an event */

int nopen = 0;
int *isopen, *cd;
int ncons = MAX_NUM_CONNECTIONS;    /* note this may be modified */
struct my_data
{
    int fd;
};
struct epoll_event *ep_event, *ep, *ep_sd;

void terminate_client (int sdes, int i)
{
    int ci;
    struct epoll_event *ept;
    for (ci = 0; ci < ncons; ci++) {
        if (sdes == cd[ci]) {
            close (sdes);
            isopen[ci] = 0;
            nopen--;
            ept = &ep_event[i];
            free (ept->data.ptr);
            break;
        }
    }
    printf ("connection closed   (cd[%2d] = %2d), nopen = %2d\n",
            ci, cd[ci], nopen);
}

void accept_it (int sd, int epfd)
{
    int ci;
    struct my_data *tptr;
    struct epoll_event *ept;

    /*  find the first open one */

    ci = accept_one (sd, isopen, cd, ncons);
    isopen[ci] = 1;

    tptr = malloc (sizeof (struct my_data));
    tptr->fd = cd[ci];
    ept = &ep_event[ci];
    ept->data.ptr = tptr;
    ept->events = EPOLLIN;
    epoll_ctl (epfd, EPOLL_CTL_ADD, cd[ci], ept);
    nopen++;
    printf
        ("connection accepted (cd[%2d] = %2d), nopen = %2d\n",
         ci, cd[ci], nopen);
}

int main (int argc, char *argv[])
{
    short port = PORTNUM;
    int sd, ci, n, i, epfd, backlog = MAX_BACKLOG, timeout = -1;
    int maxevents = 100;        /* no need to be large */
    struct my_data *tptr;

    if (argc > 1)
        ncons = atoi (argv[1]);

    cd = malloc ((ncons + 8) * sizeof (int));
    isopen = malloc ((ncons + 8) * sizeof (int));
    ep_event = malloc ((ncons + 8) * sizeof (struct epoll_event));
    memset (ep_event, 0, (ncons + 8) * sizeof (struct epoll_event));
    for (ci = 0; ci < ncons + 8; ci++)
        isopen[ci] = 0;

    /* a few extra, for 0, 1, 2 etc. */
    check_and_set_max_fd (ncons + 8);

    gethostname (hostname, 128);

    /* open an internet tcp stream socket */
    /* socket, setsockopt for reuse, bind, listen; */
    sd = get_socket (port, backlog);

    /* set up epoll */
    epfd = epoll_create (ncons);

    /* for the listening socket */

    tptr = malloc (sizeof (struct my_data));
    tptr->fd = sd;
    ep_sd = malloc (sizeof (struct epoll_event));
    ep_sd->data.ptr = tptr;
    ep_sd->events = EPOLLIN;

    epoll_ctl (epfd, EPOLL_CTL_ADD, sd, ep_sd);

    for (;;) {

        /* wait for something to happen on one of the descriptors */
        maxevents = 2;

        n = epoll_wait (epfd, ep_event, maxevents, timeout);

        if (n < 0)
            DEATH ("epoll");

        /* if you do a time out you have to deal with n = 0 */

        for (i = 0; i < n; i++) {

            ep = &ep_event[i];
            tptr = ep->data.ptr;

            /* accept new connection only if less than the maximum is there */

            if (tptr->fd == sd) {
                if (nopen < ncons)
                    accept_it (sd, epfd);
            } else {

                /* service existing connections */
                /* note arg is actual descriptor */
                if (handle_client (tptr->fd))
                    terminate_client (tptr->fd, i);
            }
        }
    }
    free (cd);
    free (isopen);
    close (epfd);
    close (sd);
    exit (EXIT_SUCCESS);
}
