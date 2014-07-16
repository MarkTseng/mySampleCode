#ifndef __GL_COMMON_H__
#define __GL_COMMON_H__

#include "gsl/gl_types.h"
#include "gsl/lambo86_hw.h"


#if (GL_BLOCK_CHECK_METHOD == 1)
#define GL_CHECK_BLOCK_IN_INTR()													\
	do {																			\
		if (GL_IntrIsrLevel() || GL_IntrDsrLevel()) {								\
			diag_printf("Illegal: ISR/DSR code at 0x%08X calls a GSL function "		\
						"which will lock the system, please revise it.\n",			\
			(GL_UINT32)__builtin_return_address(0) - 8);							\
		}																			\
	} while (0)
#define GL_CHECK_BLOCK_IN_ISR()														\
	do {																			\
		if (GL_IntrIsrLevel()) {													\
			diag_printf("Illegal: ISR code at 0x%08X calls a GSL function "			\
						"which will lock the system, please revise it.\n",			\
			(GL_UINT32)__builtin_return_address(0) - 8);							\
		}																			\
	} while (0)
#elif (GL_BLOCK_CHECK_METHOD == 2)
#define GL_CHECK_BLOCK_IN_INTR()													\
	do {																			\
		if (GL_IntrIsrLevel() || GL_IntrDsrLevel()) {								\
			diag_printf("Illegal: ISR/DSR code at 0x%08X calls a GSL function "		\
						"which will lock the system, please revise it.\n",			\
			(GL_UINT32)__builtin_return_address(0) - 8);							\
			buffered_uart_flush();													\
			while (1)																\
				;																	\
		}																			\
	} while (0)
#define GL_CHECK_BLOCK_IN_ISR()														\
	do {																			\
		if (GL_IntrIsrLevel()) {													\
			diag_printf("Illegal: ISR code at 0x%08X calls a GSL function "			\
						"which will lock the system, please revise it.\n",			\
			(GL_UINT32)__builtin_return_address(0) - 8);							\
			buffered_uart_flush();													\
			while (1)																\
				;																	\
		}																			\
	} while (0)
#else
#define GL_CHECK_BLOCK_IN_INTR()	do {} while (0)
#define GL_CHECK_BLOCK_IN_ISR()		do {} while (0)
#endif

#if (GL_TRACK_SYNC_MECHANISM == 1)
#define GL_DEL_BLOCK_INFO(info)									\
	do {														\
		GL_TaskDelBlockInfo((UINT32)(info));	\
	} while (0)

#define GL_SET_BLOCK_INFO(type, info)									\
	do {																\
		GL_TaskAddBlockInfo((UINT8)(type), (UINT32)(info));	\
	} while (0)

#else
#define GL_DEL_BLOCK_INFO(info)			do {} while (0)
#define GL_SET_BLOCK_INFO(type, info)	do {} while (0)
#endif

#define GL_PUTC(c) xxxerror	//while ((HWREG_R(uart0_lsr) & 0x01) != 1); HWREG_W(uart0_data, (c));
#define GL_GET_FRACTION(x, y, digit) xxxerror	//((((x) % (y)) * (digit)) / (y))
#define GL_RETURN_ADDR() xxxerror 	//(unsigned int)__builtin_return_address(0)

//#define GL_SchedulerLock()
//#define GL_SchedulerUnLock()

#define GL_POOLA_CACHE_ADDR	lambo_hw_info.heapA.cache
#define GL_POOLB_CACHE_ADDR	lambo_hw_info.heapB.cache
#define GL_POOLA_SIZE		lambo_hw_info.heapA.size
#define GL_POOLB_SIZE		lambo_hw_info.heapB.size


#define GL_IS_IN_INTERRUPT() 	(0)

#define GL_Trace(fmt, ...)	printf(fmt, ##__VA_ARGS__)

#if (GL_DEBUG_TRACE && GL_DEBUG_TRACE_ERROR)
#define GL_TraceError 			GL_Trace
#else
#define GL_TraceError
#endif

#if (GL_DEBUG_TRACE && GL_DEBUG_TRACE_WARNING)
#define GL_TraceWarning			GL_Trace
#else
#define GL_TraceWarning
#endif

#if (GL_DEBUG_TRACE && GL_DEBUG_TRACE_INFO)
#define GL_TraceInfo			GL_Trace
#else
#define GL_TraceInfo
#endif


typedef struct gsl_mutex {
#if GL_OBJECT_LIST_MUTEX
	void *next;
#endif
	char name[GSL_NAME_LEN + 1];
	GL_MUTEX_T mutex;
	GL_MUTEX_T lock;
	int m_type;					/* mutex type: MUTEX_NORMAL, MUTEX_RECURSIVE */
	int m_lock;					/* lock mutex deep */
	GL_TASK_T m_owner;			/*the task which create the mutex*/
} gsl_mutex_t;

typedef struct gsl_cond {
#ifdef GL_OBJECT_LIST_COND
	void *next;
#endif
	char name[GSL_NAME_LEN + 1];
	pthread_cond_t cond;
	GL_Mutex_t *mutex;
} gsl_cond_t;

typedef struct gsl_semaphore {
#if GL_OBJECT_LIST_SEMA
	void *next;
#endif
	GL_UINT8 name[GSL_NAME_LEN + 1];
	GL_UINT32 max_count;
	GL_SEM_T sem;
	GL_MUTEX_T lock;
} gsl_semaphore_t;


typedef struct gsl_flag {
#if GL_OBJECT_LIST_FLAG
	void *next;
#endif
	char name[GSL_NAME_LEN + 1];
	GL_UINT32		flag;
	GL_MUTEX_T flock;
	GL_COND_T fcond;
} gsl_flag_t;


typedef struct _msg_item {
	GL_UINT8 *data;
	GL_BOOL is_urgent;
	struct _msg_item *p_next;
} MSG_ITEM;


typedef struct gsl_queue {
#if GL_OBJECT_LIST_QUEUE
	void *next;
#endif
	char name[GSL_NAME_LEN + 1];

	//trace readable message number
	GL_SEM_T read_sem;
	//lock it before operate this structure
	GL_MUTEX_T mt;

	MSG_ITEM *idle_list;
	MSG_ITEM *used_list;
	MSG_ITEM *used_last_item;
	//keep tracing the position that urgent message is supposed to put in the used list.
	MSG_ITEM *urgent_list;
	MSG_ITEM *urgent_last_item;

	GL_UINT32 msg_size;

	GL_UINT32 normal_msg_used;
	GL_UINT32 urgent_msg_used;

	GL_UINT32 normal_msg_max;
	GL_UINT32 urgent_msg_max;
} gsl_queue_t;

#if GL_OBJECT_LIST
extern void gsl_insert_list(void *head, void *elem);
extern int gsl_remove_list(void *phead, void *pelem);
extern void gsl_insert_dlist(void *inserted, void *insert);
extern void gsl_remove_dlist(void *removed);
#endif

typedef struct gsl_mem_var_node{
    struct gsl_mem_var_node *prev;
    struct gsl_mem_var_node *next;
    GL_UINT32 size;
}gsl_mem_var_node_t;

typedef struct gsl_mempool_var {
#ifdef GL_OBJECT_LIST_MEMPOOL
    void *next;
#endif
    char name[GSL_NAME_LEN + 1];
    //cyg_handle_t handle;
    //cyg_mempool_var var;
    gsl_mem_var_node_t *nodehead;
    gsl_mem_var_node_t pseudohead;
	GL_UINT32 freemem;
    GL_UINT8  *positon;
    GL_UINT32 *mempool;
    GL_UINT32 poolsize;
    GL_MUTEX_T mutex;
    GL_COND_T cond;
} gsl_mempool_var_t;

typedef struct gsl_mempool_fix {
#ifdef GL_OBJECT_LIST_MEMPOOL
    void *next;
#endif
    char name[GSL_NAME_LEN + 1];
    //GL_UINT32 handle;
    //mempool_fix_t fix;

    GL_UINT8 *bitmap;
    //GL_UINT32 maptop;
    GL_UINT8  *positon;
    GL_UINT32 *mempool;
    GL_UINT32 blocksize;
    GL_UINT32 numblocks;
    GL_UINT32 freeblocks;
    GL_UINT32 firstfree;
    GL_MUTEX_T mutex;
    GL_COND_T cond;
} gsl_mempool_fix_t;

#if 0
#ifdef MEM_FENCE
void *malloc_r(size_t size, unsigned int ra);
void *mallocA_r(size_t size, unsigned int ra);
void *mallocB_r(size_t size, unsigned int ra);
void *calloc_r(size_t nmemb, size_t size, unsigned int ra);
void *callocA_r(size_t nmemb, size_t size, unsigned int ra);
void *callocB_r(size_t nmemb, size_t size, unsigned int ra);
void *realloc_r(void *ptr, size_t size, unsigned int ra);
void *reallocA_r(void *ptr, size_t size, unsigned int ra);
void *reallocB_r(void *ptr, size_t size, unsigned int ra);
void free_r( void *ptr, unsigned int ra);

#ifdef SUPPORT_MEM_FENCE_POOL
void cyg_mempool_var_create_r(void *base, cyg_int32 size, cyg_handle_t *handle, cyg_mempool_var *var, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void cyg_mempool_var_delete_r(cyg_handle_t varpool, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void *cyg_mempool_var_alloc_r(cyg_handle_t varpool, cyg_int32 size, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void *cyg_mempool_var_timed_alloc_r(cyg_handle_t varpool, cyg_int32 size, cyg_tick_count_t abstime, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void *cyg_mempool_var_try_alloc_r(cyg_handle_t varpool, cyg_int32 size, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void cyg_mempool_var_free_r(cyg_handle_t varpool, void *p, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void cyg_mempool_fix_create_r(void *base, cyg_int32 size, cyg_int32 blocksize, cyg_handle_t *handle, cyg_mempool_fix *fix, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void cyg_mempool_fix_delete_r(cyg_handle_t fixpool, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void *cyg_mempool_fix_alloc_r(cyg_handle_t fixpool, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void *cyg_mempool_fix_timed_alloc_r(cyg_handle_t fixpool, cyg_tick_count_t abstime, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void *cyg_mempool_fix_try_alloc_r(cyg_handle_t fixpool, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
void cyg_mempool_fix_free_r(cyg_handle_t fixpool, void *p, unsigned int ra CYGBLD_ATTRIB_UNUSED) __THROW;
#endif

#endif //MEM_FENCE

#endif //for #if 0

#ifdef GL_OBJECT_LIST
void gsl_insert_list(void *head, void *elem);
int gsl_remove_list(void *phead, void *pelem);
void gsl_insert_dlist(void *inserted, void *insert);
void gsl_remove_dlist(void *removed);
#endif
GL_INT32 GL_SchedulerLock(void);
GL_INT32 GL_SchedulerUnLock(void);

#if (GL_DEBUG == 1)
#define gsl_printf(fmt, arg...)		printf("[GSL] %s:%d: "fmt, __FUNCTION__, __LINE__, ##arg)
#else
#define gsl_printf(fmt, arg...)
#endif
#define gsl_err_printf(fmt, arg...)	printf("[GSL] %s:%d: Error: "fmt, __FUNCTION__, __LINE__, ##arg)

#endif // __GL_COMMON_H__
