#include <sys/time.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    struct timeval adj, old;
    int rc;

    /* Speed up the clock by 1.5 seconds. */
    adj.tv_sec=1;
    adj.tv_usec=500000;

    rc=adjtime(&adj, &old);
    if(rc==0) {
        printf("adjtime() successful. "
               "Olddelta = %u.%06u\n",
                old.tv_sec, old.tv_usec);
    }
    else {
        printf("adjtime() failed, errno = %d\n",errno);
        return -1;
    }

    return 0;
}

