/* **************** LPD:1.0 s_24/lab2_inet_udp_server.c **************** */
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
 * Internet Client/Server with UDP (SERVER)
 *
 * Now the take the example Internet client and server programs and
 * convert them from TCP to UDP.  Test in the same way you did for the
 * unix domain socket programs.
 *
 * Can your client program read back from the socket what it wrote
 * to it?  Try tracing your client with
 *
 *         strace client localhost
 *
 * and try to see in detail what is happening.
 *
 @*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_LEN 1024
#define PORT_NUMBER 7177

int main (void)
{
    struct sockaddr_in addr;
    int rc, sd, yes = 1;
    char message[MSG_LEN];

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    addr.sin_port = htons (PORT_NUMBER);

    sd = socket (PF_INET, SOCK_DGRAM, 0);

    setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes));
    bind (sd, (struct sockaddr *)&addr, sizeof (addr));

    for (;;) {

        rc = read (sd, message, sizeof (message));
        write (STDOUT_FILENO, message, rc);
        write (sd, message, rc);

        printf ("Received the end of input\n\n");
    }

    close (sd);
    exit (EXIT_SUCCESS);
}
