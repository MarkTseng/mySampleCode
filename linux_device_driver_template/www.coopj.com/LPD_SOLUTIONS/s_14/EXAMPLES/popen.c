/* **************** LPD:1.0 s_14/popen.c **************** */
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
    FILE *fp;
    int nbytes, BSIZE=getpagesize();
    char *buffer = malloc(BSIZE);
    fp = popen ("finger", "r");
    while ((nbytes = fread (buffer, 1, BSIZE, fp)) > 0)
        fwrite (buffer, 1, nbytes, stdout);
    printf ("EOF = %d and Error = %d\n", feof (fp), ferror (fp));
    pclose (fp);
    exit (EXIT_SUCCESS);
}

