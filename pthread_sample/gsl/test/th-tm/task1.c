/*
 * gl_task.c
 *
 */
#include "gsl.h"

typedef struct GL_Test {
    GL_Task_t th;
    int cnt;
} GL_Test_t;


char *GL_TaskName(GL_Task_t taskId);


void thread_hd0(void *p)
{
    GL_Test_t *th = p;

    if (!th) return;

	while (1) {
    	printf("Group0: [%s>]%lu\n", GL_TaskName(th->th), pthread_self());
    	usleep(1000 * 500);
	}
}

void thread_hd1(void *p)
{
    GL_Test_t *th = p;

    if (!th) return;

    while (1) {
        printf("Group1: [%s>]%lu\n", GL_TaskName(th->th), pthread_self());
        usleep(1000 * 500);
    }
}

/*
 *
 */
int main(void)
{
    GL_Test_t th[32];
    char name[32];
    GL_Status_t ret;
    int i = 0;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 1. suspend & activate ...\n\n");

    for (i=0; i<16; i++) {
        sprintf(name, "th%02d", i);
        printf("[#%d]GL_TaskCreate %s ...\n", i, name);
		if ((i & 0x01) == 0) {
			ret = GL_TaskCreate(name, thread_hd0, &th[i], 1, 64 * 1024, FALSE, &th[i].th);
		} else {
			ret = GL_TaskCreate(name, thread_hd1, &th[i], 1, 64 * 1024, TRUE, &th[i].th);
		}
        printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

        usleep(1000 * 100);
    }

    sleep(4);

    for (i=0; i<16; i++) {
        if ((i & 0x01) == 0) {
            ret = GL_TaskActivate(th[i].th);
            printf("[%s]GL_TaskActivate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);
        }
        else {
            ret = GL_TaskSuspend(th[i].th);
            printf("[%s]GL_TaskSuspend(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);
        }

        usleep(1000 * 100);
    }

    sleep(5);

    for (i=0; i<16; i++) {
        ret = GL_TaskActivate(th[i].th);
        printf("[%s]GL_TaskActivate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

        usleep(1000 * 100);
    }

    for (i=0; i<16; i++) {
        printf("[%s]GL_TaskDestroyOne(%d)%lu ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self());
        ret = GL_TaskDestroyOne(th[i].th);
    }
#endif

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. ...\n");
    sleep(1);

    return 0;
}
