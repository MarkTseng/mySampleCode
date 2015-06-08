/* **************** LPD:1.0 s_25/lab5_socketpair.c **************** */
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
 * Using socketpair() on a socket
 *
 * Using socketpair() write a simple program that creates a pair of
 * connected sockets.
 *
 * Write an identifying message into each socket, clear it, and then
 * read from the socket.
 *
 * Print out what is sent to and read from the sockets.  Are you
 * getting full duplex operation?
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#define STR_LEN 128

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    int j, rc, sv[2];
    char string[2][STR_LEN];

    if (socketpair (PF_LOCAL, SOCK_STREAM, 0, sv))
        DEATH ("Failed to make a socket pair");

    printf ("\n");
    for (j = 0; j < 2; j++) {
        sprintf (string[j], "This is the message going out on sv[%d]", j);
        printf ("Initial string %d is:          %s\n", j, string[j]);
        rc = write (sv[j], string[j], strlen (string[j]));
        printf ("     rc from write on %d is %d\n", j, rc);
        memset (&string[j], 0, STR_LEN);
    }

    printf ("\n");
    for (j = 0; j < 2; j++)
        printf ("After clear, string %d is:     %s\n", j, string[j]);

    printf ("\n");
    for (j = 0; j < 2; j++) {
        rc = read (sv[j], string[j], STR_LEN);
        printf ("     rc from read  on %d is %d\n", j, rc);
        printf ("After read back, string %d is: %s\n", j, string[j]);
        close (sv[j]);
    }
    printf ("\n");

    exit (EXIT_SUCCESS);
}
