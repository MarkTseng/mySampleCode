/* **************** LPD:1.0 s_11/lab3_symlink.c **************** */
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
 * Symbolic links
 *
 * Write a program to search a directory for symbolic links.  If it
 * finds any, check to see if the link is valid or broken.
 @*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); }

#define DIRECTORY_DEF "/"
#define MAXPATH 256

int main (int argc, char *argv[])
{
    DIR *dir;
    struct dirent *direntry;
    struct stat file_info;
    char filename[MAXPATH], filename2[MAXPATH];
    char *dirname = DIRECTORY_DEF;

    if (argc > 1)
        dirname = argv[1];

    if (chdir (dirname))
        DEATH (dirname);

    if (!(dir = opendir ("./")))
        DEATH (dirname);

    /* Read each entry in the directory... */

    while ((direntry = readdir (dir))) {
        strcpy (filename, direntry->d_name);

        if (lstat (filename, &file_info))
            fprintf (stderr, "Error lstating %s\n", filename);

        /* ...if it's a link, find the other end. */

        else if (S_ISLNK (file_info.st_mode)) {

            memset (filename2, 0, sizeof (filename2));

            if (-1 == readlink (filename, filename2, sizeof (filename2))) {
                printf ("failed using readlink\n");

            } else {

                if (lstat (filename2, &file_info))
                    printf ("%s is a broken link to %s\n", filename, filename2);
                else
                    printf ("%s is a link to %s\n", filename, filename2);
            }
        }
    }
    closedir (dir);
    exit (0);
}
