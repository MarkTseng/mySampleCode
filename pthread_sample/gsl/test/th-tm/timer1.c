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
    printf("[%s]%s ...\n", GL_TimerName(tm->tm), __FUNCTION__);
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
        sprintf(name, "tms%02d", i);
        ret = GL_TimerCreate(name, timerp1, &tm[i], TRUE, (i % 5  + 1) * 200, FALSE, &tm[i].tm);
        printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
        
        usleep(1000 * 100);
    }
    
    sleep(4);
    for (i=0; i<16; i++) {
        ret = GL_TimerDelete(tm[i].tm);
        printf("[%s]GL_TimerDelete(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
    }

    printf("\n\n");
    for (i=0; i<16; i++) {
        sprintf(name, "tmm%02d", i);
        ret = GL_TimerCreate(name, timerp1, &tm[i], TRUE, (i % 5  + 1) * 100, TRUE, &tm[i].tm);
        printf("[%s]GL_TimerCreate(%d)0x%x / %d ...\n", GL_TimerName(tm[i].tm), __LINE__, GL_GetTimestamp(), ret);
        
        usleep(1000 * 100);
    }
    
    sleep(30);
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
