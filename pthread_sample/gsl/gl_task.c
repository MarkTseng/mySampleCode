/*
 * gl_task.c
 *
 */
#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <gsl.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <gsl/lambo86_hw.h>
#include <gsl/gl_thread.h>
#include "gl_common.h"

#define perror_thread(prestr, err)    \
    fprintf(stderr, "[#e]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

#define pwarning_thread(prestr, err)    \
    fprintf(stderr, "[#w]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

#define DEFAULT_PRIORITY	31

//#define GL_TRACK_SYNC_MECHANISM 0
#define SIG_SUSPEND (SIGRTMIN + 6)

#ifdef GL_OBJECT_LIST_TASK
static gsl_list_t thread_dump_list = { .next = NULL };
#endif

#if (GL_TRACK_SYNC_MECHANISM == 1)
#define GL_BLOCK_INFO_BUFFER_SIZE	512
static void gl_TaskFreeBlockInfo(GL_Task_t task_id);
#endif

/*
 * static variable & function
 */
static pthread_once_t tsk_once = PTHREAD_ONCE_INIT;
static pthread_key_t tsk_key;

static void thread_suspend_handler(int sig)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    int err;

    if (NULL == th) {
        printf("[#W]%s(%d) %lu ...\n", __FUNCTION__, __LINE__, pthread_self());
        return;
    }

    if (SIG_SUSPEND == sig) {
//        printf("[##]%s(%lu) - %d SUSPEND.\n", th->name, pthread_self(), th->scount);
        pthread_mutex_lock(&th->slock);
        th->scount++;
        while (th->scount) {
            err = pthread_cond_wait(&th->sready, &th->slock);
            if (err != 0) {
                pwarning_thread("pthread_cond_wait", err);
            }
        }
        pthread_mutex_unlock(&th->slock);
    }
}

static void thread_once_init(void)
{
    int err = pthread_key_create(&tsk_key, NULL);
    struct sigaction act;

    if (err != 0) {
        perror_thread("pthread_key_create", err);
    }

    sigaction(SIG_SUSPEND, NULL, &act);
    act.sa_handler = thread_suspend_handler;
    if (0 != sigaction(SIG_SUSPEND, &act, NULL)) {
         perror_thread("sigaction", errno);
	}
}

static void thread_cleanup(void *argp)
{
    gsl_thread_t *th = (gsl_thread_t *)argp;
    int err;

    if (NULL == th) {
        return;
    }

#ifdef GL_OBJECT_LIST_TASK
	gsl_remove_list(&thread_dump_list, th);
#endif
//    printf("[#I]%s(%d) / %s(%lu) ...\n", __FUNCTION__, __LINE__, th->name, pthread_self());
    err = pthread_setspecific(tsk_key, NULL);
    if (err != 0) {
        pwarning_thread("pthread_setspecific", err);
    }

    err = pthread_mutex_destroy(&th->slock);
    if (err != 0) {
        pwarning_thread("pthread_mutex_destroy", err);
    }

    err = pthread_cond_destroy(&th->sready);
    if (err != 0) {
        pwarning_thread("pthread_cond_destroy", err);
    }

    err = pthread_mutex_destroy(&th->lock);
    if (err != 0) {
        pwarning_thread("pthread_mutex_destroy", err);
    }

    th->threadid = 0;
    free(th);
}

/*
 * For debugging purpose.
 */
#define __SYSCALL_CLOBBERS "$1", "$3", "$8", "$9", "$10", "$11", "$12", "$13", \
	"$14", "$15", "$24", "$25", "hi", "lo", "memory"

#define internal_syscall0(ncs_init, cs_init, input, err, dummy...)	\
({									\
	long _sys_result;						\
									\
	{								\
	register long __v0 asm("$2") ncs_init;				\
	register long __a3 asm("$7");					\
	__asm__ volatile (						\
	".set\tnoreorder\n\t"						\
	cs_init								\
	"syscall\n\t"							\
	".set reorder"							\
	: "=r" (__v0), "=r" (__a3)					\
	: input								\
	: __SYSCALL_CLOBBERS);						\
	err = __a3;							\
	_sys_result = __v0;						\
	}								\
	_sys_result;							\
})

static pid_t gettid(void)
{
	pid_t tid;

	register long __v0 asm("$2") = SYS_gettid;
	register long __a3 asm("$7");

	asm volatile(
		".set noreorder\n"
		"li	$2, %2\n"
		"syscall\n"
		"move	%0, $2\n"
		".set reorder\n"
		: "=r"(__v0), "=r"(__a3)
		: "i"(SYS_gettid)
		: __SYSCALL_CLOBBERS);

	tid = (pid_t)__v0;

	return tid;
}

static void *thread_thread_handler(void *argp)
{
    gsl_thread_t *th = (gsl_thread_t *)argp;
    int err;
#if 0
	/* FIXME: th->threadid should be a correct value.  */
    if (!th || !th->threadid) {
        return NULL;
    }
#else
    if (!th) {
        return NULL;
    }
	th->threadid = pthread_self();

	//pthread_setname_np(th->threadid, th->name);
	th->sched_other_tid = syscall(SYS_gettid);
	th->tid = (void *)gettid();
#endif
    err = pthread_setspecific(tsk_key, th);
    if (err != 0) {
        pwarning_thread("pthread_setspecific", err);
    }

    if (!pthread_equal(th->threadid, pthread_self())) {
        printf("[#E]%s(%d) ...\n", __FUNCTION__, __LINE__);
    }

    #if 1   // ? PTHREAD_CREATE_DETACHED
    err = pthread_detach(th->threadid);
    if (err != 0) {
        pwarning_thread("pthread_detach", err);
    }
    #endif

    pthread_cleanup_push(thread_cleanup, th);

	pthread_mutex_lock(&th->slock);
	while (th->scount) {
		err = pthread_cond_wait(&th->sready, &th->slock);
			if (err != 0) {
			pwarning_thread("pthread_cond_wait", err);
		}
	}
	pthread_mutex_unlock(&th->slock);

	prctl(PR_SET_NAME, (unsigned long)th->name);

    /* customer function do ... */
    th->p_th_func(th->p_th_arg);

    pthread_cleanup_pop(1);

    return NULL;
}

static void *thread_sched_other_handler(void *argp)
{
    gsl_thread_t *th = (gsl_thread_t *)argp;
    int err;
#if 0
	/* FIXME: th->threadid should be a correct value.  */
    if (!th || !th->threadid) {
        return NULL;
    }
#else
    if (!th) {
        return NULL;
    }
	th->threadid = pthread_self();

	//pthread_setname_np(th->threadid, th->name);
	th->sched_other_tid = syscall(SYS_gettid);
	th->tid = (void *)gettid();

	/* set priority */
	setpriority(PRIO_PROCESS, th->sched_other_tid, th->priority - DEFAULT_PRIORITY);
#endif
    err = pthread_setspecific(tsk_key, th);
    if (err != 0) {
        pwarning_thread("pthread_setspecific", err);
    }

    if (!pthread_equal(th->threadid, pthread_self())) {
        printf("[#E]%s(%d) ...\n", __FUNCTION__, __LINE__);
    }

    #if 1   // ? PTHREAD_CREATE_DETACHED
    err = pthread_detach(th->threadid);
    if (err != 0) {
        pwarning_thread("pthread_detach", err);
    }
    #endif

    pthread_cleanup_push(thread_cleanup, th);

	pthread_mutex_lock(&th->slock);
	while (th->scount) {
		err = pthread_cond_wait(&th->sready, &th->slock);
			if (err != 0) {
			pwarning_thread("pthread_cond_wait", err);
		}
	}
	pthread_mutex_unlock(&th->slock);

	prctl(PR_SET_NAME, (unsigned long)th->name);

    /* customer function do ... */
    th->p_th_func(th->p_th_arg);

    pthread_cleanup_pop(1);

    return NULL;
}

/* it will get more pthread execute time when system booting. It depends on how many threads will 
 * create on boot time by "tusage" mon command.
 */
static int boot_thread_count = 0;
#define BOOT_THREADS	60
/*
 * GL_TaskCreate
 */
GL_Status_t GL_TaskCreate(char *szName, void (*pEntryPoint)(void *pArg), void *pTaskArg, GL_INT32 dPriority, GL_UINT32 dStackSize, Boolean fTaskRun, GL_Task_t *pTaskId)
{
    gsl_thread_t *th = NULL;
    int err;

    /* Check for NULL pointers */
    if( (NULL == pEntryPoint) || (dPriority > 31) )
        return GL_FAILURE;

#if (GL_TRACK_SYNC_MECHANISM == 1)
    th = (gsl_thread_t *)malloc(sizeof(gsl_thread_t) + GL_BLOCK_INFO_BUFFER_SIZE);
#else
    th = (gsl_thread_t *)malloc(sizeof(gsl_thread_t));
#endif

    if (NULL == th)
        return GL_FAILURE;
	
	boot_thread_count++;

	if(boot_thread_count < BOOT_THREADS)
		usleep(100);

    memset(th, 0, sizeof(gsl_thread_t));

	/*
    ** Set pthread attribute
    */
    pthread_attr_t thattr;
    err = pthread_attr_init(&thattr);
    if (err != 0) {
        perror_thread("pthread_attr_init", err);
        return GL_FAILURE;
    }

    /*
    ** Set stack size
    */
    if (0 != dStackSize) {
    dStackSize = dStackSize < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : dStackSize;
        err = pthread_attr_setstacksize(&thattr, (size_t)dStackSize);
    if (err != 0) {
        pwarning_thread("pthread_attr_setstacksize", err);
    }
    }
#if (GL_TRACK_SYNC_MECHANISM == 1)
    GL_MempoolFixCreate(szName, (void *)((UINT32)th + sizeof(gsl_thread_t)), GL_BLOCK_INFO_BUFFER_SIZE,
                        (sizeof(gsl_thread_block_info) + HAL_DCACHE_LINE_SIZE - 1) & ~(HAL_DCACHE_LINE_SIZE - 1), &(th->ext_info.pMempool));
#else
    th->ext_info.pMempool = NULL;
#endif

    /*
    ** Set priority ?? SCHED_RR
    */
    if (0 == geteuid()) {
        err = pthread_attr_setinheritsched(&thattr, PTHREAD_EXPLICIT_SCHED);
        if (err != 0) {
            pwarning_thread("pthread_attr_setinheritsched", err);
        }

        err = pthread_attr_setschedpolicy(&thattr, SCHED_RR);
        if (0 == err) {
            int minprio = sched_get_priority_min(SCHED_RR);
        if (-1 != minprio) {
                struct sched_param priority_holder;
                priority_holder.sched_priority = minprio + DEFAULT_PRIORITY - dPriority;
                err = pthread_attr_setschedparam(&thattr, &priority_holder);
                if (err != 0) {
                    pwarning_thread("pthread_attr_setschedparam", err);
                }
            }
            else {
                pwarning_thread("sched_get_priority_min", errno);
            }
        }
        else {
    		pwarning_thread("pthread_attr_setschedpolicy", err);
        }
    }
    else {
        printf("[##]I'm sorry [%d], should use root set schedpolicy ...\n", geteuid());
    }

    /*
    ** PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE
    */
    err = pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_JOINABLE);
    if (err != 0) {
        pwarning_thread("pthread_attr_setdetachstate", err);
    }

    /*
    ** data set
    */
    strncpy(th->name, szName, GSL_NAME_LEN);
    th->phandle = (GL_Task_t *)th;
    th->p_th_func = pEntryPoint;
    th->p_th_arg = pTaskArg;
	if (!fTaskRun) {
		th->scount = 1;
	}

    pthread_mutex_init(&th->lock, NULL);
    pthread_mutex_init(&th->slock, NULL);
    pthread_cond_init(&th->sready, NULL);
    pthread_once(&tsk_once, thread_once_init);

    *pTaskId = (GL_Task_t)th;
    /*
    ** Create thread
    */
#ifdef GL_OBJECT_LIST_TASK
	gsl_insert_list(&thread_dump_list, th);
#endif
    err = pthread_create(&th->threadid, &thattr, thread_thread_handler, (void *)th);
    if (err != 0) {
        perror_thread("pthread_create", err);
        return GL_FAILURE;
    }

    err = pthread_attr_destroy(&thattr);
    if (err != 0) {
        pwarning_thread("pthread_attr_destroy", err);
    }

    return GL_SUCCESS;
}

/*
 * GL_TaskCreate_SCHED_OTHER
 */
GL_Status_t GL_TaskCreate_SCHED_OTHER(char *szName, void (*pEntryPoint)(void *pArg), void *pTaskArg, GL_UINT32 dPriority, GL_UINT32 dStackSize, Boolean fTaskRun, GL_Task_t *pTaskId)
{
    gsl_thread_t *th = NULL;
    int err;

    /* Check for NULL pointers */
    if( (NULL == pEntryPoint) || (dPriority > 31) )
        return GL_FAILURE;

    //th = (gsl_thread_t *)malloc(sizeof(gsl_thread_t));
#if (GL_TRACK_SYNC_MECHANISM == 1)
    th = (gsl_thread_t *)malloc(sizeof(gsl_thread_t) + GL_BLOCK_INFO_BUFFER_SIZE);
#else
    th = (gsl_thread_t *)malloc(sizeof(gsl_thread_t));
#endif
    if (NULL == th)
        return GL_FAILURE;

    memset(th, 0, sizeof(gsl_thread_t));

    /*
    ** Set pthread attribute
    */
    pthread_attr_t thattr;
    err = pthread_attr_init(&thattr);
    if (err != 0) {
        perror_thread("pthread_attr_init", err);
        return GL_FAILURE;
    }

    /*
    ** Set stack size
    */
    if (0 != dStackSize) {
    dStackSize = dStackSize < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : dStackSize;
        err = pthread_attr_setstacksize(&thattr, (size_t)dStackSize);
    if (err != 0) {
        pwarning_thread("pthread_attr_setstacksize", err);
    }
    }
#if (GL_TRACK_SYNC_MECHANISM == 1)
    GL_MempoolFixCreate(szName, (void *)((UINT32)th + sizeof(gsl_thread_t)), GL_BLOCK_INFO_BUFFER_SIZE,
                        (sizeof(gsl_thread_block_info) + HAL_DCACHE_LINE_SIZE - 1) & ~(HAL_DCACHE_LINE_SIZE - 1), &(th->ext_info.pMempool));
#else
    th->ext_info.pMempool = NULL;
#endif

	/*
    ** Set priority ?? SCHED_OTHER
    */
    if (0 == geteuid()) {
        err = pthread_attr_setinheritsched(&thattr, PTHREAD_EXPLICIT_SCHED);
        if (err != 0) {
            pwarning_thread("pthread_attr_setinheritsched", err);
        }

        err = pthread_attr_setschedpolicy(&thattr, SCHED_OTHER);
        if (0 == err) {
            int minprio = sched_get_priority_min(SCHED_OTHER);
        if (-1 != minprio) {
                struct sched_param priority_holder;
                priority_holder.sched_priority = minprio;
                err = pthread_attr_setschedparam(&thattr, &priority_holder);
                if (err != 0) {
                    pwarning_thread("pthread_attr_setschedparam", err);
                }
            }
            else {
                pwarning_thread("sched_get_priority_min", errno);
            }
        }
        else {
    		pwarning_thread("pthread_attr_setschedpolicy", err);
        }
    }
    else {
        printf("[##]I'm sorry [%d], should use root set schedpolicy ...\n", geteuid());
    }

    /*
    ** PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE
    */
    err = pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_JOINABLE);
    if (err != 0) {
        pwarning_thread("pthread_attr_setdetachstate", err);
    }

    /*
    ** data set
    */
    strncpy(th->name, szName, GSL_NAME_LEN);
    th->phandle = (GL_Task_t *)th;
    th->p_th_func = pEntryPoint;
    th->p_th_arg = pTaskArg;
	if (!fTaskRun) {
		th->scount = 1;
	}
	th->priority = dPriority;

    pthread_mutex_init(&th->lock, NULL);
    pthread_mutex_init(&th->slock, NULL);
    pthread_cond_init(&th->sready, NULL);
    pthread_once(&tsk_once, thread_once_init);

    *pTaskId = (GL_Task_t)th;
    /*
    ** Create thread
    */
#ifdef GL_OBJECT_LIST_TASK
	gsl_insert_list(&thread_dump_list, th);
#endif
    err = pthread_create(&th->threadid, &thattr, thread_sched_other_handler, (void *)th);
    if (err != 0) {
        perror_thread("pthread_create", err);
        return GL_FAILURE;
    }

    err = pthread_attr_destroy(&thattr);
    if (err != 0) {
        pwarning_thread("pthread_attr_destroy", err);
    }

    return GL_SUCCESS;
}




/*
 * GL_TaskSelfDelete
 */
GL_Status_t GL_TaskSelfDelete(void)
{
#ifdef GL_OBJECT_LIST_TASK
    gsl_thread_t *th = pthread_getspecific(tsk_key);
	gsl_remove_list(&thread_dump_list, th);
#endif
#if (GL_TRACK_SYNC_MECHANISM == 1)
	/* Remove blocking info */
	gl_TaskFreeBlockInfo((GL_Task_t)th);
	GL_MempoolFixDestroy(th->ext_info.pMempool);
	th->ext_info.pMempool = NULL;
#endif
    pthread_exit(NULL);
    return GL_SUCCESS;
}

/*
 * GL_TaskActivate
 */
GL_Status_t GL_TaskActivate(GL_Task_t taskId)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);

    if (!th || !th->threadid) {
        return GL_FAILURE;
    }

    pthread_mutex_lock(&th->lock);
    if (th->scount > 0) {
//        printf("[##]%s(%lu) - %d RESUME.\n", th->name, pthread_self(), th->scount);
        pthread_mutex_lock(&th->slock);
        th->scount = th->scount - 1;
        pthread_mutex_unlock(&th->slock);
        pthread_cond_signal(&th->sready);
    }
    pthread_mutex_unlock(&th->lock);

    return GL_SUCCESS;
}

/*
 * GL_TaskSuspend
 */
GL_Status_t GL_TaskSuspend(GL_Task_t taskId)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);

    if (!th || !th->threadid) {
        return GL_FAILURE;
    }

    int err = pthread_kill(th->threadid, SIG_SUSPEND);
    if (err != 0) {
        perror_thread("pthread_kill", err);
    }

    return err ? GL_FAILURE : GL_SUCCESS;
}

/*
 * GL_TaskSleep
 */
GL_Status_t GL_TaskSleep(GL_UINT32 sdTimeout)
{
    if (sdTimeout > 1000) {
		unsigned int unslept = sdTimeout / 1000;

		/* Left time for usleep.  */
		sdTimeout = sdTimeout % 1000;

		/*
		 * Deduct overhead spends in the function.
		 */
		if (sdTimeout > 1) {
			sdTimeout -= 1;
		}

		/* Avoid being interrupt.  */
		while (unslept) {
			unslept = sleep(unslept);
		}
	}
    return usleep(1000 * sdTimeout) ? GL_FAILURE : GL_SUCCESS;
}

/*
 * GL_TaskYield
 */
GL_Status_t GL_TaskYield(void)
{
    sched_yield();
    return GL_SUCCESS;
}

/*
 * GL_TaskSetPriority
 */
GL_Status_t GL_TaskSetPriority(GL_Task_t taskId, GL_UINT32 dNewPriority)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);
    GL_Status_t ret = GL_SUCCESS;

    if (!th || !th->threadid || dNewPriority > 31) {
        return GL_FAILURE;
    }

    if (0 != geteuid()) {
        printf("[##]I'm sorry [%d], should use root set schedpolicy ...\n", geteuid());
        return GL_FAILURE;
    }

    int err, policy;
    pthread_attr_t thattr;

    pthread_mutex_lock(&th->lock);
    err = pthread_getattr_np(th->threadid, &thattr);
    if (err != 0) {
        perror_thread("pthread_getattr_np", err);
        ret = GL_FAILURE;
        goto __err_exit;
    }

    err = pthread_attr_getschedpolicy(&thattr, &policy);
    if (err != 0) {
        perror_thread("pthread_attr_getschedpolicy", err);
        ret = GL_FAILURE;
        goto __err_exit;
    }

    if (policy == SCHED_RR || policy == SCHED_FIFO) {
    int minprio = sched_get_priority_min(policy);
        if (-1 != minprio) {
            struct sched_param sched_param;

            sched_param.sched_priority = minprio + DEFAULT_PRIORITY - dNewPriority;
            err = pthread_setschedparam(th->threadid, policy, &sched_param);
            if (err != 0) {
                perror_thread("pthread_setschedprio", err);
                ret = GL_FAILURE;
                goto __err_exit;
            }
        }
    }
    else {
        printf("[##]I'm sorry. SCHED_OTHER do not support set schedpolicy ...\n");
    }

__err_exit:
    pthread_mutex_unlock(&th->lock);

    return ret;

}

/*
 * GL_TaskSetPriority_SCHED_OTHER
 */
GL_Status_t GL_TaskSetPriority_SCHED_OTHER(GL_Task_t taskId, GL_UINT32 dNewPriority)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);
    GL_Status_t ret = GL_SUCCESS;

    if (!th || !th->threadid || dNewPriority > 31) {
        return GL_FAILURE;
    }

    if (0 != geteuid()) {
        printf("[##]I'm sorry [%d], should use root set schedpolicy ...\n", geteuid());
        return GL_FAILURE;
    }
	pthread_mutex_lock(&th->lock);
	/* set priority */
	setpriority(PRIO_PROCESS, th->sched_other_tid, th->priority - DEFAULT_PRIORITY);
	pthread_mutex_unlock(&th->lock);

    return ret;

}



/*
 * GL_TaskGetPriority
 */
GL_Status_t GL_TaskGetPriority(GL_Task_t taskId, GL_UINT32 *pdPriority)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);
    GL_Status_t ret = GL_SUCCESS;

    *pdPriority = 0;
    if (!th || !th->threadid) {
        return GL_FAILURE;
    }

    int err, policy;
    pthread_attr_t thattr;

    pthread_mutex_lock(&th->lock);
    err = pthread_getattr_np(th->threadid, &thattr);
    if (err != 0) {
        perror_thread("pthread_getattr_np", err);
        ret = GL_FAILURE;
        goto __err_exit;
    }

    err = pthread_attr_getschedpolicy(&thattr, &policy);
    if (err != 0) {
        perror_thread("pthread_attr_getschedpolicy", err);
        ret = GL_FAILURE;
        goto __err_exit;
    }

    if (policy == SCHED_RR || policy == SCHED_FIFO) {
        struct sched_param priority_holder;
        err = pthread_attr_getschedparam(&thattr, &priority_holder);
        if (err != 0) {
            perror_thread("pthread_attr_getschedpolicy", err);
            ret = GL_FAILURE;
            goto __err_exit;
        }

        int minprio = sched_get_priority_min(SCHED_RR);
        if (-1 == minprio) {
            perror_thread("sched_get_priority_min", errno);
            ret = GL_FAILURE;
            goto __err_exit;
        }

        *pdPriority = minprio + DEFAULT_PRIORITY - priority_holder.sched_priority;
    }
    else {
        printf("[##]I'm sorry. SCHED_OTHER do not support set schedpolicy ...\n");
        ret = GL_FAILURE;
    }

__err_exit:
    pthread_mutex_unlock(&th->lock);

    return ret;
}


/*
 * GL_TaskGetPriority_SCHED_OTHER
 */
GL_Status_t GL_TaskGetPriority_SCHED_OTHER(GL_Task_t taskId, GL_UINT32 *pdPriority)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);
    GL_Status_t ret = GL_SUCCESS;

    *pdPriority = 0;
    if (!th || !th->threadid) {
        return GL_FAILURE;
    }
	pthread_mutex_lock(&th->lock);
	/* get priority */
    *pdPriority = getpriority(PRIO_PROCESS, th->sched_other_tid);
	pthread_mutex_unlock(&th->lock);

    return ret;
}



/*
 * GL_TaskGlobalLock
 */
GL_Status_t GL_TaskGlobalLock(void)
{
    return GL_SUCCESS;
}

GL_Status_t GL_TaskGlobalUnlock(void)
{
    return GL_SUCCESS;
}

/*
 * GL_CurrentTask
 */
GL_Task_t GL_CurrentTask(void)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    return (GL_Task_t) th;
}

GL_UINT32 GL_CurrentTaskDebug(void)
{
    return (GL_UINT32)pthread_self();
}

/*
 * GL_TaskDestroyOne
 */
GL_Status_t GL_TaskDestroyOne(GL_Task_t taskId)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);

    if (!th || !th->threadid) {
        return GL_FAILURE;
    }

    int err = pthread_cancel(th->threadid);
    if (err != 0) {
        perror_thread("pthread_cancel", err);
        return GL_FAILURE;
    }

    return GL_SUCCESS;
}

/*
 * GL_TaskGetMaxStackUsage
 */
GL_UINT32 GL_TaskGetMaxStackUsage(GL_Task_t task_id, int bAutoPrint)
{
    return 0;
}

char *GL_TaskName(GL_Task_t taskId)
{
    gsl_thread_t *th = (gsl_thread_t *)(taskId);

    if (!th || !th->threadid) {
        return NULL;
    }

    return th->name;
}

#if (GL_TRACK_SYNC_MECHANISM == 1)
void GL_TaskAddBlockInfo(UINT8 type, GL_UINT32 data)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    gsl_thread_ext_info *ext_info;
    if (NULL == th)
		return;
    ext_info = (gsl_thread_ext_info *)&th->ext_info;
    if (NULL != ext_info) {
        gsl_thread_block_info *blk_info;

        blk_info = GL_MempoolFixAlloc(ext_info->pMempool, GL_NO_WAIT);
        if (NULL != blk_info) {
            blk_info->handle = data;
            blk_info->type = type;
            /* insert head */
            blk_info->next = ext_info->block_info;
            ext_info->block_info = blk_info;
        }
    }
}

void GL_TaskDelBlockInfo(GL_UINT32 data)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    gsl_thread_ext_info *ext_info;
    if (NULL == th)
		return;
	
    ext_info = (gsl_thread_ext_info *)&th->ext_info;
    if (NULL != ext_info) {
        gsl_thread_block_info *blk_info = ext_info->block_info;
        gsl_thread_block_info *tmp = blk_info;
        while (NULL != blk_info) {
            if (data == blk_info->handle) {
                if (blk_info == ext_info->block_info) {
                    ext_info->block_info = blk_info->next;
                } else {
                    tmp->next = blk_info->next;
                }
                GL_MempoolFixFree(ext_info->pMempool, (void *)blk_info);
                break;
            }
            tmp = blk_info;
            blk_info = blk_info->next;
        }
    }
}

static void gl_TaskFreeBlockInfo(GL_Task_t task_id)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    gsl_thread_ext_info *ext_info;

    ext_info = (gsl_thread_ext_info *)&th->ext_info;
    if (NULL != ext_info) {
        gsl_thread_t *th = (gsl_thread_t *)task_id;
        gsl_thread_block_info *blk_info = ext_info->block_info;
        gsl_thread_block_info *tmp;
        while (NULL != blk_info) {
            tmp = blk_info->next;
            GL_MempoolFixFree(ext_info->pMempool, (void *)blk_info);
            blk_info = tmp;
        }
        th->ext_info.block_info = NULL;
    }
}

GL_UINT32 GL_TaskGetBlockData(void)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    gsl_thread_ext_info *ext_info;

    ext_info = (gsl_thread_ext_info *)&th->ext_info;
    if (NULL != ext_info) {
        if (NULL != ext_info->block_info) {
            return ext_info->block_info->handle;
        }
    }
    return 0;
}

UINT8 GL_TaskGetBlockType(void)
{
    gsl_thread_t *th = pthread_getspecific(tsk_key);
    gsl_thread_ext_info *ext_info;

//    ext_info = (gsl_thread_ext_info *)cyg_thread_get_ext_info(cyg_thread_self());
    ext_info = (gsl_thread_ext_info *)&th->ext_info;
    if (NULL != ext_info) {
        if (NULL != ext_info->block_info) {
            return ext_info->block_info->type;
        }
    }
    return GL_BLOCK_NONE;
}
#endif

#ifdef GL_OBJECT_LIST_TASK
UINT8 GL_TaskIsDupName(const char *name)
{
    gsl_list_t *p;
    gsl_thread_t *t;

    for (p = &thread_dump_list; p->next != NULL; p = p->next) {
        t = (gsl_thread_t *)p->next;
        if (0 == strncmp(name, t->name, GSL_NAME_LEN)) {
            return 1;
        }
    }
    return 0;
}

void GL_TaskDumpList(GL_UINT16 wChannel)
{
    gsl_list_t *p;
    gsl_thread_t *t;

    wChannel = wChannel;
    printf("\n----------GSL task dump list start ---------------\n");
    printf("name\ttask\n");
    for (p = &thread_dump_list; p->next != NULL; p = p->next) {
        t = (gsl_thread_t *)p->next;
        diag_printf("%s\n", t->name);
    }
    printf("\n----------GSL task dump list end ---------------\n");
}
#endif

static const GL_INT8 *gl_GetBlockName(GL_UINT32 handle, GL_UINT8 type)
{
	switch (type) {
	case GL_BLOCK_QUEUE:
		return ((gsl_queue_t *)handle)->name;
	case GL_BLOCK_SEMA:
		return (GL_INT8 *)((gsl_semaphore_t *)handle)->name;
	case GL_BLOCK_MUTEX:
		return ((gsl_mutex_t *)handle)->name;
	case GL_BLOCK_FLAG:
		//return GL_SUCCESS;
		return ((gsl_flag_t *)handle)->name;
	case GL_BLOCK_COND:
		//return GL_SUCCESS;
		return ((gsl_cond_t *)handle)->name;
	case GL_BLOCK_MEMPOOL_VAR:
		//return GL_SUCCESS;
		return ((gsl_mempool_var_t *)handle)->name;
	case GL_BLOCK_MEMPOOL_FIX:
		//return GL_SUCCESS;
		return ((gsl_mempool_fix_t *)handle)->name;
	default:			// Others
		return NULL;
	}
}

void GL_Show_ThreadStat(GL_INT32 detail)
{
#ifdef GL_OBJECT_LIST_TASK
	gsl_list_t *p;
	UINT8 b_type;
	gsl_thread_block_info *blk_info;
	const char *b_info_name;
	char szStatFilePath[64];
	int sdFd;
	char *str;
	char *szFileStat = malloc(2048);;
	int i;
	
	printf("%7s %10s %6s %9s %10s %7s %c %30s %24s\n", "TID", "Handle", "%CPU", "Priority", "StkBase", "StkSize", 'B', "Block Info", "Name");
	printf("-----------------------------------------------------------------------\n");

	long int sdCpuStat[9];
	for (p = &thread_dump_list; p->next != NULL; p = p->next) 
	{
		gsl_thread_t *t = (gsl_thread_t *)p->next;
		sprintf(szStatFilePath, "/proc/%d/task/%d/stat", (unsigned int) getpid(), (unsigned int) t->tid);
		sdFd = open("/proc/stat", O_RDONLY);
		if (sdFd > 0)
		{
			read(sdFd, szFileStat, 2048);
			close(sdFd);
			char szCpu[4];
			sscanf(szFileStat, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",szCpu, &sdCpuStat[0], &sdCpuStat[1], &sdCpuStat[2], &sdCpuStat[3], &sdCpuStat[4], &sdCpuStat[5], &sdCpuStat[6], &sdCpuStat[7], &sdCpuStat[8]);
			t->sdCpuSnap = 0;
			for (i = 0; i < 9; i++)
				t->sdCpuSnap += sdCpuStat[i];
		}
		
		sdFd = open(szStatFilePath, O_RDONLY);
		if (sdFd > 0)
		{
			read(sdFd, szFileStat, 2048);
			close(sdFd);
			str = strstr(szFileStat, ")")+2; // tid (name) other_information
			long int tmpint, utime, stime, cutime, cstime;
			char bState;
			sscanf(str, "%c %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &bState, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &utime, &stime, &cutime, &cstime);
			t->sdThreadSnap = utime + stime + cutime + cstime;;
		}
	}
	GL_TaskSleep(10);
	for (p = &thread_dump_list; p->next != NULL; p = p->next) 
	{
		gsl_thread_t *t;
		pthread_attr_t attr;
		unsigned int stackaddr;
		size_t stacksize;
		struct sched_param s;
		long int sdCurThreadSnap = 0, sdCurCpuSnap = 0;

		t = (gsl_thread_t *)p->next;
		pthread_getattr_np(t->threadid, &attr);
		pthread_attr_getschedparam(&attr, &s);
		pthread_attr_getstack(&attr, (void **)&stackaddr, &stacksize);

		b_type = GL_BLOCK_NONE;
		b_info_name = " ";
		blk_info = NULL;
		sprintf(szStatFilePath, "/proc/%d/task/%d/stat", (unsigned int) getpid(), (unsigned int) t->tid);
		
		sdFd = open("/proc/stat", O_RDONLY);
		if (sdFd > 0)
		{
			read(sdFd, szFileStat, 2048);
			close(sdFd);
			char szCpu[4];
			sscanf(szFileStat, "%s %ld %ld %ld %ld %ld %ld %ld %ld %ld\n",szCpu, &sdCpuStat[0], &sdCpuStat[1], &sdCpuStat[2], &sdCpuStat[3], &sdCpuStat[4], &sdCpuStat[5], &sdCpuStat[6], &sdCpuStat[7], &sdCpuStat[8]);
			sdCurCpuSnap = 0;
			for (i = 0; i < 9; i++)
				sdCurCpuSnap += sdCpuStat[i];
		}		
		// Get thread state
		sdFd = open(szStatFilePath, O_RDONLY);
		if (sdFd > 0)
		{
			read(sdFd, szFileStat, 2048);
			close(sdFd);
			str = strstr(szFileStat, ")")+2; // tid (name) other_information
			long int tmpint, utime, stime, cutime, cstime;
			char bState;
			sscanf(str, "%c %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld", &bState, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &tmpint, &utime, &stime, &cutime, &cstime);
			sdCurThreadSnap = utime + stime + cutime + cstime;
		}

		// Get block type
		if (NULL != t->ext_info.block_info) {
			blk_info =t->ext_info.block_info;
			if (NULL != blk_info) {
				b_type = blk_info->type;
				b_info_name = gl_GetBlockName(blk_info->handle, b_type);
				blk_info = blk_info->next;
			}
		}
		//if (sdCurThreadSnap != t->sdThreadSnap)
			//printf("sdCurThreadSnap = %ld, t->sdThreadSnap = %ld, sdCurCpuSnap = %ld, sdLastCpuSnap = %ld\n", sdCurThreadSnap, t->sdThreadSnap, sdCurCpuSnap, t->sdCpuSnap);
		printf("%7u 0x%08X %2ld.%03ld %3d (%2d) 0x%08X %7u %c %30s %24s\n",
			   (unsigned int)t->tid,
			   (unsigned int)t->threadid,
			   (long int)(sdCurThreadSnap - t->sdThreadSnap)*100 / (sdCurCpuSnap - t->sdCpuSnap) ,
			   (long int)((sdCurThreadSnap - t->sdThreadSnap)*100 % (sdCurCpuSnap - t->sdCpuSnap)) * 1000 / (sdCurCpuSnap - t->sdCpuSnap),
			   32 - s.sched_priority,
			   s.sched_priority,
			   stackaddr,
			   stacksize,
			   b_type,
			   b_info_name,
			   t->name);
    }
    	free(szFileStat);
	printf("boot_thread_count:%d\n",boot_thread_count);
#else
	printf("Enable definition \"GL_OBJECT_LIST_TASK\" to support this.\n");
#endif
}

void GL_TaskUninit(void)
{
#ifdef GL_OBJECT_LIST_TASK
	gsl_list_t *p;


	p = &thread_dump_list;

	if (!p->next) {
		return;
	}
    for (p = (gsl_list_t *)thread_dump_list.next; p != NULL; ) {
		gsl_thread_t *t;

        t = (gsl_thread_t *)p;
		p = p->next;
		gsl_printf("delete thread \"%s\"\n", t->name);
		pthread_cancel(t->threadid);
	}
#endif
}

