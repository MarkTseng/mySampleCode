#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include "test.h"

GL_INT32 Test_usSleepProc(GL_INT32 testtimeval);

pthread_t pthread_t1;
pthread_t pthread_t2;
GL_Status_t Test_GSL_OS_GetFileSize( void )
{
	GL_INT32 fd;
	GL_Status_t iRet;
	GL_UINT32 FileSize = 0;

	fd = open("1.html", 'r');
    if(fd<0)
    {
        printf("hanjun debug:open file failed!!\r\n");
    }
//    printf("hanjun debug: open file fd is %d\r\n", fd);
    iRet = GL_GetFileSize(fd, &FileSize);
	if( iRet == GL_SUCCESS )
	{
//		printf("hanjun debug: get file size is %d\r\n", FileSize);
//		printf("hanjun debug: get file size is 0x%x\r\n", FileSize);
	}
	close(fd);

	return GL_SUCCESS;
}
void *thr_fn1( void *arg )
{
#if 0
	pid_t pthreadID;
	GL_INT32 ulStackSize = 0;
	GL_UINT32 *pAddr;

	gl_thread_get_stack_base(pthread_t1, (void*)pAddr);
	printf("hanjun debug: gl_thread_get_stack_base11111 is 0x%x!!\r\n", (GL_UINT32)pAddr);
	pthreadID = gl_thread_get_id(  );
	printf("hanjun debug: gl_thread_get_id1 is 0x%x!!\r\n", (GL_UINT32)pthreadID);
#endif
	printf("hanjun dbueg: thread 1 returning\r\n");
    return ((void *)1);
}
void *thr_fn2( void *arg )
{
#if 0
	pid_t pthreadID;
	GL_INT32 ulStackSize = 0;
	GL_UINT32 *pAddr;

	gl_thread_get_stack_base(pthread_t2, (void*)pAddr);
	printf("hanjun debug: gl_thread_get_stack_base22222 is 0x%x!!\r\n", (GL_UINT32)pAddr);
	ulStackSize = gl_thread_get_stack_size( pthread_t2 );
	printf("hanjun debug: gl_thread_get_stack_size222222 is 0x%x!!\r\n", (GL_UINT32)ulStackSize);
	pthreadID = 0;
	pthreadID = gl_thread_get_id(  );
	printf("hanjun debug: gl_thread_get_id2 is 0x%x!!\r\n", (GL_UINT32)pthreadID);
#endif
	printf("hanjun dbueg: thread 2 returning\r\n");

	return ((void *)1);
}
GL_Status_t Test_GSL_OS_GetpthreadSelf(void)
{
//	pthread_t pthread_t1;
//	pthread_attr_t pthread_attr_t1;
//	pthread_t pthread_t2;
//	pthread_attr_t pthread_attr_t2;
	GL_INT32 err;
//	GL_INT32 ulStackSize = 0;
//	pid_t pthreadID = 0;

//	err = pthread_attr_init(&pthread_attr_t1);
///	if(err != 0)
//	{
//		printf("hanjun debug: pthread_attr_init failed!!\r\n");
//		return GL_FAILURE;
//	}
	err = pthread_create ( &pthread_t1, NULL, (void *)thr_fn1, NULL);
	if( err != 0 )
	{
		printf("hanjun debug: Test_GSL_OS_GetpthreadSelf pthread_create failed!!\r\n");
		return GL_FAILURE;
	}
//	pthread_attr_destroy(&pthread_attr_t1);
//	ulStackSize = gl_thread_get_stack_size( pthread_t1 );
//	printf("hanjun debug: gl_thread_get_stack_size is 0x%x!!\r\n", (GL_UINT32)ulStackSize);

//	pthreadID = gl_thread_get_id(  );
//	printf("hanjun debug: gl_thread_get_id is 0x%x!!\r\n", (GL_UINT32)pthreadID);

	err = pthread_create ( &pthread_t2, NULL, (void *)thr_fn2, NULL);
	if( err != 0 )
	{
		printf("hanjun debug: Test_GSL_OS_GetpthreadSelf pthread_create failed!!\r\n");
		return GL_FAILURE;
	}

	sleep(1);

	return GL_SUCCESS;
}

GL_Status_t Test_Gsl_OS( void )
{
	GL_Status_t iRet;

	iRet = Test_GSL_OS_GetFileSize();
	if( iRet != GL_SUCCESS )
	{
		printf("hanjun debug: Test Get file size failed !!\r\n");
	}
	else
	{
		printf("hanjun debug: Test Get file size Successed !!\r\n");
	}

	iRet = Test_GSL_OS_GetpthreadSelf();
	if( iRet != GL_SUCCESS )
	{
		printf("hanjun debug: Test_GSL_OS_GetpthreadSelf failed !!\r\n");
	}
	else
	{
		printf("hanjun debug: Test_GSL_OS_GetpthreadSelf Successed !!\r\n");
	}
    Test_usSleepProc(5);
    Test_usSleepProc(50);
    Test_usSleepProc(500);
    Test_usSleepProc(5000);
    Test_usSleepProc(10000);

	return iRet;
}

GL_INT32 Test_usSleepProc(GL_INT32 testtimeval)
{
    struct timeval tv1,tv2;
    GL_INT32 err;
//    GL_UINT32 usoffset, usinit,uscurrent;
//    GL_INT32 testtimeval;
    GL_INT32 delayerrnum;

    err = gettimeofday(&tv1, NULL);
    if(err != 0)
    {
        printf("hanjun debug: test gettimeofday failed!!\r\n");
    }
//    testtimeval = 5000;
    GL_DelayUs(testtimeval);
    err = gettimeofday(&tv2, NULL);
    if(err != 0)
    {
        printf("hanjun debug: test gettimeofday failed!!\r\n");
    }
    delayerrnum = abs(tv2.tv_usec-tv1.tv_usec-testtimeval);
//    printf("hanjun debug: usdelay %d delayerrnum is %d\r\n",testtimeval,delayerrnum);
    if(delayerrnum<=3)
    {
        // printf("hanjun debug:usdelay %d delayerrnum is %d\r\n",testtimeval,delayerrnum);
        printf("hanjun debug: usdelay %d pass!!!\r\n", testtimeval);
    }
    else
    {
        printf("hanjun debug: usdelay %d delayerrnum is %d\r\n",testtimeval,delayerrnum);
    }


    return 1;
 }


