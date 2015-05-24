/* **************** LPD:1.0 s_24/lab2_inet_udp_client.c **************** */
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
 * Internet Client/Server with UDP (CLIENT)
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define MSG_LEN 1024
#define PORT_NUMBER 7177

int main (int argc, char **argv)
{
    int sd, msg_len;
    char message[MSG_LEN];
    struct sockaddr_in addr;
    struct hostent *hostent;

    sd = socket (PF_INET, SOCK_DGRAM, 0);

    hostent = gethostbyname (argv[1]);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT_NUMBER);
    memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);

    connect (sd, (struct sockaddr *)&addr, sizeof (addr));

    msg_len = strlen (fgets (message, MSG_LEN, stdin));
    write (sd, message, msg_len);

    memset (message, 0, MSG_LEN);
    msg_len = read (sd, message, MSG_LEN);
    write (STDOUT_FILENO, message, msg_len);

    close (sd);
    exit (0);
}
