#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>
#include <errno.h>

static size_t blockSize;
static int numThreads, numBlocks;

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
} while (0)

	static void *
thread_func(void *arg)
{
	int j;
	int tn = (int) arg;

	/* The multiplier '(2 + tn)' ensures that each thread (including
	   the main thread) allocates a different amount of memory */

	for (j = 0; j < numBlocks; j++)
		if (malloc(blockSize * (2 + tn)) == NULL)
			errExit("malloc-thread");

	sleep(100);         /* Sleep until main thread terminates */
	return NULL;
}

	int
main(int argc, char *argv[])
{
	int j, tn, sleepTime;
	pthread_t *thr;

	if (argc < 4) {
		fprintf(stderr,
				"%s num-threads num-blocks block-size [sleep-time]\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	numThreads = atoi(argv[1]);
	numBlocks = atoi(argv[2]);
	blockSize = atoi(argv[3]);
	sleepTime = (argc > 4) ? atoi(argv[4]) : 0;

	thr = calloc(numThreads, sizeof(pthread_t));
	if (thr == NULL)
		errExit("calloc");

	printf("============ Before allocating blocks ============\n");
	malloc_info(0, stdout);

	/* Create threads that allocate different amounts of memory */

	for (tn = 0; tn < numThreads; tn++) {
		errno = pthread_create(&thr[tn], NULL, thread_func,
				(void *) tn);
		if (errno != 0)
			errExit("pthread_create");

		/* If we add a sleep interval after the start-up of each
		   thread, the threads likely won't contend for malloc
		   mutexes, and therefore additional arenas won't be
		   allocated (see malloc(3)). */

		if (sleepTime > 0)
			sleep(sleepTime);
	}

	/* The main thread also allocates some memory */

	for (j = 0; j < numBlocks; j++)
		if (malloc(blockSize) == NULL)
			errExit("malloc");

	sleep(2);           /* Give all threads a chance to
						   complete allocations */

	printf("\n============ After allocating blocks ============\n");
	malloc_info(0, stdout);

	exit(EXIT_SUCCESS);
}

