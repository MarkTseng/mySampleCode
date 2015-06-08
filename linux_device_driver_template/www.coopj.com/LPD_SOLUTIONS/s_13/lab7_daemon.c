/* **************** LPD:1.0 s_13/lab7_daemon.c **************** */
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
/* Lab: Daemons
 * 
 * Write a simple program that starts a daemon process.  It can be as
 * simple as just calling pause() to go to sleep until a signal is
 * received.
 * 
 * Once the program is running, type ps lx to verify it is running in
 * background and that its parent process is init
 * 
 * Send a signal to the daemon to interrupt the sleep; for instance
 * one could do kill -SIGUSR1 &lt;pid&gt;.  The process should
 * disappear.
 @*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main ()
{
    daemon (0, 0);
    pause ();
    printf ("got out of the pause\n");
    exit (0);
}
