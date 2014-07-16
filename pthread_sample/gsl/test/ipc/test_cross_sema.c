#include "test.h"

static GL_Semaphore_t cross_semid = NULL;


static void* thread_sema_creat(void *arg)
{
	static int count_create=0,  count_take;

	while(1){
		GL_TraceInfo("[%s]GL_SemaphoreCreate: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_SemaphoreCreate("semTestCross", 16, 0, &cross_semid)){
			GL_TraceError("[%s-%4d]GL_SemaphoreCreate fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		GL_TraceInfo("[%s]GL_SemaphoreCreate: addr=0x%x\n", __FUNCTION__, (unsigned int)cross_semid);

		for(count_take=0; count_take<1000; count_take++){
		//while(1){
			GL_TraceInfo("[%s]GL_SemaphoreTake: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_SemaphoreTake(cross_semid, GL_INFINITE_WAIT)){
				GL_TraceError("[%s-%4d]GL_SemaphoreTake GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
				exit(1); //break;
			}

			GL_TraceInfo("[%s]GL_SemaphoreTake: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_SemaphoreTake(cross_semid, GL_NO_WAIT)){
				GL_TraceError("[%s-%4d]GL_SemaphoreTake GL_NO_WAIT fail\n", __FUNCTION__, __LINE__);
			}

			GL_TraceInfo("[%s]GL_SemaphoreTake: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_SemaphoreTake(cross_semid, 1000)){
				GL_TraceError("[%s-%4d]GL_SemaphoreTake TIMED_OUT_1000 fail\n", __FUNCTION__, __LINE__);
			}
		}

		GL_TraceInfo("[%s]GL_SemaphoreDelete: count_creat=%d\n", __FUNCTION__, count_create);
		GL_TraceInfo("[%s]GL_SemaphoreDelete: addr=0x%x\n", __FUNCTION__, (unsigned int)cross_semid);
		if(GL_SUCCESS != GL_SemaphoreDelete(cross_semid)){
			GL_TraceError("[%s-%4d]GL_SemaphoreDelete fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}
		cross_semid = NULL;
		count_create++;
	}

	return NULL;
}

static void* thread_sema_take_10Hz(void *arg)
{
	while(1){
		GL_TraceInfo("[%s]GL_SemaphoreTake\n", __FUNCTION__);
		if(GL_SUCCESS != GL_SemaphoreTake(cross_semid, GL_INFINITE_WAIT)){
			GL_TraceError("[%s-%4d]GL_SemaphoreTake GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
			exit(1); //
		}
		GL_TaskSleep(100);
	}
	return NULL;
}

static void* thread_sema_give_10Hz(void *arg)
{
	while(1){
		GL_TraceInfo("[%s]GL_SemaphoreGive\n", __FUNCTION__);
		if(GL_SUCCESS != GL_SemaphoreGive(cross_semid)){
			GL_TraceError("[%s-%4d]GL_SemaphoreGive  fail\n", __FUNCTION__, __LINE__);
			exit(1); //
		}
		GL_TaskSleep(100);
	}
	return NULL;
}

static void timer_sema_give_50Hz(void *arg)
{
	GL_TraceInfo("[%s]GL_SemaphoreGive\n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreGive(cross_semid)){
		GL_TraceError("[%s-%4d]GL_SemaphoreGive  fail\n", __FUNCTION__, __LINE__);
		exit(1); //
	}
}

static void timer_sema_give_10Hz(void *arg)
{
	GL_TraceInfo("[%s]GL_SemaphoreGive\n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreGive(cross_semid)){
		GL_TraceError("[%s-%4d]GL_SemaphoreGive  fail\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

}

static void timer_sema_peek_1Hz(void *arg)
{
	GL_UINT32 value;

	GL_TraceInfo("[%s]GL_SemaphorePeek: addr=0x%x\n", __FUNCTION__, (unsigned int)cross_semid);
	if(GL_SUCCESS != GL_SemaphorePeek(cross_semid, &value)){
		GL_TraceError("[%s-%4d]GL_SemaphorePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1); //
	}else{
		GL_TraceInfo("[%s-%4d]GL_SemaphorePeek  %d\n", __FUNCTION__, __LINE__, value);
	}
}

GL_Status_t test_GL_Cross_Semaphore(void)
{
//	GL_Status_t ret = GL_SUCCESS;
	pthread_t  pthread_id1, pthread_id2, pthread_id3;
	timer_t timerid_50Hz, timerid_10Hz, timerid_1Hz;

	GL_TraceInfo("[%s-%4d] enter\n", __FUNCTION__, __LINE__);

	/*create the create thread*/
	if(0 != pthread_create(&pthread_id1, NULL, thread_sema_creat, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	/*create the give thread*/
	if(0 != pthread_create(&pthread_id2, NULL, thread_sema_give_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the take thread*/
	if(0 != pthread_create(&pthread_id3, NULL, thread_sema_take_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	/*create the give timer*/
	if(0 != cross_GL_TimerCreate("tcross_sem_50hz", timer_sema_give_50Hz, NULL, 1, 20, 1, &timerid_50Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	if(0 != cross_GL_TimerCreate("tcross_sem_10hz", timer_sema_give_10Hz, NULL, 1, 100, 1, &timerid_10Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	if(0 != cross_GL_TimerCreate("tcross_sem_1hz", timer_sema_peek_1Hz, NULL, 1, 1000, 1, &timerid_1Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1); //
	}

	GL_TraceInfo("[%s-%4d] exit\n", __FUNCTION__, __LINE__);

	return GL_SUCCESS;
}
