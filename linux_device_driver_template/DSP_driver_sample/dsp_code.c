/******************************************************************************
 * Filename    : dsp_code.c
 * 
 * Description : dsp source code file
 * Language    : C Language 
 *           
 * Revision History: 
 * ------------------
 * [100401]hcsu Initial Draft
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
#include "include/aud_global.h"
#include "include/memmap_dsp.h"
#include "include/dsp_code.h"

/******************************************************************************
    DSP codes
 *****************************************************************************/
#include "dsp_codec/aac_code.inc"
#include "dsp_codec/aac_romtable.inc"
#include "dsp_codec/ac3_enc_code.inc"
#include "dsp_codec/ac3_enc_romtable.inc"
#include "dsp_codec/alac_code.inc"
#include "dsp_codec/amr_code.inc"
#include "dsp_codec/amr_romtable.inc"
#include "dsp_codec/ape_code.inc"
#include "dsp_codec/ape_romtable.inc"
#include "dsp_codec/avs_code.inc"
#include "dsp_codec/avs_romtable.inc"
#include "dsp_codec/bank0_code.inc"
#include "dsp_codec/ddp_code.inc"
#include "dsp_codec/ddp_romtable.inc"
#include "dsp_codec/dts_enc_code.inc"
#include "dsp_codec/dts_enc_romtable.inc"
#include "dsp_codec/dtshd_code.inc"
#include "dsp_codec/dtshd_romtable.inc"
#include "dsp_codec/dtslbr_code.inc"
#include "dsp_codec/emu_code.inc"
#include "dsp_codec/flac_code.inc"
#include "dsp_codec/heaac_code.inc"
#include "dsp_codec/heaac_romtable.inc"
#include "dsp_codec/midi_code.inc"
#include "dsp_codec/mpeg_code.inc"
#include "dsp_codec/mpeg_enc_code.inc"
#include "dsp_codec/mpeg_enc_romtable.inc"
#include "dsp_codec/mpeg_romtable.inc"
#include "dsp_codec/ogg_code.inc"
#include "dsp_codec/ogg_romtable.inc"
#include "dsp_codec/pcm_code.inc"
#include "dsp_codec/ra_code.inc"
#include "dsp_codec/ra_romtable.inc"
#include "dsp_codec/swapp_code.inc"
#include "dsp_codec/truehd_code.inc"
#include "dsp_codec/wma_code.inc"
#include "dsp_codec/wmapro_code.inc"
#include "dsp_codec/wmapro_romtable.inc"
#include "dsp_codec/wma_romtable.inc"

/******************************************************************************
    Local Defines
 *****************************************************************************/
//#define DEBUG_DUMP_DSP_CODEINFO
#define DEBUG_DUMP_DSP_SIZE
#define DISABLE_DSPCODE_SCRAMBLE			// if enabled, no scrambled


#define ALIGNMENT(x,n)              ((x>>n)<<n)             	// align x with 2^n 
#define ALIGNSIZE(x,n)              (ALIGNMENT(x,n)+(1<<n))   	// align x with 2^n     
#define MAX(x,y) 					((x) >= (y)) ? (x):(y)
#define MIN(x,y)                	((x) >= (y)) ? (y):(x)


/******************************************************************************
    Global variables
 ******************************************************************************/
static DspCode* gCodebankPtrList[MAX_CODEBANK_ID];	
static DspCodeSize gDspCodeSize;
static DspCode gCodebank0x00;
static DspCode gCodebank0x01;
static DspCode gCodebank0x02;
static DspCode gCodebank0x03;
static DspCode gCodebank0x04;
static DspCode gCodebank0x05;
static DspCode gCodebank0x06;
static DspCode gCodebank0x07;
static DspCode gCodebank0x08;
static DspCode gCodebank0x09;
static DspCode gCodebank0x0a;
static DspCode gCodebank0x0b;
static DspCode gCodebank0x0c;
static DspCode gCodebank0x0d;
static DspCode gCodebank0x0e;
static DspCode gCodebank0x0f;
static DspCode gCodebank0x10;
static DspCode gCodebank0x11;
static DspCode gCodebank0x12;
static DspCode gCodebank0x13;
static DspCode gCodebank0x14;
static DspCode gCodebank0x15;
static DspCode gCodebank0x16;
static DspCode gCodebank0x17;
static DspCode gCodebank0x18;
static DspCode gCodebank0x19;
static DspCode gCodebank0x1a;
static DspCode gCodebank0x1b;
static DspCode gCodebank0x1c;
static DspCode gCodebank0x1d;
static DspCode gCodebank0x1e;
static DspCode gCodebank0x1f;

/******************************************************************************
   Static Function Implementations
 *****************************************************************************/

static const UINT8* DSP_ReadWord(const UINT8* pSource, WORD* word)
{
	// Big endian -> little endian
	*word  = (WORD)pSource[1];
	*word |= (WORD)pSource[0]<<8;
	return (pSource+2);
}

static const UINT8* DSP_ReadDWord(const UINT8* pSource, DWORD* dword)
{
	// Big endian -> little endian 	
	*dword  = (DWORD)pSource[3];
	*dword |= (DWORD)pSource[2]<<8;
	*dword |= (DWORD)pSource[1]<<16;
	*dword |= (DWORD)pSource[0]<<24;	
	return (pSource+4);
}



static void DSP_SetDSPCodeInfo(UINT8 codecID, DspCodeInfo * pDspCodeInfo)
{
//
// for OLD memory DSP settings, should manually maintained if needs 
// (Please use new DSP code with codeinfo, not continue this)
//
#define K_DRAM_BUF_SIZE_MAIN   						 700
#define K_DRAM_BUF_SIZE_PCM   						 100
#define K_DRAM_BUF_SIZE_DDP   						 135
#define K_DRAM_BUF_SIZE_TRUEHD 						 57
#define K_DRAM_BUF_SIZE_DTSLBR 						 62
#define K_DRAM_BUF_SIZE_DTSHD 						 208
#define K_DRAM_BUF_SIZE_MPEG  						 130
#define K_DRAM_BUF_SIZE_AAC   						 96
#define K_DRAM_BUF_SIZE_WMA   						 132
#define K_DRAM_BUF_SIZE_RA     						 100
#define K_DRAM_BUF_SIZE_OGG   						 100
#define K_DRAM_BUF_SIZE_EMU   						 30
#define K_DRAM_BUF_SIZE_WMAPRO 						 100
#define K_DRAM_BUF_SIZE_FLAC 						 145
#define K_DRAM_BUF_SIZE_AMR 						 100
#define K_DRAM_BUF_SIZE_MIDI                         700
#define K_DRAM_BUF_SIZE_APE                          38
#define K_DRAM_BUF_SIZE_AVS                          135
#define K_DRAM_BUF_SIZE_MPEG_ENC 					 40
#define K_DRAM_BUF_SIZE_AC3_ENC 					 34
#define K_DRAM_BUF_SIZE_DTS_ENC 					 14

	switch (codecID)
    {
    case ID_MAIN:
		pDspCodeInfo->dwCodeSize       = sizeof(bank0_code);		//K_DSPKRNL_CODE_SIZE_BANK0;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = 0;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_MAIN;   
        break;
    case ID_DECODER_PCM:
		pDspCodeInfo->dwCodeSize       = sizeof(pcm_code);			//K_DSPKRNL_CODE_SIZE_PCM;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_PCM;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_PCM;  		
        break;
    case ID_DECODER_DDP:
		pDspCodeInfo->dwCodeSize       = sizeof(ddp_code);			//K_DSPKRNL_CODE_SIZE_DDP;   
		pDspCodeInfo->dwRomtableSize   = sizeof(ddp_romtable);		//K_DSPKRNL_ROMTABLE_SIZE_DDP;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_DDP;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_DDP;  		
        break;
    case ID_DECODER_TRUEHD:
		pDspCodeInfo->dwCodeSize       = sizeof(truehd_code);		//K_DSPKRNL_CODE_SIZE_TRUEHD;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_TRUEHD;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_TRUEHD;  		
        break;
    case ID_DECODER_DTSLBR:
		pDspCodeInfo->dwCodeSize       = sizeof(dtslbr_code);		//K_DSPKRNL_CODE_SIZE_DTSLBR;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_DTSLBR;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_DTSLBR;  		
        break;
    case ID_DECODER_DTSHD:
		pDspCodeInfo->dwCodeSize       = sizeof(dtshd_code);		//K_DSPKRNL_CODE_SIZE_DTSHD;   
		pDspCodeInfo->dwRomtableSize   = sizeof(dtshd_romtable);	//K_DSPKRNL_ROMTABLE_SIZE_DTSHD;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_DTSHD;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_DTSHD;  		
        break;
    case ID_DECODER_MPEG:
		pDspCodeInfo->dwCodeSize       = sizeof(mpeg_code);			//K_DSPKRNL_CODE_SIZE_MPEG;   
		pDspCodeInfo->dwRomtableSize   = sizeof(mpeg_romtable);		//K_DSPKRNL_ROMTABLE_SIZE_MPEG;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_MPEG;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_MPEG;  		
        break;
    case ID_DECODER_AAC:
		pDspCodeInfo->dwCodeSize       = sizeof(aac_code);			//K_DSPKRNL_CODE_SIZE_AAC;   
		pDspCodeInfo->dwRomtableSize   = sizeof(aac_romtable);		//K_DSPKRNL_ROMTABLE_SIZE_AAC;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_AAC;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_AAC;  		
        break;
    case ID_DECODER_WMA:
		pDspCodeInfo->dwCodeSize       = sizeof(wma_code);			//K_DSPKRNL_CODE_SIZE_WMA;   
		pDspCodeInfo->dwRomtableSize   = sizeof(wma_romtable);		//K_DSPKRNL_ROMTABLE_SIZE_WMA;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_WMA;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_WMA;  		
        break;
    case ID_DECODER_RA:
		pDspCodeInfo->dwCodeSize       = sizeof(ra_code);			//K_DSPKRNL_CODE_SIZE_RA;   
		pDspCodeInfo->dwRomtableSize   = sizeof(ra_romtable);		//K_DSPKRNL_ROMTABLE_SIZE_RA;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_RA;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_RA;  		
        break;
    case ID_DECODER_OGG:
		pDspCodeInfo->dwCodeSize       = sizeof(ogg_code);			//K_DSPKRNL_CODE_SIZE_OGG;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_OGG;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_OGG;  		
        break;
    case ID_DECODER_EMU:
		pDspCodeInfo->dwCodeSize       = sizeof(emu_code);			//K_DSPKRNL_CODE_SIZE_EMU;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_EMU;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_EMU;  		
        break;
    case ID_DECODER_WMAPRO:
		pDspCodeInfo->dwCodeSize       = sizeof(wmapro_code);		//K_DSPKRNL_CODE_SIZE_WMAPRO;   
		pDspCodeInfo->dwRomtableSize   = sizeof(wmapro_romtable);	//K_DSPKRNL_ROMTABLE_SIZE_WMAPRO;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_WMAPRO;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_WMAPRO;  		
        break;		
    case ID_DECODER_FLAC:
		pDspCodeInfo->dwCodeSize       = sizeof(flac_code);			//K_DSPKRNL_CODE_SIZE_FLAC;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_FLAC;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_FLAC;  		
		break;		
    case ID_DECODER_AMR:
		pDspCodeInfo->dwCodeSize       = sizeof(amr_code);			//K_DSPKRNL_CODE_SIZE_AMR;   
		pDspCodeInfo->dwRomtableSize   = sizeof(amr_romtable);		//K_DSPKRNL_ROMTABLE_SIZE_AMR;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_AMR;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_AMR;  		
		break;		
	case ID_DECODER_MIDI:
		pDspCodeInfo->dwCodeSize       = sizeof(midi_code);			//K_DSPKRNL_CODE_SIZE_MIDI;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_MIDI;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_MIDI;  		
		break;				
    case ID_ENCODER_MPEG:
		pDspCodeInfo->dwCodeSize       = sizeof(mpeg_enc_code);		//K_DSPKRNL_CODE_SIZE_MPEG_ENC;   
		pDspCodeInfo->dwRomtableSize   = sizeof(mpeg_enc_romtable);	//K_DSPKRNL_ROMTABLE_SIZE_MPEG_ENC;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_MPEG_ENC;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_MPEG_ENC;  		
        break;		
    case ID_ENCODER_AC3:
		pDspCodeInfo->dwCodeSize       = sizeof(ac3_enc_code);		//K_DSPKRNL_CODE_SIZE_AC3_ENC;   
		pDspCodeInfo->dwRomtableSize   = sizeof(ac3_enc_romtable);	//K_DSPKRNL_ROMTABLE_SIZE_AC3_ENC;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_AC3_ENC;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_AC3_ENC;  		
        break;			
    case ID_ENCODER_DTS:
		pDspCodeInfo->dwCodeSize       = sizeof(dts_enc_code);		//K_DSPKRNL_CODE_SIZE_DTS_ENC;   
		pDspCodeInfo->dwRomtableSize   = sizeof(dts_enc_romtable);	//K_DSPKRNL_ROMTABLE_SIZE_DTS_ENC;   
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_DTS_ENC;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_DTS_ENC;  		
        break;		
    case ID_DECODER_APE:		
		pDspCodeInfo->dwCodeSize       = sizeof(ape_code);			
		pDspCodeInfo->dwRomtableSize   = sizeof(ape_romtable);		
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_APE;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_APE;  				
		break;
	case ID_DECODER_AVS:		
		pDspCodeInfo->dwCodeSize       = sizeof(avs_code);			
		pDspCodeInfo->dwRomtableSize   = sizeof(avs_romtable);		
		pDspCodeInfo->bsbufMinSize     = K_BS_BUF_SIZE_AVS;   
		pDspCodeInfo->dramMinSize      = K_DRAM_BUF_SIZE_AVS;
		break;
	case ID_SWAPP:
		pDspCodeInfo->dwCodeSize       = sizeof(swapp_code);		//K_DSPKRNL_CODE_SIZE_SWAPP;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = 0;   
		pDspCodeInfo->dramMinSize      = 0;  		
        break;						

    default:
		pDspCodeInfo->dwCodeSize       = 0;   
		pDspCodeInfo->dwRomtableSize   = 0;   
		pDspCodeInfo->bsbufMinSize     = 0;   
		pDspCodeInfo->dramMinSize      = 0;  		
		break;
    }    

	pDspCodeInfo->header  	= 0x6e6f;		// 'n''o'
	pDspCodeInfo->dwCodeID  = 0x00000000;   // unknown
	if(pDspCodeInfo->dwRomtableSize)
	{
		pDspCodeInfo->Attribute = 1<<K_CODEINFO_HAS_ROMTABLE_BIT;   				
	}
#ifndef DISABLE_DSPCODE_SCRAMBLE
	pDspCodeInfo->Attribute    |= 1<<K_CODEINFO_ENA_SCRAMBLE_BIT;   
    pDspCodeInfo->dwCodeKey	    = 0xabcd1234;   		// default codekey
#endif
}

// ---------------------------------------------------------------------
// dsp memory alocation ( use DRAM_B)
//     DRAM_A : 0x80000000 (0xA0000000) 32bit
//     DRAM_B : 0x90000000 (0xB0000000) 16bit
// ---------------------------------------------------------------------
// dsp dram resources 
// 1. fixed allocation 
// 2. assume continuous code and data memroy physically
// ---------------------------------------------------------------------
//     code memory |  bank0  | K_DSP_CODE_SIZE_BANK0  
//                 |  bank1  | K_DSP_CODE_MAX_SIZE_DEC
//                 |  bank2  | K_DSP_CODE_MAX_SIZE_DEC
//                 |  bank3  | K_DSP_CODE_MAX_SIZE_ENC 
//                 |  bank4  | K_DSP_CODE_SIZE_SWAPP
// ---------------------------------------------------------------------
//     data memory |  risc param | K_DRAM_SIZE_RISC_PARAMS0 (256 bytes)
//                 |  dsp param  | K_DRAM_SIZE_DSP_PARAMS0 (256 bytes)
// ---------------------------------------------------------------------

static void DSP_SetCodeSize(void)
{
	int i;
	DspCode* pDspCode = NULL;	

	for(i=0; i<MAX_CODEBANK_ID; i++)
	{
		pDspCode = gCodebankPtrList[i];
		if (NULL == pDspCode) continue;

#ifdef DEBUG_DUMP_DSP_CODEINFO		
		DSP_PRINT("[%02x]attribute       :0x%04x\n",i,pDspCode->CodeInfo.Attribute);
		DSP_PRINT("[%02x]dwCodeID        :0x%08x\n",i,pDspCode->CodeInfo.dwCodeID);
		DSP_PRINT("[%02x]dwCodeSize      :%d bytes\n",i,pDspCode->CodeInfo.dwCodeSize);
		DSP_PRINT("[%02x]dwRomtableSize  :%d bytes\n",i,pDspCode->CodeInfo.dwRomtableSize);				
		DSP_PRINT("[%02x]bsbufMinSize    :%d kbytes\n",i,pDspCode->CodeInfo.bsbufMinSize);
		DSP_PRINT("[%02x]dramMinSize     :%d kbytes\n",i,pDspCode->CodeInfo.dramMinSize);
		DSP_PRINT("[%02x]dwCodeChksum    :0x%08x\n",i,pDspCode->CodeInfo.dwCodeChksum);
		DSP_PRINT("[%02x]dwRomtableChksum:0x%08x\n",i,pDspCode->CodeInfo.dwRomtableChksum);
		DSP_PRINT("[%02x]dwCodeKey       :0x%08x\n",i,pDspCode->CodeInfo.dwCodeKey);
#endif	//DEBUG_DUMP_DSP_CODEINFO

		switch (i)
	    {
	    case ID_MAIN:
			gDspCodeSize.mainCodeSize = ALIGNSIZE(pDspCode->CodeInfo.dwCodeSize  , 8);
			gDspCodeSize.mainBsSize   = pDspCode->CodeInfo.bsbufMinSize;			
			gDspCodeSize.mainDramSize = pDspCode->CodeInfo.dramMinSize;									
    	    break;
	    case ID_DECODER_PCM:
	    case ID_DECODER_DDP:
	    case ID_DECODER_TRUEHD:
	    case ID_DECODER_DTSLBR:
	    case ID_DECODER_DTSHD:
	    case ID_DECODER_MPEG:
	    case ID_DECODER_AAC:
    	case ID_DECODER_WMA:
	    case ID_DECODER_RA:
	    case ID_DECODER_OGG:
	    case ID_DECODER_EMU:
	    case ID_DECODER_WMAPRO:
	    case ID_DECODER_FLAC:
	    case ID_DECODER_AMR:
		case ID_DECODER_MIDI:
		case ID_DECODER_APE:			
		case ID_DECODER_AVS:
			gDspCodeSize.decCodeSize = MAX(gDspCodeSize.decCodeSize,ALIGNSIZE(pDspCode->CodeInfo.dwCodeSize,8));
			gDspCodeSize.decBsSize   = MAX(gDspCodeSize.decBsSize, pDspCode->CodeInfo.bsbufMinSize); 
			gDspCodeSize.decDramSize = MAX(gDspCodeSize.decDramSize,pDspCode->CodeInfo.dramMinSize); 			
			break;
	    case ID_ENCODER_MPEG:		
	    case ID_ENCODER_AC3:
	    case ID_ENCODER_DTS:	
			gDspCodeSize.encCodeSize = MAX(gDspCodeSize.encCodeSize,ALIGNSIZE(pDspCode->CodeInfo.dwCodeSize,8));
			gDspCodeSize.encBsSize   = MAX(gDspCodeSize.encBsSize,pDspCode->CodeInfo.bsbufMinSize); 			
			gDspCodeSize.encDramSize = MAX(gDspCodeSize.encDramSize,pDspCode->CodeInfo.dramMinSize); 						
	        break;		
	
		case ID_SWAPP:
			gDspCodeSize.swappCodeSize = ALIGNSIZE(pDspCode->CodeInfo.dwCodeSize  , 8);
			gDspCodeSize.swappDramSize = pDspCode->CodeInfo.dramMinSize;				
	        break;						

	   // default:
			
	    }    		
	}

#ifdef DEBUG_DUMP_DSP_SIZE
	DSP_DumpDspSizeInfo();
#endif
}

/******************************************************************************
   Public Function Implementations
 *****************************************************************************/

DspCode* DSP_GetDspCodePtr(UINT8 codecID)
{
	if (codecID >= MAX_CODEBANK_ID) return NULL;

	return (gCodebankPtrList[codecID]);	
}

DspCodeSize* DSP_GetCodeSizePtr(void)
{
	return (&gDspCodeSize);
}


void DSP_DumpDspSizeInfo(void)
{
//typedef struct _DspCodeSize{
//	UINT32 mainCodeSize;
//	UINT32 decCodeSize;
//	UINT32 encCodeSize;
//	UINT32 swappCodeSize;
//	UINT16 mainBsSize;	
//	UINT16 decBsSize;
//	UINT16 encBsSize;		
//	UINT16 mainDramSize;		
//	UINT16 decDramSize;
//	UINT16 encDramSize;
//	UINT16 swappDramSize;		// included in mainDramSize
//}DspCodeSize; 	
	DSP_PRINT("\nDSP_DumpDspSizeInfo::mainCodeSize :%6d   bytes\n",gDspCodeSize.mainCodeSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::mainBsSize   :%6d   kbytes\n",gDspCodeSize.mainBsSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::mainDramSize :%6d   kbytes\n",gDspCodeSize.mainDramSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::swappCodeSize:%6d   bytes\n",gDspCodeSize.swappCodeSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::swappDramSize:%6d   kbytes\n",gDspCodeSize.swappDramSize);	
	DSP_PRINT("DSP_DumpDspSizeInfo::decCodeSize  :%6d   bytes\n",gDspCodeSize.decCodeSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::decBsSize    :%6d   kbytes\n",gDspCodeSize.decBsSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::decDramSize  :%6d   kbytes\n",gDspCodeSize.decDramSize);	
	DSP_PRINT("DSP_DumpDspSizeInfo::encCodeSize  :%6d   bytes\n",gDspCodeSize.encCodeSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::encBsSize    :%6d   kbytes\n",gDspCodeSize.encBsSize);
	DSP_PRINT("DSP_DumpDspSizeInfo::encDramSize  :%6d   kbytes\n",gDspCodeSize.encDramSize);
	
}

void DSP_CheckCodec(UINT8 codecID, UINT8 codeBankID)
{
	UINT32 codeChecksum = 0;
	UINT32 romChecksum = 0;
	DspCode* pDspCode = NULL;
	UINT32 codeBankBase;
	UINT32 dataBankBase;


	pDspCode = DSP_GetDspCodePtr(codecID);
	if (pDspCode == NULL)
	{
		diag_printf("DSP_CheckCodec::CodecID:0x%x is NULL!\n",codecID);  									
		return;
	}
	
    switch (codeBankID)
    {
    case CODE_BANK0:     
		codeBankBase = DSP_GetInfo(INFO_CODE_BANK0_BASE); 		
		dataBankBase = DSP_GetInfo(INFO_MAIN_STATIC_BASE); 		
        break;
    case CODE_BANK1:
		codeBankBase = DSP_GetInfo(INFO_CODE_BANK1_BASE); 		
		dataBankBase = DSP_GetInfo(INFO_DEC1_STATIC_BASE); 		
        break;
    case CODE_BANK2:    
		codeBankBase = DSP_GetInfo(INFO_CODE_BANK2_BASE); 		
		dataBankBase = DSP_GetInfo(INFO_DEC2_STATIC_BASE); 		
        break;      
    case CODE_BANK3:              
		codeBankBase = DSP_GetInfo(INFO_CODE_BANK3_BASE); 		
		dataBankBase = DSP_GetInfo(INFO_ENC_STATIC_BASE); 		
        break;     
    case CODE_BANK4:              
		codeBankBase = DSP_GetInfo(INFO_CODE_BANK4_BASE); 		
		dataBankBase = DSP_GetInfo(INFO_SWAPP_STATIC_BASE); 		
        break;    		
	default:
		diag_printf("DSP_CheckCodec::unsupported codeBankID:%d!\n",codeBankID);  		
		return;
    }
	

	codeChecksum = DSP_GenChecksum((const UINT8*)codeBankBase, pDspCode->CodeInfo.dwCodeSize);
	if (codeChecksum == pDspCode->CodeInfo.dwCodeChksum) 
	{
		diag_printf("DSP_CheckCodebank::codecID:0x%02x:code checksum matched!\n",codecID);
	}
	else
	{
		diag_printf("DSP_CheckCodebank::codecID:0x%02x:code checksum failed!\n",codecID);	
	}
	if(HAS_ROMTABLE(pDspCode->CodeInfo.Attribute))
	{
		if (dataBankBase == 0) 
		{
			diag_printf("\nDSP_CheckCodec::dataBankBase is not initialed yet!\n"); 
			return;
		}
			
		romChecksum = DSP_GenChecksum((const UINT8*)dataBankBase, pDspCode->CodeInfo.dwRomtableSize);				
		if (romChecksum == pDspCode->CodeInfo.dwRomtableChksum) 
		{
			diag_printf("DSP_CheckCodec::codecID:0x%02x:romtable checksum matched!\n",codecID);  											
		}
		else
		{
			diag_printf("DSP_CheckCodec::codecID:0x%02x:romtable checksum failed!\n",codecID);  											
		}
	}
}

void DSP_CheckCodebank(void)
{

	UINT16 runMode = DSP_GetRiscParam0(RISC_MAIN_MODE);
	UINT16 codecID = 0;

	DSP_CheckCodec(ID_MAIN,CODE_BANK0);
	DSP_CheckCodec(ID_SWAPP,CODE_BANK4);	
	
	if(IS_DEC1_ENABLE(runMode))
	{
		codecID = DSP_GetRiscParam0(RISC_DEC1_ID);
		if ((codecID > ID_MAIN) & (codecID <=ID_MAX_DECID)) DSP_CheckCodec(codecID,CODE_BANK1);
	}
	if(IS_DEC2_ENABLE(runMode))
	{
		codecID = DSP_GetRiscParam0(RISC_DEC2_ID);
		if ((codecID > ID_MAIN) & (codecID <=ID_MAX_DECID)) DSP_CheckCodec(codecID,CODE_BANK2);		
	}
	if(IS_ENC_ENABLE(runMode))
	{
		codecID = DSP_GetRiscParam0(RISC_ENC_ID);
		if ((ID_ENCODER_MPEG == codecID) |
(ID_ENCODER_AC3 == codecID)| (ID_ENCODER_DTS == codecID))		
		DSP_CheckCodec(codecID,CODE_BANK3);
	}	
	
}

void DSP_DumpDspCodeInfo(UINT8 codeID)
{
	DspCode* pDspCode = DSP_GetDspCodePtr(codeID);
	if (NULL == pDspCode)
	{
		DSP_PRINT("DSP_DumpDspCodeInfo::e:codeID:0x%x not exist\n",codeID);					
		return;
	}
	DspCodeInfo* pDspCodeInfo = &pDspCode->CodeInfo;
	if (NULL == pDspCodeInfo)
	{
		DSP_PRINT("DSP_DumpDspCodeInfo::e:pDspCodeInfo not exist\n");					
		return;
	}	

	DSP_PRINT("\nDSP_DumpDspCodeInfo::header          :%08x\n",pDspCodeInfo->header);
	DSP_PRINT("DSP_DumpDspCodeInfo::Attribute       :0x%08x\n",pDspCodeInfo->Attribute);
	DSP_PRINT("DSP_DumpDspCodeInfo::dwCodeID        :0x%08x\n",(UINT32)pDspCodeInfo->dwCodeID);
	DSP_PRINT("DSP_DumpDspCodeInfo::dwCodeSize      :%8d  bytes\n",(UINT32)pDspCodeInfo->dwCodeSize);
	DSP_PRINT("DSP_DumpDspCodeInfo::dwRomtableSize  :%8d  bytes\n",(UINT32)pDspCodeInfo->dwRomtableSize);
	DSP_PRINT("DSP_DumpDspCodeInfo::bsbufMinSize    :%8d  kbytes\n",(UINT16)pDspCodeInfo->bsbufMinSize);
	DSP_PRINT("DSP_DumpDspCodeInfo::dramMinSize     :%8d  kbytes\n",(UINT16)pDspCodeInfo->dramMinSize);
	DSP_PRINT("DSP_DumpDspCodeInfo::dwCodeChksum:   :0x%08x\n",(UINT32)pDspCodeInfo->dwCodeChksum);
	DSP_PRINT("DSP_DumpDspCodeInfo::dwRomtableChksum:0x%08x\n",(UINT32)pDspCodeInfo->dwRomtableChksum);
	DSP_PRINT("DSP_DumpDspCodeInfo::dwCodeKey       :0x%08x\n",(UINT32)pDspCodeInfo->dwCodeKey);
	
}

DspCodeSize* DSP_InitCodeBank(void)
{		
	gCodebank0x00.pRawHex = bank0_code;   	// 0x00: mainprogram    
	gCodebank0x01.pRawHex = pcm_code;  	  	// 0x01: PCM
	gCodebank0x02.pRawHex = ddp_code;     	// 0x02: Dolby DD+/AC3
	gCodebank0x03.pRawHex = truehd_code;  	// 0x03: Dolby TrueHD
	gCodebank0x04.pRawHex = dtslbr_code;  	// 0x04: DTS Lbr
	gCodebank0x05.pRawHex = dtshd_code;   	// 0x05: DTS HD/core/ma
	gCodebank0x06.pRawHex = mpeg_code;    	// 0x06: Mpeg L1/L2/L3
	gCodebank0x07.pRawHex = aac_code;     	// 0x07: AAC/HEAAC
	gCodebank0x08.pRawHex = wma_code;     	// 0x08: WMA
	gCodebank0x09.pRawHex = ra_code;      	// 0x09: Real Audio
	gCodebank0x0a.pRawHex = ogg_code;     	// 0x0a: Ogg Vorbis
	gCodebank0x0b.pRawHex = emu_code;     	// 0x0b: Emulation (sine/pink/random)
	gCodebank0x0c.pRawHex = wmapro_code;  	// 0x0c: WMA Pro
	gCodebank0x0d.pRawHex = flac_code;    	// 0x0d: FLAC (Lossless)
	gCodebank0x0e.pRawHex = amr_code;     	// 0x0e: AMR (NB/WB/WB+)
	gCodebank0x0f.pRawHex = midi_code;    	// 0x0f: MIDI
	gCodebank0x10.pRawHex = mpeg_enc_code;	// 0x10: Mpeg L3 Encoder
	gCodebank0x11.pRawHex = ac3_enc_code; 	// 0x11: AC3 Encoder
	gCodebank0x12.pRawHex = dts_enc_code; 	// 0x12: DTS Encoder
	gCodebank0x13.pRawHex = ape_code;      	// 0x13: APE decoder
	gCodebank0x14.pRawHex = NULL;	      	// 0x14: Reserved
	gCodebank0x15.pRawHex = avs_code;		// 0x15: AVS decoder
	gCodebank0x16.pRawHex = NULL;		  	// 0x16: Reserved
	gCodebank0x17.pRawHex = NULL;		  	// 0x17: Reserved
	gCodebank0x18.pRawHex = NULL;		  	// 0x18: Reserved
	gCodebank0x19.pRawHex = NULL;		  	// 0x19: Reserved
	gCodebank0x1a.pRawHex = NULL;		  	// 0x1a: Reserved
	gCodebank0x1b.pRawHex = NULL;			// 0x1b: Reserved
	gCodebank0x1c.pRawHex = NULL;			// 0x1c: Reserved
	gCodebank0x1d.pRawHex = NULL;			// 0x1d: Reserved
	gCodebank0x1e.pRawHex = NULL;			// 0x1e: Reserved
	gCodebank0x1f.pRawHex = swapp_code;	  	// 0x1f: Software APP (GM5/PL2x/Neo6)

	//
	// note: maybe romtable could be merged into code inc file
	// 
	gCodebank0x00.pRomtable = NULL;             	// 0x00: mainprogram                                    	
	gCodebank0x01.pRomtable = NULL;              	// 0x01: PCM                                            
	gCodebank0x02.pRomtable = ddp_romtable;        	// 0x02: Dolby DD+/AC3                                  
	gCodebank0x03.pRomtable = NULL;             	// 0x03: Dolby TrueHD                                   
	gCodebank0x04.pRomtable = NULL;             	// 0x04: DTS Lbr                                        
	gCodebank0x05.pRomtable = dtshd_romtable;   	// 0x05: DTS HD/core/ma                                 
	gCodebank0x06.pRomtable = mpeg_romtable;    	// 0x06: Mpeg L1/L2/L3                                  
	gCodebank0x07.pRomtable = aac_romtable;     	// 0x07: AAC/HEAAC                                      
	gCodebank0x08.pRomtable = wma_romtable;     	// 0x08: WMA                                            
	gCodebank0x09.pRomtable = ra_romtable;      	// 0x09: Real Audio                                     
	gCodebank0x0a.pRomtable = ogg_romtable;      	// 0x0a: Ogg Vorbis                                     
	gCodebank0x0b.pRomtable = NULL;             	// 0x0b: Emulation (sine/pink/random)                   
	gCodebank0x0c.pRomtable = wmapro_romtable;  	// 0x0c: WMA Pro                                        
	gCodebank0x0d.pRomtable = NULL;             	// 0x0d: FLAC (Lossless)                                
	gCodebank0x0e.pRomtable = amr_romtable;     	// 0x0e: AMR (NB/WB/WB+)                                
	gCodebank0x0f.pRomtable = NULL;    				// 0x0f: MIDI                                           
	gCodebank0x10.pRomtable = mpeg_enc_romtable;	// 0x10: Mpeg L3 Encoder                                
	gCodebank0x11.pRomtable = ac3_enc_romtable; 	// 0x11: AC3 Encoder                                    
	gCodebank0x12.pRomtable = dts_enc_romtable; 	// 0x12: DTS Encoder                                    
	gCodebank0x13.pRomtable = ape_romtable;         // 0x13: APE decoder
	gCodebank0x14.pRomtable = NULL;	            	// 0x14: Reserved                                       
	gCodebank0x15.pRomtable = avs_romtable;		    // 0x15: AVS decoder                                   
	gCodebank0x16.pRomtable = NULL;		        	// 0x16: Reserved                                       
	gCodebank0x17.pRomtable = NULL;		        	// 0x17: Reserved                                       
	gCodebank0x18.pRomtable = NULL;		        	// 0x18: Reserved                                       
	gCodebank0x19.pRomtable = NULL;		        	// 0x19: Reserved                                       
	gCodebank0x1a.pRomtable = NULL;		        	// 0x1a: Reserved                                       
	gCodebank0x1b.pRomtable = NULL;			    	// 0x1b: Reserved                                       
	gCodebank0x1c.pRomtable = NULL;			    	// 0x1c: Reserved                                       
	gCodebank0x1d.pRomtable = NULL;			    	// 0x1d: Reserved                                       
	gCodebank0x1e.pRomtable = NULL;					// 0x1e: Reserved                                       
	gCodebank0x1f.pRomtable = NULL;	            	// 0x1f: Software APP (GM5/PL2x/Neo6)  

	
	//
	// initial supported codecs to gCodebankPtrList
	//
	gCodebankPtrList[0x00] = &gCodebank0x00;	// 0x00: mainprogram                  
	gCodebankPtrList[0x01] = &gCodebank0x01;    // 0x01: PCM                          
	gCodebankPtrList[0x02] = &gCodebank0x02;    // 0x02: Dolby DD+/AC3                
	gCodebankPtrList[0x03] = &gCodebank0x03;    // 0x03: Dolby TrueHD                 
	gCodebankPtrList[0x04] = &gCodebank0x04;    // 0x04: DTS Lbr                      
	gCodebankPtrList[0x05] = &gCodebank0x05;    // 0x05: DTS HD/core/ma               
	gCodebankPtrList[0x06] = &gCodebank0x06;    // 0x06: Mpeg L1/L2/L3                
	gCodebankPtrList[0x07] = &gCodebank0x07;    // 0x07: AAC/HEAAC                    
	gCodebankPtrList[0x08] = &gCodebank0x08;    // 0x08: WMA                          
	gCodebankPtrList[0x09] = &gCodebank0x09;    // 0x09: Real Audio                   
	gCodebankPtrList[0x0a] = &gCodebank0x0a;    // 0x0a: Ogg Vorbis                   
	gCodebankPtrList[0x0b] = &gCodebank0x0b;    // 0x0b: Emulation (sine/pink/random) 
	gCodebankPtrList[0x0c] = &gCodebank0x0c;    // 0x0c: WMA Pro                      
	gCodebankPtrList[0x0d] = &gCodebank0x0d;    // 0x0d: FLAC (Lossless)              
	gCodebankPtrList[0x0e] = &gCodebank0x0e;    // 0x0e: AMR (NB/WB/WB+)              
#ifdef SUPPORT_CODEC_MIDI		
	gCodebankPtrList[0x0f] = &gCodebank0x0f;    // 0x0f: MIDI                         
#else
	gCodebankPtrList[0x0f] = NULL;    // 0x0f: MIDI                         
#endif	
	gCodebankPtrList[0x10] = &gCodebank0x10;    // 0x10: Mpeg L3 Encoder              
	gCodebankPtrList[0x11] = &gCodebank0x11;    // 0x11: AC3 Encoder                  
	gCodebankPtrList[0x12] = &gCodebank0x12;    // 0x12: DTS Encoder                  
	gCodebankPtrList[0x13] = &gCodebank0x13;	// 0x13: APE
	gCodebankPtrList[0x14] = NULL;//&gCodebank0x14;    // 0x14: Reserved                     
	gCodebankPtrList[0x15] = &gCodebank0x15;    // 0x15: AVS                 
	gCodebankPtrList[0x16] = NULL;//&gCodebank0x16;    // 0x16: Reserved                     
	gCodebankPtrList[0x17] = NULL;//&gCodebank0x17;    // 0x17: Reserved                     
	gCodebankPtrList[0x18] = NULL;//&gCodebank0x18;    // 0x18: Reserved                     
	gCodebankPtrList[0x19] = NULL;//&gCodebank0x19;    // 0x19: Reserved                     
	gCodebankPtrList[0x1a] = NULL;//&gCodebank0x1a;    // 0x1a: Reserved                     
	gCodebankPtrList[0x1b] = NULL;//&gCodebank0x1b;    // 0x1b: Reserved                     
	gCodebankPtrList[0x1c] = NULL;//&gCodebank0x1c;    // 0x1c: Reserved                     
	gCodebankPtrList[0x1d] = NULL;//&gCodebank0x1d;    // 0x1d: Reserved                     
	gCodebankPtrList[0x1e] = NULL;//&gCodebank0x1e;    // 0x1e: Reserved                     
	gCodebankPtrList[0x1f] = &gCodebank0x1f;    // 0x1f: Software APP (GM5/PL2x/Neo6) 


	//
	// Check if has header
	// 
	int i;	
	DspCode* pDspCode = NULL;
	const UINT8* pRawDspCodeInfoV1 = NULL;
	for (i=0; i<MAX_CODEBANK_ID; i++)
	{
		pDspCode = gCodebankPtrList[i];
		if (NULL == pDspCode) continue;		
		if (NULL == pDspCode->pRawHex) continue;

		// header is: 'i''f'
		if (('i' == pDspCode->pRawHex[0])&&('f' == pDspCode->pRawHex[1]))
		{
			// header is found, continue with attribute
			pRawDspCodeInfoV1 = pDspCode->pRawHex;
			pRawDspCodeInfoV1 = DSP_ReadWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.header);
			pRawDspCodeInfoV1 = DSP_ReadWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.Attribute);
			pRawDspCodeInfoV1 = DSP_ReadDWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dwCodeID);
			pRawDspCodeInfoV1 = DSP_ReadDWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dwCodeSize);
			pRawDspCodeInfoV1 = DSP_ReadDWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dwRomtableSize);
			pRawDspCodeInfoV1 = DSP_ReadWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.bsbufMinSize);
			pRawDspCodeInfoV1 = DSP_ReadWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dramMinSize);
			pRawDspCodeInfoV1 = DSP_ReadDWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dwCodeChksum);
			pRawDspCodeInfoV1 = DSP_ReadDWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dwRomtableChksum);
			pRawDspCodeInfoV1 = DSP_ReadDWord(pRawDspCodeInfoV1,&pDspCode->CodeInfo.dwCodeKey);				

			pDspCode->pCode = &pDspCode->pRawHex[K_DSP_CODE_INFO_V1_SIZE];								
		}
		else
		{
			// if no DspCodeInfoV1 header 						
			pDspCode->pCode = pDspCode->pRawHex;
			DSP_SetDSPCodeInfo(i,&pDspCode->CodeInfo);	
			
			pDspCode->CodeInfo.dwCodeChksum = DSP_GenChecksum(pDspCode->pCode, pDspCode->CodeInfo.dwCodeSize);  
			pDspCode->CodeInfo.dwRomtableChksum = DSP_GenChecksum(pDspCode->pRomtable,pDspCode->CodeInfo.dwRomtableSize);   		
		}
	}

	DSP_SetCodeSize();


	return (&gDspCodeSize);
}
