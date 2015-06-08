/* **************** LPD:1.0 s_11/lab6_inotify.c **************** */
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
/* Using inotify to Monitor a Directory
 * 
 * Use inotify to monitory changes to the /tmp directory.
 *
 * When reading events check the bits in the returned mask to see what
 * kinds of events have occurred, when you create files, remove them,
 * read them, change their attributes, etc.
 *
 * You man want to consult the man page for inotify to get a complete
 * list of event bits.
@*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/inotify.h>
#include <string.h>

#define checkfor(m, a, s) if (ev->mask & a) printf("mask includes %s\n", s);

int main (int argc, char *argv[])
{
    int fd, nb, offset, bufsize;
    char *buf;
    struct inotify_event *ev;

    bufsize = getpagesize ();
    buf = malloc (bufsize);
    printf ("sizeof inotify_event = %ld\n", sizeof (struct inotify_event));
    fd = inotify_init ();
    printf ("fd from inotify_init=%d\n", fd);
    printf ("inotify_add_watch=%d\n",
            inotify_add_watch (fd, "/tmp", IN_ALL_EVENTS));

    for (;;) {
        offset = 0;
        memset (buf, 0, bufsize);
        nb = read (fd, buf, bufsize);
        printf ("\nnb from read=%d\n\n", nb);
        while (offset < nb) {
            ev = (struct inotify_event *)(buf + offset);

            printf ("Event: wd=%d, mask=%0x, cookie=%d, len=%4d, name=%s\n",
                    ev->wd, ev->mask, ev->cookie, ev->len, ev->name);
            checkfor (ev->mask, IN_ACCESS, "IN_ACCESS");
            checkfor (ev->mask, IN_ATTRIB, "IN_ATTRIB");
            checkfor (ev->mask, IN_CLOSE_WRITE, "IN_CLOSE_WRITE");
            checkfor (ev->mask, IN_CLOSE_NOWRITE, "IN_CLOSE_NOWRITE");
            checkfor (ev->mask, IN_CREATE, "IN_CREATE");
            checkfor (ev->mask, IN_DELETE, "IN_DELETE");
            checkfor (ev->mask, IN_DELETE_SELF, "IN_DELETE_SELF");
            checkfor (ev->mask, IN_MODIFY, "IN_MODIFY");
            checkfor (ev->mask, IN_MOVE_SELF, "IN_MOVE_SELF");
            checkfor (ev->mask, IN_MOVED_FROM, "IN_MOVED_FROM");
            checkfor (ev->mask, IN_MOVED_TO, "IN_MOVED_TO");
            checkfor (ev->mask, IN_OPEN, "IN_OPEN");
            offset += sizeof (struct inotify_event) + ev->len;
        }
    }
    exit (0);
}
