// ?!
#include "gsl.h"
#include "gsl/gl_taskpool.h"

#if 0
#define tp_printf(fmt, arg...)
#else
#define tp_printf(fmt, arg...)  printf("[TASKPOOL] "fmt, ##arg)
#endif

/*
	Used to separate every stack of threads, and it can be used to check
	if stack usage is out of boundary.
*/
#define TASK_STACK_MAGIC_WORD	0xC8C81919

/* misc of taskpool */
struct taskpool_t {
	GL_UINT32 entries;
};
static struct taskpool_t taskpool;

/* Refer to "taskpool.c" */
extern struct gl_task_t gl_taskpool[];

static struct taskpool_inner_t {
	GL_Task_t handle;
//	GL_UINT32	state;
} *pool;

GL_Status_t GL_TaskPoolInit(void)
{
	GL_UINT32 i = 0;
	while (gl_taskpool[i].name != NULL) {
		i++;
	}
	taskpool.entries = i;
	pool = (struct taskpool_inner_t *)malloc(sizeof(struct taskpool_inner_t) * i);

	return GL_SUCCESS;
}

GL_Task_t GL_TaskPoolGetHandle(const char *name)
{
	GL_UINT32 i;
	struct gl_task_t *task;
	GL_Task_t ret = NULL;

	for (i = 0; i < taskpool.entries; ++i) {
		task = &gl_taskpool[i];
		if (strncmp(name, task->name, strlen(task->name)) == 0) {
			GL_TaskCreate((char *)task->name,
						task->entry,
						task->entry_arg,
						task->priority,
						task->stk_size,
						FALSE,
						&(pool[i].handle));
			ret = pool[i].handle;
#ifdef MEM_FENCE
			{
				gsl_thread_t *th = (gsl_thread_t *)pool[i].handle;
				mem_fence_tag_start(th->threadid, 0);
			}
#endif
			break;
		}
	}
	if (i == taskpool.entries) {
		tp_printf("Cannot find handle for task name '%s'\n", name);
		tp_printf("\tCaller addr = 0x%08x\n", (GL_UINT32)__builtin_return_address(0));
		return NULL;
	}
	for (++i; i < taskpool.entries; ++i) {
		task = &gl_taskpool[i];
		if (strncmp(name, task->name, strlen(task->name)) == 0) {
			tp_printf("~~~~~~~~~~~ Duplicate task entryies ~~~~~~~~~~~~~~~\n");
			tp_printf("~     Duplicate task names are not allowed.       ~\n");
			tp_printf("~         Please check 'taskpool.c'.              ~\n");
			tp_printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			tp_printf("\tTask name   = %s\n", task->name);
			tp_printf("\tCaller addr = 0x%08x\n", (GL_UINT32)__builtin_return_address(0));
			tp_printf("\n");
			asm volatile("break;");
		}
	}

	return ret;
}

GL_Status_t GL_TaskPoolActivate(GL_Task_t taskId)
{
	gsl_thread_t *th = (gsl_thread_t *)(taskId);

	GL_CHECK_BLOCK_IN_ISR();
	if (th == NULL || taskId == 0) {
		return GL_FAILURE;
	}
//?! machao@20111105	cyg_thread_resume(th->thread_handle);
    GL_TaskActivate(th);

	return GL_SUCCESS;
}

GL_Status_t GL_TaskPoolSelfDelete()
{
#ifdef MEM_FENCE
	/*
	 * The current taskpool mechanism is run-time creating task, that is the same
	 * task will not the same handle if create after deletion. Therefore, we
	 * should delete memory fence tag and the memory info of the task will be
	 * deleted.
	 */
	mem_fence_tag_end(cyg_thread_self(), 0);
	//mem_fence_tag_end(cyg_thread_self(), FENCE_TAG_OP_KEEP);
#endif
	GL_TaskSelfDelete();
	/* It should not reach here. */
	return GL_SUCCESS;
}

GL_Status_t GL_TaskPoolCheckStack(void)
{
	/* TODO */
	return GL_SUCCESS;
}

GL_Task_t GL_TaskPoolCurTask(void)
{
	GL_CHECK_BLOCK_IN_INTR();
	return GL_CurrentTask();
}

