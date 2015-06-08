/* **************** LPD:1.0 s_11/lab2_mmap.c **************** */
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
 * Memory Mapping
 *
 * Write a program that uses mmap to access a file.
 *
 * Have the program both read and write to the file using memory
 * access rather than read/write functions.  (Make sure you work on a
 * file you don't care about!)
 @*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char **argv)
{
    int fd, size, rc, j;
    char *area, *tmp;
    struct stat filestat;
    char c[2] = "CX";

    if ((fd = open (argv[1], O_RDWR)) < 0)
        DEATH ("problems opening the file");

    if (fstat (fd, &filestat))
        DEATH (" problems fstating the file:");

    size = filestat.st_size;
    printf (" Memory Mapping File: %s, of size %d bytes\n", argv[1], size);

    area = mmap (NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    if (area == MAP_FAILED)
        DEATH ("error mmaping");

    /* can close the file now */

    close (fd);

    /* put the string repeatedly in the file */

    tmp = area;
    for (j = 0; j < size - 1; j += 2, tmp += 2)
        memcpy (tmp, &c, 2);

    /* just cat out the file to see if it worked */

    rc = write (STDOUT_FILENO, area, size);

    if (rc != size)
        DEATH ("problems writing");

    exit (0);
}
