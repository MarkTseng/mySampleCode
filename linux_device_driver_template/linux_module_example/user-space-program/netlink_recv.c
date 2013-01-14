#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>


#define MAX_PAYLOAD 1024 

int main(int argc,char *argv[])
{
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	int sock_fd;
	struct msghdr msg;
	int ret;
	
	sock_fd = socket(PF_NETLINK, SOCK_RAW,NETLINK_USERSOCK);
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();  /* self pid */
	src_addr.nl_groups = 1;  /* not in mcast groups */
	bind(sock_fd, (struct sockaddr*)&src_addr,sizeof(src_addr));
	
	memset(&dest_addr, 0, sizeof(dest_addr));
	printf("Waiting for message from kernel\n");
  nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
  memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

  iov.iov_base = (void *)nlh;
  iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
  msg.msg_name = (void *)&dest_addr;
  msg.msg_namelen = sizeof(dest_addr);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;


  while (1)
  { 
 		/* Read message from kernel */
 		ret=recvmsg(sock_fd, &msg, 0);
 		if (ret>0)
 			printf("MC1:Received message payload: %s\n",(char *)NLMSG_DATA(msg.msg_iov->iov_base));
 		
 	}
	close(sock_fd);
	return 0;
}

