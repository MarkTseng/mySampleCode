#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>  
#include <sys/time.h>  
#include <errno.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/select.h>  


int main(int argc, char **argv)  
{  
    unsigned int nTimeTestSec = 0;  
    unsigned int nTimeTest = 0;  
    struct timeval tvBegin;  
    struct timeval tvNow;  
    int ret = 0;  
    unsigned int nDelay = 0;  
    struct timeval tv;  
    int fd = 1;  
    int i = 0;  
    struct timespec req;  

    unsigned int delay[20] =   
    {500000, 100000, 50000, 10000, 1000, 900, 500, 100, 10, 1, 0};  
    int nReduce = 0; //误差  

    fprintf(stderr, "%19s%12s%12s%12s\n", "fuction", "time(usec)", "realtime", "reduce");  
    fprintf(stderr, "----------------------------------------------------\n");  
    for (i = 0; i < 20; i++)  
    {  
        if (delay[i] <= 0)  
            break;  
        nDelay = delay[i];  
        //test sleep  
        gettimeofday(&tvBegin, NULL);  
        ret = usleep(nDelay);  
        if(ret == -1)  
        {  
            fprintf(stderr, "usleep error, errno=%d [%s]\n", errno, strerror(errno));  
        }  
        gettimeofday(&tvNow, NULL);  
        nTimeTest = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;  
        nReduce = nTimeTest - nDelay;  

        fprintf (stderr, "\t usleep       %8u   %8u   %8d\n", nDelay, nTimeTest,nReduce);  

        //test nanosleep  
        req.tv_sec = nDelay/1000000;  
        req.tv_nsec = (nDelay%1000000) * 1000;  

        gettimeofday(&tvBegin, NULL);  
        ret = nanosleep(&req, NULL);  
        if (-1 == ret)  
        {  
            fprintf (stderr, "\t nanousleep   %8u   not support\n", nDelay);  
        }  
        gettimeofday(&tvNow, NULL);  
        nTimeTest = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;  
        nReduce = nTimeTest - nDelay;  
        fprintf (stderr, "\t nanosleep    %8u   %8u   %8d\n", nDelay, nTimeTest,nReduce);  

        //test select  
        tv.tv_sec = 0;  
        tv.tv_usec = nDelay;  

        gettimeofday(&tvBegin, NULL);  
        ret = select(0, NULL, NULL, NULL, &tv);  
        if (-1 == ret)  
        {  
            fprintf(stderr, "select error. errno = %d [%s]\n", errno, strerror(errno));  
        }  

        gettimeofday(&tvNow, NULL);  
        nTimeTest = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;  
        nReduce = nTimeTest - nDelay;  
        fprintf (stderr, "\t select       %8u   %8u   %8d\n", nDelay, nTimeTest,nReduce);  

        //pselcet  
        req.tv_sec = nDelay/1000000;  
        req.tv_nsec = (nDelay%1000000) * 1000;  

        gettimeofday(&tvBegin, NULL);  
        ret = pselect(0, NULL, NULL, NULL, &req, NULL);  
        if (-1 == ret)  
        {  
            fprintf(stderr, "select error. errno = %d [%s]\n", errno, strerror(errno));  
        }  

        gettimeofday(&tvNow, NULL);  
        nTimeTest = (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec - tvBegin.tv_usec;  
        nReduce = nTimeTest - nDelay;  
        fprintf (stderr, "\t pselect      %8u   %8u   %8d\n", nDelay, nTimeTest,nReduce);  

        fprintf (stderr, "--------------------------------\n");  

    }  

    return 0;  
}  
