/* **************** LPD:1.0 s_10/simplecp.c **************** */
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
#include <fcntl.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int nb, fdin, fdout;
    char buf[4096];
    fdin = open (argv[1], O_RDONLY);
    fdout = open (argv[2], O_RDWR | O_CREAT | O_TRUNC, 0666);
    while ((nb = read (fdin, &buf, 4096)) > 0)
        write (fdout, &buf, nb);
    close (fdin);
    close (fdout);
    exit (0);
}
