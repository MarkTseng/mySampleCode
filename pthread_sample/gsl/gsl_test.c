#include <cyg/hal/hal_arch.h>
#include <cyg/kernel/kapi.h>
#include <cyg/infra/testcase.h>

#include "config_sys.h"

#define CHECK_RES(__bool)	\
	do {					\
		if(!(__bool)) {		\
			diag_printf("check failed! file : %s, line : %d\n", __FILE__, __LINE__); \
			while(1) {		\
				continue;	\
			}				\
		}					\
	} while (0)

static GL_Task_t task1;
static GL_Task_t task2;
static GL_Task_t task3;
static GL_Queue_t queue_id1;

static void entry3(void *arg)
{
	char msg[215];
	GL_Status_t res;

	memset(msg, 'B', sizeof(msg));
	while (1) {
		res = GL_QueueSendUrgent(queue_id1, (void *)msg, 215);
		GL_TaskSleep(100);
		diag_printf(">>>>sent one urgent\n");
	}
}

static void entry2(void *arg)
{
	GL_Status_t res;
	int i;
	char rec_msg[326];

	while (1) {
		res = GL_QueueReceive(queue_id1, rec_msg, 326, GL_INFINITE_WAIT);
		if (res != GL_SUCCESS) {
			diag_printf("receive timeout\n");
			continue;
		}
		diag_printf("message recieved : dumped:\n");
		for (i = 0; i < 215; i++) {
			diag_printf("%x ", rec_msg[i]);
		}
		diag_printf("entry2 alive\n");
	}
}

static void entry1(void *arg)
{
	char msg[215];
	GL_Status_t res;

	memset(msg, 'A', sizeof(msg));
	while (1) {
		res = GL_QueueSend(queue_id1, (void *)msg, 215);
		GL_TaskSleep(80);
		diag_printf(">>>>sent one\n");
	}
}

extern void cyg_start(void)
{
	//CYG_TEST_INIT();
	//CYG_TEST_NA("Kernel C API layer disabled");
	GL_Status_t res;

	diag_printf("GSL test beginning\n");

	GL_Init();

	res = GL_QueueCreate("queue1", 511, 10, 5, &queue_id1);
	CHECK_RES((res == GL_SUCCESS)) res = GL_TaskCreate("Task1", entry1, NULL, 12, 4 * 1024, true, &task1);
	CHECK_RES((res == GL_SUCCESS)) res = GL_TaskCreate("Task2", entry2, NULL, 12, 4 * 1024, true, &task2);
	CHECK_RES((res == GL_SUCCESS)) res = GL_TaskCreate("Task3", entry3, NULL, 12, 4 * 1024, true, &task3);
	CHECK_RES((res == GL_SUCCESS)) cyg_scheduler_start();
}

