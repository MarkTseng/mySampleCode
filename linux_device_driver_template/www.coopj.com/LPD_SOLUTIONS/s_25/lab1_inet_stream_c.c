/* **************** LPD:1.0 s_25/lab1_inet_stream_c.c **************** */
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
 * Using send() and recv() (Internet Client)
 *
 * Take the examples given for simple unix domain socket stream client
 * and server programs and convert them to work with send() and recv.
 *
 * Test them as usual.
 *
 * This may seem a little more than necessary but we are going
 * to use the codes in the following labs.
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

    sd = socket (PF_INET, SOCK_STREAM, 0);

    hostent = gethostbyname (argv[1]);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT_NUMBER);
    memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);

    connect (sd, (struct sockaddr *)&addr, sizeof (addr));

    msg_len = strlen (fgets (message, MSG_LEN, stdin));
    send (sd, message, msg_len, 0);

    memset (message, 0, MSG_LEN);
    msg_len = recv (sd, message, MSG_LEN, 0);
    write (STDOUT_FILENO, message, msg_len);

    close (sd);
    exit (EXIT_SUCCESS);
}
