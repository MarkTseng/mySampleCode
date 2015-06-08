/* **************** LPD:1.0 s_11/lab5_tmp.c **************** */
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
/* Making Temporary Files
 *
 * Write a program that uses mkstemp() and tmpfile() to create and
 * open temporary files, and mkdtemp() for a directory.
 *
 * While the program is running (you can use sleep() do induce a
 * delay) verify the files are created and check their names against
 * the template for mkstemp(), mkdtemp(). For tmpnam() you may want to
 * run your program under strace to see what is really happening.
 @*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main ()
{
    int fd;
    FILE *fp;
    char *f_mkdtemp;
    char template_file[] = "AAXXXXXX", template_dir[] = "BBXXXXXX";

    printf ("template_file = %s\n", template_file);
    fd = mkstemp (template_file);
    printf ("template_file = %s\n", template_file);

    f_mkdtemp = mkdtemp (template_dir);

    printf ("f_mkdtemp = %s template_dir=%s\n", f_mkdtemp, template_dir);

    fp = tmpfile ();
    sleep (10);
    unlink (template_file);
    rmdir (f_mkdtemp);
    exit (0);
}
