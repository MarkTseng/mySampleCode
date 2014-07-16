#include "gsl/gl_config.h"
#include "gsl/gl_types.h"
#include "gl_common.h"

#if GL_OBJECT_LIST
void gsl_insert_list(void *head, void *elem)
{
	gsl_list_t *h = (gsl_list_t *)head;
	gsl_list_t *e = (gsl_list_t *)elem;

	GL_SchedulerLock();
	e->next = h->next;
	h->next = elem;
	GL_SchedulerUnLock();
}

int gsl_remove_list(void *phead, void *pelem)
{
	gsl_list_t *head = (gsl_list_t *)phead;
	gsl_list_t *elem = (gsl_list_t *)pelem;

	GL_SchedulerLock();
	for (; head != NULL; head = head->next) {
		if (head->next == pelem) {
			head->next = elem->next;
			GL_SchedulerUnLock();
			return 0;
		}
	}
	GL_SchedulerUnLock();

	return -1;
}

void gsl_insert_dlist(void *inserted, void *insert)
{
	gsl_dlist_t *pinsert = (gsl_dlist_t *)inserted;
	gsl_dlist_t *elem = (gsl_dlist_t *)insert;
	gsl_dlist_t *p;

	GL_SchedulerLock();
	elem->prev = pinsert;
	elem->next = pinsert->next;
	p = (gsl_dlist_t *)pinsert->next;
	if (p != NULL) {
		p->prev = elem;
	}
	pinsert->next = elem;
	GL_SchedulerUnLock();
}

void gsl_remove_dlist(void *removed)
{
	gsl_dlist_t *elem = (gsl_dlist_t *)removed;
	gsl_dlist_t *p;

	GL_SchedulerLock();
	p = (gsl_dlist_t *)elem->prev;
	p->next = elem->next;
	p = elem->next;
	if (p != NULL) {
		p->prev = elem->prev;
	}

	GL_SchedulerUnLock();
}
#endif

