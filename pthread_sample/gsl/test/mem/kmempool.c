#include "gsl.h"

int main(int argc, char *argv[])
{
    void* pkmempool;
    GL_UINT32 nVarMpSize = 0x4000;
    GL_UINT32 nFixMpSize = 0x400;
    GL_UINT32 nFixMpBlockSize = 0x100;
    GL_UINT32 fixallocsize[4] = {0x100, 0x80, 0x100, 0x10};
    GL_UINT32 varallocsize[8] = {0x100, 0x80, 0x480, 0x180, 0x100, 0x80,};
    GL_UINT8* buf[32] = {0};
    int i, ret = 0;

    GL_TraceInfo("========================== test  begin ==========================\r\n");

    //BGET_INIT_ONCE();
	GL_Init();

    pkmempool = KMempoolOpen(nVarMpSize, nFixMpSize, nFixMpBlockSize);
    if(NULL == pkmempool){
        ret =1;
        goto __EXIT;
    }
    KMempoolInfo(pkmempool);

    GL_TraceInfo(">>>>test malloc size<=fixmempool_blocksize \r\n");
    for(i=0; i<4; i++){
        buf[i] = (GL_UINT8*)KMempoolAlloc(pkmempool, fixallocsize[i]);
        KMempoolInfo(pkmempool);
    }

    GL_TraceInfo(">>>>test malloc size<=fixmempool_blocksize when fixmempool full\r\n");
    for(i=0; i<2; i++){
        buf[i+4] = (GL_UINT8*)KMempoolAlloc(pkmempool, varallocsize[i]);
        KMempoolInfo(pkmempool);
    }

    GL_TraceInfo(">>>>test malloc size>fixmempool_blocksize when fixmempool full\r\n");
    for(i=2; i<6; i++){
        buf[i+4] = (GL_UINT8*)KMempoolAlloc(pkmempool, varallocsize[i]);
        KMempoolInfo(pkmempool);
    }

    GL_TraceInfo(">>>>test malloc size<=fixmempool_blocksize again when fixmempool has space\r\n");
    KMempoolFree(pkmempool, buf[0]);
    KMempoolInfo(pkmempool);

    buf[0] = (GL_UINT8*)KMempoolAlloc(pkmempool, fixallocsize[0]);
    KMempoolInfo(pkmempool);

    GL_TraceInfo(">>>>test malloc varmempool the min space to alloc\r\n");
    KMempoolFree(pkmempool, buf[6]);
    KMempoolInfo(pkmempool);

    KMempoolFree(pkmempool, buf[8]);
    KMempoolInfo(pkmempool);

    buf[6] = (GL_UINT8*)KMempoolAlloc(pkmempool, 0x80);
    KMempoolInfo(pkmempool);

    buf[8] = (GL_UINT8*)KMempoolAlloc(pkmempool, 0x10);
    KMempoolInfo(pkmempool);

    GL_TraceInfo(">>>>test free all\r\n");
    for(i=0; i<10; i++){
        KMempoolFree(pkmempool, buf[i]);
        KMempoolInfo(pkmempool);
    }

    KMempoolClose(pkmempool);

__EXIT:

    if(ret){
        GL_TraceError("%s test fail!\r\n", argv[0]);
    }else{
        GL_TraceError("%s test pass!\r\n", argv[0]);
    }

	GL_Uninit();

    GL_TraceInfo("========================== test  exit ==========================\r\n");
    return ret;

}

