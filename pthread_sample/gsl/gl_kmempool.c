//
// FILE
// mempool.c
//
// DESCRIPTION
// kernel interface
//

/*******************
* INCLUDE FILES    *
********************/
#include <gsl.h>
#include "gl_common.h"

typedef struct tagKernelMempool {
	// var mempool
	void *pVarMp;
	GL_UINT32 nVarMpSize;
	GL_MEMPOOL_t stVarMpObj;

	// fix mempool
	void *pFixMp;
	GL_UINT32 nFixMpSize;
	GL_UINT32 nFixMpBlockSize;
	GL_MEMPOOL_t stFixMpObj;

	GL_UINT32 nFixMpStart;
	GL_UINT32 nFixMpEnd;
} KERN_MEMPOOL, *LP_KERN_MEMPOOL;

/**********************
* FUNCTION PROTOTYPES *
***********************/

void *KMempoolOpen(GL_UINT32 nVarMpSize, GL_UINT32 nFixMpSize, GL_UINT32 nFixMpBlockSize)
{
    if (nVarMpSize == 0) {
        return NULL;
    }

    KERN_MEMPOOL *pKerMp = NULL;
    pKerMp = (KERN_MEMPOOL*)malloc(sizeof(KERN_MEMPOOL));
    if (pKerMp == NULL) {
        GL_TraceError("%s: << malloc KERN_MEMPOOL fail\n", __FUNCTION__);
        return NULL;
    }

    memset(pKerMp, 0, sizeof(KERN_MEMPOOL));

    pKerMp->nVarMpSize = nVarMpSize;
    pKerMp->nFixMpSize = nFixMpSize;
    pKerMp->nFixMpBlockSize = nFixMpBlockSize;

    // var mempool
    pKerMp->pVarMp = malloc(pKerMp->nVarMpSize);
    if (pKerMp->pVarMp == NULL) {
        GL_TraceError("%s: << malloc var mempool space fail\n", __FUNCTION__);
        free(pKerMp);
        pKerMp = NULL;
        return NULL;
    }

    if(GL_SUCCESS != GL_MempoolVarCreate("kvarmempool", pKerMp->pVarMp, pKerMp->nVarMpSize, &(pKerMp->stVarMpObj))){
        GL_TraceError("%s: <<  GL_MempoolVarCreate fail\n", __FUNCTION__);
        free(pKerMp->pVarMp);
        free(pKerMp);
        pKerMp = NULL;
        return NULL;
    }


    // fix mempool
    if (pKerMp->nFixMpSize && pKerMp->nFixMpBlockSize) {
        pKerMp->pFixMp = malloc(pKerMp->nFixMpSize);

        if(pKerMp->pFixMp == NULL) {
            GL_TraceError("%s: << malloc fix mempool space fail\n", __FUNCTION__);
            return (void *)pKerMp;
        }

        if(GL_SUCCESS !=  GL_MempoolFixCreate("kfixmempool", pKerMp->pFixMp, pKerMp->nFixMpSize,
                                                                        pKerMp->nFixMpBlockSize, &(pKerMp->stFixMpObj))){
            GL_TraceError("%s: <<  GL_MempoolFixCreate fail\n", __FUNCTION__);
            free(pKerMp->pFixMp);
            pKerMp->pFixMp = NULL;
            return (void *)pKerMp;
        }

        pKerMp->nFixMpStart = (GL_UINT32)pKerMp->pFixMp;
        pKerMp->nFixMpEnd = pKerMp->nFixMpStart + pKerMp->nFixMpSize;
    }

    return (void *)pKerMp;

}

void KMempoolClose(void *hKerMp)
{
    KERN_MEMPOOL *pKerMp = (KERN_MEMPOOL *)hKerMp;
    if (pKerMp->pVarMp) {
        GL_MempoolVarDestroy(pKerMp->stVarMpObj);
        free(pKerMp->pVarMp);
    }

    if (pKerMp->pFixMp) {
        GL_MempoolFixDestroy(pKerMp->stFixMpObj);
        free(pKerMp->pFixMp);
    }

    free(pKerMp);
}

void *KMempoolAlloc(void *hKerMp, GL_UINT32 nMemSize)
{
    KERN_MEMPOOL *pKerMp = (KERN_MEMPOOL *)hKerMp;
    void *pData = NULL;

    if ((nMemSize <= pKerMp->nFixMpBlockSize) && pKerMp->pFixMp) {
        pData = GL_MempoolFixAlloc(pKerMp->stFixMpObj, GL_NO_WAIT);;
        if (pData) {
            return pData;
        }
        GL_TraceError("%s: << try alloc from fixmempool error, then malloc from varmempool\r\n", __FUNCTION__);
    }

    if (pKerMp->pVarMp) {
        pData = GL_MempoolVarAlloc(pKerMp->stVarMpObj, nMemSize, GL_INFINITE_WAIT);
    }

    return pData;
}

void KMempoolFree(void *hKerMp, void *pData)
{
    KERN_MEMPOOL *pKerMp = (KERN_MEMPOOL*)hKerMp;
    if (pKerMp->pFixMp) {
        if (((GL_UINT32)pData >= pKerMp->nFixMpStart) && ((GL_UINT32)pData < pKerMp->nFixMpEnd)) {
            // this memory belong to fix mempool
            GL_MempoolFixFree(pKerMp->stFixMpObj, pData);
            return;
        }
    }

    if (pKerMp->pVarMp) {
        GL_MempoolVarFree(pKerMp->stVarMpObj, pData);
    }
}

void KMempoolInfo(void *hKerMp)
{
    KERN_MEMPOOL *pKerMp = (KERN_MEMPOOL*)hKerMp;

    GL_MempoolFixInfo(pKerMp->stFixMpObj);

    GL_MempoolVarInfo(pKerMp->stVarMpObj);
}

