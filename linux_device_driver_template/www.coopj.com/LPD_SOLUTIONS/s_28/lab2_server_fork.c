/* **************** LPD:1.0 s_28/lab2_server_fork.c **************** */
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
 * Concurrent Servers (fork() solution)
 *
 @*/

#include <signal.h>
#include <sys/wait.h>

#include "lab_server.h"

#define STACK_SIZE 64           /* in KB */
int nopen = 0;
pid_t *pid;
int *isopen, *cd;
int ncons = MAX_NUM_CONNECTIONS;    /* note this may be modified */

void terminate_client (int ci)
{
    close (cd[ci]);
    printf ("connection closed for pid=%d  (cd[%2d] = %2d)\n",
            getpid (), ci, cd[ci]);
    fflush (stdout);
    exit (EXIT_SUCCESS);
}

void cleanupkids (int sig)
{
    pid_t pid_test;
    int ci, status;

    for (ci = 0; ci < ncons; ci++) {

        if (!isopen[ci])
            continue;

        pid_test = waitpid (pid[ci], &status, WNOHANG);

        if (pid_test <= 0)      /* this guy is not done */
            continue;

        /* this guy is finished */
        isopen[ci] = 0;
        nopen--;
        printf ("caught the kid pid=%d (cd[%2d] = %2d), nopen = %2d\n",
                pid_test, ci, cd[ci], nopen);
        fflush (stdout);
        close (cd[ci]);
    }
}

int main (int argc, char *argv[])
{
    int sd, ci, backlog = MAX_BACKLOG;
    short port = PORTNUM;

    if (argc > 1)
        ncons = atoi (argv[1]);

    /* decrease stack size, non-privilged operation */
    /*    shrink_stack (STACK_SIZE); not needed for parent */

    /* increase maximum number of file descriptors, must be root! */
    /* a few extra, for 0, 1, 2 etc. */
    check_and_set_max_fd (ncons + 8);

    cd = malloc (ncons * sizeof (int));
    isopen = malloc (ncons * sizeof (int));
    pid = malloc (ncons * sizeof (pid_t));
    gethostname (hostname, 128);

    signal (SIGCHLD, cleanupkids);

    /* open an internet tcp stream socket */
    /* socket, setsockopt for reuse, bind, listen; */

    sd = get_socket (port, backlog);

    for (ci = 0; ci < ncons; ci++)
        isopen[ci] = 0;

    for (;;) {

        /* accept new connection only if less than
           the maximum is there */
        while (nopen == ncons) {
            /* reap any children; there may be a
               race condition or signal pileup! */
            cleanupkids (SIGCHLD);
        }

        /*  find the first open one */

        ci = accept_one (sd, isopen, cd, ncons);
        isopen[ci] = 1;
        nopen++;
        printf ("connection accepted (cd[%2d] = %2d), nopen = %2d\n",
                ci, cd[ci], nopen);
        fflush (stdout);

        /* fork off a child to handle the connection */

        pid[ci] = fork ();
        if (pid[ci] < 0)
            DEATH ("Forking");

        if (pid[ci] == 0) {     /* child */

            /* decrease stack size, non-privilged operation */
            shrink_stack (STACK_SIZE);
            /* can set back for kids */
            set_max_fd (16);

            while (!handle_client (cd[ci])) ;
            terminate_client (ci);
        } else {                /* parent */
            printf (" I forked for ci=%d, pid=%d\n", ci, pid[ci]);
            fflush (stdout);
        }
    }

    close (sd);
    free (isopen);
    free (cd);
    exit (EXIT_SUCCESS);
}
