/* **************** LPD:1.0 s_12/lab2_pgid.c **************** */
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
 * Process Groups
 *
 * Write a program that first prints out its process group using both
 * getpgid() and getpgrp().
 *          
 * Then feeding as an argument to the program the process ID of a
 * currently running process (you could simply do cat & and use the
 * echo'ed process ID as an argument) print out the pgid of the
 * process.
 *              
 * Now try to change it with setpgid() to be the same as that of your
 * program.  Read the man page to understand your results.
 *
 @*/

#define _XOPEN_SOURCE 500

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

int main (int argc, char *argv[])
{
    pid_t mypid, pid;
    int rc;

    /* print information about myself */

    mypid = getpid ();
    printf ("pgroup from getpgid(pid) = %d\n", getpgid (mypid));
    printf ("pgroup from getpgrp() = %d\n", getpgrp ());

    if (argc < 2)
        exit (EXIT_FAILURE);
    if ((pid = atoi (argv[1])) < 0) {
        printf (" give a running pid as an argument\n");
        exit (EXIT_FAILURE);
    }
    printf ("pgroup for pid=%d = %d\n", pid, getpgid (pid));

    /* try to change the pgroup to my group */

    rc = setpgid (pid, mypid);
    printf ("rc from setpgid(pid,mypid)= %d\n", rc);
    printf ("pgroup for pid=%d = %d\n", pid, getpgid (pid));
    exit (EXIT_SUCCESS);
}
