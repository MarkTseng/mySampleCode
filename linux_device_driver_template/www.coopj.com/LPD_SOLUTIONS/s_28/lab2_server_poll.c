/* **************** LPD:1.0 s_28/lab2_server_poll.c **************** */
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
 * Concurrent Servers (poll() solution)
 @*/

#include <sys/poll.h>
#include "lab_server.h"

struct pollfd *ufds;
int fdmax = 0;
int nopen = 0;
int *isopen, *cd;
int ncons = MAX_NUM_CONNECTIONS;    /* note this may be modified */

void terminate_client (int ci)
{
    struct pollfd *pfd = &ufds[ci];
    close (cd[ci]);
    isopen[ci] = 0;
    pfd->fd = -1;
    pfd->events = 0;
    nopen--;
    printf ("connection closed   (cd[%2d] = %2d), nopen = %2d\n",
            ci, cd[ci], nopen);
    if (cd[ci] == fdmax)        /* has to be reset */
        fdmax = get_max_fd (fdmax, isopen, cd, ncons);
}
int main (int argc, char *argv[])
{
    int sd, ci, n, backlog = MAX_BACKLOG, timeout = -1, nfds, nconsp1;
    short port = PORTNUM;
    struct pollfd *pfd, *pfd0;

    if (argc > 1)
        ncons = atoi (argv[1]);
    nconsp1 = ncons + 1;

    cd = malloc ((nconsp1 + 8) * sizeof (int));
    isopen = malloc ((nconsp1 + 8) * sizeof (int));
    ufds = malloc ((nconsp1 + 8) * sizeof (struct pollfd));
    memset (ufds, 0, (nconsp1 + 8) * sizeof (struct pollfd));

    /* increase maximum number of file descriptors, must be root! */

    /* a few extra, for 0, 1, 2 etc. */
    check_and_set_max_fd (nconsp1 + 8);

    gethostname (hostname, 128);

    /* open an internet tcp stream socket */
    /* socket, setsockopt for reuse, bind, listen; */
    sd = get_socket (port, backlog);

    /* for the listening socket */

    pfd0 = &ufds[0];
    pfd0->fd = sd;
    pfd0->events = POLLIN;

    isopen[0] = 0;

    for (ci = 1; ci < nconsp1 + 8; ci++) {
        pfd = &ufds[ci];
        pfd->fd = -1;
        pfd->events = 0;
        isopen[ci] = 0;
    }

    for (;;) {

        /* wait for something to happen on one of the descriptors */

        nfds = 1 + nopen;
        if (sd > fdmax)
            fdmax = sd;         /* to begin with */

        /*        n = poll (ufds, nconsp1, timeout);  */
        n = poll (&ufds[0], fdmax + 1, timeout);

        if (n < 0)
            DEATH ("poll");

        /* if you do a time out you have to deal with n = 0 */

        /* accept new connection only if less than the maximum is there */

        if ((pfd0->revents && POLLIN) && (nopen < ncons)) {
            isopen[0] = 1;

            /*  find the first open one */

            ci = accept_one (sd, isopen, cd, ncons);
            isopen[ci] = 1;
            pfd = &ufds[ci];
            pfd->fd = cd[ci];
            pfd->events = POLLIN;
            nopen++;
            if (cd[ci] > fdmax)
                fdmax = cd[ci];
            printf
                ("connection accepted (cd[%2d] = %2d), nopen = %2d\n",
                 ci, cd[ci], nopen);

        }
        /* service existing connections */

        for (ci = 1; ci < nconsp1; ci++) {
            pfd = &ufds[ci];
            if (isopen[ci] && (pfd->revents && POLLIN)) {
                if (handle_client (cd[ci]))
                    terminate_client (ci);
            }
            fflush (stdout);
        }
    }
    close (sd);
    free (cd);
    free (isopen);
    exit (EXIT_SUCCESS);
}
