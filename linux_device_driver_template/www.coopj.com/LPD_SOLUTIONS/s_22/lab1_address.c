/* **************** LPD:1.0 s_22/lab1_address.c **************** */
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
 * Addresses, Services and Protocols
 *
 * Write a program to take three parameters:
 *
 *   An IP address in dot-quad notation or a host name
 *   A service name or a port number
 *   A protocol name (tcp or udp)
 *
 * From this information configure a sockaddr_in address
 * structure. Test with parameters localhost, www, and tcp, You should
 * get:
 *
 *    sin_family =            2
 *    sin_addr   =   2130706433 (0x7F000001) (127.0.0.1)
 *    sin_port   =           80 (0x00000050)
 @*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define DEFAULT_HOST        "localhost"
#define DEFAULT_SERVICE     "http"
#define DEFAULT_PROTOCOL    "tcp"

static int resolve (const char *host, const char *service, const char *proto,
                    struct sockaddr_in *addr);

int main (int argc, char *argv[])
{
    char *host = DEFAULT_HOST;
    char *service = DEFAULT_SERVICE;
    char *protocol = DEFAULT_PROTOCOL;
    struct sockaddr_in addr;
    unsigned short port;
    unsigned long address;
    int err;

    if (argc > 1)
        host = argv[1];

    if (argc > 2)
        service = argv[2];

    if (argc > 3)
        protocol = argv[3];

    err = resolve (host, service, protocol, &addr);

    if (!err) {
        port = ntohs (addr.sin_port);
        address = ntohl (addr.sin_addr.s_addr);
        printf ("sin_family = %12d\n", addr.sin_family);
        printf ("sin_addr   = %12lu (0x%08lX) (%s)\n", address, address,
                inet_ntoa (addr.sin_addr));
        printf ("sin_port   = %12hu (0x%08hX)\n", port, port);
    }
    return err;
}

static int
resolve (const char *host, const char *service, const char *proto,
         struct sockaddr_in *addr)
{
    unsigned short port;
    struct hostent *hostentry = NULL;
    struct servent *serventry = NULL;
    extern int h_errno;

    addr->sin_family = AF_INET;

    if (!(hostentry = gethostbyname (host))) {
        herror (host);
        return h_errno;
    }
    memcpy (&addr->sin_addr, hostentry->h_addr, hostentry->h_length);

    /* assume service was specified by port number */
    port = (unsigned short)atoi (service);

    if (port > 0) {
        /* success: service port specified by number */
        addr->sin_port = htons (port);
    } else {
        /* failed: service was probably specified by name, look it up */
        serventry = getservbyname (service, proto);

        if (serventry == NULL) {
            fprintf (stderr, "Failed to get service port: %s\n", service);
            return 1;
        }
        addr->sin_port = serventry->s_port;
    }

    return 0;
}

/* end of main */
