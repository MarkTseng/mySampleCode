/* **************** LPD:1.0 s_28/lab2_server_select.c **************** */
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
 * Concurrent Servers (select() solution)
 @*/

#include "lab_server.h"

fd_set fdset, testset;
int fdmax = 0;
int nopen = 0;
int *isopen, *cd;
int ncons = MAX_NUM_CONNECTIONS;    /* note this may be modified */

void terminate_client (int ci)
{
    close (cd[ci]);
    isopen[ci] = 0;
    FD_CLR (cd[ci], &fdset);
    nopen--;
    printf ("connection closed   (cd[%2d] = %2d), nopen = %2d\n",
            ci, cd[ci], nopen);
    if (cd[ci] == fdmax)        /* has to be reset */
        fdmax = get_max_fd (fdmax, isopen, cd, ncons);
}

int main (int argc, char *argv[])
{
    int sd, ci, n, backlog = MAX_BACKLOG;
    short port = PORTNUM;

    if (argc > 1)
        ncons = atoi (argv[1]);
    if (ncons > FD_SETSIZE)
        DEATH ("Can't have more than 1024 for select(), and ulimit -s"
               "  must be a bit larger")

            cd = malloc (ncons * sizeof (int));
    isopen = malloc (ncons * sizeof (int));
    gethostname (hostname, 128);

    /* open an internet tcp stream socket */
    /* socket, setsockopt for reuse, bind, listen; */

    sd = get_socket (port, backlog);

    for (ci = 0; ci < ncons; ci++)
        isopen[ci] = 0;

    FD_ZERO (&fdset);
    FD_SET (sd, &fdset);

    for (;;) {

        /* wait for something to happen on one of the descriptors */

        testset = fdset;
        if (sd > fdmax)
            fdmax = sd;         /* to begin with */
        n = select (fdmax + 1, &testset, NULL, NULL, NULL);

        if (n < 0)
            DEATH ("select");

        /* if you do a time out you have to deal with n = 0 */

        /* accept new connection only if less than the maximum is there */

        if (FD_ISSET (sd, &testset) && (nopen < ncons)) {

            /*  find the first open one */

            ci = accept_one (sd, isopen, cd, ncons);
            isopen[ci] = 1;
            FD_SET (cd[ci], &fdset);
            nopen++;
            if (cd[ci] > fdmax)
                fdmax = cd[ci];
            printf
                ("connection accepted (cd[%2d] = %2d), nopen = %2d\n",
                 ci, cd[ci], nopen);

        }
        /* service existing connections */

        for (ci = 0; ci < ncons; ci++) {
            if (isopen[ci] && FD_ISSET (cd[ci], &testset))
                if (handle_client (cd[ci]))
                    terminate_client (ci);
        }
    }

    close (sd);
    free (cd);
    free (isopen);
    exit (EXIT_SUCCESS);
}
