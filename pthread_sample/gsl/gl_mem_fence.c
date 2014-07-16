#include "gsl.h"

#if GL_OBJECT_MEM_FENCE //#replace the old define  MEM_FENCE */

#define DIAG_PRINTF	diag_printf_nt

/* For taskpool, there will be only one tag in the begin of the task memory.  */
#define TASKPOOL_USAGE

/*
 * Memory fence spends lots of memory to create fences surrounding user-allocate
 * memory. Generally, overwriting cases occurs in tailing fence. It maybe no need
 * to create heading fence to decrease memory usage of memory fence.
 */
#define SUPPORT_FENCE_HEAD

/* Print debug messages of memory fence.  */
// #define MEM_FENCE_DEBUG


#ifdef SUPPORT_MEM_FENCE_POOL
/*
 * The header size of memory fence using in the base of a memory pool. They are
 * stored in the tail of the memory base.
 *
 * Due to alignment issue, we can use at most (MEM_FENCE_POOL_HEAD_SIZE - 4) bytes
 */
#define MEM_FENCE_POOL_HEAD_SIZE	8

#define CHECK_INVALID_HANDLE(handle)											\
	do {																		\
		if (handle == (cyg_handle_t)NULL) {										\
			DIAG_PRINTF("\n\n\n\n\n%s:Invalid handle=0x%08X (caller=0x%08X):",	\
						__FUNCTION__,											\
						(GL_UINT32)handle,											\
						(GL_UINT32)__builtin_return_address(0));					\
			asm volatile ("break;");											\
		}																		\
	} while (0)
#endif

#define BASIC_NODE_MEMBER					\
	GL_UINT32 type;							\
	GL_UINT32 caller;							\
	GL_UINT32 tid;								\
	cyg_handle_t thread;					\
	struct mem_fence_thread_info_s *tinfo;	\
	struct mem_fence_node_s *prev;			\
	struct mem_fence_node_s *next

#define MEM_FENCE_MAGIC_WORD				0x3355AACC
#define MEM_FENCE_MAGIC_WORD_DOUBLE_FREE	0xACBDFE01

/*
 * Change the value to change the size of fence:
 *
 * size of fence = MEM_FENCE_GL_UINT32_COUNT * sizeof(GL_UINT32) * 2
 */
static GL_UINT32 MEM_FENCE_GL_UINT32_COUNT = 8;

/* Real memory allocation functions.  */
extern void *__real_malloc(size_t c);
extern void __real_realloc(void *ptr, size_t c);
extern void __real_free(void *ptr);

struct mem_fence_thread_info_s;

/* Doubly link list.  */
typedef struct mem_fence_node_s {
	BASIC_NODE_MEMBER;
} mem_fence_node;

typedef struct {
	BASIC_NODE_MEMBER;
	GL_UINT32 size;
	GL_UINT32 align_size;
	GL_UINT32 fence[0];
} mem_fence_head;

typedef struct {
	GL_UINT32 fence[0];
} mem_fence_tail;

typedef struct {
	BASIC_NODE_MEMBER;
	GL_UINT32 tag;			/* To identify different tags.  */
} mem_fence_tag;

#ifdef SUPPORT_MEM_FENCE_POOL
#define BASIC_MEMPOOL_MEMBER	\
	cyg_handle_t pool_hnd;		\
	void *pool_base;			\
	GL_UINT32 pool_type;			\
	GL_UINT32 pool_size;			\
	mem_fence_node *node_list

enum {
	POOL_TYPE_VAR,
	POOL_TYPE_FIX
};

typedef struct {
	BASIC_NODE_MEMBER;
	BASIC_MEMPOOL_MEMBER;
} mem_fence_pool;

typedef struct {
	BASIC_NODE_MEMBER;
	BASIC_MEMPOOL_MEMBER;
	GL_UINT32 blocksize;
	GL_UINT32 blocksize_aligned;
	GL_UINT32 blocksize_real;
} mem_fence_pool_fix;
#endif

typedef struct mem_handle_s {
	GL_UINT32 handle;				/* Thread handle who added the tag.  */
	struct mem_handle_s *next;
} mem_handle;

typedef struct mem_fence_thread_info_s {
	cyg_handle_t th;
	char name[GSL_NAME_LEN + 1];
	GL_UINT32 total_size;
	mem_fence_node *node_list;
	mem_handle *tag_list;
	struct mem_fence_thread_info_s *prev;
	struct mem_fence_thread_info_s *next;
} mem_fence_thread_info;

static mem_fence_thread_info *mem_fence_thread_list_head = NULL;

static mem_handle *mem_fence_find_tag(mem_fence_thread_info *info, cyg_handle_t th);
static mem_fence_tag *mem_fence_insert_tag(mem_fence_thread_info *info, cyg_handle_t th, GL_UINT32 op);
static int mem_fence_remove_tag(mem_fence_thread_info *info, cyg_handle_t th, GL_UINT32 op);

/*
 * Call this function in the initializing period of run-time.
 * Otherwise, it may cause fatal errors.
 */
inline void set_mem_fence_length(GL_UINT32 length)
{
	MEM_FENCE_GL_UINT32_COUNT = length;
}

inline GL_UINT32 get_mem_fence_length(void)
{
	return MEM_FENCE_GL_UINT32_COUNT;
}

enum {
	MEMINFO_BRIEF	= 0,
	MEMINFO_ALL		= 1,
	MEMINFO_FENCE	= 2,
	MEMINFO_POOL	= 3,
	MEMINFO_TAG		= 4
};

enum {
	NODE_TYPE_FENCE	= 1,
	NODE_TYPE_TAG	= 2,
	NODE_TYPE_POOL	= 3
};

#ifdef MEM_FENCE_DEBUG
#define fence_printf(fmt, arg...) DIAG_PRINTF(fmt, ##arg)
#else
#define fence_printf(fmt, arg...)
#endif

#define SIZEOF_FENCE		(sizeof(GL_UINT32) * MEM_FENCE_GL_UINT32_COUNT)
#define SIZEOF_FENCE_TAIL	(sizeof(mem_fence_tail) + SIZEOF_FENCE)

#ifdef SUPPORT_FENCE_HEAD
#define SIZEOF_FENCE_HEAD	(sizeof(mem_fence_head) + SIZEOF_FENCE)
#else
#define SIZEOF_FENCE_HEAD	(sizeof(mem_fence_head))
#endif

#define MALLOC_FENCE_ERROR(p)												\
	do {																	\
		if ((p) == NULL) {													\
			DIAG_PRINTF("\n\n\nmemory fence: CTITCIAL ERROR!!!");			\
			DIAG_PRINTF("in file %s, line %d\n\n\n\n", __FILE__, __LINE__);	\
			while (1) {														\
				continue;													\
			}																\
		}																	\
	} while (0)

#define LIST_INSERT(list, node)		\
	do {							\
		if ((list) == NULL) {		\
			(list) = (node);		\
			(node)->next = NULL;	\
		} else {					\
			(node)->next = (list);	\
			(list) = (node);		\
		}							\
	} while (0)

#define LIST_REMOVE(list, node)		\
	do {							\
		if ((list) == (node)) {		\
			(list) = (node)->next;	\
		}							\
	} while (0)

#define DLIST_INSERT(list, node)			\
	do {									\
		if ((list) == NULL) {				\
			(list) = (node);				\
			(list)->prev = NULL;			\
			(list)->next = NULL;			\
		} else {							\
			(node)->next = (list);			\
			(list)->prev = (node);			\
			(node)->prev = NULL;			\
			(list) = (node);				\
		}									\
	} while (0)

#define DLIST_REMOVE(list, node)				\
	do {										\
		if ((list) == (node)) {					\
			(list) = (node)->next;				\
		} else {								\
			(node)->prev->next = (node)->next;	\
		}										\
		if ((node)->next != NULL) {				\
			(node)->next->prev = (node)->prev;	\
		}										\
	} while (0)

#define MEM_FENCE_CHECK_ONLY(ptr, head, tail, flag)					\
	do {															\
		int i;														\
		for (i = 0; i < MEM_FENCE_GL_UINT32_COUNT; i++) {				\
			if ((head->fence[i] != MEM_FENCE_MAGIC_WORD)			\
				|| (tail->fence[i] != MEM_FENCE_MAGIC_WORD)) {		\
				DIAG_PRINTF("    fence check: magic word error !!! "\
							"alloc caller=0x%08X, "					\
							" ptr=0x%08X, size=%d\n",				\
							(GL_UINT32)head->caller,					\
							(GL_UINT32)ptr,							\
							head->size);							\
				flag = 1;											\
				break;												\
			}														\
		}															\
	} while (0)

#define MEM_FENCE_CHECK(_ptr, head, tail, ra)							\
	do {																\
		int i;															\
		if (((head)->fence[0] == MEM_FENCE_MAGIC_WORD_DOUBLE_FREE)		\
			&& ((tail)->fence[0] == MEM_FENCE_MAGIC_WORD_DOUBLE_FREE)) {\
				DIAG_PRINTF("fence check: it may doubly free !!!\n"		\
							"caller=0x%08X, alloc caller=0x%08X,\n"		\
							"   ptr=0x%08X, size=%d\n\n\n\n\n",			\
							(ra),										\
							(GL_UINT32)(head)->caller,						\
							(GL_UINT32)(_ptr),								\
							(head)->size);								\
				asm volatile("break;");									\
		}																\
		for (i = 0; i < MEM_FENCE_GL_UINT32_COUNT; i++) {					\
			if (((head)->fence[i] != MEM_FENCE_MAGIC_WORD)				\
				|| ((tail)->fence[i] != MEM_FENCE_MAGIC_WORD)) {		\
				DIAG_PRINTF("fence check: magic word error !!!\n"		\
							"caller=0x%08X, alloc caller=0x%08X,\n"		\
							"   ptr=0x%08X, size=%d\n\n\n\n\n",			\
							(ra),										\
							(GL_UINT32)(head)->caller,						\
							(GL_UINT32)(_ptr),								\
							(head)->size);								\
				asm volatile("break;");									\
			}															\
		}																\
	} while (0)


#define PRINT_INDENT(indent)		\
	do {							\
		unsigned int i = indent;	\
		while (0 != i) {			\
			DIAG_PRINTF("    ");	\
			--i;					\
		}							\
	} while (0)

#if 0
static void mem_fence_pool_dump(mem_fence_node *node, const unsigned int indent)
{
	unsigned int i = indent;

	mem_fence_pool *pool = (mem_fence_pool *)node;
	mem_fence_node *f = pool->node_list;
	DIAG_PRINTF("[ POOL] caller=0x%08X, handle=0x%08X, base=0x%08X, size=%d, id=%d\n",
				(GL_UINT32)pool->caller, (GL_UINT32)pool->pool_hnd, (GL_UINT32)pool->pool_base, pool->pool_size, pool->tid);
	if (NULL == f) {
		i = indent + 1;
		DIAG_PRINTF("");
		while (0 != i) {
			DIAG_PRINTF("    ");
			--i;
		}
		DIAG_PRINTF("No memory allocation record.\n");
	} else {
		do {
			mem_fence_node_dump(f, indent + 1);
			f = f->next;
		} while (NULL != f);
	}

}
#endif

static void mem_fence_node_dump(mem_fence_node *node, const unsigned int indent)
{
	PRINT_INDENT(indent);

	switch (node->type) {
	case NODE_TYPE_FENCE: {
		mem_fence_head *f = (mem_fence_head *)node;
		DIAG_PRINTF("[fence] caller=0x%08X: ptr=0x%08X, size=%d, id=%d\n",
					(GL_UINT32)f->caller,
					(GL_UINT32)f + SIZEOF_FENCE_HEAD,
					(GL_UINT32)f->size,
					f->tid);
		break;
	}
#ifdef SUPPORT_MEM_FENCE_POOL
	case NODE_TYPE_POOL: {
		mem_fence_pool *pool = (mem_fence_pool *)node;
		mem_fence_node *f = pool->node_list;
		DIAG_PRINTF("[ pool] caller=0x%08X, handle=0x%08X, base=0x%08X, size=%d, type=%s, id=%d\n",
					(GL_UINT32)pool->caller,
					(GL_UINT32)pool->pool_hnd,
					(GL_UINT32)pool->pool_base,
					pool->pool_size,
					(pool->pool_type == POOL_TYPE_VAR) ? "var" : "fix",
					pool->tid);
		if (NULL == f) {
			PRINT_INDENT(indent + 1);
			DIAG_PRINTF("No memory allocation record.\n");
		} else {
			do {
				/* Dump records of the memory pool.  */
				mem_fence_node_dump(f, indent + 1);
				f = f->next;
			} while (NULL != f);
		}
		break;
	}
#endif
	case NODE_TYPE_TAG: {
		mem_fence_tag *t = (mem_fence_tag *)node;
		DIAG_PRINTF("[  TAG] TagID=0x%08X, caller=0x%08X\n",
					(GL_UINT32)t->caller, t->tid);
		break;
	}
	default:
		return;
	}
}

/*
 * Find memory fence thread info. If not exist, create it.
 */
static mem_fence_thread_info *thread_info_lookup(cyg_handle_t th)
{
	mem_fence_thread_info *t;
	cyg_handle_t thread = 0;
	cyg_uint16 id = 0;
	cyg_thread_info info;

	t = mem_fence_thread_list_head;
	while (t != NULL) {
		if (t->th == th) {
			return t;
		}
		t = t->next;
	}

	/* If not found, create a new one.  */
	t = (mem_fence_thread_info *)__real_malloc(sizeof(mem_fence_thread_info));
	MALLOC_FENCE_ERROR(t);

	t->th = th;
	t->total_size = 0;
	t->node_list = NULL;
	t->tag_list = NULL;

	/*
	 * Get the thread name.
	 *
	 * NOTICE: If the same thread is used to create thread more than one
	 * time (ex; create -> delete -> create), and the name is changed,
	 * then the name stored in memory fence will be incorrect.
	 */
	while (cyg_thread_get_next(&thread, &id)) {
		if (thread == th) {
			char *from;
			char *to;

			cyg_thread_get_info(thread, id, &info);
			from = info.name;
			to = t->name;

			if (from) {
				strncpy(to, from, GSL_NAME_LEN);
			} else {
				*to = 0;
			}
			break;
		}
	}
	DLIST_INSERT(mem_fence_thread_list_head, t);

	return t;
}

static void thread_info_delete(mem_fence_thread_info *info)
{
	DLIST_REMOVE(mem_fence_thread_list_head, info);
	__real_free(info);
}

static void mem_fence_insert(mem_fence_head *f)
{
	mem_fence_thread_info *info;
	cyg_handle_t th;

	th = cyg_thread_self();
	info = thread_info_lookup(th);

	f->tid = cyg_thread_get_id(th);
	f->tinfo = info;

	info->total_size += f->size;

	DLIST_INSERT(info->node_list, (mem_fence_node *)f);
}

static void mem_fence_delete(mem_fence_node *node)
{
	mem_fence_thread_info *info = node->tinfo;

	switch (node->type) {
	case NODE_TYPE_FENCE:
		info->total_size -= ((mem_fence_head *)node)->size;
		break;
#ifdef SUPPORT_MEM_FENCE_POOL
	case NODE_TYPE_POOL:
		break;
#endif
	case NODE_TYPE_TAG:
		break;
	default:
		GL_ASSERT(node->type == NODE_TYPE_FENCE);
		break;
	}

	DLIST_REMOVE(info->node_list, (mem_fence_node *)node);

	/*
	 * If there is no memory allocation records and tag list is empty,
	 * we can delete the thread info.
	 */
	if ((info->total_size == 0) && (info->tag_list == NULL)) {
		thread_info_delete(info);
	}
}

static mem_fence_tag *mem_fence_insert_tag(mem_fence_thread_info *info, cyg_handle_t th, GL_UINT32 op)
{
	mem_handle *hnd;
	mem_fence_tag *tg;

#if 0
	if (mem_fence_find_tag(info, th) != NULL) {
		if ((op & FENCE_TAG_OP_IGNORE) == 0) {
			DIAG_PRINTF("Thread (handle=0x%08X) tag has existed: TagID=0x%08X\n", info->th, th);
		}

		if ((op & FENCE_TAG_OP_FORCE) > 0) {
			// remove old tag
			mem_fence_remove_tag(info, th, op);
		} else {
			return NULL;
		}
	}
#endif
	/* Insert TagID into tag_list.  */
	hnd = (mem_handle *)__real_malloc(sizeof(mem_handle));
	MALLOC_FENCE_ERROR(hnd);
	hnd->handle = th;
	LIST_INSERT(info->tag_list, hnd);

	/* Insert tag node into fence node list.  */
	tg = (mem_fence_tag *)__real_malloc(sizeof(mem_fence_tag));
	MALLOC_FENCE_ERROR(tg);
	tg->type = NODE_TYPE_TAG;
	tg->tid = th;
	tg->tinfo = info;
	DLIST_INSERT(info->node_list, (mem_fence_node *)tg);

	return tg;
}

static mem_handle *mem_fence_find_tag(mem_fence_thread_info *info, cyg_handle_t th)
{
#ifdef TASKPOOL_USAGE
	/* Not check ID. */
	return info->tag_list;
#else
	mem_handle *tg;

	tg = info->tag_list;
	while (tg != NULL) {
		if (tg->handle == th) {
			break;
		}
		tg = tg->next;
	}
	return tg;
#endif
}

static int mem_fence_remove_tag(mem_fence_thread_info *info, cyg_handle_t th, GL_UINT32 op)
{
	mem_handle *tg;
	mem_fence_node *node;

	/* Search tag node.  */
	tg = mem_fence_find_tag(info, th);

	if (tg == NULL) {
		if ((op & FENCE_TAG_OP_IGNORE) == 0) {
			DIAG_PRINTF("    tag does not exist. TagID=0x%08X\n", th);
		}
		return -1;
	}

	/* Traverse from head node to the tag ndoe.  */
	node = info->node_list;
#if 0
	if ((op & FENCE_TAG_OP_FREE) > 0) {
		mem_fence_head *ftmp;

		/* Free the node from head node to the first tag node.  */
		while (f != NULL) {
			if (IS_FENCE_TAG(f)) {
#ifdef TASKPOOL_USAGE
				/* For taskpool, there will be only one tag in the begin of the task memory.  */
				break;
#else
				if (f->tid == th) {
					break;
				}
				// it is a tag added by another thread
				f = f->next;
				continue;
#endif
			}
			// free memory allocation
			DIAG_PRINTF("    free fence node (caller=0x%08X): ptr=0x%08X, size=%d\n",
						  f->caller,
						  (GL_UINT32)((char *)(f) + SIZEOF_FENCE_HEAD),
						  f->size);
			ftmp = f;
			f = f->next;
			mem_fence_delete(f->prev);
			__real_free(ftmp);
		}
	} else
#endif
	{
		GL_UINT32 flag = 0;
		while ((node != NULL) && (0 == flag)) {
			switch (node->type) {
			case NODE_TYPE_FENCE: {
				mem_fence_head *f = (mem_fence_head *)node;
				DIAG_PRINTF("    find fence node (caller=0x%08X): ptr=0x%08X, size=%d\n",
							(GL_UINT32)f->caller,
							(GL_UINT32)((char *)f + SIZEOF_FENCE_HEAD),
							f->size);
				node = node->next;
				break;
			}
#ifdef SUPPORT_MEM_FENCE_POOL
			case NODE_TYPE_POOL:
//				mem_fence_pool_dump((mem_fence_pool *)node);
				break;
#endif
			case NODE_TYPE_TAG:
#ifdef TASKPOOL_USAGE
				flag = 1;
				/* For taskpool, there will be only one tag in the begin of the task memory.  */
				break;
#else
				if (node->tid == th) {
					flag = 1;
					break;
				}
				node = node->next;
				continue;
#endif
			default:
				GL_ASSERT(node->type == NODE_TYPE_FENCE);
				break;
			}
		}
	}
	GL_ASSERT(node->type == NODE_TYPE_TAG);

	if ((op & FENCE_TAG_OP_KEEP) > 0) {
		/* Keep the tag node and do not free it.  */
		return 0;
	}

	/* Free tag in the tag_list.  */
	LIST_REMOVE(info->tag_list, tg);
	__real_free(tg);

	/* Free tag node.  */
	DIAG_PRINTF("    tag node remove (caller=0x%08X): TagID=0x%08X\n", node->caller, th);
	mem_fence_delete(node);
	__real_free(node);

	return 0;
}

static size_t mem_fence_show_thread_real(mem_fence_thread_info *t, cyg_handle_t th, GL_UINT32 detail)
{
	size_t total_size = 0;
	mem_fence_node *node;
	cyg_handle_t thread = 0;
	cyg_uint16 id = 0;
	cyg_thread_info info;
	GL_UINT32 valid = 0;

	if (NULL == t) {
		DIAG_PRINTF("Thread (handle=0x%08X), total_alloc_size=0, no fence info\n", th);
		return total_size;
	}

	total_size = t->total_size;
	DIAG_PRINTF("Thread (hnd=0x%08X, id=%u), total_alloc_size=%d, ",
				t->th, cyg_thread_get_id(t->th), total_size);

	while (cyg_thread_get_next(&thread, &id)) {
		if (thread == th) {
			cyg_thread_get_info(thread, id, &info);
			valid = 1;
			break;
		}
	}

	DIAG_PRINTF("Name: ");
	if (info.name != NULL) {
		DIAG_PRINTF("%s", info.name);
	} else {
		DIAG_PRINTF("(no name)");
	}
	if (valid == 0) {
		DIAG_PRINTF(" (a dead thread) !!!\n");
	} else {
		DIAG_PRINTF("\n");
	}

	if (MEMINFO_BRIEF != detail) {
		DIAG_PRINTF(", memory info:\n");

		node = t->node_list;
		if (MEMINFO_ALL == detail) {
			while (node != NULL) {
				mem_fence_node_dump(node, 1);
				if ((id != node->tid) && (NODE_TYPE_TAG != node->type)) {
					DIAG_PRINTF("        id mismatch: task_id=%d, fence_id=%d !!!\n", id, node->tid);
				}
				node = node->next;
			}
		} else {
			GL_UINT32 type;

			switch (detail) {
			case MEMINFO_FENCE:
			default:
				type = NODE_TYPE_FENCE;
				break;
#ifdef SUPPORT_MEM_FENCE_POOL
			case MEMINFO_POOL:
				type = NODE_TYPE_POOL;
				break;
#endif
			case MEMINFO_TAG:
				type = NODE_TYPE_TAG;
				break;
			}
			while (node != NULL) {
				if (type == node->type) {
					mem_fence_node_dump(node, 1);
					if ((id != node->tid) && (NODE_TYPE_TAG != node->type)) {
						DIAG_PRINTF("        id mismatch: task_id=%d, fence_id=%d !!!\n", id, node->tid);
					}
				}
				node = node->next;
			}
		}
	}

	return total_size;
}

#endif

int mem_fence_tag_start(GL_UINT32 target, GL_UINT32 op)
{
#if 0
	mem_fence_thread_info *t = NULL;
	mem_fence_tag *tg;
	cyg_handle_t thread = 0;
	cyg_handle_t th;
	cyg_uint16 id = 0;
	GL_UINT32 ra;

	ra = (GL_UINT32)__builtin_return_address(0);

	DIAG_PRINTF("============== MEMORY FENCE TAG <START> ==============\n");
	th = (cyg_handle_t)target;
	if (th < 0x80000000) {
		th = cyg_thread_find(th);
	}

	cyg_scheduler_lock();
	while (cyg_thread_get_next(&thread, &id)) {
		if (thread == th) {
			t = thread_info_lookup(thread);
			break;
		}
	}
	if (t == NULL) {
		DIAG_PRINTF("    Thread does not exist: handle=0x%08x\n", th);
		cyg_scheduler_unlock();
		return 0;
	}
	DIAG_PRINTF("    Thread handle: 0x%08X, op: %x, Name: \"%s\"\n", thread, op, t->name);
	tg = mem_fence_insert_tag(t, cyg_thread_self(), op);
	if (tg != NULL) {
		tg->caller = ra;
	}
	cyg_scheduler_unlock();
#endif
	return 1;
}

int mem_fence_tag_start_all(GL_UINT32 op)
{
#if 0
	mem_fence_thread_info *t;
	mem_fence_tag *tg;
	cyg_handle_t thread = 0;
	cyg_uint16 id = 0;
	GL_UINT32 ra;
	int num = 0;

	ra = (GL_UINT32)__builtin_return_address(0);
	DIAG_PRINTF("============ MEMORY FENCE TAG ALL<START> ============\n");

	cyg_scheduler_lock();
	/* It's time consuming if there are a large amount of threads */
	/* go through all created threads */
	while (cyg_thread_get_next(&thread, &id)) {
		t = thread_info_lookup(thread);
		tg = mem_fence_insert_tag(t, cyg_thread_self(), op);
		if (NULL != tg) {
			tg->caller = ra;
			DIAG_PRINTF("    Thread (handle=0x%08X) insert tag (caller=0x%08X):"
						" TagID=0x%08x, Name: \"%s\"\n",
						t->th, tg->caller, tg->tid, t->name);
			++num;
		}
		t = t->next;
	}
	cyg_scheduler_unlock();

	return num;
#endif
	return 1;

}

int mem_fence_tag_end(GL_UINT32 target, GL_UINT32 op)
{
#if 0
	mem_fence_thread_info *t;
	cyg_handle_t th;
	int num = 0;

	DIAG_PRINTF("============== MEMORY FENCE TAG <END> ==============\n");
	th = (cyg_handle_t)target;
	if (th < 0x80000000) {
		th = cyg_thread_find(th);
		DIAG_PRINTF("thread handle: 0x%08X\n", th);
	}

	cyg_scheduler_lock();
	t = mem_fence_thread_list_head;
	while (t != NULL) {
		if (t->th == th) {		// found it
			break;
		}
		t = t->next;
	}
	if (t == NULL) {
		DIAG_PRINTF("Thread does not exist: handle=0x%08X\n", th);
		return 0;
	}
	DIAG_PRINTF("Thread handle: 0x%08X, op: %u, Name: \"%s\"\n", t->th, op, t->name);
	mem_fence_remove_tag(t, cyg_thread_self(), op);
	cyg_scheduler_unlock();

	return num;
#endif

	return 1;
}

int mem_fence_tag_end_all(GL_UINT32 op)
{
#if 0
	mem_fence_thread_info *t;
	mem_fence_thread_info *tnext;
	int num = 0;

	DIAG_PRINTF("============ MEMORY FENCE TAG ALL <END> ============\n");
	cyg_scheduler_lock();
	t = mem_fence_thread_list_head;
	while (t != NULL) {
		tnext = t->next;	// while removing tag, it may remove self.
		DIAG_PRINTF("Thread hnd: 0x%08X, id: %u, Name: \"%s\"\n",
					t->th, cyg_thread_get_id(t->th), t->name);
		mem_fence_remove_tag(t, cyg_thread_self(), op);
		t = tnext;
	}
	cyg_scheduler_unlock();

	return num;
#endif 

	return 1;
}

size_t mem_fence_show_thread(GL_UINT32 target, GL_UINT32 detail)
{
#if 0
	size_t total_size;
	mem_fence_thread_info *t;
	cyg_handle_t th;

	DIAG_PRINTF("=============== MEMORY FENCE (log start) ===============\n");
	th = (cyg_handle_t) target;
	/* if it is a thread id, find its handle */
	if (th < 0x80000000) {
		th = cyg_thread_find(th);
	}
	cyg_scheduler_lock();
	t = mem_fence_thread_list_head;
	while (t != NULL) {
		if (t->th == th) {		// found it
			break;
		}
		t = t->next;
	}
	total_size = mem_fence_show_thread_real(t, th, detail);	// t == NULL is allowed
	cyg_scheduler_unlock();
	DIAG_PRINTF("===============  MEMORY FENCE (log end)  ===============\n");

	return total_size;
#endif
	return 0;
}

size_t mem_fence_show_all(GL_UINT32 detail)
{
#if 0
	size_t total_size = 0;
	mem_fence_thread_info *t;

	DIAG_PRINTF("=============== MEMORY FENCE (log start) ===============\n");
	cyg_scheduler_lock();
	t = mem_fence_thread_list_head;
	while (t != NULL) {
		total_size += mem_fence_show_thread_real(t, t->th, detail);
		t = t->next;
	}
	cyg_scheduler_unlock();
	DIAG_PRINTF("===============  MEMORY FENCE (log end)  ===============\n");

	return total_size;
#endif
	return 0;
}

/************************************************************
 *	Description:
 *	Check if the memory usage is out of malloc memory boundary.
 *	To see if the fence still keeps the value of MEM_FENCE_MAGIC_WORD
 ************************************************************/
void mem_fence_memory_check(GL_UINT32 target)
{
#if 0
	GL_UINT32 flag = 0;
	cyg_handle_t th;
	mem_fence_thread_info *t = NULL;
	mem_fence_node *node;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;
	unsigned char *ptr;

	th = (cyg_handle_t) target;
	/* if it is a thread id, find its handle */
	if (th < 0x80000000) {
		th = cyg_thread_find(th);
	}

	cyg_scheduler_lock();
	t = thread_info_lookup(th);
	if (t == NULL) {
		cyg_scheduler_unlock();
		DIAG_PRINTF("Thread does not exist: handle=0x%08X\n", th);
		return;
	}
	DIAG_PRINTF("Thread handle: 0x%08X, Name: %s\n", th, t->name);

	node = t->node_list;
	while (NULL != node) {
		switch (node->type) {
		case NODE_TYPE_FENCE:
		default:
			fence_head = (mem_fence_head *)node;
			ptr = ((unsigned char *)fence_head) + SIZEOF_FENCE_HEAD;
			fence_tail = (mem_fence_tail *)(ptr + fence_head->align_size);
			MEM_FENCE_CHECK_ONLY(ptr, fence_head, fence_tail, flag);
			break;
#ifdef SUPPORT_MEM_FENCE_POOL
		case NODE_TYPE_POOL: {
			mem_fence_node *n = ((mem_fence_pool *)node)->node_list;
			while (NULL != n) {
				fence_head = (mem_fence_head *)n;
				ptr = ((unsigned char *)fence_head) + SIZEOF_FENCE_HEAD;
				fence_tail = (mem_fence_tail *)(ptr + fence_head->align_size);
				MEM_FENCE_CHECK_ONLY(ptr, fence_head, fence_tail, flag);
				n = n->next;
			}
			break;
		}
#endif
		case NODE_TYPE_TAG:
			node = node->next;
			continue;
		}
		node = node->next;
	}

	if (0 == flag) {
		DIAG_PRINTF("    No error occurred.\n");
	}

	cyg_scheduler_unlock();
#endif
}

void mem_fence_memory_check_all(void)
{
#if 0
	mem_fence_thread_info *t = NULL;

	cyg_scheduler_lock();
	t = mem_fence_thread_list_head;
	if (t == NULL) {
		cyg_scheduler_unlock();
		DIAG_PRINTF("No thread info exist\n");
		return;
	}
	while (t != NULL) {
		mem_fence_memory_check((GL_UINT32)t->th);
		t = t->next;
	}
	cyg_scheduler_unlock();
#endif
}

#if GL_OBJECT_MEM_FENCE /*junxian add define of GL_OBJECT_MEM_FENCE*/

/* we must let __wrap_malloc_real() inline to decode real caller address */
static inline void *__wrap_malloc_real(size_t size, void *(*malloc_ptr)(size_t)) __attribute__ ((always_inline));
static inline void *__wrap_malloc_real(size_t size, void *(*malloc_ptr)(size_t))
{
	int i;
	size_t align_size;
	unsigned char *p;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;

	GL_CHECK_BLOCK_IN_ISR();

	if (size != 0) {
		align_size = (size + 3) & 0xFFFFFFFC;
		p = malloc_ptr(SIZEOF_FENCE_HEAD + align_size + SIZEOF_FENCE_TAIL);
		if (NULL != p) {
			fence_head = (mem_fence_head *)p;
			fence_tail = (mem_fence_tail *)(p + SIZEOF_FENCE_HEAD + align_size);
			fence_head->caller = (GL_UINT32)__builtin_return_address(0);
			fence_head->size = size;
			fence_head->align_size = align_size;
			fence_head->type = NODE_TYPE_FENCE;

			for (i = 0; i < MEM_FENCE_GL_UINT32_COUNT; i++) {
				fence_head->fence[i] = MEM_FENCE_MAGIC_WORD;
				fence_tail->fence[i] = MEM_FENCE_MAGIC_WORD;
			}
			p += SIZEOF_FENCE_HEAD;

			cyg_scheduler_lock();
			mem_fence_insert(fence_head);
			cyg_scheduler_unlock();
		}

		fence_printf("fence malloc (caller=0x%08X): ptr=0x%08X, size=%d\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p, size);
	} else {
		p = __real_malloc(0);
	}

	return p;
}

void *__wrap_malloc(size_t size)
{
	return __wrap_malloc_real(size, &__real_malloc);
}

void *__wrap_calloc(size_t nmemb, size_t size)
{
	size_t total_size;
	void *p;

	total_size = nmemb * size;
	p = __wrap_malloc_real(total_size, &__real_malloc);

	if (NULL != p) {
		memset(p, 0, total_size);
	}

	return p;
}

static inline void *__wrap_realloc_real(void *ptr, size_t size, void *(*malloc_ptr)(size_t)) __attribute__ ((always_inline));
static inline void *__wrap_realloc_real(void *ptr, size_t size, void *(*malloc_ptr)(size_t))
{
	void *p;

	if (NULL == ptr) {
		return __wrap_malloc_real(size, malloc_ptr);
	}

	if (0 == size) {
		free(ptr);
		return __wrap_malloc_real(0, malloc_ptr);
	}

	p = __wrap_malloc_real(size, malloc_ptr);
	if (NULL != p) {
		mem_fence_head *fence_head = (mem_fence_head *)((char *)ptr - SIZEOF_FENCE_HEAD);
		unsigned int old_size = fence_head->size;

		memcpy(p, ptr, size < old_size ? size : old_size);
		free(ptr);
	}

	return p;
}

void *__wrap_realloc(void *ptr, size_t size)
{
	return __wrap_realloc_real(ptr, size, &__real_malloc);
}

extern void *__real_mallocA(size_t c);
extern void *__real_mallocB(size_t c);

void *__wrap_mallocA(size_t size)
{
	return __wrap_malloc_real(size, &__real_mallocA);
}

void *__wrap_mallocB(size_t size)
{
	return __wrap_malloc_real(size, &__real_mallocB);
}

void *__wrap_callocA(size_t nmemb, size_t size)
{
	size_t total_size;
	void *p;

	total_size = nmemb * size;
	p = __wrap_malloc_real(total_size, &__real_mallocA);

	if (NULL != p) {
		memset(p, 0, total_size);
	}

	return p;
}

void *__wrap_callocB(size_t nmemb, size_t size)
{
	size_t total_size;
	void *p;

	total_size = nmemb * size;
	p = __wrap_malloc_real(total_size, &__real_mallocB);

	if (NULL != p) {
		memset(p, 0, total_size);
	}

	return p;
}

void *__wrap_reallocA(void *ptr, size_t size)
{
	return __wrap_realloc_real(ptr, size, &__real_mallocA);
}

void *__wrap_reallocB(void *ptr, size_t size)
{
	return __wrap_realloc_real(ptr, size, &__real_mallocB);
}

void __wrap_free(void *ptr)
{
	unsigned char *p;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;

	GL_CHECK_BLOCK_IN_ISR();

	if (NULL != ptr) {
#ifdef CYGSEM_MEMALLOC_MALLOC_ZERO_RETURNS_SPECIAL
		if (CYGSEM_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR == (unsigned int)ptr) {
			return;
		}
#endif
		p = (unsigned char *)ptr;

		fence_head = (mem_fence_head *)(p - SIZEOF_FENCE_HEAD);
		fence_tail = (mem_fence_tail *)(p + fence_head->align_size);

		fence_printf("fence free (caller=0x%08X, fence alloc caller=0x%08X): ptr=0x%08X, size=%d\n",
					 (GL_UINT32)__builtin_return_address(0),
					 (GL_UINT32)fence_head->caller, (GL_UINT32)ptr, fence_head->size);
		MEM_FENCE_CHECK(ptr, fence_head, fence_tail, (GL_UINT32)__builtin_return_address(0));
		// let further __wrap_free() fail
		fence_head->fence[0] = MEM_FENCE_MAGIC_WORD_DOUBLE_FREE;
		fence_tail->fence[0] = MEM_FENCE_MAGIC_WORD_DOUBLE_FREE;

		cyg_scheduler_lock();
		mem_fence_delete((mem_fence_node *)fence_head);
		cyg_scheduler_unlock();

		__real_free(fence_head);
	} else {
		DIAG_PRINTF("\n\n\n\n\nfence free (caller=0x%08X): try to free NULL !!!\n\n\n\n\n",
					(GL_UINT32)__builtin_return_address(0));
		asm volatile("break;");
	}
}

#endif

#if GL_OBJECT_MEM_FENCE /*junxian add define of GL_OBJECT_MEM_FENCE*/

#ifdef SUPPORT_MEM_FENCE_POOL
extern void __real_cyg_mempool_var_create(void *base, cyg_int32 size, cyg_handle_t *handle, cyg_mempool_var *var);
extern void __real_cyg_mempool_var_delete(cyg_handle_t varpool);
extern void *__real_cyg_mempool_var_alloc(cyg_handle_t varpool, cyg_int32 size);
extern void *__real_cyg_mempool_var_try_alloc(cyg_handle_t varpool, cyg_int32 size);
extern void *__real_cyg_mempool_var_timed_alloc(cyg_handle_t varpool, cyg_int32 size, cyg_tick_count_t abstime);
extern void __real_cyg_mempool_var_free(cyg_handle_t varpool, void *ptr);

static int _var_get_fence_pool(cyg_handle_t handle, mem_fence_pool **pool)
{
	cyg_mempool_info info;
	char *p;

	cyg_mempool_var_get_info(handle, &info);
	if (info.size < MEM_FENCE_POOL_HEAD_SIZE) {
		return -1;
	}
	p = (char *)info.base;
	p = p + info.size;
	p = (char *)(((GL_UINT32)p + 3) & ~3);
	*pool = *(mem_fence_pool **)p;

	return 0;
}

static void mem_fence_insert_pool(mem_fence_pool *pool)
{
	mem_fence_thread_info *info;
	cyg_handle_t th;

	th = cyg_thread_self();
	info = thread_info_lookup(th);

	pool->tid = cyg_thread_get_id(th);
	pool->tinfo = info;
	pool->type = NODE_TYPE_POOL;

	DLIST_INSERT(info->node_list, (mem_fence_node *)pool);
}

static void mem_fence_delete_pool(mem_fence_pool *pool, GL_UINT32 ra)
{
	mem_fence_thread_info *info = pool->tinfo;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;
	GL_UINT32 flag = 0;

	if (NULL != pool->node_list) {
#if 0
		DIAG_PRINTF("\n\n\n\n\n");
		DIAG_PRINTF("fence delete pool error (caller=0x%08X): memory pool is still in use!!!\n", ra);
		mem_fence_node_dump((mem_fence_node *)pool, 1);
		DIAG_PRINTF("\n\n\n\n\n");
		asm volatile("break");
#endif
		mem_fence_node *n = pool->node_list;
		mem_fence_node *tmp;

		while (NULL != n) {
			unsigned char *ptr;
			fence_head = (mem_fence_head *)n;
			ptr = ((unsigned char *)fence_head) + SIZEOF_FENCE_HEAD;
			fence_tail = (mem_fence_tail *)(ptr + fence_head->align_size);
			MEM_FENCE_CHECK_ONLY(ptr, fence_head, fence_tail, flag);
			tmp = n;
			n = n->next;
		}

		if (1 == flag) {
			DIAG_PRINTF("    fence checking error while deleting pool !!! pool info:\n");
			mem_fence_node_dump((mem_fence_node *)pool, 2);
			asm volatile("break");
		}
	}

	DLIST_REMOVE(info->node_list, (mem_fence_node *)pool);

	/*
	 * If there is no memory allocation records and tag list is empty,
	 * we can delete the thread info.
	 */
	if ((info->total_size == 0) && (info->tag_list == NULL)) {
		thread_info_delete(info);
	}
}

void __wrap_cyg_mempool_var_create(void *base, cyg_int32 size, cyg_handle_t *handle, cyg_mempool_var *var)
{
	mem_fence_pool *pool;

	GL_CHECK_BLOCK_IN_ISR();

	if (size < MEM_FENCE_POOL_HEAD_SIZE) {
		fence_printf("fence create var mempool failed due to small amount of base memory !! (caller=0x%08X): handle=0x%08X, base=0x%08X\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)handle, (GL_UINT32)base);
		__real_cyg_mempool_var_create(base, size, handle, var);
		return;
	}

	if ((0 == size) || (NULL == base)
#ifdef CYGSEM_MEMALLOC_MALLOC_ZERO_RETURNS_SPECIAL
		|| (CYGSEM_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR == (unsigned int)base)
#endif
	) {
		DIAG_PRINTF("fence create var mempool failed !! caller=0x%08X, handle=0x%08X, base=0x%08X, size=%u\n",
					(GL_UINT32)__builtin_return_address(0), (GL_UINT32)handle, (GL_UINT32)base, size);
		asm volatile ("break");
	}

	pool = malloc(sizeof(mem_fence_pool));
	if (NULL != pool) {
		char *p = (char *)base;

		pool->pool_hnd = (cyg_handle_t)var;
		pool->pool_type = POOL_TYPE_VAR;
		pool->pool_base = base;
		pool->pool_size = size;
		pool->caller = (GL_UINT32)__builtin_return_address(0);
		pool->node_list = NULL;

		p = p + pool->pool_size - MEM_FENCE_POOL_HEAD_SIZE;
		p = (char *)(((GL_UINT32)p + 3) & ~3);
		*(GL_UINT32 *)p = (GL_UINT32)pool;

		cyg_scheduler_lock();
		mem_fence_insert_pool(pool);
		cyg_scheduler_unlock();

		__real_cyg_mempool_var_create(pool->pool_base, pool->pool_size - MEM_FENCE_POOL_HEAD_SIZE, handle, var);
	}
	fence_printf("fence create var mempool (caller=0x%08X): handle=0x%08X, base=0x%08X, size=%d\n",
				 (GL_UINT32)__builtin_return_address(0), *handle, (GL_UINT32)pool->pool_base, pool->pool_size);
}

void __wrap_cyg_mempool_var_delete(cyg_handle_t varpool)
{
	mem_fence_pool *pool = NULL;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(varpool);

	if (0 != _var_get_fence_pool(varpool, &pool)) {
		__real_cyg_mempool_var_delete(varpool);
		return;
	}

	fence_printf("fence delete pool (caller=0x%08X, fence alloc caller=0x%08X): ptr=0x%08X, size=%d\n",
				 (GL_UINT32)__builtin_return_address(0),
				 (GL_UINT32)pool->caller, (GL_UINT32)varpool, pool->pool_size);

	cyg_scheduler_lock();
	mem_fence_delete_pool(pool, (GL_UINT32)__builtin_return_address(0));
	cyg_scheduler_unlock();

	free(pool);
	__real_cyg_mempool_var_delete(varpool);
}

/* Algorithm for var mempool. It's like algorithm for malloc.  */
static void *var_mempool_alloc_inner(mem_fence_pool *pool, unsigned char *p, cyg_int32 size, GL_UINT32 ra)
{
	int i;
	size_t align_size;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;

	align_size = (size + 3) & 0xFFFFFFFC;
	fence_head = (mem_fence_head *)p;
	fence_tail = (mem_fence_tail *)(p + SIZEOF_FENCE_HEAD + align_size);
	fence_head->caller = ra;
	fence_head->size = size;
	fence_head->align_size = align_size;
	fence_head->type = NODE_TYPE_FENCE;
	fence_head->tid = cyg_thread_get_id(cyg_thread_self());
	fence_head->tinfo = NULL;	/* We can recognize current thread by its ID.  */

	for (i = 0; i < MEM_FENCE_GL_UINT32_COUNT; i++) {
		fence_head->fence[i] = MEM_FENCE_MAGIC_WORD;
		fence_tail->fence[i] = MEM_FENCE_MAGIC_WORD;
	}
	p = p + SIZEOF_FENCE_HEAD;

	cyg_scheduler_lock();
	DLIST_INSERT(pool->node_list, (mem_fence_node *)fence_head);
	cyg_scheduler_unlock();

	return (void *)p;
}

void *__wrap_cyg_mempool_var_alloc(cyg_handle_t varpool, GL_UINT32 size)
{
	mem_fence_pool *pool = NULL;
	unsigned char *p;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(varpool);

	if (0 != _var_get_fence_pool(varpool, &pool)) {
		return __real_cyg_mempool_var_alloc(varpool, size);
	}

	p = __real_cyg_mempool_var_alloc(varpool, SIZEOF_FENCE_HEAD + ((size + 3) & 0xFFFFFFFC) + SIZEOF_FENCE_TAIL);
	if (NULL != p) {
		p = var_mempool_alloc_inner(pool, p, size, (GL_UINT32)__builtin_return_address(0));
		fence_printf("fence pool malloc (caller=0x%08X): ptr=0x%08X, size=%d\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p, size);
	} else {
		p = malloc(size);
	}

	return p;
}

void *__wrap_cyg_mempool_var_try_alloc(cyg_handle_t varpool, cyg_int32 size)
{
	mem_fence_pool *pool = NULL;
	unsigned char *p;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(varpool);

	if (0 != _var_get_fence_pool(varpool, &pool)) {
		return __real_cyg_mempool_var_try_alloc(varpool, size);
	}

	p = __real_cyg_mempool_var_try_alloc(varpool, SIZEOF_FENCE_HEAD + ((size + 3) & 0xFFFFFFFC) + SIZEOF_FENCE_TAIL);
	if (NULL != p) {
		p = var_mempool_alloc_inner(pool, p, size, (GL_UINT32)__builtin_return_address(0));
		fence_printf("fence pool malloc (caller=0x%08X): ptr=0x%08X, size=%d\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p, size);
	} else {
		p = malloc(size);
	}

	return p;
}

void *__wrap_cyg_mempool_var_timed_alloc(cyg_handle_t varpool, cyg_int32 size, cyg_tick_count_t abstime)
{
	mem_fence_pool *pool = NULL;
	unsigned char *p;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(varpool);

	if (0 != _var_get_fence_pool(varpool, &pool)) {
		return __real_cyg_mempool_var_timed_alloc(varpool, size, abstime);
	}

	p = __real_cyg_mempool_var_timed_alloc(varpool, SIZEOF_FENCE_HEAD + ((size + 3) & 0xFFFFFFFC) + SIZEOF_FENCE_TAIL, abstime);
	if (NULL != p) {
		p = var_mempool_alloc_inner(pool, p, size, (GL_UINT32)__builtin_return_address(0));
		fence_printf("fence pool malloc (caller=0x%08X): ptr=0x%08X, size=%d\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p, size);
	} else {
		p = malloc(size);
	}

	return p;
}

void __wrap_cyg_mempool_var_free(cyg_handle_t varpool, void *ptr)
{
	unsigned char *p;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;
	mem_fence_pool *pool;

	GL_CHECK_BLOCK_IN_ISR();

	if ((NULL == ptr) || (0 != _var_get_fence_pool(varpool, &pool))) {
		__real_cyg_mempool_var_free(varpool, ptr);
		return;
	}

	p = (unsigned char *)ptr;
	fence_head = (mem_fence_head *)(p - SIZEOF_FENCE_HEAD);
	fence_tail = (mem_fence_tail *)(p + fence_head->align_size);

	/*
	 * The previous memory allocation was failed in the memory pool, so
	 * we allocated from heap then.
	 */
	// if (NODE_TYPE_FENCE == fence_head->type) {
	if (NULL != fence_head->tinfo) {
		free(ptr);
		return;
	}

	fence_printf("fence pool free (caller=0x%08X, fence alloc caller=0x%08X): "
				 "handle=0x%08X, ptr=0x%08X, size=%d\n",
				 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)fence_head->caller,
				 varpool, (GL_UINT32)ptr, fence_head->size);

	MEM_FENCE_CHECK(ptr, fence_head, fence_tail, (GL_UINT32)__builtin_return_address(0));

	// let further free operations fail
	fence_head->fence[0] = 0;
	fence_tail->fence[0] = 0;

	cyg_scheduler_lock();
	DLIST_REMOVE(pool->node_list, (mem_fence_node *)fence_head);
	cyg_scheduler_unlock();

	__real_cyg_mempool_var_free(varpool, ptr);
}

/* Fixed memory pool APIs.  */
extern void __real_cyg_mempool_fix_create(void *base, cyg_int32 size, cyg_int32 blocksize, cyg_handle_t *handle, cyg_mempool_fix *fix);
extern void __real_cyg_mempool_fix_delete(cyg_handle_t fixpool);
extern void *__real_cyg_mempool_fix_alloc(cyg_handle_t fixpool);
extern void *__real_cyg_mempool_fix_try_alloc(cyg_handle_t fixpool);
extern void *__real_cyg_mempool_fix_timed_alloc(cyg_handle_t fixpool, cyg_tick_count_t abstime);
extern void __real_cyg_mempool_fix_free(cyg_handle_t fixpool, void *ptr);

static int _fix_get_fence_pool(cyg_handle_t handle, mem_fence_pool **pool)
{
	cyg_mempool_info info;
	char *p;

	cyg_mempool_fix_get_info(handle, &info);
	if (info.size < MEM_FENCE_POOL_HEAD_SIZE) {
		return -1;
	}
	p = (char *)info.base;
	p = p + info.size;
	p = (char *)(((GL_UINT32)p + 3) & ~3);
	*pool = *(mem_fence_pool **)p;

	return 0;
}

/* Algorithm for var mempool. It's like algorithm for malloc.  */
static void *fix_mempool_alloc_inner(mem_fence_pool_fix *pool, unsigned char *p, GL_UINT32 ra)
{
	int i;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;

	fence_head = (mem_fence_head *)p;
	fence_tail = (mem_fence_tail *)(p + SIZEOF_FENCE_HEAD + pool->blocksize_aligned);
	fence_head->caller = ra;
	fence_head->size = pool->blocksize;
	fence_head->align_size = pool->blocksize_aligned;
	fence_head->type = NODE_TYPE_FENCE;
	fence_head->tid = cyg_thread_get_id(cyg_thread_self());
	fence_head->tinfo = NULL;	/* We can recognize current thread by its ID.  */

	for (i = 0; i < MEM_FENCE_GL_UINT32_COUNT; i++) {
		fence_head->fence[i] = MEM_FENCE_MAGIC_WORD;
		fence_tail->fence[i] = MEM_FENCE_MAGIC_WORD;
	}
	p = p + SIZEOF_FENCE_HEAD;

	cyg_scheduler_lock();
	DLIST_INSERT(pool->node_list, (mem_fence_node *)fence_head);
	cyg_scheduler_unlock();

	return (void *)p;
}

void __wrap_cyg_mempool_fix_create(void *base, cyg_int32 size, cyg_int32 blocksize, cyg_handle_t *handle, cyg_mempool_fix *fix)
{
	mem_fence_pool_fix *pool;

	GL_CHECK_BLOCK_IN_ISR();

	if (size < MEM_FENCE_POOL_HEAD_SIZE) {
		fence_printf("fence create fix pool failed due to small amount of base memory !! (caller=0x%08X): handle=0x%08X, base=0x%08X\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)handle, (GL_UINT32)base);
		__real_cyg_mempool_fix_create(base, size, blocksize, handle, fix);
		return;
	}

	if ((0 == size) || (NULL == base)
#ifdef CYGSEM_MEMALLOC_MALLOC_ZERO_RETURNS_SPECIAL
		|| (CYGSEM_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR == (unsigned int)base)
#endif
	) {
		DIAG_PRINTF("fence create fix pool failed !! (caller=0x%08X): handle=0x%08X, base=0x%08X, size=%u\n",
					(GL_UINT32)__builtin_return_address(0), (GL_UINT32)handle, (GL_UINT32)base, size);
		asm volatile ("break");
	}

	pool = malloc(sizeof(mem_fence_pool_fix));
	if (pool != NULL) {
		unsigned char *p = (unsigned char *)base;

		pool->pool_hnd = (cyg_handle_t)fix;
		pool->pool_type = POOL_TYPE_FIX;
		pool->pool_base = base;
		pool->pool_size = size;
		pool->caller = (GL_UINT32)__builtin_return_address(0);
		pool->node_list = NULL;
		pool->blocksize = blocksize;
		pool->blocksize_aligned = (blocksize + 3) & 0xFFFFFFFC;
		pool->blocksize_real = pool->blocksize_aligned + (SIZEOF_FENCE_HEAD + SIZEOF_FENCE_TAIL);

		p = p + pool->pool_size - MEM_FENCE_POOL_HEAD_SIZE;
		p = (unsigned char *)(((GL_UINT32)p + 3) & ~3);
		*(GL_UINT32 *)p = (GL_UINT32)pool;

		cyg_scheduler_lock();
		mem_fence_insert_pool((mem_fence_pool *)pool);
		cyg_scheduler_unlock();

		__real_cyg_mempool_fix_create(pool->pool_base, pool->pool_size - MEM_FENCE_POOL_HEAD_SIZE, pool->blocksize_real, handle, fix);
	}
	fence_printf("fence create fixed pool (caller=0x%08X): handle=0x%08X, base=0x%08X, size=%d, blksize=%d, real_blksize=%d\n",
				 (GL_UINT32)__builtin_return_address(0),
				 *handle,
				 (GL_UINT32)pool->pool_base,
				 pool->pool_size,
				 pool->blocksize,
				 pool->blocksize_real);
}

void __wrap_cyg_mempool_fix_delete(cyg_handle_t fixpool)
{
	mem_fence_pool *pool = NULL;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(fixpool);

	if (0 != _fix_get_fence_pool(fixpool, &pool)) {
		__real_cyg_mempool_fix_delete(fixpool);
		return;
	}

	fence_printf("fence delete fixed pool (caller=0x%08X, fence alloc caller=0x%08X): ptr=0x%08X, size=%d\n",
				 (GL_UINT32)__builtin_return_address(0),
				 (GL_UINT32)pool->caller, (GL_UINT32)fixpool, pool->pool_size);

	cyg_scheduler_lock();
	mem_fence_delete_pool(pool, (GL_UINT32)__builtin_return_address(0));
	cyg_scheduler_unlock();

	free(pool);
	__real_cyg_mempool_fix_delete(fixpool);
}

void *__wrap_cyg_mempool_fix_alloc(cyg_handle_t fixpool)
{
	mem_fence_pool_fix *pool = NULL;
	unsigned char *p;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(fixpool);

	if (0 != _fix_get_fence_pool(fixpool, (mem_fence_pool **)&pool)) {
		return __real_cyg_mempool_fix_alloc(fixpool);
	}

	p = __real_cyg_mempool_fix_alloc(fixpool);
	if (NULL != p) {
		p = fix_mempool_alloc_inner(pool, p, (GL_UINT32)__builtin_return_address(0));
		fence_printf("fence fixed pool alloc (caller=0x%08X): ptr=0x%08X\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p);
	} else {
		p = malloc(pool->blocksize);
	}

	return p;
}

void *__wrap_cyg_mempool_fix_try_alloc(cyg_handle_t fixpool)
{
	mem_fence_pool_fix *pool = NULL;
	unsigned char *p;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(fixpool);

	if (0 != _fix_get_fence_pool(fixpool, (mem_fence_pool **)&pool)) {
		return __real_cyg_mempool_fix_try_alloc(fixpool);
	}

	p = __real_cyg_mempool_fix_try_alloc(fixpool);
	if (NULL != p) {
		p = fix_mempool_alloc_inner(pool, p, (GL_UINT32)__builtin_return_address(0));
		fence_printf("fence fixed pool try alloc (caller=0x%08X): ptr=0x%08X\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p);
	} else {
		p = malloc(pool->blocksize);
	}

	return p;
}

void *__wrap_cyg_mempool_fix_timed_alloc(cyg_handle_t fixpool, cyg_tick_count_t abstime)
{
	mem_fence_pool_fix *pool = NULL;
	unsigned char *p;

	GL_CHECK_BLOCK_IN_ISR();

	CHECK_INVALID_HANDLE(fixpool);

	if (0 != _fix_get_fence_pool(fixpool, (mem_fence_pool **)&pool)) {
		return __real_cyg_mempool_fix_timed_alloc(fixpool, abstime);
	}

	p = __real_cyg_mempool_fix_timed_alloc(fixpool, abstime);
	if (NULL != p) {
		p = fix_mempool_alloc_inner(pool, p, (GL_UINT32)__builtin_return_address(0));
		fence_printf("fence fixed pool timed alloc (caller=0x%08X): ptr=0x%08X\n",
					 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)p);
	} else {
		p = malloc(pool->blocksize);
	}

	return p;
}

void __wrap_cyg_mempool_fix_free(cyg_handle_t fixpool, void *ptr)
{
	unsigned char *p;
	mem_fence_head *fence_head;
	mem_fence_tail *fence_tail;
	mem_fence_pool *pool;

	GL_CHECK_BLOCK_IN_ISR();

	if ((NULL == ptr) || (0 != _fix_get_fence_pool(fixpool, &pool))) {
		__real_cyg_mempool_fix_free(fixpool, ptr);
		return;
	}

	p = (unsigned char *)ptr;
	fence_head = (mem_fence_head *)(p - SIZEOF_FENCE_HEAD);
	fence_tail = (mem_fence_tail *)(p + fence_head->align_size);

	/*
	 * The previous memory allocation was failed in the memory pool, so
	 * we allocated from heap then.
	 */
	// if (NODE_TYPE_FENCE == fence_head->type) {
	if (NULL != fence_head->tinfo) {
		free(ptr);
		return;
	}

	fence_printf("fence pool free (caller=0x%08X, fence alloc caller=0x%08X): "
				 "handle=0x%08X, ptr=0x%08X\n",
				 (GL_UINT32)__builtin_return_address(0), (GL_UINT32)fence_head->caller,
				 fixpool, (GL_UINT32)ptr);

	MEM_FENCE_CHECK(ptr, fence_head, fence_tail, (GL_UINT32)__builtin_return_address(0));

	// let further free operations fail
	fence_head->fence[0] = 0;
	fence_tail->fence[0] = 0;

	cyg_scheduler_lock();
	DLIST_REMOVE(pool->node_list, (mem_fence_node *)fence_head);
	cyg_scheduler_unlock();

	__real_cyg_mempool_fix_free(fixpool, ptr);
}
#endif
#endif //#if GL_OBJECT_MEM_FENCE /*junxian add define of GL_OBJECT_MEM_FENCE*/

