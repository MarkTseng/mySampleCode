/******************************************************************************
 * Filename    : dsp_util.c
 *
 * Description : audio dsp util
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
///#include "regmap_8600.h"
///#include "gsl/gl_api.h"
#include <asm/mach-spv9200/regmap_9200.h>
#include <linux/printk.h>

//#include "dsp_main_driver.h"

/******************************************************************************
	Local Defines
 *****************************************************************************/


/******************************************************************************
    global&static variables
 *****************************************************************************/
UINT32 gPrintFilter = 0xFFFFFFFF; //enable all

/******************************************************************************
	function implementations
 *****************************************************************************/

void DSP_Memcpy(void *dest, const void *src, size_t n)
{
	//if (IS_CACHED_ADDRESS((UINT32)dest)) dest = (void *)((UINT32)dest | 0x20000000);
	//if (IS_CACHED_ADDRESS((UINT32)src))  src  = (const void *)((UINT32)src | 0x20000000);
	memcpy(dest,src,n);
}

void DSP_Memcpy_Cached(void *dest, const void *src, size_t n)
{
	//if (IS_CACHED_ADDRESS((UINT32)dest)) dest = (void *)((UINT32)dest | 0x20000000);
	memcpy(dest,src,n);
}

void dumpDspIDM(UINT32 pData, UINT32 nByteSize)
{
	UINT32 i;
	UINT16 j = 0;
	UINT8 *pBytedata;
	UINT32 IdmStartAddr = (UINT32)HWREG_R(dsp_dma2_sram_addr);
	UINT16 nWord16Size = nByteSize/2;

	pBytedata = (UINT8*)pData;

	DSP_PRINT("dumpDspIDM::0x%x[%d]\n",(UINT32)pBytedata,nWord16Size);

	for (i=0; i<nWord16Size; i++)
	{
		if (0 == j) DSP_PRINT("0x%08x : ",IdmStartAddr);
		IdmStartAddr++;
		DSP_PRINT("%02x%02x ",(UINT8)*pBytedata,(UINT8)*(pBytedata+1));
		j++;
		pBytedata = pBytedata+2;
		if (8 == j)
		{
			j =0;
			DSP_PRINT("\n");
		}
	}
	DSP_PRINT("\n");

}

void dumpByte(UINT32 pData, UINT32 nByteSize)
{
	UINT32 i;
	UINT16 j = 0;
	UINT8 *pBytedata;

	pBytedata = (UINT8*)pData;

	DSP_PRINT("dumpByte::0x%x[%d]\n",(UINT32)pBytedata,nByteSize);

	for (i=0; i<nByteSize; i++)
	{
		if (0 == j) DSP_PRINT("0x%08x : ",(UINT32)pBytedata);
		DSP_PRINT("%02x ",(UINT8)*pBytedata);
		j++;
		pBytedata++;
		if (16 == j)
		{
			j =0;
			DSP_PRINT("\n");
		}
	}
	DSP_PRINT("\n");
}

void dumpWord16(UINT32 pData, UINT32 nWord16Size)
{
	UINT32 i;
	UINT16 j = 0;
	UINT8 *pBytedata;

	pBytedata = (UINT8*)pData;


	DSP_PRINT("dumpWord16::0x%x[%d]\n",(UINT32)pBytedata,nWord16Size);

	for (i=0; i<nWord16Size; i++)
	{
		if (0 == j) DSP_PRINT("0x%08x : ",(UINT32)pBytedata);
		DSP_PRINT("%02x%02x ",(UINT8)*pBytedata,(UINT8)*(pBytedata+1));
		j++;
		pBytedata = pBytedata+2;
		if (8 == j)
		{
			j =0;
			DSP_PRINT("\n");
		}
	}
	DSP_PRINT("\n");

}

void dumpWord24(UINT32 pData, UINT32 nWord24Size)
{
	UINT32 i;
	UINT16 j = 0;
	UINT8 *pBytedata;

	pBytedata = (UINT8*)pData;

	DSP_PRINT("dumpWord24::0x%x[%d]\n",(UINT32)pBytedata,nWord24Size);

	for (i=0; i<nWord24Size; i++)
	{
		if (0 == j) DSP_PRINT("0x%08x : ",(UINT32)pBytedata);
		DSP_PRINT("%02x%02x%02x ",(UINT8)*pBytedata,(UINT8)*(pBytedata+1),(UINT8)*(pBytedata+2));
		j++;
		pBytedata = pBytedata+3;
		if (8 == j)
		{
			j =0;
			DSP_PRINT("\n");
		}
	}
	DSP_PRINT("\n");
}


void dumpWord32(UINT32 pData, UINT32 nWord32Size)
{
	UINT32 i;
	UINT16 j = 0;
	UINT8 *pBytedata;

	pBytedata = (UINT8*)pData;


	DSPAPI_PRINT("dumpWord32::0x%x[%d]\n",(UINT32)pBytedata,nWord32Size);

	for (i=0; i<nWord32Size; i++)
	{
		if (0 == j) DSP_PRINT("0x%08x : ",(UINT32)pBytedata);
		DSP_PRINT("%02x%02x%02x%02x ",(UINT8)*pBytedata,(UINT8)*(pBytedata+1),(UINT8)*(pBytedata+2),(UINT8)*(pBytedata+3));
		j++;
		pBytedata = pBytedata+4;
		if (4 == j)
		{
			j =0;
			DSP_PRINT("\n");
		}
	}
	DSP_PRINT("\n");
}

void dumpHex(UINT32 pData, UINT32 nHexWordSize)
{
	UINT32 i;
	UINT8 *pBytedata;

	pBytedata = (UINT8*)pData;


	DSP_PRINT("dumpHex::0x%x[%d]\n",(UINT32)pBytedata,nHexWordSize);

	for (i=0; i<nHexWordSize; i++)
	{
		DSP_PRINT("0x%02x%02x\n",(UINT8)*pBytedata,(UINT8)*(pBytedata+1));
		pBytedata = pBytedata+2;
	}
}




#if 0
#define FPGA_TEST_DONE				0xFFFF
#define FPGA_TEST_PASS				0xC00D
#define FPGA_TEST_FAIL				0xDEAD

#define FPGA_TEST_MEM_STORE        	0x0A00
#define FPGA_TEST_BANK_SWITCH      	0x0B00
#define FPGA_TEST_DMA              	0x0C00
#define FPGA_TEST_ISR				0x0D00
#define FPGA_TEST_TIMER				0x0E00
#define FPGA_TEST_PROFILER			0x0F00


void FPGAMsgloop(void);
void FPGATestItem(UINT16 FPGAtestItem);

 /****************************************************************************
  FPGATestItem
 ****************************************************************************/

void FPGATestItem(UINT16 FPGAtestItem)
{
	switch (FPGAtestItem)
	{
	case FPGA_TEST_MEM_STORE:
		DSPAPI_PRINT("FPGA_TEST_MEM_STORE :");
		break;
	case FPGA_TEST_BANK_SWITCH:
		DSPAPI_PRINT("FPGA_TEST_BANK_SWITCH :");
		break;
	case FPGA_TEST_DMA:
		DSPAPI_PRINT("FPGA_TEST_DMA :");
		break;
	default:
		DSPAPI_PRINT("Unknown Test Item : %x",FPGAtestItem);
	}

}

/****************************************************************************
  FPGAMsgloop
 ****************************************************************************/
void FPGAMsgloop(void)
{
	UINT32 nDSPMsg = 0;
	UINT16 nFPGATestItem = 0;
	UINT16 nFPGAMsg = 0;
	UINT32 i=0;

	while(1)
	{
		nDSPMsg = dspGetMessage();
		nFPGATestItem = (nDSPMsg>>16)&0xFFFF;
		nFPGAMsg = nDSPMsg&0xFFFF;
		switch(nFPGAMsg)
		{
		case FPGA_TEST_PASS:
			FPGATestItem(nFPGATestItem);DSPAPI_PRINT(" Passed\n");
			//DSP_STALL();
			HWREG_W(rdif_ctrl, 1<<1 | 1<<5);

			return;
		case FPGA_TEST_FAIL:
			FPGATestItem(nFPGATestItem);DSPAPI_PRINT(" Failed\n");
			//DSP_STALL();
			HWREG_W(rdif_ctrl, 1<<1 | 1<<5);
			return;
		case FPGA_TEST_DONE:
			FPGATestItem(nFPGATestItem);;DSPAPI_PRINT(" Done\n");
			//DSP_STALL();
			HWREG_W(rdif_ctrl, 1<<1 | 1<<5);
			return;
		case 0x0:
			i++;						// no message
			break;
		default:
			i = 0;						// FPGA dsp message received, reset i
		}
		if (i >= 0x10000) return;
	}
}

#endif


// OCEM module
/*
void ocm_busy_wait()
{
   int ocm_status;
   int ocm_status_busy;

   do {
      dspWaitloop(1);
      ocm_status = HWREG_R(REG_OCEM_STAT);
   } while ( ocm_status!=0 );
}
*/

int ocm_io_read(int OCM_REG)
{
   HWREG_W(ocm_inst_reg, OCM_REG);
   HWREG_W(ocm_data_reg[0], 0x0);
   HWREG_W(ocm_cntl_reg, 0x1);
   OCEM_BUSY_WAIT();
   return (HWREG_R(ocm_data_reg[0]));
}

//void ocm_io_write(int OCM_REG, int WRITE_DATA)
void ocm_io_write(int WRITE_DATA)
{
   HWREG_W(ocm_inst_reg, OCEM_INST_SCAN);
   HWREG_W(ocm_data_reg[0], WRITE_DATA);
   HWREG_W(ocm_cntl_reg, 0x1);
   OCEM_BUSY_WAIT();
}

void ocm_ins_scan(int REG0, int REG1, int REG2, int REG3, int REG4, int REG5, int REG6, int REG7)
{
   HWREG_W(ocm_inst_reg, OCEM_INST_SCAN);

   HWREG_W(ocm_data_reg[0], REG0);
   HWREG_W(ocm_data_reg[1], REG1);
   HWREG_W(ocm_data_reg[2], REG2);
   HWREG_W(ocm_data_reg[3], REG3);
   HWREG_W(ocm_data_reg[4], REG4);
   HWREG_W(ocm_data_reg[5], REG5);
   HWREG_W(ocm_data_reg[6], REG6);
   HWREG_W(ocm_data_reg[7], REG7);
   HWREG_W(ocm_cntl_reg, 0x1);
   OCEM_BUSY_WAIT();
}
/*
int DSP_ipage_addr_convert(int page_adr, int index)
{
   int addr_msb4;

   addr_msb4 = (page_adr>>28)&0xf;
   page_adr = ((page_adr & 0x0fffffff) >> 8);
   if ( (addr_msb4==0x9)||(addr_msb4==0xb) ) {
      page_adr = 0x10000000 | (index<<24) | page_adr;
   } else {
      page_adr = 0x00000000 | (index<<24) | page_adr;
   }//if
   return page_adr;
}//dsp_ipage_addr_convert

int DSP_dpage_addr_convert(int page_adr)
{
   int addr_msb4;

   addr_msb4 = (page_adr>>28)&0xf;
   page_adr = (page_adr & 0x0fffffff);
   if ( (addr_msb4==0x9)||(addr_msb4==0xb) ) {
      page_adr = 0x10000000 | page_adr;
   } else {
      page_adr = 0x00000000 | page_adr;
   }//if
   return page_adr;
}//DSP_dpage_addr_convert

int audio_hw_addr_convert(int page_adr)
{
   int addr_msb4;

   addr_msb4 = (page_adr>>28)&0xf;
   page_adr = (page_adr & 0x0fffffff);
   if ( (addr_msb4==0x9)||(addr_msb4==0xb) ) {
      page_adr = 0x40000000 | page_adr;
   } else {
      page_adr = 0x00000000 | page_adr;
   }
   return 1;

}//audio_hw_a
*/
#define DUMMY_MASTER0_BW_ALLOCATE(a,b,c) do{                      \
                                           int shift   = (a==4) ? 5 : (a==8) ? 4 : 0; \
                                           int temp    = (( ((1000   << shift) * c ) >> 4) / (b*10)); \
                                           int natural = temp - 1;\
                                           int decimal = (( ((100000 << shift) * c ) >> 4) / (b*10)) - (100 * temp); \
                                           UINT32* pDummyRequestPeriod = (UINT32*)0xbffe0714; \
                                            *pDummyRequestPeriod = (natural & 0xffff) | ((decimal&0x7f) << 16); \
                                       } while (0)
#define DUMMY_MASTER1_BW_ALLOCATE(a,b,c) do{                      \
                                           int shift   = (a==4) ? 5 : (a==8) ? 4 : 0; \
                                           int temp    = (( ((1000   << shift) * c ) >> 4) / (b*10)); \
                                           int natural = temp - 1;\
                                           int decimal = (( ((100000 << shift) * c ) >> 4) / (b*10)) - (100 * temp); \
                                           UINT32* pDummyRequestPeriod = (UINT32*)0xbffe0754; \
                                           *pDummyRequestPeriod = (natural & 0xffff) | ((decimal&0x7f) << 16); \
                                       } while (0)
/*

#define DUMMY_MASTER0_BW_ALLOCATE(a,b) do{                      \
                                           int temp    = (((1000   << (64/a)) >> 4) / (b*10)); \
                                           int natural = temp - 1;\
                                           int decimal = (((100000 << (64/a)) >> 4) / (b*10)) - (100 * temp); \
                                           UINT32* pDummyRequestPeriod = (UINT32*)0xbffe0714; \
                                           *pDummyRequestPeriod = (natural & 0xffff) | ((decimal&0x7f) << 16); \
                                       } while (0)

#define DUMMY_MASTER1_BW_ALLOCATE(a,b) do{                      \
                                           int temp    = (((1000   << (64/a)) >> 4) / (b*10)); \
                                           int natural = temp - 1;\
                                           int decimal = (((100000 << (64/a)) >> 4) / (b*10)) - (100 * temp); \
                                           UINT32* pDummyRequestPeriod = (UINT32*)0xbffe0754; \
                                           *pDummyRequestPeriod = (natural & 0xffff) | ((decimal&0x7f) << 16); \
                                       } while (0)                                       
//a = # of byte per cycle from SDRAM, SDRAM0 = 8, SDRAM1 = 4
//b = dummy bandwidth
*/

void SetDummyMaster(void)
{

#ifdef USE_DRAM_A
		DSPAPI_PRINT("SetFPGADummyMaster::using dram A\n");
#else

	// DRAMB: (DSP:13%, DummyMaster:66%, dsp overhead:4%)
    // DMA performance: write:216 cycles ,read: 211 cycles

	DSPAPI_PRINT("SetFPGADummyMaster::using dram B\n");

	// 1st Step:
	   HWREG_W(cbus_mx_type, 0x100000); 		//DSP DMA -> RT	          
//       HWREG_W(cbus_m4_bdwh_reg, (600 << 16));
		   
	   HWREG_W(mbar1_globe_setting, 500);	// ARB_PREIOD(reg29[11:0]) = 500
	   HWREG_W(mbar1_page_setting, 0);		// master control page
	   HWREG_W(mbar1_m_setting[27], 0x25);		// dummy master:disable urgent(0x20)/realtime(0x05)
	   HWREG_W(mbar1_m_setting[2], 0x6);    	// AUD: low latnecy
	   HWREG_W(mbar1_m_setting[0], 0x6);		// cbus: low latency(0x6)
	   HWREG_W(mbar1_page_setting, 2);		// master bandwidth page
	   										// total bw = 1600MB
	   										// 13%  = 1600*0.13  = 208M
	   										// 6.5% = 1600*0.066 =
	   HWREG_W(mbar1_m_setting[0], 130/2);    	// DSP BW(CBUS):6->47, 13->79 60->319
	   HWREG_W(mbar1_m_setting[2], 100/2);    	// AUD: low latnecy:10%
	   HWREG_W(mbar1_m_setting[27], 572/2);    // dummy master:66.2%
	   HWREG_W(mbar1_page_setting, 1);		// master priority page
	   HWREG_W(mbar1_m_setting[0], 82);    	// 130*32/1000*16+15 = 81.56, 
	   HWREG_W(mbar1_m_setting[2], 53);    	// 100*32/1000*16+15 = 52.7
	   HWREG_W(mbar1_m_setting[27], 307);      // priority:(572*32/1000 <<4)+15 = 307

// QAE377 A
/*
    	HWREG_W(dummy1_addr_base, 0x11E00000); 	// addr_h
  		HWREG_W(dummy1_request_period, 0x4);    		// Total bandwidth. Unit is number of bytes per cycle. Set 8 for Luma at 8600 and 4 for Chroma at 8600
  		HWREG_W(dummy1_addr_offset, 0x80);
		DUMMY_MASTER0_BW_ALLOCATE(4,98);		// percentage of used bandwidth. default is 50% ->92  		
  		HWREG_W(dummy1_op_mode, 0x2);    		// OP mode : 1 = write only; 2 = read only, 3 = read after write
*/

//QAE377 B
  		HWREG_W(dummy1_addr_offset, 0x80);
/*		DUMMY_MASTER1_BW_ALLOCATE(4,98);		// BW=4 byte/cycle, 98%*/
		//DUMMY_MASTER0_BW_ALLOCATE(8,50,32);     // dummy master0 ¤@¦¸ request access 32 µ§ double-word ¥e SDRAM0 50% ÀW¼e
	    DUMMY_MASTER1_BW_ALLOCATE(4,50,32);     // dummy master1 ¤@¦¸ request access 32 µ§ double-word ¥e SDRAM1 50% ÀW¼e		
	    HWREG_W(dummy1_op_mode, 0x00010002);   // default mode : read after write with MBurstLegnth = 32 (A32B32)
	    HWREG_W(dummy1_urgent, 0x0);          // disable urgent 
        HWREG_W(dummy1_control, 0x1);    		// enable dummy master 

DSPAPI_PRINT("op = %x, urgent = %x, bw = %x, enable = %x \n", HWREG_R(dummy1_op_mode), HWREG_R(dummy1_urgent), HWREG_R(dummy1_request_period), HWREG_R(dummy1_control));

			// fix DTSHD hang up issue 20091218 by dylan
			HWREG_W(dummy1_addr_base, HWREG_R(dummy1_addr_base) & ~(1<<27));

#endif
}



#ifdef QAE377_FPGA_MODE

//=============================================================
// FPGA only
/*
			0: //48k
			1://96k
			2://192k
			3://44.1k
			4://24k
			5: //12k
			6://32k
			7://16k
			8://8k
			9://22.05k
			default://48k
// Sample Rate Specified by RISC parameters
// Sample Rate ID
// 44.1k: 0x0000 88.2k: 0x0001 22.05k: 0x0002 176.4k: 0x0003 11.025k: 0x0004 5.5125k: 0x0005
// 48  k: 0x0010 96  k: 0x0011 24   k: 0x0012 192  k: 0x0013 12    k: 0x0014      6k: 0x0015
// 32  k: 0x0020 64  k: 0x0021 16   k: 0x0022 128  k: 0x0023  8    k: 0x0024      4k: 0x0025
// 768 k: 0x0030 Not indicated: 0x0031
*/

#define FS_192K			2
#define FS_96K			1
#define FS_48K			0	

void SetFpgaIEC0SampleRate(UINT16 sampleRateID)
{

	switch (sampleRateID&0x000F)
	{
	case 0x0:
		DSPAPI_PRINT("SetFpgaIEC0SampleRate::FS_48K output group:0x%x\n",sampleRateID);			  
	    HWREG_W(aud_iec0_bclk_cfg, FS_48K);   
		break;
	case 0x1:
		DSPAPI_PRINT("SetFpgaIEC0SampleRate::FS_96K output group:0x%x\n",sampleRateID);	
	    HWREG_W(aud_iec0_bclk_cfg, FS_96K);    
		break;
	case 0x3:
		DSPAPI_PRINT("SetFpgaIEC0SampleRate::FS_192K output group:0x%x\n",sampleRateID);		
	    HWREG_W(aud_iec0_bclk_cfg, FS_192K);   
		break;
	default:
		DSPAPI_PRINT("SetFpgaIEC0SampleRate::FS_48K output group:0x%x\n",sampleRateID);						
	    HWREG_W(aud_iec0_bclk_cfg, FS_48K);   
		break;
	}

}

void SetFpgaIEC1SampleRate(UINT16 sampleRateID)
{

	switch (sampleRateID&0x000F)
	{
	case 0x0:
		DSPAPI_PRINT("SetFpgaIEC1SampleRate::FS_48K output group:0x%x\n",sampleRateID);			
	    HWREG_W(aud_iec1_bclk_cfg, FS_48K);   
		break;
	case 0x1:
		DSPAPI_PRINT("SetFpgaIEC1SampleRate::FS_96Koutput group:0x%x\n",sampleRateID);					
	    HWREG_W(aud_iec1_bclk_cfg, FS_96K);    
		break;
	case 0x3:
		DSPAPI_PRINT("SetFpgaIEC1SampleRate::FS_192K output group:0x%x\n",sampleRateID);					
	    HWREG_W(aud_iec1_bclk_cfg, FS_192K);   
		break;
	default:
		DSPAPI_PRINT("SetFpgaIEC1SampleRate::FS_48K output group:0x%x\n",sampleRateID);			
	    HWREG_W(aud_iec1_bclk_cfg, FS_48K);   
		break;
	}
}

void SetFpgaPCMSampleRate(UINT16 sampleRateID)
{	
	switch (sampleRateID&0x000F)
	{
	case 0x0:
		DSPAPI_PRINT("SetFpgaPCMSampleRate::FS_48K output group:0x%x\n",sampleRateID);			
	    HWREG_W(aud_ext_dac_bck_cfg, FS_48K);   
	    HWREG_W(aud_int_dac_bck_cfg, FS_48K);   		
		break;
	case 0x1:
		DSPAPI_PRINT("SetFpgaPCMSampleRate::FS_96Koutput group:0x%x\n",sampleRateID);					
	    HWREG_W(aud_ext_dac_bck_cfg, FS_96K);    
	    HWREG_W(aud_int_dac_bck_cfg, FS_96K);   		
		break;
	case 0x3:
		DSPAPI_PRINT("SetFpgaPCMSampleRate::FS_192K output group:0x%x\n",sampleRateID);					
	    HWREG_W(aud_ext_dac_bck_cfg, FS_192K);    
	    HWREG_W(aud_int_dac_bck_cfg, FS_192K);  		
		break;
	default:
		DSPAPI_PRINT("SetFpgaPCMSampleRate::FS_48K output group:0x%x\n",sampleRateID);					
	    HWREG_W(aud_ext_dac_bck_cfg, FS_48K);   
	    HWREG_W(aud_int_dac_bck_cfg, FS_48K);   		
		break;
	}
}


void SetFpgaEnvironment(void)
	{
	SetFpgaPCMSampleRate(K_PCM_OUTPUT_SAMPLE_RATE_ID);
	SetFpgaIEC0SampleRate(K_IEC0_OUTPUT_SAMPLE_RATE_ID);
	SetFpgaIEC1SampleRate(K_IEC1_OUTPUT_SAMPLE_RATE_ID);	

	DSP_PRINT("SetFpgaEnvironment::HWREG_R(aud_ext_dac_bck_cfg):[0x%x]0x%x\n",&HWREG_R(aud_ext_dac_bck_cfg),HWREG_R(aud_ext_dac_bck_cfg));
	DSP_PRINT("SetFpgaEnvironment::HWREG_R(aud_int_dac_bck_cfg):[0x%x]0x%x\n",&HWREG_R(aud_int_dac_bck_cfg),HWREG_R(aud_int_dac_bck_cfg));
	DSP_PRINT("SetFpgaEnvironment::HWREG_R(aud_iec0_bck_cfg):[0x%x]0x%x\n",&HWREG_R(aud_iec0_bclk_cfg),HWREG_R(aud_iec0_bclk_cfg));
	DSP_PRINT("SetFpgaEnvironment::HWREG_R(aud_iec1_bck_cfg):[0x%x]0x%x\n",&HWREG_R(aud_iec1_bclk_cfg),HWREG_R(aud_iec1_bclk_cfg));
}

#endif //QAE377_FPGA_MODE

void dspDumpPCM(void)
{
		UINT32 nA0dramAddress= HWREG_R(aud_audhwya) + HWREG_R(aud_a0_base);
		DSPAPI_PRINT("dspDumpPCM::A0 dram:0x%x base:0x%x Len:0x%x Ptr:0x%x Cnt:0x%x\n",nA0dramAddress,HWREG_R(aud_a0_base),HWREG_R(aud_a0_length),HWREG_R(aud_a0_ptr),HWREG_R(aud_a0_cnt));
		DSPAPI_PRINT("dspDumpPCM::master_gain:0x%x, gain_0:0x%x, gain_4:0x:%x, delta_volume:0x%x,delta_ramp_pcm:0x:%x\n",HWREG_R(aud_grm_master_gain),HWREG_R(aud_grm_gain_control_0),HWREG_R(aud_grm_gain_control_4),HWREG_R(aud_grm_delta_volume),HWREG_R(aud_grm_delta_ramp_pcm));
		if (HWREG_R(aud_a0_cnt)>=16*3)
			dumpByte(nA0dramAddress+HWREG_R(aud_a0_ptr),16*3);
		//cyg_thread_delay(1);
		//GL_TaskSleep(1);
		msleep(1);
}


void dumpCodecNowID(void)
{
	DSPAPI_PRINT("dsp_inst_page0::0x%08x \n",HWREG_R(dsp_inst_page[0]));
    DSPAPI_PRINT("dsp_inst_page1::0x%08x \n",HWREG_R(dsp_inst_page[1]));
    DSPAPI_PRINT("dsp_inst_page2::0x%08x \n",HWREG_R(dsp_inst_page[2]));
    DSPAPI_PRINT("dsp_inst_page3::0x%08x \n",HWREG_R(dsp_inst_page[3]));
    DSPAPI_PRINT("dsp_inst_page4::0x%08x \n",HWREG_R(dsp_inst_page[4]));
    DSPAPI_PRINT("dsp_inst_page5::0x%08x \n",HWREG_R(dsp_inst_page[5]));
    DSPAPI_PRINT("dsp_inst_page6::0x%08x \n",HWREG_R(dsp_inst_page[6]));
    DSPAPI_PRINT("dsp_inst_page7::0x%08x \n",HWREG_R(dsp_inst_page[7]));
    DSPAPI_PRINT("dsp_data_page ::0x%08x \n",HWREG_R(dsp_data_page));


	DSPAPI_PRINT("dram_inst_page0_len::0x%08x \n",HWREG_R(dram_inst_page0_len));
    DSPAPI_PRINT("dram_inst_page1_len::0x%08x \n",HWREG_R(dram_inst_page1_len));
    DSPAPI_PRINT("dram_inst_page2_len::0x%08x \n",HWREG_R(dram_inst_page2_len));
    DSPAPI_PRINT("dram_inst_page3_len::0x%08x \n",HWREG_R(dram_inst_page3_len));
    DSPAPI_PRINT("dram_inst_page4_len::0x%08x \n",HWREG_R(dram_inst_page4_len));
    DSPAPI_PRINT("dram_inst_page5_len::0x%08x \n",HWREG_R(dram_inst_page5_len));
    DSPAPI_PRINT("dram_inst_page6_len::0x%08x \n",HWREG_R(dram_inst_page6_len));
    DSPAPI_PRINT("dram_inst_page7_len::0x%08x \n",HWREG_R(dram_inst_page7_len));
}



UINT32 DSP_GenChecksum(const UINT8 *pCode, UINT32 size)
{
    UINT32 i;
    UINT32 checksum_value = 0;

	pCode = (const UINT8 *)pCode;
    for(i=0; i<size; i++)
    {
        checksum_value ^= (pCode[i]);
        checksum_value = (checksum_value>>31) | (checksum_value<<1);
    }

    return checksum_value;

}

#ifdef DEBUG_ENABLE_AUDDRV_PRINT
//#include <stdarg.h>
void DSP_Printf(UINT32 id, const char *format,...)
{

	va_list args;
	va_start( args, format );
	/*
	   extern UINT32 gPrintFilter;

	   if (id & gPrintFilter)
	   {
	   UART_MUTEX_LOCK();
	   va_list args;
	   va_start( args, format );
	   vprintf( format, args );
	   va_end( args );
	   UART_MUTEX_UNLOCK();
	   }
	   */
	vprintk(format,args);
	va_end(args);
	//printk(KERN_ALERT "CEVADSP: " format, args);
}
#endif

#ifdef DEBUG_ENABLE_FILE_LOG
static FILE* sfpLog = NULL;
void DSP_FileLog(const char *format,...)
{
	// may log file to USB ?, but how?
	if (NULL == sfpLog)
		sfpLog = fopen("/audio/dsp.log","a");

    va_list args;
	va_start( args, format );
	vfprintf(sfpLog,format, args );
    va_end( args );

//	fclose(sfpLog);

}
#endif







