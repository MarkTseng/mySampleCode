#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>

#define MAX_POLL_EVENTS (6)
#define EVENTS_TIMEOUT (30000)

#define FRAME_START (POLLIN)
#define AE_DONE (POLLOUT)
#define AWB_DONE (POLLPRI)

struct pollfd fds[MAX_POLL_EVENTS];
int timeout_msecs = EVENTS_TIMEOUT;
int ret;
int i;

int main(int argc, char *argv[])
{
    memset(fds, 0 , sizeof(fds));

	/* Open STREAMS device. */
	fds[5].fd = open("/dev/isp", O_RDWR);
	fds[5].events = POLLIN | POLLOUT | POLLPRI;
    struct sched_param param;
    memset(&param,0,sizeof(param));
    param.sched_priority = 98;
    //sched_setscheduler(getpid(), SCHED_RR, &param);

	while(1)
	{
	    //printf("start poll events \n");
		ret = poll(fds, MAX_POLL_EVENTS, timeout_msecs);

        // timeout
        if(ret == 0)
        {
                    
        }
        // event 
		if (ret > 0) {
			/* An event on one of the fds has occurred. */
			for (i=0; i<MAX_POLL_EVENTS; i++) {
                printf("revents = %#x\n", fds[i].revents);
                if(fds[i].revents & FRAME_START)
				    printf("FS event \n");
                if(fds[i].revents & AE_DONE)
				    printf("AE_ROW_DONE event \n");
                if(fds[i].revents & AWB_DONE)
				    printf("AWB_DONE event \n");
			}
		}	
    }

}
