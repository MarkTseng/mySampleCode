#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>

#define MAX_EPOLL_EVENTS (6)
#define EVENTS_TIMEOUT (30000)

#define FRAME_START (EPOLLIN)
#define AE_DONE (EPOLLOUT)
#define AWB_DONE (EPOLLPRI)

int main(int argc, char *argv[])
{
  int efd, fd;
  int n, i;
  struct epoll_event event;
  struct epoll_event events[MAX_EPOLL_EVENTS];
  fd = open("/dev/isp", O_RDWR | O_SYNC);
  efd = epoll_create1(0);
  if (efd == -1)
    {
      perror ("epoll_create");
      abort ();
    }
  
  /* Buffer where events are returned */
  //events = calloc (MAX_EPOLL_EVENTS, sizeof event);
  
  event.data.fd = fd;
  event.events = EPOLLIN | EPOLLOUT | EPOLLPRI;
  epoll_ctl (efd, EPOLL_CTL_ADD, fd, &event);
  

  struct sched_param param;
  memset(&param,0,sizeof(param));
  param.sched_priority = 98;
  //sched_setscheduler(getpid(), SCHED_RR, &param);

   while (1)
   {
      // block here to wait interrupt events
      n = epoll_wait (efd, events, MAX_EPOLL_EVENTS, -1);
      for (i = 0; i < n; i++)
	  	{
             printf("events %#x\n", events[i].events);
             if(events[i].events & FRAME_START)
				    printf("FS event \n");
             if(events[i].events & AE_DONE)
				    printf("AE_ROW_DONE event \n");
             if(events[i].events & AWB_DONE)
				    printf("AWB_DONE event \n");
	   }
  }
  system("ls -l /proc/`pidof mixer`/fd/");
	
	
  free (events);
  close(efd);
  close (fd);

}
