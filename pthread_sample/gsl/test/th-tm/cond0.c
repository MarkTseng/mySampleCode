/*
 * cond0.c
 *
 */
#include <sys/time.h>
#include <gsl.h>

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
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 1. wait cond ...\n\n");

    printf("[%s>]%lu\n", GL_TaskName(th->th), pthread_self());
    ret = GL_MutexLock(th->mv);
    printf("[##]GL_MutexLock(%d)%d ...\n", __LINE__, ret);

    printf("[#>]GL_CondWait(%d) ...\n", __LINE__);
    ret = GL_CondWait(th->cv, GL_INFINITE_WAIT);
    printf("[#<]GL_CondWait(%d)%d ...\n", __LINE__, ret);

    ret = GL_MutexUnlock(th->mv);
    printf("[##]GL_MutexUnlock(%d)%d ...\n", __LINE__, ret);
#endif

    struct timeval tv;
    long t1, t2;

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. timed wait cond ...\n\n");

    /* timed wait */
    ret = GL_MutexLock(th->mv);
    printf("[##]GL_MutexLock(%d)%d ...\n", __LINE__, ret);

    gettimeofday(&tv, NULL);
    t1 = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    printf("[#>]GL_CondWait(%d) ...\n", __LINE__);
    ret = GL_CondWait(th->cv, 2000);
    gettimeofday(&tv, NULL);
    t2 = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    printf("[#<]GL_CondWait(%d)%d / time = %ld ...\n", __LINE__, ret, t2 - t1);

    ret = GL_MutexUnlock(th->mv);
    printf("[##]GL_MutexUnlock(%d)%d ...\n", __LINE__, ret);

    while(th->cnt --) {
        printf("\n\n");
        printf("/* --------------------------------------------------- */\n");
        printf(" * 3. timeout(%d) wait cond ...\n\n", th->cnt);

        /* timed wait */
        ret = GL_MutexLock(th->mv);
        printf("[##]GL_MutexLock(%d)%d ...\n", __LINE__, ret);

        gettimeofday(&tv, NULL);
        t1 = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        printf("[#>]GL_CondWait(%d) ...\n", __LINE__);
        ret = GL_CondWait(th->cv, 600);
        gettimeofday(&tv, NULL);
        t2 = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        printf("[#<]GL_CondWait(%d)%d / time = %ld ...\n", __LINE__, ret, t2 - t1);

        ret = GL_MutexUnlock(th->mv);
        printf("[##]GL_MutexUnlock(%d)%d ...\n", __LINE__, ret);
    }

    sleep(1);
}


/*
 *
 */
int main(void)
{
    GL_Test_t th[8];
    char name[32];
    GL_Status_t ret;
    int i = 0;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");

    th[i].cnt = 5;
    ret = GL_MutexCreate("condm00", &th[i].mv);
    printf("[##]GL_MutexCreate(%d)%d ...\n", __LINE__, ret);

    ret = GL_CondCreate("condv00", &th[i].cv, th[i].mv);
    printf("[##]GL_CondCreate(%d)%d ...\n", __LINE__, ret);

    sprintf(name, "th%02d", i);
    ret = GL_TaskCreate(name, thread_hd0, &th[i], 1, 1024 * 1024, TRUE, &th[i].th);
    printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th[i].th), __LINE__, pthread_self(), ret);

    sleep(1);
    ret = GL_CondSignal(th[i].cv);
    printf("[##]GL_CondSignal(%d)%d ...\n", __LINE__, ret);
#endif

    sleep(1);
    ret = GL_CondSignal(th[i].cv);
    printf("[##]GL_CondSignal(%d)%d ...\n", __LINE__, ret);

    while (th[i].cnt >= 0) sleep(1);
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 4. ...\n");

    return 0;
}
