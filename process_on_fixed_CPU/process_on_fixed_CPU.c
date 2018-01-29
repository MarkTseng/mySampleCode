#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

	int
main(int argc, char *argv[])
{
	int s, j;
	cpu_set_t cpuset;

	CPU_ZERO(&cpuset);
	CPU_SET(2, &cpuset); // on CPU 2

	s = sched_setaffinity(getpid(), sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "sched_setaffinity");

	// your code here
	while(1);
	exit(EXIT_SUCCESS);
}

