/* **************** LPD:1.0 s_23/lab1_echo_client.c **************** */
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
 * Simple Internet echo Client
 *
 * Take the sample Internet echo client program, and make it more
 * robust, checking for errors at each stage, such as creating the
 * socket, connecting to it, and reading and writing from it.
 *
 * Try it first on your local machine. (Make sure the echo server is
 * on with chkconfig --list | grep echo)
 *
 * To work with other machines you may also have to open up your
 * firewall to the echo port.
 *
 * Once it is turned it on you should be able to run it with:
 *
 * lab1_echo_client localhost
 *
 * Try running it on any other Internet address you think the echo
 * server might be running on. What happens if it is not?
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
#define PORT_NUMBER 7
#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char **argv)
{
    int sd, msg_len;
    char message[MSG_LEN];
    struct sockaddr_in addr;
    struct hostent *hostent;

    if (argc < 2) {
        printf ("USAGE: %s host\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    if ((sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
        DEATH (" Can't open a socket");

    printf ("Opened the socket just fine\n");

    if (!(hostent = gethostbyname (argv[1])))
        DEATH ("Can't get host");

    printf ("Got the host just fine\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT_NUMBER);
    memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);

    if (connect (sd, (struct sockaddr *)&addr, sizeof (addr)))
        DEATH ("Can't connect to host");

    printf ("Connected to the host just fine\n");

    printf ("\nType in a message to send to %s\n\n", argv[1]);

    msg_len = strlen (fgets (message, MSG_LEN, stdin));
    if (write (sd, message, msg_len) < 0)
        DEATH ("Failed writing to the socket");

    memset (message, 0, MSG_LEN);
    printf
        ("\nCleared the message, am going to read it back from the socket\n\n");
    msg_len = read (sd, message, MSG_LEN);

    if (msg_len < 0)
        DEATH ("Failed reading from the socket");

    write (STDOUT_FILENO, message, msg_len);

    close (sd);
    exit (0);
}
