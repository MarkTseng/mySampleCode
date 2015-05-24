/* **************** LPD:1.0 s_14/pipe1.c **************** */
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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main ()
{
    int rc, filep[2], length;
    char *in_data = "pdat";
    char *out_data;
    length = strlen (in_data) + 1;
    out_data = malloc (length);
    memset (out_data, 0, length);

    if (pipe (filep) == 0) {
        rc = write (filep[1], in_data, 4);
        printf ("wrote %d bytes\n", rc);
        rc = read (filep[0], out_data, 4);
        printf ("read %d bytes which were %s\n", rc, out_data);
        exit (EXIT_SUCCESS);
    }
    exit (EXIT_FAILURE);
}
