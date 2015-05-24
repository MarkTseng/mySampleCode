/* **************** LPD:1.0 s_22/lab3_services.c **************** */
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
 * Getting All Services
 *
 * Write a program that lists all services, together with their port
 * numbers, possible protocols, and aliases, on your local machine.
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

void printit (struct servent *s);

int main (int argc, char *argv[])
{
    struct servent *result;

    setservent (0);

    while ((result = getservent ()))
        printit (result);

    endservent ();

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
