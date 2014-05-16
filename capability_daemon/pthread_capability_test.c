#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/capability.h>
#include <errno.h>
#include <sys/prctl.h>
#include <pthread.h>

static void *_Thread(void *arg)
{
    (void)arg;
	while(1)
	{
    	printf("Thread running!\n");
		sleep(1);
    }
	return NULL;
}

int main(void)
{
    int retVal;
    pthread_attr_t attr;
    struct sched_param schedParam;
    pthread_t thread;


     struct __user_cap_header_struct cap_header_data;
     cap_user_header_t cap_header = &cap_header_data;

     struct __user_cap_data_struct cap_data_data;
     cap_user_data_t cap_data = &cap_data_data;
	 int ret;

    if (setresuid(0, 0, 0)) {
        fprintf(stderr, "Cannot switch to root: %s.\n", strerror(errno));
        return 1;
    }

     cap_header->pid = getpid();
     cap_header->version = _LINUX_CAPABILITY_VERSION;
 
     if (capget(cap_header, cap_data) < 0) {
         perror("Failed capget");
         exit(1);
     }
     printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
         cap_data->permitted, cap_data->inheritable);

     /* Clear all but the capability to bind to low ports */
     cap_data->effective |= CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE);
     cap_data->permitted |= CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE);
     cap_data->inheritable |= CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE); 

	 if (capset(cap_header, cap_data) < 0)
		 printf("capset failed");

	 printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
			 cap_data->permitted, cap_data->inheritable);

	ret = nice(-1);
	printf("nice: %d\n", ret);

	 /* Tell kernel not clear capabilities when dropping root */
	 if (prctl(PR_SET_KEEPCAPS, 1) < 0)
		 printf("prctl(PR_SET_KEEPCAPS) failed ");

	setresuid(1000,1000,1000);
	printf("setresuid\n");
	
	/* Clear all but the capability to bind to low ports */
     cap_data->effective |= CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE);
     cap_data->permitted |= CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE);
     cap_data->inheritable |= CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE); 

	 if (capset(cap_header, cap_data) < 0)
		 printf("capset failed");

	 printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
			 cap_data->permitted, cap_data->inheritable);




    retVal = pthread_attr_init(&attr);
    if (retVal)
    {
        fprintf(stderr, "pthread_attr_init error %d\n", retVal);
        exit(1);
    }

    retVal = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (retVal)
    {
        fprintf(stderr, "pthread_attr_setinheritsched error %d\n", retVal);
        exit(1);
    }

    retVal = pthread_attr_setschedpolicy(&attr, SCHED_RR);
    if (retVal)
    {
        fprintf(stderr, "pthread_attr_setschedpolicy error %d\n", retVal);
        exit(1);
    }

    schedParam.sched_priority = 99;
    retVal = pthread_attr_setschedparam(&attr, &schedParam);
    if (retVal)
    {
        fprintf(stderr, "pthread_attr_setschedparam error %d\n", retVal);
        exit(1);
    }

    retVal = pthread_create(&thread,
                            &attr,
                            _Thread,
                            NULL);
    if (retVal)
    {
        fprintf(stderr, "pthread_create error %d\n", retVal);
        exit(1);
    }

    retVal = pthread_join(thread, NULL);
    if (retVal)
    {
        fprintf(stderr, "pthread_join error %d\n", retVal);
        exit(1);
    }

    printf("main run successfully\n");
    return 0;
}


