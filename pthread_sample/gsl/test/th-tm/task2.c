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


void thread_hd2(void *p)
{
    GL_Test_t *th = p;

    if (!th) return;

    while (++th->cnt) {
        printf("[%s>]%lu / %d .\n", GL_TaskName(th->th), pthread_self(), th->cnt);
        usleep(1000 * 100);
        if (th->cnt > 5)  GL_TaskSelfDelete();
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
    printf(" * 1. task auto exit ...\n\n");

    for (i=0; i<16; i++) {
        sprintf(name, "th%02d", i);
        memset(&th[i], 0, sizeof(GL_Test_t));
        ret = GL_TaskCreate(name, thread_hd2, &th[i], 1, 1024, TRUE, &th[i].th);
        printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

        usleep(1000 * 100);
    }

    sleep(2);
#endif

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. ...\n");

    return 0;
}
