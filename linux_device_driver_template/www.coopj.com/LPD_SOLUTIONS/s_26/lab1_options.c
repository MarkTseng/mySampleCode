/* **************** LPD:1.0 s_26/lab1_options.c **************** */
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
 * Examining socket options
 *
 * Write a program that uses getsockopt() to examine the default
 * options for a TCP/IP socket.
 *
 * Just open a socket (you don't have to do anything with it, like
 * connecting), and then using the option names available in
 *
 * /usr/src/linux/include/arch/x86/include/asm/socket.h, obtain their values.
 *
 * Note that most of the options have an integer value, but some are
 * not, such as those which are given in a timeval data structure.
 *
 * Print out the symbolic name along with the integer option number.
 *
 * For the above you'll use SOL_SOCKET for the level parameter.  You
 * can also try other values such as SOL_IP.  Look at the man pages
 * for socket(7), tcp(7), ip(7), unix(7), etc.  to get some ideas.
 *
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEATH(mess) { perror(mess); exit(errno); }

void doit_int (int sd, int optname, char *name)
{
    socklen_t optlen = sizeof (int);
    int val;
    if (getsockopt (sd, SOL_SOCKET, optname, &val, &optlen))
        fprintf (stderr, "getsockopt failed for: %32s (%2d)", name, optname);
    else
        printf ("%15s (%2d)  optlen=%d  value:%6d\n", name, optname,
                optlen, val);
}

void doit_timeval (int sd, int optname, char *name)
{
    socklen_t optlen = sizeof (struct timeval);
    struct timeval val;
    if (getsockopt (sd, SOL_SOCKET, optname, &val, &optlen))
        fprintf (stderr, "getsockopt failed for: %32s", name);
    else
        printf ("%15s (%2d)  optlen=%d  value: %d secs %d usecs\n", name,
                optname, optlen, (int)val.tv_sec, (int)val.tv_usec);
}

int main (int argc, char **argv)
{
    int sd;

    if ((sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
        DEATH ("Couldn't open socket");

    doit_int (sd, SO_DEBUG, "SO_DEBUG");
    doit_int (sd, SO_REUSEADDR, "SO_REUSEADDR");
    doit_int (sd, SO_TYPE, "SO_TYPE");
    doit_int (sd, SO_ERROR, "SO_ERROR");
    doit_int (sd, SO_DONTROUTE, "SO_DONTROUTE");
    doit_int (sd, SO_BROADCAST, "SO_BROADCAST");
    doit_int (sd, SO_SNDBUF, "SO_SNDBUF");
    doit_int (sd, SO_RCVBUF, "SO_RCVBUF");
    doit_int (sd, SO_KEEPALIVE, "SO_KEEPALIVE");
    doit_int (sd, SO_OOBINLINE, "SO_OOBINLINE");
    doit_int (sd, SO_NO_CHECK, "SO_NO_CHECK");
    doit_int (sd, SO_PRIORITY, "SO_PRIORITY");
    doit_int (sd, SO_LINGER, "SO_LINGER");
    doit_int (sd, SO_BSDCOMPAT, "SO_BSDCOMPAT");
/* doit_int(sd, SO_REUSEPORT,"SO_REUSEPORT"); */
    doit_int (sd, SO_PASSCRED, "SO_PASSCRED");
    doit_int (sd, SO_PEERCRED, "SO_PEERCRED");
    doit_int (sd, SO_RCVLOWAT, "SO_RCVLOWAT");
    doit_int (sd, SO_SNDLOWAT, "SO_SNDLOWAT");
    doit_timeval (sd, SO_RCVTIMEO, "SO_RCVTIMEO");
    doit_timeval (sd, SO_SNDTIMEO, "SO_SNDTIMEO");
    doit_int (sd, SO_SECURITY_AUTHENTICATION, "SO_SECURITY_AUTHENTICATION");
    doit_int (sd, SO_SECURITY_ENCRYPTION_TRANSPORT,
              "SO_SECURITY_ENCRYPTION_TRANSPORT");
    doit_int (sd, SO_SECURITY_ENCRYPTION_NETWORK,
              "SO_SECURITY_ENCRYPTION_NETWORK");
    doit_int (sd, SO_BINDTODEVICE, "SO_BINDTODEVICE");
    doit_int (sd, SO_ATTACH_FILTER, "SO_ATTACH_FILTER");
    doit_int (sd, SO_DETACH_FILTER, "SO_DETACH_FILTER");
    doit_int (sd, SO_PEERNAME, "SO_PEERNAME");
    doit_int (sd, SO_TIMESTAMP, "SO_TIMESTAMP");
    doit_int (sd, SO_ACCEPTCONN, "SO_ACCEPTCONN");

    close (sd);
    exit (0);
}
