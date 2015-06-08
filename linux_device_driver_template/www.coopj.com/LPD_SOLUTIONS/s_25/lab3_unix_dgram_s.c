/* **************** LPD:1.0 s_25/lab3_unix_dgram_s.c **************** */
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
 * Using sendto() and recvfrom() for datagrams (UNIX, SERVER)
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
    struct sockaddr_un addr;
    int rc, sd;
    socklen_t alen = sizeof (struct sockaddr_un);
    char message[MSG_LEN];

    addr.sun_family = AF_UNIX;
    strcpy (addr.sun_path, "/tmp/mysock");

    sd = socket (PF_UNIX, SOCK_DGRAM, 0);
    unlink ("/tmp/mysock");
    bind (sd, (struct sockaddr *)&addr, sizeof (addr));

    for (;;) {
        rc = recvfrom (sd, message, sizeof (message), 0,
                       (struct sockaddr *)&addr, &alen);
        write (STDOUT_FILENO, message, rc);
    }

    close (sd);
    exit (EXIT_SUCCESS);
}
