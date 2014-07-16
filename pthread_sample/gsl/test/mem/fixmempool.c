#include "gsl.h"

#define TEST_FIXMEM_BLOCKSIZE    1021
#define TEST_FIXMEM_BLOCKNUM    15

int main(int argc, char *argv[])
{
    GL_MEMPOOL_t pfixpool = NULL;
    GL_UINT8 *memfixpool = NULL;
    GL_UINT32 totalsize, blocksize, i;
    GL_UINT32 *buf[TEST_FIXMEM_BLOCKNUM+1];
    int ret = 0;

    GL_TraceInfo("========================== test  begin ==========================\r\n");

	GL_Init();

    blocksize = TEST_FIXMEM_BLOCKSIZE;
    totalsize = TEST_FIXMEM_BLOCKSIZE*TEST_FIXMEM_BLOCKNUM;

    memfixpool = (GL_UINT8*)malloc(totalsize );
    if(NULL == memfixpool) {
        ret = 1;
        goto __EXIT;
    }

    if(GL_SUCCESS != GL_MempoolFixCreate("fixmempool", memfixpool+1, totalsize, blocksize, &pfixpool)){
        GL_TraceError("%s GL_MempoolFixCreate fail!\r\n", argv[0]);
        ret = 1;
        goto __EXIT;
    }
    GL_TraceInfo(">>>>test malloc until no space \r\n");
    for(i = 0; i<TEST_FIXMEM_BLOCKNUM+1; i++){
        buf[i] = (GL_UINT32*)GL_MempoolFixAlloc(pfixpool, GL_NO_WAIT);
        if(buf[i]){
            GL_TraceError("%s GL_MempoolFixAlloc buf[%d] success! addr=0x%x\r\n", argv[0], i, (unsigned int)buf[i]);
            memset(buf[i], i, TEST_FIXMEM_BLOCKSIZE);
            GL_MempoolFixInfo(pfixpool);
        }else{
            GL_TraceError("%s GL_MempoolFixAlloc buf[%d] fail!\r\n", argv[0], i);
            //ret = 1;
            //goto __EXIT;
        }
    }

#if 1
    GL_TraceInfo(">>>>test the write and read\r\n");
    for(i = 0; i<totalsize; i++){
        if(0 == (i%16)){
            GL_TraceInfo("\r\n");
        }
        GL_TraceInfo("%x ", memfixpool[i]);
    }
    GL_TraceInfo("\r\n");
#endif

    GL_TraceInfo(">>>>test free normal\r\n");
    for(i = 0; i<TEST_FIXMEM_BLOCKNUM; i++){
        GL_MempoolFixFree(pfixpool, buf[i]);
        GL_MempoolFixInfo(pfixpool);
    }

    GL_TraceInfo(">>>>test free NULL\r\n");
    GL_MempoolFixFree(pfixpool, buf[i]);
    GL_MempoolFixInfo(pfixpool);

    GL_TraceInfo(">>>>test free twice\r\n");
    GL_MempoolFixFree(pfixpool, buf[0]);
    GL_MempoolFixInfo(pfixpool);


__EXIT:

    if(pfixpool){
        GL_MempoolFixDestroy(pfixpool);
    }
    if(memfixpool) {
        free(memfixpool);
    }

    if(ret){
        GL_TraceError("%s test fail!\r\n", argv[0]);
    }else{
        GL_TraceError("%s test pass!\r\n", argv[0]);
    }

    GL_TraceInfo("========================== test  exit ==========================\r\n");
	GL_Uninit();

    return ret;

}

