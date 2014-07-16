#include <gsl.h>
#include <pthread.h>

typedef struct gsl_pthread_attr_t {
	pthread_attr_t thread_attr;
} gsl_pthread_attr;

typedef struct gsl_pthread_t {
	pthread_t thread_handle;
} gsl_pthread;

/* add@machao */
#define CYGNUM_POSIX_PTHREAD_THREADS_MAX    256
#define GL_PTHREAD_INHERIT_SCHED	PTHREAD_INHERIT_SCHED
#define GL_PTHREAD_EXPLICIT_SCHED	PTHREAD_EXPLICIT_SCHED

static GL_UINT32 thread_info_next = 0;
static gsl_pthread *gHandle[CYGNUM_POSIX_PTHREAD_THREADS_MAX];

GL_Status_t GL_PthreadInit(void)
{
	memset(gHandle, 0, sizeof(gHandle));
	return GL_SUCCESS;
}

GL_Status_t GL_PthreadCreate(GL_Pthread_t *pHandle, GL_PthreadAttr_t pAttr,
							 void *(*pEntryPoint)(void *pArg), void *pTaskArg)
{
	gsl_pthread *hnd;
	gsl_pthread_attr *attr = (gsl_pthread_attr *)pAttr;

	if (!pHandle) {
		return GL_FAILURE;
	}

	hnd = (gsl_pthread *)malloc(sizeof(*hnd));
	if (attr) {
		pthread_create(&hnd->thread_handle, &(attr->thread_attr), pEntryPoint, pTaskArg);
	} else {
		pthread_create(&hnd->thread_handle, NULL, pEntryPoint, pTaskArg);
	}
	*pHandle = (GL_Pthread_t)hnd;

	GL_UINT32 thread_next = thread_info_next;
	while (gHandle[thread_next] != NULL) {
		thread_next++;
		if (thread_next >= CYGNUM_POSIX_PTHREAD_THREADS_MAX) {
			thread_next = 0;
		}
		if (thread_next == thread_info_next) {
			return GL_FAILURE;
		}
	}
	thread_info_next = thread_next;
	gHandle[thread_next] = hnd;

	return GL_SUCCESS;
}

void GL_PthreadExit(void *pRet)
{
	GL_UINT32 i;

	pthread_t handle = pthread_self();
	for (i = 0; i < CYGNUM_POSIX_PTHREAD_THREADS_MAX; ++i) {
		if (gHandle[i] == NULL) {
			continue;
		}
		if (gHandle[i]->thread_handle == handle) {
			free(gHandle[i]);
			gHandle[i] = NULL;
		}
	}
	pthread_exit(pRet);
}

GL_Status_t GL_PthreadAttrInit(GL_PthreadAttr_t *pAttr)
{
	gsl_pthread_attr *hnd;

	if (!pAttr) {
		return GL_FAILURE;
	}

	hnd = (gsl_pthread_attr *)malloc(sizeof(*hnd));
	pthread_attr_init(&(hnd->thread_attr));
	*pAttr = (GL_PthreadAttr_t)hnd;

	return GL_SUCCESS;
}

GL_Status_t GL_PthreadAttrDestroy(GL_PthreadAttr_t pAttr)
{
	if (!pAttr) {
		return GL_FAILURE;
	}

	free(pAttr);
	return GL_SUCCESS;
}

GL_Status_t GL_PthreadAttrSetinheritsched(GL_PthreadAttr_t pAttr, GL_UINT32 inherit)
{
	gsl_pthread_attr *hnd = (gsl_pthread_attr *)pAttr;

	if (!hnd) {
		return GL_FAILURE;
	}

	if (inherit == GL_PTHREAD_INHERIT_SCHED || inherit == GL_PTHREAD_EXPLICIT_SCHED) {
	    /* modify@machao */
        //hnd->thread_attr.inheritsched = inherit;
        pthread_attr_setinheritsched(&(hnd->thread_attr), inherit);
	}

	return GL_SUCCESS;
}

GL_Status_t GL_PthreadAttrGetStkSize(GL_PthreadAttr_t pAttr, GL_UINT32 *pSize)
{
	gsl_pthread_attr *hnd = (gsl_pthread_attr *)pAttr;

	if (!pAttr) {
		return GL_FAILURE;
	}
	pthread_attr_getstacksize(&(hnd->thread_attr), pSize);
	return GL_SUCCESS;
}

GL_Status_t GL_PthreadAttrSetStkSize(GL_PthreadAttr_t pAttr, GL_UINT32 dSize)
{
	gsl_pthread_attr *hnd = (gsl_pthread_attr *)pAttr;

	if (!pAttr) {
		return GL_FAILURE;
	}
	pthread_attr_setstacksize(&(hnd->thread_attr), dSize);
	return GL_SUCCESS;
}

