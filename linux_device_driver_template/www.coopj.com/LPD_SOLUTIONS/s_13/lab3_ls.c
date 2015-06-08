/* **************** LPD:1.0 s_13/lab3_ls.c **************** */
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
 * More Forking and Waiting
 *
 * Modify your ls program from the previous section so that, for every
 * subdirectory found (you can use stat() and S_ISDIR() to detect
 * directories), it forks a child to process that subdirectory (you
 * probably want to skip processing . and ..).
 *
 * If the output gets too muddled, you can have the parent wait() for
 * each child to finish.
 *
 * It is possible to create too many processes and hang your machine.
 * See help ulimit and type ulimit -a to get the current limits.  Set
 * the maximum number of processes to a reasonable value.
 @*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define DIRECTORY_DEF "."
#define DEATH(mess) { perror(mess); exit(errno); }

void process_dir (char *this_dir);

int main (int argc, char *argv[])
{
    char *dirname = DIRECTORY_DEF;
    if (argc > 1)
        dirname = argv[1];
    printf ("DIRECTORY: %s - %d\n", dirname, getpid ());
    process_dir (dirname);
    exit (EXIT_SUCCESS);
}

void process_dir (char *this_dir)
{
    DIR *dir;
    struct dirent *direntry;
    struct stat file_info;
    int status;

    if (chdir (this_dir))
        DEATH (this_dir);

    if (!(dir = opendir ("./")))
        DEATH (this_dir);

    /* List all directory entries */

    while ((direntry = readdir (dir))) {
        if (lstat (direntry->d_name, &file_info))
            fprintf (stderr, "Error lstat()ing %s\n", direntry->d_name);
        printf ("     %d:  %s\n", getpid (), direntry->d_name);
    }

    /* rewind and then recurse through subdirectories */

    rewinddir (dir);

    while ((direntry = readdir (dir))) {

        if (lstat (direntry->d_name, &file_info))
            fprintf (stderr, "Error lstat()ing %s\n", direntry->d_name);

        /* ...if it's a directory.... */
        else if (S_ISDIR (file_info.st_mode)) {

            pid_t pid;

            /* don't do it if it is . or .. */

            if (strcmp (direntry->d_name, ".") &&
                strcmp (direntry->d_name, "..")) {

                fflush (stdout);
                pid = fork ();
                if (pid == 0) {
                    printf ("\nPID  %d DIRECTORY: %s\n",
                            getpid (), direntry->d_name);
                    process_dir (direntry->d_name);
                    exit (EXIT_SUCCESS);
                }
                if (pid > 0)
                    wait (&status);
                else
                    DEATH ("fork failed");

            }                   /* not . or .. */
        }                       /* directory */
    }                           /* while direntry */
    closedir (dir);
}
