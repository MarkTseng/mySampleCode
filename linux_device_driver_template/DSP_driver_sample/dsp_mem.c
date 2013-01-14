/******************************************************************************
 * Filename    : dsp_mem.c
 * 
 * Description : dsp memory map file
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
#include "include/setting.h"
//#include "stypes.h"

#include "include/aud_global.h"
#include "include/dsp_api.h"
#include "include/auddrv.h"
#include "include/aud_static.h"
///#include "regmap_8600.h"
#include "include/dsp_mem.h"
#include "include/memmap_dsp.h"
#include <asm/mach-spv9200/regmap_9200.h>

/******************************************************************************
    Local Defines
 *****************************************************************************/
#define DEBUG_DUMP_DRAM_PARAMS              // Dump dram parameters
#define GetValidIndex(a,max)                (a) > (max) ? 0:(a);


/******************************************************************************
    Global variables
 ******************************************************************************/
static UINT8 *gCodeBank0;
static UINT8 *gCodeBank1;
static UINT8 *gCodeBank2;
static UINT8 *gCodeBank3;
static UINT8 *gCodeBank4;
//static UINT8 *gCodeBank5;
//static UINT8 *gCodeBank6;
//static UINT8 *gCodeBank7;
static UINT8 *gDataBank;

static UINT16* gRiscParams0;
static UINT16* gDspParams0;
static UINT16* gRiscParams1;
static UINT16* gDspParams1;

static BsBufInfo gBsBufInfoDec1;
static BsBufInfo gBsBufInfoDec2;
static BsBufInfo gBsBufInfoEnc;

const UINT16 gCodecBsBufSizeTbl[] = { 0,                        // ID = 0x0, reserved for error ID
	K_BS_BUF_SIZE_PCM,        // ID = 0x1 (ID_DECODER_PCM)        
	K_BS_BUF_SIZE_DDP,        // ID = 0x2 (ID_DECODER_DDP)
	K_BS_BUF_SIZE_TRUEHD,     // ID = 0x3 (ID_DECODER_TRUEHD)
	K_BS_BUF_SIZE_DTSLBR,     // ID = 0x4 (ID_DECODER_DTSLBR)                                               
	K_BS_BUF_SIZE_DTSHD,      // ID = 0x5 (ID_DECODER_DTSHD)                     
	K_BS_BUF_SIZE_MPEG,       // ID = 0x6 (ID_DECODER_MPEG)
	K_BS_BUF_SIZE_AAC,        // ID = 0x7 (ID_DECODER_AAC)
	K_BS_BUF_SIZE_WMA,        // ID = 0x8 (ID_DECODER_WMA)
	K_BS_BUF_SIZE_RA,         // ID = 0x9 (ID_DECODER_RA)
	K_BS_BUF_SIZE_OGG,        // ID = 0xA (ID_DECODER_OGG)
	K_BS_BUF_SIZE_EMU,        // ID = 0xB (ID_DECODER_EMU)
	K_BS_BUF_SIZE_WMAPRO,     // ID = 0xC (ID_DECODER_WMAPRO)
	K_BS_BUF_SIZE_FLAC,    	// ID = 0xD,(ID_DECODER_FLAC)
	K_BS_BUF_SIZE_AMR,        // ID = 0xE,(ID_DECODER_AMR)
	K_BS_BUF_SIZE_MIDI,       // ID = 0xF,(ID_DECODER_MIDI)
	K_BS_BUF_SIZE_MPEG_ENC,   // ID = 0x10 (ID_ENCODER_MPEG)
	K_BS_BUF_SIZE_AC3_ENC,    // ID = 0x11 (ID_ENCODER_AC3)       
	K_BS_BUF_SIZE_DTS_ENC,    // ID = 0x12 (ID_ENCODER_DTS)
	K_BS_BUF_SIZE_APE,        // ID = 0x13 (ID_DECODER_APE)
	0,                        // ID = 0x14 
	0,						// ID = 0x15 
	0,						// ID = 0x16 
	0,						// ID = 0x17 
	0,						// ID = 0x18 
	0,						// ID = 0x19 
	0,						// ID = 0x1a 
	0,						// ID = 0x1b 
	0,						// ID = 0x1c 
	0,						// ID = 0x1d 
	0,						// ID = 0x1e
	0,						// ID = 0x1f
};


// Linux porting 
//auddrv_param aud_param;
//auddrv_InitRiscParam aud_InitRiscParam;


/******************************************************************************
    Function Implementations
 *****************************************************************************/
static void DSP_hwDramMap(UINT32 dspDramBase)
{

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
//     data memory |  risc param0 | K_DRAM_SIZE_RISC_PARAMS0 (256 bytes)
//                 |  dsp param0  | K_DRAM_SIZE_DSP_PARAMS0 (256 bytes)
//                 |  risc param1 | K_DRAM_SIZE_RISC_PARAMS1 (192 bytes)
//                 |  dsp param1  | K_DRAM_SIZE_DSP_PARAMS1 (64 bytes)
// ---------------------------------------------------------------------
// 
#if 0
	if (IS_CACHED_ADDRESS(dspDramBase))
	{
		dspDramBase  |= 0x20000000;
	}
#endif

	DspCodeSize* pDspCodeSize = DSP_InitCodeBank();	

	// Code banks:
	// support main bank + swapp bank + 2 decoder banks + 1 encoder bank
	UINT32 totalCodeBankSize = pDspCodeSize->mainCodeSize + \
		                     2 * pDspCodeSize->decCodeSize + \
		                     1 * pDspCodeSize->encCodeSize + \
		                     pDspCodeSize->swappCodeSize;

    UINT32 codeDrambase = dspDramBase;
    UINT32 dataDrambase = dspDramBase + totalCodeBankSize;  	
	
    DSPAPI_PRINT("DSP_hwDramMap::dspDramBase:0x%x, codeDramBase:0x%x, dataDramBase:0x%x\n",dspDramBase,codeDrambase,dataDrambase);    	

    // code bank = CODE_BANK_NUM (4)
    gCodeBank0 = (UINT8*)codeDrambase;    
    gCodeBank1 = (UINT8*)(gCodeBank0 + pDspCodeSize->mainCodeSize);
    gCodeBank2 = (UINT8*)(gCodeBank1 + pDspCodeSize->decCodeSize);
    gCodeBank3 = (UINT8*)(gCodeBank2 + pDspCodeSize->decCodeSize);
    gCodeBank4 = (UINT8*)(gCodeBank3 + pDspCodeSize->encCodeSize);
    //gCodeBank5 = (UINT8*)(gCodeBank4 + pDspCodeSize->swappCodeSize);
    //gCodeBank6 = (UINT8*)(gCodeBank5 + pDspCodeSize->decCodeSize);
    //gCodeBank7 = (UINT8*)(gCodeBank6 + pDspCodeSize->decCodeSize);   

    DSPAPI_PRINT("DSP_hwDramMap::gCodeBank0:0x%x, size:0x%x\n",(UINT32)gCodeBank0,pDspCodeSize->mainCodeSize);
    DSPAPI_PRINT("DSP_hwDramMap::gCodeBank1:0x%x, size:0x%x\n",(UINT32)gCodeBank1,pDspCodeSize->decCodeSize);
    DSPAPI_PRINT("DSP_hwDramMap::gCodeBank2:0x%x, size:0x%x\n",(UINT32)gCodeBank2,pDspCodeSize->decCodeSize);
    DSPAPI_PRINT("DSP_hwDramMap::gCodeBank3:0x%x, size:0x%x\n",(UINT32)gCodeBank3,pDspCodeSize->encCodeSize); 
    DSPAPI_PRINT("DSP_hwDramMap::gCodeBank4:0x%x, size:0x%x\n",(UINT32)gCodeBank4,pDspCodeSize->swappCodeSize);    
    //DSPAPI_PRINT("DSP_hwDramMap::gCodeBank5:0x%x, size:0x%x\n",(UINT32)gCodeBank5, pDspCodeSize->decCodeSize); //reserved
    //DSPAPI_PRINT("DSP_hwDramMap::gCodeBank6:0x%x, size:0x%x\n",(UINT32)gCodeBank6, pDspCodeSize->decCodeSize); //reserved
    //DSPAPI_PRINT("DSP_hwDramMap::gCodeBank7:0x%x, size:0x%x\n",(UINT32)gCodeBank7, pDspCodeSize->decCodeSize); //reserved 

    gDataBank   = (UINT8*)dataDrambase;
    gRiscParams0 = (UINT16 *)gDataBank;
    gDspParams0  = (UINT16 *)(gDataBank + K_DRAM_SIZE_RISC_PARAMS0);
    gRiscParams1 = (UINT16 *)((UINT8*)gDspParams0 + K_DRAM_SIZE_DSP_PARAMS0);
    gDspParams1  = (UINT16 *)((UINT8*)gRiscParams1 + K_DRAM_SIZE_RISC_PARAMS1);
    
    DSPAPI_PRINT("DSP_hwDramMap::gDataBank:0x%x, RiscParams:0x%x, DspParams:0x%x\n",(UINT32)gDataBank,(UINT32)gRiscParams0,(UINT32)gDspParams0);  
    

	if (IS_DRAM_BANKA(dspDramBase))
	{
		// DRAM bank A
    // Set Codepage registers
    // Page0: default page, will always on, no need to enable it specificly
	    HWREG_W(dsp_inst_page[0], ((UINT32)gCodeBank0&0xFFFFFFF)>>8);
	    HWREG_W(dsp_inst_page[1], ((UINT32)gCodeBank1&0xFFFFFFF)>>8);
	    HWREG_W(dsp_inst_page[2], ((UINT32)gCodeBank2&0xFFFFFFF)>>8);
	    HWREG_W(dsp_inst_page[3], ((UINT32)gCodeBank3&0xFFFFFFF)>>8);
	    HWREG_W(dsp_inst_page[4], ((UINT32)gCodeBank4&0xFFFFFFF)>>8);
    	//HWREG_W(dsp_inst_page[5], ((UINT32)gCodeBank5&0xFFFFFFF)>>8);
    	//HWREG_W(dsp_inst_page[6], ((UINT32)gCodeBank6&0xFFFFFFF)>>8);
	    //HWREG_W(dsp_inst_page[7], ((UINT32)gCodeBank7&0xFFFFFFF)>>8);  

	    // Set datapage registers
	    HWREG_W(dsp_data_page, (UINT32)gDataBank&0xFFFFFFF);      

	    HWREG_W(aud_audhwya, (UINT32)gDataBank&0xFFFFFFF);
	    HWREG_W(app_apphwya, (UINT32)gDataBank&0xFFFFFFF);		
	
	}
	else
	{
		// DRAM bank B
    // Set Codepage registers
    // Page0: default page, will always on, no need to enable it specificly
	    HWREG_W(dsp_inst_page[0], ((((UINT32)gCodeBank0&0xFFFFFFF)|0x10000000)>>8));
	    HWREG_W(dsp_inst_page[1], ((((UINT32)gCodeBank1&0xFFFFFFF)|0x10000000)>>8));
	    HWREG_W(dsp_inst_page[2], ((((UINT32)gCodeBank2&0xFFFFFFF)|0x10000000)>>8));
	    HWREG_W(dsp_inst_page[3], ((((UINT32)gCodeBank3&0xFFFFFFF)|0x10000000)>>8));
	    HWREG_W(dsp_inst_page[4], ((((UINT32)gCodeBank4&0xFFFFFFF)|0x10000000)>>8));
    	//HWREG_W(dsp_inst_page[5], ((((UINT32)gCodeBank5&0xFFFFFFF)|0x10000000)>>8));
    	//HWREG_W(dsp_inst_page[6], ((((UINT32)gCodeBank6&0xFFFFFFF)|0x10000000)>>8));
	    //HWREG_W(dsp_inst_page[7], ((((UINT32)gCodeBank7&0xFFFFFFF)|0x10000000)>>8));
	    
	    // Set datapage registers
	    HWREG_W(dsp_data_page, ((UINT32)gDataBank&0xFFFFFFF)|0x10000000);      
	    HWREG_W(aud_audhwya, ((UINT32)gDataBank&0xFFFFFFF)|0x40000000);      
	    HWREG_W(app_apphwya, ((UINT32)gDataBank&0xFFFFFFF)|0x40000000);      
	}
	

    HWREG_W(dram_inst_page0_len, pDspCodeSize->mainCodeSize);
    HWREG_W(dram_inst_page1_len, pDspCodeSize->decCodeSize);
    HWREG_W(dram_inst_page2_len, pDspCodeSize->decCodeSize);    
    HWREG_W(dram_inst_page3_len, pDspCodeSize->encCodeSize);    
    HWREG_W(dram_inst_page4_len, pDspCodeSize->swappCodeSize);  
    //HWREG_W(dram_inst_page5_len, pDspCodeSize->decCodeSize);  
    //HWREG_W(dram_inst_page6_len, pDspCodeSize->decCodeSize);      
    //HWREG_W(dram_inst_page7_len, pDspCodeSize->decCodeSize);      
    
    HWREG_W(dram_data_page_len, DSP_DRAMSIZE - totalCodeBankSize);          
    

    DSPAPI_PRINT("HWREG_R(dsp_inst_page0)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[0]),(UINT32)HWREG_R(dsp_inst_page[0]), \
                            (UINT32)&HWREG_R(dram_inst_page0_len),(UINT32)HWREG_R(dram_inst_page0_len));
    DSPAPI_PRINT("HWREG_R(dsp_inst_page1)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[1]),(UINT32)HWREG_R(dsp_inst_page[1]), \
                            (UINT32)&HWREG_R(dram_inst_page1_len),(UINT32)HWREG_R(dram_inst_page1_len));
    DSPAPI_PRINT("HWREG_R(dsp_inst_page2)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[2]),(UINT32)HWREG_R(dsp_inst_page[2]), \
                            (UINT32)&HWREG_R(dram_inst_page2_len),(UINT32)HWREG_R(dram_inst_page2_len));
    DSPAPI_PRINT("HWREG_R(dsp_inst_page3)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[3]),(UINT32)HWREG_R(dsp_inst_page[3]), \
                            (UINT32)&HWREG_R(dram_inst_page3_len),(UINT32)HWREG_R(dram_inst_page3_len));

    DSPAPI_PRINT("HWREG_R(dsp_inst_page4)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[4]),(UINT32)HWREG_R(dsp_inst_page[4]), \
                            (UINT32)&HWREG_R(dram_inst_page4_len),(UINT32)HWREG_R(dram_inst_page4_len));
/*

    DSPAPI_PRINT("HWREG_R(dsp_inst_page5)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[5]),(UINT32)HWREG_R(dsp_inst_page[5]), \
                            (UINT32)&HWREG_R(dram_inst_page5_len),(UINT32)HWREG_R(dram_inst_page5_len));

    DSPAPI_PRINT("HWREG_R(dsp_inst_page6)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[6]),(UINT32)HWREG_R(dsp_inst_page[6]), \
                            (UINT32)&HWREG_R(dram_inst_page6_len),(UINT32)HWREG_R(dram_inst_page6_len));

    DSPAPI_PRINT("HWREG_R(dsp_inst_page7)::[0x%08x]0x%08x,len:[0x%08x]0x%08x\n", \
                            (UINT32)&HWREG_R(dsp_inst_page[7]),(UINT32)HWREG_R(dsp_inst_page[7]), \
                            (UINT32)&HWREG_R(dram_inst_page7_len),(UINT32)HWREG_R(dram_inst_page7_len));
*/

    DSPAPI_PRINT("HWREG_R(dsp_data_page) ::0x%08x,len:0x%x\n",HWREG_R(dsp_data_page),HWREG_R(dram_data_page_len));    
    DSPAPI_PRINT("HWREG_R(aud_audhwya) ::0x%08x \n",HWREG_R(aud_audhwya));
    DSPAPI_PRINT("HWREG_R(app_apphwya) ::0x%08x \n",HWREG_R(app_apphwya));
    DSPAPI_PRINT("HWREG_R(rdif_ctrl):0x%08x:0x%08x\n",(UINT32)&HWREG_R(rdif_ctrl),(UINT32)HWREG_R(rdif_ctrl));            
}


static void DSP_DramDataMap(void)
{

//             | Usage                    | Dsp parameters         | Unit
// -----------------------------------------------------------------------------------
//             | Risc parameter0          | None                   | None
//             | Dsp parameter0           | None                   | None
//             | Risc parameter1          | None                   | None
//             | Dsp parameter1           | None                   | None
//             | Audio fifo               | None                   | None
//             | Bitstream buffer         | DSP_DRAM_BSBUF_DEC1    | kbytes
//             |                          | DSP_DRAM_BSBUF_DEC2    | kbytes
// DATA_MEMORY |                          | DSP_DRAM_BSBUF_ENC     | kbytes
//             | Main static (+mixer+app) | DSP_DRAM_MAIN_STATIC   | 128 bytes
//             | SwAPP static (SW)        | DSP_DRAM_SWAPP_STATIC  | 128 bytes
//             | Dec1 static              | DSP_DRAM_DEC1_STATIC   | 128 bytes
//             | Dec2 static              | DSP_DRAM_DEC2_STATIC   | 128 bytes
//             | Enc  static              | DSP_DRAM_ENC_STATIC    | 128 bytes
// -----------------------------------------------------------------------------------      

	UINT32 dec1BsBufSize = 0;
	UINT32 dec2BsBufSize = 0;
	UINT32 encBsBufSize = 0;
    
#ifdef ENABLE_DSP_RUNTIME_MEMMAP
    UINT16 codecID = 0;
    codecID = GetValidIndex(WORD16(gRiscParams0[RISC_DEC1_PARAMS_START]),ID_MAX_DECID);
    dec1BsBufSize = gCodecBsBufSizeTbl[codecID];
    codecID = GetValidIndex(WORD16(gRiscParams0[RISC_DEC2_PARAMS_START]),ID_MAX_DECID);
    dec2BsBufSize = gCodecBsBufSizeTbl[codecID];
    codecID = GetValidIndex(WORD16(gRiscParams0[RISC_ENC_PARAMS_START]),ID_MAX_ENCID);
    encBsBufSize = gCodecBsBufSizeTbl[codecID];
#else
	DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
    dec1BsBufSize = pDspCodeSize->decBsSize; 
    dec2BsBufSize = pDspCodeSize->decBsSize; 
    encBsBufSize  = pDspCodeSize->encBsSize; 
#endif


	// Init BsBufInfo ID 
    gBsBufInfoDec1.bsBufID = BSBUF_ID_DEC1;
	gBsBufInfoDec2.bsBufID = BSBUF_ID_DEC2;
	gBsBufInfoEnc.bsBufID  = BSBUF_ID_ENC;


	//
	// Update Risc Parameters
	//
	gRiscParams0[RISC_MAIN_FIFO_DEPTH_CFG] = WORD16(FIFO_DEPTH_SETTING);    	
    gRiscParams0[RISC_MAIN_DRAM_SIZE_BSBUF_DEC1] = WORD16(dec1BsBufSize);           
    gRiscParams0[RISC_MAIN_DRAM_SIZE_BSBUF_DEC2] = WORD16(dec2BsBufSize);           
    gRiscParams0[RISC_MAIN_DRAM_SIZE_BSBUF_ENC]  = WORD16(encBsBufSize);         

}

static UINT32 GetBsBufBase(UINT8 codecBankID)
{
	UINT32 bsbufBase,offset;
	UINT8 dspState = DSP_GetState(DSP_STATE_SYSTEM);
	if (MAIN_STATE_INIT == dspState || MAIN_STATE_READY == dspState || MAIN_STATE_DEAD == dspState)
	{
	    ERROR_PRINT("DSP_UpdateBsInfo::e:invalid state:%d \n",dspState);    
//		diag_printf("GetBsBufBase::e:invalid state:%d \n",dspState);    
		return 0;
	}


	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
	    offset = (UINT32)WORD16(gDspParams0[DSP_DRAM_BSBUF_DEC1])<<7;
		bsbufBase      = (offset > 0) ? ((UINT32)gDataBank+offset):0;		
		break;
	case CODEC_BANK_DEC2:
	    offset = (UINT32)WORD16(gDspParams0[DSP_DRAM_BSBUF_DEC2])<<7;
		bsbufBase      = (offset > 0) ? ((UINT32)gDataBank+offset):0;				
		break;
	case CODEC_BANK_ENC:
	    offset = (UINT32)WORD16(gDspParams0[DSP_DRAM_BSBUF_ENC])<<7;
		bsbufBase      = (offset > 0) ? ((UINT32)gDataBank+offset):0;						
		break;		
	default:				
		DSP_PRINT("DSP_UpdateBsInfo::wrong codecBankID:%d\n",codecBankID);	
		bsbufBase = 0;
	}


//	DSP_PRINT("DSP_GetBsBufBase::id:%d, base:0x%x\n",codecBankID,bsbufBase);
	return bsbufBase; 


}


#ifdef ENABLE_DSP_RUNTIME_MEMMAP
static UINT32 GetBsBufSize(UINT8 codecBankID)
{
	UINT32 bsBufSize = 0;
	UINT16 codecID = 0;
	UINT16 runMode = WORD16(gRiscParams0[RISC_MAIN_MODE]);
	
	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		if(IS_DEC1_ENABLE(runMode))	
			codecID = WORD16(gRiscParams0[RISC_DEC1_PARAMS_START]);
		break;
	case CODEC_BANK_DEC2:
		if(IS_DEC2_ENABLE(runMode))			
			codecID = WORD16(gRiscParams0[RISC_DEC2_PARAMS_START]);
		break;
	case CODEC_BANK_ENC:
		if(IS_ENC_ENABLE(runMode))				
			codecID = WORD16(gRiscParams0[RISC_ENC_PARAMS_START]);
		break;		
	default:				
		DSP_PRINT("DSP_UpdateBsInfo::wrong codecBankID:%d\n",codecBankID);			
	}

	bsBufSize = gCodecBsBufSizeTbl[codecID]<<10;
//	DSP_PRINT("DSP_GetBsBufSize::id:%d,size:%d\n",codecBankID,bsBufSize);		
	return bsBufSize;
	
}
#else
static UINT32 GetBsBufSize(UINT8 codecBankID)
{
	DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
	UINT32 bsBufSize = 0;

	UINT16 runMode = WORD16(gRiscParams0[RISC_MAIN_MODE]);

	
	UINT8 dspState = DSP_GetState(DSP_STATE_SYSTEM);
	if (MAIN_STATE_INIT == dspState || MAIN_STATE_READY == dspState || MAIN_STATE_DEAD == dspState)
	{
	    DSP_PRINT("DSP_UpdateBsInfo::e:invalid state:%d \n",dspState);    
		return 0;
	}

	switch (codecBankID)
	{
	case CODEC_BANK_DEC1:
		if(IS_DEC1_ENABLE(runMode))		
			bsBufSize = pDspCodeSize->decBsSize<<10;					
		break;
	case CODEC_BANK_DEC2:
		if(IS_DEC2_ENABLE(runMode))				
			bsBufSize = pDspCodeSize->decBsSize<<10;			
		break;
	case CODEC_BANK_ENC:
		if(IS_ENC_ENABLE(runMode))					
			bsBufSize = pDspCodeSize->encBsSize<<10;		
		break;		
	default:				
		DSP_PRINT("DSP_UpdateBsInfo::wrong codecBankID:%d\n",codecBankID);			
	}

	return bsBufSize;
}
#endif

UINT32 DSP_Initial( UINT32 DSP_DRAM_BASE_Addr ,UINT16 dsp_init_type )
{
	DSP_hwDramMap(DSP_DRAM_BASE_Addr);    	

	if( dsp_init_type == AUDIF_System_Boot_init )
		DSP_InitRiscParams0();    

	DSP_InitDspParams0();

	if( dsp_init_type == AUDIF_System_Boot_init )
		DSP_InitRiscParams1();

	DSP_InitDspParams1();    

	DSP_InitDSPMailbox();

	DSP_DramDataMap();		
	DSP_FifoMonitor(0, DSP_DRAM_BASE_Addr);
     
	return DSP_ERROR_NONE;
}


UINT32 DSP_GetInfo(UINT32 Info)
{
    UINT32 nReturn = 0;
	UINT32 offset;

    switch(Info)
    {   

    case INFO_CODE_BANK0_BASE:
        nReturn = (UINT32)gCodeBank0;
        break;      
    case INFO_CODE_BANK0_SIZE:
		{
			DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();
        	nReturn = (UINT32)pDspCodeSize->mainCodeSize;
    	}
        break;              
    case INFO_CODE_BANK1_BASE:
        nReturn = (UINT32)gCodeBank1;       
        break;      
    case INFO_CODE_BANK1_SIZE:
		{
			DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();
        	nReturn = (UINT32)pDspCodeSize->decCodeSize;
    	}		
        break;                      
    case INFO_CODE_BANK2_BASE:
        nReturn = (UINT32)gCodeBank2;       
        break;      
    case INFO_CODE_BANK2_SIZE:
		{
			DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();
        	nReturn = (UINT32)pDspCodeSize->decCodeSize;
    	}		
        break;                      
    case INFO_CODE_BANK3_BASE:
        nReturn = (UINT32)gCodeBank3;       
        break;
    case INFO_CODE_BANK3_SIZE:
		{
			DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();
        	nReturn = (UINT32)pDspCodeSize->encCodeSize;
    	}			
        break;              
    case INFO_CODE_BANK4_BASE:
        nReturn = (UINT32)gCodeBank4;       
        break;
    case INFO_CODE_BANK4_SIZE:
		{
			DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();
        	nReturn = (UINT32)pDspCodeSize->swappCodeSize;
    	}					
        break; 		

    case INFO_DATA_BANK_BASE:
        nReturn = (UINT32)gDataBank;        
        break;      
    case INFO_DATA_BANK_SIZE:
			//hcsu:note add a new func?
		{
			DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();

			UINT32 totalCodeBankSize = pDspCodeSize->mainCodeSize + \
		                     2 * pDspCodeSize->decCodeSize + \
		                     1 * pDspCodeSize->encCodeSize + \
		                     pDspCodeSize->swappCodeSize;
			
        	nReturn = DSP_DRAMSIZE - totalCodeBankSize ;
    	}	
        
        break;  
     
    case INFO_RISC_PARAM_BASE:
        nReturn = (UINT32)gRiscParams0;
        break;

    case INFO_DSP_PARAM_BASE:       
        nReturn = (UINT32)gDspParams0;
        break;      

    case INFO_RISC_DEC1_PARAM_BASE:
        nReturn = (UINT32)&gRiscParams0[RISC_DEC1_PARAMS_START];
        break;
    case INFO_RISC_DEC2_PARAM_BASE:
        nReturn = (UINT32)&gRiscParams0[RISC_DEC2_PARAMS_START];
        break;      
    case INFO_RISC_ENC_PARAM_BASE:
        nReturn = (UINT32)&gRiscParams0[RISC_ENC_PARAMS_START];
        break;      
        
    case INFO_DSP_DEC1_PARAM_BASE:
        nReturn = (UINT32)&gDspParams0[DSP_DEC1_PARAMS_START];
        break;
    case INFO_DSP_DEC2_PARAM_BASE:
        nReturn = (UINT32)&gDspParams0[DSP_DEC2_PARAMS_START];
        break;      
    case INFO_DSP_ENC_PARAM_BASE:
        nReturn = (UINT32)&gDspParams0[RISC_ENC_PARAMS_START];
        break;
        
    case INFO_AUD_FIFO_BASE:
        nReturn = (UINT32)(gDataBank + HWREG_R(aud_a0_base));
        break;      
      
    case INFO_MAIN_STATIC_BASE: 		    
    	offset = WORD16(gDspParams0[DSP_DRAM_MAIN_STATIC])<<7;
	    nReturn   = (offset > 0) ? (UINT32)(gDataBank+offset):0;		
        break;      
    case INFO_SWAPP_STATIC_BASE:
    	offset = WORD16(gDspParams0[DSP_DRAM_SWAPP_STATIC])<<7;
	    nReturn   = (offset > 0) ? (UINT32)(gDataBank+offset):0;	
        break;      
    case INFO_DEC1_STATIC_BASE:
    	offset = WORD16(gDspParams0[DSP_DRAM_DEC1_STATIC])<<7;
	    nReturn   = (offset > 0) ? (UINT32)(gDataBank+offset):0;	
        break;      
    case INFO_DEC2_STATIC_BASE:
    	offset = WORD16(gDspParams0[DSP_DRAM_DEC2_STATIC])<<7;
	    nReturn   = (offset > 0) ? (UINT32)(gDataBank+offset):0;	
        break;
    case INFO_ENC_STATIC_BASE:
    	offset = WORD16(gDspParams0[DSP_DRAM_ENC_STATIC])<<7;
	    nReturn   = (offset > 0) ? (UINT32)(gDataBank+offset):0;	
        break;  

    case INFO_MAIN_STATIC_LEN:
	{
		DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
        nReturn = 1024 * pDspCodeSize->mainDramSize;
    }
        break;      
    case INFO_SWAPP_STATIC_LEN:
	{
		DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
        nReturn = 1024 * pDspCodeSize->swappDramSize;
    }
        break;      
    case INFO_DEC1_STATIC_LEN:
	{
		UINT16 runMode = WORD16(gRiscParams0[RISC_MAIN_MODE]);
		DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
		if (IS_DEC1_ENABLE(runMode))
	        nReturn = 1024 * pDspCodeSize->decDramSize;
    }
        break;              
    case INFO_DEC2_STATIC_LEN:
	{
		UINT16 runMode = WORD16(gRiscParams0[RISC_MAIN_MODE]);
		DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
		if (IS_DEC2_ENABLE(runMode))		
	        nReturn = 1024 * pDspCodeSize->decDramSize;
    }
        break;                      
    case INFO_ENC_STATIC_LEN:
	{
		UINT16 runMode = WORD16(gRiscParams0[RISC_MAIN_MODE]);		
		DspCodeSize* pDspCodeSize = DSP_GetCodeSizePtr();  
		if (IS_ENC_ENABLE(runMode))
	        nReturn = 1024 * pDspCodeSize->encDramSize;
    }
        break; 
      
    case INFO_DEC1_BSBUF_BASE: 
	    nReturn = GetBsBufBase(CODEC_BANK_DEC1);
		break;
    case INFO_DEC2_BSBUF_BASE:
		nReturn = GetBsBufBase(CODEC_BANK_DEC2);
		break;
    case INFO_ENC_BSBUF_BASE:
		nReturn = GetBsBufBase(CODEC_BANK_ENC);
		break;
    case INFO_DEC1_BSBUF_LEN:
		nReturn = GetBsBufSize(CODEC_BANK_DEC1);
        break;
    case INFO_DEC2_BSBUF_LEN:
		nReturn = GetBsBufSize(CODEC_BANK_DEC2);
        break;      
    
    case INFO_ENC_BSBUF_LEN: 
		nReturn = GetBsBufSize(CODEC_BANK_ENC);
      	break;      
		
    case INFO_BSINFO_DEC1:
		gBsBufInfoDec1.bsBufID    = BSBUF_ID_DEC1;
		gBsBufInfoDec1.pBsBufBase = (UINT8 *)GetBsBufBase(CODEC_BANK_DEC1);
		gBsBufInfoDec1.bsBufLen   = GetBsBufSize(CODEC_BANK_DEC1);
		gBsBufInfoDec1.dumpPtr    = HWREG_R(dsp_port[2]);
		gBsBufInfoDec1.barrPtr    = HWREG_R(dsp_port[3]);
		nReturn = (UINT32)&gBsBufInfoDec1;	
        break;
    case INFO_BSINFO_DEC2:
		gBsBufInfoDec2.bsBufID    = BSBUF_ID_DEC2;
		gBsBufInfoDec2.pBsBufBase = (UINT8 *)GetBsBufBase(CODEC_BANK_DEC2);
		gBsBufInfoDec2.bsBufLen   = GetBsBufSize(CODEC_BANK_DEC2);
		gBsBufInfoDec2.dumpPtr    = HWREG_R(dsp_port[4]);
		gBsBufInfoDec2.barrPtr    = HWREG_R(dsp_port[5]);		
		nReturn = (UINT32)&gBsBufInfoDec2;	
        break;      
    case INFO_BSINFO_ENC:       
		gBsBufInfoEnc.bsBufID    = BSBUF_ID_ENC;
		gBsBufInfoEnc.pBsBufBase = (UINT8 *)GetBsBufBase(CODEC_BANK_ENC);
		gBsBufInfoEnc.bsBufLen   = GetBsBufSize(CODEC_BANK_ENC);
		gBsBufInfoEnc.dumpPtr    = HWREG_R(dsp_port[8]);
		gBsBufInfoEnc.barrPtr    = HWREG_R(dsp_port[9]);		
		nReturn = (UINT32)&gBsBufInfoEnc;			
        break;
        
        
    default:
        nReturn = 0;
    }

    return nReturn;
            
}

/******************************************************************************
    dsp dram parameters
 *****************************************************************************/ 
#if 0 // remove hw gain control for 9200
UINT32 DSP_SetMasterGAIN(void)
{
	extern const BYTE  VolTab_qae377[VOLUME_SCALE];
	UINT16 gainvalue;

	if( aud_param.volume==0 || aud_param.mute == MUTE_STATUS_ON )
	{
		gainvalue = 0;
		aud_InitRiscParam.chn_gain.MG = 0;
	}
	else if( aud_param.volume == (VOLUME_SCALE-1) )
	{
		gainvalue = VolTab_qae377[aud_param.volume];
		aud_InitRiscParam.chn_gain.MG = (gainvalue<<24);
	}
	else
	{
		gainvalue = VolTab_qae377[aud_param.volume];
		aud_InitRiscParam.chn_gain.MG = (gainvalue<<24 | 0x00FFFF00);
	}

	return aud_InitRiscParam.chn_gain.MG;

}
#endif

void DSP_InitRiscParams0_preload(void) 
{
	// ccyang, 2009.08.17
	// give an initial value for 'aud_InitRiscParam'. 
	// if setup-menu is done, this function will be no more need.
#if 0 // remove hw gain control for 9200
	DSP_SetMasterGAIN();
#else
	aud_InitRiscParam.chn_gain.MG			= 0x80000000;
#endif
	aud_InitRiscParam.chn_gain.Lm           = 0x8000;
	aud_InitRiscParam.chn_gain.Rm           = 0x8000;
	aud_InitRiscParam.chn_gain.A0           = 0x8080;
	aud_InitRiscParam.chn_gain.A1           = 0x8080;
	aud_InitRiscParam.chn_gain.A2           = 0x8080;
	aud_InitRiscParam.chn_gain.A3           = 0x8080;

	aud_InitRiscParam.disc_type	            = 0x0000; //DVD
	aud_InitRiscParam.spdif_mode            = SPDIF_OFF;
	aud_InitRiscParam.hdmi_mode             = HDMI_PCM;
//	aud_InitRiscParam.DAC_info.fs_limit     = K_PCM_OUTPUT_SAMPLE_RATE_ID;
	aud_InitRiscParam.DAC_info.pcm_priority = pcm_NEGATIVE;
	aud_InitRiscParam.echo_decay            = 0x003c;
	aud_InitRiscParam.echo_delay            = 0x2710;
	aud_InitRiscParam.AD_gain               = AD_level_1;
	aud_InitRiscParam.mic_info.upper_th     = 0x0100;
	aud_InitRiscParam.mic_info.presensity   = 0x0040;
	aud_InitRiscParam.mic_info.aftersensity = 0x0009;
	aud_InitRiscParam.mic_info.lower_th     = 0x0055;
	aud_InitRiscParam.keyshift              = (KS_0<<8)|0x01;
	aud_InitRiscParam.EQ_gain40HI           = EQ_STANDARD_40HI;
	aud_InitRiscParam.EQ_gain40LO           = EQ_STANDARD_40LO;
	aud_InitRiscParam.EQ_gain65             = EQ_STANDARD_65;
	aud_InitRiscParam.Surround_3D           = SURROUND_SATNDARD;
	aud_InitRiscParam.bass_mgt              = 0x0;
	aud_InitRiscParam.bass_mgt_cfg          = 0x0;
	aud_InitRiscParam.spk_delay_fnt         = 0x0;
	aud_InitRiscParam.spk_delay_srud        = 0x0;
	aud_InitRiscParam.spk_delay_back        = 0x0;
	aud_InitRiscParam.app_cmode             = 0x8888;
	aud_InitRiscParam.app_control           = 0x163F;
	aud_InitRiscParam.downmix_mode          = dwmix_off;
	aud_InitRiscParam.ch_mode               = OUT_STEREO;
	aud_InitRiscParam.cdrip_mode            = real_mode;
	aud_InitRiscParam.ADC_line_mode         = ext_ADC;
	aud_InitRiscParam.sound_eft_gain        = 0x0;
	aud_InitRiscParam.karaoke_vocal         = vocal_off;
#ifdef SUPPORT_SOFTWARE_APP
	#ifdef SUPPORT_PROLOGIC_II
		aud_InitRiscParam.swapp             = en_prologic2;
	#endif
	#ifdef AUD_SUPPORT_NEO6
		aud_InitRiscParam.swapp             = en_neo6;
	#endif
	#ifdef AUD_SUPPORT_GM7
		aud_InitRiscParam.swapp             = en_gm7;
	#endif
#endif //#ifdef SUPPORT_SOFTWARE_APP


}

UINT32 DSP_InitRiscParams0(void) 
{
    //UINT16 i = 0;

	DSP_InitRiscParams0_preload();  // ccyang, 2009.08.17

    memset(gRiscParams0,0,K_DRAM_SIZE_RISC_PARAMS0);    

    // Writing default risc parameters
#ifdef DEBUG_ENABLE_DEC1
    gRiscParams0[RISC_MAIN_MODE] = WORD16(1<<MODE_DEC1_ENABLE_BIT);
#endif	
#ifdef DEBUG_ENABLE_APP	
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_APP_ENABLE_BIT); 
#endif
#ifdef DEBUG_ENABLE_DEC2	
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_DEC2_ENABLE_BIT); 
#endif	
#ifdef DEBUG_ENABLE_ENC
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_ENC_ENABLE_BIT); 
#endif
#ifdef DEBUG_ENABLE_EFF
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_EFF_ENABLE_BIT); 
#endif
#ifdef DEBUG_ENABLE_MIC
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_MIC_ENABLE_BIT); 
#endif
#ifdef DEBUG_ENABLE_AUXIN
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_AUXIN_ENABLE_BIT); 
#endif
#if 0 //def DEBUG_ENABLE_SPDIFIN
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_SPDIFIN_ENABLE_BIT); 
#endif
#ifdef DEBUG_ENABLE_SNDEFFECT
	gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_SNDEFFECT_ENABLE_BIT); 
#endif
#ifdef DEBUG_ENABLE_SIFIN
    gRiscParams0[RISC_MAIN_MODE] |= WORD16(1<<MODE_SIFIN_ENABLE_BIT); 
#endif


    gRiscParams0[RISC_MAIN_FIFO_CFG_L] = WORD16(0x0007);  // A0-A2:PCM
#ifdef DEBUG_ENABLE_FIFO_A5
    gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<5);   // A5:SPDIF
#endif

#ifdef DEBUG_ENABLE_MIC
	gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<7);    // A7:MIC
	gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<10);   // A10:echo
	gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<9);    // A9:record  // for karaoke sound effect
#endif
	
    // 20110614 always enable A13 FIFO for spdif bypass mode (spdif-out form spdif-in)
	gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<13);   // A13:SPDIF IN

#ifdef ENA_9202_DEMDEC
    gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<12);   
#else //ENA_9202_DEMDEC
#ifdef DEBUG_ENABLE_SIFIN
    gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<14);   
#endif

#ifdef DEBUG_ENABLE_AUXIN
    gRiscParams0[RISC_MAIN_FIFO_CFG_L] |= WORD16(1<<12);   
#endif
#endif //ENA_9202_DEMDEC

    gRiscParams0[RISC_MAIN_FIFO_CFG_H] = WORD16(0x0000);
#if 1 // remove hw gain control for 9200
    gRiscParams0[RISC_MAIN_MASTER_GAIN_H] = WORD16(0x8000); 
    gRiscParams0[RISC_MAIN_MASTER_GAIN_L] = WORD16(0x0000);   
#else	
    gRiscParams0[RISC_MAIN_MASTER_GAIN_H] = WORD16(aud_InitRiscParam.chn_gain.MG>>16); 
	gRiscParams0[RISC_MAIN_MASTER_GAIN_L] = WORD16(aud_InitRiscParam.chn_gain.MG&0xffff);   
#endif
    gRiscParams0[RISC_MAIN_LM_GAIN] = WORD16(aud_InitRiscParam.chn_gain.Lm);    
    gRiscParams0[RISC_MAIN_RM_GAIN] = WORD16(aud_InitRiscParam.chn_gain.Rm);    
    gRiscParams0[RISC_MAIN_A0_GAIN] = WORD16(aud_InitRiscParam.chn_gain.A0);    
    gRiscParams0[RISC_MAIN_A1_GAIN] = WORD16(aud_InitRiscParam.chn_gain.A1);    
    gRiscParams0[RISC_MAIN_A2_GAIN] = WORD16(aud_InitRiscParam.chn_gain.A2);    
    gRiscParams0[RISC_MAIN_A3_GAIN] = WORD16(aud_InitRiscParam.chn_gain.A3);    
    gRiscParams0[RISC_MAIN_DISC_FORMAT] = WORD16(aud_InitRiscParam.disc_type);    // DVD:0, CD:1, DTSCD:2
    gRiscParams0[RISC_MAIN_SPDIF_MODE] = WORD16(aud_InitRiscParam.spdif_mode);     // None:0, PCM:1, RAW:2 REENC:3
    gRiscParams0[RISC_MAIN_HDMI_MODE] = WORD16(aud_InitRiscParam.hdmi_mode);      // None:0, PCM:1, RAW:2
    // 44.1k: 0x0000 88.2k: 0x0001 22.05k: 0x0002 176.4k: 0x0003 11.025k: 0x0004 5.5125k: 0x0005
    // 48  k: 0x0010 96  k: 0x0011 24   k: 0x0012 192  k: 0x0013 12    k: 0x0014      6k: 0x0015
    // 32  k: 0x0020 64  k: 0x0021 16   k: 0x0022 128  k: 0x0023  8    k: 0x0024      4k: 0x0025
    // 768 k: 0x0030 Not indicated: 0x0031
    gRiscParams0[RISC_MAIN_DAC_FS]  = WORD16(K_PCM_OUTPUT_SAMPLE_RATE_ID);    
    gRiscParams0[RISC_MAIN_IEC0_FS] = WORD16(K_IEC0_OUTPUT_SAMPLE_RATE_ID);    
    gRiscParams0[RISC_MAIN_IEC1_FS] = WORD16(K_IEC1_OUTPUT_SAMPLE_RATE_ID);    	
    gRiscParams0[RISC_MAIN_DAC_POLARITY] = WORD16(aud_InitRiscParam.DAC_info.pcm_priority);       
	
#ifdef DEBUG_TEST_IEC0_DELAY
    gRiscParams0[RISC_MAIN_IEC0_DELAY_SAMPLE] = WORD16(0x0);       // max:12032
    gRiscParams0[RISC_MAIN_IEC1_DELAY_SAMPLE] = WORD16(0);           // 
#endif	

    gRiscParams0[RISC_MAIN_DOWNMIX_MODE] = WORD16(0x0);           // 0x0:off 0x1:Lo/Ro 0x2:Lt/Rt        
    //gRiscParams0[RISC_MAIN_MIXER_OUTPUT] = WORD16(0x0);           // 0x0:mixer 0x1:dec1 0x2:dec2    
    gRiscParams0[RISC_MAIN_MIXER_OUTPUT] = WORD16(0x0);           // 0x0:mixer 0x1:dec1 0x2:dec2        

    gRiscParams0[RISC_MAIN_ECHO_GAIN]    		= WORD16(0x0000);
    gRiscParams0[RISC_MAIN_MIC_SWITCH]   		= WORD16(0x0003);
    gRiscParams0[RISC_MAIN_MIC0_GAIN]    		= WORD16(0x8000);
    gRiscParams0[RISC_MAIN_MIC1_GAIN]    		= WORD16(0x8000);
    gRiscParams0[RISC_MAIN_RISC_AD_GAIN] 		= WORD16(0x0033);
    gRiscParams0[RISC_MAIN_MIC_LOWER_TH] 		= WORD16(0x3000);
    gRiscParams0[RISC_MAIN_MIC_UPPER_TH] 		= WORD16(0x4000);  
    gRiscParams0[RISC_MAIN_MIC_PRESENSITY] 		= WORD16(0x1000);
    gRiscParams0[RISC_MAIN_KARAOKE_VOCAL_SCORE] 	= WORD16(0x0000);
    gRiscParams0[RISC_MAIN_KARAOKE_SOUND_EFFECT] = WORD16(0x0000);

    // TruVolume init
    gRiscParams0[RISC_TRUVOL_PARAM0]				= WORD16(0x0080);
    gRiscParams0[RISC_TRUVOL_PARAM1]				= WORD16(0x0804);

    // SRS init
    gRiscParams0[RISC_SRS_TRUHD_CTRL]               = WORD16(0x4B44);    //disable SRS
    //gRiscParams0[RISC_SRS_TRUHD_CTRL]               = WORD16(0xCB44);    //enable SRS
    gRiscParams0[RISC_SRS_WOW_CTRL]                 = WORD16(0xEB2D);
    gRiscParams0[RISC_SRS_TRUBASS_CTRL]             = WORD16(0x1B20);
    
	// SUNSUR init
    	gRiscParams0[RISC_SUNSUR_CTRL]				= WORD16(0x0644);	
	DSP_DramDataMap();
	
#ifdef DEBUG_APP_VERIFICATION	
    #include "APPsetting.inc"
#else
    gRiscParams0[RISC_MAIN_KEY_SHIFT] = WORD16(aud_InitRiscParam.keyshift);	
    gRiscParams0[RISC_MAIN_EQ_GAIN_4_0_HI] = WORD16(aud_InitRiscParam.EQ_gain40HI);
    gRiscParams0[RISC_MAIN_EQ_GAIN_4_0_LO] = WORD16(aud_InitRiscParam.EQ_gain40LO);
    gRiscParams0[RISC_MAIN_EQ_GAIN_6_5] = WORD16(aud_InitRiscParam.EQ_gain65);
    gRiscParams0[RISC_MAIN_3D_SURROUND] = WORD16(aud_InitRiscParam.Surround_3D);
	gRiscParams0[RISC_MAIN_BASS_MANAGEMENT_SPK] = WORD16(aud_InitRiscParam.bass_mgt);
    gRiscParams0[RISC_MAIN_BASS_MANAGEMENT_CFG] = WORD16(aud_InitRiscParam.bass_mgt_cfg);
    gRiscParams0[RISC_MAIN_SPK_TIME_DELAY_FRONT] = WORD16(aud_InitRiscParam.spk_delay_fnt);
    gRiscParams0[RISC_MAIN_SPK_TIME_DELAY_SURROUND] = WORD16(aud_InitRiscParam.spk_delay_srud);
    gRiscParams0[RISC_MAIN_SPK_TIME_DELAY_BACK] = WORD16(aud_InitRiscParam.spk_delay_back);
    gRiscParams0[RISC_MAIN_APP_CMODE] = WORD16(aud_InitRiscParam.app_cmode);
    gRiscParams0[RISC_MAIN_APP_CONTROL] = WORD16(aud_InitRiscParam.app_control);
#endif

#if 1
    gRiscParams0[RISC_MAIN_DUALMPEG_DEC1_GAIN]   = WORD16(0x3FFF);   
    gRiscParams0[RISC_MAIN_DUALMPEG_DEC2_GAIN_H] = WORD16(0x3FFF);
    gRiscParams0[RISC_MAIN_DUALMPEG_DEC2_GAIN_L] = WORD16(0x3FFF);   
    gRiscParams0[RISC_MAIN_MIXER_SELECT]         = WORD16(0x1);  //0x0:BD mixer  0x1:DualMpeg mixer
#endif

    gRiscParams0[RISC_SWAPP_ID] = WORD16(aud_InitRiscParam.swapp);   
    gRiscParams0[RISC_SWAPP_PARAM0] = WORD16(0x0);   
    gRiscParams0[RISC_SWAPP_PARAM1] = WORD16(0x0);   
    gRiscParams0[RISC_SWAPP_PARAM2] = WORD16(0x0); 

#ifdef DEBUG_DUMP_DRAM_PARAMS
	DSP_DumpRiscParams0();
#endif              

    return DSP_ERROR_NONE;
}

UINT32 DSP_InitDspParams0(void) 
{
    
    memset(gDspParams0,0,K_DRAM_SIZE_DSP_PARAMS0);
    
#ifdef DEBUG_DUMP_DRAM_PARAMS 
	DSP_DumpDspParams0();
#endif      

    return DSP_ERROR_NONE;
}
 
UINT32 DSP_WriteRiscParams0(UINT16* pParams)
{
    if (NULL != pParams)
    {
        UINT32 i;

        DSP_PRINT("DSP_WriteRiscParams0::copy parameters from input parameters\n");       

        for (i=0; i<K_RISC_PARAMS0; i++)
        {
            gRiscParams0[i] = WORD16(pParams[i]);        
        }

#ifdef DEBUG_DUMP_DRAM_PARAMS
	DSP_DumpRiscParams0();
#endif  

    }
    else
    {
        DSP_PRINT("DSP_WriteRiscParams0::default intiial risc parameters\n");     
        DSP_InitRiscParams0();
    }

    return DSP_ERROR_NONE;
}

UINT32 DSP_InitDSPMailbox(void) 
{
	/*
	UINT32 i;
	for(i=0; i<31; i++)
	{
		HWREG_W(dsp_port[i], 0);
	}
	*/
	
	HWREG_W(dsp_port[0], 0x80000000);
	HWREG_W(dsp_port[1], 0x00000000);	

	HWREG_W(dsp_port[2], 0x00000000);
	HWREG_W(dsp_port[3], 0x00000000);	

	HWREG_W(dsp_port[4], 0x00000000);
	HWREG_W(dsp_port[5], 0x00000000);		

	HWREG_W(dsp_port[8], 0x00000000);
	HWREG_W(dsp_port[9], 0x00000000);			

	HWREG_W(dsp_port[10], 0x80000000);
	HWREG_W(dsp_port[11], 0x00000000);				

    return DSP_ERROR_NONE;
}



UINT32 DSP_WriteCodecParams(UINT8 codecBankID, UINT16* pParams)
{
	UINT32 i;
    UINT16* pCodecParams = NULL;

    DSP_PRINT("DSP_WriteCodecParams::codecBankID:0x%x ,pParams:0x%x\n",codecBankID,(UINT32)pParams);
	

    switch (codecBankID)
    {
    case CODEC_BANK_DEC1:
		pCodecParams = &gRiscParams0[RISC_DEC1_PARAMS_START];
        break;
    case CODEC_BANK_DEC2:
		pCodecParams = &gRiscParams0[RISC_DEC2_PARAMS_START];
        break;
    case CODEC_BANK_ENC:
		pCodecParams = &gRiscParams0[RISC_ENC_PARAMS_START];
        break;      
    default:                
        DSP_PRINT("DSP_WriteCodecParams::wrong Bank ID:%d\n",codecBankID);        
        return DSP_ERROR_CODEC_BANKID;      
    }   


	if (NULL == pParams)
	{
    	ERROR_PRINT("DSP_WriteCodecParams::e:Reset codec parameters to zero (no codec)!\n");
        // initial CodecParams
        memset(pCodecParams,0,K_DRAM_SIZE_CODEC_PARAMS);      
		return DSP_ERROR_CODEC_PARAM;
	}

    DSP_PRINT("DSP_WriteCodecParams::copy parameters from input parameters\n");       
    for (i=0; i<K_CODEC_PARAMS; i++)
    {
        pCodecParams[i] = WORD16(pParams[i]);       
    }

	DSP_DramDataMap();

#ifdef DEBUG_DUMP_DRAM_PARAMS
	DSP_DumpCodecParams(codecBankID);
#endif  	
    
    return DSP_ERROR_NONE;
}

UINT32 DSP_SetRiscParam0(UINT16 nOffset,UINT16 nValue)
{
    if (nOffset > K_RISC_PARAMS0)
        return DSP_ERROR_RISC_PARAM;    

    DSP_PRINT("DSP_SetRiscParam0::offset:0x%x, value:0x%x\n",nOffset,nValue);

    gRiscParams0[nOffset] = WORD16(nValue);
        
    return DSP_ERROR_NONE;
}

UINT16 DSP_GetRiscParam0(UINT16 nOffset)
{
    if (nOffset > K_RISC_PARAMS0)
    {
        ERROR_PRINT("DSP_GetRiscParam0::invalid offset:%d\n",nOffset);
        return DSP_ERROR_RISC_PARAM;    
    }

    UINT16 nValue = WORD16(gRiscParams0[nOffset]);

//    DSPAPI_PRINT("DSP_GetRiscParam0::offset:0x%x, value:0x%x\n",nOffset,nValue);
        
    return nValue;
}


UINT16 DSP_GetDspParam0(UINT16 nOffset)
{

    if (nOffset > K_DSP_PARAMS0)
    {
        ERROR_PRINT("DSP_GetDspParam0::invalid offset:%d\n",nOffset);
        return DSP_ERROR_DSP_PARAM;
    }

    UINT16 nValue = WORD16(gDspParams0[nOffset]);
//    DSPAPI_PRINT("DSP_GetDspParam0::offset:0x%x, value:%04x\n",nOffset,nValue);  

     return nValue;
}

void DSP_DumpRiscParams0(void)
{
	DSP_PRINT("-------------------------------------------------------\n");
	DSP_PRINT("DSP_DumpRiscParams0::dump risc parameters values:\n");
	dumpWord16((UINT32)gRiscParams0, K_RISC_PARAMS0);
	DSP_PRINT("-------------------------------------------------------\n");
}

void DSP_DumpCodecParams(UINT8 codecBankID)
{
	UINT16* pCodecParams = NULL;

    switch (codecBankID)
    {
    case CODEC_BANK_DEC1:
		pCodecParams = &gRiscParams0[RISC_DEC1_PARAMS_START];
        break;
    case CODEC_BANK_DEC2:
		pCodecParams = &gRiscParams0[RISC_DEC2_PARAMS_START];
        break;
    case CODEC_BANK_ENC:
		pCodecParams = &gRiscParams0[RISC_ENC_PARAMS_START];
        break;      
	default:
		return;
    } 
	DSP_PRINT("-----------------------------------------------------------\n");	
    DSP_PRINT("DSP_DumpCodecParams::dump bank:%d codec parameters values:\n",codecBankID);
    dumpWord16((UINT32)pCodecParams, K_CODEC_PARAMS);	
	DSP_PRINT("-----------------------------------------------------------\n");
}

void DSP_DumpDspParams0(void)
{
	DSP_PRINT("-------------------------------------------------------\n");
	DSP_PRINT("DSP_DumpDspParams0::dump dsp parameters values:\n");
	dumpWord16((UINT32)gDspParams0, K_DSP_PARAMS0);
	DSP_PRINT("-------------------------------------------------------\n");
}

void DSP_DumpMemmap()
{			
	UINT32 address;
	UINT32 base;

	base = DSP_GetInfo(INFO_DATA_BANK_BASE);

	address = DSP_GetInfo(INFO_DATA_BANK_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DATA_BANK_BASE   :0x%08x, offset:0x%08x\n",address,address - base);  
	address = DSP_GetInfo(INFO_RISC_PARAM_BASE);  
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_RISC_PARAM_BASE  :0x%08x, offset:0x%08x\n",address,address - base);  
	address = DSP_GetInfo(INFO_DSP_PARAM_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DSP_PARAM_BASE   :0x%08x, offset:0x%08x\n",address,address - base); 
	address = DSP_GetInfo(INFO_AUD_FIFO_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_AUD_FIFO_BASE    :0x%08x, offset:0x%08x\n",address, address>0?address-base:0); 	
	address = DSP_GetInfo(INFO_DEC1_BSBUF_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC1_BSBUF_BASE  :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 				
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC1_BSBUF_LEN   :0x%08x\n",DSP_GetInfo(INFO_DEC1_BSBUF_LEN)); 					
	address = DSP_GetInfo(INFO_DEC2_BSBUF_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC2_BSBUF_BASE  :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 				
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC2_BSBUF_LEN   :0x%08x\n",DSP_GetInfo(INFO_DEC2_BSBUF_LEN)); 					
	address = DSP_GetInfo(INFO_ENC_BSBUF_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_ENC_BSBUF_BASE   :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 				
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_ENC_BSBUF_LEN    :0x%08x\n",DSP_GetInfo(INFO_ENC_BSBUF_LEN)); 						
	address = DSP_GetInfo(INFO_MAIN_STATIC_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_MAIN_STATIC_BASE :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 		
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_MAIN_STATIC_LEN  :0x%08x\n",DSP_GetInfo(INFO_MAIN_STATIC_LEN)); 				
	address = DSP_GetInfo(INFO_DEC1_STATIC_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC1_STATIC_BASE :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 			
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC1_STATIC_LEN  :0x%08x\n",DSP_GetInfo(INFO_DEC1_STATIC_LEN)); 					
	address = DSP_GetInfo(INFO_DEC2_STATIC_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC2_STATIC_BASE :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 			
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_DEC2_STATIC_LEN  :0x%08x\n",DSP_GetInfo(INFO_DEC2_STATIC_LEN)); 					
	address = DSP_GetInfo(INFO_ENC_STATIC_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_ENC_STATIC_BASE  :0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 			
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_ENC_STATIC_LEN   :0x%08x\n",DSP_GetInfo(INFO_ENC_STATIC_LEN)); 					
	address = DSP_GetInfo(INFO_SWAPP_STATIC_BASE);
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_SWAPP_STATIC_BASE:0x%08x, offset:0x%08x\n",address,address>0?address-base:0); 		
    DSPAPI_PRINT("DSP_DumpMemmap::INFO_SWAPP_STATIC_LEN :0x%08x\n",DSP_GetInfo(INFO_SWAPP_STATIC_LEN)); 	

	DSP_DumpData(DSP_DUMP_DRAMBASE,0,0);
	
}

UINT16 DSP_InitRiscParams1(void) 
{
    memset(gRiscParams1,0,K_DRAM_SIZE_RISC_PARAMS1);    

    gRiscParams1[RISC_MAGDET_THR_H]          = WORD16(0x0000);
    gRiscParams1[RISC_MAGDET_THR_L]          = WORD16(0x8585);

    gRiscParams1[RISC_NMUTE_FMA2_SAP_H]      = WORD16(0x0400);
    gRiscParams1[RISC_NMUTE_FMA2_SAP_L]      = WORD16(0x0400);

    gRiscParams1[RISC_NMUTE_MPX_H]           = WORD16(0x0400);
    gRiscParams1[RISC_NMUTE_MPX_L]           = WORD16(0x0400);

    gRiscParams1[RISC_NMUTE_EIAJ_H]          = WORD16(0x0040);
    gRiscParams1[RISC_NMUTE_EIAJ_L]          = WORD16(0x0040);

    gRiscParams1[RISC_NICAM_CFG_H]           = WORD16(0x0400);
    gRiscParams1[RISC_NICAM_CFG_L]           = WORD16(0x0012);

    gRiscParams1[RISC_DDEP_OPTIONS1]         = WORD16(0x8400);

    gRiscParams1[RISC_DDEP_CTRL_H]           = WORD16(0x00c8);
    gRiscParams1[RISC_DDEP_CTRL_L]           = WORD16(0x6400);

    gRiscParams1[RISC_DEM_CFG_H]             = WORD16(0x0000);
    gRiscParams1[RISC_DEM_CFG_L]             = WORD16(0x0010);

    gRiscParams1[RISC_DEM_CA1_H]             = WORD16(0x0800);
    gRiscParams1[RISC_DEM_CA1_L]             = WORD16(0x0000);

    gRiscParams1[RISC_DEM_CA2_H]             = WORD16(0x0000);
    gRiscParams1[RISC_DEM_CA2_L]             = WORD16(0x0000);

    gRiscParams1[RISC_DEM_MPX_CFG_H]         = WORD16(0x0000);
    gRiscParams1[RISC_DEM_MPX_CFG_L]         = WORD16(0x0000);

    gRiscParams1[RISC_DEM_FMSUB_CFG_H]       = WORD16(0x0000);
    gRiscParams1[RISC_DEM_FMSUB_CFG_L]       = WORD16(0x0000);

    gRiscParams1[RISC_DEM_OUT_CFG_H]         = WORD16(0x0040);
    gRiscParams1[RISC_DEM_OUT_CFG_L]         = WORD16(0x0180);
    
    gRiscParams1[RISC_SRC_CTRL]              = WORD16(0x0000);

    gRiscParams1[RISC_LEV_ADJ_DEM_H]         = WORD16(0x0000);
    gRiscParams1[RISC_LEV_ADJ_DEM_L]         = WORD16(0x0000);

    gRiscParams1[RISC_LEV_ADJ_IO]            = WORD16(0x0000);

    gRiscParams1[RISC_SOU_APP_MOD_H]         = WORD16(0x00C2);
    gRiscParams1[RISC_SOU_APP_MOD_L]         = WORD16(0x0200);
    
    gRiscParams1[RISC_SOU_DVM_CTRL]          = WORD16(0x007C);
    gRiscParams1[RISC_SOU_EFF]               = WORD16(0x0000);
    gRiscParams1[RISC_MAIN_SOU_EFF]          = WORD16(0x0701); //set default turn-on SW-APP2

    gRiscParams1[RISC_DOL_CTRL]              = WORD16(0x5000);

    gRiscParams1[RISC_MASTER_VOL]            = WORD16(0x0000);
    gRiscParams1[RISC_MAIN_VOL]              = WORD16(0x0000);

    gRiscParams1[RISC_SW_C_S_VOL_H]          = WORD16(0x0000);
    gRiscParams1[RISC_SW_C_S_VOL_L]          = WORD16(0x0000);
    gRiscParams1[RISC_MAIN_TON_CTRL]         = WORD16(0x0000);
    gRiscParams1[RISC_CENTER_TON_CTRL]       = WORD16(0x0000);
    gRiscParams1[RISC_SURR_TON_CTRL]         = WORD16(0x0000);
    gRiscParams1[RISC_EQMAIN1_TON_CTRL]      = WORD16(0xA000);
    gRiscParams1[RISC_EQMAIN2_TON_CTRL]      = WORD16(0x0000);
    gRiscParams1[RISC_EQCENTER1_TON_CTRL]    = WORD16(0x0000);
    gRiscParams1[RISC_EQCENTER2_TON_CTRL]    = WORD16(0x0000);
    gRiscParams1[RISC_DD_FLID]               = WORD16(0x0002);
    gRiscParams1[RISC_DD_FLID_COEF_H]        = WORD16(0x0000);
    gRiscParams1[RISC_DD_FLID_COEF_L]        = WORD16(0x0000);

    gRiscParams1[RISC_INF_DD_FLIDSTATUS_H]   = WORD16(0x0000);
    gRiscParams1[RISC_INF_DD_FLIDSTATUS_L]   = WORD16(0x0000);
    gRiscParams1[RISC_SILDETECT]             = WORD16(0x50F0);

    // main output path selection: 0:Output_PCMInfo 1:SIF   2:ADC
    gRiscParams1[RISC_MAIN_MAINOUTPUT]       = WORD16(0x0000);

    // aux output path selection : 0:main follow   1:SIF   2:ADC   3:OFF
    gRiscParams1[RISC_MAIN_AUXOUTPUT]       = WORD16(0x0303);  //bit[7:0] aux1
                                                               //bit[15:8]aux2
    gRiscParams1[RISC_MAIN_AUX1_VOL]        = WORD16(0x0000);
    gRiscParams1[RISC_MAIN_AUX2_VOL]        = WORD16(0x0000);

    gRiscParams1[RISC_MAIN_PCMOUTPUT_CTRL] = WORD16(0x0000);
    gRiscParams1[RISC_MAIN_PCMOUTPUT_CTRL] |= WORD16(aud_param.pcm_output_phase & 0x7);
    
    // ADC LR inverse bit 0=no inverse, 1=inverse
    // gRiscParams1[RISC_MAIN_PCMOUTPUT_CTRL] |= WORD16(0x1<<3);    
	return DSP_ERROR_NONE;
}

UINT16 DSP_InitDspParams1(void) 
{
    
    memset(gDspParams1,0,K_DRAM_SIZE_DSP_PARAMS1);

    return DSP_ERROR_NONE;
}

UINT16 DSP_SetRiscParam1(UINT16 nOffset,UINT16 nValue)
{
    if (nOffset > K_RISC_PARAMS1)
        return DSP_ERROR_RISC_PARAM;    

    DSP_PRINT("DSP_SetRiscParam1::offset:0x%02x, value:0x%04x\n",nOffset,nValue);

	gRiscParams1[nOffset] = WORD16(nValue);
        
    return DSP_ERROR_NONE;
}

UINT16 DSP_GetRiscParam1(UINT16 nOffset)
{
    UINT16 nValue;

	if (nOffset > K_RISC_PARAMS1)
    {
        ERROR_PRINT("DSP_GetRiscParam1::invalid offset:%d\n",nOffset);
        return DSP_ERROR_RISC_PARAM;    
    }

	nValue = WORD16(gRiscParams1[nOffset]);
        
    return nValue;
}

UINT16 DSP_GetDspParam1(UINT16 nOffset)
{
	UINT16 nValue;

    if (nOffset > K_DSP_PARAMS1)
    {
        ERROR_PRINT("DSP_GetDspParam1::invalid offset:%d\n",nOffset);
        return DSP_ERROR_DSP_PARAM;
    }

    nValue = WORD16(gDspParams1[nOffset]);

    return nValue;
}

UINT16 DSP_GetSSDBit(UINT16 nOffset, UINT32 dSSDDataBitMask, UINT8 dSSDDataBitIndex)
{
    UINT16  dSSDData;
	dSSDData = DSP_GetRiscParam1(nOffset);
    dSSDData=(dSSDData&dSSDDataBitMask)>>dSSDDataBitIndex;
    return dSSDData;
}

void DSP_SetSSDBit(UINT16 nOffset, UINT32 dSSDDataBitMask, UINT8 bSSDDataBitIndex, UINT16 dSSDData)
{
    UINT16  u16Tmp;

	u16Tmp = DSP_GetRiscParam1(nOffset);
    u16Tmp&=(~dSSDDataBitMask);

    dSSDData=(dSSDData<<bSSDDataBitIndex)&dSSDDataBitMask;
	DSP_SetRiscParam1(nOffset, (u16Tmp|dSSDData));
}
UINT16 DSP_GetBitMaskRiscParam1(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo)
{
    UINT16  dData, mask;    
    
	dData = DSP_GetRiscParam1(nOffset);
    mask = (1<<(dDataBitHi - dDataBitLo + 1)) - 1;      

    return ((dData>>dDataBitLo)&mask);    
}

void DSP_SetBitMaskRiscParam1(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo, UINT16 dMaskedData)
{
	UINT16 dData, mask;

    dData= DSP_GetRiscParam1(nOffset);
    mask = (1<<(dDataBitHi - dDataBitLo + 1)) - 1;

    dData &= ~(mask<<dDataBitLo);
    dData |= ((dMaskedData&mask)<<dDataBitLo);

    DSP_SetRiscParam1(nOffset, dData);
}

UINT16 DSP_GetBitMaskRiscParam0(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo)
{
    UINT16  dData, mask;    
    
	dData = DSP_GetRiscParam0(nOffset);
    mask = (1<<(dDataBitHi - dDataBitLo + 1)) - 1;      

    return ((dData>>dDataBitLo)&mask);    
}

void DSP_SetBitMaskRiscParam0(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo, UINT16 dMaskedData)
{
	UINT16 dData, mask;

    dData= DSP_GetRiscParam0(nOffset);
    mask = (1<<(dDataBitHi - dDataBitLo + 1)) - 1;

    dData &= ~(mask<<dDataBitLo);
    dData |= ((dMaskedData&mask)<<dDataBitLo);

    DSP_SetRiscParam0(nOffset, dData);
}

INT16 DSP_GetSignedBitMaskRiscParam1(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo)
{
    UINT16  dData, mask, sign;    
    
	dData = DSP_GetRiscParam1(nOffset);
    mask = (1<<(dDataBitHi - dDataBitLo + 1)) - 1;      
    dData = (dData>>dDataBitLo)&mask;
    sign = dData & ((mask + 1)>>1);    

    if (sign)
    {
        return ((INT16)(dData|(~mask)));
    }
    else
    {
        return ((INT16)(dData & mask));
    }    
}
