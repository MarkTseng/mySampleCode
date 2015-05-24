/* **************** LPD:1.0 s_06/memprofexamp.c **************** */
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
#include <malloc.h>

void doit (void)
{
    char *buf;
    buf = malloc (1000);
}

int main (int argc, char *argv[])
{
    int j;
    for (j = 0; j < 10; j++) {
        doit ();
        printf ("I allocated the %3d buffer\n", j);
    }
    exit (0);
}
