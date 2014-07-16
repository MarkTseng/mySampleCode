#include "test.h"


static void * thread_sem_take(void *arg)
{
	GL_Status_t ret;

	GL_TraceInfo("[T1]	new thread 1\r\n");

	GL_TraceInfo("[T1]	GL_SemaphoreTake GL_NO_WAIT check timeout return\r\n");
	ret = GL_SemaphoreTake(*(GL_Semaphore_t*)arg, GL_NO_WAIT);
	GL_TraceInfo("[T1]	GL_SemaphoreTake GL_NO_WAIT return=%d\r\n", ret);
	if (ret == GL_SUCCESS) {
		return (void *)GL_FAILURE;
	}

	GL_TraceInfo("[T1]	GL_SemaphoreTake GL_INFINITE_WAIT check wait...\r\n");
	ret = GL_SemaphoreTake(*(GL_Semaphore_t*)arg, GL_INFINITE_WAIT);
	GL_TraceInfo("[T1]	GL_SemaphoreTake GL_INFINITE_WAIT return=%d\r\n", ret);
	if (ret != GL_SUCCESS) {
		return (void *)GL_FAILURE;
	}

	GL_TraceInfo("[T1]	GL_SemaphoreTake TIME_OUT check wait...\r\n");
	ret = GL_SemaphoreTake(*(GL_Semaphore_t*)arg, 6000);
	GL_TraceInfo("[T1]	GL_SemaphoreTake TIME_OUT return=%d\r\n", ret);
	if (ret == GL_SUCCESS) {
		return (void *)GL_FAILURE;
	}

	GL_TraceInfo("[T1]	GL_SemaphoreTake TIME_OUT check wait...\r\n");
	ret = GL_SemaphoreTake(*(GL_Semaphore_t*)arg, 2000);
	GL_TraceInfo("[T1]	GL_SemaphoreTake TIME_OUT return=%d\r\n", ret);
	if (ret != GL_SUCCESS) {
		return (void *)GL_FAILURE;
	}

	return (void *)GL_SUCCESS;
}

static void * thread_sem_give(void *arg)
{
	GL_Status_t ret;
	GL_UINT32 i;

	GL_TraceInfo("[T2]	new thread 2\r\n");

	sleep(1);

	GL_TraceInfo("[T1]	GL_SemaphoreGive \r\n");
	ret = GL_SemaphoreGive(*(GL_Semaphore_t*)arg);
	GL_TraceInfo("[T1]	GL_SemaphoreGive return=%d\r\n", ret);

	for(i=0; i<7; i++){ GL_TraceInfo("sleep:%d\n",i+1); sleep(1); }

	GL_TraceInfo("[T1]	GL_SemaphoreGive \r\n");
	ret = GL_SemaphoreGive(*(GL_Semaphore_t*)arg);
	GL_TraceInfo("[T2]	GL_SemaphoreGive return=%d\r\n", ret);


	return ((void *)NULL);
}

void * thread_sem_sync_give_1(void *arg)
{
	GL_Status_t ret;
	GL_UINT32 value;

	GL_TraceInfo("[SYSNC GIVE 1] start!\n");

	while(1){
		GL_TraceInfo("[SYSNC GIVE 1]	GL_SemaphorePeek	");
		ret = GL_SemaphorePeek(*(GL_Semaphore_t*)arg, &value);
		GL_TraceInfo("[SYSNC GIVE 1]	GL_SemaphorePeek value=%d, ret=%d\r\n", value, ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC GIVE 1]	sleep 1\n");
			sleep(1);
		}

		GL_TraceInfo("[SYSNC GIVE 1]	GL_SemaphoreGive	");
		ret = GL_SemaphoreGive(*(GL_Semaphore_t*)arg);
		GL_TraceInfo("[SYSNC GIVE 1]	GL_SemaphoreGive ret=%d\r\n", ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC GIVE 1]	sleep 1\n");
			sleep(2);
		}
	}

	GL_TraceInfo("################### [SYSNC GIVE 1] end! ###################\n");
	return ((void *)NULL);
}

void * thread_sem_sync_give_2(void *arg)
{
	GL_Status_t ret;
	GL_UINT32 value;

	GL_TraceInfo("[SYSNC GIVE 2] start!\n");

	while(1){
		GL_TraceInfo("[SYSNC GIVE 2]	GL_SemaphorePeek	");
		ret = GL_SemaphorePeek(*(GL_Semaphore_t*)arg, &value);
		GL_TraceInfo("[SYSNC GIVE 2]	GL_SemaphorePeek value=%d, ret=%d\r\n", value, ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC GIVE 2]	sleep 1\n");
			sleep(1);
		}

		GL_TraceInfo("[SYSNC GIVE 2]	GL_SemaphoreGive	");
		ret = GL_SemaphoreGive(*(GL_Semaphore_t*)arg);
		GL_TraceInfo("[SYSNC GIVE 2]	GL_SemaphoreGive ret=%d\r\n", ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC GIVE 2]	sleep 1\n");
			sleep(2);
		}
	}

	GL_TraceInfo("################### [SYSNC GIVE 2] end! ###################\n");
	return ((void *)NULL);
}

void * thread_sem_sync_take_1(void *arg)
{
	GL_Status_t ret;
	GL_UINT32 value;

	GL_TraceInfo("[SYSNC TAKE 1] start!\n");

	while(1){
		GL_TraceInfo("[SYSNC TAKE 1]	GL_SemaphorePeek	");
		ret = GL_SemaphorePeek(*(GL_Semaphore_t*)arg, &value);
		GL_TraceInfo("[SYSNC TAKE 1]	GL_SemaphorePeek value=%d, ret=%d\r\n", value, ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC TAKE 1]	sleep 1\n");
			sleep(1);
		}

		GL_TraceInfo("[SYSNC TAKE 1]	GL_SemaphoreTake	");
		ret = GL_SemaphoreTake(*(GL_Semaphore_t*)arg, GL_NO_WAIT);
		GL_TraceInfo("[SYSNC TAKE 1]	GL_SemaphoreTake ret=%d\r\n", ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC TAKE 1]	sleep 1\n");
			sleep(1);
		}
	}

	GL_TraceInfo("################### [SYSNC TAKE 1] end! ###################\r\n");
	return ((void *)NULL);
}

void * thread_sem_sync_take_2(void *arg)
{
	GL_Status_t ret;
	GL_UINT32 value;

	GL_TraceInfo("[SYSNC TAKE 2] start!\n");

	while(1){
		GL_TraceInfo("[SYSNC TAKE 2]	GL_SemaphorePeek	");
		ret = GL_SemaphorePeek(*(GL_Semaphore_t*)arg, &value);
		GL_TraceInfo("[SYSNC TAKE 2]	GL_SemaphorePeek value=%d, ret=%d\r\n", value, ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC TAKE 2]	sleep 1\n");
			sleep(1);
		}

		GL_TraceInfo("[SYSNC TAKE 2]	GL_SemaphoreTake	");
		ret = GL_SemaphoreTake(*(GL_Semaphore_t*)arg, GL_NO_WAIT);
		GL_TraceInfo("[SYSNC TAKE 2]	GL_SemaphoreTake ret=%d\r\n", ret);
		if(ret != GL_SUCCESS){
			GL_TraceInfo("[SYSNC TAKE 2]	sleep 1\n");
			sleep(1);
		}
	}

	GL_TraceInfo("################### [SYSNC TAKE 2] end! ###################\r\n");
	return ((void *)NULL);
}

static GL_Status_t test_GL_Semaphore_PV(void)
{
	void *ret, *tmp;
	GL_Semaphore_t semTestPV;
	pthread_t  pthread_id1, pthread_id2;

	GL_TraceInfo("[%s]GL_SemaphoreCreate\n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreCreate("semTestMaxCount", 16, 0, &semTestPV)){
		GL_TraceError("[%s-%4d]GL_SemaphoreCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

#if 1
	if(0 != pthread_create(&pthread_id1, NULL, thread_sem_take, (void*)(&semTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}
#endif

#if 1
	if(0 != pthread_create(&pthread_id2, NULL, thread_sem_give, (void*)(&semTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

#endif

	sleep(10);/*sleep for thread executing*/

	GL_TraceInfo("[%s]GL_SemaphoreDelete\n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreDelete(semTestPV)){
		GL_TraceError("[%s-%4d]GL_SemaphoreDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	pthread_join(pthread_id1, (void **)&ret);
	pthread_join(pthread_id2, (void **)&tmp);
	if ((ret != GL_SUCCESS) || (tmp != GL_SUCCESS)) {
		return GL_FAILURE;
	}

	return GL_SUCCESS;
}


static GL_Status_t test_GL_Semaphore_Sync(void)
{
	GL_Semaphore_t semTestPV;
	pthread_t  pthread_id[4];

	GL_TraceInfo("[%s]GL_SemaphoreCreate\n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreCreate("semTestMaxCount", 16, 0, &semTestPV)){
		GL_TraceError("[%s-%4d]GL_SemaphoreCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

#if 1
	if(0 != pthread_create(&pthread_id[0], NULL, thread_sem_sync_give_1, (void*)(&semTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	if(0 != pthread_create(&pthread_id[1], NULL, thread_sem_sync_give_2, (void*)(&semTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	if(0 != pthread_create(&pthread_id[2], NULL, thread_sem_sync_take_1, (void*)(&semTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	if(0 != pthread_create(&pthread_id[3], NULL, thread_sem_sync_take_2, (void*)(&semTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}
#endif

	sleep(10);/*sleep for thread executing*/

	GL_TraceInfo("[%s]GL_SemaphoreDelete\n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreDelete(semTestPV)){
		GL_TraceError("[%s-%4d]GL_SemaphoreDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}
	return GL_SUCCESS;
}

static GL_Status_t test_GL_Semaphore_MaxCount(void)
{
	GL_Semaphore_t semTestMaxCount = NULL;
	GL_UINT32 i, value;
	const int max = 100;


	if (GL_SUCCESS != GL_SemaphoreCreate("semTestMaxCount", max, 0, &semTestMaxCount)) {
		GL_TraceError("[%s-%4d]GL_SemaphoreCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

#if 1
	for (i = 0; i < (max + 1); i++) {

		if (GL_SUCCESS != GL_SemaphoreGive(semTestMaxCount)) {
			GL_TraceError("[%s-%4d]GL_SemaphoreGive fail\n", __FUNCTION__, __LINE__);
			break;
		}

		if (GL_SUCCESS != GL_SemaphorePeek(semTestMaxCount, &value)) {
			GL_TraceError("[%s-%4d]GL_SemaphorePeek fail\n", __FUNCTION__, __LINE__);
			break;
		}

		GL_TraceInfo("GL_SemaphoreGive Test: loopcnt=%4d, current=%4d\n", i, value);
	}
#endif

	GL_SEMAPHORE_DUMP_LIST(0);

	GL_TraceInfo("[%s]GL_SemaphoreDelete \n", __FUNCTION__);
	if(GL_SUCCESS != GL_SemaphoreDelete(semTestMaxCount)){
		GL_TraceError("[%s-%4d]GL_SemaphoreDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	return (i == max) ? GL_SUCCESS : GL_FAILURE;
}

GL_Status_t Test_GL_Semaphore(void)
{
	int i;
	struct {
		char *name;
		GL_Status_t (*func)(void);
		int result;
	} test_patrn[] = {
		{"test_GL_Semaphore_MaxCount", test_GL_Semaphore_MaxCount, 0},
		{"test_GL_Semaphore_PV", test_GL_Semaphore_PV, 0},
		{"test_GL_Semaphore_Sync", test_GL_Semaphore_Sync, 0}
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
}
