#include "test.h"

static GL_Status_t test_GL_QueueDump(void)
{
	GL_Status_t ret = GL_SUCCESS;
	GL_Queue_t queueDump;
	GL_UINT32 const msgsize = 64;
	GL_UINT32 const initNum = 12;
	GL_UINT32 const initNumUrgent = 4;
	GL_UINT8  msgbuf[msgsize];
	GL_UINT8  recbuf[msgsize];
	GL_UINT32 i;

	for(i=0; i<10; i++){
		msgbuf[i] = i+0x30;
	}
	msgbuf[10] = 0;

	GL_TraceError("[%s]GL_QueueCreate\n", __FUNCTION__);
	if(GL_SUCCESS != (ret = GL_QueueCreate("listDump", msgsize, initNum, initNumUrgent, &queueDump))){
		GL_TraceError("[%s-%4d]GL_QueueCreate fail\n", __FUNCTION__, __LINE__);
		return ret;
	}
	GL_QUEUE_DUMP_LIST(0);

	GL_TraceError("[%s]GL_QueueSend %d times\n", __FUNCTION__, initNum+1);
	for(i=0; i<initNum+1; i++){
		if(GL_SUCCESS != (ret = GL_QueueSend(queueDump, (void*)msgbuf, 10))){
			GL_TraceError("[%s-%4d]GL_QueueSend fail\n", __FUNCTION__, __LINE__);
			//goto __exit;
			break;
		}
		GL_TraceInfo("GL_QueueSend:%s\n", msgbuf);
		GL_QUEUE_DUMP_LIST(0);
	}

	GL_TraceError("[%s]GL_QueueSendUrgent %d times\n", __FUNCTION__, initNumUrgent+1);
	for(i=0; i<initNumUrgent+1; i++){
		if(GL_SUCCESS != (ret = GL_QueueSendUrgent(queueDump, (void*)msgbuf, 10))){
			GL_TraceError("[%s-%4d]GL_QueueSendUrgent fail\n", __FUNCTION__, __LINE__);
			//goto __exit;
			break;
		}
		GL_TraceInfo("GL_QueueSendUrgent:%s\n", msgbuf);
		GL_QUEUE_DUMP_LIST(0);
	}

	GL_TraceError("[%s]GL_QueueReceive %d times\n", __FUNCTION__, initNum+initNumUrgent+1);
	for(i=0; i<initNum+initNumUrgent+1; i++){
		if(GL_SUCCESS != (ret = GL_QueueReceive(queueDump, (void*)recbuf, 10, GL_NO_WAIT))){
			GL_TraceError("[%s-%4d]GL_QueueReceive fail! ret=%d\n", __FUNCTION__, __LINE__, ret);
			//goto __exit;
			break;
		}
		recbuf[10] = 0;
		GL_TraceInfo("GL_QueueReceive:%s\n", recbuf);
		GL_QUEUE_DUMP_LIST(0);
	}

//__exit:

	GL_TraceError("[%s]GL_QueueDelete\n", __FUNCTION__);
	if(GL_SUCCESS != (ret = GL_QueueDelete(queueDump))){
		GL_TraceError("[%s-%4d]GL_QueueDelete fail\n", __FUNCTION__, __LINE__);
		return ret;
	}

	GL_QUEUE_DUMP_LIST(0);
	return ret;
}

GL_Status_t test_GL_QueueFlush(void)
{
	GL_Status_t ret = GL_SUCCESS;
	GL_Queue_t queueDump;
	GL_UINT32 const msgsize = 64;
	GL_UINT32 const initNum = 12;
	GL_UINT32 const initNumUrgent = 4;
	GL_UINT8  msgbuf[] = "0123456789";
	GL_UINT32 i, val1, val2;


	GL_TraceError(">>> Test test_GL_QueueFlush function\n");
	if (GL_SUCCESS != (ret = GL_QueueCreate("listDump", msgsize, initNum, initNumUrgent, &queueDump))) {
		GL_TraceError("[%s-%4d]GL_QueueCreate fail\n", __FUNCTION__, __LINE__);
		return ret;
	}

	for (i = 0; i < initNum; i++) {
		if(GL_SUCCESS != (ret = GL_QueueSend(queueDump, (void*)msgbuf, sizeof(msgbuf) + 1))){
			GL_TraceError("[%s-%4d]GL_QueueSend fail\n", __FUNCTION__, __LINE__);
			goto __exit;
		}
	}

	for (i = 0; i < initNumUrgent; i++) {
		if (GL_SUCCESS != (ret = GL_QueueSendUrgent(queueDump, (void*)msgbuf, sizeof(msgbuf) + 1))) {
			GL_TraceError("[%s-%4d]GL_QueueSendUrgent fail\n", __FUNCTION__, __LINE__);
			goto __exit;
		}
	}

	/* Get number of free normal, urgent msg spaces.  */
	GL_TraceError("Peek the queue and waana to see zero free spaces ...\n");
	GL_QueuePeekNormal(queueDump, &val1);
	GL_QueuePeekUrgent(queueDump, &val2);
	if ((val1 | val2) != 0) {
		ret = GL_FAILURE;
		GL_TraceError("[%s-%4d]GL_QueuePeek get abnormal values (%u, %u)!\n",
					  __FUNCTION__, __LINE__, val1, val2);
		goto __exit;
	}

	GL_TraceError("Do flush the queue ...\n");
	GL_QueueFlushNormal(queueDump);
	GL_QueueFlushUrgent(queueDump);

	GL_TraceError("Peek again ...\n");
	GL_QueuePeekNormal(queueDump, &val1);
	GL_QueuePeekUrgent(queueDump, &val2);
	if ((val1 != initNum) || (val2 != initNumUrgent)) {
		ret = GL_FAILURE;
		GL_TraceError("[%s-%4d] Error: queue have messages. (%u, %u)!\n",
					  __FUNCTION__, __LINE__, val1, val2);
		goto __exit;
	}

__exit:
	if (GL_SUCCESS != GL_QueueDelete(queueDump)) {
		GL_TraceError("[%s-%4d]GL_QueueDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	return ret;
}

void *thread_queue_receive(void *arg)
{
	GL_Status_t ret;
	GL_UINT32 msg;

	do {
		ret = GL_QueueReceive(*(GL_Queue_t *)arg, (void *)&msg, sizeof(msg), 2000);
		if (GL_SUCCESS == ret) {
			printf("Recv %u\n", msg);
		} else {
			break;
		}
	} while (1);

	return NULL;
}

GL_Status_t test_GL_QueueMultiThread(void)
{
	GL_Status_t ret = GL_SUCCESS;
	GL_Queue_t queue;
	GL_UINT32 const initNum = 10;
	GL_UINT32 const initNumUrgent = 10;
	GL_UINT32 i, val1, val2;
	void *val;
	pthread_t pthread_id[4];
	const int pthread_num = sizeof(pthread_id) / sizeof(pthread_id[0]);

	GL_TraceError(">>> Test multi threads work with queue\n");
	if (GL_SUCCESS != (ret = GL_QueueCreate("listDump", sizeof(i), initNum, initNumUrgent, &queue))) {
		GL_TraceError("[%s-%4d]GL_QueueCreate fail\n", __FUNCTION__, __LINE__);
		return ret;
	}

	for (i = 0; i < pthread_num; ++i) {
		if(0 != pthread_create(&pthread_id[i], NULL, thread_queue_receive, (void*)(&queue))){
			GL_TraceError("[%s-%4d]pthread_create fail\r\n", __FUNCTION__, __LINE__);
		}
	}

	sleep(1);

	GL_TraceError("Begin to send messages ...\n");
	for (i = 0; i < 100; ++i) {
		if (0 == (i & 0x01)) {
			GL_QueueSend(queue, (void *)&i, sizeof(i));
		} else {
			GL_QueueSendUrgent(queue, (void *)&i, sizeof(i));
		}
	}

	GL_TraceError("Wait threads ...\n");
	for (i = 0; i < pthread_num; ++i) {
		pthread_join(pthread_id[i], &val);
	}

	GL_TraceError("Peek the queue ...\n");
	GL_QueuePeekNormal(queue, &val1);
	GL_QueuePeekUrgent(queue, &val2);
	if ((val1 != initNum) || (val2 != initNumUrgent)) {
		ret = GL_FAILURE;
		GL_TraceError("[%s-%4d] Error: queue have messages. (%u, %u)!\n",
					  __FUNCTION__, __LINE__, val1, val2);
	}

	if (GL_SUCCESS != GL_QueueDelete(queue)) {
		GL_TraceError("[%s-%4d]GL_QueueDelete fail\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	return ret;
}

GL_Status_t Test_GL_Queue(void)
{
	int i;
	struct {
		char *name;
		GL_Status_t (*func)(void);
		int result;
	} test_patrn[] = {
		{"test_GL_QueueDump", test_GL_QueueDump, 0},
		{"test_GL_QueueFlush", test_GL_QueueFlush, 0},
		{"test_GL_QueueMultiThread", test_GL_QueueMultiThread, 0},
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
