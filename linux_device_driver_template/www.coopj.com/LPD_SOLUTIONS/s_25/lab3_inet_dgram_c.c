/* **************** LPD:1.0 s_25/lab3_inet_dgram_c.c **************** */
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
 * Using sendto() and recvfrom() for datagrams (INET, CLIENT)
 *
 * Now provide solutions for both unix domain and Internet sockets
 * that work with sendto() and recvfrom, and test them as usual.
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
    int sd, msg_len, rc;
    char message[MSG_LEN];
    struct sockaddr_in addr, from_addr;
    struct hostent *hostent;
    socklen_t alen = sizeof (struct sockaddr_in);

    sd = socket (PF_INET, SOCK_DGRAM, 0);

    hostent = gethostbyname (argv[1]);
    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT_NUMBER);
    memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);

    msg_len = strlen (fgets (message, MSG_LEN, stdin));
    sendto (sd, message, msg_len, 0, (struct sockaddr *)&addr, sizeof (addr));

    memset (message, 0, MSG_LEN);

    rc = recvfrom (sd, message, MSG_LEN, 0, (struct sockaddr *)&from_addr,
                   &alen);
    write (STDOUT_FILENO, message, rc);

    close (sd);
    exit (EXIT_SUCCESS);
}
