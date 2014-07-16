/*
 * cond0.c
 *
 */
#include "gsl.h"

typedef struct GL_Test {
    GL_Task_t th;
    GL_Mutex_t mv;
    GL_Cond_t cv;
    int cnt;
} GL_Test_t;

char *GL_TaskName(GL_Task_t taskId);


/*
 *
 */
void thread_hd0(void *p)
{
    GL_Test_t *th = p;
    GL_Status_t ret;

    if (!th) return;

#if 1
    while ( ++th->cnt ) {
        printf("\n\n");
        printf("/* --------------------------------------------------- */\n");
        printf(" * %s wait(%d) cond ...\n\n", GL_TaskName(th->th), th->cnt);

        ret = GL_MutexLock(th->mv);
        printf("[#%s]GL_MutexLock(%d)%d ...\n", GL_TaskName(th->th), __LINE__, ret);

        printf("[#%s>]GL_CondWait(%d) ...\n", GL_TaskName(th->th), __LINE__);
        ret = GL_CondWait(th->cv, GL_INFINITE_WAIT);
        printf("[#%s<]GL_CondWait(%d)%d ...\n", GL_TaskName(th->th), __LINE__, ret);

        ret = GL_MutexUnlock(th->mv);
        printf("[#%s]GL_MutexUnlock(%d)%d ...\n", GL_TaskName(th->th), __LINE__, ret);

		if (th->cnt > 5) {
			break;
		}
    }
	th->cnt = 0;
#endif
}

/*
 *
 */
int main(void)
{
    GL_Test_t th[8];
    char name[32];
    GL_Status_t ret;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");

    memset(&th[0], 0, sizeof(GL_Test_t));
    ret = GL_MutexCreate("condm00", &th[0].mv);
    printf("[##]GL_MutexCreate(%d)%d ...\n", __LINE__, ret);

    ret = GL_CondCreate("condv00", &th[0].cv, th[0].mv);
    printf("[##]GL_CondCreate(%d)%d ...\n", __LINE__, ret);

    memcpy(&th[1], &th[0], sizeof(GL_Test_t));
    sprintf(name, "th%02d", 0);
    ret = GL_TaskCreate(name, thread_hd0, &th[0], 1, 1024 * 1024, TRUE, &th[0].th);
    printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[0].th), __LINE__, pthread_self(), ret);

    sprintf(name, "th%02d", 1);
    ret = GL_TaskCreate(name, thread_hd0, &th[1], 1, 1024 * 1024, TRUE, &th[1].th);
    printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[1].th), __LINE__, pthread_self(), ret);
#endif

    while (1) {
        sleep(1);
        ret = GL_CondBroadcast(th[0].cv);
        printf("[##]GL_CondBroadcast(%d)%d ...\n", __LINE__, ret);
		if ((th[0].cnt | th[1].cnt) == 0) {
			break;
		}
    }

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 4. ...\n");

    return 0;
}
