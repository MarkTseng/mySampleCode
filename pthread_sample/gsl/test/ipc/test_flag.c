#include "test.h"

static void * thread_flag_set(void *arg)
{
	GL_UINT32 value;
	GL_Status_t ret;

	sleep(1);
	value = 0x04;
	GL_TraceInfo("[T1]	GL_FlagSetbits: 0x%x\r\n", value);
	ret = GL_FlagSetbits(*(GL_Flag_t*)arg, value);
	GL_TraceInfo("[T1]	GL_FlagSetbits  return=%d\r\n", ret);

	sleep(1);
	value = 0x01;
	GL_TraceInfo("[T1]	GL_FlagSetbits: 0x%x\r\n", value);
	ret = GL_FlagSetbits(*(GL_Flag_t*)arg, value);
	GL_TraceInfo("[T1]	GL_FlagSetbits  return=%d\r\n", ret);

	sleep(1);
	value = 0x02;
	GL_TraceInfo("[T1]	GL_FlagSetbits: 0x%x\r\n", value);
	ret = GL_FlagSetbits(*(GL_Flag_t*)arg, value);
	GL_TraceInfo("[T1]	GL_FlagSetbits  return=%d\r\n", ret);

	return ((void *)NULL);
}
static void * thread_flag_wait(void *arg)
{
	GL_Status_t ret;
	GL_Status_t retval;

	GL_TraceInfo("[T2]	GL_FlagWait TIME_OUT check wait...\r\n");
	ret = GL_FlagWait(*(GL_Flag_t*)arg, 0x3, GL_FLAG_WAITMODE_OR, 4000);
	if (ret != GL_SUCCESS) {
		retval = GL_FAILURE;
	}
	GL_TraceInfo("[T2]	GL_FlagWait TIME_OUT return=%d\r\n", ret);

	GL_TraceInfo("[T2]	GL_FlagWait GL_INFINITE_WAIT check wait...\r\n");
	ret = GL_FlagWait(*(GL_Flag_t*)arg, 0x6, GL_FLAG_WAITMODE_AND | GL_FLAG_WAITMODE_CLR, GL_INFINITE_WAIT);
	if (ret != GL_SUCCESS) {
		retval = GL_FAILURE;
	}
	GL_TraceInfo("[T2]	GL_FlagWait GL_INFINITE_WAIT return=%d\r\n", ret);

	GL_TraceInfo("[T2]	GL_FlagWait TIME_OUT check wait...\r\n");
	ret = GL_FlagWait(*(GL_Flag_t*)arg, 0x3, GL_FLAG_WAITMODE_AND, 3000);
	if (ret == GL_SUCCESS) {
		retval = GL_FAILURE;
	}
	GL_TraceInfo("[T2]	GL_FlagWait TIME_OUT return=%d\r\n", ret);

	return (void *)retval;
}

static GL_Status_t test_GL_Flag_PV(void)
{
	GL_Flag_t flagTestPV;
	pthread_t pthread_id1, pthread_id2;
	GL_UINT32 val;
	void *retval;
	GL_Status_t ret = GL_SUCCESS;

	GL_TraceInfo("[%s]GL_FlagCreate\n", __FUNCTION__);
	if(GL_SUCCESS != GL_FlagCreate("flagTestPV", &flagTestPV)){
		GL_TraceError("[%s-%4d]GL_FlagCreate fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}


	if(0 != pthread_create(&pthread_id1, NULL, thread_flag_set, (void*)(&flagTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	if(0 != pthread_create(&pthread_id2, NULL, thread_flag_wait, (void*)(&flagTestPV))){
		GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
	}

	pthread_join(pthread_id2, &retval);

	if (retval != GL_SUCCESS) {
		GL_TraceError("[%s-%4d]pthread_join get bad ret value\n", __FUNCTION__, __LINE__);
		ret = GL_FAILURE;
	}

	GL_FlagPeek(flagTestPV, &val);
	if (val != 0) {
		GL_TraceError("[%s-%4d]GL_FlagPeek has wrong value (0x%08X)\n", __FUNCTION__, __LINE__, val);
		ret = GL_FAILURE;
	}
	GL_TraceInfo("[%s]GL_FlagDelete\n", __FUNCTION__);
	if(GL_SUCCESS != GL_FlagDelete(flagTestPV)){
		GL_TraceError("[%s-%4d]GL_FlagDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	return ret;
}

GL_Status_t Test_GL_Flag(void)
{
	int i;
	struct {
		char *name;
		GL_Status_t (*func)(void);
		int result;
	} test_patrn[] = {
		{"test_GL_Flag_PV", test_GL_Flag_PV, 0},
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
