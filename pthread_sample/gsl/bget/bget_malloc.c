/*******************************************************************************
* author:		junxian@sunnorth.com.cn
* create date:	2010-11-16
* chang log:
*	V0.1	2010-11-16	fisrt version
*******************************************************************************/
#include <gsl.h>
#include "c++/bget-api.h"
#include "../gl_common.h"

/*******************************************************************************
*	Constant
*******************************************************************************/
#define  BGET_THREAD_PROTECT		1
#define  BGET_INIT_CHECK			1

#define  BGET_INIT_MAGIC_NUM 		0x13572468
#define  BGET_POOL_MAX_SIZE		    0x7fffffff

#define  POOL_A 			0
#define  POOL_B 			1
#define  POOL_MAX 		    1

#define BGET_ADDR_TYPE_CACHE		0
#define BGET_ADDR_TYPE_UNCACHE	    1
#define BGET_ADDR_TYPE_PHYSICAL	    2

/*******************************************************************************
*	type define
*******************************************************************************/
typedef struct bget_mem_pool_s{
	char *startaddr;
	bget_bufsize_t size;
	bget_handle_t	 handle;

#if BGET_THREAD_PROTECT
	pthread_mutex_t lock;
#endif

}bget_mem_pool_t;

typedef struct bget_mem_s{
	int initflag;
	bget_mem_pool_t pool[POOL_MAX];
}bget_mem_t;

/*******************************************************************************
*	Global Valiable
*******************************************************************************/
#if BGET_INIT_CHECK
static bget_mem_t bget_mem={
	.initflag = 0
};
#else
static bget_mem_t bget_mem;
#endif

#if GL_BGET_TEST_ON_PC
struct lambo86_hw_info lambo_hw_info = {
	.heapA_size = 32*1024,
	.heapB_size = 60*1024,
};
#else
#include <gsl/lambo86_hw.h>
#endif

/*******************************************************************************
*	extern
*******************************************************************************/


/*******************************************************************************
*	functions
*******************************************************************************/
//#define bget_log(args...) printf(args)
#define bget_log(args...)


#if BGET_THREAD_PROTECT
static int thread_lock_init(void)
{
	int i, ret;

	for(i=0; i<POOL_MAX; i++){
		ret =  pthread_mutex_init(&(bget_mem.pool[i].lock), NULL);
		if(0 != ret){
			bget_log("[%s]line:%d, pthread_mutex_init error!", __FUNCTION__, __LINE__);
			break;
		}
	}

	return ret;
}

static int thread_lock_deinit(void)
{
	int i = POOL_MAX;
	int ret = 0;

	while(i--){
		ret |= pthread_mutex_destroy(&(bget_mem.pool[i].lock));
	}

	return ret;
}

static void thread_lock(int poolIdx)
{
	pthread_mutex_lock(&(bget_mem.pool[poolIdx].lock));
}

static void thread_unlock(int poolIdx)
{
	pthread_mutex_unlock(&(bget_mem.pool[poolIdx].lock));
}

#define BGET_THREAD_PROTECT_INIT()			do{if(0!=thread_lock_init()) return -1;}while(0)
#define BGET_THREAD_PROTECT_DEINIT()			do{if(0!=thread_lock_deinit()) return -1;}while(0)
#define BGET_THREAD_PROTECT_BEGIN(poolIdx)	thread_lock(poolIdx)
#define BGET_THREAD_PROTECT_END(poolIdx)		thread_unlock(poolIdx)
#else
#define BGET_THREAD_PROTECT_INIT()
#define BGET_THREAD_PROTECT_DEINIT()
#define BGET_THREAD_PROTECT_BEGIN(poolIdx)
#define BGET_THREAD_PROTECT_END(poolIdx)
#endif

#if BGET_INIT_CHECK
#define	BGET_INIT_CHECK_ERR_RETURN_NULL() 			\
	do{													\
		if(BGET_INIT_MAGIC_NUM!=bget_mem.initflag){	\
			bget_log("[%s]line:%d, have not init!",__FUNCTION__,__LINE__); \
			return NULL;									\
		}												\
	}while(0)

#define	BGET_INIT_CHECK_ERR_RETURN() 					\
	do{													\
		if(BGET_INIT_MAGIC_NUM!=bget_mem.initflag){	\
			bget_log("[%s]line:%d, have not init!",__FUNCTION__,__LINE__); \
			return;										\
		}												\
	}while(0)

#else

#define	BGET_INIT_CHECK_ERR_RETURN_NULL()	do{}while(0)
#define	BGET_INIT_CHECK_ERR_RETURN()			do{}while(0)
#endif

#if GL_USE_BGET_MALLOC
#if GL_BGET_TEST_ON_PC
void BGET_INIT_ONCE(void)
{
	if(BGET_INIT_MAGIC_NUM!=bget_mem.initflag){
		bget_meminit();
	}
}
#endif
#endif

/*******************************************************************************
* functions name:
* description:
* params:
* return:
*******************************************************************************/
static int bget_poolwhich(void* buffer)
{
	int i;

	if(NULL == buffer){
		return POOL_MAX;
	}

	for(i=0; i<POOL_MAX; i++){
		if( (bget_mem.pool[i].startaddr < (char*)buffer) &&
		    ((char*)buffer < bget_mem.pool[i].startaddr + bget_mem.pool[i].size) )
		{
			break;
		}
	}

	return i;
}

/*******************************************************************************
* functions name:
* description:
* params:
* return:
*******************************************************************************/
static void* bget_poolmalloc(int poolIdx, bget_bufsize_t size)
{
	void *retbuf;

	BGET_THREAD_PROTECT_BEGIN(poolIdx);
	retbuf = bget_get(bget_mem.pool[poolIdx].handle, size);
	BGET_THREAD_PROTECT_END(poolIdx);

	return retbuf;
}

/*******************************************************************************
* functions name:
* description:
* params:
* return:
*******************************************************************************/
static void* bget_poolrealloc(int poolIdx, void *buffer, bget_bufsize_t newsize)
{
	void *retbuf;

	BGET_THREAD_PROTECT_BEGIN(poolIdx);
	retbuf = bget_getr(bget_mem.pool[poolIdx].handle, buffer, newsize);
	BGET_THREAD_PROTECT_END(poolIdx);

	bget_log("### realloc return NULL, but not free the old buf!###\r\n");

	return retbuf;
}

/*******************************************************************************
* functions name:
* description:
* params:
* return:
*******************************************************************************/
static void bget_poolfree(int poolIdx, void *buffer)
{
	BGET_THREAD_PROTECT_BEGIN(poolIdx);
	bget_brel(bget_mem.pool[poolIdx].handle, buffer);
	BGET_THREAD_PROTECT_END(poolIdx);
}

/*******************************************************************************
* functions name:
* description:
* params:
* return:
*******************************************************************************/
static void bget_poolinfo(int poolIdx, int dumpalloc, int dumfree)
{
	BGET_THREAD_PROTECT_BEGIN(poolIdx);
	bget_log("The Node list of alloc/free is as following:\r\n");
	bget_bpoold(bget_mem.pool[poolIdx].handle, bget_mem.pool[poolIdx].startaddr, dumpalloc, dumfree);
	BGET_THREAD_PROTECT_END(poolIdx);
}

/*******************************************************************************
* functions name:
* description:
* params:
* return:
*******************************************************************************/
static void bget_poolstat(int poolIdx)
{
	bget_bufsize_t curalloc, totfree, maxfree, nget, nrel;

	BGET_THREAD_PROTECT_BEGIN(poolIdx);
	bget_bstats(bget_mem.pool[poolIdx].handle, &curalloc, &totfree, &maxfree, &nget, &nrel);
	bget_log("\r\n%8s %8s %8s %8s %8s %8s %8s %8s %8s\r\n" , "PoolIndx", "saddr", "eaddr", "totsize", "curalloc", "totfree", "maxfree", "numalloc", "numfree");
	bget_log("%8d %8x %8x %8ld %8ld %8ld %8ld %8ld %8ld\r\n\r\n",
				poolIdx, \
				(unsigned int)bget_mem.pool[poolIdx].startaddr, \
				(unsigned int)(bget_mem.pool[poolIdx].startaddr+bget_mem.pool[poolIdx].size), \
				bget_mem.pool[poolIdx].size, \
				curalloc, totfree, maxfree, nget, nrel);

	BGET_THREAD_PROTECT_END(poolIdx);

}

/*******************************************************************************
* functions name:	bget_meminit()
*
* description:		init the memory pools, see POOL_MAX. if BGET_THREAD_PROTECT==1, init the pthread_mutex
*
* params:		void
*
* return:			0:	success
*				-1:	init the pthread_mutex fail
*				-2: 	call bget_create_pool for pool create fail
*				-3: 	init again
*******************************************************************************/
int bget_meminit(void)
{
	int i;

	#if BGET_INIT_CHECK
	if(BGET_INIT_MAGIC_NUM==bget_mem.initflag){
		bget_log("[%s]line:%d, bget_meminit have been init!\r\n", __FUNCTION__, __LINE__);
		return -3;
	}
	#endif

	BGET_THREAD_PROTECT_INIT();

	#if GL_BGET_TEST_ON_PC
	bget_mem.pool[POOL_A].startaddr = (char*)malloc(GL_POOLA_SIZE);
	bget_mem.pool[POOL_A].size = GL_POOLA_SIZE;
	bget_mem.pool[POOL_B].startaddr =(char*)malloc(GL_POOLB_SIZE);
	bget_mem.pool[POOL_B].size = GL_POOLB_SIZE;
	lambo_hw_info.heapA_cache = lambo_hw_info.heapA_uncache = bget_mem.pool[POOL_A].startaddr;
	lambo_hw_info.heapB_cache = lambo_hw_info.heapB_uncache = bget_mem.pool[POOL_B].startaddr;
	#else
	bget_mem.pool[POOL_A].startaddr = GL_POOLA_CACHE_ADDR;
	bget_mem.pool[POOL_A].size = GL_POOLA_SIZE;
	bget_mem.pool[POOL_B].startaddr = GL_POOLB_CACHE_ADDR;
	bget_mem.pool[POOL_B].size = GL_POOLB_SIZE;
	#endif

	for(i=0; i<POOL_MAX; i++){
		bget_mem.pool[i].handle = bget_create_pool(bget_mem.pool[i].startaddr, bget_mem.pool[i].size);
		if(BGET_FAIL == bget_mem.pool[i].handle){
			return -2;
		}
	}

	#if BGET_INIT_CHECK
	bget_mem.initflag = BGET_INIT_MAGIC_NUM;
	#endif

	return 0;
}

/*******************************************************************************
* functions name:	bget_memdeinit()  //not need to be called
*
* description:		if BGET_THREAD_PROTECT==1, destroyed the pthread_mutex. for memory pool need exist
*				all the time when the system is running, so this function is not need to be called
*
* params:		void
*
* return:			void
*******************************************************************************/
int bget_memdeinit(void)
{
	BGET_THREAD_PROTECT_DEINIT();

#if BGET_TEST_ON_PC
	free(bget_mem.pool[0].startaddr);
	free(bget_mem.pool[1].startaddr);
#else
	int i;

	for (i = 0; i < POOL_MAX; i++) {
		bget_delete_pool(bget_mem.pool[i].handle);
	}
#endif

	return 0;
}

/*******************************************************************************
* functions name:	bget_mallocA()
*
* description:		malloc memory from pool[POOL_A]
*
* params:		size: the size to malloc, if zero, will cause a assert error and the system hang
*
* return:			if success, it will return the pointer to the malloced memory
*				if fail, it will return NULL
*******************************************************************************/
void* bget_mallocA(bget_bufsize_t size)
{
	BGET_INIT_CHECK_ERR_RETURN_NULL();

	return bget_poolmalloc(POOL_A, size);
}

/*******************************************************************************
* functions name:	bget_mallocB()
*
* description:		malloc memory from pool[POOL_B]
*
* params:		size: the size to malloc, if zero, will cause a assert error and the system hang
*
* return:			if success, it will return the pointer to the malloced memory
*				if fail, it will return NULL
*******************************************************************************/
void* bget_mallocB(bget_bufsize_t size)
{
	BGET_INIT_CHECK_ERR_RETURN_NULL();

	return bget_poolmalloc(POOL_B, size);
}

/*******************************************************************************
* functions name:	bget_malloc()
*
* description:		malloc memory from any pool form pool[0] to  pool[POOL_MAX]
*
* params:		size: the size to malloc, if zero, will cause a assert error and the system hang
*
* return:			if success, it will return the pointer to the malloced memory
*				if fail, it will return NULL
*******************************************************************************/
void* bget_malloc(bget_bufsize_t size)
{
	void* retbuf = NULL;
	int i;

	BGET_INIT_CHECK_ERR_RETURN_NULL();

	for(i=0; i<POOL_MAX; i++){
		if(NULL != (retbuf = bget_poolmalloc(i, size))){
			break;
		}
	}

	return retbuf;
}


/*******************************************************************************
* functions name:	bget_reallocA()
*
* description:		realloc memory from pool[POOL_A]
*
* params:		buffer: the old buffer for realloc, which will not be free if realloc fail
*				size: the size to malloc, if zero, will cause a assert error and the system hang
*
* return:			if success, it will return the pointer to the new malloced memory
*				if fail, it will return NULL
*******************************************************************************/
void* bget_reallocA(void *buffer, bget_bufsize_t newsize)
{
	BGET_INIT_CHECK_ERR_RETURN_NULL();

	return bget_poolrealloc(POOL_A, buffer, newsize);
}

/*******************************************************************************
* functions name:	bget_reallocB()
*
* description:		realloc memory from pool[POOL_B]
*
* params:		buffer: the old buffer for realloc, which will not be free if realloc fail
*				size: the size to malloc, if zero, will cause a assert error and the system hang
*
* return:			if success, it will return the pointer to the new malloced memory
*				if fail, it will return NULL
*******************************************************************************/
void* bget_reallocB(void *buffer, bget_bufsize_t newsize)
{
	BGET_INIT_CHECK_ERR_RETURN_NULL();

	return bget_poolrealloc(POOL_B, buffer, newsize);
}

/*******************************************************************************
* functions name:	bget_realloc()
*
* description:		realloc memory from the pool which the buffer malloced/realloced.
*
* params:		buffer: the old buffer for realloc, which will not be free if realloc fail
*				size: the size to malloc, if zero, will cause a assert error and the system hang
*
* return:			if success, it will return the pointer to the new malloced memory
*				if fail, it will return NULL
*******************************************************************************/
void* bget_realloc(void *buffer, bget_bufsize_t newsize)
{
	int i ;

	BGET_INIT_CHECK_ERR_RETURN_NULL();

	i = bget_poolwhich(buffer);
	if(i>= POOL_MAX){
		bget_log("[%s]line:%d, pointer error!\r\n", __FUNCTION__, __LINE__);
		return NULL;
	}

	#if 1
	/*realloc in the same pool*/
	return bget_poolrealloc(i, buffer, newsize);
	#else
	/*free and then alloc in any pool*/
	bget_poolfree(i, buffer);
	return bget_malloc(newsize);
	#endif
}

/*******************************************************************************
* functions name:	bget_free()
*
* description:		free the memory to the pool which the buffer malloced/realloced.
*
* params:		buffer: the malloced or realloced memory address. if it has been freed, will cause
*				a assert error and the system hang, return normal when NULL.
*
* return:			void
*******************************************************************************/
void bget_free(void *buffer)
{
	int i;

	BGET_INIT_CHECK_ERR_RETURN();

	i = bget_poolwhich(buffer);
	if(i>= POOL_MAX){
		bget_log("[%s]line:%d, pointer error!\r\n", __FUNCTION__, __LINE__);
		return;
	}

	bget_log("[%s]:pool:%d, pointer:0x%x!\r\n", __FUNCTION__, i, (unsigned int)buffer);
	bget_poolfree(i, buffer);
}

/*******************************************************************************
* functions name:	bget_meminfo()
*
* description:		show the detail infomation for all the pools
*
* params:		void
*
* return:			void
*******************************************************************************/
void bget_meminfo(void)
{
	int i;

	BGET_INIT_CHECK_ERR_RETURN();

	bget_log("---------------------Bget mem pool information----------------------------------\r\n");
	bget_log("--------------------------------------------------------------------------------\r\n");
	for(i=0; i<POOL_MAX; i++){
		bget_poolstat(i);
		bget_poolinfo(i, 0, 0);
		bget_log("--------------------------------------------------------------------------------\r\n");
	}
}
