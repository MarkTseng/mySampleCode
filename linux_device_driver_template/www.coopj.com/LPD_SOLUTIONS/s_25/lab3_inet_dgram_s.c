/* **************** LPD:1.0 s_25/lab3_inet_dgram_s.c **************** */
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
 * Using sendto() and recvfrom() for datagrams (INET, SERVER)
 *
 * Now provide solutions for both unix domain and Internet sockets
 * that work with sendto() and recvfrom, and test them as usual.
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
    socklen_t addr_len = sizeof (struct sockaddr_in);
    char message[MSG_LEN];

    addr_len = sizeof (addr);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    addr.sin_port = htons (PORT_NUMBER);

    sd = socket (PF_INET, SOCK_DGRAM, 0);

    setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes));
    bind (sd, (struct sockaddr *)&addr, sizeof (addr));

    for (;;) {

        printf ("\nAccepting input on port %d\n", PORT_NUMBER);

        rc = recvfrom (sd, message, sizeof (message), 0,
                       (struct sockaddr *)&addr, &addr_len);
        write (STDOUT_FILENO, message, rc);
        sendto (sd, message, rc, 0, (struct sockaddr *)&addr, sizeof (addr));

        printf ("Received the end of input\n\n");
    }

    close (sd);
    exit (EXIT_SUCCESS);
}
