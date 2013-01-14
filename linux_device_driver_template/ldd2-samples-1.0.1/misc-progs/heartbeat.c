/*
 * heatbeat.c -- flash NumLock in an hearthbeat fashion
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

int main(int argc, char **argv)
{
    char led;
    int chosenled = 1;
    char hearth[]={1,0,1,1,0,0,0,0,0,0,0,0,0};
    int udelay = 100000;
    int load = 0;
    char *prgname = argv[0];
    FILE *f;

    if (argc > 1 && isdigit(argv[1][0])) { /* the time delay */
        udelay = 1000 * atoi(argv[1]);
        if (udelay < 1000)
            fprintf(stderr, "%s: delay too short\n", prgname);
        else {
            argv++; argc--;
        }
    }

    nice(-20); /* in case is succeeds... */

    udelay *= 100; /* prepare for a later division */

    if (argc > 1 && strlen(argv[1]) == 1) {
        argv++, argc--;
        if (tolower(argv[0][0]) == 's') chosenled = 1; /* scroll lock */
        else 
            if (tolower(argv[0][0]) == 'n') chosenled = 2; /* num lock */
        else
            if (tolower(argv[0][0]) == 'c') chosenled = 4; /* caps lock */
        else {
            fprintf(stderr, "%s: unknown led '%s'\n", prgname, argv[1]);
            argc++;
        }
    }
    if (argc>1) {
        fprintf(stderr, "%s: usage \"%s [delay ms] [ n | c | s ]\"\n",
                prgname, prgname);
        exit(1);
    }

   
    
    /* ok, now do your loop */

    for (;;) {
        int consolefd=open("/dev/tty0",O_RDONLY);

        int i;
        f=fopen("/proc/loadavg", "r");
        if (f) {
            fscanf(f, "%d.%d", &load, &i);
            fclose(f);
        } else {
            load = i = 0;
        }
        load = 100 + load * 100 + i;

        for (i=0; i < sizeof(hearth)/sizeof(hearth[0]); i++) {
            if (ioctl(consolefd, KDGETLED, &led)
                || ioctl(consolefd, KDSETLED,
                         (led & ~chosenled) | chosenled * hearth[i])) {
                fprintf(stderr, "%s: ioctl(): %s\n", prgname, strerror(errno));
                exit(2);
            }
            usleep(udelay/load);
        }
        close(consolefd);
    }
    exit(0); /* never happen */
}
                










