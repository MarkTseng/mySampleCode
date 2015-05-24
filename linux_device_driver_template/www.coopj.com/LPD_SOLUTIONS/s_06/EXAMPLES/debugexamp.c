/* **************** LPD:1.0 s_06/debugexamp.c **************** */
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

int main (int argc, char *argv[])
{
    char *buf;
    int size = 800, index = 801;
    if (argc > 1)
        index = atoi (argv[1]);
    if (argc > 2)
        size = atoi (argv[2]);
    buf = malloc (size);

    printf ("I allocated  %d bytes, I'm going to use the %d element\n",
            size, index);
    buf[index] = 1;
    printf ("buf[index] = %d\n", buf[index]);
    exit (0);
}
