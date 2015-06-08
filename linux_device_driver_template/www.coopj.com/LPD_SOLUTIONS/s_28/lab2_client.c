/* **************** LPD:1.0 s_28/lab2_client.c **************** */
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
 * Concurrent Servers (Client program)
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PORTNUM 7177
#define MAX_NUM_CONNECTIONS 10000
#define MSG_MAXSIZE 1024
#define MAX_NWRITES 10

#define DEATH(mess) { perror(mess); exit(errno); };

#define GET_ELAPSED_TIME(tv1,tv2) ( \
  (double)( (tv2.tv_sec - tv1.tv_sec) \
            + .000001 * (tv2.tv_usec - tv1.tv_usec)))

int ncons = 3, *sdvec;
char message[MSG_MAXSIZE], hostname[128];
int nwrites = MAX_NWRITES;

double open_sockets (struct sockaddr_in s_addr)
{
    int j;
    struct timeval tv1, tv2;
    gettimeofday (&tv1, NULL);
    for (j = 0; j < ncons; j++) {
        if ((sdvec[j] = socket (PF_INET, SOCK_STREAM, 0)) < 0)
            DEATH ("socket");
        if (connect (sdvec[j], (struct sockaddr *)&s_addr,
                     sizeof (struct sockaddr_in)) == -1)
            DEATH ("connect");

        /* put in a short sleep so accept is not overwhelmed */
        usleep (1000);          /* 1 microsecond */
    }
    gettimeofday (&tv2, NULL);
    return GET_ELAPSED_TIME (tv1, tv2);
}

double write_random_sockets (void)
{
    int j, k, count;
    struct timeval tv1, tv2;

    srand (getpid ());          /* initialize random number generator */

    gettimeofday (&tv1, NULL);

    for (k = 0; k < nwrites; k++) {

        /* pick the socket descriptor at random */

        j = rand () % ncons;

        sprintf (message, "Hi. I'm PID %d, socket %3d (k=%3d), on %s\n",
                 getpid (), sdvec[j], k, hostname);

        send (sdvec[j], message, strlen (message), 0);

        /* read and display the response */
        count = recv (sdvec[j], message, MSG_MAXSIZE, 0);
        /*
           printf ("socket descriptor %d: ", sdvec[j]);
           fflush (stdout);
           write (STDOUT_FILENO, message, count);
         */
    }
    gettimeofday (&tv2, NULL);

    return GET_ELAPSED_TIME (tv1, tv2);
}

double close_sockets (void)
{
    int j;
    struct timeval tv1, tv2;
    gettimeofday (&tv1, NULL);
    for (j = 0; j < ncons; j++)
        close (sdvec[j]);
    gettimeofday (&tv2, NULL);
    return GET_ELAPSED_TIME (tv1, tv2);
}

int main (int argc, char *argv[])
{
    double open_time, write_time, close_time, elapsed_time;
    struct sockaddr_in s_addr;
    struct hostent *s_hostent = NULL;
    struct rlimit rlp;

    if (argc < 2)
        DEATH ("USAGE: client [hostname|ip_address] #connections #writes\n");

    if (argc > 2)
        ncons = atoi (argv[2]);
    if (ncons > MAX_NUM_CONNECTIONS)
        ncons = MAX_NUM_CONNECTIONS;
    if (argc > 3)
        nwrites = atoi (argv[3]);

    /* increase maximum number of file descriptors, must be root! */
    rlp.rlim_cur = rlp.rlim_max = ncons + 8;

    if (setrlimit (RLIMIT_NOFILE, &rlp) < 0)
        DEATH
            (" Failed to set maxinum number of file descriptors, run as root\n");

    gethostname (hostname, 128);

    printf
        ("On %s, opening %d connections to %s with  "
         "%d writes randomly distributed\n", hostname, ncons, argv[1], nwrites);

    /* determine the server's address */

    memset (&s_addr, 0, sizeof (struct sockaddr_in));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons (PORTNUM);

    if (!(s_hostent = gethostbyname (argv[1])))
        DEATH ("gethostbyname");

    memcpy (&s_addr.sin_addr, s_hostent->h_addr, s_hostent->h_length);

    /* open multiple  internet tcp stream socket */

    sdvec = malloc (ncons * sizeof (int));
    open_time = open_sockets (s_addr);

    fprintf (stderr, "Opened   %6d connections in %12f seconds,"
             "  now going to fire %d writes\n", ncons, open_time, nwrites);

    write_time = write_random_sockets ();

    fprintf (stderr, "Finished %6d writes      in %12f seconds\n", nwrites,
             write_time);

    close_time = close_sockets ();
    fprintf (stderr, "Closed   %6d sockets     in %12f seconds\n", ncons,
             close_time);
    free (sdvec);

    elapsed_time = open_time + write_time + close_time;
    fprintf (stderr, "             TOTALS: %12f %12f %12f %12f\n",
             open_time, write_time, close_time, elapsed_time);

    exit (EXIT_SUCCESS);
}
