/******************************************************************************
 * Filename    : dsp_msg.c
 * 
 * Description : audio dsp message handling implementation file
 * Language    : C Language 
 *           
 * Revision History: 
 * ------------------
 * [090804]hcsu Initial Draft
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
#include "include/setting.h"
//#include "stypes.h"

#include "include/dsp_api.h"
#include "include/aud_global.h"
#include "include/auddrv.h"
///#include "include/playlist.h"
///#include "regmap_8600.h"
#include <asm/mach-spv9200/regmap_9200.h>

/******************************************************************************
	typedef
 *****************************************************************************/ 


/******************************************************************************
	macros Defines
 *****************************************************************************/ 
#define MAX(x,y) 						(x >= y) ? (x):(y)
#define MIN(x,y)                		(x >= y) ? (y):(x)


/******************************************************************************
	Local Defines
 *****************************************************************************/ 
#define MAX_DSPMON_NUM 					10

#ifdef DEBUG_PRINT_DRAM_MESSAGE
	#define DSP_SHOW_DRAM_MESSAGE		DspShowDramMessage
#else
	#define DSP_SHOW_DRAM_MESSAGE		//
#endif

#ifdef DEBUG_PRINT_DSP_MESSAGE
	#define DSP_SHOW_MESSAGE			DspShowMessage
#else
	#define DSP_SHOW_MESSAGE			//
#endif
/******************************************************************************
    global&static variables
 *****************************************************************************/
static ProfileHistoryInfo_t PrfHistoryInfo[MAX_DSPMON_NUM];	

UINT16 gDspMsgIDFilter 	 	= DEFAULT_DSP_MSG_ID_FILTER;   // message on
UINT16 gDspMsgSubIDFilter 	= DEFAULT_DSP_MSG_SUBID_FILTER;   // message on

#ifdef AUDIO_TEST_AUTO_BATCH     	
extern void doCmpCodecChksum(UINT32 index,UINT32 checksum);
#endif
static FifoMonInfo_t FifoMonInfo;
			
/******************************************************************************
	function implementations
 *****************************************************************************/
#ifdef DEBUG_CHECKSUM_VERIFICATION
INT32 HexToInt_i(char token[],int start_i)
{
	char ch;	
	int sum, v1;	
	sum=0;
	do {	   
		ch=token[start_i];
		if ( ch>='a' && ch <='f' )
			v1 = ch -'a' + 10;
		else if ( ch >= 'A' && ch <='F' )
			v1 = ch -'A' + 10;
		else
			v1 = ch -'0';

		sum = sum*16 + v1;
		start_i++;
	} while (token[start_i]!='\0');
	return sum;
}

INT32 StrToInt(char token[])
{
	if ( token[0]=='0' && (token[1]=='x'||token[1]=='X') )
		return HexToInt_i(token,2);
	else
		return HexToInt_i(token,0);
}


static DspChksumInfo_t gDec1ChksumInfo; 
static DspChksumInfo_t gDec2ChksumInfo; 

INT32 DSP_InitGoldChksumTbl(UINT8 codecBankID, char* pFilename)
{
	FILE* fp_golden = NULL;
	UINT32 idx = 0;
	UINT32* pChkSumTbl = NULL;
	UINT32 file_len = 0;
	char token[32];

	DspChksumInfo_t* pChecksumInfo = NULL;

	if (NULL == pFilename)
	{
		pChecksumInfo->bChksumExist = 0;		
		DSP_PRINT("DSP_InitGoldChksumTbl::e:filename is NULL!\n");      
		return 1;		
	}

    fp_golden = fopen(pFilename,"rb");	
	if( fp_golden == NULL )
	{
		pChecksumInfo->bChksumExist = 0;			
		DSP_PRINT("DSP_InitGoldChksumTbl::e:open failed:%s\n",pFilename);      
		return 1;
	}	

	// support only dec1&dec2 checksum comparsion
	switch(codecBankID)
	{
	case CODEC_BANK_DEC1:
    	memset((void*)&gDec1ChksumInfo, 0, sizeof(gDec1ChksumInfo));	

		pChecksumInfo = &gDec1ChksumInfo;
		strcpy(pChecksumInfo->filename, pFilename);
		pChecksumInfo->bChksumExist = 1;
		break;
	case CODEC_BANK_DEC2:
    	memset((void*)&gDec2ChksumInfo, 0, sizeof(gDec2ChksumInfo));			
		pChecksumInfo = &gDec2ChksumInfo;
		strcpy(pChecksumInfo->filename, pFilename);		
		pChecksumInfo->bChksumExist = 1;		
		break;		
	default:
		DSP_PRINT("DSP_InitGoldChksumTbl::e:codecBankID not support!\n");      
		return 1;		
	}


	fseek(fp_golden, 0, SEEK_END);
	file_len = ftell(fp_golden);		
	fseek(fp_golden, 0, SEEK_SET); // rewind	
	fscanf(fp_golden, "%s", token);
	if ( token[0]=='0' && (token[1]=='x'||token[1]=='X') )
	{
		pChecksumInfo->nChksumTblSize = file_len/12;  //0x12345678\CR\LF
	}
	else
	{
		pChecksumInfo->nChksumTblSize = file_len/10;
	}		

	if (pChecksumInfo->nChksumTblSize > K_MAX_GOLDEN_LEN)
	{
		DSP_PRINT("DSP_InitGoldChksumTbl::e:golden file entities are too large:%d\n",pChecksumInfo->nChksumTblSize);			
		pChecksumInfo->bChksumExist = 0;
		return 1;		
	}
		
	pChkSumTbl = pChecksumInfo->chksumTbl;
	pChkSumTbl[idx++] = StrToInt(token);        		
	while(1 == fscanf(fp_golden, "%s", token))
	{            
		if (idx > pChecksumInfo->nChksumTblSize)
		{
			DSP_PRINT("DSP_InitGoldChksumTbl::e:can't open golden file %s\n",pFilename);
			break;
		}
		pChkSumTbl[idx++] = StrToInt(token);
	}
	fclose(fp_golden);

	if (pChecksumInfo->bChksumExist)
	{
		DSP_PRINT("DSP_InitGoldChksumTbl::i:filename:%s, size:%d\n",pChecksumInfo->filename,pChecksumInfo->nChksumTblSize);      	
	}
	
	return 0;
	
	
}

#endif //DEBUG_CHECKSUM_VERIFICATION

#ifdef DEBUG_PRINT_DSP_MESSAGE
static void DspShowMessage(UINT32 dspMsgCmd, UINT32 dspMsgParam)
{

	static UINT32 oldDspMsg;
    UINT8  nID = (BYTE)((dspMsgCmd>>24) & 0xF);
    UINT8  nSubID = (BYTE)((dspMsgCmd>>20) & 0xF);

	//if (oldDspMsg == dspMsgParam) return;

	oldDspMsg = dspMsgParam;

	
	if ((gDspMsgIDFilter&(1<<nID)) && (gDspMsgSubIDFilter&(1<<nSubID)))		
    switch(nID)
    {
        /* dsp message always turned on  */     
        case MSG_ID_MAIN:
			if (dspMsgParam == 0xCDEF)
				diag_printf("DSP::main:%x:0x%08x\n",nSubID,dspMsgParam);
			else
				DSP_PRINT("DSP::main:%x:0x%08x\n",nSubID,dspMsgParam);
            break;
        case MSG_ID_DEC1:
				DSP_PRINT("DSP::dec1:%x:0x%08x\n",nSubID,dspMsgParam);
            break;
        case MSG_ID_DEC2:
				DSP_PRINT("DSP::dec2:%x:0x%08x\n",nSubID,dspMsgParam);
            break;
        case MSG_ID_ENC:
				DSP_PRINT("DSP::enc :%x:0x%08x\n",nSubID,dspMsgParam);
            break;
        case MSG_ID_DBG:			
				diag_printf("DSP::dbg:%x:0x%08x\n",nSubID,dspMsgParam);
            break;
        case MSG_ID_PRF:
				DSP_PRINT("DSP::prf :%x:%010d\n",nSubID,dspMsgParam); 
            break;
        case MSG_ID_CHKSUM:
				DSP_PRINT("DSP::chk :%x:0x%08x\n",nSubID,dspMsgParam);    
            break;  
        case MSG_ID_DMA:            
				DSP_PRINT("DSP::dma :%x:%010d\n",nSubID,dspMsgParam);
            break;
        case MSG_ID_ISR:            
				DSP_PRINT("DSP::isr :%x:0x%08x\n",nSubID,dspMsgParam);
            break;          
        case MSG_ID_APP:            
				DSP_PRINT("DSP::app :%x:0x%08x\n",nSubID,dspMsgParam);
            break;   
        case MSG_ID_SWAPP:            
				DSP_PRINT("DSP::sapp:%x:0x%08x\n",nSubID,dspMsgParam);
            break;              
        case MSG_ID_MIXER:           
				DSP_PRINT("DSP::mix :%x:0x%08x\n",nSubID,dspMsgParam);
            break; 						
        case MSG_ID_FPGA:   
				DSP_PRINT("DSP::fpga:%x:0x%08x\n",nSubID,dspMsgParam);
            break;              
 			
        default:                        
            DSP_PRINT("DSP::id%x:%x:0x%08x\n",nID,nSubID,dspMsgParam);                        
    };    
}
#endif //DEBUG_PRINT_DSP_MESSAGE

#ifdef DEBUG_ENABLE_DSP_MONITOR
static UINT32 DspGetSampleRate(UINT16 sampleRateID)
{
	UINT32 sampleRate = 0;
	
	// ND Sampling Rate ID:
    // 44.1k: 0x0000 88.2k: 0x0001 22.05k: 0x0002 176.4k: 0x0003 11.025k: 0x0004 5.5125k: 0x0005
    // 48  k: 0x0010 96  k: 0x0011 24   k: 0x0012 192  k: 0x0013 12    k: 0x0014      6k: 0x0015
    // 32  k: 0x0020 64  k: 0x0021 16   k: 0x0022 128  k: 0x0023  8    k: 0x0024      4k: 0x0025
    // 768 k: 0x0030 Not indicated: 0x0031
    // K_DSP_SAMPLE_RATE_ID_44K
	switch(sampleRateID)
	{
	case 0x0:
		sampleRate = 44100;
		break;
	case 0x1:
		sampleRate = 88200;
		break;			
	case 0x2:
		sampleRate = 22050;
		break;			
	case 0x3:
		sampleRate = 176400;
		break;
	case 0x4:
		sampleRate = 11025;
		break;			
	case 0x5:
		sampleRate = 5512;
		break;			
	case 0x10:
		sampleRate = 48000;
		break;			
	case 0x11:
		sampleRate = 96000;
		break;			
	case 0x12:
		sampleRate = 24000;
		break;			
	case 0x13:
		sampleRate = 192000;			
		break;			
	case 0x14:
		sampleRate = 12000;			
		break;			
	case 0x15:
		sampleRate = 6000;		
		break;			
	case 0x20:
		sampleRate = 32000;
		break;			
	case 0x21:
		sampleRate = 64000;
		break;			
	case 0x22:
		sampleRate = 16000;
		break;			
	case 0x23:
		sampleRate = 128000;			
		break;			
	case 0x24:
		sampleRate = 8000;			
		break;			
	case 0x25:
		sampleRate = 6000;				
		break;			
	case 0x30:
		sampleRate = 768000;		
		break;			
	default:
		sampleRate = 0;		
	}		
	
	return sampleRate;	
}


ProfileHistoryInfo_t* DspMonGetPrfHistoryInfo(UINT16 monID)
{
	//
	// monID          : 1 ~ (MAX_DSPMON_NUM)
	// PrfHistoryInfo : 0 ~ (MAX_DSPMON_NUM -1)
	if (monID > MAX_DSPMON_NUM) 
		return NULL;
	else
		return (&PrfHistoryInfo[monID-1]);
}

void DspMonInitPrfHistory(UINT16 monID)
{
	DSPAPI_PRINT("dsp_Mon_InitPrfHistory::monID:%d\n",monID);

	if (monID >= MAX_DSPMON_NUM)
	{
		ERROR_PRINT("dsp_Mon_InitPrfHistory:e:monID >= MAX_DSPMON_NUM\n");
		return;
	}
	
	memset(&PrfHistoryInfo[monID-1],0,sizeof(ProfileHistoryInfo_t));
	
	ProfileHistoryInfo_t* pPrfHisInfo = &PrfHistoryInfo[monID-1];

	pPrfHisInfo->MonID = monID;
	pPrfHisInfo->mips_min = 0xFFFFFFFF;
	pPrfHisInfo->mips_cache_min = 0xFFFFFFFF; 
	pPrfHisInfo->mips_dma_min   = 0xFFFFFFFF; 	
}

#if 0
void DspMonPrintHistory(UINT16 monID)
{
	float mips_avg;
	float dmaBW_avg;
	float dma_avg;
	float cache_avg;
	float cacheBW_avg;
	ProfileHistoryInfo_t* pPrfHisInfo = &PrfHistoryInfo[monID-1];

	if (pPrfHisInfo->MonID == 0)
	{
		return;
	}

	mips_avg    = pPrfHisInfo->allCycles/pPrfHisInfo->time_us;
	dma_avg     = pPrfHisInfo->allDmaCycles/pPrfHisInfo->time_us;
	cache_avg   = pPrfHisInfo->allCacheCycles/pPrfHisInfo->time_us;
	dmaBW_avg   = pPrfHisInfo->allDMABandWidthCnt * 128/pPrfHisInfo->time_us;
	cacheBW_avg = pPrfHisInfo->allCacheBWCount/pPrfHisInfo->time_us;
	MONPRF_PRINT("DSP::DspMonPrintHistory:\n");	
	MONPRF_PRINT("DSP::avg :%d:dsp:0x%08x:%.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,mips_avg);
	MONPRF_PRINT("DSP::max :%d:dsp:0x%08x:%06.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->mips_max_frame,pPrfHisInfo->mips_max);
	MONPRF_PRINT("DSP::min :%d:dsp:0x%08x:%06.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->mips_min_frame,pPrfHisInfo->mips_min);	
	MONPRF_PRINT("DSP::avg :%d:che:0x%08x:%.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,cache_avg);	
	MONPRF_PRINT("DSP::max :%d:che:0x%08x:%06.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->mips_cache_max_frame,pPrfHisInfo->mips_cache_max);
	MONPRF_PRINT("DSP::min :%d:che:0x%08x:%06.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->mips_cache_min_frame,pPrfHisInfo->mips_cache_min);		
	MONPRF_PRINT("DSP::avg :%d:che:0x%08x:%.2f (bw)\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,cacheBW_avg);					
	MONPRF_PRINT("DSP::avg :%d:dma:0x%08x:%.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,dma_avg);
	MONPRF_PRINT("DSP::max :%d:dma:0x%08x:%06.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->mips_dma_max_frame,pPrfHisInfo->mips_dma_max);
	MONPRF_PRINT("DSP::min :%d:dma:0x%08x:%06.2f\n",pPrfHisInfo->MonID,pPrfHisInfo->mips_dma_min_frame,pPrfHisInfo->mips_dma_min);				
	MONPRF_PRINT("DSP::avg :%d:dma:0x%08x:%.2f (bw)\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,dmaBW_avg);		

//	MONPRF_PRINT("DSP::avg :%d:che:0x%08x:%d\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,pPrfHisInfo->allCacheBurstCnt);
//	MONPRF_PRINT("DSP::avg :%d:che:0x%08x:%d\n\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts,pPrfHisInfo->allCacheSingleCnt);	

	
}

static void DspMonProfiler(UINT32 drameAddress)
{
	// convert a bulk dram address to type ProfilerInfo_t
	ProfilerInfo_t* pDspProfilerInfo = (ProfilerInfo_t*)drameAddress;
	ProfileHistoryInfo_t* pPrfHisInfo = NULL;
	
	float mips_now;
	float mips_cache;
	float mips_dma;		
	float dma_bw;
	float cache_bw;
	float time_us;
	UINT32 sampleRate;
#ifndef AUDIO_TEST_AUTO_BATCH     	    
	UINT8 bChksumMatched = 1;
#endif
	ProfilerInfo_t NowPrfInfo;		
	NowPrfInfo.MonID              = WORD16(pDspProfilerInfo->MonID);
	NowPrfInfo.dwFrameCount       = WORD32(pDspProfilerInfo->dwFrameCount);
	NowPrfInfo.SampleRateID       = WORD16(pDspProfilerInfo->SampleRateID);                           
	NowPrfInfo.BlockSize          = WORD16(pDspProfilerInfo->BlockSize);                     
	NowPrfInfo.dwChecksum         = WORD32(pDspProfilerInfo->dwChecksum);                    
	NowPrfInfo.dwDspCycleCnt      = WORD32(pDspProfilerInfo->dwDspCycleCnt);                 
	NowPrfInfo.dwDspDMACycleCnt   = WORD32(pDspProfilerInfo->dwDspDMACycleCnt);              
	NowPrfInfo.dwDspCacheCycleCnt = WORD32(pDspProfilerInfo->dwDspCacheCycleCnt);            
	NowPrfInfo.dwDspDMABWCnt      = WORD32(pDspProfilerInfo->dwDspDMABWCnt);                   
//	NowPrfInfo.dwDspCacheMissedCnt= WORD32(pDspProfilerInfo->dwDspCacheMissedCnt);             
	NowPrfInfo.dwDspCacheBurstCnt = WORD32(pDspProfilerInfo->dwDspCacheBurstCnt);             
	NowPrfInfo.dwDspCacheSingleCnt= WORD32(pDspProfilerInfo->dwDspCacheSingleCnt);  
	NowPrfInfo.dwDspDMACycleMax	  = WORD32(pDspProfilerInfo->dwDspDMACycleMax);				
	NowPrfInfo.dwDspDMACycleMin   = WORD32(pDspProfilerInfo->dwDspDMACycleMin);  	
	
	sampleRate = DspGetSampleRate(NowPrfInfo.SampleRateID);	
	time_us = 1000000*NowPrfInfo.BlockSize/sampleRate;
		
	mips_now   = NowPrfInfo.dwDspCycleCnt/time_us;
	mips_cache = NowPrfInfo.dwDspCacheCycleCnt/time_us;
	mips_dma   = NowPrfInfo.dwDspDMACycleCnt/time_us;
	dma_bw     = NowPrfInfo.dwDspDMABWCnt*128/time_us;
	cache_bw   = (32*NowPrfInfo.dwDspCacheBurstCnt+8*NowPrfInfo.dwDspCacheSingleCnt)/time_us;

	// Check NowPrfInfo.MonID
	// MonID
	if (NowPrfInfo.MonID >= MAX_DSPMON_NUM)
	{
		ERROR_PRINT("DspMonProfiler:e:NowPrfInfo.MonID >= MAX_DSPMON_NUM\n");
		return;
	}

	pPrfHisInfo = DspMonGetPrfHistoryInfo(NowPrfInfo.MonID);
	if ( pPrfHisInfo == NULL ) 
	{
		ERROR_PRINT("DspMonProfiler:e:pPrfHisInfo is NULL\n");
		return; 
	}
		
	if (0 == pPrfHisInfo->MonID) DspMonInitPrfHistory(NowPrfInfo.MonID);	

/*
	DSP_PRINT("DspMonPrintHistory::drameAddress:%x\n",drameAddress);
	DSP_PRINT("DspMonPrintHistory::NowPrfInfo:MonID:%d\n",NowPrfInfo.MonID );
	DSP_PRINT("DspMonPrintHistory::NowPrfInfo:dwFrameCount:%x\n",NowPrfInfo.dwFrameCount);
*/		


#ifdef AUDIO_TEST_AUTO_BATCH     	
	// compare use Dec1
	if (NowPrfInfo.MonID == 1)
	{
		doCmpCodecChksum(NowPrfInfo.dwFrameCount-1, NowPrfInfo.dwChecksum);
		return;
	}
#else	
		
	switch(NowPrfInfo.MonID)
	{
	case 1:					
#ifdef DEBUG_CHECKSUM_VERIFICATION

		if (gDec1ChksumInfo.bChksumExist)
		{
			UINT32 nGolden = gDec1ChksumInfo.chksumTbl[NowPrfInfo.dwFrameCount - 1];	
			if(NowPrfInfo.dwChecksum != nGolden)
			{
				DSP_PRINT("\nDSP::dec1:chksum error:[%d]golden:0x%08x, chk:0x%08x\n", NowPrfInfo.dwFrameCount,nGolden, NowPrfInfo.dwChecksum);
				bChksumMatched = 0;

#ifdef CHECKSUM_ERROR_DSP_LOCK	
				if (NowPrfInfo.dwFrameCount > gDec1ChksumInfo.nChksumTblSize)
					DSP_PRINT("DSP::dec1:dwFrameCount is larger than nChksumTblSize, please check golden checksum file\n");
				else
					while(1) GL_TaskSleep(100);
#endif //CHECKSUM_ERROR_DSP_LOCK					
		
			}
			else
			{
				//DSP_PRINT("DSP::dec1:%04d:golden:0x%08x,chk:0x%08x\n", NowPrfInfo.dwFrameCount,nGolden, NowPrfInfo.dwChecksum);
				bChksumMatched = 1;
			}
		}
	
#endif  //DEBUG_CHECKSUM_VERIFICATION
		break;		
	case 2:		
#ifdef DEBUG_CHECKSUM_VERIFICATION

		if (gDec2ChksumInfo.bChksumExist)
		{
			UINT32 nGolden = gDec2ChksumInfo.chksumTbl[NowPrfInfo.dwFrameCount - 1];	
			if(NowPrfInfo.dwChecksum != nGolden)
			{
				DSP_PRINT("\nDSP::dec2:chksum error:[%d]golden:0x%08x, chk:0x%08x\n", NowPrfInfo.dwFrameCount,nGolden, NowPrfInfo.dwChecksum);				
				bChksumMatched = 0;

#ifdef CHECKSUM_ERROR_DSP_LOCK	
				if (NowPrfInfo.dwFrameCount > gDec2ChksumInfo.nChksumTblSize)
					DSP_PRINT("DSP::dec2:dwFrameCount is larger than nChksumTblSize, please check golden checksum file\n");
				else
					while(1) GL_TaskSleep(100);
#endif //CHECKSUM_ERROR_DSP_LOCK				
		
			}
			else
			{
				//DSP_PRINT("DSP::dec2:%04d:golden:0x%08x,chk:0x%08x\n", NowPrfInfo.dwFrameCount,nGolden, NowPrfInfo.dwChecksum);
				bChksumMatched = 1;
			}
		}
	
#endif  //DEBUG_CHECKSUM_VERIFICATION
		break;	
	case 4:
/*		for fixed DTS encoder only
		
		NowPrfInfo.BlockSize = 512;
		sampleRate = 48000;		
		time_us = 1000000*NowPrfInfo.BlockSize/sampleRate;
		mips_now   = NowPrfInfo.dwDspCycleCnt/time_us;
		mips_cache = NowPrfInfo.dwDspCacheCycleCnt/time_us;
		mips_dma   = NowPrfInfo.dwDspDMACycleCnt/time_us;
		dma_bw     = NowPrfInfo.DspDMABWCnt*128/time_us;		
*/
	case 3:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:		
		bChksumMatched = 1;
		break;
	}
	
	if(bChksumMatched)					// if checksum matched, do statistics
	{
		pPrfHisInfo->frameCounts += 1;
		pPrfHisInfo->time_us += time_us;
		
		pPrfHisInfo->mips_max = MAX(pPrfHisInfo->mips_max, mips_now);
		if ((int)pPrfHisInfo->mips_max == (int)mips_now)
			pPrfHisInfo->mips_max_frame = NowPrfInfo.dwFrameCount;				
		pPrfHisInfo->mips_min = MIN(pPrfHisInfo->mips_min, mips_now);
		if ((int)pPrfHisInfo->mips_min == (int)mips_now)		
			pPrfHisInfo->mips_min_frame = NowPrfInfo.dwFrameCount;				

		pPrfHisInfo->mips_dma_max = MAX(pPrfHisInfo->mips_dma_max, mips_dma);
		if ((int)pPrfHisInfo->mips_dma_max == (int)mips_dma)
			pPrfHisInfo->mips_dma_max_frame = NowPrfInfo.dwFrameCount;				
		pPrfHisInfo->mips_dma_min = MIN(pPrfHisInfo->mips_dma_min, mips_dma);
		if ((int)pPrfHisInfo->mips_dma_min == (int)mips_dma)		
			pPrfHisInfo->mips_dma_min_frame = NowPrfInfo.dwFrameCount;				

		pPrfHisInfo->mips_cache_max = MAX(pPrfHisInfo->mips_cache_max, mips_cache);
		if ((int)pPrfHisInfo->mips_cache_max == (int)mips_cache)
			pPrfHisInfo->mips_cache_max_frame = NowPrfInfo.dwFrameCount;				
		pPrfHisInfo->mips_cache_min = MIN(pPrfHisInfo->mips_cache_min, mips_cache);
		if ((int)pPrfHisInfo->mips_cache_min == (int)mips_cache)		
			pPrfHisInfo->mips_cache_min_frame = NowPrfInfo.dwFrameCount;				
		
	    pPrfHisInfo->allCycles += NowPrfInfo.dwDspCycleCnt ;
		pPrfHisInfo->allCacheCycles += NowPrfInfo.dwDspCacheCycleCnt;
		pPrfHisInfo->allDmaCycles += NowPrfInfo.dwDspDMACycleCnt;			
		pPrfHisInfo->allDMABandWidthCnt += NowPrfInfo.dwDspDMABWCnt;
//		pPrfHisInfo->allCacheMissedCount += NowPrfInfo.dwDspCacheMissedCnt;
		// Cache Bw (Byte) = 32*BurstCnt + 8*SingleCnt
		pPrfHisInfo->allCacheBWCount += 32*NowPrfInfo.dwDspCacheBurstCnt + 8*NowPrfInfo.dwDspCacheSingleCnt;
		pPrfHisInfo->allCacheBurstCnt += NowPrfInfo.dwDspCacheBurstCnt;
		pPrfHisInfo->allCacheSingleCnt += NowPrfInfo.dwDspCacheSingleCnt;
		
		pPrfHisInfo->dma_max = MAX(pPrfHisInfo->dma_max, NowPrfInfo.dwDspDMACycleMax);
		pPrfHisInfo->dma_min = MIN(pPrfHisInfo->dma_min, NowPrfInfo.dwDspDMACycleMin);
	}
	
//	DSP_PRINT("DspMonProfiler::pPrfHisInfo->MonID:%d,pPrfHisInfo->frameCounts:%d\n",pPrfHisInfo->MonID,pPrfHisInfo->frameCounts);	
			
#ifdef DEBUG_ENABLE_PERFRAME_MONITOR
	MONPRF_PRINT("DSP::Profiler information:\n");	
	MONPRF_PRINT("DSP::%d:blk:0x%08x:0x%08x\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,NowPrfInfo.BlockSize);			
	MONPRF_PRINT("DSP::%d:chk:0x%08x:0x%08x\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,NowPrfInfo.dwChecksum);			
	MONPRF_PRINT("DSP::%d:dsp:0x%08x:%.2f\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,mips_now);
	MONPRF_PRINT("DSP::%d:che:0x%08x:%.2f\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,mips_cache);
	MONPRF_PRINT("DSP::%d:che:0x%08x:%.2f (bw)\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,cache_bw);		
	MONPRF_PRINT("DSP::%d:dma:0x%08x:%.2f\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,mips_dma);	
	MONPRF_PRINT("DSP::%d:dma:0x%08x:%.2f (bw)\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,dma_bw);			
//	MONPRF_PRINT("DSP::%d:che:0x%08x:%d\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,NowPrfInfo.dwDspCacheBurstCnt);	
//	MONPRF_PRINT("DSP::%d:che:0x%08x:%d\n",NowPrfInfo.MonID,NowPrfInfo.dwFrameCount,NowPrfInfo.dwDspCacheSingleCnt);		
#endif //DEBUG_ENABLE_PERFRAME_MONITOR
#endif //AUDIO_TEST_AUTO_BATCH

}
#endif //DEBUG_ENABLE_DSP_MONITOR

#endif

static void DspShowDramMessage(UINT32 dspMsgCmd, UINT32 dspMsgParam)
{

    #define MSG_DRAM_ID_DUMP_BYTE		0x0 	// dump dram contents
    #define MSG_DRAM_ID_MON_INFO		0x1		// dsp monitor profiler
    #define MSG_DRAM_ID_DUMP_IDM        0x2

//	DSPAPI_PRINT("dspShowDramMessage::dspMsgCmd:0x%x, dspMsgParam:0x%x\n",dspMsgCmd,dspMsgParam);
	
	UINT32 drameAddress = DSP_GetInfo(INFO_DATA_BANK_BASE) + (dspMsgParam&0xFFFFFF);
	UINT16 drameLen = dspMsgCmd&0xFFFF; 	
	UINT8  dramID = (BYTE)((dspMsgCmd>>24) & 0xF);

	switch(dramID)
	{
	case MSG_DRAM_ID_DUMP_BYTE:
		DSPAPI_PRINT("DSP::dram:0x%x:%d\n",drameAddress,drameLen);
		dumpByte(drameAddress,drameLen);
		break;
	case MSG_DRAM_ID_MON_INFO:
		//DspMonProfiler(drameAddress);		
		break;	
	case MSG_DRAM_ID_DUMP_IDM:
		DSPAPI_PRINT("DSP::dram:0x%x:%d\n",drameAddress,drameLen);
		dumpDspIDM(drameAddress,drameLen);
		break;
	}		
}

 
UINT32 DSP_GetMessage(UINT32* dspMsg)
{
	UINT32 dspMsgCmdPort = HWREG_R(dsp_port[10]);
    UINT32 dspMsgParamPort = HWREG_R(dsp_port[11]); 			// if long message
    
    UINT8  type = (BYTE)((dspMsgCmdPort>>28) & 0x3);
	
	if (0x80000000 & dspMsgCmdPort)
        return DSP_ERROR_NO_MESSAGE;	
	
	if (MSG_TYPE_DRAM == type)
	{
		// check if dram message
		DSP_SHOW_DRAM_MESSAGE(dspMsgCmdPort,dspMsgParamPort);
	}
	else
	{
		// short message or long message
		
		if (MSG_TYPE_SHORT == type) 
			dspMsgParamPort = dspMsgCmdPort&0xFFFF; 
		
		DSP_SHOW_MESSAGE(dspMsgCmdPort, dspMsgParamPort);	
		
	}	
	*dspMsg = dspMsgParamPort;

	HWREG_W(dsp_port[10], HWREG_R(dsp_port[10]) | 0x80000000);	// Set MSB of message port	
	
    return DSP_ERROR_NONE;
}

// hcsu:todo:should create a port register usage map

UINT32 DSP_GetMessageFlag(void)
{	
	UINT32 nDSPPort10 = HWREG_R(dsp_port[10]);
	return (( 0 == (0x80000000 & nDSPPort10)));	
}

void DSP_SetMessageFlag(void)
{
	HWREG_W(dsp_port[10], HWREG_R(dsp_port[10]) | 0x80000000);
	return;
}


UINT32 DSP_GetNewMessage(void)
{
	
	UINT32 dspMsgCmdPort = HWREG_R(dsp_port[10]);
    UINT32 dspMsgParamPort = HWREG_R(dsp_port[11]); // if long message   
    UINT8  type = (BYTE)((dspMsgCmdPort>>28) & 0x3);

	
	// Currently only three types 
	// 1. MSG_TYPE_SHORT : short message
	// 2. MSG_TYPE_LONG  : long message
	// 3. MSG_TYPE_DRAM  : dram message

	if (MSG_TYPE_DRAM == type)
	{
		// check if dram message
		DSP_SHOW_DRAM_MESSAGE(dspMsgCmdPort,dspMsgParamPort);
	}
	else
	{
		// short message or long message
		
		if (MSG_TYPE_SHORT == type) 
			dspMsgParamPort = dspMsgCmdPort&0xFFFF; 

		DSP_SHOW_MESSAGE(dspMsgCmdPort, dspMsgParamPort);	
	}
	
    return dspMsgParamPort;
}
 
 

void DSP_ShowPrfResult(void)
{
	UINT16 monID;
	UINT32 saveFilter;
	ProfileHistoryInfo_t* pPrfHistoryInfo = NULL;
	
	for (monID = 1; monID<MAX_DSPMON_NUM; monID++)
	{	
		pPrfHistoryInfo =  DspMonGetPrfHistoryInfo(monID);
		if (NULL == pPrfHistoryInfo) continue;
		extern UINT32 gPrintFilter;
		saveFilter = gPrintFilter;
		gPrintFilter |= PRINT_ID_MONPRF;
		//DspMonPrintHistory(monID);
		gPrintFilter = saveFilter;
	}
}


void DSP_FifoMonitor(UINT8 cmd, UINT32 param)
{
    // cmd                
    #define FIFOMON_INIT 0  // 0 : to initialize FIFO monitor state machine
    #define FIFOMON_TRIG 1  // 1 : to trigger the FIFO monitor counter
    #define FIFOMON_SHOW 2  // 2 : to print   the FIFO monitor counter
    #define FIFOMON_STOP 3  // 3 : to clean   the FIFO monitor counter

    UINT32 dspDramBase = param;
    
    switch(cmd)
    {
        case FIFOMON_INIT:                        

            if (0==dspDramBase)
            {
                DSPAPI_PRINT("DSP_FifoMonitor::e:Illegal DramBase Addr 0x%08X\n", dspDramBase);
                break;
            }

            DSPAPI_PRINT("DSP_FifoMonitor::INIT STATE\n");
            
            FifoMonInfo.state = 1;
            FifoMonInfo.isRead = 0;

            if (IS_DRAM_BANKA(dspDramBase))
                FifoMonInfo.isDramA = 1;
            else
                FifoMonInfo.isDramA = 0;            

            HWREG_W(sd0_asref_0,0x3813);
            DSPAPI_PRINT("DSP_FifoMonitor::G33.8 value: 0x%08X(%d)\n",HWREG_R(sd0_asref_0), HWREG_R(sd0_asref_0));
            
            break;

        case FIFOMON_TRIG:
            if (FifoMonInfo.state < 1)
            {                
                DSPAPI_PRINT("DSP_FifoMonitor::e:state [0x%02X], need to initialize\n", FifoMonInfo.state);
                return;
            }   

            DSPAPI_PRINT("DSP_FifoMonitor::set TRIG STATE\n");

            //DSPAPI_PRINT("DSP_FifoMonitor::G33.8 value: 0x%08X(%d)\n",HWREG_R(sd0_asref_0), HWREG_R(sd0_asref_0));

            // trigger FIFO lantency measurement
            HWREG_W(g68_reserved[24],0x3);

            if (FifoMonInfo.isDramA)  //use DRAM A
                HWREG_W(g30_reserved[0], 0xa);
            else
                HWREG_W(g43_reserved[0], 0xa);

            FifoMonInfo.state = 2;
            
            break;


        case FIFOMON_SHOW:
            if (FifoMonInfo.state < 2)
            {                
                DSPAPI_PRINT("DSP_FifoMonitor::e:state [0x%02X], need to trigger\n", FifoMonInfo.state);
                break;
            }

            UINT32 temp0, temp1, temp2;
            UINT32 cmd_cnt = HWREG_R(g68_reserved[25]);
            UINT32 cmd_cyc = HWREG_R(g68_reserved[26]);
            UINT32 max_cyc = HWREG_R(g68_reserved[27]);

            if ((cmd_cnt==0xFFFFFFFF) || (cmd_cyc==0xFFFFFFFF))
            {
                DSPAPI_PRINT("DSP_FifoMonitor::e:count exploded, please reset FIFO mon\n");             
                DSPAPI_PRINT("DSP_FifoMonitor::e:cmd_cnt:  0x%08X\n", cmd_cnt);             
                DSPAPI_PRINT("DSP_FifoMonitor::e:cmd_cyc:  0x%08X\n", cmd_cyc);                         
                break;
            }
            
            //float avg_cnt = ((float)cmd_cnt)/((float)cmd_cyc);            
            
            diag_printf("DSP_FifoMonitor::max_cyc  :0x%08X (%d)\n", max_cyc, max_cyc);                         
            diag_printf("DSP_FifoMonitor::cmd_cnt  :0x%08X (%d)\n", cmd_cnt, cmd_cnt); 
            diag_printf("DSP_FifoMonitor::cmd_cyc  :0x%08X (%d)\n", cmd_cyc, cmd_cyc);             
            //diag_printf("DSP_FifoMonitor::avg_cyc  :%f\n", avg_cnt);             
         
            if(FifoMonInfo.isDramA)  //use DRAM A
            {
                temp0 = HWREG_R(g30_reserved[2])&0xFFFF;
                temp1 = HWREG_R(g30_reserved[3])&0xFFFF;
                temp2 = HWREG_R(g30_reserved[4])&0xFFFF;
            }
            else
            {
                temp0 = HWREG_R(g43_reserved[2])&0xFFFF;
                temp1 = HWREG_R(g43_reserved[3])&0xFFFF;
                temp2 = HWREG_R(g43_reserved[4])&0xFFFF;
            }

            diag_printf("DSP_FifoMonitor::mbar_cnt1:0x%04X (%5d)\n",temp0, temp0);
            diag_printf("DSP_FifoMonitor::mbar_cnt2:0x%04X (%5d)\n",temp1, temp1); 
            diag_printf("DSP_FifoMonitor::mbar_cnt3:0x%04X (%5d)\n",temp2, temp2); 

            
            break;

        case FIFOMON_STOP:

            DSPAPI_PRINT("DSP_FifoMonitor::STOP STATE:reset counters\n");
            
            HWREG_W(g68_reserved[24],0x0);

            if (FifoMonInfo.isDramA)  //use DRAM A
                HWREG_W(g30_reserved[0], 0x80000000);
            else
                HWREG_W(g43_reserved[0], 0x80000000);            

            FifoMonInfo.state = 1;
            
            break;

        default:
            DSPAPI_PRINT("DSP_FifoMonitor::e: error cmd\n");                    
    }
}
