#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void errexit(char *errMsg){
    printf("\n About to exit: %s", errMsg);
    fflush(stdout);
    exit(1);
}

int main()
{
    char fifoName[]="/tmp/isp_cmdq";
    FILE *cfp;

    cfp = fopen(fifoName,"w");
    if(cfp == NULL) 
        errexit("Unable to open fifo for writing");
    fprintf(cfp,"%d",1000);
    fflush(cfp);
    fclose(cfp);
    exit(0);
}
