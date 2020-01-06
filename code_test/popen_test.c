#include <stdio.h>

int main(int argc, char *argv[]) {
    char buf[128];
    FILE *pp;

    while(1){
        if ((pp = popen("cat /proc/meminfo;exit", "r")) == NULL) {
            printf("popen() error!\n");
            exit(1);
        }

        while (fgets(buf, sizeof buf, pp)) {
            printf("%s", buf);
        }
        pclose(pp);
        sleep(1);
    }
    return 0;
}

