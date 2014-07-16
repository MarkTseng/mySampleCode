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

    while (++th->cnt) {
        printf("[%s>]%lu / %d ...\n", GL_TaskName(th->th), pthread_self(), th->cnt);
        usleep(1000 * 100);

        if (th->cnt == 10)  GL_TaskSuspend(th->th);
    }
}

/*
 *
 */
#define TEST_TH_CNT     8
int main(void)
{
    GL_Test_t th[TEST_TH_CNT];
    char name[32];
    GL_Status_t ret;
    int i = 0, j = 0;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 1. task self suspend ...\n\n");

    for (i=0; i<TEST_TH_CNT; i++) {
        sprintf(name, "th%02d", i);
        memset(&th[i], 0, sizeof(GL_Test_t));
        ret = GL_TaskCreate(name, thread_hd0, &th[i], 1, 1024, TRUE, &th[i].th);
        printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

        usleep(1000 * 40);
    }

    sleep(4);
    for (i=0; i<TEST_TH_CNT; i++) {
        ret = GL_TaskActivate(th[i].th);
        printf("[%s]GL_TaskActivate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);
    }

    sleep(4);
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. multi suspend & activate ...\n\n");

    for (j=0; j<5; j++) {
        printf(" * ----------------------------------------\n");
        printf(" * suspend times %d ...\n", j);
        for (i=0; i<TEST_TH_CNT; i++) {
            ret = GL_TaskSuspend(th[i].th);
            printf("[%s]GL_TaskSuspend(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);
        }

        sleep(1);
    }

    sleep(2);
    for (j=0; j<5; j++) {
        printf(" * ----------------------------------------\n");
        printf(" * activate times %d ...\n", j);
        for (i=0; i<TEST_TH_CNT; i++) {
            ret = GL_TaskActivate(th[i].th);
            printf("[%s]GL_TaskActivate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);
        }

        sleep(1);
    }

    sleep(3);
    for (i=0; i<TEST_TH_CNT; i++) {
        printf("[%s]GL_TaskDestroyOne(%d)%lu ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self());
        ret = GL_TaskDestroyOne(th[i].th);
    }
#endif

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 3. ...\n");
    sleep(1);

    return 0;
}
