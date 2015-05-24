/* **************** LPD:1.0 s_10/lab2_hexedit_offset.c **************** */
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
 * Files With Offset
 *
 * Modify the previous lab to start that hex display at a file offset
 * specified as a parameter.
 @*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define FILE_NAME_DEFAULT "/etc/passwd"
#define BYTES_PER_LINE  16

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    int fd, bytes_read, i, start_addr = 0;
    unsigned char line[BYTES_PER_LINE];
    char *filename = FILE_NAME_DEFAULT;

    if (argc > 1)
        filename = argv[1];

    if (argc > 2)
        start_addr = atoi (argv[2]);

    printf
        ("\n **** DISPLAYING (IN HEX) FILE: %s at offset = %d (=0x%x) ****\n\n",
         filename, start_addr, start_addr);

    if ((fd = open (filename, O_RDONLY)) < 0)
        DEATH (filename);

    if (-1 == lseek (fd, start_addr, SEEK_SET))
        DEATH (filename);

    printf ("starting at 0x%x:\n", start_addr);

    do {
        if ((bytes_read = read (fd, line, BYTES_PER_LINE)) < 0)
            DEATH (filename);

        for (i = 0; i < bytes_read; i++)
            printf ("%2.2x ", line[i]);

        printf ("\n");
    }
    while (bytes_read > 0);

    close (fd);
    exit (0);
}
