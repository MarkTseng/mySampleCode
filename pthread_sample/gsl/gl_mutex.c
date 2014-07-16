#include <gsl.h>
#include "gl_common.h"

/* Mutex type */
#define MUTEX_NORMAL		0
#define MUTEX_RECURSIVE		1

#if GL_OBJECT_LIST_MUTEX
static gsl_list_t mutex_dump_list = { .next = NULL };
#endif

static inline GL_Status_t gl_MutexCreate(char *szName, GL_Mutex_t *pMutexId, int m_type)
{
	gsl_mutex_t *mutex = NULL;
	GL_INT32 err_mutex= -1;
	GL_INT32 err_lock = -1;

	if (!pMutexId) {
		return GL_FAILURE;
	}

	if(NULL == (mutex = (gsl_mutex_t *)malloc(sizeof(gsl_mutex_t)))){
		return GL_FAILURE;
	}

	if(0 != (err_lock = pthread_mutex_init(&mutex->lock, NULL))){
		free((void*)mutex);
		return GL_FAILURE;
	}

	if(0 != (err_mutex = pthread_mutex_init(&mutex->mutex, NULL))){
		free((void*)mutex);
		pthread_mutex_destroy(&mutex->lock);
		return GL_FAILURE;
	}

	memset(mutex, 0, sizeof(gsl_mutex_t));
	gsl_copy_name(mutex->name, szName);
	mutex->m_type = m_type;
	mutex->m_lock = 0;
	mutex->m_owner = 0;

	*pMutexId = (GL_Mutex_t) mutex;

#if GL_OBJECT_LIST_MUTEX
	gsl_insert_list(&mutex_dump_list, mutex);
#endif

	return GL_SUCCESS;
}

GL_Status_t GL_MutexCreate(char *szName, GL_Mutex_t *pMutexId)
{
	return gl_MutexCreate(szName, pMutexId, MUTEX_NORMAL);
}

GL_Status_t GL_MutexCreateRecursive(char *szName, GL_Mutex_t *pMutexId)
{
	return gl_MutexCreate(szName, pMutexId, MUTEX_RECURSIVE);
}

GL_Status_t GL_MutexDelete(GL_Mutex_t mutexId)
{
	GL_INT32 ret = 0;
	gsl_mutex_t *mutex = (gsl_mutex_t *)(mutexId);

	if (!mutex) {
		return GL_FAILURE;
	}

#if GL_OBJECT_LIST_MUTEX
	if(0 != gsl_remove_list(&mutex_dump_list, mutex)){
		GL_TraceError("error!func : %s, line : %d\n", __FUNCTION__, __LINE__);
	}
#endif

	switch(mutex->m_type){
	case MUTEX_NORMAL:
		pthread_mutex_lock(&mutex->lock);
		mutex->m_lock = 0;
		mutex->m_owner= 0;
		ret = pthread_mutex_destroy(&mutex->mutex);
		pthread_mutex_unlock(&mutex->lock);
		break;
	case MUTEX_RECURSIVE:
		pthread_mutex_lock(&mutex->lock);
		if(0 != mutex->m_lock) {
			pthread_mutex_unlock(&mutex->mutex);
		}
		mutex->m_lock = 0;
		mutex->m_owner= 0;
		ret = pthread_mutex_destroy(&mutex->mutex);
		pthread_mutex_unlock(&mutex->lock);
		break;
	default:
		ret = -1;
		break;
	}

	ret |= pthread_mutex_destroy(&mutex->lock);
	free((void *)mutex);

	return (0 == ret)? GL_SUCCESS : GL_FAILURE;
}

GL_Status_t GL_MutexLock(GL_Mutex_t mutexId)
{
	GL_INT32 ret = 0;
	gsl_mutex_t *mutex = (gsl_mutex_t *)(mutexId);

	if (!mutex) {
		return GL_FAILURE;
	}
	GL_SET_BLOCK_INFO(GL_BLOCK_MUTEX, mutexId);
	switch(mutex->m_type){
	case MUTEX_NORMAL:
		/*It will result dead lock when lock the normal mutex twice in the same task*/
		if((ret = pthread_mutex_lock(&mutex->mutex)) == 0){
			pthread_mutex_lock(&mutex->lock);
			mutex->m_lock = 1;
			mutex->m_owner = pthread_self();
			pthread_mutex_unlock(&mutex->lock);
		}
		break;
	case MUTEX_RECURSIVE:
		pthread_mutex_lock(&mutex->lock);

		//pthread_equal() return 1 if equal
		if( (mutex->m_lock == 0) || (!pthread_equal(mutex->m_owner, pthread_self())) ){
			pthread_mutex_unlock(&mutex->lock);
			//pthread_mutex_lock will hold until the other task unlock this mutex;
			ret = pthread_mutex_lock(&mutex->mutex);
			pthread_mutex_lock(&mutex->lock);
			if(ret == 0){
				mutex->m_lock = 1;
				mutex->m_owner = pthread_self();
			}
		}else{
			mutex->m_lock++;  //locked in the same task of MUTEX_RECURSIVE type
		}
		pthread_mutex_unlock(&mutex->lock);
		break;
	default:
		ret = -1;
		break;
	}
	GL_DEL_BLOCK_INFO(mutexId);
	return (0 == ret) ? GL_SUCCESS : GL_FAILURE;
}

GL_Status_t GL_MutexTryLock(GL_Mutex_t mutexId)
{
	GL_INT32 ret = -1;
	gsl_mutex_t *mutex = (gsl_mutex_t *)(mutexId);

	if (!mutex) {
		return GL_FAILURE;
	}

	switch(mutex->m_type){
	case MUTEX_NORMAL:
		if ((ret = pthread_mutex_trylock(&mutex->mutex)) == 0) {
			pthread_mutex_lock(&mutex->lock);
			mutex->m_lock = 1;
			mutex->m_owner = pthread_self();
			pthread_mutex_unlock(&mutex->lock);
		}
		break;
	case MUTEX_RECURSIVE:
		pthread_mutex_lock(&mutex->lock);
		if( (mutex->m_lock == 0) || (!pthread_equal(mutex->m_owner, pthread_self())) ){
			//pthread_mutex_lock will hold until the other task unlock this mutex;
			if((ret = pthread_mutex_trylock(&mutex->mutex)) == 0){
				mutex->m_lock = 1;
				mutex->m_owner = pthread_self();
			}
		}else{
			mutex->m_lock++;  //locked in the same task of MUTEX_RECURSIVE type
		}
		pthread_mutex_unlock(&mutex->lock);
		break;
	default:
		ret = -1;
		break;
	}

	return (0 == ret)? GL_SUCCESS : GL_FAILURE;
}

GL_Status_t GL_MutexUnlock(GL_Mutex_t mutexId)
{
	GL_INT32  ret = -1;
	gsl_mutex_t *mutex = (gsl_mutex_t *)(mutexId);

	if (!mutex) {
		return GL_FAILURE;
	}

	switch(mutex->m_type){
	case MUTEX_NORMAL:
		pthread_mutex_lock(&mutex->lock);
		if (0 == pthread_equal(mutex->m_owner, pthread_self())) {
			pthread_mutex_unlock(&mutex->lock);
			return GL_FAILURE;
		}
		mutex->m_lock = 0;
		mutex->m_owner = 0;
		ret = pthread_mutex_unlock(&mutex->mutex);
		pthread_mutex_unlock(&mutex->lock);
		break;
	case MUTEX_RECURSIVE:
		pthread_mutex_lock(&mutex->lock);
		if( (mutex->m_lock == 0) || (!pthread_equal(mutex->m_owner, pthread_self())) ){
			ret = -1;
		}else{  //(MUTEX_RECURSIVE) && (mutex->m_lock) && (mutex->m_owner == pthread_self())
			ret = 0;
			mutex->m_lock--;
			if(0 == mutex->m_lock){
				mutex->m_owner = 0;
				ret = pthread_mutex_unlock(&mutex->mutex);
			}
		}
		pthread_mutex_unlock(&mutex->lock);
		break;
	default:
		ret = -1;
		break;
	}
	return (0 == ret)? GL_SUCCESS : GL_FAILURE;
}

#if GL_OBJECT_LIST_MUTEX
GL_UINT8 GL_MutexIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_mutex_t *m;

	GL_SchedulerLock();
	for (p = &mutex_dump_list; p->next != NULL; p = (gsl_list_t *)(p->next)) {
		m = (gsl_mutex_t *)(p->next);
		if (0 == strncmp(name, m->name, GSL_NAME_LEN)) {
			GL_SchedulerUnLock();
			return 1;
		}
	}
	GL_SchedulerUnLock();
	return 0;
}

void GL_MutexDumpList(GL_UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_mutex_t *m;
	wChannel = wChannel;

	GL_TraceInfo("\n-------GSL mutex dump list start ------------\n");
	for (p = &mutex_dump_list; p->next != NULL; p = (gsl_list_t *)(p->next)) {
		m = (gsl_mutex_t *)(p->next);
		GL_TraceInfo("mutex name:%s  m_type=%d, m_lock = %d, m_owner=%d\n", m->name, m->m_type, m->m_lock, (int)m->m_owner);
	}
	GL_TraceInfo("\n-------GSL mutex dump list end  ------------\n");
}
#endif

