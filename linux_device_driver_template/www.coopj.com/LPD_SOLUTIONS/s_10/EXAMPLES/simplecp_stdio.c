/* **************** LPD:1.0 s_10/simplecp_stdio.c **************** */
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
#include <stdlib.h>
#include <stdio.h>

int main (int argc, char *argv[])
{
    FILE *fpin, *fpout;
    char c;
    fpin = fopen (argv[1], "r");
    fpout = fopen (argv[2], "w+");
    while (fread (&c, 1, 1, fpin) > 0)
        fwrite (&c, 1, 1, fpout);
    fclose (fpin);
    fclose (fpout);
    exit (0);
}
