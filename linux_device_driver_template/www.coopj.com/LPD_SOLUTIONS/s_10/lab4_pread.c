/* **************** LPD:1.0 s_10/lab4_pread.c **************** */
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
 * Using pread() and pwrite()
 *     
 * Write a program that uses pread() and pwrite(). Obtain the offset
 * within the file using lseek() after each I/O operation and verify
 * that it does not change.
 *    
 * Using pwrite() fill up a file with distinguishable values depending
 * on position. Then read them back and verify their contents.
@*/
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define FILE_NAME_DEFAULT "/tmp/junkfile"
#define NVECS 4
#define BLEN 1024

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    int fd, j;
    char *filename = FILE_NAME_DEFAULT;
    off_t pos;
    char *buf;

    if (argc > 1)
        filename = argv[1];

    buf = malloc (BLEN);

    if ((fd = open (filename, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0)
        DEATH ("open");

    /* write to the junkfile */

    for (j = 0; j < NVECS; j++) {
        memset (buf, '0' + j, BLEN);
        pos = j * BLEN;
        printf ("Going to write at pos = %d\n", (int)pos);
        pwrite (fd, buf, BLEN, pos);
        pos = lseek (fd, 0, SEEK_CUR);
        printf ("After the %d'th write, the position is %d\n", j, (int)pos);
    }

    /* read from the junkfile */

    for (j = 0; j < NVECS; j++) {
        memset (buf, 0, BLEN);
        pos = j * BLEN;
        printf ("Going to read at pos = %d\n", (int)pos);
        pread (fd, buf, BLEN, pos);
        pos = lseek (fd, 0, SEEK_CUR);
        printf ("After the %d'th read, the position is %d\n", j, (int)pos);
        write (STDOUT_FILENO, buf, BLEN);
        write (STDOUT_FILENO, "\n\n", 2);
    }

    free (buf);
    close (fd);
    exit (0);
}
