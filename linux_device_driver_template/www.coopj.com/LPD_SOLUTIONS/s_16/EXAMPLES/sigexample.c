/* **************** LPD:1.0 s_16/sigexample.c **************** */
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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void ouch (int sig)
{
    printf ("OUCH! - I got a signal %d\n", sig);
    signal (sig, SIG_DFL);
}

int main ()
{
    signal (SIGINT, ouch);
    while (1) {
        printf ("Hello World!\n");
        sleep (1);
    }
}
