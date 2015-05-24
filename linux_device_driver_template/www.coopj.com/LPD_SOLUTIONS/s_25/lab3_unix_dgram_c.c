/* **************** LPD:1.0 s_25/lab3_unix_dgram_c.c **************** */
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
 * Using sendto() and recvfrom() for datagrams (UNIX, CLIENT)
 *
 * Now provide solutions for both Unix domain and Internet sockets
 * that work with sendto() and recvfrom, and test them as usual.
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
    int msg_len, sd;
    char message[MSG_LEN];

    uaddr.sun_family = AF_UNIX;
    strcpy (uaddr.sun_path, "/tmp/mysock");

    sd = socket (PF_UNIX, SOCK_DGRAM, 0);

    msg_len = strlen (fgets (message, MSG_LEN, stdin));
    sendto (sd, message, msg_len, 0, (struct sockaddr *)&uaddr, sizeof (uaddr));

    close (sd);
    exit (EXIT_SUCCESS);
}
