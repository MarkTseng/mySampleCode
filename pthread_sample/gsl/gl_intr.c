#include <sys/select.h>
#include <gsl.h>
#include <gsl/lambo86_hwop.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include "gl_common.h"

#ifdef GL_DEBUG_INTR
#define gl_intr_printf(fmt, arg...)	gsl_printf(fmt, ##arg)
#else
#define gl_intr_printf(fmt, arg...)
#endif

#define SUPPORT_GSL_INTR_EPOLL
#define SUPPORT_GSL_RIORITY_INTERRUPT

// 20120921: Mark, change ISR to multi-thread base.
#define MULTI_THREAD_ISR
#ifdef MULTI_THREAD_ISR
	#undef SUPPORT_GSL_INTR_EPOLL
	#undef SUPPORT_GSL_RIORITY_INTERRUPT
#endif


#define GL_INTR_SIGNATURE		0x494E5452	/* ASCII of 'INTR' */

#if 0
#define INTR_ACK(i)		cyg_interrupt_acknowledge((i))
#define INTR_MASK(i)	cyg_interrupt_mask((cyg_vector_t)(i))
#define INTR_UNMASK(i)	cyg_interrupt_unmask((cyg_vector_t)(i))
#define INTR_DISABLE(i)	HAL_DISABLE_INTERRUPTS((i))
#define INTR_ENABLE(i)	HAL_RESTORE_INTERRUPTS((i))
#else
#define INTR_ACK(i)
#define INTR_MASK(i)				\
	do {							\
		int fd = pool.fd[(i)];		\
		if (fd > 0) {				\
			lambo86_irq_free(fd);	\
		}							\
	} while (0)
#define INTR_UNMASK(i)				\
	do {							\
		int fd = pool.fd[(i)];		\
		if (fd > 0) {				\
			lambo86_irq_request(fd);\
		}							\
	} while (0)
#define INTR_DISABLE(i)
#define INTR_ENABLE(i)
#endif

#define CYGNUM_HAL_ISR_COUNT 	64
#define CYGNUM_HAL_ISR_MAX		96

#define NUM_MAX_RECORD	3
#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_USAGE
extern cyg_uint32 cyg_interrupt_isr_record[CYGNUM_HAL_ISR_COUNT][NUM_MAX_RECORD];
extern cyg_uint32 cyg_interrupt_dsr_record[CYGNUM_HAL_ISR_COUNT][NUM_MAX_RECORD];
extern cyg_uint64 cyg_interrupt_isr_clk[CYGNUM_HAL_ISR_COUNT];
extern cyg_uint64 cyg_interrupt_dsr_clk[CYGNUM_HAL_ISR_COUNT];
extern cyg_uint32 cyg_interrupt_isr_cnt[CYGNUM_HAL_ISR_COUNT];
extern cyg_uint32 cyg_interrupt_dsr_cnt[CYGNUM_HAL_ISR_COUNT];
#endif

//!? machao@20101110
typedef UINT32 cyg_handle_t;
typedef UINT32 cyg_interrupt;
typedef UINT32 cyg_uint32;
typedef UINT32 cyg_vector_t;
typedef UINT32 cyg_addrword_t;
typedef UINT32 cyg_ucount32;
typedef UINT32 CYG_INTERRUPT_STATE;

struct GL_IntrHnd_s;
typedef struct GL_IsrHnd_s {
	struct GL_IntrHnd_s *pIntrHnd;
	UINT8 szName[GL_INTR_MAX_NAME_LEN + 1];
	UINT8 fCallIsr;			/* enable or disable the ISR */
	UINT8 fCallDsr;
	UINT8 bEnable;			// 20101207
	GL_IntrIsr_t isr;
	GL_IntrDsr_t dsr;
	UINT32 dTimeMax[2][NUM_MAX_RECORD];	// ISR and DSR
	UINT64 dTimeSum[2];	// executing time
	UINT32 dTimeCnt[2];	// executing times
	void *pParam;
} GL_IsrHnd;

typedef struct GL_IntrHnd_s {
	UINT32 dSignature;
	cyg_handle_t handle;
	cyg_interrupt intr;
	UINT8 bEmpty;
	UINT8 bShareCnt;
	UINT8 bNoUnmask;
	UINT32 dVecNum;
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
	UINT8 bPriority; // skypine 20120824 for linux user space fld intr 
#endif
	GL_IsrHnd *pIsrHnd[0];
} GL_IntrHnd;

typedef struct GL_IntrList_s {
	GL_IntrHnd *pIntrHnd;
	struct GL_IntrList_s *next;
} GL_IntrList;

typedef struct {
	UINT32 dEntry;
	GL_IntrList *pIntrList;
	INT32 fd[CYGNUM_HAL_ISR_MAX];
} GL_Isrpool_t;

static volatile signed char run = 0;
static GL_Task_t gTaskHndIntr;
#ifdef MULTI_THREAD_ISR
static GL_Task_t gTaskHndISR[20];
static int gTaskHndISRCnt = 0;
#endif
static GL_Isrpool_t pool;
static struct gl_intr_t *__gl_intrpool = NULL;
static GL_Mutex_t sIntrMutex;
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
static GL_Task_t gTaskHndIntr2;
#endif

static inline UINT32 service_routine_inner(GL_IsrHnd *h, UINT32 idx)
{
	UINT32 time1, time2;
	UINT32 ret;
	UINT32 (*sr)(void *);
	UINT32 *dTimeMax;

	if (0 == idx) {
		sr = h->isr;
		dTimeMax = h->dTimeMax[0];
	} else {
		sr = h->dsr;
		dTimeMax = h->dTimeMax[1];
	}
	time1 = GL_GetCpuCnt();
	ret = sr(h->pParam);
	time2 = GL_GetCpuCnt();
	asm volatile ("subu	%0, %2, %1;" : "=r"(time2) : "r"(time1), "r"(time2));
	h->dTimeSum[idx] += time2;
	h->dTimeCnt[idx] += 1;
	if (dTimeMax[0] < time2) {
		if (dTimeMax[1] < time2) {
			dTimeMax[0] = dTimeMax[1];
			if (dTimeMax[2] < time2) {
				dTimeMax[1] = dTimeMax[2];
				dTimeMax[2] = time2;
			} else {
				dTimeMax[1] = time2;
			}
		} else {
			dTimeMax[0] = time2;
		}
	}
	return ret;
}

static cyg_uint32 gsl_chain_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	int i;
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *h;
	GL_IsrReturn_t ret;
	UINT8 fCallDsr;

	pIntrHnd = (GL_IntrHnd *)data;
	pIntrHnd->bNoUnmask = FALSE;
	fCallDsr = FALSE;
	for (i = 0; i < pIntrHnd->bEmpty; i++) {
		h = pIntrHnd->pIsrHnd[i];
		if (TRUE == h->fCallIsr) {
			ret = (GL_IsrReturn_t)service_routine_inner(h, 0);
			if (ret & GL_ISR_CALL_DSR) {
				h->fCallDsr = TRUE;
				fCallDsr = TRUE;
			}
			if (ret & GL_ISR_NO_UNMASK) {
				pIntrHnd->bNoUnmask = TRUE;
			}
		}
	}
	INTR_ACK(vector);

	if (fCallDsr) {
		/* It is user-mode isr so we don't need to mask interrupt.  */
//		INTR_MASK(vector);
		return (GL_ISR_HANDLED | GL_ISR_CALL_DSR);
	}

	return GL_ISR_HANDLED;
}

static void gsl_chain_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	int i;
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *h;
	GL_DsrReturn_t ret;

	pIntrHnd = (GL_IntrHnd *)data;
	for (i = 0; i < pIntrHnd->bEmpty; i++) {
		h = pIntrHnd->pIsrHnd[i];
		if ((h->fCallDsr == TRUE) && (h->dsr != NULL)) {
			ret = (GL_DsrReturn_t)service_routine_inner(h, 1);
			if (ret & GL_DSR_NO_UNMASK) {
				pIntrHnd->bNoUnmask = TRUE;
			}
		}
		h->fCallDsr = FALSE;
	}

	if (TRUE == pIntrHnd->bNoUnmask) {
		INTR_MASK(vector);
	}
}

static cyg_uint32 gsl_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *h;
	GL_IsrReturn_t ret;
	cyg_uint32 retval = GL_ISR_HANDLED;

	pIntrHnd = (GL_IntrHnd *)data;
	pIntrHnd->bNoUnmask = FALSE;
	h = pIntrHnd->pIsrHnd[0];
	if (h->fCallIsr == TRUE) {
		ret = (GL_IsrReturn_t)service_routine_inner(h, 0);
		if (ret & GL_ISR_NO_UNMASK) {
			pIntrHnd->bNoUnmask = TRUE;
		}
		if (ret & GL_ISR_CALL_DSR) {
			/* It is user-mode isr so we don't need to mask interrupt.  */
//			INTR_MASK(vector);
			retval |= GL_ISR_CALL_DSR;
		}
	}
	INTR_ACK(vector);

	return retval;
}

static void gsl_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *h;
	GL_DsrReturn_t ret;

	pIntrHnd = (GL_IntrHnd *)data;
	h = pIntrHnd->pIsrHnd[0];
	if (h->dsr != NULL) {
		ret = (GL_DsrReturn_t)service_routine_inner(h, 1);
		if (ret & GL_DSR_NO_UNMASK) {
			pIntrHnd->bNoUnmask = TRUE;
		}
	}
	if (TRUE == pIntrHnd->bNoUnmask) {
		INTR_MASK(vector);
	}
}

static GL_IntrHnd *gl_IntrCreate(UINT32 dVecNum, UINT8 bShareCnt)
{
	GL_IntrHnd *pIntrHnd;
	UINT32 dSize, dHandlerSize;
	int fd;

	if (0 == bShareCnt) {
		return NULL;
	}

	dHandlerSize = sizeof(GL_IsrHnd) * bShareCnt;
	dSize = sizeof(GL_IntrHnd) + dHandlerSize;
	pIntrHnd = (GL_IntrHnd *)malloc(dSize);

	pIntrHnd->dVecNum = dVecNum;
	pIntrHnd->bShareCnt = bShareCnt;
	pIntrHnd->bEmpty = 0;
	// clear all handler
	memset(&(pIntrHnd->pIsrHnd[0]), 0, dHandlerSize);

	if ((fd = lambo86_irq_proc_open(dVecNum)) > 0) {
		pool.fd[dVecNum] = fd;
	} else {
		pool.fd[dVecNum] = 0;
	}
	pIntrHnd->dSignature = GL_INTR_SIGNATURE;

	return pIntrHnd;
}

static void gl_IntrDelete(GL_IntrHnd *pIntrHnd)
{
	UINT32 dVecNum;

	dVecNum = pIntrHnd->dVecNum;
	if (GL_INTR_SIGNATURE != pIntrHnd->dSignature) {
		gl_intr_printf("wrong vector number: %d\n", dVecNum);
		return;
	}

	gl_intr_printf("delete handler of vector number %d\n", dVecNum);
	if (pool.fd[dVecNum] > 0) {
		lambo86_irq_proc_close(pool.fd[dVecNum]);
	}

	free(pIntrHnd);
}

static inline GL_IsrHnd *gl_IsrCreate(GL_IntrHnd *pIntrHnd, UINT32 idxPool)
{
	GL_IsrHnd *pIsrHnd = NULL;
	struct gl_intr_t *intr;

	intr = &__gl_intrpool[idxPool];
	pIsrHnd = (GL_IsrHnd *)malloc(sizeof(GL_IsrHnd));
	memset(pIsrHnd, 0, sizeof(*pIsrHnd));
	strncpy((char *)pIsrHnd->szName, (char *)intr->name, GL_INTR_MAX_NAME_LEN);
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
	pIntrHnd->bPriority = intr->bPriority;
#endif
	pIsrHnd->pIntrHnd = pIntrHnd;
	pIsrHnd->isr = intr->isr;
	pIsrHnd->dsr = intr->dsr;
	pIsrHnd->pParam = (void *)pIsrHnd;

	gl_intr_printf("Create ISR object, name=%s, isrObj = 0x%08X\n", intr->name, (UINT32)pIsrHnd);
	return (GL_IntrObjHandle_t)pIsrHnd;
}

static inline void gl_IsrDelete(GL_IsrHnd *pIsrHnd)
{
	if (NULL == pIsrHnd) {
		return;
	}
	free(pIsrHnd);
}

static inline void gl_IntrMask(UINT32 dVecNum)
{
	INTR_MASK((cyg_vector_t)dVecNum);
}

static inline void gl_IntrUnmask(UINT32 dVecNum)
{
	INTR_UNMASK(dVecNum);
}
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
static int g_sdCurFld;
extern int monflg;
static GL_IntrHnd *pstIntrHdlPend = NULL;
static int sdPendFld = -1;
static int sdPendCnt = 0;
static void gl_FldTodo(void *p)
{
	GL_Isrpool_t *ppl = (GL_Isrpool_t *)p;
	int cnt = 0;
	GL_IntrList *plst;
	struct epoll_event ev,*events;
	for (plst = ppl->pIntrList; plst; plst = plst->next) 
	{
		if (plst->pIntrHnd->bPriority == 0)
		{
			cnt++;
		}
	}

	events = (struct epoll_event *) GL_MemAlloc(sizeof(struct epoll_event)*cnt);
	int epfd=epoll_create(cnt);
	for (plst = ppl->pIntrList; plst; plst = plst->next) 
	{
		if (plst->pIntrHnd->bPriority == 0)
		{
			ev.data.ptr = plst->pIntrHnd;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(epfd, EPOLL_CTL_ADD, ppl->fd[plst->pIntrHnd->dVecNum], &ev);
		}
	}
	while (run) {
		int maxrdy;
		if ((maxrdy = epoll_wait(epfd, events, cnt, -1)) <= 0)
		{
			if (maxrdy == 0)
				continue;
			switch (errno) {
			case EINTR:
				continue;
			default:
				printf("[##]GL_IntrTodo select, errno = %d, %s ...\n", errno, strerror(errno));
				continue;
			}
		}
		int i;
		GL_IntrHnd *pstIntrHdl;
		for (i = 0; i < maxrdy; i++)
		{
			pstIntrHdl = (GL_IntrHnd *)events[i].data.ptr;
			read(ppl->fd[pstIntrHdl->dVecNum], &cnt, sizeof(cnt));
			if (cnt > 0)
			{
				if (pstIntrHdl->dVecNum == INTR_NUM_TGEN_FLD_START)
				{
					if (g_sdCurFld == 0)
					{
						pstIntrHdlPend = pstIntrHdl;
						sdPendFld = 0;
						sdPendCnt = cnt;
						//write(ppl->fd[pstIntrHdl->dVecNum], (void *)cnt, sizeof(cnt));
						continue;
					}
					g_sdCurFld = 0;
				}
				else if (pstIntrHdl->dVecNum == INTR_NUM_TGEN_FLD_END)
				{
					if (g_sdCurFld == 1)
					{
						pstIntrHdlPend = pstIntrHdl;
						sdPendFld = 1;
						sdPendCnt = cnt;
						//write(ppl->fd[pstIntrHdl->dVecNum], (void *)cnt, sizeof(cnt));
						continue;
					}
					g_sdCurFld = 1;
				}
				GL_MutexLock(sIntrMutex);
				if (pstIntrHdl->bShareCnt > 1) {
					if (gsl_chain_isr(pstIntrHdl->dVecNum, (cyg_addrword_t)pstIntrHdl) & GL_ISR_CALL_DSR) {
						gsl_chain_dsr(pstIntrHdl->dVecNum, 0, (cyg_addrword_t)pstIntrHdl);
					}
				} else {
					if (gsl_isr(pstIntrHdl->dVecNum, (cyg_addrword_t)pstIntrHdl) & GL_ISR_CALL_DSR) {
						gsl_dsr(pstIntrHdl->dVecNum, 0, (cyg_addrword_t)pstIntrHdl);
					}
				}
				GL_MutexUnlock(sIntrMutex);
			}
			write(ppl->fd[pstIntrHdl->dVecNum], (void *)cnt, sizeof(cnt));
		}

		if (pstIntrHdlPend)
		{
			if (sdPendFld == g_sdCurFld)
			{
				if (monflg == 0x234)
					diag_printf("same pending field[%d]\n", sdPendFld);
			}
			else
			{
				if (monflg == 0x234)
					diag_printf("diff pending field[%d/%d]\n", sdPendFld, g_sdCurFld);
				GL_MutexLock(sIntrMutex);
				if (pstIntrHdlPend->bShareCnt > 1) {
					if (gsl_chain_isr(pstIntrHdlPend->dVecNum, (cyg_addrword_t)pstIntrHdlPend) & GL_ISR_CALL_DSR) {
						gsl_chain_dsr(pstIntrHdlPend->dVecNum, 0, (cyg_addrword_t)pstIntrHdlPend);
					}
				} else {
					if (gsl_isr(pstIntrHdlPend->dVecNum, (cyg_addrword_t)pstIntrHdlPend) & GL_ISR_CALL_DSR) {
						gsl_dsr(pstIntrHdlPend->dVecNum, 0, (cyg_addrword_t)pstIntrHdlPend);
					}
				}
				GL_MutexUnlock(sIntrMutex);
				write(ppl->fd[pstIntrHdlPend->dVecNum], (void *)sdPendCnt, sizeof(sdPendCnt));
				g_sdCurFld = sdPendFld;
				pstIntrHdlPend = NULL;
			}
		}
	}
}
#endif
#ifndef SUPPORT_GSL_INTR_EPOLL
#ifndef MULTI_THREAD_ISR
static void gl_IntrTodo(void *p)
{
	GL_Isrpool_t *ppl = (GL_Isrpool_t *)p;
	GL_IntrList *plst;
	int no, cnt;
	int maxfd = 0;
	int fd;
	fd_set fdset;
	fd_set rfdset;
	FD_ZERO(&fdset);
	for (plst = ppl->pIntrList; plst; plst = plst->next) {
		fd = ppl->fd[plst->pIntrHnd->dVecNum];
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
		if (plst->pIntrHnd->bPriority == 1)
		{
#endif
			if (fd > 0) {
				FD_SET(fd, &fdset);
				maxfd = fd > maxfd ? fd : maxfd;
			}
		}
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
	}
#endif
	while (run) {
		int maxrdy;

		memcpy(&rfdset, &fdset, sizeof(fdset));
		if ((maxrdy = select(maxfd + 1, &rfdset, NULL, NULL, NULL)) <= 0) {
			switch (errno) {
			case EINTR:
				continue;
			default:
				printf("[##]GL_IntrTodo select, errno = %d, %s ...\n", errno, strerror(errno));
				continue;
			}
		}

		for (plst = ppl->pIntrList; plst && maxrdy; plst = plst->next) {
			GL_IntrHnd *pIntrHnd = plst->pIntrHnd;
			no = pIntrHnd->dVecNum;
			fd = ppl->fd[no];

			if (fd > 0 && FD_ISSET(fd, &rfdset)) {
				maxrdy--;
				if (read(fd, &cnt, sizeof(cnt)) != sizeof(cnt)) {
					;//printf("[##]GL_IntrTodo read, %s ...\n", strerror(errno));
				}
				if (cnt > 0) {
					GL_MutexLock(sIntrMutex);
					//printf("[##]cnt = %d, fd = %d------------\n", cnt, fd);
					if (pIntrHnd->bShareCnt > 1) {
						if (gsl_chain_isr(no, (cyg_addrword_t)pIntrHnd) & GL_ISR_CALL_DSR) {
							gsl_chain_dsr(no, 0, (cyg_addrword_t)pIntrHnd);
						}
					} else {
						if (gsl_isr(no, (cyg_addrword_t)pIntrHnd) & GL_ISR_CALL_DSR) {
							gsl_dsr(no, 0, (cyg_addrword_t)pIntrHnd);
						}
					}
					GL_MutexUnlock(sIntrMutex);
				}
				if (write(fd, (void *)cnt, sizeof(cnt)) != sizeof(cnt)) {
					;//printf("[##]GL_IntrTodo write, %s ...\n", strerror(errno));
				}
			}
		}
	}
}
#endif
#else
static void gl_IntrTodo(void *p)
{
	GL_Isrpool_t *ppl = (GL_Isrpool_t *)p;
	GL_IntrList *plst;
	int no, cnt;
	int fd;
	int ev_cnt = 0;
	int epfd;
	struct epoll_event *events, ev;
	for (plst = ppl->pIntrList; plst; plst = plst->next) {
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
		if (plst->pIntrHnd->bPriority == 1)
		{
#endif
			ev_cnt++;
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
		}
#endif
	}
	events = (struct epoll_event *) GL_MemAlloc(ev_cnt*sizeof(struct epoll_event));
	epfd=epoll_create(ev_cnt);	
	
	for (plst = ppl->pIntrList; plst; plst = plst->next) {
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
		if (plst->pIntrHnd->bPriority == 1)
		{
#endif
			ev.data.ptr = plst->pIntrHnd;
			ev.events = EPOLLIN | EPOLLET;
			epoll_ctl(epfd, EPOLL_CTL_ADD, ppl->fd[plst->pIntrHnd->dVecNum], &ev);				
#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
		}
#endif
	}	

	while (run) {
		int maxrdy;

		if ((maxrdy = epoll_wait(epfd, events, ev_cnt, -1)) <= 0)
		{
			if (maxrdy == 0)
				continue;
			
			switch (errno) {
			case EINTR:
				continue;
			default:
				printf("[##]GL_IntrTodo select, errno = %d, %s ...\n", errno, strerror(errno));
				continue;
			}
		}
		int i;
		for (i = 0; i < maxrdy; i++)
		{
			GL_IntrHnd *pIntrHnd = (GL_IntrHnd*)events[i].data.ptr;
			no = pIntrHnd->dVecNum;
			fd = ppl->fd[no];
			if (read(fd, &cnt, sizeof(cnt)) != sizeof(cnt)) {
				;//printf("[##]GL_IntrTodo read, %s ...\n", strerror(errno));
			}
			if (cnt > 0) {
				GL_MutexLock(sIntrMutex);
				//printf("[##]cnt = %d, fd = %d------------\n", cnt, fd);
				if (pIntrHnd->bShareCnt > 1) {
					if (gsl_chain_isr(no, (cyg_addrword_t)pIntrHnd) & GL_ISR_CALL_DSR) {
						gsl_chain_dsr(no, 0, (cyg_addrword_t)pIntrHnd);
					}
				} else {
					if (gsl_isr(no, (cyg_addrword_t)pIntrHnd) & GL_ISR_CALL_DSR) {
						gsl_dsr(no, 0, (cyg_addrword_t)pIntrHnd);
					}
				}
				GL_MutexUnlock(sIntrMutex);
			}
			if (write(fd, (void *)cnt, sizeof(cnt)) != sizeof(cnt)) {
				;//printf("[##]GL_IntrTodo write, %s ...\n", strerror(errno));
			}
		}
	}
}
#endif

#ifdef MULTI_THREAD_ISR
#define EPOLL_MAX_CONN 1
#define EPOLL_RUN_TIMEOUT -1
//#define SHOW_ISR_TIMESTAMP
static void gl_ISR_Routine(void *IRQ)
{

	int IRQnum = *(int *)IRQ;
	GL_Isrpool_t *ppl = &pool;
	GL_IntrList *plst;
	int no;
	int fd;
	int ev_cnt = EPOLL_MAX_CONN;
	int epfd;
	struct epoll_event *events, ev;
	GL_IntrHnd *pIntrHnd = NULL;

#ifdef SHOW_ISR_TIMESTAMP
	struct timeval isr_timestamp1, isr_timestamp2;
#endif

	events = (struct epoll_event *) GL_MemAlloc(ev_cnt*sizeof(struct epoll_event));
	epfd = epoll_create(EPOLL_MAX_CONN);
	
	// search IRQ num in pool
	for (plst = ppl->pIntrList; plst; plst = plst->next) {
		if(IRQnum == plst->pIntrHnd->dVecNum )
		{
			pIntrHnd = (GL_IntrHnd*)plst->pIntrHnd;
			ev.events = EPOLLIN | EPOLLET;
			ev.data.fd = ppl->fd[plst->pIntrHnd->dVecNum];
			epoll_ctl(epfd, EPOLL_CTL_ADD, ppl->fd[plst->pIntrHnd->dVecNum], &ev);
		}
	}

	no = pIntrHnd->dVecNum;
	fd = ppl->fd[no];

	while (run) {
		epoll_wait(epfd, events, ev_cnt, EPOLL_RUN_TIMEOUT);

#ifdef SHOW_ISR_TIMESTAMP
		if (pIntrHnd->dVecNum == INTR_NUM_TGEN_FLD_END)
		{
			gettimeofday(&isr_timestamp1,NULL);
		}
#endif
		if (pIntrHnd->bShareCnt > 1) {
			if (gsl_chain_isr(no, (cyg_addrword_t)pIntrHnd) & GL_ISR_CALL_DSR) {
				gsl_chain_dsr(no, 0, (cyg_addrword_t)pIntrHnd);
			}
		} else {
			if (gsl_isr(no, (cyg_addrword_t)pIntrHnd) & GL_ISR_CALL_DSR) {
				gsl_dsr(no, 0, (cyg_addrword_t)pIntrHnd);
			}
		}
#ifdef SHOW_ISR_TIMESTAMP
		if (pIntrHnd->dVecNum == INTR_NUM_TGEN_FLD_END)
		{
			gettimeofday(&isr_timestamp2,NULL);
			long unsigned int timediff = isr_timestamp2.tv_usec - isr_timestamp1.tv_usec;
			if(timediff > 400)
				printf("isr time: %06luus\n",timediff);
		}
#endif	
	}
}

#endif //MULTI_THREAD_ISR

GL_Status_t GL_IntrInit(struct gl_intr_t *intrpool)
{
	struct gl_intr_t *intr;
	GL_IntrList *list;
	GL_IntrHnd *pIntrHnd;
	UINT32 i;
	UINT32 vec_num;
	struct _intrInfo {
		UINT16 numIsr;
		UINT16 idxIsr;
		GL_IntrHnd *pIntrHnd;
	} *intrInfo;

	GL_CHECK_BLOCK_IN_INTR();

	__gl_intrpool = intrpool;

	if (GL_SUCCESS != GL_MutexCreate("GSL_INTR", &sIntrMutex)) {
		printf("%s:%d: Cannot create mutext for gsl interrupt.\n", __FUNCTION__, __LINE__);
		return GL_FAILURE;
	}

	intrInfo = (struct _intrInfo *)malloc((CYGNUM_HAL_ISR_MAX + 1) * sizeof(struct _intrInfo));
	memset(intrInfo, 0, (CYGNUM_HAL_ISR_MAX + 1) * sizeof(struct _intrInfo));

	i = 0;
	memset(&pool, 0, sizeof(GL_Isrpool_t));
	while (__gl_intrpool[i].num != ~0) {
		if ((__gl_intrpool[i].num > CYGNUM_HAL_ISR_MAX) ||
			((__gl_intrpool[i].num < CYGNUM_HAL_INTERRUPT_VECTOR_BASE) &&
			(__gl_intrpool[i].num >= 2)))  {
			gl_intr_printf("Invalid interrupt number!!! number = %u\n", __gl_intrpool[i].num);
			++i;
			continue;
		}
#if defined(WORKAROUND_UART0_RXFIFO) && defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS) && !defined(CYGDBG_HAL_DEBUG_GDB_BY_MON)
		/*
		 * Due to interrupt mode UART0 RX, don't create it when using GDB
		 */
		if (INTR_NUM_UART0 != __gl_intrpool[i].num)
#endif
		++intrInfo[__gl_intrpool[i].num].numIsr;
		++i;
	}
	pool.dEntry = i;

	list = NULL;
	for (i = 0; i < pool.dEntry; ++i) {
#if defined(WORKAROUND_UART0_RXFIFO) && defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS) && !defined(CYGDBG_HAL_DEBUG_GDB_BY_MON)
		if (INTR_NUM_UART0 == __gl_intrpool[i].num) {
			continue;
		}
#endif

		intr = &(__gl_intrpool[i]);
		vec_num = __gl_intrpool[i].num;

		if (intrInfo[vec_num].idxIsr == 0) {
			/* It is the first ISR of the interrupt */
			pIntrHnd = gl_IntrCreate(vec_num, intrInfo[vec_num].numIsr);

			/* Insert interrupt object into interrupt object list in the pool */
			if (list != NULL) {
				list->next = (GL_IntrList *)malloc(sizeof(GL_IntrList));
				list = list->next;
			} else {
				list = (GL_IntrList *)malloc(sizeof(GL_IntrList));
				pool.pIntrList = list;
			}
			list->pIntrHnd = pIntrHnd;
			list->next = NULL;
			intrInfo[vec_num].idxIsr = 1;
			intrInfo[vec_num].pIntrHnd = pIntrHnd;
		} else {
			/* Find the intr object */
			pIntrHnd = intrInfo[vec_num].pIntrHnd;
		}
		/* insert the first isr/dsr */
		*__gl_intrpool[i].hnd = (GL_IntrObjHandle_t)gl_IsrCreate(pIntrHnd, i);
	}
	gl_intr_printf("Number of isr entries = %d\n", pool.dEntry);
	free(intrInfo);

	/* Create interrupt handler task.  */
	run = 1;

#ifndef MULTI_THREAD_ISR
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr", gl_IntrTodo, &pool, -1, 131072, TRUE, &gTaskHndIntr)) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}
	#ifdef SUPPORT_GSL_RIORITY_INTERRUPT // skypine 20120824 for linux user space fld intr 
	if (GL_SUCCESS != GL_TaskCreate("GSL_Fld_Intr", gl_FldTodo, &pool, -2, 131072, TRUE, &gTaskHndIntr2)) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}
	#endif
#else //MULTI_THREAD_ISR

	int irq = INTR_NUM_IOP;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_IOP", gl_ISR_Routine, &irq, -15, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

#if defined(SUPPORT_SMARTCARD)
#if defined(SMARTCARD_USE_UART1)
 	irq = INTR_NUM_UART1;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_SMC_UART1", gl_ISR_Routine, &irq, 1, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}
#elif defined(SMARTCARD_USE_UART2)
	irq = INTR_NUM_UART2;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_SMC_UART2", gl_ISR_Routine, &irq, 1, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}
#endif
#endif
	
	irq = INTR_NUM_DEMUX2;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_DEMUX2", gl_ISR_Routine, &irq, -16, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

	irq = INTR_NUM_TGEN_FLD_START;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_TGEN_FLD_START", gl_ISR_Routine, &irq, -17, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

	irq = INTR_NUM_TGEN_FLD_END;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_TGEN_FLD_END", gl_ISR_Routine, &irq, -18, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

	irq = INTR_NUM_DEC_END;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_DEC_END", gl_ISR_Routine, &irq, -17, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

	irq = INTR_NUM_VLD_MP;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_VLD_MP", gl_ISR_Routine, &irq, -15, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

	irq = INTR_NUM_MJPEG;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_NUM_MJPEG", gl_ISR_Routine, &irq, -15, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

	irq = INTR_NUM_HDMI_TX;
	if (GL_SUCCESS != GL_TaskCreate("GSL_Intr_HDMI_TX", gl_ISR_Routine, &irq, 1, 131072, TRUE, &gTaskHndISR[gTaskHndISRCnt++])) {
		printf("%s:%d: Create intr task error ...\n", __FUNCTION__, __LINE__);
	}

#endif //MULTI_THREAD_ISR



	return GL_SUCCESS;
}

void GL_IntrUninit(struct gl_intr_t *intrpool)
{
	int i;

	/* Disable and detach every ISR object.  */
	for (i = 0; i < pool.dEntry; ++i) {
		GL_IsrHnd *pIsrHnd;

		pIsrHnd = *__gl_intrpool[i].hnd;
		GL_IntrDisable(pIsrHnd);
		GL_IntrDetach(pIsrHnd);
	}

	/* Let intr thread stop smoothly.  */
	run = 0;

	/* Destroy every ISR object.  */
	for (i = 0; i < pool.dEntry; ++i) {
		gl_IsrDelete(*__gl_intrpool[i].hnd);
	}

	/* Delete INTR object.  */
	while (pool.pIntrList) {
		GL_IntrList *tmp = pool.pIntrList;

		pool.pIntrList = pool.pIntrList->next;
		gl_IntrDelete(tmp->pIntrHnd);
		free(tmp);
	}

	/* Force to kill intr thread.  */
#ifndef MULTI_THREAD_ISR
	GL_TaskDestroyOne(gTaskHndIntr);
#else
	for(i=0;i<gTaskHndISRCnt;i++)
	GL_TaskDestroyOne(&gTaskHndIntr[i]);
#endif
	if (GL_SUCCESS != GL_MutexDelete(sIntrMutex)) {
		gl_intr_printf("cannot delete mutex\n");
	}

}

GL_Status_t GL_IntrEnable(GL_IntrObjHandle_t h)
{
	UINT32 i;
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *pIsrHnd = (GL_IsrHnd *)h;

	if (pIsrHnd == NULL) {
		return GL_ERR_INVALID_PARAM;
	}

	pIntrHnd = pIsrHnd->pIntrHnd;
    printf("%s int handle = %p\n", __FUNCTION__, pIntrHnd);

	if ((pIntrHnd == NULL) || (pIntrHnd->dSignature != GL_INTR_SIGNATURE)) {
		return GL_ERR_INVALID_PARAM;
	}

	GL_MutexLock(sIntrMutex);
	if ((1 == pIntrHnd->bShareCnt) || (1 == pIntrHnd->bEmpty)) {
		i = 0;
		/*
		 * For those interrupts has only one isr/dsr, they'll be enabled/disabled
		 * by calling enabling/disalbing functions.
		 */
		if (pIntrHnd->pIsrHnd[0] == pIsrHnd) {
			pIntrHnd->pIsrHnd[0]->fCallIsr = TRUE;
			gl_IntrUnmask(pIntrHnd->dVecNum);
		} else {
			i = pIntrHnd->bEmpty;
		}
	} else {
		for (i = 0; i < pIntrHnd->bEmpty; ++i) {
			if (pIntrHnd->pIsrHnd[i] == pIsrHnd) {
				pIntrHnd->pIsrHnd[i]->fCallIsr = TRUE;
				break;
			}
		}
	}

	if (i == pIntrHnd->bEmpty) {	/* not found  */
		GL_MutexUnlock(sIntrMutex);
		return GL_ERR_INVALID_OP;
	}
	GL_MutexUnlock(sIntrMutex);
	return GL_SUCCESS;
}

GL_Status_t GL_IntrDisable(GL_IntrObjHandle_t h)
{
	UINT32 i;
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *pIsrHnd = (GL_IsrHnd *)h;

	if (pIsrHnd == NULL) {
		return GL_ERR_INVALID_PARAM;
	}

	pIntrHnd = pIsrHnd->pIntrHnd;
	if ((pIntrHnd == NULL) || (pIntrHnd->dSignature != GL_INTR_SIGNATURE)) {
		return GL_ERR_INVALID_PARAM;
	}

	GL_MutexLock(sIntrMutex);
	if ((1 == pIntrHnd->bShareCnt) || (1 == pIntrHnd->bEmpty)) {
		i = 0;
		/*
		 * For those interrupts has only one isr/dsr, they'll be enabled/disabled
		 * by calling enabling/disalbing functions.
		 */
		if (pIntrHnd->pIsrHnd[0] == pIsrHnd) {
			pIntrHnd->pIsrHnd[0]->fCallIsr = FALSE;
			gl_IntrMask(pIntrHnd->dVecNum);
		} else {
			i = pIntrHnd->bEmpty;
		}
	} else {
		for (i = 0; i < pIntrHnd->bEmpty; i++) {
			if (pIntrHnd->pIsrHnd[i] == pIsrHnd) {
				pIntrHnd->pIsrHnd[i]->fCallIsr = FALSE;
				break;
			}
		}
	}

	if (i == pIntrHnd->bEmpty) {	/* not found  */
		GL_MutexUnlock(sIntrMutex);
		return GL_ERR_INVALID_OP;
	}
	GL_MutexUnlock(sIntrMutex);

	return GL_SUCCESS;
}

GL_Status_t GL_IntrAttach(GL_IntrObjHandle_t h)
{
	UINT32 i;
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *pIsrHnd = (GL_IsrHnd *)h;

	/* check parameters */
	if (pIsrHnd == NULL) {
		return GL_ERR_INVALID_PARAM;
	}
	pIntrHnd = pIsrHnd->pIntrHnd;

	if ((pIntrHnd == NULL) || (pIntrHnd->dSignature != GL_INTR_SIGNATURE)) {
		return GL_ERR_INVALID_PARAM;
	}

	GL_MutexLock(sIntrMutex);

	/* no more space */
	if (pIntrHnd->bEmpty == pIntrHnd->bShareCnt) {
		GL_MutexUnlock(sIntrMutex);
		return GL_FAILURE;
	}
	/* check duplication */
	for (i = 0; i < pIntrHnd->bEmpty; i++) {
		if (pIntrHnd->pIsrHnd[i] == pIsrHnd) {
			GL_MutexUnlock(sIntrMutex);
			gl_intr_printf("Attach isr duplicately! name = %s\n", pIsrHnd->szName);
			return GL_ERR_INVALID_OP;
		}
	}

	/* fill the structure */
	pIsrHnd->fCallIsr = FALSE;	// disable the isr now, waiting for enable
	pIsrHnd->fCallDsr = FALSE;
	i = pIntrHnd->bEmpty;
	pIntrHnd->pIsrHnd[pIntrHnd->bEmpty] = pIsrHnd;
	(pIntrHnd->bEmpty)++;

	GL_MutexUnlock(sIntrMutex);

	return GL_SUCCESS;
}

GL_Status_t GL_IntrDetach(GL_IntrObjHandle_t h)
{
	UINT32 i, j;
	GL_IntrHnd *pIntrHnd;
	GL_IsrHnd *pIsrHnd = (GL_IsrHnd *)h;

	if (pIsrHnd == NULL) {
		return GL_ERR_INVALID_PARAM;
	}

	pIntrHnd = pIsrHnd->pIntrHnd;
	if ((pIntrHnd == NULL) || (pIntrHnd->dSignature != GL_INTR_SIGNATURE)) {
		return GL_ERR_INVALID_PARAM;
	}

	GL_MutexLock(sIntrMutex);
	for (i = 0; i < pIntrHnd->bEmpty; i++) {
		if (pIntrHnd->pIsrHnd[i] == pIsrHnd) {
			j = pIntrHnd->bEmpty - 1;
			pIntrHnd->pIsrHnd[i] = pIntrHnd->pIsrHnd[j];
			pIntrHnd->pIsrHnd[i]->fCallIsr = FALSE;
			pIntrHnd->pIsrHnd[i]->fCallDsr = FALSE;
			pIntrHnd->pIsrHnd[j] = NULL;
			break;
		}
	}
	if (i == pIntrHnd->bEmpty) {	/* not found  */
		GL_MutexUnlock(sIntrMutex);
		return GL_ERR_INVALID_OP;
	}
	pIntrHnd->bEmpty--;
	if (0 == pIntrHnd->bEmpty) {
		/* from 1 to 0, mask the interrupt */
		gl_IntrMask(pIntrHnd->dVecNum);
	}
	GL_MutexUnlock(sIntrMutex);

	return GL_SUCCESS;
}

void GL_IntrAck(UINT32 dVecNum)
{
	INTR_ACK(dVecNum);
}

GL_Status_t GL_IsrStatus(GL_IntrObjHandle_t h, UINT32 detail)
{
	UINT32 i;
	UINT32 isAttach = 0;
	GL_IntrHnd *pIntrHnd;

	/* Use to avoid disabling intr too long.  */
	GL_IsrHnd tmp;
	GL_IsrHnd *pIsrHnd;
	UINT32 dVecNum;

	if (h == NULL) {
		return GL_FAILURE;
	}
	GL_MutexLock(sIntrMutex);
	pIntrHnd = ((GL_IsrHnd *)h)->pIntrHnd;
	for (i = 0; i < pIntrHnd->bEmpty; ++i) {
		if (h == pIntrHnd->pIsrHnd[i]) {
			isAttach = 1;
			break;
		}
	}
	dVecNum = pIntrHnd->dVecNum;
	memcpy((void *)&tmp, h, sizeof(tmp));
	GL_MutexUnlock(sIntrMutex);
	pIsrHnd = (GL_IsrHnd *)&tmp;

	printf("%4u %16s 0x%08X 0x%08X 0x%08X %3u %3u %10u %10u %10u %10u %10u %10u %10u %10u %10u %10u\n",
				dVecNum,
				pIsrHnd->szName,
				(UINT32)pIsrHnd,
				(UINT32)pIsrHnd->isr,
				(UINT32)pIsrHnd->dsr,
				isAttach,
				pIsrHnd->fCallIsr,
				pIsrHnd->dTimeMax[0][2],
				pIsrHnd->dTimeMax[0][1],
				pIsrHnd->dTimeMax[0][0],
				pIsrHnd->dTimeCnt[0] ? ((UINT32)(pIsrHnd->dTimeSum[0] / pIsrHnd->dTimeCnt[0])) : (0),
				pIsrHnd->dTimeCnt[0],
				pIsrHnd->dTimeMax[1][2],
				pIsrHnd->dTimeMax[1][1],
				pIsrHnd->dTimeMax[1][0],
				pIsrHnd->dTimeCnt[1] ? ((UINT32)(pIsrHnd->dTimeSum[1] / pIsrHnd->dTimeCnt[1])) : (0),
				pIsrHnd->dTimeCnt[1]);
	return GL_SUCCESS;
}

static void print_isr_status_title(void)
{
	printf("%4s %16s %10s %10s %10s %3s %3s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s\n",
				"Num", "Name", "Handle", "ISR", "DSR", "At.", "En.",
				"MaxClkISR1", "MaxClkISR2", "MaxClkISR3", "AvgClkISR", "CntISR",
				"MaxClkDSR1", "MaxClkDSR2", "MaxClkDSR3", "AvgClkDSR", "CntDSR");
}

#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_USAGE
static void print_intr_status_title(void)
{
	printf("%4s %10s %10s %10s %10s %10s %10s %10s %10s %10s %10s\n",
				"Num", "MaxClkISR1", "MaxClkISR2", "MaxClkISR3", "AvgClkISR", "CntISR",
				"MaxClkDSR1", "MaxClkDSR2", "MaxClkDSR3", "AvgClkDSR", "CntDSR");
}

static void print_intr_status_content(UINT32 i)
{
	/*
	 * Don't care about that the results may change while printing. If it happens,
	 * just do another print to get accurate results.
	 */
	printf("%4u %10u %10u %10u %10u %10u %10u %10u %10u %10u %10u\n",
	i,
	cyg_interrupt_isr_record[i][2],
	cyg_interrupt_isr_record[i][1],
	cyg_interrupt_isr_record[i][0],
	cyg_interrupt_isr_cnt[i] ? ((UINT32)(cyg_interrupt_isr_clk[i] / cyg_interrupt_isr_cnt[i])) : (0),
	cyg_interrupt_isr_cnt[i],
	cyg_interrupt_dsr_record[i][2],
	cyg_interrupt_dsr_record[i][1],
	cyg_interrupt_dsr_record[i][0],
	cyg_interrupt_dsr_cnt[i] ? ((UINT32)(cyg_interrupt_dsr_clk[i] / cyg_interrupt_dsr_cnt[i])) : (0),
	cyg_interrupt_dsr_cnt[i]);
}
#endif

GL_Status_t GL_IntrStatus(UINT32 dIntNumber, UINT32 detail)
{
	UINT32 i = 0;
	UINT32 isFind = 0;

	print_isr_status_title();
	while (__gl_intrpool[i].num != ~0) {
		if (__gl_intrpool[i].num == dIntNumber) {
			GL_IsrStatus(*__gl_intrpool[i].hnd, detail);
			isFind = 1;
		}
		++i;
	}
	if (isFind == 1) {
		return GL_SUCCESS;
	}
	return GL_FAILURE;
}

void GL_IntrUsageReset(UINT32 vec)
{
	UINT32 i = 0;

	GL_MutexLock(sIntrMutex);

	/* Reset one interrupt */
	if (vec < CYGNUM_HAL_ISR_COUNT) {
		while (__gl_intrpool[i].num != ~0) {
			if (__gl_intrpool[i].num == vec) {
				GL_IsrHnd *pIsrHnd = (GL_IsrHnd *)(*__gl_intrpool[i].hnd);
				pIsrHnd->dTimeMax[0][0] = 0;
				pIsrHnd->dTimeMax[0][1] = 0;
				pIsrHnd->dTimeMax[0][2] = 0;
				pIsrHnd->dTimeMax[1][0] = 0;
				pIsrHnd->dTimeMax[1][1] = 0;
				pIsrHnd->dTimeMax[1][2] = 0;
			}
			++i;
		}
#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_USAGE
		cyg_interrupt_isr_record[vec][0] = 0;
		cyg_interrupt_isr_record[vec][1] = 0;
		cyg_interrupt_isr_record[vec][2] = 0;
		cyg_interrupt_dsr_record[vec][0] = 0;
		cyg_interrupt_dsr_record[vec][1] = 0;
		cyg_interrupt_dsr_record[vec][2] = 0;
		cyg_interrupt_isr_clk[vec] = 0;
		cyg_interrupt_dsr_clk[vec] = 0;
		cyg_interrupt_isr_cnt[vec] = 0;
		cyg_interrupt_dsr_cnt[vec] = 0;
#endif

		GL_MutexUnlock(sIntrMutex);
		return;
	}
	/* Reset all interrupts */
	GL_MutexLock(sIntrMutex);
	while (__gl_intrpool[i].num != ~0) {
		GL_IsrHnd *pIsrHnd = (GL_IsrHnd *)(*__gl_intrpool[i].hnd);
		pIsrHnd->dTimeMax[0][0] = 0;
		pIsrHnd->dTimeMax[0][1] = 0;
		pIsrHnd->dTimeMax[0][2] = 0;
		pIsrHnd->dTimeMax[1][0] = 0;
		pIsrHnd->dTimeMax[1][1] = 0;
		pIsrHnd->dTimeMax[1][2] = 0;
		++i;
	}
#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_USAGE
	memset(cyg_interrupt_isr_record, 0, sizeof(cyg_interrupt_isr_record));
	memset(cyg_interrupt_dsr_record, 0, sizeof(cyg_interrupt_dsr_record));
	memset(cyg_interrupt_isr_clk, 0, sizeof(cyg_interrupt_isr_clk));
	memset(cyg_interrupt_isr_cnt, 0, sizeof(cyg_interrupt_isr_cnt));
	memset(cyg_interrupt_dsr_clk, 0, sizeof(cyg_interrupt_dsr_clk));
	memset(cyg_interrupt_dsr_cnt, 0, sizeof(cyg_interrupt_dsr_cnt));
#endif
	GL_MutexUnlock(sIntrMutex);
}

void GL_IntrUsageResult(UINT32 vec)
{
#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_USAGE
	UINT32 i;

	if ((vec - CYGNUM_HAL_INTERRUPT_VECTOR_BASE) < CYGNUM_HAL_ISR_COUNT) {
		print_intr_status_title();
		print_intr_status_content(vec);
		return;
	}
	/*
	 * Print interrupt time usage. The format may need to chage
	 * if number of interrupt is increased/decreased.
	 */
	print_intr_status_title();
	for (i = CYGNUM_HAL_INTERRUPT_VECTOR_BASE; i < CYGNUM_HAL_ISR_COUNT; ++i) {
		if (0 == cyg_interrupt_isr_cnt[i]) {
			/* If they are both zero, skip it. */
			continue;
		}
		print_intr_status_content(i);
	}
#endif // #ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_USAGE
}

void GL_IntrDump(UINT32 detail)
{
	UINT32 i = 0;

	print_isr_status_title();
	while (__gl_intrpool[i].num != ~0) {
		GL_IsrStatus(*__gl_intrpool[i].hnd, detail);
		++i;
	}
	printf("\n");
	GL_IntrUsageResult(0);	// 0 means all interrupts
}

UINT8 GL_IntrIsrLevel(void)
{
#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_ISR_CONTEXT
	return cyg_interrupt_isr_level();
#else
	printf("%s isn't supported!\n", __FUNCTION__);
	return 0;
#endif
}

UINT8 GL_IntrDsrLevel(void)
{
#ifdef CYGIMP_KERNEL_INTERRUPTS_TRACK_DSR_CONTEXT
	return cyg_interrupt_dsr_level();
#else
	printf("%s isn't supported!\n", __FUNCTION__);
	return 0;
#endif
}
