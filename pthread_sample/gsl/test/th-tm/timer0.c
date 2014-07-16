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
    printf("[>%s]%s ...\n", GL_TimerName(tm->tm), __FUNCTION__);
    
    sleep(1);
    
    printf("[<%s]%s ...\n", GL_TimerName(tm->tm), __FUNCTION__);
}

int runp2flag = 1;
void timerp2(void *p)
{
    GL_Test_t *tm = p;
    GL_Status_t ret;

    if (!tm) return;

    printf("[#>]%s(%d) ...\n", GL_TimerName(tm->tm), tm->cnt++);
    if (tm->cnt == 10) {
        ret = GL_TimerSchedule(tm->tm, 400);
        printf("[%s]%s / %s(%d) ...\n", GL_TimerName(tm->tm), __FUNCTION__, "GL_TimerSchedule", ret);
    }
    
    if (tm->cnt == 20) {
        ret = GL_TimerSchedule(tm->tm, 200);
        printf("[%s]%s / %s(%d) ...\n", GL_TimerName(tm->tm), __FUNCTION__, "GL_TimerSchedule", ret);
    }
    
    if (tm->cnt == 30) {
        ret = GL_TimerSchedule(tm->tm, 100);
        printf("[%s]%s / %s(%d) ...\n", GL_TimerName(tm->tm), __FUNCTION__, "GL_TimerSchedule", ret);
    }
    
    if (tm->cnt == 40) {
        ret = GL_TimerSchedule(tm->tm, 50);
        printf("[%s]%s / %s(%d) ...\n", GL_TimerName(tm->tm), __FUNCTION__, "GL_TimerSchedule", ret);
    }
    
    if (tm->cnt == 50) {
        ret = GL_TimerSchedule(tm->tm, 10);
        printf("[%s]%s / %s(%d) ...\n", GL_TimerName(tm->tm), __FUNCTION__, "GL_TimerSchedule", ret);
    }

    if (tm->cnt == 60) {
        ret = GL_TimerDisable(tm->tm);
        printf("[%s]%s / %s(%d) ...\n", GL_TimerName(tm->tm), __FUNCTION__, "GL_TimerDisable", ret);
        
        runp2flag = 0;
    }
}


int main(void)
{
    GL_Test_t tm1, tm2;
    GL_Status_t ret;
    int i = 0;

#if 1
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 1. create & delete ...\n\n");
    
    memset(&tm1, 0, sizeof(GL_Test_t));
    memset(&tm2, 0, sizeof(GL_Test_t));
    
    while( (i += 100) < 1000 ) {
        ret = GL_TimerCreate("timer1", timerp1, &tm1, TRUE, i + 50, TRUE, &tm1.tm);
        printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp(), ret);
        
        ret = GL_TimerCreate("timer2", timerp1, &tm2, TRUE, i + 30, TRUE, &tm2.tm);
        printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp(), ret);
        
        sleep(2);

        printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp());
        ret = GL_TimerDelete(tm1.tm);
        printf(" / %d ...\n", ret);
    
        printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp());
        ret = GL_TimerDelete(tm2.tm);
        printf(" / %d ...\n", ret);
    }
#endif

#if 0
    runp2flag  = 1;
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 2. schedule & disable ...\n\n");
    
    memset(&tm1, 0, sizeof(GL_Test_t));
    memset(&tm2, 0, sizeof(GL_Test_t));
    
    ret = GL_TimerCreate("timer3", timerp2, &tm1, TRUE, 100, TRUE, &tm1.tm);
    printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp(), ret);
    
    ret = GL_TimerCreate("timer4", timerp2, &tm2, TRUE, 200, TRUE, &tm2.tm);
    printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp(), ret);
    
    while (runp2flag)   sleep(1);

    printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp());
    ret = GL_TimerDelete(tm2.tm);
    printf(" / %d ...\n", ret);

    printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp());
    ret = GL_TimerDelete(tm1.tm);
    printf(" / %d ...\n", ret);
#endif

#if 0
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 3. enable & disable ...\n\n");
    
    memset(&tm1, 0, sizeof(GL_Test_t));
    memset(&tm2, 0, sizeof(GL_Test_t));
    
    ret = GL_TimerCreate("timer5", timerp1, &tm1, FALSE, 500, TRUE, &tm1.tm);
    printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp(), ret);
    
    ret = GL_TimerCreate("timer6", timerp1, &tm2, TRUE, 300, TRUE, &tm2.tm);
    printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp(), ret);
    
    sleep(2);
    ret = GL_TimerEnable(tm1.tm);
    printf("[%s]GL_TimerEnable(%d)0x%x / %d ...\n", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp(), ret);
    ret = GL_TimerDisable(tm2.tm);
    printf("[%s]GL_TimerDisable(%d)0x%x / %d ...\n", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp(), ret);

    sleep(2);
    ret = GL_TimerEnable(tm2.tm);
    printf("[%s]GL_TimerEnable(%d)0x%x / %d ...\n", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp(), ret);
    ret = GL_TimerDisable(tm1.tm);
    printf("[%s]GL_TimerDisable(%d)0x%x / %d ...\n", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp(), ret);
    
    sleep(2);
    ret = GL_TimerDisable(tm2.tm);
    printf("[%s]GL_TimerDisable(%d)0x%x / %d ...\n", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp(), ret);
    ret = GL_TimerDisable(tm1.tm);
    printf("[%s]GL_TimerDisable(%d)0x%x / %d ...\n", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp(), ret);
    
    sleep(2);
    
    printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm2.tm), __LINE__, GL_GetTimestamp());
    ret = GL_TimerDelete(tm2.tm);
    printf(" / %d ...\n", ret);
    
    printf("[%s]GL_TimerDelete(%d)0x%x", GL_TimerName(tm1.tm), __LINE__, GL_GetTimestamp());
    ret = GL_TimerDelete(tm1.tm);
    printf(" / %d ...\n", ret);
#endif

    sleep(5);
    printf("\n\n");
    printf("/* --------------------------------------------------- */\n");
    printf(" * 4. ...\n");

    return 0;
}
