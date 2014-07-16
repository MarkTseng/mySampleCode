/********************************************************
 * An example source module to accompany...
 *
 * "Using POSIX Threads: Programming with Pthreads"
 *     by Brad nichols, Dick Buttlar, Jackie Farrell
 *     O'Reilly & Associates, Inc.
 *
 ********************************************************
 * struct.c
 *
 * Showing what various locking scheme choices look
 * like in the data structures
 */
#include <pthread.h>

#define DATA_SIZE 100

pthread_mutex_t data1_lock= PTHREAD_MUTEX_INITIALIZER;
   
struct data1_record {
	int	code;
	int	field1;
	int	field2;
	int	field3;
} data1[DATA_SIZE];

struct data2_record {
	int     code;
        int     field1;
        int     field2;
        int     field3;
	pthread_mutex_t data1_lock;
} data2[DATA_SIZE];

main()
{
	 



}


