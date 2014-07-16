#include <gsl.h>
#include "gl_common.h"

#define GL_MESSAGE_LIST_ELEMINCREASE	1
#define GL_MESSAGE_LIST_MAGICKEY		0x4D53474C	//ASCII "MSGL"
#define GL_MESSAGE_LIST_CHECKMAGIC(pt)	((pt)->magickey == GL_MESSAGE_LIST_MAGICKEY)
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define SAFE_ADD_ONE(pCount)		++(pCount)
#define SAFE_SUB_ONE(pCount)		--(pCount)


#if GL_OBJECT_LIST_LIST
static gsl_list_t messagelist_dump_list = { .next = NULL };
#endif

typedef struct gsl_message_elem {
	void *item;
	void *next;
} gsl_message_elem_t;

typedef struct gsl_message_list_elem {
	gsl_message_elem_t *head;
	gsl_message_elem_t *tail;
	//GL_MUTEX_T lock; //use the lock in gsl_message_list in top layer
#if GL_DEBUG
	GL_UINT32 count;
#endif
} gsl_message_list_elem_t;

typedef struct gsl_message_list {
#if GL_OBJECT_LIST_LIST
	void *next;
#endif
	char name[GSL_NAME_LEN + 1];
	GL_UINT32 dElemSize;
	GL_UINT32 magickey;

	gsl_message_list_elem_t *pFreeList;
	gsl_message_list_elem_t *pNormalList;
	gsl_message_list_elem_t *pUrgentList;

	GL_SEM_T sem;
	GL_MUTEX_T lock;

} gsl_message_list_t;


static void gl_list_init(gsl_message_list_elem_t **list)
{
	*list = (gsl_message_list_elem_t *)malloc(sizeof(gsl_message_list_elem_t));
	if(*list == NULL){
		GL_TraceError("gl_list_init malloc error!\n");
		//return;
	}

	(*list)->head = NULL;
	(*list)->tail = NULL;

#if (GL_LIST_COUNTER == 1)
	(*list)->count = 0;
#endif
}

static void gl_list_destroy(gsl_message_list_elem_t **list)
{
	gsl_message_elem_t *pitem;
	gsl_message_elem_t *ptemp;

	if (*list == NULL) {
		GL_TraceError("gl_list_destroy list is null!\n");
		//return GL_FAILURE;
	}

	pitem = (*list)->head;
	while (pitem != NULL) {
	    ptemp = pitem;
	    pitem = pitem->next;
	    free(ptemp);
	}

	free(*list);
	*list = NULL;
}

static GL_Status_t gsl_list_pophead(gsl_message_list_elem_t *list, gsl_message_elem_t **head)
{
	gsl_message_elem_t* ptemp;

	if (list == NULL) {
		GL_TraceError("gsl_list_pophead list is null!\n");
		//return GL_FAILURE;
	}

	if (list->head == NULL) {  /*list is empty*/
		return GL_FAILURE;
	}

	ptemp = list->head;
	list->head = ptemp->next;
	ptemp->next = NULL;
	*head = ptemp;

	if (list->head == NULL) {
		list->tail = NULL;
	}

#if (GL_LIST_COUNTER == 1)
	SAFE_SUB_ONE(list->count);
#endif

	return GL_SUCCESS;
}

static void gl_list_pushtail(gsl_message_list_elem_t *list, gsl_message_elem_t *elem)
{
	if (list == NULL) {
		GL_TraceError("gl_list_pushtail list is null!\n");
		//return GL_FAILURE;
	}

	elem->next = NULL;

	if(list->tail == NULL){
		list->tail = elem;
	}else{
		list->tail->next = elem;
		list->tail = elem;
	}

	if (list->head == NULL) {
		list->head = elem;
	}

#if (GL_LIST_COUNTER == 1)
	SAFE_ADD_ONE(list->count);
#endif

	return;
}

/*!
 * @brief	To get a free node from free list. If there is no free node, allocate memory for a new free node.
 *
 * @param	pt	container of gsl list
 *
 * @return	container of a free node
*/
static gsl_message_elem_t *gl_list_get_freenode(gsl_message_list_elem_t *list, GL_UINT32 size)
{
	gsl_message_elem_t *ptemp;			/* To store the original free list head */
	gsl_message_elem_t *pitem = NULL;

	if (list == NULL) {
		GL_TraceError("gl_list_get_freenode list is null!\n");
		//return GL_FAILURE;
	}

	ptemp = list->head;
	if (ptemp == NULL) {
		pitem = (gsl_message_elem_t *)malloc(sizeof(gsl_message_elem_t) + size);
		pitem->item = (GL_UINT8 *)pitem + sizeof(gsl_message_elem_t);
		pitem->next = NULL;
		/* no need to change head's value, it keeps to be NULL. */
	} else {
		list->head = ptemp->next;
		pitem = ptemp;
		pitem->next = NULL;

		if(list->head == NULL){
			list->tail = NULL;
		}

#if (GL_LIST_COUNTER == 1)
		SAFE_SUB_ONE(list->count);
#endif
	}

	return pitem;
}

GL_Status_t GL_ListCreate(char *szName, GL_UINT32 dElementSize, GL_UINT32 dNumInitElem, GL_List_t *listId)
{
	GL_UINT32 i;
	GL_Status_t ret;
	gsl_message_list_t *pt;
	gsl_message_elem_t *ptemp;


	if (!listId) {
		return GL_FAILURE;
	}

	pt = (gsl_message_list_t *)malloc(sizeof(gsl_message_list_t));
	if (pt == NULL) {
		return GL_FAILURE;
	}
	memset(pt, 0, sizeof(gsl_message_list_t));  //reset all

	memset(&pt->sem, 0, sizeof(GL_SEM_T));
	if(sem_init(&(pt->sem), 0, 0) != 0){
		free(pt);
		return GL_FAILURE;
	}

	if(pthread_mutex_init(&(pt->lock), NULL) != 0){
		sem_destroy(&(pt->sem));
		free(pt);
		return GL_FAILURE;
	}

	gl_list_init(&(pt->pFreeList));
	gl_list_init(&(pt->pNormalList));
	gl_list_init(&(pt->pUrgentList));

	if (dNumInitElem ){
		/* allocate free nodes and concatenate them */
		pt->pFreeList->head = (gsl_message_elem_t *)malloc(sizeof(gsl_message_elem_t) + dElementSize);
		if (pt->pFreeList->head == NULL) {
			ret = GL_ERR_MEMORY_ALLOC;
			goto __list_creat_err_exit;
		}
		ptemp = pt->pFreeList->head;
		for (i = 0; i < dNumInitElem - 1; ++i) {
			ptemp->item = (GL_UINT8 *)ptemp + sizeof(gsl_message_elem_t);
			ptemp->next = (gsl_message_elem_t *)malloc(sizeof(gsl_message_elem_t) + dElementSize);
			if (ptemp->next == NULL) {
				ret = GL_ERR_MEMORY_ALLOC;
				goto __list_creat_err_exit;
			}
			ptemp = ptemp->next;
		}
		ptemp->item = (GL_UINT8 *)ptemp + sizeof(gsl_message_elem_t);
		ptemp->next = NULL;
		pt->pFreeList->tail = ptemp;
	}

#if (GL_LIST_COUNTER == 1)
	pt->pFreeList->count = dNumInitElem;
#endif

	gsl_copy_name(pt->name, szName);
	pt->dElemSize = dElementSize;
	pt->magickey = GL_MESSAGE_LIST_MAGICKEY;
	*listId = (GL_List_t)((GL_UINT32)pt);

#if GL_OBJECT_LIST_LIST
	gsl_insert_list(&messagelist_dump_list, (void *)pt);
#endif

	return GL_SUCCESS;

__list_creat_err_exit:

	gl_list_destroy(&(pt->pFreeList));
	gl_list_destroy(&(pt->pNormalList));
	gl_list_destroy(&(pt->pUrgentList));

	sem_destroy(&(pt->sem));
	pthread_mutex_destroy(&(pt->lock));

	free(pt);

	return ret;
}

GL_Status_t GL_ListDelete(GL_List_t listId)
{
	gsl_message_list_t *pt = (gsl_message_list_t *)listId;

	if(NULL == pt){
		return GL_FAILURE;
	}

	if (!GL_MESSAGE_LIST_CHECKMAGIC(pt)) {
		return GL_ERR_INVALID_PARAM;
	}

	pthread_mutex_lock(&(pt->lock));
	pt->magickey = 0;

#if GL_OBJECT_LIST_LIST
	if (0 != gsl_remove_list(&messagelist_dump_list, pt)) {
		GL_TraceError("GL_ListDelete failed! line : %d\n", __LINE__);
		while (1) {continue;}
	}
#endif

	gl_list_destroy(&(pt->pFreeList));
	gl_list_destroy(&(pt->pNormalList));
	gl_list_destroy(&(pt->pUrgentList));

	sem_destroy(&(pt->sem));
	pthread_mutex_unlock(&(pt->lock));
	pthread_mutex_destroy(&(pt->lock));

	free(pt);
	return GL_SUCCESS;
}

GL_Status_t GL_ListSend(GL_List_t listId, void *data, GL_UINT32 dSize)
{
	gsl_message_list_t *pt = (gsl_message_list_t *)listId;
	gsl_message_elem_t *pitem;

	if (!GL_MESSAGE_LIST_CHECKMAGIC(pt)) {
		return GL_ERR_INVALID_PARAM;
	}

	pthread_mutex_lock(&(pt->lock));

	pitem = gl_list_get_freenode(pt->pFreeList, pt->dElemSize);
	memcpy(pitem->item, data, MIN(dSize, pt->dElemSize));

	gl_list_pushtail(pt->pNormalList, pitem);

	sem_post(&(pt->sem));

	pthread_mutex_unlock(&(pt->lock));

	return GL_SUCCESS;
}

GL_Status_t GL_ListSendUrgent(GL_List_t listId, void *data, GL_UINT32 dSize)
{
	gsl_message_list_t *pt = (gsl_message_list_t *)listId;
	gsl_message_elem_t *pitem;

	if (!GL_MESSAGE_LIST_CHECKMAGIC(pt)) {
		return GL_ERR_INVALID_PARAM;
	}

	pthread_mutex_lock(&(pt->lock));

	pitem = gl_list_get_freenode(pt->pFreeList, pt->dElemSize);
	memcpy(pitem->item, data, MIN(dSize, pt->dElemSize));

	gl_list_pushtail(pt->pUrgentList, pitem);

	sem_post(&(pt->sem));

	pthread_mutex_unlock(&(pt->lock));

	return GL_SUCCESS;
}

GL_Status_t GL_ListReceive(GL_List_t listId, void *data, GL_UINT32 dSize, GL_UINT32 timeout)
{
	gsl_message_list_t *pt = (gsl_message_list_t *)listId;
	gsl_message_elem_t *pitem = NULL;
	GL_Status_t ret = GL_FAILURE;

	if (!GL_MESSAGE_LIST_CHECKMAGIC(pt)) {
		return GL_ERR_INVALID_PARAM;
	}

__list_receive_again:

	if (timeout == GL_INFINITE_WAIT) {
		sem_wait(&(pt->sem));
	} else if (timeout == GL_NO_WAIT) {
		if (0 != sem_trywait(&(pt->sem))) {
			return GL_QUEUE_EMPTY;
		}
	} else {
		struct timespec tm;

		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += timeout/1000;
		tm.tv_nsec += (timeout%1000)*1000000;
		if(tm.tv_nsec >= 1000000000){
			tm.tv_nsec -= 1000000000;
			tm.tv_sec++;
		}

		if (0 != sem_timedwait(&(pt->sem), &tm)){
			return GL_TIMEOUT;
		}
	}

	pthread_mutex_lock(&(pt->lock));

	ret = gsl_list_pophead(pt->pUrgentList, &pitem);
	if (ret != GL_SUCCESS) {
		ret = gsl_list_pophead(pt->pNormalList, &pitem);
	}
	if (ret != GL_SUCCESS) {
		/*maybe be received by another task*/
		GL_TraceError("ERROR:empty after sem_wait in GL_ListReceive.\n");
		pthread_mutex_unlock(&(pt->lock));
		goto __list_receive_again;
	}

	memcpy(data, pitem->item, MIN(dSize, pt->dElemSize));

	/* Put the node back to free list */
	gl_list_pushtail(pt->pFreeList, pitem);

	pthread_mutex_unlock(&(pt->lock));

	return GL_SUCCESS;
}

#if GL_OBJECT_LIST_LIST
GL_UINT8 GL_ListIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_message_list_t *pt;

	GL_SchedulerLock();
	for (p = &messagelist_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		pt = (gsl_message_list_t *)p->next;
		if (0 == strncmp(name, pt->name, GSL_NAME_LEN)) {
			GL_SchedulerUnLock();
			return 1;
		}
	}
	GL_SchedulerUnLock();
	return 0;
}

void GL_ListDump(GL_UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_message_list_t *pt;
	gsl_message_elem_t *ptemp;
	wChannel = wChannel;

	GL_UINT32 free_cnt = 0;
	GL_UINT32 normal_cnt = 0;
	GL_UINT32 urgent_cnt = 0;

	for (p = &messagelist_dump_list; p->next != NULL; p = (gsl_list_t *)p->next) {
		pt = (gsl_message_list_t *)p->next;
		if (GL_MESSAGE_LIST_CHECKMAGIC(pt)) {
			GL_TraceInfo("MSGL:<%s> list\n",pt->name);
			GL_TraceInfo("Free List:   Head: 0x%08X, Tail: 0x%08X\n", (GL_UINT32)pt->pFreeList->head, (GL_UINT32)pt->pFreeList->tail);
			GL_TraceInfo("Normal List: Head: 0x%08X, Tail: 0x%08X\n", (GL_UINT32)pt->pNormalList->head, (GL_UINT32)pt->pNormalList->tail);
			GL_TraceInfo("Urgent List: Head: 0x%08X, Tail: 0x%08X\n", (GL_UINT32)pt->pUrgentList->head, (GL_UINT32)pt->pUrgentList->tail);
#if (GL_LIST_COUNTER == 1)
			free_cnt = pt->pFreeList->count;
			normal_cnt = pt->pNormalList->count;
			urgent_cnt = pt->pUrgentList->count;
#else
			free_cnt = 0;
			ptemp = pt->pFreeList->head;
			while (ptemp != NULL) {
				++free_cnt;
				ptemp = ptemp->next;
			}
			normal_cnt = 0;
			ptemp = pt->pNormalList->head;
			while (ptemp != NULL) {
				++normal_cnt;
				ptemp = ptemp->next;
			}
			urgent_cnt = 0;
			ptemp = pt->pUrgentList->head;
			while (ptemp != NULL) {
				++urgent_cnt;
				ptemp = ptemp->next;
			}
#endif
			GL_TraceInfo("Total Alloc: %d = (%d, %d, %d), Max Element Size: %d\n",
				free_cnt + normal_cnt + urgent_cnt, free_cnt, normal_cnt, urgent_cnt, pt->dElemSize);
		} else {
			GL_TraceInfo("Invalued Msglist!!\n");
		}
	}
	GL_TraceInfo("------------  GSL-MSGL dump list end  ------------\n\n");
}
#endif

