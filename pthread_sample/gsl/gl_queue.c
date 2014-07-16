#include <gsl.h>
#include "gl_common.h"

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define GSL_QUEUE_ALERT_LIMIT	5


#if GL_OBJECT_LIST_QUEUE
static gsl_list_t queue_dump_list = { .next = NULL };
#endif

/*
 * Message Queue APIs
 */

GL_Status_t GL_QueueCreate(char *szName, GL_UINT32 dElementSize, GL_UINT32 dMaxElements, GL_UINT32 dMaxUrgentElements, GL_Queue_t *pQueueId)
{
	gsl_queue_t *p_queue;
	void *p_temp;
	GL_UINT8 *msg_buffer;
	GL_UINT32 actual_msg_size;
	GL_UINT32 total_msg_max;
	GL_UINT32 i;

	if (dElementSize < 0 || dMaxElements < 0) {/*dMaxUrgentElements can be zero*/
		return GL_FAILURE;
	}

	total_msg_max = dMaxElements + dMaxUrgentElements;
	actual_msg_size = (dElementSize + 0x03) & (~0x03);	/*align to word*/

	p_temp = malloc(sizeof(gsl_queue_t) + (sizeof(MSG_ITEM) + actual_msg_size) * total_msg_max);
	if (p_temp == NULL) {
		return GL_FAILURE;
	}

	p_queue = (gsl_queue_t *)p_temp;

	if(pthread_mutex_init(&p_queue->mt,NULL) < 0){
		free(p_temp);
		return GL_FAILURE;
	}

	memset(&p_queue->read_sem, 0, sizeof(GL_SEM_T));
	if(sem_init(&p_queue->read_sem, 0, 0) < 0){
		pthread_mutex_destroy(&p_queue->mt);
		free(p_temp);
		return GL_FAILURE;
	}

	msg_buffer = (GL_UINT8 *)p_temp + sizeof(gsl_queue_t);

	/*link the message items together */
	for (i = 0; i < total_msg_max; i++) {
		MSG_ITEM *p_msg_item;
		p_msg_item = (MSG_ITEM *)(msg_buffer + i * (sizeof(MSG_ITEM) + actual_msg_size));
		p_msg_item->data = (GL_UINT8 *)((GL_UINT8 *)p_msg_item + sizeof(MSG_ITEM));

		if (i != (total_msg_max - 1)) {
			p_msg_item->p_next = (MSG_ITEM *)((GL_UINT8 *)p_msg_item
											  + sizeof(MSG_ITEM) + actual_msg_size);
		} else {
			p_msg_item->p_next = NULL;
		}
	}

	gsl_copy_name(p_queue->name, szName);
	p_queue->idle_list = (MSG_ITEM *)msg_buffer;
	p_queue->used_list = NULL;
	p_queue->used_last_item = NULL;
	p_queue->urgent_list = NULL;
	p_queue->urgent_last_item = NULL;

	p_queue->msg_size = actual_msg_size;
	p_queue->normal_msg_max = dMaxElements;
	p_queue->urgent_msg_max = dMaxUrgentElements;
	p_queue->normal_msg_used = 0;
	p_queue->urgent_msg_used = 0;

	*pQueueId = (GL_Queue_t) p_queue;

#if GL_OBJECT_LIST_QUEUE
	gsl_insert_list(&queue_dump_list, (void *)p_queue);
#endif

	return GL_SUCCESS;

}

GL_Status_t GL_QueueDelete(GL_Queue_t queueId)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)(queueId);
	GL_INT32 ret = 0;

	if (p_queue == NULL) {
		return GL_FAILURE;
	}

#if GL_OBJECT_LIST_QUEUE
	if(0 != gsl_remove_list(&queue_dump_list, p_queue)){
		GL_TraceError("GL_QueueDelete failed! line : %d\n", __LINE__);
		while (1) {continue;}
	}
#endif

	pthread_mutex_lock(&p_queue->mt);
	ret |= sem_destroy(&p_queue->read_sem);
	pthread_mutex_unlock(&p_queue->mt);

	ret |= pthread_mutex_destroy(&p_queue->mt);
	free(p_queue);

	return ret;
}

GL_Status_t GL_QueueSend(GL_Queue_t queueId, void *pBuffer, GL_UINT32 dSize)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;
	GL_Status_t ret = GL_SUCCESS;
#ifdef GSL_QUEUE_ALERT_LIMIT
	static int alert = GSL_QUEUE_ALERT_LIMIT;
#endif

	if (p_queue == NULL) {
		GL_TraceError("The queueId is incorrect!\n");
		return GL_FAILURE;
	}

	if (dSize > p_queue->msg_size) {
		GL_TraceError("The size to send is too big!\n");
		return GL_FAILURE;
	}

	pthread_mutex_lock(&p_queue->mt);

	if (p_queue->normal_msg_used >= p_queue->normal_msg_max) {
#ifdef GSL_QUEUE_ALERT_LIMIT
		if (0 > --alert) {
			alert = GSL_QUEUE_ALERT_LIMIT;
			GL_TraceError("[%s] queue normal space has full!, current/max=%d/%d\n",
						  p_queue->name,p_queue->normal_msg_used, p_queue->normal_msg_max);
		}
#else
		GL_TraceError("[%s] queue normal space has full!, current/max=%d/%d\n",
					  p_queue->name,p_queue->normal_msg_used, p_queue->normal_msg_max);
#endif
		ret = GL_QUEUE_FULL;
		goto __queue_send_exit;
	}
#ifdef GSL_QUEUE_ALERT_LIMIT
	else {
		alert = GSL_QUEUE_ALERT_LIMIT;
	}
#endif
	if (!(p_queue->idle_list)) {
		GL_TraceError("Function : %s failed! Line : %d\n", __FUNCTION__, __LINE__);
		GL_TraceError("Msg name is [%s] \n", p_queue->name);
		GL_TraceError("There are no space for more message, message_used : %d, max msg number allowed: %d\n",
				   p_queue->normal_msg_used, p_queue->normal_msg_max);
		//asm volatile ("break;");
		ret = GL_FAILURE;
		goto __queue_send_exit;
	}
	if (!(p_queue->used_last_item)) {	/*used list is empty */
		// pump a new msg item from idle list
		p_queue->used_last_item = p_queue->idle_list;
		p_queue->used_list = p_queue->idle_list;
		p_queue->idle_list = p_queue->idle_list->p_next;
		p_queue->used_last_item->p_next = NULL;
	} else {
		// pump a new msg item from idle list and put it into used list tail
		p_queue->used_last_item->p_next = p_queue->idle_list;
		p_queue->idle_list = p_queue->idle_list->p_next;
		p_queue->used_last_item = p_queue->used_last_item->p_next;
		p_queue->used_last_item->p_next = NULL;
	}
	// copy the data to the new msg item
	memcpy(p_queue->used_last_item->data, pBuffer, dSize);
	p_queue->used_last_item->is_urgent = 0;
	p_queue->normal_msg_used++;

	sem_post(&p_queue->read_sem);

__queue_send_exit:


	pthread_mutex_unlock(&p_queue->mt);
	return ret;
}

GL_Status_t GL_QueueSendUrgent(GL_Queue_t queueId, void *pBuffer, GL_UINT32 dSize)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;
	MSG_ITEM *p_temp;
	GL_Status_t ret = GL_SUCCESS;
#ifdef GSL_QUEUE_ALERT_LIMIT
	static int alert = GSL_QUEUE_ALERT_LIMIT;
#endif

	if (GL_IS_IN_INTERRUPT() != 0) {
		return GL_FAILURE;
	}
	if (p_queue == NULL) {
		return GL_FAILURE;
	}
	if (dSize > p_queue->msg_size) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&p_queue->mt);
	if (p_queue->urgent_msg_used >= p_queue->urgent_msg_max) {
#ifdef GSL_QUEUE_ALERT_LIMIT
		if (0 > --alert) {
			alert = GSL_QUEUE_ALERT_LIMIT;
			GL_TraceError("[%s] queue urgent space has full!, current/max=%d/%d\n",
						  p_queue->name,p_queue->urgent_msg_used, p_queue->urgent_msg_max);
		}
#else
		GL_TraceError("[%s] queue urgent space has full!, current/max=%d/%d\n",
					  p_queue->name,p_queue->urgent_msg_used, p_queue->urgent_msg_max);
#endif
		ret = GL_QUEUE_FULL;
		goto __queue_send_urgent_exit;
	}
#ifdef GSL_QUEUE_ALERT_LIMIT
	else {
		alert = GSL_QUEUE_ALERT_LIMIT;
	}
#endif
	if (!(p_queue->idle_list)) {
		GL_TraceError("Function : %s failed! Line : %d\n", __FUNCTION__, __LINE__);
		GL_TraceError("name = %s, num_of_ur_msg = %d\n", p_queue->name, p_queue->urgent_msg_used);
		//asm volatile("break;");
		ret = GL_FAILURE;
		goto __queue_send_urgent_exit;
	}

	if (!p_queue->urgent_list) {
		/* urgent list is empty */
		//pick up one from idle list
		p_queue->urgent_list = p_queue->idle_list;
		p_queue->idle_list = p_queue->idle_list->p_next;
		//insert into the first of the used list
		p_queue->urgent_list->p_next = p_queue->used_list;
		p_queue->used_list = p_queue->urgent_list;
		if (!p_queue->used_last_item) {
			/*used list is empty */
			p_queue->used_last_item = p_queue->used_list;
		}
		p_queue->urgent_last_item = p_queue->urgent_list;
	} else {
		/* urgent list is not empty */
		// pump a new msg item from idle
		p_temp = p_queue->idle_list;
		p_queue->idle_list = p_queue->idle_list->p_next;

		// insert the new msg between the urgent tail and norml head
		p_temp->p_next = p_queue->urgent_last_item->p_next;
		p_queue->urgent_last_item->p_next = p_temp;

		// set the urgent_last_item is the new msg
		p_queue->urgent_last_item = p_temp;

		// used_last_item is urgent means all the msg are urgent,
		// we need to update the used_last_item as new msg
		if (p_queue->used_last_item->is_urgent) {
			p_queue->used_last_item = p_temp;
		}
	}

	// copy the new data to the new msg item
	memcpy(p_queue->urgent_last_item->data, pBuffer, dSize);
	p_queue->urgent_last_item->is_urgent = 1;
	p_queue->urgent_msg_used++;

	sem_post(&p_queue->read_sem);

__queue_send_urgent_exit:
	pthread_mutex_unlock(&p_queue->mt);

	return ret;
}

GL_Status_t GL_QueueReceive(GL_Queue_t queueId, void *pBuffer, GL_UINT32 dSize, GL_INT32 sdTimeout)
{
	MSG_ITEM *p_item_temp;
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;
	struct timespec tm;
	int ret;

	if (!queueId) {
		return GL_FAILURE;
	}

	if (!pBuffer) {
		return GL_FAILURE;
	}

gl_queue_receive_again:
	switch (sdTimeout) {
	case GL_INFINITE_WAIT:
	case GL_NO_WAIT:
		break;
	default:
		clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += sdTimeout / 1000;
		tm.tv_nsec += (sdTimeout % 1000) * 1000000;
		if (tm.tv_nsec >= 1000000000) {
			tm.tv_nsec -= 1000000000;
			tm.tv_sec++;
		}
	}

	do {
		if (sdTimeout == GL_INFINITE_WAIT) {
			GL_SET_BLOCK_INFO(GL_BLOCK_QUEUE, (UINT32)queueId);
			ret = sem_wait(&p_queue->read_sem);
			GL_DEL_BLOCK_INFO(queueId);
		} else if (sdTimeout == GL_NO_WAIT) {
			ret = (sem_trywait(&p_queue->read_sem));
		} else {
			GL_SET_BLOCK_INFO(GL_BLOCK_QUEUE, (UINT32)queueId);
			ret = (sem_timedwait(&p_queue->read_sem, &tm));
			GL_DEL_BLOCK_INFO(queueId);
		}
		if (__builtin_expect(ret != 0, 0)) {
			switch (errno) {
				case ETIMEDOUT:
					return GL_TIMEOUT;
				case EINTR:
					continue;
				default:
					return GL_QUEUE_EMPTY;
			}
		} else {
			break;
		}
	} while (1);

	pthread_mutex_lock(&p_queue->mt);

	if (!(p_queue->used_list)) {
		/* GL_QueueFlush should be the reason to cause the situation! */
		pthread_mutex_unlock(&p_queue->mt);
		goto gl_queue_receive_again;
	}
	// copy the msg data to the buffer for return
	memcpy(pBuffer, p_queue->used_list->data, MIN(dSize, p_queue->msg_size));
	if (p_queue->used_list->is_urgent) {
		p_queue->urgent_msg_used--;
	} else {
		p_queue->normal_msg_used--;
	}

	p_item_temp = p_queue->used_list;
	p_queue->used_list = p_queue->used_list->p_next;
	// put the pumped msg back to the idle list head
	p_item_temp->p_next = p_queue->idle_list;
	p_queue->idle_list = p_item_temp;

	// if used_list is null, it means queue empty now.
	if (p_queue->used_list == NULL) {
		p_queue->used_last_item = NULL;
		p_queue->urgent_list = NULL;
		p_queue->urgent_last_item = NULL;
	} else {
		if (p_queue->used_list->is_urgent) {
			// if the used_list is urgent, it means we need to update urgent list
			p_queue->urgent_list = p_queue->used_list;
		} else {
			// current used_list are all normal msg
			p_queue->urgent_list = NULL;
			p_queue->urgent_last_item = NULL;
		}
	}

	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

GL_Status_t GL_QueueFlushNormal(GL_Queue_t queueId)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&p_queue->mt);

	// check if there is any msg
	if (p_queue->used_list != NULL) {
		// check if there is any normal msg
		if (!p_queue->used_last_item->is_urgent) {
			p_queue->used_last_item->p_next = p_queue->idle_list;
			// check if there is any urgnet msg
			if (p_queue->urgent_list == NULL) {
				// There is no urgent msg
				p_queue->idle_list = p_queue->used_list;
				p_queue->used_list = NULL;
				p_queue->used_last_item = NULL;
			} else {
				p_queue->idle_list = p_queue->urgent_last_item->p_next;
				p_queue->used_last_item = p_queue->urgent_last_item;
				p_queue->used_last_item->p_next = NULL;
			}
			p_queue->normal_msg_used = 0;
		}
	}

	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

GL_Status_t GL_QueueFlushUrgent(GL_Queue_t queueId)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&p_queue->mt);

	// check if there is urgent msg
	if (p_queue->urgent_list != NULL) {
		// check if there is any normal msg
		if (p_queue->used_last_item->is_urgent) {
			// there is no normal msg
			p_queue->used_list = NULL;
			p_queue->used_last_item = NULL;
		} else {
			p_queue->used_list = p_queue->urgent_last_item->p_next;
		}
		// concatenate idle list with urgent list to be a new idle list
		p_queue->urgent_last_item->p_next = p_queue->idle_list;
		p_queue->idle_list = p_queue->urgent_list;
		p_queue->urgent_list = NULL;
		p_queue->urgent_last_item = NULL;
		p_queue->urgent_msg_used = 0;
	}

	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

GL_Status_t GL_QueueFlush(GL_Queue_t queueId)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_FAILURE;
	}

	pthread_mutex_lock(&p_queue->mt);

	if (p_queue->used_list != NULL) {
		p_queue->used_last_item->p_next = p_queue->idle_list;
		p_queue->idle_list = p_queue->used_list;
	}
	p_queue->used_list = NULL;
	p_queue->used_last_item = NULL;
	p_queue->urgent_list = NULL;
	p_queue->urgent_last_item = NULL;
	p_queue->normal_msg_used = 0;
	p_queue->urgent_msg_used = 0;

	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

GL_Status_t GL_QueuePeek(GL_Queue_t queueId, GL_UINT32 *value)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_ERR_INVALID_PARAM;
	}

	pthread_mutex_lock(&p_queue->mt);
	*value = (p_queue->urgent_msg_max + p_queue->normal_msg_max) - (p_queue->urgent_msg_used + p_queue->normal_msg_used);
	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

GL_Status_t GL_QueuePeekUrgent(GL_Queue_t queueId, GL_UINT32 *value)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_ERR_INVALID_PARAM;
	}

	pthread_mutex_lock(&p_queue->mt);
	*value = p_queue->urgent_msg_max - p_queue->urgent_msg_used;
	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

GL_Status_t GL_QueuePeekNormal(GL_Queue_t queueId, GL_UINT32 *value)
{
	gsl_queue_t *p_queue = (gsl_queue_t *)queueId;

	if (!queueId) {
		return GL_ERR_INVALID_PARAM;
	}

	pthread_mutex_lock(&p_queue->mt);
	*value = p_queue->normal_msg_max - p_queue->normal_msg_used;
	pthread_mutex_unlock(&p_queue->mt);

	return GL_SUCCESS;
}

#if GL_OBJECT_LIST_QUEUE
GL_UINT8 GL_QueueIsDupName(const char *name)
{
	gsl_list_t *p;
	gsl_queue_t *q;

	GL_SchedulerLock();
	for (p = &queue_dump_list; p->next != NULL; p = p->next) {
		q = (gsl_queue_t *)p->next;
		if (0 == strncmp(name, q->name, GSL_NAME_LEN)) {
			GL_SchedulerUnLock();
			return 1;
		}
	}
	GL_SchedulerUnLock();
	return 0;
}

void GL_QueueDumpList(GL_UINT16 wChannel)
{
	gsl_list_t *p;
	gsl_queue_t *q;

	wChannel = wChannel;
	GL_TraceInfo("\n--------------GSL queue dump list start--------------------\n");
	GL_SchedulerLock();
	for (p = &queue_dump_list; p->next != NULL; p = p->next) {
		q = (gsl_queue_t *)p->next;
		GL_TraceInfo(" queue name : %s\n normal message used: %d\n urgent message used: %d\n msgsize=%d\n\n",
					q->name, q->normal_msg_used, q->urgent_msg_used, q->msg_size);
	}
	GL_SchedulerUnLock();
	GL_TraceInfo("\n--------------GSL queue dump list end--------------------\n\n");

}
#endif

