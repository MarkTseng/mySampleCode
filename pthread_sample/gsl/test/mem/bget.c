#include "gsl.h"

#define MALLOC_MAX 0x1000  //max malloc size
#define TEST_TIMES  1000

#define HANDLER_MAX 0x100  //max number of  buf handlers

#define RUN_STEP 	0
#define RUN_AUTO	1

static unsigned long int randseed = 1;
static unsigned long int runmode = RUN_AUTO;
static char* buf[HANDLER_MAX+1];


void test_bget_srand(unsigned int seed)
{
	GL_TraceInfo("test_bget_srand: %d\r\n", seed);
	randseed = seed;
}

int test_bget_rand()
{
	randseed = randseed * 1103515245L + 12345;
	return (unsigned int) (randseed / 65536L) % 32768L;
}

void test_bget_inithandle(void)
{
	int i;

	for (i = 0; i < HANDLER_MAX; i++) {
		buf[i] = NULL;
	}
}

int  test_bget_get_alloca_handle(void)
{
	int i;

	for (i = 0; i < HANDLER_MAX; i++) {
		if(NULL == buf[i]){
			break;
		}
	}
	return i;
}

int  test_bget_get_free_handle(void)
{
#if 1
	return test_bget_rand() & (HANDLER_MAX-1);
#else
	int i = 0;
	int count = 0;
	int randidx = test_bget_rand() & (HANDLER_MAX-1);

	if(buf[randidx] != NULL){
		return randidx;
	}

	for (i = 0; i < HANDLER_MAX; i++) {
		if(NULL != buf[i]){
			count++;
		}
	}

	if(count == 0) {
		return 0;
	}

	return randidx%(5*count); //can adjust rate of hit, yet may be null;


	randidx%=count;
	count = 0;

	for (i = 0; i < HANDLER_MAX; i++) {
		if(NULL != buf[i]){
			if(randidx == count){
				return i;
			}
			count++;
		}
	}

#endif
}

int bget_maintest(void)
{
	int i;
	long size;
	char* retbuf;
	int idx;

	test_bget_inithandle();

	/* Seed the random number generator.  If Repeatable is defined, we
	always use the same seed.  Otherwise, we seed from the clock to
	shake things up from run to run. */
	//test_bget_srand(0x1234);
	test_bget_srand((int) time((long *) NULL));

	if(RUN_AUTO != runmode){
		GL_TraceInfo("Please input the runmode(r/s/q): ");
		char c = getchar();
		GL_TraceInfo("\r\n");
		if(c == 'r'){
			runmode = RUN_AUTO;
		}else if(c == 'q'){
			return 0;
		}
	}


	for (i = 0; i < TEST_TIMES; i++) {

		GL_TraceInfo("\r\n\r\n\r\n\r\n****************************[MainTest Loop:%6d]****************************\r\n", i);

		/* Based on a random size, alloca a buffer and store the addr in first NULL buf[]. */
		idx = test_bget_get_alloca_handle();
		if (idx < HANDLER_MAX) {
			unsigned int rand;

			size = test_bget_rand() & (MALLOC_MAX - 1);
			if (size < 4) {
				size = 4;
			}

			rand = test_bget_rand() % 3;
			if (0 == rand) {
				GL_TraceInfo("bget_malloc ");
				retbuf = (char *) bget_malloc(size);
			} else if (1 == rand) {
				GL_TraceInfo("bget_mallocB");
				retbuf = (char *) bget_mallocB(size);
			} else {
				GL_TraceInfo("bget_mallocA");
				retbuf = (char *) bget_mallocA(size);
			}

			GL_TraceInfo("\tsize=%d", (unsigned int)size);

			if (retbuf == NULL) {
				GL_TraceInfo(" fail\r\n");
				bget_meminfo();
				//continue;
			} else {
				buf[idx] = retbuf;
				GL_TraceInfo(" buf[%d]=0x%x\r\n", idx, (unsigned int)retbuf);
			}
		}


		/* Based on a random cast, release a random buffer in the array buf[]. NOTE: maybe is not allocated */
		idx = test_bget_get_free_handle();
		GL_TraceInfo("bget_free\t\tidx=%d, buf=0x%x\r\n", idx, (unsigned int)buf[idx]);
		if(buf[idx] != NULL){
			bget_free(buf[idx]);
			buf[idx] = NULL;
		}

		#if 1
		/* Based on a random cast, reallocate a random buffer in the list to a random size */
		idx = test_bget_get_free_handle();
		size = test_bget_rand() & (MALLOC_MAX - 1);
		if(size<4){
			size = 4;
		}
		GL_TraceInfo("bget_realloc\tidx=%d, oldbuf=0x%x, newsize=%d", idx, (unsigned int)buf[idx], (unsigned int)size);
		if(buf[idx] != NULL){
			retbuf = bget_realloc(buf[idx], size);
			if (retbuf == NULL) {
				GL_TraceInfo(" fail\r\n");
				//buf[idx] = NULL;  must reserv this pointer for free
				//continue;
			}else{
				buf[idx] = retbuf;
				GL_TraceInfo("newbuf=0x%x\r\n", (unsigned int)buf[idx]);
			}
		}else{
			GL_TraceInfo("\r\n");
		}
		#endif

		bget_meminfo();

		if(RUN_AUTO != runmode){
			char c = getchar();
			if(c == 'r'){
				runmode = RUN_AUTO;
			}else if(c == 'q'){
				break;
			}
		}
	}

	GL_TraceInfo("\r\n\r\n\r\n\r\n****************************Free all the buf allocated****************************\r\n");
	for(i=0; i<HANDLER_MAX; i++){
		if(buf[i] != NULL){
			bget_free(buf[i]);
			buf[i] = NULL;
		}
	}
	bget_meminfo();

	return 0;
}


int main(int argc, char *argv[])
{
	int ret = 0;
//	char *buf[16];

	GL_TraceInfo("========================== test  begin ==========================\r\n");

	GL_TraceInfo(">>>>test bget_meminit\r\n");

 	if(0 != ret){
 		goto __EXIT;
 	}
	GL_Init();
	bget_meminfo();

#if 0
	GL_TraceInfo(">>>>test bget_malloc 0\r\n");
	buf[0] = (char*)bget_malloc(10*1024);
	if(buf[0]){
		printf("malloc buf[0] %x\r\n", buf[0]);
	}else{
		goto __EXIT;
	}
	bget_meminfo();

	GL_TraceInfo(">>>>test bget_malloc 1\r\n");
	buf[1] = (char*)bget_malloc(40*1024);
	if(buf[1]){
		printf("malloc buf[1] %x\r\n", buf[1]);
	}else{
		goto __EXIT;
	}

	bget_meminfo();

	GL_TraceInfo(">>>>test bget_free 0\r\n");
	bget_free(buf[0]);
	bget_meminfo();

	GL_TraceInfo(">>>>test bget_free 1\r\n");
	bget_free(buf[1]);
	bget_meminfo();

	//GL_TraceInfo(">>>>test bget_free the same\r\n");
	//bget_free(buf[1]);
	//bget_meminfo();

	GL_TraceInfo(">>>>test bget_free NULL\r\n");
	bget_free(NULL);
	bget_meminfo();

#else

	ret = bget_maintest();
#endif


__EXIT:
	if(ret){
		GL_TraceError("%s test fail!\r\n", argv[0]);
	}else{
		GL_TraceError("%s test pass!\r\n", argv[0]);
	}

//	bget_memdeinit();
	GL_Uninit();

	GL_TraceInfo("========================== test  exit ==========================\r\n");

	return ret;

}

