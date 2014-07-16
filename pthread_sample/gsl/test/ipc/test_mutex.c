#include "test.h"


void * thread_mutex_1(void *arg)
{
	GL_Status_t ret;

	GL_TraceInfo("[T1]	new thread 1\r\n");

	GL_TraceInfo("[T1]	sleep(2)\r\n");
	sleep(2);

	GL_TraceInfo("[T1]	GL_MutexLock\r\n");
	ret = GL_MutexLock(*(GL_Mutex_t*)arg);
	GL_TraceInfo("[T1]	GL_MutexLock ret=%d\r\n", ret);

	GL_TraceInfo("[T1]	sleep(1)\r\n");
	sleep(1);

	GL_TraceInfo("[T1]	GL_MutexUnLock\r\n");
	ret = GL_MutexUnlock(*(GL_Mutex_t*)arg);
	GL_TraceInfo("[T1]	GL_MutexUnLock ret=%d\r\n", ret);

	return ((void *)NULL);
}

void * thread_mutex_2(void *arg)
{
	GL_Status_t ret;

	GL_TraceInfo("[T2]	new thread 2\r\n");

	GL_TraceInfo("[T2]	sleep(1)\r\n");
	sleep(1);

	GL_TraceInfo("[T2]	GL_MutexUnLock\r\n");
	ret = GL_MutexUnlock(*(GL_Mutex_t*)arg);
	GL_TraceInfo("[T2]	GL_MutexUnLock ret=%d\r\n", ret);

	return ((void *)NULL);
}

static GL_Status_t test_GL_mutex_PV(void)
{
	GL_Mutex_t mutexTestPV;
	pthread_t  pthread_id1, pthread_id2;
	GL_Status_t ret;

	if(GL_SUCCESS != GL_MutexCreate("mutexTestPV", &mutexTestPV)){
		GL_TraceError("[%s-%4d]GL_MutexCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	if(0 != pthread_create(&pthread_id1, NULL, thread_mutex_1, (void*)(&mutexTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	if(0 != pthread_create(&pthread_id2, NULL, thread_mutex_2, (void*)(&mutexTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	//GL_TraceInfo("[P0]	sleep(2)\r\n");
	//sleep(2);

	GL_TraceInfo("[P0]	GL_MutexLock\r\n");
	ret = GL_MutexLock(mutexTestPV);
	GL_TraceInfo("[P0]	GL_MutexLock ret=%d\r\n", ret);

	GL_TraceInfo("[P0]	sleep(3)\r\n");
	sleep(3);

	GL_TraceInfo("[P0]	GL_MutexUnLock\r\n");
	ret = GL_MutexUnlock(mutexTestPV);
	GL_TraceInfo("[P0]	GL_MutexUnLock ret=%d\r\n", ret);

	sleep(1);	// wait thread_1 to lock/unlock mutex

	if(GL_SUCCESS != GL_MutexDelete(mutexTestPV)){
		GL_TraceError("[%s-%4d]GL_MutexDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	sleep(4); /*sleep for thread executing*/

	return GL_SUCCESS;
}

static GL_Status_t test_GL_Mutex_Noraml(void)
{
	GL_Status_t ret = GL_SUCCESS;
	GL_Mutex_t mutexNormal;

	if(GL_SUCCESS != GL_MutexCreate("mutexNormal", &mutexNormal)){
		GL_TraceError("[%s-%4d]GL_MutexCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	GL_MUTEX_DUMP_LIST(0);
	if(GL_SUCCESS != GL_MutexLock(mutexNormal)){
		ret = GL_FAILURE;
		GL_TraceError("[%s-%4d]GL_MutexLock fail\n", __FUNCTION__, __LINE__);
		goto __test_mutex_normal_exit;
	}

	GL_MUTEX_DUMP_LIST(0);
	if(GL_SUCCESS == GL_MutexTryLock(mutexNormal)){
		ret = GL_FAILURE;
		GL_TraceError("[%s-%4d]GL_MutexTryLock succeeds but it should fail.\n", __FUNCTION__, __LINE__);
		goto __test_mutex_normal_exit;
	}

	GL_MUTEX_DUMP_LIST(0);
	if(GL_SUCCESS != GL_MutexUnlock(mutexNormal)){
		ret = GL_FAILURE;
		GL_TraceError("[%s-%4d]GL_MutexUnlock fail\n", __FUNCTION__, __LINE__);
		goto __test_mutex_normal_exit;
	}

__test_mutex_normal_exit:

	GL_MUTEX_DUMP_LIST(0);
	if(GL_SUCCESS != GL_MutexDelete(mutexNormal)){
		GL_TraceError("[%s-%4d]GL_MutexDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	return ret;
}

static GL_Status_t test_GL_Mutex_Recursive(void)
{
	GL_Status_t ret = GL_SUCCESS;
	GL_Mutex_t mutexNormal;
	GL_UINT32 i;

	GL_TraceInfo("[%s]GL_MutexCreate\n", __FUNCTION__);
	if(GL_SUCCESS != GL_MutexCreateRecursive("mutexNormal", &mutexNormal)){
		GL_TraceError("[%s-%4d]GL_MutexCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	for (i = 0; i < 100; i++) {
		GL_TraceInfo("[%s]GL_MutexLock %d times\n", __FUNCTION__, i + 1);
		if (GL_SUCCESS != GL_MutexLock(mutexNormal)){
			GL_TraceError("[%s-%4d]GL_MutexLock fail\n", __FUNCTION__, __LINE__);
			ret = GL_FAILURE;;
			goto __test_mutex_normal_exit;
		}
	}

	for (i = 0; i < 100; i++) {
		GL_TraceInfo("[%s]GL_MutexUnlock %d times\n", __FUNCTION__, i + 1);
		if (GL_SUCCESS != GL_MutexUnlock(mutexNormal)) {
			GL_TraceError("[%s-%4d]GL_MutexUnlock fail\n", __FUNCTION__, __LINE__);
			ret = GL_FAILURE;;
			goto __test_mutex_normal_exit;
		}
	}


__test_mutex_normal_exit:

	GL_TraceInfo("[%s]GL_MutexDelete\n", __FUNCTION__);
	if (GL_SUCCESS != GL_MutexDelete(mutexNormal)) {
		GL_TraceError("[%s-%4d]GL_MutexDelete fail\n", __FUNCTION__, __LINE__);
		ret = GL_FAILURE;
	}

	return ret;
}

GL_Status_t Test_GL_Mutex(void)
{
	int i;
	struct {
		char *name;
		GL_Status_t (*func)(void);
		int result;
	} test_patrn[] = {
		{"test_GL_Mutex_Noraml", test_GL_Mutex_Noraml, 0},
		{"test_GL_Mutex_Recursive", test_GL_Mutex_Recursive, 0},
		{"test_GL_mutex_PV", test_GL_mutex_PV, 0}
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
