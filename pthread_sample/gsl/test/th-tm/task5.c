/*
 * gl_task.c
 *
 */
#include "gsl.h"
#include <pwd.h>

typedef struct GL_Test {
    GL_Task_t th;
    int cnt;
} GL_Test_t;

/*
 *
*/
void thread_hd0(void *p)
{
    GL_Test_t *th = p;

    if (!th) return;

    usleep(1000 * 400);
    printf("[%s<]%s(%lu) ...\n", GL_TaskName(th->th), __FUNCTION__, pthread_self());

    while(1)    sleep(1);
}

/*
 *
 */
int main(void)
{
    GL_Test_t th;
    GL_Status_t ret;

    if (0 != geteuid()) {
        struct passwd *pw;

        pw = getpwuid(geteuid());
        printf("[##]I'm sorry %s, you should use root/sudo run this case(set schedpolicy) ...\n", pw->pw_name);

        return 0;
    }

    ret = GL_TaskCreate("task0", thread_hd0, &th, 21, 1024, TRUE, &th.th);
    printf("[%s]GL_TaskCreate(%d)%lu / %d ...\n", GL_TaskName(th.th), __LINE__, pthread_self(), ret);

    sleep(2);

    ret = GL_TaskSetPriority(th.th, 0);
    printf("[%s]GL_TaskSetPriority(%d)%lu / %d ...\n", GL_TaskName(th.th), __LINE__, pthread_self(), ret);

    sleep(1);

    ret = GL_TaskSetPriority(th.th, 31);
    printf("[%s]GL_TaskSetPriority(%d)%lu / %d ...\n", GL_TaskName(th.th), __LINE__, pthread_self(), ret);

    sleep(1);

    GL_UINT32 pdPriority;
    ret = GL_TaskGetPriority(th.th, &pdPriority);
    printf("[%s]GL_TaskGetPriority(%d)%lu / %d - %d ...\n", GL_TaskName(th.th), __LINE__, pthread_self(), ret, pdPriority);

    sleep(1);

    printf("[%s]GL_TaskDestroyOne(%d)%lu ...\n", GL_TaskName(th.th), __LINE__, pthread_self());
    ret = GL_TaskDestroyOne(th.th);

    sleep(1);
    return 0;
}
