#include "test.h"


static GL_Flag_t cross_flagid = NULL;


static void* thread_flag_creat(void *arg)
{
	static int count_create=0,  count_take;
	GL_UINT32 ret;

	while(1){
		GL_TraceInfo("[%s]GL_FlagCreate: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_FlagCreate("flagTestCross",&cross_flagid)){
			GL_TraceError("[%s-%4d]GL_FlagCreate fail\n", __FUNCTION__, __LINE__);
			return NULL;
		}

		for(count_take=0; count_take<1000; count_take++){

			GL_TraceInfo("[%s]GL_FlagWait: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(0 == (ret = GL_FlagWait(cross_flagid, 0x03, GL_FLAG_WAITMODE_AND, GL_INFINITE_WAIT))){
				GL_TraceError("[%s-%4d]GL_FlagWait GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
				exit(1);
			}
			GL_TraceInfo("[%s]GL_FlagWait flag=%d\n", __FUNCTION__, ret);

			GL_TraceInfo("[%s]GL_FlagWait: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(0 == (ret = GL_FlagWait(cross_flagid, 0x03, GL_FLAG_WAITMODE_AND | GL_FLAG_WAITMODE_CLR, GL_NO_WAIT))){
				GL_TraceError("[%s-%4d]GL_FlagWait GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
			}
			GL_TraceInfo("[%s]GL_FlagWait flag=%d\n", __FUNCTION__, ret);

			GL_TraceInfo("[%s]GL_FlagWait: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(0 == (ret = GL_FlagWait(cross_flagid, 0x03, GL_FLAG_WAITMODE_AND | GL_FLAG_WAITMODE_CLR, 800))){
				GL_TraceError("[%s-%4d]GL_FlagWait GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
				exit(1);
			}
			GL_TraceInfo("[%s]GL_FlagWait flag=%d\n", __FUNCTION__, ret);

		}

		GL_TraceInfo("[%s]GL_FlagDelete: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_FlagDelete(cross_flagid)){
			GL_TraceError("[%s-%4d]GL_FlagDelete fail\n", __FUNCTION__, __LINE__);
			return NULL;
		}
		cross_flagid = NULL;
		count_create++;
	}
	return NULL;
}

static void* thread_flag_wait_50Hz(void *arg)
{
	GL_UINT32 ret;
	while(1){
		GL_TraceInfo("[%s]GL_FlagWait\n", __FUNCTION__);
		if(0 == (ret = GL_FlagWait(cross_flagid, 0x03, GL_FLAG_WAITMODE_OR, GL_INFINITE_WAIT))){
			GL_TraceError("[%s-%4d]GL_FlagWait GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
			exit(1);
		}
		GL_TraceInfo("[%s]GL_FlagWait flag=%d\n", __FUNCTION__, ret);
		GL_TaskSleep(20);
	}
	return NULL;
}

static void* thread_flag_wait_10Hz(void *arg)
{
	GL_UINT32 ret;
	while(1){
		GL_TraceInfo("[%s]GL_FlagWait\n", __FUNCTION__);
		if(0 == (ret = GL_FlagWait(cross_flagid, 0x03, GL_FLAG_WAITMODE_OR, GL_INFINITE_WAIT))){
			GL_TraceError("[%s-%4d]GL_FlagWait GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
			exit(1);
		}
		GL_TraceInfo("[%s]GL_FlagWait flag=%d\n", __FUNCTION__, ret);
		GL_TaskSleep(100);
	}
	return NULL;
}

static void* thread_flag_set_1Hz(void *arg)
{
	GL_UINT32 value;

	while(1){
		GL_TraceInfo("[%s]GL_FlagSetbits\n", __FUNCTION__);
		if(GL_SUCCESS != GL_FlagSetbits(cross_flagid, 0x2)){
			GL_TraceError("[%s-%4d]GL_FlagSetbits  fail\n", __FUNCTION__, __LINE__);
			exit(1);
		}

		if(GL_SUCCESS != GL_FlagPeek(cross_flagid, &value)){
			GL_TraceError("[%s-%4d]GL_FlagPeek  fail\n", __FUNCTION__, __LINE__);
			exit(1);
		}
		GL_TraceInfo("[%s]GL_FlagPeek  %d\n", __FUNCTION__, value);

		GL_TaskSleep(1000);
	}
	return NULL;
}

static void timer_flag_set_50Hz(void *arg)
{
	GL_UINT32 value;

	GL_TraceInfo("[%s]GL_FlagSetbits\n", __FUNCTION__);
	if(GL_SUCCESS != GL_FlagSetbits(cross_flagid, 0x1)){
		GL_TraceError("[%s-%4d]GL_FlagSetbits  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	if(GL_SUCCESS != GL_FlagPeek(cross_flagid, &value)){
		GL_TraceError("[%s-%4d]GL_FlagPeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}
	GL_TraceInfo("[%s]GL_FlagPeek  %d\n", __FUNCTION__, value);
}

static void timer_flag_set_10Hz(void *arg)
{
	GL_UINT32 value;

	GL_TraceInfo("[%s]GL_FlagSetbits\n", __FUNCTION__);
	if(GL_SUCCESS != GL_FlagSetbits(cross_flagid, 0x2)){
		GL_TraceError("[%s-%4d]GL_FlagSetbits  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	if(GL_SUCCESS != GL_FlagPeek(cross_flagid, &value)){
		GL_TraceError("[%s-%4d]GL_FlagPeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}
	GL_TraceInfo("[%s]GL_FlagPeek  %d\n", __FUNCTION__, value);
}

static void timer_flag_peek_1Hz(void *arg)
{
	GL_UINT32 value;

	GL_TraceInfo("[%s]GL_FlagPeek\n", __FUNCTION__);
	if(GL_SUCCESS != GL_FlagPeek(cross_flagid, &value)){
		GL_TraceError("[%s-%4d]GL_FlagPeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}
	GL_TraceInfo("[%s-%4d]GL_FlagPeek  %d\n", __FUNCTION__, __LINE__, value);
}

GL_Status_t test_GL_Cross_Flag(void)
{
//	GL_Status_t ret = GL_SUCCESS;
	pthread_t  pthread_id1, pthread_id2, pthread_id3;
//	pthread_t pthread_id4;
	timer_t timerid_50Hz, timerid_10Hz, timerid_1Hz;

	GL_TraceInfo("[%s-%4d] enter\n", __FUNCTION__, __LINE__);

	/*create the create thread*/
	if(0 != pthread_create(&pthread_id1, NULL, thread_flag_creat, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the wait thread*/
	if(0 != pthread_create(&pthread_id2, NULL, thread_flag_wait_50Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the wait thread*/
	if(0 != pthread_create(&pthread_id3, NULL, thread_flag_wait_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the set thread*/
	if(0 != pthread_create(&pthread_id3, NULL, thread_flag_set_1Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the set timer*/
	if(0 != cross_GL_TimerCreate("tcross_flag_50hz", timer_flag_set_50Hz, NULL, 1, 20, 1, &timerid_50Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the set timer*/
	if(0 != cross_GL_TimerCreate("tcross_flag_10hz", timer_flag_set_10Hz, NULL, 1, 100, 1, &timerid_10Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the peek timer*/
	if(0 != cross_GL_TimerCreate("tcross_flag_1hz", timer_flag_peek_1Hz, NULL, 1, 1000, 1, &timerid_1Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	GL_TraceInfo("[%s-%4d] exit\n", __FUNCTION__, __LINE__);

	return GL_SUCCESS;
}
