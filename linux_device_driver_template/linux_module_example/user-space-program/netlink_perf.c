#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define NETLINK_TEST 18
//#define MAX_PAYLOAD 1024
//#define MAX_PAYLOAD 2048
//#define MAX_PAYLOAD 4096
#define MAX_PAYLOAD 8192
#define MESSAGE_COUNT 1048576

struct sockaddr_nl src_addr, dest_addr;
struct msghdr msg;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;

void main()
{
	int _i = 0;

	time_t startTime = 0;
	time_t endTime = 0;

	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();
	src_addr.nl_groups = 0;

	bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;    // send to kernel
	dest_addr.nl_groups = 0;

	startTime = time(NULL);
	printf("start time: %s\n", ctime(&startTime));
	for (_i = 0; _i < MESSAGE_COUNT; _i++)
	{
		nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
		memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

		nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
		nlh->nlmsg_pid = getpid();
		nlh->nlmsg_flags = 0;

		iov.iov_base = (void *)nlh;
		iov.iov_len = nlh->nlmsg_len;
		msg.msg_name = (void *)&dest_addr;
		msg.msg_namelen = sizeof(dest_addr);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;


		strcpy(NLMSG_DATA(nlh), "Hello Kernel");
		sendmsg(sock_fd, &msg, 0);  // send message to kernel
		memset(nlh, 0, NLMSG_SPACE(strlen("Hello Kernel")));
		recvmsg(sock_fd, &msg, 0);

		free(nlh);
	}

	endTime = time(NULL);
	printf("end time: %s\n", ctime(&endTime));

	printf("Avg speed is %lf Byte/s\n", (double)((unsigned long long)MESSAGE_COUNT * (unsigned long long)MAX_PAYLOAD) / difftime(endTime, startTime));

	close(sock_fd);
}

