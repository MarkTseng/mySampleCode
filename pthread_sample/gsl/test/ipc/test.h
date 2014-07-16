#ifndef __TEST_H__
#define __TEST_H__

#include "gsl.h"

extern GL_Status_t Test_GL_Semaphore(void);
extern GL_Status_t Test_GL_Mutex(void);
extern GL_Status_t Test_GL_Queue(void);
extern GL_Status_t Test_GL_List(void);
extern GL_Status_t Test_GL_Flag(void);
extern GL_Status_t Test_GL_Cross(void);

extern GL_Status_t test_GL_Cross_Semaphore(void);
extern GL_Status_t test_GL_Cross_Mutex(void);
extern GL_Status_t test_GL_Cross_Queue(void);
extern GL_Status_t test_GL_Cross_List(void);
extern GL_Status_t test_GL_Cross_Flag(void);

#if 1
extern GL_Status_t cross_GL_TimerCreate(
	char *szName, void (*function)(void*), void *pTimerArg, int fTimerStart,
	GL_UINT32 dInterval, int fPeriodic, timer_t * pTimerId);
extern GL_Status_t cross_GL_TimerDelete(timer_t timerId);
#endif

#endif
