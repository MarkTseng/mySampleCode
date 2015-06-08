/* **************** LPD:1.0 s_21/lab2_gethost.c **************** */
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
 * Examining Host Entries
 *
 * Write a program that takes as input an internet address and returns
 * information about its address, obtained with gethostbyname().
 *
 * Some good examples to try would be:
 *
 *     www.us.kernel.org
 *     localhost
 *     www.yahoo.com
 *     coopj.com
 *     google.com
 *
 * For the address the program should print out:
 *
 *     The address name, type, and length.
 *     Any aliases.
 *     The full list of addresses used by the host.
 *
 * i.e., you should dump out the hostent structure.
 *
 * For a little bit extra, print out the addresses in dotted
 * quad notation, without using the helper functions.
 *
 * Do all hosts fill in the aliases array?
 *
 * Compare your results with what you get from typing:
 *
 *    host <address>
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

void print_quad (unsigned int x);

int main (int argc, char *argv[])
{
    struct hostent *hent;
    int j;
    struct in_addr addrh;
    char **tmp;

    if (argc < 2) {
        printf ("\nUSAGE: %s address, try again!\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    printf ("\nGetting information on: %s\n", argv[1]);

    if (!(hent = gethostbyname (argv[1]))) {
        herror ("Failed to get the host name");
        exit (EXIT_FAILURE);
    }

    printf ("\nh_name         = %s\n", hent->h_name);
    printf ("h_addrtype     = %d\n", hent->h_addrtype);
    printf ("h_length       = %d\n\n", hent->h_length);

    if (*hent->h_aliases)
        for (j = 0, tmp = hent->h_aliases; *tmp != NULL; j++, tmp++)
            printf ("h_aliases[%d]   = %s\n", j, *tmp);
    else
        printf ("h_aliases[%d]   = %s\n", 0, "No aliases found!");

    printf ("\n");
    for (j = 0; hent->h_addr_list[j] != NULL; j++) {
        memcpy (&addrh, hent->h_addr_list[j], hent->h_length);
        printf ("h_addr_list[%d] = ", j);
        print_quad (addrh.s_addr);
    }
    printf ("\n");
    exit (EXIT_SUCCESS);
}

/*
   Print out 32 bit network address (in network order) as dotted quad
   note we are going to do a byte swap!
*/

void print_quad (unsigned int x)
{
    unsigned char b1, b2, b3, b4;

    b4 = (x & 0xFF000000) >> 24;
    b3 = (x & 0x00FF0000) >> 16;
    b2 = (x & 0x0000FF00) >> 8;
    b1 = (x & 0x000000FF);
    printf ("%02x %02x %02x %02x", b1, b2, b3, b4);
    printf ("   (%03d.%03d.%03d.%03d)", b1, b2, b3, b4);
    printf ("\n");
}
