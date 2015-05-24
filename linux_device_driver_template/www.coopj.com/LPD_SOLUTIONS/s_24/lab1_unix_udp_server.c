/* **************** LPD:1.0 s_24/lab1_unix_udp_server.c **************** */
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
 * Unix Client/Server with UDP (SERVER)
 *
 * Take the examples given for simple Unix domain socket TCP client
 * and server programs (which can be found in the SOLUTIONS/EXAMPLES
 * subdirectory tree) and convert them to work with UDP.
 *
 * Once the server is running, any line of input typed to the client
 * is merely echoed back by the server, and then the client
 * terminates.
 *
 * Test this by starting the server in one window, and then running
 * the client in another.  Try this first with the unaltered TCP
 * programs, and then move on to your modified UDP ones.
 *
 * Now test by starting two instances of the client, and then sending
 * output first from the second one started, and then from the first
 * one.  In what order does the server get the input?
 *
 * Do the same test with the TCP server examples.  Are the results the
 * same, or is the order inverted?  Explain.
 *
 @*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MSG_LEN 1024

int main (void)
{
    struct sockaddr_un uaddr;
    int rc, sd;
    char message[MSG_LEN];

    uaddr.sun_family = AF_UNIX;
    strcpy (uaddr.sun_path, "/tmp/mysock");

    sd = socket (PF_UNIX, SOCK_DGRAM, 0);
    unlink ("/tmp/mysock");
    bind (sd, (struct sockaddr *)&uaddr, sizeof (uaddr));

    for (;;) {
        rc = read (sd, message, sizeof (message));
        write (STDOUT_FILENO, message, rc);
    }

    close (sd);
    exit (EXIT_SUCCESS);
}
