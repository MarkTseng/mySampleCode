/* **************** LPD:1.0 s_25/lab1_unix_stream_c.c **************** */
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
 * Using send() and recv() (Unix Client)
 *
 * Take the examples given for simple Unix domain socket stream client
 * and server programs and convert them to work with send() and recv.
 *
 * Test them as usual.
 *
 * This may seem a little more than necessary but we are going
 * to use the codes in the following labs.
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

    sd = socket (PF_UNIX, SOCK_STREAM, 0);
    connect (sd, (struct sockaddr *)&uaddr, sizeof (uaddr));

    msg_len = strlen (fgets (message, MSG_LEN, stdin));
    send (sd, message, msg_len, 0);

    close (sd);
    exit (EXIT_SUCCESS);
}
