/* **************** LPD:1.0 s_11/lab4_lock.c **************** */
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
 * File locking
 *
 * Write a program to examine file locking. Arguments may be specified
 * on the command line or from an input file, which say whether the
 * lock should be exclusive or not, whether it should be read or
 * write, and what region of the file (or the whole file) it should
 * apply to.  Run more than one instance of the program simultaneously
 * to examine what happens.
 @*/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

struct flock mylock;

int main (int argc, char *argv[])
{
    int fd, cmd, flags, rc;

    pid_t pid;
    char *filename, *which;

    flags = O_RDWR;
    pid = getpid ();
    mylock.l_type = F_RDLCK;

    if (argc < 3) {
        printf ("USAGE: %s filename {r:w}\n", argv[0]);
        exit (-1);
    }
    filename = argv[1];
    which = argv[2];

    if ((fd = open (filename, flags)) < 0)
        DEATH (filename);

    printf ("\nPID=%d, Opening: %s, with fd = %d\n", pid, filename, fd);

    cmd = F_GETLK;
    rc = fcntl (fd, cmd, &mylock);
    printf ("\nPID=%d, F_GETLK returns rc=%d:\n", pid, rc);
    printf
        ("     l_type=%d, l_whence=%d, l_start=%d, l_len=%d, l_pid=%d\n\n",
         mylock.l_type, mylock.l_whence, (int)mylock.l_start,
         (int)mylock.l_len, mylock.l_pid);

    cmd = F_SETLK;

    if (!strncmp (which, "r", 1)) {
        mylock.l_type = F_RDLCK;
        printf ("PID=%d, doing F_RDLCK\n", pid);
    } else {
        mylock.l_type = F_WRLCK;
        printf ("PID=%d, doing F_WRLCK\n", pid);
    }

    rc = fcntl (fd, cmd, &mylock);
    mylock.l_len = 0;
    printf ("PID=%d, F_SETLK returns rc=%d:\n", pid, rc);
    printf
        ("     l_type=%d, l_whence=%d, l_start=%d, l_len=%d, l_pid=%d\n\n",
         mylock.l_type, mylock.l_whence, (int)mylock.l_start,
         (int)mylock.l_len, mylock.l_pid);

    sleep (6);
    printf (" PID=%d is done sleeping 6 seconds \n", pid);
    exit (0);

}
