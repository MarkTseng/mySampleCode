#include <unistd.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <grp.h>
#include <errno.h>

#include <string.h>
#include <sched.h>
#include <stdio.h>


void test_priority(const char *const name, const int policy)
{
    const pid_t         me = getpid();
    struct sched_param  param;

    param.sched_priority = sched_get_priority_max(policy);
    printf("sched_get_priority_max(%s) = %d\n", name, param.sched_priority);
    if (sched_setscheduler(me, policy, &param) == -1)
        printf("sched_setscheduler(getpid(), %s, { %d }): %s.\n", name, param.sched_priority, strerror(errno));
    else
        printf("sched_setscheduler(getpid(), %s, { %d }): Ok.\n", name, param.sched_priority);
}


int main(void)
{

    test_priority("SCHED_RR", SCHED_RR);

	while(1)
	{
		sleep(1);
	}
    return 0;
}

