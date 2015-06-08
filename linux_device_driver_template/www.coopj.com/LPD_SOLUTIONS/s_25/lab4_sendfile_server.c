/* **************** LPD:1.0 s_25/lab4_sendfile_server.c **************** */
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
 * Using sendfile() (SERVER)
 *
 * Generate an input file by whatever method you choose.  We will then
 * send this data across the network using and not using sendfile and
 * compare results.
 *
 * You are provided with a server program to receive the data as
 * supplied by clients with and without sendfile().
 *
 * The client programs take as arguments the input file name, the
 * server machine name argument, the number of chunks the I/O should
 * be done in, and the number of times the whole operation should
 * be repeated in order to produce better averaged timing information.
 *
 * Time the results with sendfile() on and off and compare.
 *
 * You'll probably want to do this several times, making sure your
 * system is lightly loaded. The time command will report on real (or
 * wall clock), user, and system time. The system time may not be
 * reliable, so to be rough, just concentrate on the wall clock.
 *
 * If you want to get fancier you can put timing instrumentation in
 * the code using gettimeofday() or a similar function.
 *
 * Note: It is quite possible your timings will be limited by disk I/O
 * and not the network transmission. In order to make sure this is not
 * the case, work with a ram disk. You can do this by:
 *
 *      mkdir ramdisk
 *      mount -t tmpfs none ./ramdisk
 * and then make sure your data file is on the ramdisk.  (Even easier,
 * you can just place your files in /dev/shm which sould
 * already be mounted.)
 @*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_LEN 64*1024         /* limited by socket buffer size anyway */
#define PORT_NUMBER 7177

int main (void)
{
    struct sockaddr_in addr, con_addr;
    int rc, sd, cd, yes = 1, fd2, bytes_received;
    socklen_t alen = sizeof (struct sockaddr_in);
    char message[MSG_LEN];

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    addr.sin_port = htons (PORT_NUMBER);

    sd = socket (PF_INET, SOCK_STREAM, 0);

    setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes));
    bind (sd, (struct sockaddr *)&addr, sizeof (addr));
    listen (sd, 5);

    for (;;) {

        /* reopen the outfile each time so it doesn't grow large */

        cd = accept (sd, (struct sockaddr *)&con_addr, &alen);
        bytes_received = 0;

        /* reopen the outfile each time so it doesn't grow large */
        fd2 = open ("outfile", O_RDWR | O_CREAT | O_TRUNC, 0666);
        printf ("\nAccepting input on port %d\n", PORT_NUMBER);

        do {
            rc = recv (cd, message, sizeof (message), 0);
            bytes_received += rc;
            write (fd2, message, rc);
        } while (rc > 0);

        printf ("Received:  %d bytes\n", bytes_received);
        printf ("Received the end of input\n\n");
        close (cd);
        close (fd2);
    }

    close (sd);

    exit (EXIT_SUCCESS);
}
