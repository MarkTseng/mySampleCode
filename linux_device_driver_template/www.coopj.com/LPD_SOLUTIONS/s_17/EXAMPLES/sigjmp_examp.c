/* **************** LPD:1.0 s_17/sigjmp_examp.c **************** */
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
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>

#define DEATH(mess) { perror(mess); exit(errno); }

sigjmp_buf senv;

void sig_int (int what)
{
    static int counter = 0;
    printf ("We have received SIGINT\n");
    siglongjmp (senv, ++counter);
}
int main (int argc, char *argv[])
{
    int savesigs = 1;
    struct sigaction act;
    memset (&act, 0, sizeof (act));
    act.sa_handler = sig_int;
    if (sigaction (SIGINT, &act, NULL) < 0) /* for CTRL-C */
        DEATH ("sigaction");
    printf ("Successfully installed signal handler for SIGINT\n");
    printf ("returning from setjmp/longjmp, rc=%d\n",
            sigsetjmp (senv, savesigs));
    /* hit Ctl-C while the sleep is proceeding */
    sleep (10);
    DEATH ("returned to the wrong place!\n");
}
