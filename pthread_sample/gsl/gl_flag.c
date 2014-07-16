#include <gsl.h>
#include "gl_common.h"

#if GL_OBJECT_LIST_FLAG
static gsl_list_t flag_dump_list = { .next = NULL };
#endif
/*
 * Flag
 */
GL_Status_t GL_FlagCreate(char *szName, GL_Flag_t *pFlagId)
{
	gsl_flag_t *flag;

	if (!pFlagId) {
		return GL_FAILURE;
	}

	flag = (gsl_flag_t *)malloc(sizeof(gsl_flag_t));
	if (flag == NULL) {
		return GL_FAILURE;
	}

	if(pthread_mutex_init(&flag->flock, NULL) < 0){
		free((void*)flag);
		return GL_FAILURE;
	}

	if(pthread_cond_init(&flag->fcond, NULL) != 0){
		pthread_mutex_destroy(&flag->flock);
		free((void*)flag);
		return GL_FAILURE;
	}

	gsl_copy_name(flag->name, szName);
	flag->flag = 0;

	*pFlagId = (GL_Semaphore_t) flag;

#if GL_OBJECT_LIST_FLAG
	gsl_insert_list(&flag_dump_list, flag);
#endif

	return GL_SUCCESS;
}

GL_Status_t GL_FlagDelete(GL_Flag_t pFlagId)
{
	gsl_flag_t *flag = (gsl_flag_t *)(pFlagId);
	int ret = 0;

	if (!flag) {
		return GL_FAILURE;
	}

#if GL_OBJECT_LIST_FLAG
	if (0 !=  gsl_remove_list(&flag_dump_list, flag)) {
		diag_printf("GL_FlagDelete error!func : %s, line : %d\n", __FUNCTION__, __LINE__);
	}
#endif

	flag->flag = 0;
	ret |= pthread_mutex_destroy(&flag->flock);
	ret |= pthread_cond_destroy(&flag->fcond);

	free((void *)flag);

	return (0 == ret) ? GL_SUCCESS : GL_FAILURE;
}

GL_Status_t GL_FlagSetbits(GL_Flag_t pFlagId, GL_UINT32 value_i)
{
	gsl_flag_t *flag = (gsl_flag_t *)(pFlagId);
	int ret = 0;

	if (!flag) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&flag->flock);
	flag->flag |= value_i;
	pthread_mutex_unlock(&flag->flock);
	ret = pthread_cond_broadcast(&flag->fcond);

	return (0 == ret) ? GL_SUCCESS : GL_FAILURE;
}

static GL_Status_t gsl_check_flag(GL_UINT32 flag, GL_UINT32 value, GL_UINT8 mode)
{
	if( (GL_FLAG_WAITMODE_OR & mode) !=0){
		return ((flag & value) != 0) ? GL_SUCCESS : GL_FAILURE;
	}else{
		// default is AND because of GL_FLAG_WAITMODE_AND == 0
		return ((flag & value) == value) ? GL_SUCCESS : GL_FAILURE;
	}
}

GL_UINT32 GL_FlagWait(GL_Flag_t pFlagId, GL_UINT32 value_i, GL_UINT8 mode_i, GL_INT32 sdTimeout)
{
	gsl_flag_t *flag = (gsl_flag_t *)(pFlagId);
	GL_UINT32 res = 0;
	int ret = 0;

	if (!flag) {
		return 0;
	}

	pthread_mutex_lock(&flag->flock);

	if (sdTimeout == GL_INFINITE_WAIT) {
		GL_SET_BLOCK_INFO(GL_BLOCK_FLAG, pFlagId);
		while(GL_SUCCESS != gsl_check_flag(flag->flag, value_i, mode_i)){
			ret = pthread_cond_wait(&flag->fcond, &flag->flock);
		}
		GL_DEL_BLOCK_INFO(pFlagId);
	}else{
		struct timespec tm;

		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += sdTimeout/1000;
		tm.tv_nsec += (sdTimeout%1000)*1000000;
		if(tm.tv_nsec >= 1000000000){
			tm.tv_nsec -= 1000000000;
			tm.tv_sec++;
		}
		while(GL_SUCCESS != gsl_check_flag(flag->flag, value_i, mode_i)){
			GL_SET_BLOCK_INFO(GL_BLOCK_FLAG, pFlagId);
			ret = pthread_cond_timedwait(&flag->fcond, &flag->flock, &tm);
			GL_DEL_BLOCK_INFO(pFlagId);
			if(ret != 0){
				break; //time out;
			}
		}
	}

	if( 0 == ret ){
		res = flag->flag;
		if( (GL_FLAG_WAITMODE_CLR & mode_i) != 0){
			flag->flag = 0;
		}
	}

	pthread_mutex_unlock(&flag->flock);

	return res;
}

GL_Status_t GL_FlagPeek(GL_Flag_t pFlagId, GL_UINT32 *value_o)
{
	gsl_flag_t *flag = (gsl_flag_t *)(pFlagId);

	if (!flag) {
		return GL_FAILURE;
	}

	//pthread_mutex_lock(&flag->flock);
	*value_o = flag->flag;
	//pthread_mutex_unlock(&flag->flock);

	return GL_SUCCESS;
}

#if GL_OBJECT_LIST_FLAG
GL_UINT8 GL_FlagIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_flag_t *f;

	GL_SchedulerLock();
	for (p = &flag_dump_list; p->next != NULL; p = (gsl_list_t *)(p->next)) {
		f = (gsl_flag_t *)(p->next);
		if (0 == strncmp(name, f->name, GSL_NAME_LEN)) {
			cyg_scheduler_unlock();
			return 1;
		}
	}
	GL_SchedulerLock();
	return 0;
}

void GL_FlagDumpList(GL_UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_flag_t *f;
	wChannel = wChannel;

	GL_TraceInfo("\n-------GSL Flag dump list start ------------\n");
	for (p = &flag_dump_list; p->next != NULL; p = (gsl_list_t *)(p->next)) {
		f = (gsl_flag_t *)(p->next);
		GL_TraceInfo("%s\tFlag %d\n", f->name, f->flag);
	}
	GL_TraceInfo("\n-------GSL Flag dump list end  ------------\n");
}
#endif


