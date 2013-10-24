#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "unix_socket_utils.h"          /* some utility functions */
#define KBD_SOCKET_NAME "/tmp/kdb_socket"

int main(void) {
	struct sockaddr_un address;
	int sock, conn;
	size_t addrLength;

	if ((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0)
		die("socket");

	/* Remove any preexisting socket (or other file) */
	unlink(KBD_SOCKET_NAME);

	address.sun_family = AF_UNIX;       /* Unix domain socket */
	strcpy(address.sun_path, KBD_SOCKET_NAME);

	/* The total length of the address includes the sun_family
	   element */
	addrLength = sizeof(address.sun_family) +
		strlen(address.sun_path);

	if (bind(sock, (struct sockaddr *) &address, addrLength))
		die("bind");

	if (listen(sock, 5))
		die("listen");

	while ((conn = accept(sock, (struct sockaddr *) &address,
					&addrLength)) >= 0) {
		printf("---- getting data\n");
		SvrCopyData(conn, 1);
		printf("---- done\n");
		close(conn);
		break;
	}

	if (conn < 0)
		die("accept");

	close(sock);
	return 0;
}

