#include "test.h"

#if 1

typedef void (*timer_func)(sigval_t);

GL_Status_t cross_GL_TimerCreate(
	char *szName, void (*function)(void*), void *pTimerArg, int fTimerStart,
	GL_UINT32 dInterval, int fPeriodic, timer_t * pTimerId)
{
	struct sigevent   se;
  	struct itimerspec   ts,   ots;

	se.sigev_notify   		= SIGEV_THREAD;
	se.sigev_signo		= 0;
	se.sigev_value.sival_int	= 0;
	se.sigev_value.sival_ptr	= pTimerArg;
	se.sigev_notify_function =   (timer_func)function;
	se.sigev_notify_attributes = NULL;

	if( timer_create(CLOCK_REALTIME, &se, pTimerId) < 0) {
		GL_TraceError("GL_TimerCreate: create fail! errno=%d\r\n", errno);
		return GL_FAILURE;
	}

	ts.it_value.tv_sec   =   dInterval/1000;
	ts.it_value.tv_nsec   =   (dInterval%1000)*1000000;
	ts.it_interval.tv_sec   =   ts.it_value.tv_sec ;
	ts.it_interval.tv_nsec   =   ts.it_value.tv_nsec;

      if( timer_settime(*pTimerId, 0, &ts,  &ots) < 0){ //TIMER_ABSTIME

		GL_TraceError("GL_TimerCreate: settime fail! errno=%d\r\n", errno);
		if(timer_delete(*pTimerId)<0){
			GL_TraceError("GL_TimerCreate: timer_delete fail! errno=%d\r\n", errno);
			return GL_FAILURE;
		}

		return GL_FAILURE;
	}

	return GL_SUCCESS;
}


GL_Status_t cross_GL_TimerDelete(timer_t timerId)
{
	if(timer_delete(timerId)<0){
		GL_TraceError("GL_TimerDelete fail! errno=%d\r\n", errno);
		return GL_FAILURE;
	}

	return GL_SUCCESS;
}
#endif


GL_Status_t Test_GL_Cross(void)
{
	int i;
	struct {
		char *name;
		GL_Status_t (*func)(void);
		int result;
	} test_patrn[] = {
		{"test_GL_Cross_Semaphore", test_GL_Cross_Semaphore, 0},
		{"test_GL_Cross_Mutex", test_GL_Cross_Mutex, 0},
		{"test_GL_Cross_Queue", test_GL_Cross_Queue, 0},
		{"test_GL_Cross_List", test_GL_Cross_List, 0},
		{"test_GL_Cross_Flag", test_GL_Cross_Flag, 0},
	};

	for (i = 0; i < sizeof(test_patrn) / sizeof(test_patrn[0]); ++i) {
		if (GL_SUCCESS == test_patrn[i].func()) {
			test_patrn[i].result = 1;
		}
	}

	GL_TraceError("\n\n[Test results]:\n");
	for (i = 0; i < sizeof(test_patrn) / sizeof(test_patrn[0]); ++i) {
		GL_TraceError("\t%s %s !!!\n", test_patrn[i].name,
		(0 == test_patrn[i].result) ? ("FAIL") : ("pass"));
	}

	return GL_SUCCESS;

	while (getchar() != 'q') {
		sleep(1);
	}

	return GL_SUCCESS;
}
