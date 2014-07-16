/*
 * gl_cond.c
 *
 */
#include <gsl.h>
#include "gl_common.h"

#ifdef GL_OBJECT_LIST_COND
static gsl_list_t cond_dump_list = { .next = NULL };
#endif

#define perror_condv(prestr, err)    \
    fprintf(stderr, "[#e]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

#define pwarning_condv(prestr, err)    \
    fprintf(stderr, "[#w]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

/*
 *
 */
GL_Status_t GL_CondCreate(char *szName, GL_Cond_t *pCondId, GL_Mutex_t pMutexId)
{
    gsl_cond_t *cond;

    if (!pCondId || !pMutexId) {
        return GL_FAILURE;
    }

    cond = (gsl_cond_t *)malloc(sizeof(gsl_cond_t));
    if (NULL == cond) {
        return GL_FAILURE;
    }

    memset(cond, 0, sizeof(gsl_cond_t));
    strncpy(cond->name, szName, GSL_NAME_LEN);
    cond->mutex = (GL_Mutex_t *)pMutexId;
    int err = pthread_cond_init(&cond->cond, NULL);
    if (err != 0) {
        free(cond);
        perror_condv("pthread_cond_init", err);
        return GL_FAILURE;
    }

    *pCondId = (GL_Cond_t)cond;

#ifdef GL_OBJECT_LIST_COND
    gsl_insert_list(&cond_dump_list, cond);
#endif

    return GL_SUCCESS;
}

/*
 *
 */
GL_Status_t GL_CondDelete(GL_Cond_t pCondId)
{
    gsl_cond_t *cond = (gsl_cond_t *)(pCondId);

    GL_CHECK_BLOCK_IN_INTR();

    if (NULL == cond) {
        return GL_FAILURE;
    }

#ifdef GL_OBJECT_LIST_COND
    {
        if (gsl_remove_list(&cond_dump_list, cond) != 0) {
            printf("error!func : %s, line : %d\n", __FUNCTION__, __LINE__);
        }
    }
#endif

    int err = pthread_cond_destroy(&cond->cond);
    if (err != 0) {
        perror_condv("pthread_cond_destroy", err);
        return GL_FAILURE;
    }

    free((void *)cond);

    return GL_SUCCESS;
}

/*
 *
 */
GL_Status_t GL_CondWait(GL_Cond_t pCondId, GL_INT32 sdTimeout)
{
    gsl_cond_t *cond = (gsl_cond_t *)pCondId;
    gsl_mutex_t *mutex = (gsl_mutex_t *)cond->mutex;
    int err;

    GL_CHECK_BLOCK_IN_INTR();

    if (NULL == cond || NULL == mutex) {
        return GL_FAILURE;
    }

    GL_SET_BLOCK_INFO(GL_BLOCK_COND, pCondId);
    if (sdTimeout == GL_INFINITE_WAIT) {
        GL_CHECK_BLOCK_IN_INTR();
        GL_DEL_BLOCK_INFO(cond->mutex);
        GL_SET_BLOCK_INFO(GL_BLOCK_MUTEX, cond->mutex);
        err = pthread_cond_wait(&cond->cond, &mutex->mutex);
        if (0 != err) {
            perror_condv("pthread_cond_wait", err);
            err = GL_FAILURE;
        }
		pthread_mutex_lock(&mutex->lock);
		mutex->m_lock = 1;
		mutex->m_owner = pthread_self();
		pthread_mutex_unlock(&mutex->lock);
    }
    else if (sdTimeout != GL_NO_WAIT){
        GL_CHECK_BLOCK_IN_INTR();

        struct timespec ts;
        err = clock_gettime(CLOCK_REALTIME, &ts);
        if (err != 0) {
            perror_condv("clock_gettime", err);
            return GL_FAILURE;
        }
        ts.tv_sec += sdTimeout / 1000;
        ts.tv_nsec += (sdTimeout % 1000) * 1000000;
        for ( ; ts.tv_nsec >= 1000000000; ) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }

        GL_DEL_BLOCK_INFO(cond->mutex);
        GL_SET_BLOCK_INFO(GL_BLOCK_MUTEX, cond->mutex);
        err = pthread_cond_timedwait(&cond->cond, &mutex->mutex, &ts);
        if (0 == err) {
            err = GL_SUCCESS;
			pthread_mutex_lock(&mutex->lock);
			mutex->m_lock = 1;
			mutex->m_owner = pthread_self();
			pthread_mutex_unlock(&mutex->lock);
        }
        else if (ETIMEDOUT == err){
            pwarning_condv("pthread_cond_timedwait", err);
            err = GL_TIMEOUT;
        }
        else {
            perror_condv("pthread_cond_timedwait", err);
            err = GL_FAILURE;
        }
    }
    else {
        err = GL_FAILURE;
    }
    GL_DEL_BLOCK_INFO(pCondId);

    return err;
}

/*
 *
 */
GL_Status_t GL_CondSignal(GL_Cond_t pCondId)
{
    gsl_cond_t *cond = (gsl_cond_t *)pCondId;

    GL_CHECK_BLOCK_IN_INTR();

    if (NULL == cond) {
        return GL_FAILURE;
    }

    int err = pthread_cond_signal(&cond->cond);
    if (err != 0) {
        perror_condv("pthread_cond_signal", err);
        return GL_FAILURE;
    }

    return GL_SUCCESS;
}

/*
 *
 */
GL_Status_t GL_CondBroadcast(GL_Cond_t pCondId)
{
    gsl_cond_t *cond = (gsl_cond_t *)pCondId;

    GL_CHECK_BLOCK_IN_INTR();

    if (NULL == cond) {
        return GL_FAILURE;
    }

    int err = pthread_cond_broadcast(&cond->cond);
    if (err != 0) {
        perror_condv("pthread_cond_broadcast", err);
        return GL_FAILURE;
    }

    return GL_SUCCESS;
}

/*
 *
 */
#ifdef GL_OBJECT_LIST_COND
UINT8 GL_CondIsDupName(const char *name)
{
    gsl_list_t *p;
    gsl_cond_t *c;

// ?   cyg_scheduler_lock();
    for (p = &cond_dump_list; p->next != NULL; p = (gsl_list_t *)(p->next)) {
        c = (gsl_cond_t *)(p->next);
        if (0 == strncmp(name, c->name, GSL_NAME_LEN)) {
            cyg_scheduler_unlock();
            return 1;
        }
    }
// ?   cyg_scheduler_unlock();
    return 0;
}

void GL_CondDumpList(UINT16 wChannel)
{
    gsl_list_t *p;
    gsl_cond_t *c;

    wChannel = wChannel;
    printf("\n-------GSL condition var dump list start ------------\n");
    for (p = &cond_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
        c = (gsl_cond_t *)p->next;
        printf("%s\n", c->name);
    }
    printf("\n-------GSL condition var dump list end  ------------\n");
}
#endif

#ifdef GL_OBJECT_LIST_COND
UINT8 GL_CondIsDupName(const char *name)
{
    gsl_list_t *p;
    gsl_cond_t *c;

    cyg_scheduler_lock();
    for (p = &cond_dump_list; p->next != NULL; p = (gsl_list_t *)(p->next)) {
        c = (gsl_cond_t *)(p->next);
        if (0 == strncmp(name, c->name, GSL_NAME_LEN)) {
            cyg_scheduler_unlock();
            return 1;
        }
    }
    cyg_scheduler_unlock();
    return 0;
}

void GL_CondDumpList(UINT16 wChannel)
{
    gsl_list_t *p;
    gsl_cond_t *c;

    wChannel = wChannel;
    diag_printf("\n-------GSL condition var dump list start ------------\n");
    for (p = &cond_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
        c = (gsl_cond_t *)p->next;
        diag_printf("%s\n", c->name);
    }
    diag_printf("\n-------GSL condition var dump list end  ------------\n");
}
#endif

