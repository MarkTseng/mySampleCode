#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/capability.h>
#include <errno.h>
#include <sys/prctl.h>


#define handle_error_en(en, msg) \
	do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

	static void
usage(char *prog_name, char *msg)
{
	if (msg != NULL)
		fputs(msg, stderr);

	fprintf(stderr, "Usage: %s [options]\n", prog_name);
	fprintf(stderr, "Options are:\n");
#define fpe(msg) fprintf(stderr, "\t%s", msg);          /* Shorter */
	fpe("-a<policy><prio> Set scheduling policy and priority in\n");
	fpe("                 thread attributes object\n");
	fpe("                 <policy> can be\n");
	fpe("                     f  SCHED_FIFO\n");
	fpe("                     r  SCHED_RR\n");
	fpe("                     o  SCHED_OTHER\n");
	fpe("-A               Use default thread attributes object\n");
	fpe("-i {e|s}         Set inherit scheduler attribute to\n");
	fpe("                 'explicit' or 'inherit'\n");
	fpe("-m<policy><prio> Set scheduling policy and priority on\n");
	fpe("                 main thread before pthread_create() call\n");
	exit(EXIT_FAILURE);
}

	static int
get_policy(char p, int *policy)
{
	switch (p) {
		case 'f': *policy = SCHED_FIFO;     return 1;
		case 'r': *policy = SCHED_RR;       return 1;
		case 'o': *policy = SCHED_OTHER;    return 1;
		default:  return 0;
	}
}

	static void
display_sched_attr(int policy, struct sched_param *param)
{
	printf("    policy=%s, priority=%d\n",
			(policy == SCHED_FIFO)  ? "SCHED_FIFO" :
			(policy == SCHED_RR)    ? "SCHED_RR" :
			(policy == SCHED_OTHER) ? "SCHED_OTHER" :
			"???",
			param->sched_priority);
}

	static void
display_thread_sched_attr(char *msg)
{
	int policy, s;
	struct sched_param param;

	s = pthread_getschedparam(pthread_self(), &policy, &param);
	if (s != 0)
		handle_error_en(s, "pthread_getschedparam");

	param.sched_priority += 1;  

	s = pthread_setschedparam(pthread_self(), policy, &param);
		if (s != 0)
			handle_error_en(s, "pthread_setschedparam");

	printf("%s\n", msg);
	printf("pid: %d\n", getpid());
	printf("uid: %d\n", getuid());
	display_sched_attr(policy, &param);
}

	static void *
thread_start(void *arg)
{
	while(1)
	{
		display_thread_sched_attr("Scheduler attributes of new thread");
		sleep(2);
	}
	return NULL;
}

	int
main(int argc, char *argv[])
{
	int s, opt, inheritsched, use_null_attrib, policy;
	pthread_t thread;
	pthread_attr_t attr;
	pthread_attr_t *attrp;
	char *attr_sched_str, *main_sched_str, *inheritsched_str;
	struct sched_param param;


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
     cap_data->inheritable = cap_data->permitted; 

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
     cap_data->inheritable = CAP_TO_MASK(CAP_SYS_ADMIN) | CAP_TO_MASK(CAP_SETUID) | CAP_TO_MASK(CAP_SYS_NICE); 

	 if (capset(cap_header, cap_data) < 0)
		 printf("capset failed");

	 printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
			 cap_data->permitted, cap_data->inheritable);




	/* Process command-line options */

	use_null_attrib = 0;
	attr_sched_str = NULL;
	main_sched_str = NULL;
	inheritsched_str = NULL;

	while ((opt = getopt(argc, argv, "a:Ai:m:")) != -1) {
		switch (opt) {
			case 'a': attr_sched_str = optarg;      break;
			case 'A': use_null_attrib = 1;          break;
			case 'i': inheritsched_str = optarg;    break;
			case 'm': main_sched_str = optarg;      break;
			default:  usage(argv[0], "Unrecognized option\n");
		}
	}

	if (use_null_attrib &&
			(inheritsched_str != NULL || attr_sched_str != NULL))
		usage(argv[0], "Can't specify -A with -i or -a\n");

	/* Optionally set scheduling attributes of main thread,
	   and display the attributes */

	if (main_sched_str != NULL) {
		if (!get_policy(main_sched_str[0], &policy))
			usage(argv[0], "Bad policy for main thread (-s)\n");
		param.sched_priority = strtol(&main_sched_str[1], NULL, 0);

		s = pthread_setschedparam(pthread_self(), policy, &param);
		if (s != 0)
			handle_error_en(s, "pthread_setschedparam");
	}

	display_thread_sched_attr("Scheduler settings of main thread");
	printf("\n");

	/* Initialize thread attributes object according to options */

	attrp = NULL;

	if (!use_null_attrib) {
		s = pthread_attr_init(&attr);
		if (s != 0)
			handle_error_en(s, "pthread_attr_init");
		attrp = &attr;
	}

	if (inheritsched_str != NULL) {
		if (inheritsched_str[0] == 'e')
			inheritsched = PTHREAD_EXPLICIT_SCHED;
		else if (inheritsched_str[0] == 'i')
			inheritsched = PTHREAD_INHERIT_SCHED;
		else
			usage(argv[0], "Value for -i must be 'e' or 'i'\n");

		s = pthread_attr_setinheritsched(&attr, inheritsched);
		if (s != 0)
			handle_error_en(s, "pthread_attr_setinheritsched");
	}

	if (attr_sched_str != NULL) {
		if (!get_policy(attr_sched_str[0], &policy))
			usage(argv[0],
					"Bad policy for 'attr' (-a)\n");
		param.sched_priority = strtol(&attr_sched_str[1], NULL, 0);

		s = pthread_attr_setschedpolicy(&attr, policy);
		if (s != 0)
			handle_error_en(s, "pthread_attr_setschedpolicy");
		s = pthread_attr_setschedparam(&attr, &param);
		if (s != 0)
			handle_error_en(s, "pthread_attr_setschedparam");
	}

	/* If we initialized a thread attributes object, display
	   the scheduling attributes that were set in the object */

	if (attrp != NULL) {
		s = pthread_attr_getschedparam(&attr, &param);
		if (s != 0)
			handle_error_en(s, "pthread_attr_getschedparam");
		s = pthread_attr_getschedpolicy(&attr, &policy);
		if (s != 0)
			handle_error_en(s, "pthread_attr_getschedpolicy");

		printf("Scheduler settings in 'attr'\n");
		display_sched_attr(policy, &param);

		s = pthread_attr_getinheritsched(&attr, &inheritsched);
		printf("    inheritsched is %s\n",
				(inheritsched == PTHREAD_INHERIT_SCHED)  ? "INHERIT" :
				(inheritsched == PTHREAD_EXPLICIT_SCHED) ? "EXPLICIT" :
				"???");
		printf("\n");
	}

	/* Create a thread that will display its scheduling attributes */

	s = pthread_create(&thread, attrp, &thread_start, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_create");

	/* Destroy unneeded thread attributes object */

	s = pthread_attr_destroy(&attr);
	if (s != 0)
		handle_error_en(s, "pthread_attr_destroy");

	s = pthread_join(thread, NULL);
	if (s != 0)
		handle_error_en(s, "pthread_join");

	ret = nice(-1);
	printf("nice: %d\n", ret);

	while(1)
	{
		ret = nice(-1);
		printf("nice: %d\n", ret);
		printf("uid: %d\n", getuid());
		printf("wait....\n");
		 printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective,
			 cap_data->permitted, cap_data->inheritable);
		sleep(1);
	}


	exit(EXIT_SUCCESS);
}

