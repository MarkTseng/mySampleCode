/* **************** LPD:1.0 s_23/lab2_finger_client.c **************** */
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
 * Simple Internet finger Client
 *
 * Write a client that connects to the finger socket on some host,
 * asks it for information and displays what it returns.
 *
 * You can look in /etc/services to find out the finger socket's port.
 *
 * You might get information on all users or only selected users.
 *
 * A naked carriage return sent to the finger server will return
 * information on all logged in users (if the host will allow it).  A
 * user name followed by a carriage return will return information on
 * that user.
 *
 * The sample solution is written to work with this usage:
 *
 *    lab2_finger_client host user1 user2 ....
 *
 * Try it on the local host, and if you know any Internet sites where
 * finger is still running, try there.
 *
 * If you want to be fancy you can get it to work with the standard
 * finger  syntax:
 *
 *    finger user1@host1 user2@host2
 *    finger @host1 @host2
 *
 * Note: By default the finger server may not be installed, and if it
 * is it may not be enabled.  To take care of both steps, one could do
 * (as root):
 *
 *     yum install finger-server
 *     chkconfig finger on
 *     service xinetd restart
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
#define PORT_NUMBER 79          /* finger port -- see /etc/services */

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char **argv)
{
    int sd, msg_len;
    char message[MSG_LEN] = "";
    struct sockaddr_in addr;
    struct hostent *hostent;

    /* Parse the input arguments. */

    if (argc <= 1) {
        printf ("Usage: %s hostname <user1 user2 ...> \n", argv[0]);
        exit (0);
    }

    if (argc == 2) {
        strcpy (message, "\n");
    } else {
        int i;
        for (i = 2; i < argc; i++) {
            strcat (message, argv[i]);
            strcat (message, " ");
        }
        strcat (message, "\n");
    }

    if (!(hostent = gethostbyname (argv[1])))
        DEATH ("Can't get host");

    printf ("Got the host just fine\n");

    addr.sin_family = AF_INET;
    addr.sin_port = htons (PORT_NUMBER);
    memcpy (&addr.sin_addr, hostent->h_addr, hostent->h_length);

    /* open the socket and connect */

    if ((sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
        DEATH (" Can't open a socket");

    if (connect (sd, (struct sockaddr *)&addr, sizeof (addr)))
        DEATH ("Can't connect to host");

    printf ("Successfully Connected to: %s\n", argv[1]);

    /* send the command */

    if (write (sd, message, strlen (message)) < 0)
        DEATH ("Failed writing to the socket");

    /* clear the message and read back the response */

    memset (message, 0, MSG_LEN);
    msg_len = read (sd, message, MSG_LEN);

    if (msg_len < 0)
        DEATH ("Failed reading from the socket");

    write (STDOUT_FILENO, message, msg_len);

    close (sd);
    exit (EXIT_SUCCESS);
}
