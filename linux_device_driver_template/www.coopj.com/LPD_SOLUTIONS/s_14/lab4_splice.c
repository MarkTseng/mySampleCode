/* **************** LPD:1.0 s_14/lab4_splice.c **************** */
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
/* Using splice()
 *
 * Write a program that reads data from an input file and transfers it
 * to an output pipe using splice().
 *
 * By default the program should do it all in one system call. As an
 * optional argument give the number of bytes that should be done in
 * one transfer and loop until the total is consumed.
 *
 * Create a fifo as the output pipe and then run your program in one
 * window, while doing cat < afifo in another. Try starting the cat
 * before and after your program runs.
 *
@*/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    char infile[128] = "./infile", outpipe[128] = "./outpipe";
    int rc, fd_in, fd_out, nbuf = INT_MAX;

    /* 1st arg = infilename        (./infile by default)
       2nd arg = outpipename       (./outpipe by default)
       3rd arg = size of chunks    ( infinite by default)
     */

    if (argc > 1)
        strcpy (infile, argv[1]);
    if (argc > 2)
        strcpy (outpipe, argv[2]);
    if (argc > 3)
        nbuf = atoi (argv[3]);

    printf ("Input file: %s,  Output pipe: %s\n", infile, outpipe);
    fflush (NULL);

    if ((fd_in = open (infile, O_RDONLY)) < 0)
        DEATH ("open fd_in");

    if ((fd_out = open (outpipe, O_WRONLY)) < 0)
        DEATH ("open fd_out");

    do {
        if ((rc = splice (fd_in, NULL, fd_out, NULL, nbuf, 0)) < 0)
            DEATH ("splice");
        printf ("Transferred %8d bytes\n", rc);
    } while (rc > 0);

    close (fd_in);
    close (fd_out);

    printf ("done\n");
    exit (EXIT_SUCCESS);
}
