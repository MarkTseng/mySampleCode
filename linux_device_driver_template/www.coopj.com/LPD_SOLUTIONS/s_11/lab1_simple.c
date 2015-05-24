/* **************** LPD:1.0 s_11/lab1_simple.c **************** */
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
   Write a program to search a directory and print out the contents.
   (In other words, write an "ls" program.)
 @*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define DEATH(mess) { perror(mess); exit(errno); }
#define DIRECTORY_DEF "."

int main (int argc, char *argv[])
{
    DIR *dir;
    struct dirent *direntry;
    char *dirname = DIRECTORY_DEF;

    if (argc > 1)
        dirname = argv[1];

    printf ("\n **** DISPLAYING DIRECTORY %s ****\n\n", dirname);

    /* Open the directory */
    if (!(dir = opendir (dirname)))
        DEATH (dirname);

    /* Read each entry in the directory... */
    while ((direntry = readdir (dir)))
        printf ("%s\n", direntry->d_name);

    /* close the directory */
    closedir (dir);
    exit (0);
}
