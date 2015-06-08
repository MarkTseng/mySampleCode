/* **************** LPD:1.0 s_22/lab2_serv_port.c **************** */
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
 * Getting Services

 * Write a program that takes as arguments a service and a port
 * number; e.g., you can test your program with something like:
 *
 *      lab2 finger 80
 *
 * (Note: The service and port number need not agree.)
 *
 * Use getservbyname() to find out what port number is bound to the
 * service.  Do this for both TCP and UDP.
 *
 * Use getservbyport() to find out what service is bound to the port
 * number.  Do this for both TCP and UDP.
 *
 * In both cases, print out any aliases for the service name.
 *
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define DEATH(mess) { perror(mess); exit(errno); }

void printit (struct servent *s);

int main (int argc, char *argv[])
{
    struct servent *result;
    char proto[4];
    short inport;
    int k;

    if (argc < 3) {
        printf ("\nUSAGE: %s Service PortNumber\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    inport = (short)atoi (argv[2]);

    printf ("\nChecking things out for service = %s, port = %d\n\n",
            argv[1], inport);

    strcpy (proto, "tcp");

    for (k = 0; k < 2; k++) {
        if (k > 0)
            strcpy (proto, "udp");

        printf ("\n     Doing getservbyname for proto = %s\n\n", proto);
        if (!(result = getservbyname (argv[1], proto)))
            DEATH ("getservbyname");

        printit (result);

        printf ("\n     Doing getservbyport for proto = %s\n\n", proto);

        if (!(result = getservbyport (htons (inport), proto)))
            DEATH ("getservbyport");

        printit (result);
    }
    exit (EXIT_SUCCESS);
}

void printit (struct servent *s)
{
    int j;
    char **tmp;
    printf ("name = %15s,    port = %5d,    proto = %4s\n",
            s->s_name, ntohs (s->s_port), s->s_proto);

    if (*s->s_aliases) {
        for (j = 0, tmp = s->s_aliases; *tmp != NULL; j++, tmp++)
            printf ("  aliases[%d] = %s ", j, *tmp);
        printf ("\n");
    }
}
