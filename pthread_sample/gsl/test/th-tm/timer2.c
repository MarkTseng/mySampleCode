/*
 * gl_timer.c
 * 
 */
#include "gsl.h"


char *GL_TimerName(GL_Timer_t timerId);

typedef struct GL_Test {
    GL_Timer_t tm;
    int cnt;
} GL_Test_t;

/*
 * test thread
 */
#define perror_test(prestr, err)    \
    fprintf(stderr, "[#e]%s(%d): %s(%d) .\r\n", prestr, __LINE__, strerror(err), err)

void timerp1(void *p)
{
    GL_Test_t *tm = p;

    if (!tm) return;
    printf("[%s]%s(0x%x) ...\n", GL_TimerName(tm->tm), __FUNCTION__, GL_GetTimestamp());
}

int main(void)
{
    GL_Test_t tm[32];
    char name[32];
    GL_Status_t ret;
    int i = 0;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 1. create ...\n\n");

    for (i=0; i<16; i++) {
        sprintf(name, "tm%02d", i);
        ret = GL_TimerCreate(name, timerp1, &tm[i], FALSE, (i % 5  + 1) * 100, FALSE, &tm[i].tm);
        printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
        
        usleep(1000 * 100);
    }
    sleep(2);

    for (i=0; i<16; i++) {
        ret = GL_TimerEnable(tm[i].tm);
        printf("[%s]GL_TimerEnable(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
    }
    printf("\n * current time 0x%x ...\n", GL_GetTimestamp());
    sleep(2);
    printf("\n * current time 0x%x ...\n", GL_GetTimestamp());

    for (i=0; i<16; i++) {
        ret = GL_TimerSchedule(tm[i].tm, (i % 5  + 1) * 50);
        printf("[%s]GL_TimerSchedule(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
    }
    printf("\n * current time 0x%x ...\n", GL_GetTimestamp());
    sleep(2);
    printf("\n * current time 0x%x ...\n", GL_GetTimestamp());

    for (i=0; i<16; i++) {
        ret = GL_TimerEnable(tm[i].tm);
        printf("[%s]GL_TimerEnable(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
    }
    printf("\n * current time 0x%x ...\n", GL_GetTimestamp());
    sleep(2);
    printf("\n * current time 0x%x ...\n", GL_GetTimestamp());
    
    sleep(4);
    for (i=0; i<16; i++) {
        printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp());
        ret = GL_TimerDelete(tm[i].tm);
        printf(" / %d ...\n", ret);
    }
#endif

    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. ...\n");
    return 0;
}
