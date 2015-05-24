/* **************** LPD:1.0 s_12/lab1_limit.c **************** */
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
 * Getting and Setting Limits

 * Write a user-space program that examines and sets usage limits, and
 * reports statistics on total usage.

 * The program should first obtain and print out the current usage
 * limits.

 * It should then modify one or more of them, and then print out the
 * new limits.

 * The program should give birth to several children using fork(),
 * each of which should print out their usage limits, compare to those
 * of the parent and then sleep for a while.  The parent should wait
 * for the children using wait() or waitpid().

 * Have the parent use the getrusage() function to obtain statistics
 * both for itself and for the children.

@*/

#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

void do_limit (int limit, const char *limit_string, struct rlimit *rlim)
{
    if (getrlimit (limit, rlim))
        fprintf (stderr, "Failed in getrlimit\n");
    printf ("%15s=%2d: cur=%20lu,     max=%20lu\n", limit_string,
            limit, rlim->rlim_cur, rlim->rlim_max);
}

void print_limits (void)
{
    struct rlimit rlim;
    do_limit (RLIMIT_CPU, "RLIMIT_CPU", &rlim);
    do_limit (RLIMIT_FSIZE, "RLMIT_FSIZE", &rlim);
    do_limit (RLIMIT_DATA, "RLMIT_DATA", &rlim);
    do_limit (RLIMIT_STACK, "RLIMIT_STACK", &rlim);
    do_limit (RLIMIT_CORE, "RLIMIT_CORE", &rlim);
    do_limit (RLIMIT_RSS, "RLIMIT_RSS", &rlim);
    do_limit (RLIMIT_NPROC, "RLIMIT_NPROC", &rlim);
    do_limit (RLIMIT_NOFILE, "RLIMIT_NOFILE", &rlim);
    do_limit (RLIMIT_MEMLOCK, "RLIMIT_MEMLOCK", &rlim);
    do_limit (RLIMIT_AS, "RLIMIT_AS", &rlim);
    do_limit (RLIMIT_LOCKS, "RLIMIT_LOCKS", &rlim);
}

void print_rusage (int who)
{
    struct rusage usage;
    if (getrusage (who, &usage))
        DEATH ("getrusage failed");

    if (who == RUSAGE_SELF)
        printf ("For RUSAGE_SELF\n");
    if (who == RUSAGE_CHILDREN)
        printf ("\nFor RUSAGE_CHILDREN\n");

    printf
        ("ru_utime.tv_sec, ru_utime.tv_usec = %4d  %4d (user time used)\n",
         (int)usage.ru_utime.tv_sec, (int)usage.ru_utime.tv_usec);
    printf
        ("ru_stime.tv_sec, ru_stime.tv_usec = %4d  %4d (system time used)\n",
         (int)usage.ru_stime.tv_sec, (int)usage.ru_stime.tv_usec);
    printf ("ru_maxrss =  %4ld (max resident set size)\n", usage.ru_maxrss);
    printf ("ru_ixrss =   %4ld (integral shared memory size)\n",
            usage.ru_ixrss);
    printf ("ru_idrss =   %4ld (integral unshared data size)\n",
            usage.ru_idrss);
    printf ("ru_isrss =   %4ld (integral unshared stack size)\n",
            usage.ru_isrss);
    printf ("ru_minflt =  %4ld (page reclaims)\n", usage.ru_minflt);
    printf ("ru_majflt =  %4ld (page faults)\n", usage.ru_majflt);
    printf ("ru_nswap =   %4ld (swaps)\n", usage.ru_nswap);
    printf ("ru_inblock = %4ld (block input operations)\n", usage.ru_inblock);
    printf ("ru_oublock = %4ld (block output operations)\n", usage.ru_oublock);
    printf ("ru_msgsnd =  %4ld (messages sent)\n", usage.ru_msgsnd);
    printf ("ru_msgrcv =  %4ld (messages received)\n", usage.ru_msgrcv);
    printf ("ru_nsignals= %4ld (signals received)\n", usage.ru_nsignals);
    printf ("ru_nvcsw=    %4ld (voluntary context switches)\n", usage.ru_nvcsw);
    printf ("ru_nivcsw=   %4ld (involuntary context switches)\n",
            usage.ru_nivcsw);
}

int main (int argc, char *argv[])
{
    struct rlimit rlim;
    pid_t pid = 0;
    int status = 0, nchildren = 3, i;

    /* Print out all limits */

    printf ("Printing out all limits for pid=%d:\n", getpid ());
    print_limits ();

    /* change and printout the limit for core file size */

    printf ("\nBefore Modification, this is RLIMIT_CORE:\n");
    do_limit (RLIMIT_CORE, "RLIMIT_CORE", &rlim);
    rlim.rlim_cur = 8 * 1024 * 1024;
    printf ("I forked off a child with pid = %d\n", (int)pid);

    setrlimit (RLIMIT_CORE, &rlim);
    printf ("\nAfter  Modification, this is RLIMIT_CORE:\n");
    do_limit (RLIMIT_CORE, "RLIMIT_CORE", &rlim);

    /* fork off the nchildren */

    fflush (stdout);
    for (i = 0; i < nchildren; i++) {
        pid = fork ();
        if (pid < 0)
            DEATH ("Failed in fork");
        if (pid == 0) {         /* any child */
            printf ("\nIn child pid= %d this is RLIMIT_CORE:\n",
                    (int)getpid ());
            do_limit (RLIMIT_CORE, "RLIMIT_CORE", &rlim);
            fflush (stdout);
            sleep (3);
            exit (0);
        }
    }

    while (pid > 0) {           /* parent */
        pid = wait (&status);
        printf ("Parent got return on pid=%dn\n", (int)pid);
    }

    printf (" **************************************************** \n");
    print_rusage (RUSAGE_SELF);
    print_rusage (RUSAGE_CHILDREN);

    exit (0);
}
