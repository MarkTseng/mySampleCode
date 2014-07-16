/*
 * gl_timer.c
 *
 */
#include <gsl.h>

#define perror_timer(prestr, err)    \
    fprintf(stderr, "[#e]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

#define pwarning_timer(prestr, err)    \
    fprintf(stderr, "[#w]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

/*
 * the type of the timer
 */
typedef struct gsl_timer {
    char name[GSL_NAME_LEN + 1];
    void (*p_timer_func)(void *p_arg);
    void *p_timer_arg;
    
    timer_t timerid;
    Boolean is_periodic;
    GL_UINT32 dInterval;
    
    /* timer mutex */
    pthread_mutex_t lock;
    
    /* timer delete */
    Boolean active;
    GL_UINT32 thcnt[2];
    pthread_mutex_t qlock;
    pthread_cond_t qcond;
} gsl_timer_t;

/*
 *
 */
//static pthread_mutex_t tm_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 *
 */
static void tm_thread(union sigval sigv)
{
    gsl_timer_t *tm = sigv.sival_ptr;

    if (!tm)
        return;
 
    tm->thcnt[0]++;
    tm->p_timer_func(tm->p_timer_arg);
    tm->thcnt[1]++;
    
    if (!tm->active) {
        pthread_cond_signal(&tm->qcond);
    }
}

/*
 * GL_TimerCreate
 */
GL_Status_t GL_TimerCreate(char *szName, void (*pTimerFunction)(void *pArg), void *pTimerArg, Boolean fTimerStart, GL_UINT32 dInterval, Boolean fPeriodic, GL_Timer_t *pTimerId)
{
    gsl_timer_t *tm = NULL;
    timer_t timerid = NULL;
    struct sigevent sev;
    struct itimerspec its;
    int err;
    pthread_attr_t thattr;
 
    if (NULL == pTimerFunction)
        return GL_FAILURE;
    
    tm = (gsl_timer_t *)malloc(sizeof(gsl_timer_t));
    if (NULL == tm)
        return GL_FAILURE;
 
    bzero(tm, sizeof(gsl_timer_t));

	/*
    ** Set pthread attribute
    */
    err = pthread_attr_init(&thattr);
    if (err != 0) {
        perror("pthread_attr_init");
        return GL_FAILURE;
    }

	/*
	 ** Set priority ?? SCHED_RR
	 */
	if (0 == geteuid()) {
		err = pthread_attr_setinheritsched(&thattr, PTHREAD_EXPLICIT_SCHED);
		if (err != 0) {
			printf("pthread_attr_setinheritsched");
		}

		err = pthread_attr_setschedpolicy(&thattr, SCHED_RR);
		if (0 == err) {
			int minprio = sched_get_priority_min(SCHED_RR);
			if (-1 != minprio) {
				struct sched_param priority_holder;
				priority_holder.sched_priority = 29;
				err = pthread_attr_setschedparam(&thattr, &priority_holder);
				if (err != 0) {
					printf("pthread_attr_setschedparam");
				}
			}
			else {
				printf("sched_get_priority_min");
			}
		}
		else {
			printf("pthread_attr_setschedpolicy");
		}
	}
	else {
		printf("[##]I'm sorry [%d], should use root set schedpolicy ...\n", geteuid());
	}

	/* set policy */
	pthread_attr_setdetachstate(&thattr, PTHREAD_CREATE_DETACHED);

    /* Create the timer */
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_value.sival_ptr = tm;
    sev.sigev_notify_attributes = &thattr;
    sev.sigev_notify_function = tm_thread;
    
    if (timer_create(CLOCK_REALTIME, &sev, &timerid)) {
        perror_timer("timer_create", errno);
        goto __err_exit;
    }

    strncpy(tm->name, szName, GSL_NAME_LEN);
    tm->p_timer_func = pTimerFunction;
    tm->p_timer_arg = pTimerArg;
    tm->dInterval = dInterval;
    tm->is_periodic = fPeriodic;
    tm->timerid = timerid;
    
    tm->active = 1;
    pthread_mutex_init(&tm->lock, NULL);
    pthread_mutex_init(&tm->qlock, NULL);
    pthread_cond_init(&tm->qcond, NULL);
    
    if (fTimerStart) {
        its.it_value.tv_sec = dInterval / 1000;
        its.it_value.tv_nsec = (dInterval % 1000) * 1000000;
        its.it_interval.tv_sec = fPeriodic ? its.it_value.tv_sec : 0;
        its.it_interval.tv_nsec = fPeriodic ? its.it_value.tv_nsec : 0;

        if (timer_settime(timerid, 0, &its, NULL))  {
            perror_timer("timer_settime", errno);
            goto __err_exit2;
        }
    }
    
    *pTimerId = (GL_Timer_t)tm;
    return GL_SUCCESS;

__err_exit2:
    timer_delete(timerid);

__err_exit:
    free(tm);

    return GL_FAILURE;
}

/*
 * GL_TimerDelete
 */
#define GL_TMDEL_TIMEOUT   5000
GL_Status_t GL_TimerDelete(GL_Timer_t timerId)
{
    gsl_timer_t *tm = (gsl_timer_t *)timerId;
    int err;

    if (!tm)
        return GL_FAILURE;

    struct itimerspec its;
    pthread_mutex_lock(&tm->lock); // ?
    memset(&its, 0, sizeof(struct itimerspec));
    if (timer_settime(tm->timerid, 0, &its, NULL)) {
        pthread_mutex_unlock(&tm->lock);
        perror_timer("timer_settime", errno);
        return GL_FAILURE;
    }

    while (tm->thcnt[0] > tm->thcnt[1]) {
        pthread_mutex_lock(&tm->qlock);
        tm->active = 0;
        err = pthread_cond_wait(&tm->qcond, &tm->qlock);
        if (err != 0) {
            pwarning_timer("pthread_cond_wait", err);
        }
        pthread_mutex_unlock(&tm->qlock);
        
//        printf("[#W]%s timeout(%d - %d). \n", tm->name, tm->thcnt[0], tm->thcnt[1]);
    }

    if (timer_delete(tm->timerid)) {
        pthread_mutex_unlock(&tm->lock);
        perror_timer("timer_delete", errno);
        return GL_FAILURE;
    }
    
    pthread_mutex_destroy(&tm->qlock);
    tm->timerid = (timer_t)~0;
    pthread_mutex_unlock(&tm->lock);
    pthread_mutex_destroy(&tm->lock);
    free(tm);
    
    return GL_SUCCESS;
}

/*
 * GL_TimerDisable
 */
GL_Status_t GL_TimerDisable(GL_Timer_t timerId)
{
    gsl_timer_t *tm = (gsl_timer_t *)timerId;

    if (!tm)
        return GL_FAILURE;

    struct itimerspec its;
    pthread_mutex_lock(&tm->lock);
    memset(&its, 0, sizeof(struct itimerspec));
    if (timer_settime(tm->timerid, 0, &its, NULL)) {
        pthread_mutex_unlock(&tm->lock);
        perror_timer("timer_settime", errno);
        return GL_FAILURE;
    }
    pthread_mutex_unlock(&tm->lock);
    
    return GL_SUCCESS;
}

/*
 * GL_TimerEnable
 */
GL_Status_t GL_TimerEnable(GL_Timer_t timerId)
{
    gsl_timer_t *tm = (gsl_timer_t *)timerId;

    if (!tm)
        return GL_FAILURE;

    struct itimerspec its;
    pthread_mutex_lock(&tm->lock);
    its.it_value.tv_sec = tm->dInterval / 1000;
    its.it_value.tv_nsec = (tm->dInterval % 1000) * 1000000;
    its.it_interval.tv_sec = tm->is_periodic ? its.it_value.tv_sec : 0;
    its.it_interval.tv_nsec = tm->is_periodic ? its.it_value.tv_nsec : 0;

    if (timer_settime(tm->timerid, 0, &its, NULL)) {
        pthread_mutex_unlock(&tm->lock);
        perror_timer("timer_settime", errno);
        return GL_FAILURE;
    }
    pthread_mutex_unlock(&tm->lock);

    return GL_SUCCESS;
}

/*
 * GL_TimerSchedule
 */
GL_Status_t GL_TimerSchedule(GL_Timer_t timerId, GL_UINT32 dNewInterval)
{
    gsl_timer_t *tm = (gsl_timer_t *)timerId;

    if (!tm)
        return GL_FAILURE;
        
    struct itimerspec its;
    pthread_mutex_lock(&tm->lock);
    its.it_value.tv_sec = dNewInterval / 1000;
    its.it_value.tv_nsec = (dNewInterval % 1000) * 1000000;
    its.it_interval.tv_sec = tm->is_periodic ? its.it_value.tv_sec : 0;
    its.it_interval.tv_nsec = tm->is_periodic ? its.it_value.tv_nsec : 0;
        
    if (timer_settime(tm->timerid, 0, &its, NULL)) {
        pthread_mutex_unlock(&tm->lock);
        perror_timer("timer_settime", errno);
        return GL_FAILURE;
    }

    tm->dInterval = dNewInterval;
    pthread_mutex_unlock(&tm->lock);
    
    return GL_SUCCESS;
}

/*
 * 
 */
GL_UINT32 GL_GetTimestamp(void)
{
#if 0   // tick, * 10 ms ? PC : alias 10ms
    clock_t tick = (clock_t)times(NULL);
    unsigned int hz = sysconf(_SC_CLK_TCK);
	return (unsigned int)tick * 1000 / hz;
#else
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (unsigned int)(tp.tv_sec * 1000 + tp.tv_nsec / 1000000);
#endif
}

GL_UINT32 GL_CalcElapsedTimeMsec(GL_UINT32 dTimestamp)
{
	dTimestamp = dTimestamp;
	return 0;
}

char *GL_TimerName(GL_Timer_t timerId)
{
    gsl_timer_t *tm = (gsl_timer_t *)timerId;

    if (!tm)
        return "(bad timer)";

    return tm->name;
}
