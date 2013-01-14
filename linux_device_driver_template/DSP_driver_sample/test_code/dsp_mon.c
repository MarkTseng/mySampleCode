/*
 * Sunplus  DSP mon program
 * Author : Mark Tseng
 * DOCUMENT: implement dsp mon command with netlink socket protocol
 * Note: 1. refer to dsp_mon.c in driver for more detail mon command
 *       2. To define DEBUG_ENABLE_DSPDEBUG_PRINT macro for debug message in aud_global.h
 * 
 */
static char const rcsid[] = "$Id: dsp_mon, v0.1.0.0 " __DATE__ " " __TIME__ " Mark Exp $";

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

#define MAX_PAYLOAD 1024  /* maximum payload size*/

#define NETLINK_DSPMON      20      /* sunplus dsp mon */

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

int main(int argc, char* argv[]) 
{
 	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_DSPMON);
	memset(&msg, 0, sizeof(msg));
 	memset(&src_addr, 0, sizeof(src_addr));
 	src_addr.nl_family = AF_NETLINK;
 	src_addr.nl_pid = getpid();  /* self pid */
 	src_addr.nl_groups = 0;  /* not in mcast groups */
 	bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
 	dest_addr.nl_family = AF_NETLINK;
 	dest_addr.nl_pid = 0;   /* For Linux Kernel */
 	dest_addr.nl_groups = 0; /* unicast */

 	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
 	/* Fill the netlink message header */
 	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
 	nlh->nlmsg_pid = getpid();  /* self pid */
 	nlh->nlmsg_flags = 0;
 	/* Fill in the netlink message payload */
 	strcpy(NLMSG_DATA(nlh), argv[1]);

 	iov.iov_base = (void *)nlh;
 	iov.iov_len = nlh->nlmsg_len;
 	msg.msg_name = (void *)&dest_addr;
 	msg.msg_namelen = sizeof(dest_addr);
 	msg.msg_iov = &iov;
 	msg.msg_iovlen = 1;

 	sendmsg(sock_fd, &msg, 0);

#if 0
 	/* Read message from kernel */
 	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
 	recvmsg(sock_fd, &msg, 0);
 	printf(" Received message payload: %s\n",
	NLMSG_DATA(nlh));
#endif

	 /* Close Netlink Socket */
 	close(sock_fd);

	return 0;
}


