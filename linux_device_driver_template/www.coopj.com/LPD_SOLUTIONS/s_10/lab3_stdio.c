/* **************** LPD:1.0 s_10/lab3_stdio.c **************** */
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
 * Using the Standard I/O Library
 *
 * Inplement the previous lab using the standard I/O library instead
 * of Unix I/O to see the differences.
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define FILE_NAME_DEFAULT "/etc/passwd"
#define BYTES_PER_LINE  16

#define DEATH(mess) { perror(mess); exit(errno); }

int main (int argc, char *argv[])
{
    FILE *fp;
    int i = 0, start_addr = 0;
    char c;
    char *filename = FILE_NAME_DEFAULT;

    if (argc > 1)
        filename = argv[1];

    if (argc > 2)
        start_addr = atoi (argv[2]);

    printf
        ("\n **** DISPLAYING (IN HEX) FILE: %s at offset = %d (=0x%x) ****\n\n",
         filename, start_addr, start_addr);

    if (!(fp = fopen (filename, "r")))
        DEATH (filename);

    if (-1 == fseek (fp, start_addr, SEEK_SET))
        DEATH (filename);

    printf ("starting at 0x%x:\n", start_addr);

    while (!feof (fp)) {
        if (fread (&c, 1, 1, fp) < 0)
            DEATH (filename);
        printf ("%2.2x ", c);
        if (i % BYTES_PER_LINE == 0)
            printf ("\n");
        i++;
    }
    printf ("\n");
    fclose (fp);
    exit (0);
}
