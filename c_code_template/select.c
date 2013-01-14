#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) 
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	while(1) {
		/* Wait up to five seconds. */
		tv.tv_sec = 0;
		tv.tv_usec = 10000;


		retval = select(1, &rfds, NULL, NULL, &tv);
		/* Don’t rely on the value of tv now! */

		if (retval == -1)
			perror("select()");
		else if (retval)
			printf("Data is available now.\n");
		/* FD_ISSET(0, &rfds) will be true. */
		else
			printf("No data within five seconds.\n");
	}
	
	return 0;
}
