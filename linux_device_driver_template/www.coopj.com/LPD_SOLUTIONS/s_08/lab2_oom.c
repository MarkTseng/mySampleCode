/* **************** LPD:1.0 s_08/lab2_oom.c **************** */
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
 * Lab: Running Out of Memory</SUBTOPIC>
 * 
 * Write a short program that allocates memory in chunks until the
 * system can no longer obtain any more.
 *
 * Eventually the OOM killer should be invoked. Look at
 * /var/log/messages to see what decisions it made about what to
 * processes to exterminate.  
 *
 * Try this with swap turned off and on, which you can do (as root)
 * with swapoff -a and swapon -a.
@*/

#include <stdio.h>
#include <stdlib.h>
#define MB (1024*1024)

int main ()
{
    int j;
    char *buf;
    /* allocate 4 MB at a time */
    for (j = 0;; j++) {
        if (j%100 == 0)
            printf ("%d ", j);
        buf = malloc (MB);
    }
    exit (0);
}
