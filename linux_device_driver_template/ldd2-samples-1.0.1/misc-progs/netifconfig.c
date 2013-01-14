/*
 * netifconfig.c -- look and change the interface configuration
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
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>

char *prgname;


int getconfig(int sock, struct ifreq *req, int print)
{
    if ( ioctl(sock, SIOCGIFMAP, req) < 0) {
        fprintf(stderr, "%s: ioctl(SIOCGIFMAP): %s\n",
                prgname,strerror(errno));
        return -1;
    }
    if (print) 
        printf("%s: mem=0x%lx-0x%lx, io=0x%x, irq=%i, dma=%i, port=%i\n",
               req->ifr_name,
               (long)req->ifr_map.mem_start,
               (long)req->ifr_map.mem_end,
               req->ifr_map.base_addr,
               req->ifr_map.irq,
               req->ifr_map.dma,
               req->ifr_map.port);
    return 0;
}

int setconfig(int sock, struct ifreq *req)
{
    if ( ioctl(sock, SIOCSIFMAP, req) < 0) {
        fprintf(stderr, " %s: ioctl(SIOCSIFMAP): %s\n",
                prgname,strerror(errno));
        return -1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int sock;
    struct ifreq req;
    char *action = NULL;
    prgname = argv[0];

    if (argc < 2) {
        fprintf(stderr,"%s: usage is \"%s <ifname> [<operation>]\"\n",
                prgname, prgname);
        exit(1);
    }
    strcpy(req.ifr_name, argv[1]);
    argc--; argv++; /* shift the interface out */

    /* a silly raw socket just for ioctl()ling it */
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0) {
        fprintf(stderr, "%s: socket(): %s\n", argv[0],strerror(errno));
        exit(1);
    }

    if (getconfig(sock,&req, (argc==1) )) /* print if no commands */
        exit(1);

    /* ok, now loop through the options */
    while (argc > 1) {
        char *value;
        int ivalue = 0;

        action = argv[1];
        value = strchr(action,'=');
        if (value) {
            *value=0; /* terminate action */
            value++; /* skip the terminator */
            sscanf(value,"%i",&ivalue);
            /* fprintf(stderr,"--> %s %i\n",action,ivalue); */
        }
        if (!strcmp(action,"mem_start"))
            req.ifr_map.mem_start=ivalue;
        else if (!strcmp(action,"mem_end"))
            req.ifr_map.mem_end=ivalue;
        else if (!strcmp(action,"io"))
            req.ifr_map.base_addr=ivalue;
        else if (!strcmp(action,"irq"))
            req.ifr_map.irq=ivalue;
        else if (!strcmp(action,"dma"))
            req.ifr_map.dma=ivalue;
        else if (!strcmp(action,"port"))
            req.ifr_map.port=ivalue;
        else if (!strcmp(action,"tell"))
            getconfig(sock,&req, 1);
        else {
            fprintf(stderr,"%s: unknown argument \"%s\"\n",
                    prgname,action);
            argc--; argv++; continue;
        }
        if (strcmp(action,"tell"))
            setconfig(sock,&req);
        argc--; argv++;
    }
    exit(0);
}
