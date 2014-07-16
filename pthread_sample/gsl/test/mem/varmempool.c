#include "gsl.h"

#define TEST_VARMEM_POOLSIZE    1024

static GL_UINT32 sizetable[32] = {0x80, 0x100, 0x80, 0x10, 0xF0, 0x100, 0xF00};
static GL_UINT32 ordertable[32] = {1,3,5,0,2,4};
int main(int argc, char *argv[])
{
    GL_MEMPOOL_t pvarpool;
    GL_UINT8 *memvarpool = NULL;
    GL_UINT32 *buf[32]= {0};
    int i, ret = 0;

    GL_TraceInfo("========================== test  begin ==========================\r\n");

//     BGET_INIT_ONCE();
	GL_Init();

    memvarpool = (GL_UINT8 *)malloc(TEST_VARMEM_POOLSIZE);
    if (NULL == memvarpool) {
        ret = 1;
        goto __EXIT;
    }

    if (GL_SUCCESS != GL_MempoolVarCreate("varmempool", memvarpool, TEST_VARMEM_POOLSIZE, &pvarpool)) {
        GL_TraceError("%s GL_MempoolVarCreate fail!\r\n", argv[0]);
        ret = 1;
        goto __EXIT;
    }
    GL_MempoolVarInfo(pvarpool);

    GL_TraceInfo(">>>>test malloc 0 size \r\n");

    buf[0] = (GL_UINT32 *)GL_MempoolVarAlloc(pvarpool, 0, GL_NO_WAIT);
    if (!buf[0]) {
        GL_TraceError("zero-size alloc test pass!\n");
    } else {
        GL_TraceError("zero-size alloc test fail! ret addr = 0x%08X\r\n", (unsigned int)buf[0]);
    }

    GL_TraceInfo(">>>>test malloc normal \r\n");
    for (i = 0; i < 6; i++) {
        buf[i] = (GL_UINT32 *)GL_MempoolVarAlloc(pvarpool, sizetable[i], GL_NO_WAIT);
        if (buf[i]) {
            GL_TraceError("%s GL_MempoolVarAlloc buf[%d] success! addr=0x%x\r\n", argv[0], i, (unsigned int)buf[i]);
            memset(buf[i], i, sizetable[i]);
            GL_MempoolVarInfo(pvarpool);
        } else {
            GL_TraceError("%s GL_MempoolVarAlloc buf[%d] fail!\r\n", argv[0], i);
            ret = 1;
            goto __EXIT;
        }
    }


    GL_TraceInfo(">>>>test malloc when no space \r\n");
    buf[i] = (GL_UINT32*)GL_MempoolVarAlloc(pvarpool, sizetable[i], GL_NO_WAIT);
    if (buf[i]) {
        GL_TraceError("%s GL_MempoolVarAlloc buf[%d] success! addr=0x%x\r\n", argv[0], i, (unsigned int)buf[i]);
    } else {
        GL_TraceError("%s GL_MempoolVarAlloc buf[%d] fail!\r\n", argv[0], i);
    }

    #if 1
    GL_TraceInfo(">>>>test the write and read\r\n");
    for (i = 0; i < TEST_VARMEM_POOLSIZE; i++) {
        if(0 == (i % 16)){
            GL_TraceInfo("\r\n");
        }
        GL_TraceInfo("%x ", memvarpool[i]);
    }
    GL_TraceInfo("\r\n");
    #endif


    GL_TraceInfo(">>>>test free normal\r\n");
    for (i = 0; i < 6; i++) {
        GL_TraceInfo("free buf[%d]=0x%x\r\n", ordertable[i], (unsigned int)buf[ordertable[i]]);
        GL_MempoolVarFree(pvarpool, buf[ordertable[i]]);
        GL_MempoolVarInfo(pvarpool);

        if(ordertable[i] == 0){
            buf[6] = (GL_UINT32*)GL_MempoolVarAlloc(pvarpool, 0x200, GL_NO_WAIT);
            if(buf[6]){
                GL_TraceError("%s GL_MempoolVarAlloc buf[%d] success! addr=0x%x\r\n", argv[0], i, (unsigned int)buf[i]);
            }else{
                GL_TraceError("%s GL_MempoolVarAlloc buf[%d] fail!\r\n", argv[0], i);
            }
            GL_MempoolVarInfo(pvarpool);
        }

        if(ordertable[i] == 2){
            buf[7] = (GL_UINT32*)GL_MempoolVarAlloc(pvarpool, 0x200, GL_NO_WAIT);
            if(buf[7]){
                GL_TraceError("%s GL_MempoolVarAlloc buf[%d] success! addr=0x%x\r\n", argv[0], i, (unsigned int)buf[i]);
            }else{
                GL_TraceError("%s GL_MempoolVarAlloc buf[%d] fail!\r\n", argv[0], i);
            }
            GL_MempoolVarInfo(pvarpool);
        }

    }

    GL_MempoolVarFree(pvarpool, buf[6]);
    GL_MempoolVarInfo(pvarpool);

    GL_MempoolVarFree(pvarpool, buf[7]);
    GL_MempoolVarInfo(pvarpool);

    GL_TraceInfo(">>>>test free NULL\r\n");
    GL_MempoolVarFree(pvarpool, 0);
    GL_MempoolVarInfo(pvarpool);

    GL_TraceInfo(">>>>test free twice\r\n");
    GL_MempoolVarFree(pvarpool, buf[0]);
    GL_MempoolVarInfo(pvarpool);

__EXIT:

    GL_TraceInfo(">>>> __EXIT\r\n");
     if(pvarpool){
        GL_MempoolVarDestroy(pvarpool);
    }

    if(memvarpool) {
        free(memvarpool);
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

