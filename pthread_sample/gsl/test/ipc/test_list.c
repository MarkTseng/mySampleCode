#include "test.h"


static GL_Status_t test_GL_MSGL_DUMP_LIST(void)
{
	GL_Status_t ret = GL_SUCCESS;
	GL_List_t listDump;
	GL_UINT32 const msgsize = 64;
	GL_UINT32 const initNum = 12;
	GL_UINT8  msgbuf[msgsize];
	GL_UINT8  recbuf[msgsize];
	GL_UINT32 i;

	for(i=0; i<10; i++){
		msgbuf[i] = i+0x30;
	}
	msgbuf[10] = 0;

	if(GL_SUCCESS != (ret = GL_ListCreate("listDump", msgsize, initNum, &listDump))){
		GL_TraceError("[%s-%4d]GL_ListCreate fail\n", __FUNCTION__, __LINE__);
		return ret;
	}
	GL_MSGL_DUMP_LIST(0);

	for(i=0; i<3; i++){
		if(GL_SUCCESS != (ret = GL_ListSend(listDump, (void*)msgbuf, 10))){
			GL_TraceError("[%s-%4d]GL_ListSend fail\n", __FUNCTION__, __LINE__);
			goto __test_list_dump_exit;
		}
		GL_TraceInfo("GL_ListSend:%s\n", msgbuf);
		GL_MSGL_DUMP_LIST(0);
	}

	for(i=0; i<initNum; i++){
		if(GL_SUCCESS != (ret = GL_ListSendUrgent(listDump, (void*)msgbuf, 10))){
			GL_TraceError("[%s-%4d]GL_ListSend fail\n", __FUNCTION__, __LINE__);
			goto __test_list_dump_exit;
		}
		GL_TraceInfo("GL_ListSendUrgent:%s\n", msgbuf);
		GL_MSGL_DUMP_LIST(0);
	}

	for(i=0; i<initNum+4; i++){
		if(GL_SUCCESS != (ret = GL_ListReceive(listDump, (void*)recbuf, 10, GL_NO_WAIT))){
			GL_TraceError("[%s-%4d]GL_ListReceive fail! ret=%d\n", __FUNCTION__, __LINE__, ret);
			goto __test_list_dump_exit;
		}
		recbuf[10] = 0;
		GL_TraceInfo("GL_ListReceive:%s\n", recbuf);
		GL_MSGL_DUMP_LIST(0);
	}

__test_list_dump_exit:

	if(GL_SUCCESS != (ret = GL_ListDelete(listDump))){
		GL_TraceError("[%s-%4d]GL_ListDelete fail\n", __FUNCTION__, __LINE__);
		return ret;
	}

	GL_MSGL_DUMP_LIST(0);
	return ret;
}


GL_Status_t Test_GL_List(void)
{
	int i;
	struct {
		char *name;
		GL_Status_t (*func)(void);
		int result;
	} test_patrn[] = {
		{"test_GL_MSGL_DUMP_LIST", test_GL_MSGL_DUMP_LIST, 0},
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
