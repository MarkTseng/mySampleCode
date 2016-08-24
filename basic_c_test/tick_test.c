#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#include <signal.h>

#include <errno.h>

unsigned long milsec(void)
{
	struct timeval  tv;
	struct timezone tz;

	if( gettimeofday( &tv,&tz ) == -1 )
		return( 0 );
	return( tv.tv_usec + (tv.tv_sec*1000*1000));
}

const char* uname(void)
{
	static char  buff[1024];
	FILE*        fp=fopen("/proc/version","r");

	if( fp==NULL )
		return( "(OS unknown)" );
	fgets(buff,sizeof(buff),fp);
	return( buff );
}

int main(int argc,char *argv[])
{
	volatile char           B1[1024];
	volatile char           B2[1024];
	volatile unsigned long  Start,Stop;
	volatile int            count,max=1000,index;
	struct timeval          tv;
	struct timespec         tvp;

	if( argc >=2 ) 
		max = atoi(argv[1]);

	printf("OS:  %s\n",uname());
	Start = milsec();
	for(count=0; count<max; ++count)
	{
		tv.tv_sec  = 0;
		tv.tv_usec = 1000;
		select(0,NULL,NULL,NULL,&tv);
	}
	Stop  = milsec();
	printf("TICK  :  Iterations[%d] Delta[%10lu] Avg[%7.3f]\n",max,Stop-Start,(double)(Stop-Start)/(double)(max));

	Start = milsec();
	for(count=0; count<max; ++count)
	{
		tvp.tv_sec  = 0;
		tvp.tv_nsec = 1000000;
		pselect(0,NULL,NULL,NULL,&tvp,NULL);
	}
	Stop  = milsec();
	printf("TICKp :  Iterations[%d] Delta[%10lu] Avg[%7.3f]\n",max,Stop-Start,(double)(Stop-Start)/(double)(max));

	////////////////////////////////////////
	// BENCH of memcpy/indexing
	////////////////////////////////////////

	Start = milsec();
	for(count=0; count<max; ++count)
	{
		for(index=0; index<sizeof(B1); ++index)
			B2[index] = B1[index];
	}
	Stop  = milsec();
	printf("MEMCPY:  Iterations[%d] Delta[%10lu] Avg[%7.3f]\n",max,Stop-Start,(double)(Stop-Start)/(double)(max));
}
