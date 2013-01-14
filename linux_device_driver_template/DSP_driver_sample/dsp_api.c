/******************************************************************************
 * Filename    : dsp_api.c
 * 
 * Description : audio dsp APIs implementation file
 * Language    : C Language 
 *           
 * Revision History: 
 * ------------------
 * [080915]hcsu Initial Draft
 *
 * -----------------------------------------------------------------------
 * Copyright (C) 2007-2009 Sunplus Technology Co., Ltd. 
 * All rights reserved. The information contained herein is the exclusive 
 * property of SUNPLUS TECHNOLOGY CO., LTD. and shall not be distributed, 
 * reproduced, or disclosed in whole or in part without prior written 
 * permission of SUNPLUS TECHNOLOGY CO., LTD.   
 *****************************************************************************/

/******************************************************************************
    Include files
 *****************************************************************************/ 
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/wait.h>

#include "include/setting.h"
//#include "stypes.h"

#include "include/aud_global.h"
#include "include/dsp_api.h"
#include "include/auddrv.h"
///#include "include/playlist.h"
///#include "gsl/gl_api.h"


/******************************************************************************
    Local Defines
 *****************************************************************************/ 
#define DEBUG_DSP_MODULE	
#ifdef DEBUG_DSP_MODULE
	#define ENABLE_PRINT_DSP_STATE_STRING
	//#define DEBUG_DUMP_CODE					// Dump IM code (256 bytes)
	//#define DEBUG_DUMP_ROMTABLE
	#define DEBUG_CODE_CHECKSUM
	#define DEBUG_ROMTABLE_CHECKSUM	
	#define DEBUG_DUMP_DSP_DRAMMAP
#endif	

/******************************************************************************
    global & static variables
 *****************************************************************************/
#ifdef ENABLE_PRINT_DSP_STATE_STRING
const char* STATE_STRING_TBL[] = {"DSP_STATE_SYSTEM","DSP_STATE_DEC1","DSP_STATE_DEC2","DSP_STATE_ENC"};
#define GET_STATE_STR(x)  	STATE_STRING_TBL[x]
#endif

// a flag record if force to reset dsp or not
UINT16 gDSP_waitstate_timeout=0;

/******************************************************************************
    Function Headers
 *****************************************************************************/


/******************************************************************************
    Function Implementations
 *****************************************************************************/

/******************************************************************************
    dsp Download related
 *****************************************************************************/

/*!
  \brief  download codec dsp codes into desird code banks
  \par    detail description:\n
  \param  CodecID    [IN]codec id
  \param  codeBankID [IN]code bank id
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   code bank == codec bank, need to unified name
  \todo   1. unified term such as code bank and codec bank\n
          2. decompress algorithm (unzip) needs to be introduced
 */
static UINT32 DSP_DownloadCode(UINT16 CodecID, UINT8 codeBankID)
{
    UINT8* pCodeBank = NULL;
	DspCode* pDspCode = DSP_GetDspCodePtr(CodecID);
	//ASSERT(NULL != pDspCode,"DSP_DownloadCode::pDspCode is NULL\n");   

    switch (codeBankID)
    {
    case CODE_BANK0:        
        pCodeBank = (UINT8* )DSP_GetInfo(INFO_CODE_BANK0_BASE);		
        break;
    case CODE_BANK1:
        pCodeBank = (UINT8* )DSP_GetInfo(INFO_CODE_BANK1_BASE);		
		HWREG_W(dsp_inst_page[1], ((HWREG_R(dsp_inst_page[1])&(~0x1f000000)) | (CodecID<<24) | (0x1<<31)));		
        DSP_PRINT("DSP_DownloadCode::HWREG_R(dsp_inst_page1):0x%08x\n",HWREG_R(dsp_inst_page[1]));               
        break;
    case CODE_BANK2:    
        pCodeBank = (UINT8* )DSP_GetInfo(INFO_CODE_BANK2_BASE);		
		HWREG_W(dsp_inst_page[2], ((HWREG_R(dsp_inst_page[2])&(~0x1f000000)) | (CodecID<<24) | (0x1<<31)));		
        DSP_PRINT("DSP_DownloadCode::HWREG_R(dsp_inst_page2):0x%08x\n",HWREG_R(dsp_inst_page[2]));                            
        break;      
    case CODE_BANK3:              
		pCodeBank = (UINT8* )DSP_GetInfo(INFO_CODE_BANK3_BASE);		
   		HWREG_W(dsp_inst_page[3], ((HWREG_R(dsp_inst_page[3])&(~0x1f000000)) | (CodecID<<24) | (0x1<<31)));		
        DSP_PRINT("DSP_DownloadCode::HWREG_R(dsp_inst_page3):0x%08x\n",HWREG_R(dsp_inst_page[3]));           
        break;     
    case CODE_BANK4:              
		pCodeBank = (UINT8* )DSP_GetInfo(INFO_CODE_BANK4_BASE);		
		HWREG_W(dsp_inst_page[4], ((HWREG_R(dsp_inst_page[4])&(~0x1f000000)) | (CodecID<<24) | (0x1<<31)));		
        DSP_PRINT("DSP_DownloadCode::HWREG_R(dsp_inst_page4):0x%08x\n",HWREG_R(dsp_inst_page[4]));           
        break;    		
    default:
        ERROR_PRINT("DSP_DownloadCode::e:Unknown codebank\n");
        return DSP_ERROR_DOWNLOAD;        
    }
          
    DSPAPI_PRINT("DSP_DownloadCode::ID:0x%02x, pCodeBank:0x%x\n",CodecID,(UINT32)pCodeBank);
    DSP_Memcpy(pCodeBank, pDspCode->pCode, 	pDspCode->CodeInfo.dwCodeSize);	

#ifdef DEBUG_CODE_CHECKSUM
	UINT32 nChecksum = DSP_GenChecksum(pCodeBank,pDspCode->CodeInfo.dwCodeSize);
	DSP_PRINT("DSP_DownloadCode::checksum:0x%08x, golden:0x%x\n",nChecksum,(UINT32)pDspCode->CodeInfo.dwCodeChksum);
	//ASSERT(nChecksum == pDspCode->CodeInfo.dwCodeChksum,"DSP_DownloadCode::code checksum error\n");
#endif  

#ifdef DEBUG_DUMP_CODE
	   dumpHex((UINT32)pCodeBank,pDspCode->CodeInfo.dwCodeSize/2);	
#endif

    return DSP_ERROR_NONE;
}


/*!
  \brie   download romtable into desird code banks
  \par    detail description:\n
  \param  CodecID    [IN]codec id
  \param  codeBankID [IN]code bank id
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \todo   1. unified term such as code bank and codec bank\n
          2. decompress algorithm (unzip) needs to be introduced
 */
static UINT32 DSP_DownloadData(UINT16 CodecID, UINT8 codeBankID)
{
	UINT32 dramStaticDest = 0;
	DspCode* pDspCode = DSP_GetDspCodePtr(CodecID);
	//ASSERT(NULL != pDspCode,"DSP_DownloadData::pDspCode is NULL\n"); 

	if (NULL == pDspCode->pRomtable) return DSP_ERROR_NONE;

	switch (codeBankID)
    {
    case CODE_BANK0:        
        dramStaticDest = DSP_GetInfo(INFO_MAIN_STATIC_BASE); 
		DSPAPI_PRINT("DSP_DownloadData::CODE_BANK0:0x%08x\n",dramStaticDest);               
        break;
    case CODE_BANK1:
        dramStaticDest = DSP_GetInfo(INFO_DEC1_STATIC_BASE); 
        DSPAPI_PRINT("DSP_DownloadData::CODE_BANK1:0x%08x\n",dramStaticDest);               
        break;
    case CODE_BANK2:    
        dramStaticDest = DSP_GetInfo(INFO_DEC2_STATIC_BASE); 
        DSPAPI_PRINT("DSP_DownloadData::CODE_BANK2:0x%08x\n",dramStaticDest);                            
        break;      
    case CODE_BANK3:            
        dramStaticDest = DSP_GetInfo(INFO_ENC_STATIC_BASE); 
        DSPAPI_PRINT("DSP_DownloadData::CODE_BANK3:0x%08x\n",dramStaticDest);           
        break;          
    default:
        ERROR_PRINT("DSP_DownloadData::e:Unknown codebank\n");
        return DSP_ERROR_DOWNLOAD;        
    }

	if (0 == dramStaticDest)
	{
	    ERROR_PRINT("DSP_DownloadData::e:dram static address == 0\n");
		return DSP_ERROR_DOWNLOAD;
	}

	 DSP_Memcpy((void*)dramStaticDest, pDspCode->pRomtable, pDspCode->CodeInfo.dwRomtableSize);

#ifdef DEBUG_ROMTABLE_CHECKSUM
    UINT32 nChecksum = DSP_GenChecksum((const UINT8*)dramStaticDest, pDspCode->CodeInfo.dwRomtableSize);
	DSP_PRINT("DSP_DownloadData::checksum:0x%08x, golden:0x%x\n",nChecksum,(UINT32)pDspCode->CodeInfo.dwRomtableChksum);
	//ASSERT(nChecksum == pDspCode->CodeInfo.dwRomtableChksum,"DSP_DownloadData::romtable checksum error\n");
#endif  	 

#ifdef DEBUG_DUMP_ROMTABLE 
    dumpHex(dramStaticDest, pDspCode->CodeInfo.dwRomtableSize/2); 
#endif        

    return DSP_ERROR_NONE;
}


UINT32 DSP_Download(UINT16 CodecID, UINT8 codeBankID)
{
	DSP_DownloadCode(CodecID,codeBankID);
	
	DSP_DownloadData(CodecID,codeBankID);

	return DSP_ERROR_NONE;
	}
        
        
UINT32 DSP_InitSystem(void)
{
	DSPAPI_PRINT("DSP_InitSystem::initial dsp system\n");        
	
	DspCode* pDspCode = DSP_GetDspCodePtr(ID_MAIN);
	//ASSERT(NULL != pDspCode,"DSP_InitSystem::pDspCode is NULL\n");   	
        
    DSP_SHUTDOWN();
	
    DSP_RESET();                
	
	if (IS_SCRAMBLE(pDspCode->CodeInfo.Attribute))
	{
		CHANGE_CODE_KEY(pDspCode->CodeInfo.dwCodeKey);
	}
	else
	{
		DIS_CODE_SCRAMBLE();
	}
	
	DSP_Download(ID_MAIN, CODE_BANK0);

	DSP_Download(ID_SWAPP, CODE_BANK4);
	
	DSP_InitDSPMailbox();	

#ifdef ENABLE_DSP_ROM_2T
	DSP_GO_2T_ROM();
	//DSP_GO_2T_PREFETCH_4();
#else
    DSP_GO();	
#endif

	// Wait system run to MAIN_STATE_READY state
	DSP_WaitState(DSP_STATE_SYSTEM, MAIN_STATE_READY);

	// avoid state transition fail
	//GL_TaskSleep(10);
	msleep(10);

	// Wait system run to MAIN_STATE_IDLE state
	DSP_SetSystemIdle();

#ifdef DEBUG_DUMP_DSP_DRAMMAP
	DSP_DumpMemmap();
#endif	 
	
	// Wait system run to MAIN_STATE_RUN state
	DSP_SetSystemRun();

    DSPAPI_PRINT("DSP_InitSystem::HWREG_R(rdif_ctrl):0x%08x:0x%08x\n",(UINT32)&HWREG_R(rdif_ctrl),(UINT32)HWREG_R(rdif_ctrl));
	
    return DSP_ERROR_NONE;	
}

UINT32 DSP_InitCodec(UINT8 codecBankID)
{
	UINT16 codecID;
	UINT8 cmdCode;
	UINT16* pCodecParams = NULL;
	UINT8 codecState;
	UINT8 stateType;
	UINT16 codeBankID;
	UINT8 bsBufID;
	
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		pCodecParams = (UINT16*)DSP_GetInfo(INFO_RISC_DEC1_PARAM_BASE);	
		cmdCode = CODE0x01_DEC1_FLOW;
		stateType = DSP_STATE_DEC1;
		codeBankID = CODE_BANK1;
		bsBufID = BSBUF_ID_DEC1;
		break;
	case CODEC_BANK_DEC2:
		pCodecParams = (UINT16*)DSP_GetInfo(INFO_RISC_DEC2_PARAM_BASE);		
		cmdCode = CODE0x02_DEC2_FLOW;
		stateType = DSP_STATE_DEC2;
		codeBankID = CODE_BANK2;
		bsBufID = BSBUF_ID_DEC2;
		break;
	case CODEC_BANK_ENC:
		pCodecParams = (UINT16*)DSP_GetInfo(INFO_RISC_ENC_PARAM_BASE);		
		cmdCode = CODE0x03_ENC_FLOW;
		stateType = DSP_STATE_ENC;
		codeBankID = CODE_BANK3;
		bsBufID = BSBUF_ID_ENC;
		break;		
	default:				
		DSP_PRINT("DSP_InitCodec::wrong codecBankID:%d\n",codecBankID);		
		return DSP_ERROR_DOWNLOAD;		
	}
	
#ifdef DEBUG_DUMP_DRAM_PARAMS
    dumpWord16(pCodecParams,16);    
#endif  

	codecID = WORD16(pCodecParams[0]);
    DSPAPI_PRINT("DSP_InitCodec::get codecID:0x%x !\n",codecID);

	// Should check decoder task state machine
	//   if TASK_INIT => TASK is not initialed, ignore downloading
	//   if TASK_READY => able to downloading (boot up mode)
	//   if TASK_STOP => switch to TASK_READY then downloading
	//   if TASK_PLAY => switch to TASK_STOP

	codecState = DSP_GetState(stateType);
	DSPAPI_PRINT("DSP_InitCodec::get codec state:%d !\n",codecState);

	if(TASK_INIT == codecState)
	{
		ERROR_PRINT("DSP_InitCodec::e:Task not initialized\n");
		return DSP_ERROR_DOWNLOAD;
	}
	
	DSP_SetCodecStop(codecBankID);

	DSP_SetCodecReady(codecBankID); // may replace by update risc parameters commands 
		
	//DSP_SetSystemIdle();	


	DSP_Download(codecID,codeBankID);	

	// Check if needs to change system configurations, if yes, needs to issue systemReady 
	
 	//DSP_SetSystemRun();		

#ifdef DEBUG_DUMP_DSP_DRAMMAP
	DSP_DumpMemmap();
#endif

	DSP_SetCodecStop(codecBankID);

	DSP_SetCodecPlay(codecBankID);

	DSP_SetSampleRate(0x00FFFFFF);	// initial sample rate of dec1 (temp)
	
	//----------------------------------------------------------------
	// enable FIFO
	// bit  0 - PCM enable
	// bit  1 - SPDIF0 enable
	// bit  2 - RISC fifo enable (QAE377B)
	// bit  3 - External ADC enable
	// bit  4 - N/A
	// bit  5 - Internal ADC enable
	// bit  6 - SPDIF0 IN enable 
	// bit  7 - Record enable  
	// bit  8 - SPDIF1 enable  
	// bit  9 - SPDIF1 IN enable  
	// bit 10 - SPDIF2 IN enable 	
	//----------------------------------------------------------------	
	DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x7FF,0x0);
	
	return DSP_ERROR_NONE;
}



/******************************************************************************
    dsp command protocols
 *****************************************************************************/ 
void DSP_Waitloop(UINT32 nCount)
{
	UINT32 i,dspMsg;

	for (i=0; i<nCount; i++)
	{		
		DSP_GetMessage(&dspMsg);
		//GL_TaskSleep(1);
		//msleep(1);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout (1);
	}
} 

void DSP_Waitlock(UINT32 nCount)
{
	UINT32 i,dspMsg;

	for (i=0; i<nCount; i++)
	{		
		DSP_GetMessage(&dspMsg);	
	}
} 

UINT8 DSP_GetState(UINT8 type)
{
	UINT32 dspState = HWREG_R(dsp_port[12]);
	UINT8 state = 0;

	switch(type)
	{
	case DSP_STATE_SYSTEM:
		state = (UINT8)(dspState&0xF);
		break;
	case DSP_STATE_DEC1:
		state = (UINT8)((dspState>>4)&0xF);
		break;
	case DSP_STATE_DEC2:
		state = (UINT8)((dspState>>8)&0xF);
		break;
	case DSP_STATE_ENC:
		state = (UINT8)((dspState>>12)&0xF);		
		break;
	default:
		state = -1;
	}

	return state;	
}



UINT32 DSP_WaitState(UINT8 type, UINT8 state)
{
	DSPCMD_MUTEX_LOCK();
	
	UINT32 count = 0;

#ifdef AUD_Enable_TimeoutProtect
	//UINT64 timer_rtc = GL_GetRtc64();
	struct timeval timer_rtc, timer_stamp;
	do_gettimeofday(&timer_rtc);
	UINT16 timeout_flag = 0;
#endif
	
	while (state != DSP_GetState(type)) 		
	{
		DSP_Waitloop(10);	
		count++;
	#ifdef AUD_Enable_TimeoutProtect
		do_gettimeofday(&timer_stamp);
	    if( (timer_stamp.tv_sec - timer_rtc.tv_sec) >= 1 ) // 1s
		{
			diag_printf("\n== DSP_WaitState TimeOut==\n");
			timeout_flag = 1;
			break;
		}
	#endif		
	}

#ifdef AUD_Enable_TimeoutProtect
	if( timeout_flag==1 )
	{
		DSPCMD_MUTEX_UNLOCK();
		DSP_Set_Timeout_Flag(1);
		return DSP_ERROR_WAIT_STATE;
	}
#endif	

#ifdef ENABLE_PRINT_DSP_STATE_STRING
	DSPAPI_PRINT("DSP_WaitState[ok]::%s:state:%d, wait state:%d, count:%d\n",GET_STATE_STR(type),DSP_GetState(type),state,count);
#endif
	
	DSPCMD_MUTEX_UNLOCK();
	
	return DSP_ERROR_NONE;
}

void DSP_Set_Timeout_Flag( UINT16 value)
{	
	// if 1, we will force to reset dsp in audio MainRoutine
	gDSP_waitstate_timeout = value;
}

UINT16 DSP_Get_Timeout_Flag(void)
{	
	return gDSP_waitstate_timeout;
}

/******************************************************************************
    dsp bitstream buffer manipulation
 *****************************************************************************/
UINT8* DSP_GetBsBufStart(UINT8 bsBufID)
{
	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(bsBufID);

	if (NULL == pBsBufInfo) return 0;	
	
	return pBsBufInfo->pBsBufBase;

}

UINT32 DSP_GetBsBufSize(UINT8 bsBufID)
{
	UINT32 bsBufsize = 0;
	
	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(bsBufID);
	
	bsBufsize = pBsBufInfo->bsBufLen;

	if (NULL == pBsBufInfo) return 0;	
	
	return bsBufsize;

}

UINT32 DSP_GetBsBufUsed(UINT8 bsBufID)
{
	UINT32 bsBufUsed = 0;

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(bsBufID);

	if (NULL == pBsBufInfo) return 0;
	//DSP_PRINT("DSP_GetBsBufUsed::ID:%d,base:0x%x, len:0x%x, dump:0x%x, barr:0x%x\n",pBsBufInfo->bsBufID,pBsBufInfo->pBsBufBase,pBsBufInfo->bsBufLen,pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);			
	bsBufUsed = BS_GET_USED(pBsBufInfo->bsBufLen, pBsBufInfo->dumpPtr, pBsBufInfo->barrPtr);
	//DSP_PRINT("DSP_GetBsBufUsed::bsBufUsed:0x%x(%d)\n",bsBufUsed,bsBufUsed);
	return bsBufUsed;

}


UINT32 DSP_GetBsBufFree(UINT8 bsBufID)
{
	UINT32 bsBufFree = 0;

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(bsBufID);

	if (NULL == pBsBufInfo) return 0;
	
	//DSP_PRINT("DSP_GetBsBufFree::ID:%d,base:0x%x, len:0x%x, dump:0x%x, barr:0x%x\n",pBsBufInfo->bsBufID,pBsBufInfo->pBsBufBase,pBsBufInfo->bsBufLen,pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);			
	bsBufFree = BS_GET_FREE(pBsBufInfo->bsBufLen, pBsBufInfo->dumpPtr, pBsBufInfo->barrPtr);	

	return bsBufFree;

}


UINT32 DSP_BsBufFeed(UINT8 bsBufID,UINT8* pBsSrcBuf, UINT32 aFeedSize)
{
	
	if ((aFeedSize <= 0) || (NULL == pBsSrcBuf))
	{
		ERROR_PRINT("DSP_BsBufFeed::e:bitstream buffer source is null\n");
		return DSP_ERROR_BSBUF_FEED;
	}

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(bsBufID);

	if (NULL == pBsBufInfo) 
	{	
		ERROR_PRINT("DSP_BsBufFeed::e:pBsBufInfo is null\n");	
		return DSP_ERROR_BSBUF_FEED;
	}


	if ((pBsBufInfo->bsBufLen <= 0) || (NULL == pBsBufInfo->pBsBufBase))
	{	
		ERROR_PRINT("DSP_BsBufFeed::e:dsp bitstream buffer not enabled\n");		

		return DSP_ERROR_BSBUF_ID;		
	}


	UINT32 bsBufFree = BS_GET_FREE(pBsBufInfo->bsBufLen,pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);
	
	//DSP_PRINT("DSP_BsBufFeed::bsBufFree:0x%x(%d)!\n",bsBufFree,bsBufFree);

	// Update bitstram buffer dump value
	if (bsBufFree > aFeedSize)			// note: dump should not exceed barrier
	{
		// check if dump exceed BsbufLen
		if ((pBsBufInfo->dumpPtr + aFeedSize) < pBsBufInfo->bsBufLen)
		{					
			DSP_Memcpy(pBsBufInfo->pBsBufBase+pBsBufInfo->dumpPtr, pBsSrcBuf, aFeedSize);
			pBsBufInfo->dumpPtr += aFeedSize;	
		}
		else
		{
			// dump wrap around case
			DSP_Memcpy(pBsBufInfo->pBsBufBase+pBsBufInfo->dumpPtr, pBsSrcBuf, (pBsBufInfo->bsBufLen-pBsBufInfo->dumpPtr));
			UINT32 RdCount = pBsBufInfo->bsBufLen-pBsBufInfo->dumpPtr;					
			DSP_Memcpy(pBsBufInfo->pBsBufBase, pBsSrcBuf+RdCount, (aFeedSize-(pBsBufInfo->bsBufLen-pBsBufInfo->dumpPtr)));			
			pBsBufInfo->dumpPtr = pBsBufInfo->dumpPtr+aFeedSize-pBsBufInfo->bsBufLen;			
		}

		// Update dump to dsp
		// hcsu:maybe set dump later		
		DSP_SetDumpPtr(bsBufID, pBsBufInfo->dumpPtr);
		
		bsBufFree = BS_GET_FREE(pBsBufInfo->bsBufLen, pBsBufInfo->dumpPtr, pBsBufInfo->barrPtr);	
		
//		DSPAPI_PRINT("DSP_BsBufFeed::%d:BsBuf:base:0x%08x,len:0x%08x,dump:0x%08x,barr:0x%08x,free:0x%x\n",bsBufID,(UINT32)pBsBufInfo->pBsBufBase,pBsBufInfo->bsBufLen,pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr,bsBufFree);
	}
	else
	{
		ERROR_PRINT("DSP_BsBufFeed::e:bsBufFree:0x%x(%d),wait dsp consume data!\n",bsBufFree,bsBufFree);
		return DSP_ERROR_BS_WAIT;
	}				

	return DSP_ERROR_NONE;
}

UINT32 DSP_BsBufFeed_Cached(UINT8 bsBufID,UINT8* pBsSrcBuf, UINT32 aFeedSize)
{
	
	if ((aFeedSize <= 0) || (NULL == pBsSrcBuf))
	{
		ERROR_PRINT("DSP_BsBufFeed::e:bitstream buffer source is null\n");
		return DSP_ERROR_BSBUF_FEED;
	}

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(bsBufID);

	if (NULL == pBsBufInfo) 
	{	
		ERROR_PRINT("DSP_BsBufFeed::e:pBsBufInfo is null\n");	
		return DSP_ERROR_BSBUF_FEED;
	}


	if ((pBsBufInfo->bsBufLen <= 0) || (NULL == pBsBufInfo->pBsBufBase))
	{	
		ERROR_PRINT("DSP_BsBufFeed::e:dsp bitstream buffer not enabled\n");		

		return DSP_ERROR_BSBUF_ID;		
	}


	UINT32 bsBufFree = BS_GET_FREE(pBsBufInfo->bsBufLen,pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);
	
	//DSP_PRINT("DSP_BsBufFeed::bsBufFree:0x%x(%d)!\n",bsBufFree,bsBufFree);

	// Update bitstram buffer dump value
	if (bsBufFree > aFeedSize)			// note: dump should not exceed barrier
	{
		// check if dump exceed BsbufLen
		if ((pBsBufInfo->dumpPtr + aFeedSize) < pBsBufInfo->bsBufLen)
		{					
			DSP_Memcpy_Cached(pBsBufInfo->pBsBufBase+pBsBufInfo->dumpPtr, pBsSrcBuf, aFeedSize);
			pBsBufInfo->dumpPtr += aFeedSize;	
		}
		else
		{
			// dump wrap around case
			DSP_Memcpy_Cached(pBsBufInfo->pBsBufBase+pBsBufInfo->dumpPtr, pBsSrcBuf, (pBsBufInfo->bsBufLen-pBsBufInfo->dumpPtr));
			UINT32 RdCount = pBsBufInfo->bsBufLen-pBsBufInfo->dumpPtr;					
			DSP_Memcpy_Cached(pBsBufInfo->pBsBufBase, pBsSrcBuf+RdCount, (aFeedSize-(pBsBufInfo->bsBufLen-pBsBufInfo->dumpPtr)));			
			pBsBufInfo->dumpPtr = pBsBufInfo->dumpPtr+aFeedSize-pBsBufInfo->bsBufLen;			
		}

		// Update dump to dsp
		// hcsu:maybe set dump later		
		DSP_SetDumpPtr(bsBufID, pBsBufInfo->dumpPtr);
		
		bsBufFree = BS_GET_FREE(pBsBufInfo->bsBufLen, pBsBufInfo->dumpPtr, pBsBufInfo->barrPtr);	
		
//		DSPAPI_PRINT("DSP_BsBufFeed::%d:BsBuf:base:0x%08x,len:0x%08x,dump:0x%08x,barr:0x%08x,free:0x%x\n",bsBufID,(UINT32)pBsBufInfo->pBsBufBase,pBsBufInfo->bsBufLen,pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr,bsBufFree);
	}
	else
	{
		ERROR_PRINT("DSP_BsBufFeed::e:bsBufFree:0x%x(%d),wait dsp consume data!\n",bsBufFree,bsBufFree);
		return DSP_ERROR_BS_WAIT;
	}				

	return DSP_ERROR_NONE;
}

UINT32 DSP_SetDumpPtr(UINT8 bsBufID, UINT32 aDump)
{
	switch (bsBufID)
	{
	case BSBUF_ID_DEC1:
		HWREG_W(dsp_port[2], aDump);
		break;
	case BSBUF_ID_DEC2:
		HWREG_W(dsp_port[4], aDump);
		break;
	case BSBUF_ID_ENC:
		HWREG_W(dsp_port[8], aDump);
		break;				
	default:				
		ERROR_PRINT("DSP_SetDumpPtr::e:wrong Bank ID:%d\n",bsBufID);		
		return DSP_ERROR_BSBUF_ID;		
	}
	return DSP_ERROR_NONE;
}

UINT32 DSP_SetBarrierPtr(UINT8 bsBufID, UINT32 aBarrier)
{
	switch (bsBufID)
	{
	case BSBUF_ID_DEC1:
		HWREG_W(dsp_port[3], aBarrier);
		break;
	case BSBUF_ID_DEC2:
		HWREG_W(dsp_port[5], aBarrier);
		break;
	case BSBUF_ID_ENC:
		HWREG_W(dsp_port[9], aBarrier);
		break;				
	default:				
		ERROR_PRINT("DSP_SetBarrierPtr::e:wrong Bank ID:%d\n",bsBufID);		
		return DSP_ERROR_BSBUF_ID;		
	}
	return DSP_ERROR_NONE;
}

BsBufInfo* DSP_UpdateBsBufInfo(UINT8 bsBufID)
{
	BsBufInfo* pBsBufInfo = NULL;
	switch (bsBufID)
	{
	case BSBUF_ID_DEC1:
		pBsBufInfo = (BsBufInfo*)DSP_GetInfo(INFO_BSINFO_DEC1);
		break;
	case BSBUF_ID_DEC2:
		pBsBufInfo = (BsBufInfo*)DSP_GetInfo(INFO_BSINFO_DEC2);
		break;
	case BSBUF_ID_ENC:
		pBsBufInfo = (BsBufInfo*)DSP_GetInfo(INFO_BSINFO_ENC);		
		break;		

	default:				
		ERROR_PRINT("DSP_UpdateBsBufInfo::e:wrong Bank ID:%d\n",bsBufID);		
		return NULL;		
	}

//	DSP_PRINT("DSP_UpdateBsBufInfo::dump:0x%x, barr:0x%x\n",pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);
	
	return pBsBufInfo;		
}


// 
// Should add flow control related command, SetStop/SetPlay/SetPause/SetReady...
// 
UINT32 DSP_SetSystemIdle(void)
{

	// Check valid state
	UINT8 nowState = DSP_GetState(DSP_STATE_SYSTEM);
	DSPAPI_PRINT("DSP_SetSystemIdle::nowState:%d\n",nowState);		
	
	
	// Check if already in MAIN_STATE_IDLE state
	if (MAIN_STATE_IDLE == nowState)
	{
		DSPAPI_PRINT("DSP_SetSystemIdle::already in MAIN_STATE_IDLE state\n");		
		return DSP_ERROR_NONE;
	}


	// Idle state allow: ready/run
	if (MAIN_STATE_READY!=nowState && MAIN_STATE_RUN!=nowState )
	{
		ERROR_PRINT("DSP_SetSystemIdle::e:nowState not allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;
	}

	// issue MAIN_STATE_IDLE command
	DSPAPI_PRINT("DSP_SetSystemIdle::send MAIN_STATE_IDLE command...\n");		
	DSP_SendCmd(RISC_CMD0x00_FLOW,CODE0x00_MAIN_FLOW,MAIN_STATE_IDLE,0);	
	//DSPAPI_PRINT("DSP_SetSystemIdle::cmd:0x%08x, param:0x%08x\n",HWREG_R(dsp_port[0]),HWREG_R(dsp_port[1]));				

	// wait system run to MAIN_STATE_IDLE state (always wait)
	DSP_WaitState(DSP_STATE_SYSTEM, MAIN_STATE_IDLE);
	DSPAPI_PRINT("DSP_SetSystemIdle::wait MAIN_STATE_IDLE state done!\n");		

	return DSP_ERROR_NONE;	

	
}
UINT32 DSP_SetSystemReady(void)
{

	// Check valid state
	UINT8 nowState = DSP_GetState(DSP_STATE_SYSTEM);
	DSP_PRINT("DSP_SetSystemReady::nowState:%d\n",nowState);			
	
	// Check if already in MAIN_STATE_IDLE state
	if (MAIN_STATE_READY == nowState)
	{
		DSPAPI_PRINT("DSP_SetSystemReady::already in MAIN_STATE_READY state\n");		
		return DSP_ERROR_NONE;
	}


	// ready state allow: init/idle
	if (MAIN_STATE_INIT!=nowState && MAIN_STATE_IDLE!=nowState )
	{
		ERROR_PRINT("DSP_SetSystemReady::e:nowState not allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;
	}

	DSPAPI_PRINT("DSP_SetSystemReady::send MAIN_STATE_READY command...\n");	
	// issue MAIN_STATE_READY command
	DSP_SendCmd(RISC_CMD0x00_FLOW,CODE0x00_MAIN_FLOW,MAIN_STATE_READY,0);	
	//DSPAPI_PRINT("DSP_SetSystemReady::cmd:0x%08x, param:0x%08x\n",HWREG_R(dsp_port[0]),HWREG_R(dsp_port[1]));					

	// wait system run to MAIN_STATE_IDLE state (always wait)

	DSP_WaitState(DSP_STATE_SYSTEM, MAIN_STATE_READY);
	DSPAPI_PRINT("DSP_SetSystemReady::wait MAIN_STATE_READY state done!\n");		

	return DSP_ERROR_NONE;	
	
}

UINT32 DSP_SetSystemRun(void)
{

	// Check valid state
	UINT8 nowState = DSP_GetState(DSP_STATE_SYSTEM);
	DSP_PRINT("DSP_SetSystemRun::nowState:%d\n",nowState);				
	
	// Check if already in MAIN_STATE_IDLE state
	if (MAIN_STATE_RUN == nowState)
	{
		DSPAPI_PRINT("DSP_SetSystemRun::already in MAIN_STATE_RUN state\n");		
		return DSP_ERROR_NONE;
	}

	// run state allow: idle
	if (MAIN_STATE_IDLE!=nowState )
	{
		ERROR_PRINT("DSP_SetSystemRun::e:nowState not allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;
	}

	// issue MAIN_STATE_RUN command
	DSPAPI_PRINT("DSP_SetSystemRun::send MAIN_STATE_RUN command...\n");	
	DSP_SendCmd(RISC_CMD0x00_FLOW,CODE0x00_MAIN_FLOW,MAIN_STATE_RUN,0);	
	//DSPAPI_PRINT("DSP_SetSystemRun::cmd:0x%08x, param:0x%08x\n",HWREG_R(dsp_port[0]),HWREG_R(dsp_port[1]));					

	// wait system run to MAIN_STATE_RUN state (always wait)	
	DSP_WaitState(DSP_STATE_SYSTEM, MAIN_STATE_RUN);
	DSPAPI_PRINT("DSP_SetSystemRun::wait MAIN_STATE_RUN state done!\n");		

	return DSP_ERROR_NONE;	

}

UINT32 DSP_SetCodecStop(UINT8 codecBankID)
{
	UINT8 cmdCode;
	UINT8 stateType;
	UINT8 nowState;
	
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		cmdCode = CODE0x01_DEC1_FLOW;
		stateType = DSP_STATE_DEC1;
		break;
	case CODEC_BANK_DEC2:
		cmdCode = CODE0x02_DEC2_FLOW;	
		stateType = DSP_STATE_DEC2;
		break;
	case CODEC_BANK_ENC:
		cmdCode = CODE0x03_ENC_FLOW;
		stateType = DSP_STATE_ENC;
		break;		
	default:				
		ERROR_PRINT("DSP_SetCodecStop::e:wrong codecBankID:%d\n",codecBankID);		
		return DSP_ERROR_CODEC_BANKID;		
	}	

	// Check valid dsp state first
	nowState = 	DSP_GetState(codecBankID);
	DSP_PRINT("DSP_SetCodecStop::nowState:%d\n",nowState);	

	// Check if already in TASK_STOP state
	if (TASK_STOP == nowState)
	{
		DSPAPI_PRINT("DSP_SetCodecStop::already in TASK_STOP state\n");		
		return DSP_ERROR_NONE;		
	}

	// STOP state allow: all state except TASK_INIT 	
	if (TASK_INIT == nowState)
	{
		ERROR_PRINT("DSP_SetCodecStop::e:nowState = TASK_INIT, should issue TASK_READY first \n");		
		return DSP_ERROR_STATE;
	}

	// Set rampdown? may add rampdown command
	//	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x01_RAMP_DOWN,0,0);

	// issue command
	DSPAPI_PRINT("DSP_SetCodecStop::send TASK_STOP command...\n");	
	DSP_SendCmd(RISC_CMD0x00_FLOW,cmdCode,TASK_STOP,0);		

	//DSPAPI_PRINT("DSP_SetCodecStop::cmd:0x%08x, param:0x%08x\n",HWREG_R(dsp_port[0]),HWREG_R(dsp_port[1]));	
	DSP_WaitState(stateType, TASK_STOP);	
	DSPAPI_PRINT("DSP_SetCodecStop::wait TASK_STOP state done!\n");	

	return DSP_ERROR_NONE;		
}


UINT32 DSP_SetCodecPlay(UINT8 codecBankID)
{
	UINT8 cmdCode;
	UINT8 stateType;
	UINT8 nowState;
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		cmdCode = CODE0x01_DEC1_FLOW;
		stateType = DSP_STATE_DEC1;
		break;
	case CODEC_BANK_DEC2:
		cmdCode = CODE0x02_DEC2_FLOW;
		stateType = DSP_STATE_DEC2;
		break;
	case CODEC_BANK_ENC:
		cmdCode = CODE0x03_ENC_FLOW;
		stateType = DSP_STATE_ENC;
		break;		
	default:				
		ERROR_PRINT("DSP_SetCodecPlay::e:wrong codecBankID:%d\n",codecBankID);		
		return DSP_ERROR_CODEC_BANKID;		
	}	
	
	// Check valid dsp state first
	nowState = 	DSP_GetState(codecBankID);
	DSPAPI_PRINT("DSP_SetCodecPlay::nowState:%d\n",nowState);	
	
	// Check if already in TASK_PLAY state
	if (TASK_PLAY == nowState)
	{
		DSPAPI_PRINT("DSP_SetCodecPlay::already in TASK_PLAY state\n");		
		return DSP_ERROR_NONE;		
	}	
	// PLAY state: allowed from:STOP/PAUSE/FF
	if (TASK_INIT == nowState || TASK_READY == nowState)
	{
		ERROR_PRINT("DSP_SetCodecPlay::e:nowState now allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;		
		
	}
		
	// Set rampup? may add rampup command
//	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x00_RAMP_UP,0,0);

	DSPAPI_PRINT("DSP_SetCodecPlay::send TASK_PLAY command...\n");	
	DSP_SendCmd(RISC_CMD0x00_FLOW,cmdCode,TASK_PLAY,0);

	DSP_WaitState(stateType, TASK_PLAY);	
	DSPAPI_PRINT("DSP_SetCodecPlay::wait TASK_PLAY state done!\n");	

	return DSP_ERROR_NONE;
	
}

UINT32 DSP_SetCodecPause(UINT8 codecBankID)
{
	UINT8 cmdCode;
	UINT8 stateType;
	UINT8 nowState;
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		cmdCode = CODE0x01_DEC1_FLOW;
		stateType = DSP_STATE_DEC1;
		break;
	case CODEC_BANK_DEC2:
		cmdCode = CODE0x02_DEC2_FLOW;
		stateType = DSP_STATE_DEC2;
		break;
	case CODEC_BANK_ENC:
		cmdCode = CODE0x03_ENC_FLOW;
		stateType = DSP_STATE_ENC;
		break;		
	default:				
		ERROR_PRINT("DSP_SetCodecPause::e:wrong codecBankID:%d\n",codecBankID);		
		return DSP_ERROR_CODEC_BANKID;		
	}	
	
	// Check valid dsp state first
	nowState = 	DSP_GetState(codecBankID);
	DSPAPI_PRINT("DSP_SetCodecPause::nowState:%d\n",nowState);	
	
	// Check if already in TASK_PAUSE state
	if (TASK_PAUSE == nowState)
	{
		DSPAPI_PRINT("DSP_SetCodecPause::already in TASK_PAUSE state\n");		
		return DSP_ERROR_NONE;		
	}	
	// PAUSE state: allowed from:STOP/PLAY/FF
	if (TASK_INIT == nowState || TASK_READY == nowState)
	{
		ERROR_PRINT("DSP_SetCodecPause::e:nowState now allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;		
		
	}
		
	// Set rampdown? may add rampdown command

	// issue command
	DSPAPI_PRINT("DSP_SetCodecPause::send TASK_PAUSE command...\n");		
	DSP_SendCmd(RISC_CMD0x00_FLOW,cmdCode,TASK_PAUSE,0);	
	
	DSP_WaitState(stateType, TASK_PAUSE);	
	DSPAPI_PRINT("DSP_SetCodecPause::wait TASK_PAUSE state done!\n");		

	return DSP_ERROR_NONE;
}

UINT32 DSP_SetCodecFastForward(UINT8 codecBankID,UINT16 ffratio)
{
	UINT8 cmdCode;
	UINT8 stateType;
	UINT8 nowState;
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		cmdCode = CODE0x01_DEC1_FLOW;
		stateType = DSP_STATE_DEC1;
		break;
	case CODEC_BANK_DEC2:
		cmdCode = CODE0x02_DEC2_FLOW;
		stateType = DSP_STATE_DEC2;
		break;
	case CODEC_BANK_ENC:
		cmdCode = CODE0x03_ENC_FLOW;
		stateType = DSP_STATE_ENC;
		break;		
	default:				
		ERROR_PRINT("DSP_SetCodecFastForward::e:wrong codecBankID:%d\n",codecBankID);		
		return DSP_ERROR_CODEC_BANKID;		
	}	
	
	// Check valid dsp state first
	nowState = 	DSP_GetState(codecBankID);
	DSPAPI_PRINT("DSP_SetCodecFastForward::nowState:%d\n",nowState);	

	
	// Check if already in TASK_FF state
	if (TASK_FF == nowState)
	{
		DSPAPI_PRINT("DSP_SetCodecFastForward::already in TASK_FF state\n");		
		return DSP_ERROR_NONE;		
	}	
	// PLAY state: allowed from:STOP/PAUSE/PLAY
	if (TASK_INIT == nowState || TASK_READY == nowState)
	{
		ERROR_PRINT("DSP_SetCodecFastForward::e:nowState now allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;		
		
	}
		
	// Set rampup? may add rampup command

	// issue command
	DSPAPI_PRINT("DSP_SetCodecFastForward::send TASK_FF command...\n");		
	DSP_SendCmd(RISC_CMD0x00_FLOW,cmdCode,TASK_FF,ffratio);	

	DSP_WaitState(stateType, TASK_FF);	
	DSP_PRINT("DSP_SetCodecFastForward::wait TASK_FF state done!\n");		

	return DSP_ERROR_NONE;
}

UINT32 DSP_SetCodecReady(UINT8 codecBankID)
{
	UINT8 cmdCode;
	UINT8 stateType;
	UINT8 nowState;
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		cmdCode = CODE0x01_DEC1_FLOW;
		stateType = DSP_STATE_DEC1;
		break;
	case CODEC_BANK_DEC2:
		cmdCode = CODE0x02_DEC2_FLOW;
		stateType = DSP_STATE_DEC2;
		break;
	case CODEC_BANK_ENC:
		cmdCode = CODE0x03_ENC_FLOW;
		stateType = DSP_STATE_ENC;
		break;		
	default:				
		ERROR_PRINT("DSP_SetCodecReady::e:wrong codecBankID:%d\n",codecBankID);		
		return DSP_ERROR_CODEC_BANKID;		
	}	

	// Check valid dsp state first
	nowState = 	DSP_GetState(codecBankID);
	DSPAPI_PRINT("DSP_SetCodecReady::nowState:%d\n",nowState);	

	
	// Check if already in TASK_READY state
	if (TASK_READY == nowState)
	{
		DSPAPI_PRINT("DSP_SetCodecReady::already in TASK_READY state\n");		
		return DSP_ERROR_NONE;		
	}	
	// READY state: allowed from:INIT,STOP
	if (TASK_INIT != nowState && TASK_STOP != nowState)
	{
		ERROR_PRINT("DSP_SetCodecReady::e:nowState now allowed:%d\n",nowState);		
		return DSP_ERROR_STATE;		
		
	}

	// issue command
	DSPAPI_PRINT("DSP_SetCodecReady::send TASK_READY command...\n");	
	DSP_SendCmd(RISC_CMD0x00_FLOW,cmdCode,TASK_READY,0);	

	DSP_WaitState(stateType, TASK_READY);	
	DSPAPI_PRINT("DSP_SetCodecReady::wait TASK_READY state done!\n");		

	return DSP_ERROR_NONE;
}



UINT32 DSP_FlushBsBuf(BSID bsBufID)
{
	DSPAPI_PRINT("DSP_FlushBsBuf::bsBufID:%d\n",bsBufID);	
	// Reset Bitstream Buffer			
	DSP_SendCmd(RISC_CMD0x02_BSBUF,0,bsBufID,0);	
	return DSP_ERROR_NONE;
	
}

#define DSP_DEC1_ENDING_CTRL_DONE	(0x1 << 0)
#define DSP_DEC2_ENDING_CTRL_DONE	(0x1 << 1)

UINT16 DSP_WaitEndingDone(UINT8 codecBankID)
{
	UINT16 EndCtrlDoneFlag;
	UINT16 Result = TRUE;

	EndCtrlDoneFlag = DSP_GetDspParam0(DSP_MAIN_ENDING_CTRL_DONE);	
	
	if (CODEC_BANK_DEC1 == codecBankID)
	{
		Result = DSP_DEC1_ENDING_CTRL_DONE == (DSP_DEC1_ENDING_CTRL_DONE & EndCtrlDoneFlag);		
	}
	
	if (CODEC_BANK_DEC2 == codecBankID)		
	{
		Result = DSP_DEC1_ENDING_CTRL_DONE == (DSP_DEC1_ENDING_CTRL_DONE & EndCtrlDoneFlag);
	}

	return Result;
	
}

UINT32 DSP_SetBsBufEnd(UINT8 codecBankID)
{
	DSP_PRINT("DSP_SetBsBufEnd::codecBankID:%d\n",codecBankID);
	if ((CODEC_BANK_DEC1 == codecBankID) | (CODEC_BANK_DEC2 == codecBankID))
	{
		DSP_SendCmd(RISC_CMD0x02_BSBUF, CODE0x01_BSBUF_ENDING, codecBankID, 0x0);
	}
	
	return DSP_ERROR_NONE;
}

UINT32 DSP_GetSampleRate( void )
{	
    UINT32 val =  HWREG_R(dsp_port[20]);
    val = val&0x00FFFFFF;
	return val;
}

void DSP_SetSampleRate( UINT32 val )
{	
    UINT32 val_reg = HWREG_R(dsp_port[20])&0xFF000000; //clean bit[23:0]
    val_reg |= (val&0x00FFFFFF);
	HWREG_W(dsp_port[20], val_reg );
	return; 
}

#ifdef ENABLE_DSP_INTERRUPT
#if 0
static UINT32 gDspMsgCmdPort;
static UINT32 gDspMsgParamPort;
cyg_uint32 dsp_int0_isr(cyg_vector_t vector, cyg_addrword_t data __UNUSED__)
{
	// ISR jobs
	gDspMsgCmdPort = HWREG_R(dsp_port[10]);
	gDspMsgParamPort = HWREG_R(dsp_port[11]);	
	if (0 == (0x80000000 & HWREG_R(dsp_port[10])))
	{
		//
		// Set Message Flags: (DSP_SetMessageFlag())
		//
		HWREG_W(dsp_port[10], HWREG_R(dsp_port[10]) | 0x80000000);
	}


	//
	// ISR return
	//
	HWREG_W(rdif_int_ctrl, 0x4000); // INT0 clear
	cyg_interrupt_acknowledge(vector);
	return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
//	return (CYG_ISR_HANDLED); //if no DSR
}

void dsp_int0_dsr(cyg_vector_t vector __UNUSED__, cyg_ucount32 count __UNUSED__, cyg_addrword_t data __UNUSED__)
{
//	diag_printf("dsp_int0_dsr::receviced DSP INT0!\n");	

#ifdef DEBUG_PRINT_DSP_MESSAGE
//	DspShowMessage(gDspMsgCmdPort,gDspMsgParamPort);
	diag_printf("dsp_int0_dsr::port10:0x%08x, port11:0x%08x\n",HWREG_R(dsp_port[10]),HWREG_R(dsp_port[11]));	
#endif	
}
#endif

cyg_uint32 dsp_int0_isr(cyg_vector_t vector, cyg_addrword_t data __UNUSED__)
{

	if (DSP_GetMessageFlag())
	{
		DSP_GetNewMessage();
		DSP_SetMessageFlag();
	}
	
	//
	// ISR return
	//
	HWREG_W(rdif_int_ctrl, 0x4000); // INT0 clear
	cyg_interrupt_acknowledge(vector);	


	//return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
	return (CYG_ISR_HANDLED); //if no DSR
	
}

void dsp_int0_dsr(cyg_vector_t vector __UNUSED__, cyg_ucount32 count __UNUSED__, cyg_addrword_t data __UNUSED__)
{
//	diag_printf("dsp_int0_dsr::receviced DSP INT0!\n");	
/*
	if (DSP_GetMessageFlag())
	{
		DSP_GetNewMessage();
		DSP_SetMessageFlag();
	}
*/	
}
/*
cyg_uint32 dsp_int1_isr(cyg_vector_t vector, cyg_addrword_t data __UNUSED__)
{
	HWREG_W(rdif_int_ctrl, 0x8000); // INT1 clear
	cyg_interrupt_acknowledge(vector);
	return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
//	return (CYG_ISR_HANDLED); //if no DSR
}

void dsp_int1_dsr(cyg_vector_t vector __UNUSED__, cyg_ucount32 count __UNUSED__, cyg_addrword_t data __UNUSED__)
{
	diag_printf("dsp_int1_dsr::receviced DSP INT1!\n");	
}
*/
#endif

/*
***************************************************************
*
* dylan: the following codes are for SPV9200 only (20101011)
*
***************************************************************
*/
DSP_DRV_Status_t DSP_DRV_AudInit(DSP_DRV_AudInitialConfig_t *pInitialParam)
{
	if(pInitialParam == NULL)
	{
		return DSP_DRV_ERR_INVALID_PARAM;
	}

#if 0  // depend on 9200 actual design
	//power down audio PLLA
	regs0->sft_cfg47 &= ~(0x01<<12);
	hal_delay_us(1000);
	regs0->sft_cfg47 |= (0x01<<12);

	//power up audio PLLA
	regs0->sft_cfg47 &= ~(0x01<<12);
	hal_delay_us(1000);
	regs0->sft_cfg47 |= (0x01<<12);
	hal_delay_us(100000);

	//enable I2S output
    //regs0->sft_cfg6|=0x008000;

	//enable SIF ADC, SSD need clock from SIF ADC
    regs0->sft_cfg52 &= ~(0x1f0000);

	//enable DAC Vcom and VRT voltage
    regs0->sft_cfg50|=0x0c;

	regs0->sft_cfg5 |=(0x1<<6);
	hal_delay_us(1000);
	regs0->sft_cfg5 &=~(0x1<<6);

	//Rex_080216 add: ADAC Frequency Select
	regs0->sft_cfg50 &= ~(0x0700);
	regs0->sft_cfg50|=0x100;

	//disable audio DAC auto sleep function
	regs0->sft_cfg50 |= 0x20;//disable DAC auto sleep function

	//disable DAC R/L swap
	HW_SetReg(REG_Ctrl_b0ce,HW_GetReg(REG_Ctrl_b0ce)&(~0x80));
#endif	

    /*Initial SSD*/
	//HW_DTL_Init();  // dylan: no need to set this flag

#if 0 // dylan will arrange DRAM resource
    /* reset DTL interface & EPICS program counter */
    HW_SSDSetReg(REG_Ssd_reset_hw, REG_SSD_RESET_HW_VALUE_RESET);
    HW_SSDSetReg(REG_Ssd_pc_reset, REG_SSD_PC_RESET_VALUE_RESET);
	HW_SSDSetReg(REG_Ssd_reset_hw, REG_SSD_RESET_HW_VALUE_NO_RESET);
	HW_SSDSetReg(REG_Ssd_pc_reset, REG_SSD_PC_RESET_VALUE_NO_RESET);

	/*Initial memory config for lip sync begin */
    HW_SetReg(REG_Ctrl_dlu_ini, REG_CTRL_DLU_INI_VALUE_NOT_FINISHED);

    /*Disable DRAM request*/
    HW_SetReg(REG_Ctrl_reqen_audio, REG_CTRL_REQEN_AUDIO_VALUE_DISABLE);

    /*Set DRAM start address*/
    HW_SetReg(REG_Ctrl_dlu_sta_adr_3, ((pInitialParam->dLipSyncMemoryStarAddress)&0x7FFFFFF)>>24);
	HW_SetReg(REG_Ctrl_dlu_sta_adr_2, ((pInitialParam->dLipSyncMemoryStarAddress)&0x7FFFFFF)>>16);
	HW_SetReg(REG_Ctrl_dlu_sta_adr_1, ((pInitialParam->dLipSyncMemoryStarAddress)&0x7FFFFFF)>>8);
    HW_SetReg(REG_Ctrl_dlu_sta_adr_0, ((pInitialParam->dLipSyncMemoryStarAddress)&0x7FFFFFF));
    /*Set DRAM end address*/
	HW_SetReg(REG_Ctrl_dlu_end_adr_3, (((pInitialParam->dLipSyncMemoryStarAddress)+((pInitialParam->dLipSyncMemorySize)*1024))&0x7FFFFFF)>>24);
	HW_SetReg(REG_Ctrl_dlu_end_adr_2, (((pInitialParam->dLipSyncMemoryStarAddress)+((pInitialParam->dLipSyncMemorySize)*1024))&0x7FFFFFF)>>16);
	HW_SetReg(REG_Ctrl_dlu_end_adr_1, (((pInitialParam->dLipSyncMemoryStarAddress)+((pInitialParam->dLipSyncMemorySize)*1024))&0x7FFFFFF)>>8);
    HW_SetReg(REG_Ctrl_dlu_end_adr_0, ((pInitialParam->dLipSyncMemoryStarAddress)+((pInitialParam->dLipSyncMemorySize)*1024))&0x7FFFFFF);

	HW_SetReg(REG_Ctrl_dlu_ini, REG_CTRL_DLU_INI_VALUE_INITIAL_DONE);

    /*Enable DRAM request*/
    HW_SetReg(REG_Ctrl_reqen_audio, REG_CTRL_REQEN_AUDIO_VALUE_ENABLE);

	HW_SSDSetReg(REG_Ssd_lipsync_bypass, REG_SSD_LipSync_Bypass_VALUE_USE_EXTERNAL_LIPSYNC);
	/*Initial memory config for lip sync end*/
#endif

#ifdef SPV9200_DEMDEC    
	/*Initial REG SSD OVMADAPT VALUE for channel noise*/
  	DSP_SetSSDParam(REG_Ssd_ovmadapt, REG_SSD_OVMADAPT_VALUE_ENABLED);
#endif

   return DSP_DRV_SUCCESS;    
}

void DSP_PC_Status()
{
	// dsp pc
	DISABLE_JTAG();
	printk(KERN_ALERT "dsp_monCmd::DSP PC:0x%x\n",GET_DSP_PC());
	ENABLE_JTAG();
}


