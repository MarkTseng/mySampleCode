/* **************** LPD:1.0 s_21/lab1_address.c **************** */
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
 * Examining Internet Addresses
 *
 * Write a program that takes as an argument an internet address in
 * dot-quad notation, e.g., 127.0.0.1 or 192.168.1.100.
 *
 * The program should convert this to a binary address with inet_aton()
 * and print out the result.
 *
 * The program should convert this to a binary address with inet_addr()
 * and print out the result.
 *
 * The program should invert the result of inet_aton() with
 * inet_ntoa() and make sure the original input comes back.
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main (int argc, char *argv[])
{
    int rc;
    char *here;
    struct in_addr addr;
    in_addr_t addrb;

    if (argc < 2) {
        printf ("\nUSAGE: %s IP address, try again!\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    here = argv[1];
    printf ("\nThe original address is:              %s\n", here);

    if (!(rc = inet_aton (here, &addr))) {
        printf (" inet_aton failed!\n");
        exit (EXIT_FAILURE);
    }
    printf ("\nFrom inet_aton: %x\n", addr.s_addr);

    addrb = inet_addr (here);
    printf ("From inet_addr: %x\n", addrb);

    printf ("\nThe return address from inet_nota is: %s\n", inet_ntoa (addr));

    exit (EXIT_SUCCESS);
}
