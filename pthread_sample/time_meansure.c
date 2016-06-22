#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

int main()
{
	struct timeval start, end;
	long mtime1, mtime2, diff;

	while(1)
	{
		gettimeofday(&start, NULL);

		usleep(100);

		gettimeofday(&end, NULL);

		mtime1 = (start.tv_sec * 1000000 + start.tv_usec);
		mtime2 = (end.tv_sec * 1000000 + end.tv_usec);

		diff = mtime2 - mtime1;
		printf("diff: %ld us\n", diff);
	}

	return 0;
}

