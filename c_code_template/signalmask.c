#include <unistd.h> 
#include <signal.h> 
#include <stdio.h> 
#include <errno.h>

void show_handler( struct sigaction * act)
{
    int sig_dfl = ( int )SIG_DFL;
     int sig_ign = ( int )SIG_IGN;

    if (act -> sa_flags == sig_dfl)
    {
        printf( "Default action\n " );
    }
    else  if (act -> sa_flags == sig_ign)
    {
        printf( " Ignore the signal\n " );
    }
    else
    {
        printf( " =========== " );
    }
}

void main()
{
    int i;
     struct sigaction act,oldact;
    act.sa_handler = show_handler;
     // act.sa_flags = SA_ONESHOT | SA_NOMASK; 
    sigaction(SIGUSR1, & act, & oldact);
     for (i = 5 ;i < 15 ;i ++ )
    {
        printf("sa_handler of signal %d ",i); 

        sigaction(i,NULL, & oldact);
    }
}

