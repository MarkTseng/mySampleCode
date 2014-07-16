#include "test.h"


static GL_Mutex_t cross_mutex = NULL;


static void* thread_mutex_creat(void *arg)
{
	static int count_create=0,  count_take;

	while(1){
		GL_TraceInfo("[%s]GL_MutexCreate: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_MutexCreate("mutexTestCross", &cross_mutex)){
			GL_TraceError("[%s-%4d]GL_MutexCreate fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		for(count_take=0; count_take<1000; count_take++){

			GL_TraceInfo("[%s]GL_MutexLock: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_MutexLock(cross_mutex)){
				GL_TraceError("[%s-%4d]GL_MutexLock  fail\n", __FUNCTION__, __LINE__);
				exit(1); //break;
			}

			GL_TaskSleep(2);

			GL_TraceInfo("[%s]GL_MutexUnlock: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_MutexUnlock(cross_mutex)){
				GL_TraceError("[%s-%4d]GL_MutexUnlock  fail\n", __FUNCTION__, __LINE__);
				exit(1); //break;
			}

			GL_TaskSleep(98);
		}

		GL_TraceInfo("[%s]GL_MutexDelete\n", __FUNCTION__);
		if(GL_SUCCESS != GL_MutexDelete(cross_mutex)){
			GL_TraceError("[%s-%4d]GL_MutexDelete fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		cross_mutex = NULL;
		count_create++;
	}
	return NULL;
}

static void* thread_mutex_10Hz(void *arg)
{
	while(1){

		GL_TraceInfo("[%s]GL_MutexLock\n", __FUNCTION__);
		if(GL_SUCCESS != GL_MutexLock(cross_mutex)){
			GL_TraceError("[%s-%4d]GL_MutexLock  fail\n", __FUNCTION__, __LINE__);
			exit(1); //break;
		}

		GL_TaskSleep(2);

		GL_TraceInfo("[%s]GL_MutexUnlock\n", __FUNCTION__);
		if(GL_SUCCESS != GL_MutexUnlock(cross_mutex)){
			GL_TraceError("[%s-%4d]GL_MutexUnlock  fail\n", __FUNCTION__, __LINE__);
			exit(1); //break;
		}

		GL_TaskSleep(98);
	}
	return NULL;
}


static void timer_mutex_50Hz(void *arg)
{
	GL_TraceInfo("[%s]GL_MutexTryLock\n", __FUNCTION__);
	if(GL_SUCCESS != GL_MutexTryLock(cross_mutex)){
		GL_TraceError("[%s-%4d]GL_MutexTryLock  fail\n", __FUNCTION__, __LINE__);
		return;
	}

	GL_TaskSleep(2);

	GL_TraceInfo("[%s]GL_MutexUnlock\n", __FUNCTION__);
	if(GL_SUCCESS != GL_MutexUnlock(cross_mutex)){
		GL_TraceError("[%s-%4d]GL_MutexUnlock  fail\n", __FUNCTION__, __LINE__);
		exit(1); //
	}
}

static void timer_mutex_1Hz(void *arg)
{
	GL_TraceInfo("[%s]GL_MutexTryLock\n", __FUNCTION__);
	if(GL_SUCCESS != GL_MutexTryLock(cross_mutex)){
		GL_TraceError("[%s-%4d]GL_MutexTryLock  fail\n", __FUNCTION__, __LINE__);
		return;
	}

	GL_TaskSleep(2);

	GL_TraceInfo("[%s]GL_MutexUnlock\n", __FUNCTION__);
	if(GL_SUCCESS != GL_MutexUnlock(cross_mutex)){
		GL_TraceError("[%s-%4d]GL_MutexUnlock  fail\n", __FUNCTION__, __LINE__);
		exit(1); //
	}
}

GL_Status_t test_GL_Cross_Mutex(void)
{
//	GL_Status_t ret = GL_SUCCESS;
	pthread_t  pthread_id1, pthread_id2;
	timer_t timerid_50Hz, timerid_1Hz;

	GL_TraceInfo("[%s-%4d] enter\n", __FUNCTION__, __LINE__);

	/*create the create thread*/
	if(0 != pthread_create(&pthread_id1, NULL, thread_mutex_creat, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

#if 1
	/*create the lock/unlock thread*/
	if(0 != pthread_create(&pthread_id2, NULL, thread_mutex_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	/*create the give timer*/
	if(0 != cross_GL_TimerCreate("tcross_mutex_50hz", timer_mutex_50Hz, NULL, 1, 20, 1, &timerid_50Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	if(0 != cross_GL_TimerCreate("tcross_mutex_1hz", timer_mutex_1Hz, NULL, 1, 1000, 1, &timerid_1Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}
#endif

	GL_TraceInfo("[%s-%4d] exit\n", __FUNCTION__, __LINE__);

	return GL_SUCCESS;
}
