/* **************** LPD:1.0 s_10/lab5_readv.c **************** */
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
 * Using readv() and writev()
 * 
 * Write a program that uses readv() and writev().
 *     
 * Fill up a series of buffers with distinguishable values and write
 * them to a file.  Then read them back and verify their contents.
@*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/uio.h>

#define FILE_NAME_DEFAULT "/tmp/junkfile"
#define NVECS 4
#define BLEN 1024

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    int fd, j;
    char *filename = FILE_NAME_DEFAULT;
    struct iovec iov[NVECS];

    if (argc > 1)
        filename = argv[1];

    /* initialize NVECS structures and buffers */

    for (j = 0; j < NVECS; j++) {
        char *buf = malloc (BLEN);
        memset (buf, '0' + j, BLEN);
        iov[j].iov_base = buf;
        iov[j].iov_len = BLEN;
    }

    if ((fd = open (filename, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
        DEATH ("open");

    /* write the buffers out to the junkfile */

    if (writev (fd, iov, NVECS) < 0)
        DEATH ("writev");

    /* clear out the buffers */

    for (j = 0; j < NVECS; j++) {
        char *buf = iov[j].iov_base;
        memset (buf, 0, BLEN);
    }

    /* reposition to beginning of the file */

    if (-1 == lseek (fd, 0, SEEK_SET))
        DEATH ("lseek");

    /* read the buffers back from the junkfile */

    if (readv (fd, iov, NVECS) < 0)
        DEATH ("readv");

    /* print them out to make sure it wall ok */

    for (j = 0; j < NVECS; j++) {
        write (STDOUT_FILENO, iov[j].iov_base, BLEN);
        write (STDOUT_FILENO, "\n\n", 2);
        free (iov[j].iov_base); /* avoid memory leaks */
    }

    close (fd);
    exit (0);
}
