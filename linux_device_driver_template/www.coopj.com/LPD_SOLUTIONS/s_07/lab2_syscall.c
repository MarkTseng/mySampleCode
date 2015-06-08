/* **************** LPD:1.0 s_07/lab2_syscall.c **************** */
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
 *: Using syscall().
 * 
 * The indirect system call function:
 *      
 * #include <unistd.h>
 * #include <sys/syscall.h>
 *
 * int syscall (int call_number, .... );
 * 
 * can be used to invoke any system call known to the system. The
 * first argument call_number can be read from the list in
 * /usr/include/bits/syscall.h:
 * ....
 * #define SYS__sysctl __NR__sysctl
 * #define SYS_access __NR_access
 * #define SYS_acct __NR_acct
 * #define SYS_add_key __NR_add_key
 * #define SYS_adjtimex __NR_adjtimex
 * #define SYS_afs_syscall __NR_afs_syscall
 * #define SYS_alarm __NR_alarm
 * #define SYS_brk __NR_brk
 * #define SYS_capget __NR_capget
 * ....
 * 
 * The remaining variable number of arguments (indicated by ....) are
 * the arguments to the actual system call.
 *    
 * This function is not meant to be widely used; it is there for the
 * use of library writers, so don't use it in normal programs unless
 * absolutely necessary.
 *    
 * Write a program that invokes syscall() to run some common system
 * calls. For example you might do:
 *       
 *    pid = syscall(_SYS_getpid);
 * 
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main (int argc, char *argv[])
{
    int rc;
    time_t t;
    unsigned long cwd_size = 1024;
    char cwd[cwd_size];

    rc = syscall (SYS_getpid);
    printf ("rc from getpid= %d\n", rc);

    rc = syscall (SYS_getppid);
    printf ("rc from getppid= %d\n", rc);

    rc = syscall (SYS_time, &t);
    printf ("rc from time= %d\n", rc);
    printf ("t from time= %ld\n", (long)t);

    rc = syscall (SYS_getcwd, &cwd, cwd_size);
    printf ("rc from getcwd= %d\n", rc);
    printf ("getcwd gives = %s\n", cwd);

    exit (EXIT_SUCCESS);
}
