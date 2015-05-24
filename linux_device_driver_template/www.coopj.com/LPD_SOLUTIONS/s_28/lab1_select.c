/* **************** LPD:1.0 s_28/lab1_select.c **************** */
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
 * I/O Multiplexing and select() (select() solution)
 *
 * Write a program that takes some files names as arguments.  It
 * should open each file, and then scan each one for incoming data.
 *
 * When any file has data ready to read it should print out the
 * filename as well as the data.  Loop indefinitely, printing the data
 * from each file as it comes.
 *
 * Test the program by creating two named pipes with the mkfifo
 * command:
 *
 *   mkfifo file1
 *   mkfifo file2
 *
 *  (we'll discuss this when we discuss pipes) and opening up three
 *  shell windows (or using three virtual terminals if you don't have
 *  X running).
 *
 * In two windows, type cat > file1 and cat > file2; in the third
 * window, run your program. You should be able to type "Hello" in the
 * first window, and see "file1 says: Hello" come from your test
 * program, then type "Goodbye" in the second window and see "file2
 * says: Goodbye", etc.
 *
 * Some hints:
 *
 *    Print out the name of each file as you open it.  Remember that
 *    if you try to open a fifo for reading, and nobody has it open
 *    for writing, you will stop dead right there! This is an easy
 *    mistake to make and printing each filename you open will make it
 *    obvious when this happens.
 *
 *    You don't have to worry about files being closed if you don't
 *    want to. For extra credit, have your program "forget" about
 *    closed files when you hear that they are closed, but this can be
 *    tricky so don't worry about it if you don't want to.
 *
 *    Don't forget that the first argument to select() should be the
 *    highest file descriptor that is set in your file descriptor set
 *    plus one, not the number of files set!  This is very important!
 *
 * Implement the program using select().
 *
 * Implement the program using epoll.
 @*/

#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_CONNS 100
#define READ_AMT (4 * 1024)

/*
 * Read from several files at once.
 */
int main (int argc, char *argv[])
{
    fd_set permSet, testSet;
    int fd[MAX_CONNS];          /* Element 0 not used. */
    int i, maxFd = -1;
    char buffer[READ_AMT + 1];
    ssize_t byteCount;

    FD_ZERO (&permSet);
    assert (argc < MAX_CONNS);
    for (i = 1; i < argc; ++i) {
        fd[i] = open (argv[i], O_RDONLY);
        if (fd[i] < 0) {
            fprintf (stderr, "Opening %s failed: %s\n", argv[i],
                     strerror (errno));
            exit (1);
        } else {
            printf ("Opened %s successfully\n", argv[i]);
        }
        FD_SET (fd[i], &permSet);

        /*
         * Must keep track of the highest fd that is set, it must be passed in as
         * the first argument to select().
         */
        if (fd[i] > maxFd) {
            maxFd = fd[i];
        }
    }
    /*
     * Files opened. "permSet" is now the set of files we care about, and
     * "fd[]" contains the file descriptors to check for each file name.
     */
    for (;;) {

        /*
         * Copy perm set because select() modifies its fd set inputs. If we
         * just passed it in, then we would destroy our original fd set.
         */
        testSet = permSet;

        i = select (maxFd + 1, &testSet, NULL, NULL, NULL);
        assert (i > 0);

        for (i = 1; i < argc; ++i) {
            if (FD_ISSET (fd[i], &testSet)) {
                byteCount = read (fd[i], buffer, READ_AMT);
                assert (byteCount > 0);

                /* Make sure the buffer ends with a null byte for printf's sake. */
                buffer[byteCount] = '\0';
                printf ("Got data from %s: %s", argv[i], buffer);
            }
        }
    }
}
