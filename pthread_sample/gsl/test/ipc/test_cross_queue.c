#include "test.h"


static GL_Queue_t cross_queueid = NULL;


static void* thread_queue_creat(void *arg)
{
	static int count_create=0,  count_take;
	static char recbuf[16];

	while(1){

		GL_TraceInfo("[%s]GL_QueueCreate: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_QueueCreate("cross_Testqueue", 128, 12, 4, &cross_queueid)){
			GL_TraceError("[%s-%4d]GL_QueueCreate fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		for(count_take=0; count_take<1000; count_take++){

			memset(recbuf, 0, 16);
			GL_TraceInfo("[%s]GL_QueueReceive: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_QueueReceive(cross_queueid, (void *)recbuf, 16, GL_INFINITE_WAIT)){
				GL_TraceError("[%s-%4d]GL_QueueReceive GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
				exit(1); //break;
			}
			GL_TraceInfo("[%s-%d]GL_QueueReceive:%s\n", __FUNCTION__,  __LINE__, recbuf);


			memset(recbuf, 0, 16);
			GL_TraceInfo("[%s]GL_QueueReceive: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_QueueReceive (cross_queueid, (void *)recbuf, 16, GL_NO_WAIT)){
				GL_TraceError("[%s-%4d]GL_QueueReceive GL_NO_WAIT fail\n", __FUNCTION__, __LINE__);
			}
			GL_TraceInfo("[%s-%d]GL_QueueReceive:%s\n", __FUNCTION__,  __LINE__, recbuf);

			memset(recbuf, 0, 16);
			GL_TraceInfo("[%s]GL_QueueReceive: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_QueueReceive(cross_queueid, (void *)recbuf, 16, 800)){
				GL_TraceError("[%s-%4d]GL_QueueReceive 800_ms fail\n", __FUNCTION__, __LINE__);
			}
			GL_TraceInfo("[%s-%d]GL_QueueReceive:%s\n", __FUNCTION__, __LINE__, recbuf);
		}

		GL_TraceInfo("[%s]GL_QueueDelete: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_QueueDelete(cross_queueid)){
			GL_TraceError("[%s-%4d]GL_QueueDelete fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		cross_queueid = NULL;
		count_create++;
	}
	return NULL;
}

static void* thread_queue_receive_10Hz(void *arg)
{
	static char recbuf[16];
	while(1){
		memset(recbuf, 0, 16);
		GL_TraceInfo("[%s]GL_QueueReceive\n", __FUNCTION__);
		if(GL_SUCCESS != GL_QueueReceive(cross_queueid, (void *)recbuf, 16, GL_INFINITE_WAIT)){
			GL_TraceError("[%s-%4d]GL_QueueReceive GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
			exit(1); //break;
		}
		GL_TraceInfo("[%s]GL_QueueReceive:%s\n", __FUNCTION__, recbuf);
		GL_TaskSleep(100);
	}
	return NULL;
}

static void* thread_queue_send_10Hz(void *arg)
{
	static char sendbuf[16]= {'s','e','n','d','f','r','o','m','p','t','1','0',0};

	while(1){
		GL_TraceInfo("[%s]GL_QueueSend:%s\n", __FUNCTION__, sendbuf);
		if(GL_SUCCESS != GL_QueueSend( cross_queueid, sendbuf, strlen((char*)sendbuf)) ){
			GL_TraceError("[%s-%4d]GL_QueueSend  fail\n", __FUNCTION__, __LINE__);
		}
		GL_TaskSleep(100);
	}
	return NULL;
}

static void queue_peek(void *arg)
{
	GL_UINT32 value, valuenormal, valueUrgent;

	GL_TraceInfo("[%s]GL_QueuePeek\n", __FUNCTION__);
	value = valuenormal = valueUrgent = 0;

	if(GL_SUCCESS != GL_QueuePeek(cross_queueid, &value)){
		GL_TraceError("[%s-%4d]GL_QueuePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	if(GL_SUCCESS != GL_QueuePeekNormal(cross_queueid, &valuenormal)){
		GL_TraceError("[%s-%4d]GL_QueuePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	if(GL_SUCCESS != GL_QueuePeekUrgent(cross_queueid, &valueUrgent)){
		GL_TraceError("[%s-%4d]GL_QueuePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	GL_TraceInfo("[%s]GL_QueuePeek  T/N/U=%d/%d/%d\n", __FUNCTION__, value, valuenormal, valueUrgent);

}

static void timer_queue_send_50Hz(void *arg)
{
	static char sendbuf[16]= {'s','e','n','d','f','r','o','m','t','m','5','0',0};

	GL_TraceInfo("[%s]GL_QueueSend:%s\n", __FUNCTION__, sendbuf);
	if(GL_SUCCESS != GL_QueueSend( cross_queueid, sendbuf, strlen((char*)sendbuf)) ){
		GL_TraceError("[%s-%4d]GL_QueueSend  fail\n", __FUNCTION__, __LINE__);
	}

	queue_peek(NULL);
}

static void timer_queue_send_1Hz(void *arg)
{
	static char sendbuf[16]= {'s','e','n','d','f','r','o','m','t','m','0','1',0};

	GL_TraceInfo("[%s]GL_QueueSend:%s\n", __FUNCTION__, sendbuf);
	if(GL_SUCCESS != GL_QueueSendUrgent( cross_queueid, sendbuf, strlen((char*)sendbuf)) ){
		GL_TraceError("[%s-%4d]GL_QueueSend  fail\n", __FUNCTION__, __LINE__);
	}

	queue_peek(NULL);
}

static void timer_queue_flush_1Hz(void *arg)
{
	GL_TraceInfo("[%s]GL_QueueFlushn", __FUNCTION__);
	if(GL_SUCCESS != GL_QueueFlush(cross_queueid)){
		GL_TraceError("[%s-%4d]GL_QueueFlush  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}
}

static void timer_queue_peek_1Hz(void *arg)
{
	GL_UINT32 value, valuenormal, valueUrgent;

	GL_TraceInfo("[%s]GL_QueuePeek\n", __FUNCTION__);
	value = valuenormal = valueUrgent = 0;

	if(GL_SUCCESS != GL_QueuePeek(cross_queueid, &value)){
		GL_TraceError("[%s-%4d]GL_QueuePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	if(GL_SUCCESS != GL_QueuePeekNormal(cross_queueid, &valuenormal)){
		GL_TraceError("[%s-%4d]GL_QueuePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	if(GL_SUCCESS != GL_QueuePeekUrgent(cross_queueid, &valueUrgent)){
		GL_TraceError("[%s-%4d]GL_QueuePeek  fail\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	GL_TraceInfo("[%s]GL_QueuePeek  T/N/U=%d/%d/%d\n", __FUNCTION__, value, valuenormal, valueUrgent);

}

GL_Status_t test_GL_Cross_Queue(void)
{
	pthread_t  pthread_id1, pthread_id2, pthread_id3;
	timer_t timerid_50Hz, timerid_1Hz, timerid_1Hz_flush, timerid_1Hz_peek;

	GL_TraceInfo("[%s-%4d] enter\n", __FUNCTION__, __LINE__);

	/*create the create thread*/
	if(0 != pthread_create(&pthread_id1, NULL, thread_queue_creat, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the give thread*/
	if(0 != pthread_create(&pthread_id2, NULL, thread_queue_receive_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the take thread*/
	if(0 != pthread_create(&pthread_id3, NULL, thread_queue_send_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the send timer*/
	if(0 != cross_GL_TimerCreate("cross_queue_50hz", timer_queue_send_50Hz, NULL, 1, 20, 1, &timerid_50Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the send timer*/
	if(0 != cross_GL_TimerCreate("cross_queue_1hz", timer_queue_send_1Hz, NULL, 1, 1000, 1, &timerid_1Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the flush timer*/
	if(0 != cross_GL_TimerCreate("cross_queue_1hz_flush", timer_queue_flush_1Hz, NULL, 1, 1000, 1, &timerid_1Hz_flush)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
	}

	/*create the peek  timer*/
	if(0 != cross_GL_TimerCreate("cross_queue_1hz_peek", timer_queue_peek_1Hz, NULL, 1, 1000, 1, &timerid_1Hz_peek)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
	}

	GL_TraceInfo("[%s-%4d] exit\n", __FUNCTION__, __LINE__);

	return GL_SUCCESS;
}
