/* **************** LPD:1.0 s_11/lab1_ls.c **************** */
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
 * Directories
 *
 * Write a program to search a directory and print out the
 * contents. (In other words, write an "ls" program.)
 *
 * Optional: Enhance your code to display file permissions in octal,
 * user/group information in decimal, and the size of the file; i.e.,
 * mimic the -l option to ls.  You'll have to play with stat() to
 * accomplish this.
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
    struct stat info;

    if (argc > 1)
        dirname = argv[1];

    printf ("\n **** DISPLAYING DIRECTORY %s ****\n\n", dirname);

    /* Open the directory */
    if (!(dir = opendir (dirname)))
        DEATH (dirname);

    /* Change to the directory to make stat work if not the current dir */

    if (chdir (dirname))
        DEATH (dirname);

    /* Read each entry in the directory... */
    printf ("   MODE     UID    GID           SIZE\n\n");

    while ((direntry = readdir (dir))) {

        if (stat (direntry->d_name, &info))
            DEATH (" problems fstating the file:");

        printf ("%8o   %4d   %4d   %12d  %s\n", info.st_mode,
                info.st_uid, info.st_gid, (int)info.st_size, direntry->d_name);
    }

    /* close the directory */
    closedir (dir);
    exit (0);
}
