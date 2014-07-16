#include <gsl.h>
#include "gl_common.h"

#if GL_OBJECT_LIST_SEMA
static gsl_list_t sem_dump_list = { .next = NULL };
#endif

/*
 * Semaphore
 */

GL_Status_t GL_SemaphoreCreate(char *szName, GL_UINT32 dMaxCount, GL_UINT32 dInitialValue, GL_Semaphore_t *pSemId)
{

	gsl_semaphore_t *sem = NULL;

	if (!pSemId) {
		return GL_FAILURE;
	}

	if ((dMaxCount < dInitialValue) || (0 == dMaxCount)) {
		return GL_FAILURE;
	}

	sem = (gsl_semaphore_t *)malloc(sizeof(gsl_semaphore_t));
	if (sem == NULL) {
		return GL_FAILURE;
	}

	memset(&sem->sem, 0, sizeof(GL_SEM_T));
	if(sem_init(&sem->sem, 0, dInitialValue) <0){ //shared between threads
		free((void*)sem);
		return GL_FAILURE;
	}

	if(pthread_mutex_init(&sem->lock, NULL) < 0){ //lock for atomic operation
		sem_destroy(&sem->sem);
		free((void*)sem);
		return GL_FAILURE;
	}

	gsl_copy_name((GL_INT8*)sem->name, szName);
	sem->max_count = dMaxCount;

	*pSemId = (GL_Semaphore_t) sem;

#if GL_OBJECT_LIST_SEMA
	gsl_insert_list(&sem_dump_list, sem);
#endif

	return GL_SUCCESS;
}

GL_Status_t GL_SemaphoreDelete(GL_Semaphore_t semId)
{
	gsl_semaphore_t *sem = (gsl_semaphore_t *)(semId);
	int ret = 0;

	if ((!sem) || (!sem->max_count)) {
		return GL_FAILURE;
	}

	/*if sem_destroy while pending or before pend, undefined behaviour*/
	/*in ubuntu10.04, wait and post still work after sem_destroy */
	pthread_mutex_lock(&sem->lock);

	sem->max_count = 0; /*note that the sem_destroy() has been called*/

#if GL_OBJECT_LIST_SEMA
	if(0 != gsl_remove_list(&sem_dump_list, sem)){
		GL_TraceError("GL_SemaphoreDelete error!func : %s, line : %d\n", __FUNCTION__, __LINE__);
	}
#endif

	ret = sem_destroy(&sem->sem);

	pthread_mutex_unlock(&sem->lock);

	ret |= pthread_mutex_destroy(&sem->lock);
	free((void *)sem);

	return (0 == ret)? GL_SUCCESS : GL_FAILURE;
}

GL_Status_t GL_SemaphorePeek(GL_Semaphore_t semId, GL_UINT32 *pVar)
{
	gsl_semaphore_t *sem = (gsl_semaphore_t *)(semId);

	if ((!sem) || (!sem->max_count)) {
		return GL_FAILURE;
	}

	if(sem_getvalue(&sem->sem, (GL_INT32 *) pVar) < 0){
		return GL_FAILURE;
	}
	return GL_SUCCESS;
}

GL_Status_t GL_SemaphoreGive(GL_Semaphore_t semId)
{
	gsl_semaphore_t *sem = (gsl_semaphore_t *)(semId);
	GL_INT32 count=0;
	GL_UINT32 ret=GL_SUCCESS;

	if ((!sem) || (!sem->max_count)) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&sem->lock);

	if(sem_getvalue(&sem->sem, &count) < 0){
		ret = GL_FAILURE;
		goto __sem_give_exit;
	}

	if (count >= sem->max_count){
		ret = GL_COUNT_EXCEEDED;
		goto __sem_give_exit;
	}

	if(sem_post(&sem->sem) < 0){
		ret = GL_FAILURE;
		goto __sem_give_exit;
	}

__sem_give_exit:

	pthread_mutex_unlock(&sem->lock);

	return ret;
}

GL_Status_t GL_SemaphoreTake(GL_Semaphore_t semId, GL_INT32 sdTimeout)
{
	GL_INT32 ret = 0;
	gsl_semaphore_t *sem = (gsl_semaphore_t *)(semId);
	struct timespec tm;

	if ((!sem) || (!sem->max_count)) {
		return GL_FAILURE;
	}

	switch (sdTimeout) {
	case GL_NO_WAIT:
	case GL_INFINITE_WAIT:
		break;
	default:
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += sdTimeout / 1000;
		tm.tv_nsec += (sdTimeout % 1000) * 1000000;
		if(tm.tv_nsec >= 1000000000){
			tm.tv_nsec -= 1000000000;
			tm.tv_sec++;
		}
	}

	do {
		GL_SET_BLOCK_INFO(GL_BLOCK_SEMA, semId);
		if (sdTimeout == GL_INFINITE_WAIT) {
			ret = sem_wait(&sem->sem);
		} else if (sdTimeout == GL_NO_WAIT) {
			ret = sem_trywait(&sem->sem);
		} else {
			ret = sem_timedwait(&sem->sem, &tm);
		}
		GL_DEL_BLOCK_INFO(semId);
		if (__builtin_expect(0 != ret, 0)) {
			switch (errno) {
			case ETIMEDOUT:
				return GL_TIMEOUT;
			case EINTR:
				continue;
			default:
				return GL_FAILURE;
			}
		} else {
			return GL_SUCCESS;
		}
	} while (1);
}

#if GL_OBJECT_LIST_SEMA
GL_UINT8 GL_SemaphoreIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_semaphore_t *s;

	GL_SchedulerLock();
	for (p = &sem_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		s = (gsl_semaphore_t *)p->next;
		if (0 == strncmp(name, s->name, GSL_NAME_LEN)) {
			GL_SchedulerUnLock();
			return 1;
		}
	}
	GL_SchedulerUnLock();
	return 0;
}

void GL_SemaphoreDumpList(GL_UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_semaphore_t *s;
	GL_INT32 value;

	wChannel = wChannel;
	GL_TraceInfo("\n-------GSL semaphore dump list start ------------\n");
	for (p = &sem_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		s = (gsl_semaphore_t *)p->next;
		if(GL_SUCCESS != GL_SemaphorePeek(s, &value)){
			GL_TraceError("[%s]GL_SemaphorePeek fail\n", __FUNCTION__);
		}
		GL_TraceInfo("sem name:%s, current value=%d\n", s->name, value);
	}
	GL_TraceInfo("\n-------GSL semaphore dump list end  ------------\n");

}
#endif

