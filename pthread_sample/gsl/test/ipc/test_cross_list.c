#include "test.h"


static GL_List_t cross_listid = NULL;


static void* thread_list_creat(void *arg)
{
	static int count_create=0,  count_take;
	static char recbuf[16];

	while(1){

		GL_TraceInfo("[%s]GL_ListCreate: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_ListCreate("cross_Testlist", 128, 12, &cross_listid)){
			GL_TraceError("[%s-%4d]GL_ListCreate fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		for(count_take=0; count_take<1000; count_take++){

			memset(recbuf, 0, 16);
			GL_TraceInfo("[%s]GL_ListReceive: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_ListReceive(cross_listid, (void *)recbuf, 16, GL_INFINITE_WAIT)){
				GL_TraceError("[%s-%4d]GL_ListReceive GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
				exit(1); //break;
			}
			GL_TraceInfo("[%s-%d]GL_ListReceive:%s\n", __FUNCTION__,  __LINE__, recbuf);


			memset(recbuf, 0, 16);
			GL_TraceInfo("[%s]GL_ListReceive: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_ListReceive (cross_listid, (void *)recbuf, 16, GL_NO_WAIT)){
				GL_TraceError("[%s-%4d]GL_ListReceive GL_NO_WAIT fail\n", __FUNCTION__, __LINE__);
			}
			GL_TraceInfo("[%s-%d]GL_ListReceive:%s\n", __FUNCTION__,  __LINE__, recbuf);

			memset(recbuf, 0, 16);
			GL_TraceInfo("[%s]GL_ListReceive: count_creat=%d, count_take=%d\n", __FUNCTION__, count_create, count_take);
			if(GL_SUCCESS != GL_ListReceive(cross_listid, (void *)recbuf, 16, 800)){
				GL_TraceError("[%s-%4d]GL_ListReceive 800_ms fail\n", __FUNCTION__, __LINE__);
			}
			GL_TraceInfo("[%s-%d]GL_ListReceive:%s\n", __FUNCTION__, __LINE__, recbuf);
		}

		GL_TraceInfo("[%s]GL_ListDelete: count_creat=%d\n", __FUNCTION__, count_create);
		if(GL_SUCCESS != GL_ListDelete(cross_listid)){
			GL_TraceError("[%s-%4d]GL_ListDelete fail\n", __FUNCTION__, __LINE__);
			exit(1); //return NULL;
		}

		cross_listid = NULL;
		count_create++;
	}
	return NULL;
}

static void* thread_list_receive_10Hz(void *arg)
{
	static char recbuf[16];
	while(1){
		memset(recbuf, 0, 16);
		GL_TraceInfo("[%s]GL_ListReceive\n", __FUNCTION__);
		if(GL_SUCCESS != GL_ListReceive(cross_listid, (void *)recbuf, 16, GL_INFINITE_WAIT)){
			GL_TraceError("[%s-%4d]GL_ListReceive GL_INFINITE_WAIT fail\n", __FUNCTION__, __LINE__);
			exit(1); //break;
		}
		GL_TraceInfo("[%s]GL_ListReceive:%s\n", __FUNCTION__, recbuf);
		GL_TaskSleep(100);
	}
	return NULL;
}

static void* thread_list_send_10Hz(void *arg)
{
	static char sendbuf[16]= {'s','e','n','d','f','r','o','m','p','t','1','0',0};

	while(1){
		GL_TraceInfo("[%s]GL_ListSend:%s\n", __FUNCTION__, sendbuf);
		if(GL_SUCCESS != GL_ListSend( cross_listid, sendbuf, strlen((char*)sendbuf)) ){
			GL_TraceError("[%s-%4d]GL_ListSend  fail\n", __FUNCTION__, __LINE__);
		}
		GL_TaskSleep(100);
	}
	return NULL;
}

static void timer_list_send_50Hz(void *arg)
{
	static char sendbuf[16]= {'s','e','n','d','f','r','o','m','t','m','5','0',0};

	GL_TraceInfo("[%s]GL_ListSend:%s\n", __FUNCTION__, sendbuf);
	if(GL_SUCCESS != GL_ListSend( cross_listid, sendbuf, strlen((char*)sendbuf)) ){
		GL_TraceError("[%s-%4d]GL_ListSend  fail\n", __FUNCTION__, __LINE__);
	}
}

static void timer_list_send_1Hz(void *arg)
{
	static char sendbuf[16]= {'s','e','n','d','f','r','o','m','t','m','0','1',0};

	GL_TraceInfo("[%s]GL_ListSend:%s\n", __FUNCTION__, sendbuf);
	if(GL_SUCCESS != GL_ListSend( cross_listid, sendbuf, strlen((char*)sendbuf)) ){
		GL_TraceError("[%s-%4d]GL_ListSend  fail\n", __FUNCTION__, __LINE__);
	}
}

GL_Status_t test_GL_Cross_List(void)
{
//	GL_Status_t ret = GL_SUCCESS;
	pthread_t  pthread_id1, pthread_id2, pthread_id3;
	timer_t timerid_50Hz, timerid_1Hz;
//	timer_t timerid_1Hz_flush, timerid_1Hz_peek;

	GL_TraceInfo("[%s-%4d] enter\n", __FUNCTION__, __LINE__);

	/*create the create thread*/
	if(0 != pthread_create(&pthread_id1, NULL, thread_list_creat, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the give thread*/
	if(0 != pthread_create(&pthread_id2, NULL, thread_list_receive_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the take thread*/
	if(0 != pthread_create(&pthread_id3, NULL, thread_list_send_10Hz, NULL)){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the send timer*/
	if(0 != cross_GL_TimerCreate("cross_list_50hz", timer_list_send_50Hz, NULL, 1, 20, 1, &timerid_50Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	/*create the send timer*/
	if(0 != cross_GL_TimerCreate("cross_list_1hz", timer_list_send_1Hz, NULL, 1, 1000, 1, &timerid_1Hz)){
		GL_TraceError("[%s-%4d]GL_TimerCreate fail\r\n", __FUNCTION__, __LINE__);
		exit(1);
	}

	GL_TraceInfo("[%s-%4d] exit\n", __FUNCTION__, __LINE__);

	return GL_SUCCESS;
}
