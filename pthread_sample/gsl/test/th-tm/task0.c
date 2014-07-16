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

	printf(".... th->th = 0x%08X, ", (unsigned int)th->th);
    printf("[%s>]%lu\n", GL_TaskName(th->th), pthread_self());
    usleep(1000 * 400);
    printf("[%s<]%lu\n", GL_TaskName(th->th), pthread_self());
}

void thread_hd1(void *p)
{
    GL_Test_t *th = p;

    if (!th) return;

    while (1) {
        printf("[%s>]%lu\n", GL_TaskName(th->th), pthread_self());
        usleep(1000 * 500);
    }
}

void thread_hd2(void *p)
{
    GL_Test_t *th = p;

    if (!th) return;

    while (1) {
        printf("[%s>]%lu\n", GL_TaskName(th->th), pthread_self());
        usleep(1000 * 500);
        GL_TaskSelfDelete();
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
    int i = 0, j = 0;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 1. create & exit ...\n\n");

	for (i = 0; i < 32; i++) {
		sprintf(name, "th%02d", i);
		printf("[#%d]GL_TaskCreate %s ...\n", i, name);
		ret = GL_TaskCreate(name, thread_hd0, &th[i], 1, 1024, TRUE, &th[i].th);
		printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

		usleep(1000 * 100);
	}

    sleep(1);
#endif

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. destroy one ...\n\n");

    for (j = 0; j < 2; j++) {
        for (i = 0; i < 8; i++) {
            sprintf(name, "th%02d", i);
            printf("[#%d]GL_TaskCreate %s ...\n", i, name);
            ret = GL_TaskCreate(name, thread_hd1, &th[i], 1, 1024, TRUE, &th[i].th);
            printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

            usleep(1000 * 100);
        }

        sleep(2);

        for (i=0; i<8; i++) {
            printf("[%s]GL_TaskDestroyOne(%d)%lu ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self());
            ret = GL_TaskDestroyOne(th[i].th);

            usleep(1000 * 50);
        }
    }

    sleep(1);
#endif

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 3. delete self ...\n\n");

	for (i = 0; i < 16; i++) {
		sprintf(name, "th%02d", i);
		printf("[#%d]GL_TaskCreate %s ...\n", i, name);
		ret = GL_TaskCreate(name, thread_hd2, &th[i], 1, 1024, TRUE, &th[i].th);
		printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

		usleep(1000 * 100);
	}

	sleep(1);
#endif

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 4. ...\n");

    return 0;
}
