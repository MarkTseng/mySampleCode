#include <gsl.h>
#include "gl_common.h"

#ifdef GL_OBJECT_LIST_MEMPOOL
static gsl_list_t mempool_var_dump_list = { .next = NULL };
static gsl_list_t mempool_fix_dump_list = { .next = NULL };
#endif

#define MEMPOOL_NODE_MAGIC 0xF1234567

/*
 * Be aware of that var_align should NOT be less than
 * sizeof(gsl_mem_var_node_t). It will cause problems
 * in variable memory pool algorithm.
 */
static const int var_align = 32;

#define GL_MEMPOOL_INNER_ALLOC	GL_MemAlloc
#define GL_MEMPOOL_INNER_FREE 	GL_MemFree

static inline unsigned int var_roundup(unsigned int size)
{
	size += sizeof(gsl_mem_var_node_t);
	size = (size + (var_align - 1)) & -var_align;
	return size;
}

GL_Status_t GL_MempoolVarCreate(char *szName, void *pos, GL_UINT32 size, GL_MEMPOOL_t *pMempool)
{
    gsl_mempool_var_t *pool = NULL;
    gsl_mem_var_node_t *initnode = NULL;
    GL_UINT32 realsize, realstartpos;
    int ret =  0;

    if ((pos == NULL) || (size == 0) || (pMempool == NULL)) {
        return GL_FAILURE;
    }

    pool = GL_MEMPOOL_INNER_ALLOC(sizeof(gsl_mempool_var_t));
    if (pool == NULL) {
        return GL_FAILURE;
    }

	realstartpos = var_roundup((UINT32)pos) - sizeof(gsl_mem_var_node_t);
	unsigned int top;
	top = (((UINT32)pos + size + sizeof(gsl_mem_var_node_t)) & -var_align) - sizeof(gsl_mem_var_node_t);
	realsize = top - realstartpos;
    //GL_TraceInfo("[%s] pos=0x%x, realtartpos=0x%x, size=%d, realsize=%d\r\n",
    //                      __FUNCTION__, pos, realstartpos, size, realsize);

    ret = pthread_mutex_init(&(pool->mutex), NULL);
    if (ret != 0) {
        GL_TraceError("[%s] init mutex fail!\r\n", __FUNCTION__);
        goto __GL_MempoolVarCreate_ERROR;
    }

    ret = pthread_cond_init(&(pool->cond), NULL);
    if (ret != 0) {
        GL_TraceError("[%s] init condition valible fail!\r\n", __FUNCTION__);
        pthread_mutex_destroy(&(pool->mutex));
        goto __GL_MempoolVarCreate_ERROR;
    }

    pool->positon = pos;
    pool->mempool = (GL_UINT32*)realstartpos;
    pool->poolsize = realsize;

    /*can malloc same var manager node at create, such as 64*/
	initnode = (gsl_mem_var_node_t *)realstartpos;
    initnode->size = realsize;
    initnode->prev = &pool->pseudohead;
    initnode->next = &pool->pseudohead;

    pool->pseudohead.prev = pool->pseudohead.next = initnode;
	pool->pseudohead.size = 0;
	pool->freemem = initnode->size;
    pool->nodehead = initnode;

    gsl_copy_name(pool->name, szName);

    *pMempool = (GL_MEMPOOL_t)pool;

#ifdef GL_OBJECT_LIST_MEMPOOL
    gsl_insert_list(&mempool_var_dump_list, pool);
#endif
    return GL_SUCCESS;

__GL_MempoolVarCreate_ERROR:
    if(pool){
        GL_MEMPOOL_INNER_FREE(pool);
    }

    return GL_FAILURE;
}

void GL_MempoolVarDestroy(GL_MEMPOOL_t pMempool)
{
    gsl_mempool_var_t *pool;

    if (pMempool == NULL) {
        return;
    }

    pool = (gsl_mempool_var_t *)pMempool;

    //VAR_DELETE(pool->handle);
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->cond));

#ifdef GL_OBJECT_LIST_MEMPOOL
    gsl_remove_list(&mempool_var_dump_list, pool);
#endif
    GL_MEMPOOL_INNER_FREE(pool);
}

static inline gsl_mem_var_node_t* gsl_mempool_var_get_allocnode(gsl_mempool_var_t *pool, GL_UINT32 size)
{
	gsl_mem_var_node_t *head = &pool->pseudohead;
    gsl_mem_var_node_t *tmpnode = head;
    gsl_mem_var_node_t *allocnode = NULL;

	size = var_roundup(size);
	do {
		tmpnode = tmpnode->next;
        if (0 == tmpnode->size) {
			return NULL;
        }
    } while (tmpnode->size < size);

	if (size != tmpnode->size) {
		tmpnode->size -= size;
		allocnode = (gsl_mem_var_node_t *)((char *)tmpnode + tmpnode->size);
	} else {
        tmpnode->prev->next = tmpnode->next;
		tmpnode->next->prev = tmpnode->prev;
		allocnode = tmpnode;
	}
	allocnode->size = size;
	allocnode->next = allocnode->prev = (gsl_mem_var_node_t *)MEMPOOL_NODE_MAGIC;
	pool->freemem -= size;

    return allocnode;
}

void *GL_MempoolVarAlloc(GL_MEMPOOL_t pMempool, GL_UINT32 size, GL_UINT32 sdTimeout)
{
    void *pmem = NULL;
    gsl_mempool_var_t *pool;
    gsl_mem_var_node_t* allocnode;
    GL_UINT32 allocsize;
    int ret = 0;

    if ((pMempool == NULL) || (size == 0)) {
        GL_TraceError("[%s] error params: pMempool=0x%x, size=%d\r\n", __FUNCTION__, (GL_UINT32)pMempool, size);
        return NULL;
    }
    pool = (gsl_mempool_var_t *)pMempool;

    pthread_mutex_lock(&(pool->mutex));

	allocsize = (size+ (var_align - 1)) & -var_align;

    if (sdTimeout == GL_INFINITE_WAIT) {
    	GL_SET_BLOCK_INFO(GL_BLOCK_MEMPOOL_VAR, pMempool);
        while(NULL == (allocnode = gsl_mempool_var_get_allocnode(pool, allocsize))){
            ret = pthread_cond_wait(&(pool->cond), &(pool->mutex));
        }
		GL_DEL_BLOCK_INFO(pMempool);
    } else {
        struct timespec tm;

        clock_gettime(CLOCK_REALTIME, &tm);
        tm.tv_sec += sdTimeout/1000;
        tm.tv_nsec += (sdTimeout%1000)*1000000;
        if(tm.tv_nsec >= 1000000000){
            tm.tv_nsec -= 1000000000;
            tm.tv_sec++;
        }
        while(NULL == (allocnode = gsl_mempool_var_get_allocnode(pool, allocsize))){
			if (sdTimeout != 0)
				GL_SET_BLOCK_INFO(GL_BLOCK_MEMPOOL_VAR, pMempool);
            ret = pthread_cond_timedwait(&(pool->cond), &(pool->mutex), &tm);
			if (sdTimeout != 0)
				GL_DEL_BLOCK_INFO(pMempool);
            if(ret != 0){
                break; //time out;
            }
        }
    }

    if (0 != ret) {
        pmem = NULL;
        goto __GL_MempoolVarAlloc_ERROR;
    }
	pmem = (void *)((char *)allocnode + sizeof(gsl_mem_var_node_t));

__GL_MempoolVarAlloc_ERROR:
    pthread_mutex_unlock(&(pool->mutex));

  return pmem;
}

static inline gsl_mem_var_node_t *gsl_mempool_var_get_pnode(gsl_mem_var_node_t* head, GL_UINT32* addr)
{
	return (gsl_mem_var_node_t *)((char *)addr - sizeof(gsl_mem_var_node_t));
}

void GL_MempoolVarFree(GL_MEMPOOL_t pMempool, void *p)
{
    gsl_mempool_var_t *pool;
    gsl_mem_var_node_t *pnode;

    if ((pMempool == NULL) || (p == NULL) || ( ((GL_UINT32)p &0x03) != 0 )) {
        GL_TraceError("[%s] error params: pMempool=0x%x, p=0x%x\r\n", __FUNCTION__, (GL_UINT32)pMempool, (GL_UINT32)p);
        return;
    }

    pool = (gsl_mempool_var_t *)pMempool;

    pthread_mutex_lock(&(pool->mutex));

	pnode = gsl_mempool_var_get_pnode(pool->nodehead, (GL_UINT32*)p);
	if (((unsigned int)pnode->next != MEMPOOL_NODE_MAGIC)
		|| ((unsigned int)pnode->prev != MEMPOOL_NODE_MAGIC)) {
		GL_TraceError("[%s] wrong mempoolvar pointer 0x%08X\n", __FUNCTION__, (unsigned int)p);
		goto  __GL_MempoolVarFree_ERROR;
	}

	/* Insert the node into free node list.  */
	{
		gsl_mem_var_node_t *head = &pool->pseudohead;
		gsl_mem_var_node_t *tmp = head;

		do {
			tmp = tmp->next;
			if (tmp > pnode) {
				break;
			};
		} while (tmp != head);

		pnode->next = tmp;
		pnode->prev = tmp->prev;
		pnode->prev->next = pnode;
		tmp->prev = pnode;

		pool->freemem += pnode->size;

		if ((pnode->next != head)
			&& (((char *)pnode + pnode->size) == (char *)pnode->next)) {
			pnode->size += pnode->next->size;
			pnode->next = pnode->next->next;
			pnode->next->prev = pnode;
		}
		if ((pnode->prev != head)
			&& (((char *)pnode->prev + pnode->prev->size) == (char *)pnode)) {
			pnode->prev->size += pnode->size;
			pnode->prev->next = pnode->next;
			pnode->next->prev = pnode->prev;
			pnode = pnode->prev;
		}
	}
    pthread_mutex_unlock(&(pool->mutex));
    pthread_cond_broadcast(&(pool->cond));
    return;

__GL_MempoolVarFree_ERROR:
    pthread_mutex_unlock(&(pool->mutex));
    return;
}

void GL_MempoolVarInfo(GL_MEMPOOL_t pMempool)
{
    gsl_mempool_var_t *pool = (gsl_mempool_var_t *)pMempool;
    gsl_mem_var_node_t *tmpnode;
    GL_INT32 i = 0;

    if (pMempool == NULL) {
        GL_TraceError("[%s] error params: pMempool=0x%x\r\n", __FUNCTION__, (GL_UINT32)pMempool);
        return;
    }

    GL_TraceInfo("valpoolname=%s, position=0x%x, mempool=0x%x, poolsize=%d, freemem=%d\r\n",
        pool->name, (GL_UINT32)pool->positon, (GL_UINT32)pool->mempool, pool->poolsize, pool->freemem);
    GL_TraceInfo("The node list is as following:\r\n");
    GL_TraceInfo("%8s %8s %8s %8s\n", "number", "addr", "size", "isused");

    tmpnode = &pool->pseudohead;
    do {
		tmpnode = tmpnode->next;
		if (tmpnode == &pool->pseudohead) {
			break;
		}
        GL_TraceInfo("%8d %8X %8d\n", i++, (GL_UINT32)tmpnode, tmpnode->size);
    } while (1);
}

GL_Status_t GL_MempoolFixCreate(char *szName, void *pos, GL_UINT32 size, GL_UINT32 blocksize, GL_MEMPOOL_t *pMempool)
{
    gsl_mempool_fix_t *pool=NULL;
    GL_UINT8 *bitmap = NULL;
    GL_UINT32 totalblocks, alignedblocksize, realsize, realstartpos;
    int err = 0;

    if ((pos == NULL) || (size == 0) || (pMempool == NULL)) {
        return GL_FAILURE;
    }

    pool = GL_MEMPOOL_INNER_ALLOC((sizeof(gsl_mempool_fix_t) + GL_HAL_DCACHE_LINE_SIZE) & ~(GL_HAL_DCACHE_LINE_SIZE - 1));
    if (pool == NULL) {
        GL_TraceError("[%s] alloc gsl_mempool_fix_t fail!\r\n", __FUNCTION__);
        return GL_FAILURE;
    }

    //FIX_CREATE(pos, size, blocksize, &pool->handle, &pool->fix);
    realstartpos = (((GL_UINT32)pos+3)>>2)<<2;
    realsize = size - (realstartpos - (GL_UINT32)pos);
    alignedblocksize = ((blocksize+3)>>2)<<2;
    totalblocks = (realsize/alignedblocksize);

    //GL_TraceInfo("[%s] pos=0x%x, realtartpos=0x%x, size=%d, realsize=%d, blocksize=%d, alignedblocksize =%d, totalblocks=%d\r\n",
    //    __FUNCTION__, pos, realstartpos, size, realsize, blocksize, alignedblocksize, totalblocks);

    bitmap = (GL_UINT8*)GL_MEMPOOL_INNER_ALLOC(sizeof(GL_UINT8) * ((totalblocks + 7) /8));
    if (bitmap == NULL) {
        GL_TraceError("[%s] alloc bitmap fail!\r\n", __FUNCTION__);
        goto __GL_MempoolFixCreate_ERROR;
    }

    err = pthread_mutex_init(&(pool->mutex), NULL);
    if (err != 0) {
        GL_TraceError("[%s] init mutex fail!\r\n", __FUNCTION__);
        goto __GL_MempoolFixCreate_ERROR;
    }

    err = pthread_cond_init(&(pool->cond), NULL);
    if (err != 0) {
        GL_TraceError("[%s] init condition valible fail!\r\n", __FUNCTION__);
         pthread_mutex_destroy(&(pool->mutex));
        goto __GL_MempoolFixCreate_ERROR;
    }

    pool->bitmap = bitmap;
	memset(pool->bitmap, 0, sizeof(GL_UINT8) * ((totalblocks + 7) / 8));
    pool->positon = pos;
    pool->mempool = (GL_UINT32*)realstartpos;
    pool->blocksize = alignedblocksize;
    pool->firstfree = 0;
    pool->freeblocks = totalblocks;
    pool->numblocks = totalblocks;
    gsl_copy_name(pool->name, szName);

    *pMempool = (GL_MEMPOOL_t)pool;

    #ifdef GL_OBJECT_LIST_MEMPOOL
    gsl_insert_list(&mempool_fix_dump_list, pool);
    #endif

    return GL_SUCCESS;

__GL_MempoolFixCreate_ERROR:

    if(bitmap){
        GL_MEMPOOL_INNER_FREE(bitmap);
    }

    if(pool){
        GL_MEMPOOL_INNER_FREE(pool);
    }

    return GL_FAILURE;
}

void GL_MempoolFixDestroy(GL_MEMPOOL_t pMempool)
{
    gsl_mempool_fix_t *pool;

    if (pMempool == NULL) {
        return;
    }

    pool = (gsl_mempool_fix_t *)pMempool;

    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->cond));

    //FIX_DELETE(pool->handle);
    if(pool->bitmap){
        GL_MEMPOOL_INNER_FREE(pool->bitmap);
    }

#ifdef GL_OBJECT_LIST_MEMPOOL
    gsl_remove_list(&mempool_fix_dump_list, pool);
#endif
    GL_MEMPOOL_INNER_FREE(pool);
}

void *GL_MempoolFixAlloc(GL_MEMPOOL_t pMempool, GL_UINT32 sdTimeout)
{
    gsl_mempool_fix_t *pool;
    void *pmem = NULL;
    GL_UINT8 *bitmap;
    GL_UINT32 tmpfree, totalblocks;
    int ret = 0;

    if (pMempool == NULL) {
        return NULL;
    }
    pool = (gsl_mempool_fix_t *)pMempool;

    pthread_mutex_lock(&(pool->mutex));

    if (sdTimeout == GL_INFINITE_WAIT) {
    	GL_SET_BLOCK_INFO(GL_BLOCK_MEMPOOL_VAR, pMempool);
        while(0 == pool->freeblocks ){
            ret = pthread_cond_wait(&(pool->cond), &(pool->mutex));
        }
		GL_DEL_BLOCK_INFO(pMempool);
    }else{
        struct timespec tm;

        clock_gettime(CLOCK_REALTIME, &tm);
        tm.tv_sec += sdTimeout/1000;
        tm.tv_nsec += (sdTimeout%1000)*1000000;
        if(tm.tv_nsec >= 1000000000){
            tm.tv_nsec -= 1000000000;
            tm.tv_sec++;
        }
        while(0 == pool->freeblocks ){
			if (sdTimeout != 0)
				GL_SET_BLOCK_INFO(GL_BLOCK_MEMPOOL_VAR, pMempool);
            ret = pthread_cond_timedwait(&(pool->cond), &(pool->mutex), &tm);
			if (sdTimeout != 0)
				GL_DEL_BLOCK_INFO(pMempool);
            if(ret != 0){
                break; //time out;
            }
        }
    }

    if(0 != ret){
        pmem = NULL;
        goto __GL_MempoolFixAlloc_ERROR;
    }

    pmem =(void*)((GL_UINT32)pool->mempool + (pool->firstfree * pool->blocksize));

    tmpfree = pool->firstfree;
    bitmap = pool->bitmap;
    bitmap[tmpfree/8] |= (0x80 >> (tmpfree%8));

    totalblocks = pool->numblocks;
    while(bitmap[tmpfree/8] & (0x80 >> (tmpfree%8))){
        tmpfree ++;
        if(tmpfree>=totalblocks){
            break;
        }
    }

    pool->firstfree = tmpfree;
    pool->freeblocks --;

__GL_MempoolFixAlloc_ERROR:

    pthread_mutex_unlock(&(pool->mutex));
    return pmem;

}

void GL_MempoolFixFree(GL_MEMPOOL_t pMempool, void *p)
{
    gsl_mempool_fix_t *pool;
    GL_UINT8 *bitmap;
    GL_UINT32 tmpfree;

	if (pMempool == NULL) {
		return;
	}

    pool = (gsl_mempool_fix_t *)pMempool;
    if ((p == NULL) || ( ((GL_UINT32)p &0x03) != 0 )) {
        GL_TraceError("[%s] error parms: p=0x%x, pMempool=0x%x\r\n", __FUNCTION__, (GL_UINT32)p, (GL_UINT32)pool->mempool);
        return;
    }

    //FIX_FREE(pool->handle, p);
    pthread_mutex_lock(&(pool->mutex));

    if ((GL_UINT32*)p < pool->mempool) {
        GL_TraceError("[%s] error, free pointer overflow down!\r\n", __FUNCTION__);
        goto __GL_MempoolFixFree_ERROR;
    }

    tmpfree = ((GL_UINT32)p - (GL_UINT32)pool->mempool) / pool->blocksize;
    if (tmpfree >= pool->numblocks) {
        GL_TraceError("[%s] error, free pointer overflow up!\r\n", __FUNCTION__);
        goto __GL_MempoolFixFree_ERROR;
    }

    if(tmpfree == pool->firstfree){
        GL_TraceError("[%s] error, free pointer has been freed!\r\n", __FUNCTION__);
        goto __GL_MempoolFixFree_ERROR;
    }

    bitmap = pool->bitmap;
    while(0 == (bitmap[tmpfree/8] & (0x80 >> (tmpfree%8)))){
        GL_TraceError("[%s] error, free pointer has been freed!\r\n", __FUNCTION__);
        goto __GL_MempoolFixFree_ERROR;
    }

    bitmap[tmpfree/8] &= (~(0x80 >> (tmpfree%8)));
    pool->freeblocks ++;

    if(tmpfree < pool->firstfree){
        pool->firstfree = tmpfree;
    }

    pthread_mutex_unlock(&(pool->mutex));
    pthread_cond_broadcast(&(pool->cond));
    return;

__GL_MempoolFixFree_ERROR:

    pthread_mutex_unlock(&(pool->mutex));

}

void GL_MempoolFixInfo(GL_MEMPOOL_t pMempool)
{
    gsl_mempool_fix_t *pool;

    if (pMempool == NULL) {
        return;
    }

    pool = (gsl_mempool_fix_t *)pMempool;
    GL_TraceInfo("fixpoolname=%s, position=0x%x\r\n", pool->name, (GL_UINT32)pool->positon);
    GL_TraceInfo("%26s %10s %8s %8s %8s %10s\n", "Name", "Base", "TotalBlk", "FreeBlk", "BlkSize", "Firstfree");
    GL_TraceInfo("%26s 0x%08X %8d %8d %8d %10d\n", pool->name, (GL_UINT32)pool->mempool,
    pool->numblocks, pool->freeblocks, pool->blocksize, pool->firstfree);
}

#if 0 //#ifdef GL_OBJECT_LIST_MEMPOOL
UINT8 GL_MempoolVarIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_mempool_var_t *v;

	cyg_scheduler_lock();
	for (p = &mempool_var_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		v = (gsl_mempool_var_t *)p->next;
		if (0 == strncmp(name, v->name, GSL_NAME_LEN)) {
			cyg_scheduler_unlock();
			return 1;
		}
	}
	cyg_scheduler_unlock();
	return 0;
}

UINT8 GL_MempoolFixIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_mempool_fix_t *f;

	cyg_scheduler_lock();
	for (p = &mempool_fix_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		f = (gsl_mempool_fix_t *)p->next;
		if (0 == strncmp(name, f->name, GSL_NAME_LEN)) {
			cyg_scheduler_unlock();
			return 1;
		}
	}
	cyg_scheduler_unlock();
	return 0;
}

void GL_MempoolVarDumpList(UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_mempool_var_t *v;

	wChannel = wChannel;
	diag_printf("-------GSL MempoolVar dump list start ------------\n");
	diag_printf("%26s %10s %10s %8s %8s\n", "Name", "GSL Handle", "Base", "Size", "FreeMem");
	for (p = &mempool_var_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		v = (gsl_mempool_var_t *)p->next;
		diag_printf("%26s 0x%08X 0x%08X %8d %8d\n", v->name, (UINT32)v, (UINT32)v->var.obase,
					v->var.osize, v->var.freemem);
	}
	diag_printf("-------GSL MempoolVar dump list end  ------------\n");
}

void GL_MempoolFixDumpList(UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_mempool_fix_t *f;

	wChannel = wChannel;
	diag_printf("-------GSL MempoolFix dump list start ------------\n");
	diag_printf("%26s %10s %10s %8s %8s %8s\n", "Name", "GSL Handle", "Base", "TotalBlk", "FreeBlk", "BlkSize");
	for (p = &mempool_fix_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		f = (gsl_mempool_fix_t *)p->next;
		diag_printf("%26s 0x%08X 0x%08X %8d %8d %8d\n", f->name, (UINT32)f, (UINT32)f->fix.mempool,
					f->fix.numblocks, f->fix.freeblocks, f->fix.blocksize);
	}
	diag_printf("-------GSL MempoolFix dump list end  ------------\n");
}
#endif

#if 0
struct mempoolvar_test {
	int id;
};

#define NUM_TASK 30
#define DEBUG(fmt, arg...) printf("%s:%d: "fmt, __FUNCTION__, __LINE__, ##arg)

static const int pool_size = 32 << 10;
static const int maximum_alloc_size = 16 << 10;

static GL_MEMPOOL_t varpool;
static void *pool_base;
static GL_Task_t var_handle_main;
static GL_Task_t var_handles[NUM_TASK];
static struct mempoolvar_test var_struct_test[NUM_TASK];
static int thread_count = 0;
static volatile int done = 0;

void test_mempool_var(void *data)
{
	struct mempoolvar_test *test = (struct mempoolvar_test *)data;
    gsl_mempool_var_t *pool = (gsl_mempool_var_t *)varpool;
	int i;

	// random delay
	GL_TaskSleep(rand() % 100);

	printf("Thread id %d activate!\n", test->id);

	for (i = 0; i < 1024; ++i) {
		int size;
		void *p;
		int j;

		if ((test->id & 0x01) == 1) {
			// odd
			size = (rand() % (maximum_alloc_size >> 1)) + (maximum_alloc_size >> 1);

		} else {
			// even
			size = rand() % maximum_alloc_size;
			if (size < 4) {
				size = 4;
			}
		}
		p = GL_MempoolVarAlloc(varpool, size, GL_INFINITE_WAIT);

		if ((p < pool_base) || (p > (void *)((char *)pool_base + pool_size))) {
			DEBUG("Thread %d: Bad pointer! 0x%08X\n", test->id, (unsigned int)p);
			exit(-1);
		}

		if (NULL != p) {
			char *c = (char *)p;

			memset(p, test->id, size);

			/* To give chances that other threads may overwrite memory.  */
			GL_TaskSleep(5);

			for (j = 0; j < size; ++j) {
				if (c[j] != (char)test->id) {
					DEBUG("Thread %d: FAIL!!\n", test->id);
				}
			}
			GL_MempoolVarFree(varpool, p);
		} else {
			DEBUG("Thread %d: Cannot allocate memory!\n", test->id);
		}
	}
	DEBUG("Thread %d: DONE! pool size = %d\n", test->id, pool->freemem);
	__sync_fetch_and_add(&thread_count, 1);
}

void test_mempool_var_main(void *data)
{
	GL_Status_t ret;
	int i;

	srand(time(NULL));

	pool_base = malloc(pool_size);
	if (NULL == pool_base) {
		DEBUG("Create mempool base failed!\n");
		return;
	}
	DEBUG("Poolbase = 0x%08X, size = %d\n", (unsigned int)pool_base, pool_size);
	ret = GL_MempoolVarCreate("VarTest", pool_base, pool_size, &varpool);
	if (GL_SUCCESS != ret) {
		DEBUG("Create mempool failed!\n");
		return;
	}

	GL_MempoolVarInfo(varpool);
	for (i = 0; i < NUM_TASK; ++i) {
		var_struct_test[i].id = i;
		ret = GL_TaskCreate("VAR_TEST", test_mempool_var, &var_struct_test[i], 1, 4096,
							true, &var_handles[i]);
		if (GL_SUCCESS != ret) {
			DEBUG("Create mempool test thread failed!\n");
		}
	}

	while (thread_count != NUM_TASK) {
		sleep(1);
	}

	GL_MempoolVarInfo(varpool);
	GL_MempoolVarDestroy(varpool);
	free(pool_base);
	system("free");
	done = 1;
}

void GL_MempoolVarTest(void)
{
	GL_Status_t ret;

	ret = GL_TaskCreate("VAR_TEST_MAIN", test_mempool_var_main, NULL, 0, 4096,
						true, &var_handle_main);
	if (GL_SUCCESS != ret) {
		DEBUG("Create mempool failed!\n");
		return;
	}

	while (done != 1) {
		sleep(1);
	}
	printf("test finised! ----------------\n");
	return;
}
#endif
