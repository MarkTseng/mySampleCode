#include <unistd.h>
#include <sys/types.h>
#include <sys/capability.h>
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

    param.sched_priority = sched_get_priority_min(policy);
    printf("sched_get_priority_min(%s) = %d\n", name, param.sched_priority);
    if (sched_setscheduler(me, policy, &param) == -1)
        printf("sched_setscheduler(getpid(), %s, { %d }): %s.\n", name, param.sched_priority, strerror(errno));
    else
        printf("sched_setscheduler(getpid(), %s, { %d }): Ok.\n", name, param.sched_priority);

}


int main(void)
{
    uid_t       user;
    cap_value_t root_caps[2] = { CAP_SYS_NICE, CAP_SETUID };
    cap_value_t user_caps[1] = { CAP_SYS_NICE, CAP_SYS_ADMIN };
    cap_t       capabilities;

    /* Get real user ID. */
    //user = getuid();
	user = 1000;

    /* Get full root privileges. Normally being effectively root
     * (see man 7 credentials, User and Group Identifiers, for explanation
     *  for effective versus real identity) is enough, but some security
     * modules restrict actions by processes that are only effectively root.
     * To make sure we don't hit those problems, we switch to root fully. */
    if (setresuid(0, 0, 0)) {
        fprintf(stderr, "Cannot switch to root: %s.\n", strerror(errno));
        return 1;
    }

    /* Create an empty set of capabilities. */
    capabilities = cap_init();

    /* Capabilities have three subsets:
     *      INHERITABLE:    Capabilities permitted after an execv()
     *      EFFECTIVE:      Currently effective capabilities
     *      PERMITTED:      Limiting set for the two above.
     * See man 7 capabilities for details, Thread Capability Sets.
     *
     * We need the following capabilities:
     *      CAP_SYS_NICE    For nice(2), setpriority(2),
     *                      sched_setscheduler(2), sched_setparam(2),
     *                      sched_setaffinity(2), etc.
     *      CAP_SETUID      For setuid(), setresuid()
     * in the last two subsets. We do not need to retain any capabilities
     * over an exec().
    */
    if (cap_set_flag(capabilities, CAP_PERMITTED, sizeof root_caps / sizeof root_caps[0], root_caps, CAP_SET) ||
        cap_set_flag(capabilities, CAP_EFFECTIVE, sizeof root_caps / sizeof root_caps[0], root_caps, CAP_SET)) {
        fprintf(stderr, "Cannot manipulate capability data structure as root: %s.\n", strerror(errno));
        return 1;
    }

    /* Above, we just manipulated the data structure describing the flags,
     * not the capabilities themselves. So, set those capabilities now. */
    if (cap_set_proc(capabilities)) {
        fprintf(stderr, "Cannot set capabilities as root: %s.\n", strerror(errno));
        return 1;
    }

    /* We wish to retain the capabilities across the identity change,
     * so we need to tell the kernel. */
    if (prctl(PR_SET_KEEPCAPS, 1L)) {
        fprintf(stderr, "Cannot keep capabilities after dropping privileges: %s.\n", strerror(errno));
        return 1;
    }

    /* Drop extra privileges (aside from capabilities) by switching
     * to the original real user. */
    if (setresuid(user, user, user)) {
        fprintf(stderr, "Cannot drop root privileges: %s.\n", strerror(errno));
        return 1;
    }

    /* We can still switch to a different user due to having the CAP_SETUID
     * capability. Let's clear the capability set, except for the CAP_SYS_NICE
     * in the permitted and effective sets. */
    if (cap_clear(capabilities)) {
        fprintf(stderr, "Cannot clear capability data structure: %s.\n", strerror(errno));
        return 1;
    }
    if (cap_set_flag(capabilities, CAP_PERMITTED, sizeof user_caps / sizeof user_caps[0], user_caps, CAP_SET) ||
        cap_set_flag(capabilities, CAP_EFFECTIVE, sizeof user_caps / sizeof user_caps[0], user_caps, CAP_SET)) {
        fprintf(stderr, "Cannot manipulate capability data structure as user: %s.\n", strerror(errno));
        return 1;
    }

    /* Apply modified capabilities. */
    if (cap_set_proc(capabilities)) {
        fprintf(stderr, "Cannot set capabilities as user: %s.\n", strerror(errno));
        return 1;
    }

    /*
     * Now we have just the normal user privileges,
     * plus user_caps.
    */

    test_priority("SCHED_OTHER", SCHED_OTHER);
    test_priority("SCHED_FIFO", SCHED_FIFO);
    test_priority("SCHED_RR", SCHED_RR);

	int ret;
	ret = nice(-20);
	printf("nice: %d\n", ret);

	while(1)
	{
		sleep(1);
	}
    return 0;
}

