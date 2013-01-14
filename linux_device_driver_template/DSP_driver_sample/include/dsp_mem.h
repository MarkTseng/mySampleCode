/*!                                                                       
************************************************************************* 
* \file    dsp_mem.h                                          
*                                                                         
* \brief   audio dsp memory allocation
* \note    
*                                                                         
* \date    2008/12/16                                                   
*                                                                         
* \author  - jason.su        <jason.su@sunplus.com>                       
*                                                                         
************************************************************************* 
*/                                                                        
 

#ifndef __DSP_MEM_H__
#define __DSP_MEM_H__

#include <asm/mach-spv9200/types.h>

/******************************************************************************
	Defines
 *****************************************************************************/						
#define DSP_UPDATE_RISC_PARAM(offset,value)		sgDspBufInfo.pRiscParams[offset] = value


/******************************************************************************
  Memory allocation related defines
 *****************************************************************************/
//
//8600A-511-BD-SKT-A DRAMB: 0x92000000 ~ 0x96000000 
//
#define DRAM_A_BASE                 0x80000000          // 32 bit, 256M
#ifdef BOARD_8600A_511_SKT_A
#define DRAM_B_BASE                 0x92000000          // 16 bit, 64M
#else
#define DRAM_B_BASE                 0x90000000          // 16 bit, 128M
//#define DRAM_B_BASE                 0x92c60000          // 16 bit, 128M
#endif


#define DRAM_A_SIZE                 (256*1024*1024)     // 256M (total)
#ifdef BOARD_8600A_511_SKT_A
#define DRAM_B_SIZE                 (64*1024*1024)     // 128M (total)
#else
#define DRAM_B_SIZE                 (128*1024*1024)     // 128M (total)
#endif


//
// Code memory anchor should be align 8 bytes
//
#define ALIGNMENT(x,n)              ((x>>n)<<n)             	// align x with 2^n 
#define ALIGNSIZE(x,n)              (ALIGNMENT(x,n)+(1<<n))   	// align x with 2^n     
#define CODE_BANK_NUM               4
//#define DSP_CODE_MEM_SIZE           (4*1024*1024)       // 4M (1 bank = 1M)

//
// Data memory : set 10 M for now, should modified 
//

#define DSP_DRAMSIZE				(10*1024*1024) // Default 10M
#define DSP_MAX_IDM_ADDRESS         (32*1024)   // Max IDM address range


#define K_BYTE_PER_SAMPLE           3

#define K_RISC_PARAMS0              128
#define K_RISC_PARAMS1              128

#define K_DSP_PARAMS0               128
#define K_DSP_PARAMS1               128

#define K_CODEC_PARAMS              16

#define K_DRAM_SIZE_RISC_PARAMS0    (K_RISC_PARAMS0*2)
#define K_DRAM_SIZE_RISC_PARAMS1    (K_RISC_PARAMS1*2)

#define K_DRAM_SIZE_DSP_PARAMS0     (K_DSP_PARAMS0*2)
#define K_DRAM_SIZE_DSP_PARAMS1     (K_DSP_PARAMS1*2)

#define K_DRAM_SIZE_CODEC_PARAMS    (K_CODEC_PARAMS*2)


/******************************************************************************
  Risc Parameters section 0
 *****************************************************************************/
#define RISC_MAIN_MODE                        0       
#define RISC_MAIN_FIFO_CFG_DW                 1       
#define RISC_MAIN_FIFO_CFG_H                  1       
#define RISC_MAIN_FIFO_CFG_L                  2       
#define RISC_MAIN_MASTER_GAIN_DW              3       
#define RISC_MAIN_MASTER_GAIN_H               3       
#define RISC_MAIN_MASTER_GAIN_L               4       
#define RISC_MAIN_LM_GAIN                     5       
#define RISC_MAIN_RM_GAIN                     6       
#define RISC_MAIN_A0_GAIN                     7       
#define RISC_MAIN_A1_GAIN                     8       
#define RISC_MAIN_A2_GAIN                     9       
#define RISC_MAIN_A3_GAIN                     10      
#define RISC_MAIN_DISC_FORMAT                 11      
#define RISC_MAIN_SPDIF_MODE                  12      
#define RISC_MAIN_HDMI_MODE                   13      
#define RISC_MAIN_DAC_FS                      14      
#define RISC_MAIN_IEC0_FS                     15      
#define RISC_MAIN_IEC1_FS                     16      
#define RISC_MAIN_DAC_POLARITY                17      
#define RISC_MAIN_IEC0_DELAY_SAMPLE           18
#define RISC_MAIN_IEC1_DELAY_SAMPLE           19
                                                   
#define RISC_MAIN_ECHO_DELAY                  22      
#define RISC_MAIN_ECHO_GAIN                   23      
#define RISC_MAIN_MIC_SWITCH                  24      
#define RISC_MAIN_MIC0_GAIN                   25      
#define RISC_MAIN_MIC1_GAIN                   26      
#define RISC_MAIN_RISC_AD_GAIN                27      
#define RISC_MAIN_MIC_UPPER_TH                30      
#define RISC_MAIN_MIC_PRESENSITY              31      
#define RISC_MAIN_MIC_AFTERSENSITY            32      
#define RISC_MAIN_MIC_LOWER_TH                33      
                                                   
#define RISC_MAIN_KEY_SHIFT                   35      
#define RISC_MAIN_EQ_GAIN_4_0_HI              36      
#define RISC_MAIN_EQ_GAIN_4_0_LO              37      
#define RISC_MAIN_EQ_GAIN_6_5                 38      
#define RISC_MAIN_3D_SURROUND                 39      
#define RISC_MAIN_BASS_MANAGEMENT_SPK         40      
#define RISC_MAIN_BASS_MANAGEMENT_CFG         41      
#define RISC_MAIN_SPK_TIME_DELAY_FRONT        42      
#define RISC_MAIN_SPK_TIME_DELAY_SURROUND     43      
#define RISC_MAIN_SPK_TIME_DELAY_BACK         44      
#define RISC_MAIN_APP_CMODE                   45      
#define RISC_MAIN_APP_CONTROL                 46 

#define RISC_MAIN_DOWNMIX_MODE                47      
#define RISC_MAIN_2CH_MODE                    48      
#define RISC_MAIN_CDRIP_MODE                  49      
#define RISC_MAIN_LINE_ADC_MODE               50      
#define RISC_MAIN_KARAOKE_SOUND_EFFECT        51      
#define RISC_MAIN_RESERVED_52                 52      
#define RISC_MAIN_RESERVED_53                 53      
#define RISC_MAIN_KARAOKE_VOCAL_SCORE         54      
#define RISC_MAIN_DUALMPEG_DEC1_GAIN          55      
#define RISC_MAIN_DUALMPEG_DEC2_GAIN_DW       56      
#define RISC_MAIN_DUALMPEG_DEC2_GAIN_H        56      
#define RISC_MAIN_DUALMPEG_DEC2_GAIN_L        57      
#define RISC_MAIN_MIXER_SELECT                58   
#define RISC_MAIN_SPK_OUTPUT                  59

#define RISC_MAIN_MIXER_OUTPUT                  63 
#define RISC_SRS_TRUHD_CTRL                     64
#define RISC_SRS_WOW_CTRL                       65
#define RISC_SRS_TRUBASS_CTRL                   66
#define RISC_SUNSUR_CTRL						67

#define RISC_MAIN_FIFO_DEPTH_CFG        	  70      
#define RISC_MAIN_DRAM_SIZE_BSBUF_DEC1         71
#define RISC_MAIN_DRAM_SIZE_BSBUF_DEC2         72
#define RISC_MAIN_DRAM_SIZE_BSBUF_ENC          73
#define RISC_TRUVOL_PARAM0                     74
#define RISC_TRUVOL_PARAM1                     75

#define RISC_SWAPP_ID                          76
#define RISC_SWAPP_PARAM0                      77
#define RISC_SWAPP_PARAM1                      78
#define RISC_SWAPP_PARAM2                      79 

#define RISC_DEC1_PARAMS_START                 80
#define RISC_DEC1_ID                           80
#define RISC_DEC1_BSBUF_END                    93
#define RISC_DEC1_FF_RATIO                     94
#define RISC_DEC1_DBG_FRAME_CNT                95

#define RISC_DEC2_PARAMS_START                 96
#define RISC_DEC2_ID                           96
#define RISC_DEC2_BSBUF_END                    109
#define RISC_DEC2_FF_RATIO                     110    
#define RISC_DEC2_DBG_FRAME_CNT                111

#define RISC_ENC_PARAMS_START                  112
#define RISC_ENC_ID                            112
#define RISC_ENC_INPUT_MODE                    113
#define RISC_ENC_DBG_FRAME_CNT                 127

//
// individual codec parameters
//
#define RISC_DEC1_DTSHD_OutputChnMode          81
#define RISC_DEC1_DTSCD_Type                   82
#define RISC_DEC1_DDP_OutMode                  81
#define RISC_DEC1_DDP_Karaoke_Mode             82
#define RISC_DEC1_DDP_OutputChnMode            86
#define RISC_ENC_SPEED_MODE                    118

/******************************************************************************
  Risc Parameters section 1
 *****************************************************************************/
#define RISC_MAGDET_THR_DW                  0
#define RISC_MAGDET_THR_H                   0
#define RISC_MAGDET_THR_L                   1
#define RISC_NMUTE_FMA2_SAP_DW              2
#define RISC_NMUTE_FMA2_SAP_H               2
#define RISC_NMUTE_FMA2_SAP_L               3
#define RISC_NMUTE_MPX_DW                   4
#define RISC_NMUTE_MPX_H                    4
#define RISC_NMUTE_MPX_L                    5
#define RISC_NMUTE_EIAJ_DW                  6
#define RISC_NMUTE_EIAJ_H                   6
#define RISC_NMUTE_EIAJ_L                   7
#define RISC_NICAM_CFG_DW                   8                       
#define RISC_NICAM_CFG_H                    8
#define RISC_NICAM_CFG_L                    9
#define RISC_DDEP_OPTIONS1                  10
#define RISC_DDEP_CTRL_DW                   11
#define RISC_DDEP_CTRL_H                    11
#define RISC_DDEP_CTRL_L                    12

#define RISC_DEM_CFG_DW                     13
#define RISC_DEM_CFG_H                      13
#define RISC_DEM_CFG_L                      14

#define RISC_DEM_CA1_DW                     15
#define RISC_DEM_CA1_H                      15
#define RISC_DEM_CA1_L                      16

#define RISC_DEM_CA2_DW                     17
#define RISC_DEM_CA2_H                      17
#define RISC_DEM_CA2_L                      18

#define RISC_DEM_MPX_CFG_DW                 19
#define RISC_DEM_MPX_CFG_H                  19
#define RISC_DEM_MPX_CFG_L                  20

#define RISC_DEM_FMSUB_CFG_DW               21
#define RISC_DEM_FMSUB_CFG_H                21
#define RISC_DEM_FMSUB_CFG_L                22

#define RISC_DEM_OUT_CFG_DW                 23
#define RISC_DEM_OUT_CFG_H                  23
#define RISC_DEM_OUT_CFG_L                  24

#define RISC_SRC_CTRL                       25
#define RISC_LEV_ADJ_DEM_DW                 26
#define RISC_LEV_ADJ_DEM_H                  26
#define RISC_LEV_ADJ_DEM_L                  27

#define RISC_LEV_ADJ_IO                     28
#define RISC_SOU_APP_MOD_DW                 29
#define RISC_SOU_APP_MOD_H                  29
#define RISC_SOU_APP_MOD_L                  30

#define RISC_SOU_DVM_CTRL                   31
#define RISC_SOU_EFF                        32
#define RISC_MAIN_SOU_EFF                   33
#define RISC_DOL_CTRL                       34
#define RISC_MASTER_VOL                     35
#define RISC_MAIN_VOL                       36
#define RISC_SW_C_S_VOL_DW                  37
#define RISC_SW_C_S_VOL_H                   37
#define RISC_SW_C_S_VOL_L                   38
#define RISC_MAIN_TON_CTRL                  39
#define RISC_CENTER_TON_CTRL                40
#define RISC_SURR_TON_CTRL                  41
#define RISC_EQMAIN1_TON_CTRL               42
#define RISC_EQMAIN2_TON_CTRL               43
#define RISC_EQCENTER1_TON_CTRL             44
#define RISC_EQCENTER2_TON_CTRL             45
#define RISC_DD_FLID                        46
#define RISC_DD_FLID_COEF_DW                47
#define RISC_DD_FLID_COEF_H                 47
#define RISC_DD_FLID_COEF_L                 48

#define RISC_INF_DD_FLIDSTATUS_DW           49
#define RISC_INF_DD_FLIDSTATUS_H            49
#define RISC_INF_DD_FLIDSTATUS_L            50
#define RISC_SILDETECT                      51
#define RISC_MAIN_MAINOUTPUT                52
#define RISC_MAIN_AUXOUTPUT                 53
#define RISC_MAIN_AUX1_VOL                  54
#define RISC_MAIN_AUX2_VOL                  55
#define RISC_MAIN_LIPSYNC                   56
#define RISC_MAIN_PCMOUTPUT_CTRL            57
#define RISC_MAIN_IEC_RX0_FS                58
#define RISC_MAIN_APPLICATION               59

/******************************************************************************
  DSP Parameters section 0
 *****************************************************************************/
#define DSP_MAIN_ID                            0  
#define DSP_MAIN_VERSION                       1  
#define DSP_DRAM_BSBUF_DEC1                    2  
#define DSP_DRAM_BSBUF_DEC2                    3  

#define DSP_DRAM_BSBUF_ENC                     5  
#define DSP_DRAM_MAIN_STATIC                   6  
#define DSP_DRAM_SWAPP_STATIC                  7
#define DSP_DRAM_DEC1_STATIC                   8  
#define DSP_DRAM_DEC2_STATIC                   9  
#define DSP_DRAM_ENC_STATIC                    10
                                               
#define DSP_MAIN_EQ_BAND0                      11
#define DSP_MAIN_EQ_BAND1                      12 
#define DSP_MAIN_EQ_BAND2                      13 
#define DSP_MAIN_EQ_BAND3                      14 
#define DSP_MAIN_EQ_BAND4                      15 
#define DSP_MAIN_EQ_BAND5                      16 
#define DSP_MAIN_EQ_BAND6                      17 
                                               
#define DSP_MAIN_SCORE_INSTANT                 18
#define DSP_MAIN_SCORE_FINAL                   19          
#define DSP_MAIN_SCORE_MIC_ENERGY              20
#define DSP_MAIN_ENDING_CTRL_DONE              21
                                               
#define DSP_DEC1_PARAMS_START                  32
#define DSP_DEC1_ID                            32 
#define DSP_DEC1_VERSION                       33 
#define DSP_DEC1_BITRATE                       34              
#define DSP_DEC1_SAMPLE_RATE_ID                35
#define DSP_DEC1_CHAN_NUM                      36
                                               
#define DSP_DEC1_MPEG_CHAN_MODE                37	// MPEG dsp channel mode param.
#define DSP_DEC1_PCM_HDCD_FLAG                 37   // PCM dsp parameters
#define DSP_DEC1_CHANNEL_MASK                  41
#define DSP_DEC1_BIT_RATE_HW                   42
#define DSP_DEC1_BIT_RATE_LW                   43
#define DSP_DEC1_MPEG_BSType                   44
                                               
#define DSP_DEC2_PARAMS_START                  64                                       
#define DSP_DEC2_ID                            64 
#define DSP_DEC2_VERSION                       65 
#define DSP_DEC2_BITRATE                       66              
#define DSP_DEC2_SAMPLE_RATE_ID                67
#define DSP_DEC2_CHAN_NUM                      68
#define DSP_DEC2_CHANNEL_MASK                  73
#define DSP_DEC2_BIT_RATE_HW                   74
#define DSP_DEC2_BIT_RATE_LW                   75
                                               
#define DSP_ENC_PARAMS_START                   96                                       
#define DSP_ENC_ID                             96 
#define DSP_ENC_VERSION                        97 

/******************************************************************************
  DSP Parameters section 1
 *****************************************************************************/
#define DSP_INF_DEV_STA_DW                      0
#define DSP_INF_DEV_STA_H                       0
#define DSP_INF_DEV_STA_L                       1
#define DSP_INF_NIC_STA_DW                      2
#define DSP_INF_NIC_STA_H                       2
#define DSP_INF_NIC_STA_L                       3
#define DSP_INF_NIC_ADD                         4
#define DSP_INF_SRCSTATUS                       5
#define DSP_INF_LEV_MON_DW                      6
#define DSP_INF_LEV_MON_H                       6
#define DSP_INF_LEV_MON_L                       7
#define DSP_INF_MPX_LEV_DW                      8
#define DSP_INF_MPX_LEV_H                       8
#define DSP_INF_MPX_LEV_L                       9
#define DSP_INF_DC1_DW                          10
#define DSP_INF_DC1_H                           10
#define DSP_INF_DC1_L                           11
#define DSP_INF_SUBMAGN_DW                      12
#define DSP_INF_SUBMAGN_H                       12
#define DSP_INF_SUBMAGN_L                       13
#define DSP_INF_NOISELEV_DW                     14
#define DSP_INF_NOISELEV_H                      14
#define DSP_INF_NOISELEV_L                      15
#define DSP_INF_OVMADAPT_DW                     16
#define DSP_INF_OVMADAPT_H                      16
#define DSP_INF_OVMADAPT_L                      17
#define DSP_INF_REVISION_ID_DW                  18
#define DSP_INF_REVISION_ID_H                   18
#define DSP_INF_REVISION_ID_L                   19
#define DSP_INF_SAMPLERATE_DW                   20
#define DSP_INF_SAMPLERATE_H                    20
#define DSP_INF_SAMPLERATE_L                    21
#define DSP_INF_MISC1_DW                        22
#define DSP_INF_MISC1_H                         22
#define DSP_INF_MISC1_L                         23
#define DSP_INF_CLIP_MGT                        24

//                                             
// RISC_MAIN_MODE
//                                            
#define MODE_DEC1_ENABLE_BIT                   0   
#define MODE_DEC2_ENABLE_BIT                   1
#define MODE_EFF_ENABLE_BIT                    2
#define MODE_APP_ENABLE_BIT                    3
#define MODE_ENC_ENABLE_BIT                    4
#define MODE_MIC_ENABLE_BIT                    5
#define MODE_AUXIN_ENABLE_BIT                  6
#define MODE_SPDIFIN_ENABLE_BIT                7
#define MODE_SNDEFFECT_ENABLE_BIT              8
#define MODE_SIFIN_ENABLE_BIT                  9

//
// RISC_MAIN_FIFO_CFG0/RISC_MAIN_FIFO_CFG1 (32 bit)
//
#define FIFO_MAP_A0_BIT                        0       // O:LF/RF bit 
#define FIFO_MAP_A1_BIT                        1       // O:LS/RS bit 
#define FIFO_MAP_A2_BIT                        2       // O:C/LFE bit 
#define FIFO_MAP_A3_BIT                        3       // O:LB/RB bit 
#define FIFO_MAP_A4_BIT                        4       // O:LM/RM bit 
#define FIFO_MAP_A5_BIT                        5       // O:SPDIF bit 
#define FIFO_MAP_A6_BIT                        6       // O:HDMI bit 
#define FIFO_MAP_A7_BIT                        7       // O:ECHO bit 
#define FIFO_MAP_A8_BIT                        8       // O:2nd PCM
#define FIFO_MAP_A9_BIT                        9       // O:RISC(LR) bit  
      
#define FIFO_MAP_A10_BIT                       10      // I:MIC 
#define FIFO_MAP_A11_BIT                       11      // I:LINE IN 
#define FIFO_MAP_A12_BIT                       12      // I:REC_IN
#define FIFO_MAP_A13_BIT                       13      // I:SPDIF0_IN
#define FIFO_MAP_A14_BIT                       14      // I:SPDIF1_IN
#define FIFO_MAP_A15_BIT                       15      // I:reserved    
#define FIFO_MAP_A16_BIT                       16      // I:PCM0_IN
#define FIFO_MAP_A17_BIT                       17      // I:PCM1_IN
#define FIFO_MAP_A18_BIT                       18      // I:PCM2_IN
#define FIFO_MAP_A19_BIT                       19      // I:PCM3_IN
#define FIFO_MAP_A20_BIT                       20      // I:SACD0_IN
#define FIFO_MAP_A21_BIT                       21      // I:SACD1_IN
#define FIFO_MAP_A22_BIT                       22      // I:SACD2_IN


//
// RISC_MAIN_FIFO_DEPTH_CFG
//
#define FIFO_DEPTH_COMPACT         0x0
#define FIFO_DEPTH_NORMAL          0x1
#define FIFO_DEPTH_LARGE           0x2
#define FIFO_DEPTH_EXTREME         0x3


/******************************************************************************
        Data type defines
 *****************************************************************************/

typedef enum _CodeBankID
{
	CODE_BANK0 = 0,
	CODE_BANK1,		
	CODE_BANK2,
	CODE_BANK3,	
	CODE_BANK4,
	CODE_BANK5,
	CODE_BANK6,
	CODE_BANK7,	
}CodeBankID;

typedef struct _BANK_CODECID
{
	UINT8 Bank1CodecID;
	UINT8 Bank2CodecID;
	UINT8 Bank3CodecID;	
	//UINT8 Bank4CodecID;
	//UINT8 Bank5CodecID;
	//UINT8 Bank6CodecID;
	//UINT8 Bank7CodecID;
}BANK_CODECID;


typedef enum _DSPInfo
{
	// CODE BANK (IM code memory base)		
	INFO_CODE_BANK0_BASE,
	INFO_CODE_BANK0_SIZE,
	INFO_CODE_BANK1_BASE,
	INFO_CODE_BANK1_SIZE,
	INFO_CODE_BANK2_BASE,
	INFO_CODE_BANK2_SIZE,
	INFO_CODE_BANK3_BASE,	
	INFO_CODE_BANK3_SIZE,
	INFO_CODE_BANK4_BASE,	
    INFO_CODE_BANK4_SIZE,
//	INFO_CODE_BANK5_BASE,		
//  INFO_CODE_BANK5_SIZE,
//	INFO_CODE_BANK6_BASE,			
//  INFO_CODE_BANK6_SIZE,
//	INFO_CODE_BANK7_BASE,
//  INFO_CODE_BANK7_SIZE,
    
    // Data Bank (data memory)	
	INFO_DATA_BANK_BASE,
	INFO_DATA_BANK_SIZE, 
	
		
	INFO_RISC_PARAM_BASE,
	INFO_RISC_DEC1_PARAM_BASE,
	INFO_RISC_DEC2_PARAM_BASE,	
	INFO_RISC_ENC_PARAM_BASE,	
	INFO_DSP_PARAM_BASE,	
	INFO_DSP_DEC1_PARAM_BASE,		
	INFO_DSP_DEC2_PARAM_BASE,		
	INFO_DSP_ENC_PARAM_BASE,		
	
	INFO_AUD_FIFO_BASE,		
	INFO_MAIN_STATIC_BASE,	
	INFO_SWAPP_STATIC_BASE,
	INFO_DEC1_STATIC_BASE,
	INFO_DEC2_STATIC_BASE,
	INFO_ENC_STATIC_BASE,

	INFO_MAIN_STATIC_LEN,
	INFO_SWAPP_STATIC_LEN,		
	INFO_DEC1_STATIC_LEN,
	INFO_DEC2_STATIC_LEN,	
	INFO_ENC_STATIC_LEN,	
	

	INFO_DEC1_BSBUF_BASE,			
	INFO_DEC2_BSBUF_BASE,
	INFO_ENC_BSBUF_BASE,	
	INFO_DEC1_BSBUF_LEN,		
	INFO_DEC2_BSBUF_LEN,		
	INFO_ENC_BSBUF_LEN,

	INFO_BSINFO_DEC1,
	INFO_BSINFO_DEC2,
	INFO_BSINFO_ENC,
	
}DSPInfo;


typedef struct _CODEC_PARAMS
{
	UINT16 Params[16];	
}CODEC_PARAMS;



typedef struct _BsBufInfo
{
	UINT8  bsBufID;
	UINT8* pBsBufBase;
	UINT32 bsBufLen;
	UINT32 dumpPtr;
	UINT32 barrPtr;
}BsBufInfo;


/******************************************************************************
        Globals
 *****************************************************************************/ 

/******************************************************************************
	Macros
 *****************************************************************************/

/******************************************************************************
        C Funciton Prototype
 *****************************************************************************/
/*!
  \brief audio dsp system initialization\n
  \par detail description:\n
  audio dsp system initial function would initial the following:\n
  1. allocate required memory for audio dsp system \n
  2. set hardware code&data page and aud/apphwya registers\n
     Hardware totally support 8 codebanks (max),for now only 4 bankes will be enabled\n
     The usage of the codebanks will be fixed as following:\n
     Bank0: mainprogram + system related codes\n            
     Bank1: Primary decoder\n
     Bank2: Secondary decoder\n
     Bank3: Encoder\n  
  3. reset default risc parameters (may link to menu) \n\n
  
  \return DSP_ERROR_NONE: success, otherwise: failed               
  \note the link to menu default setting with default risc parameters not done yet             
  \todo Dynamically allocation according to codec selection.   
 */
//UINT32 DSP_Initial( UINT32 DSP_DRAM_BASE_Addr );
UINT32 DSP_Initial( UINT32 DSP_DRAM_BASE_Addr ,UINT16 dsp_init_type );

/*!
  \brief Get dsp internal infomation (memory layout/size...)\n
  \par detail description:\n
  Generic dsp internal information retrieve funtion\n
  \param  Info [IN]requist information 
  \return requist information in UINT32 type, may need type convertion.
  \note     
  \todo code refactoring
 */  
UINT32 DSP_GetInfo(UINT32 Info);

/*!
  \brief Get dram buffer base from dsp parameters\n
  \par detail description:\n
  Get DSP internal dram buffer layout:\n
  The dram memory allocation will be get by
  1. Risc set configurations to risc parameters
  2. While system state transition from Ready->Idle, configurations will be valid\n
  3. Retrieve internal dram memory layout from dsp parameters\n
  \param  Info [IN]requist information 
  \return DSP_ERROR_NONE: success\n
          DSP_ERROR_INIT_SYSTEM: failed
  \note dsp API layer internal used only    
  \todo 1. dsp API layer internal used only, needs code refactoring\n
        2. take care of dynamically allocation
 */   
UINT32 DSP_GetDramBufBase(void);

/*!
  \brief  reset risc parameters
  \par    detail description:\n
  \param  pParams  [IN]Pointer to risc parameters\n
          if NULL, reset to default values
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   
  \todo   Add linking to menu default values\n
 */
UINT32 DSP_WriteRiscParams0(UINT16* pParams);

/*!
  \brief  initial risc parameters "Preload"
  \par    detail description:\n
  \return void
  \note   
  \todo   Add linking to menu default values\n
 */
void DSP_InitRiscParams0_preload(void);

/*!
  \brief  initial risc parameters
  \par    detail description:\n
  \return DSP_ERROR_NONE: succeed initial risc parameters\n
          others: failed  
  \note   
  \todo   Add linking to menu default values\n
 */
UINT32 DSP_InitRiscParams0(void);


/*!
  \brief  initial dsp parameters
  \par    detail description:\n
  \return DSP_ERROR_NONE: succeed initial dsp parameters\n
          others: failed  
  \note   
 */
UINT32 DSP_InitDspParams0(void);

/*!
  \brief  write codec parameters
  \par    detail description:\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \param  pParams  [IN]Pointer to codec parameters\n
          if NULL, reset codec parameters
  \return DSP_ERROR_NONE: succeed and system is running\n
          DSP_ERROR_CODEC_PARAM: no codec parameters\n
          others: failed  
  \note   
 */
UINT32 DSP_WriteCodecParams(UINT8 codecBankID, UINT16* pParams);


/*!
  \brief  set single specific risc parameter by offset
  \par    detail description:\n
  \param  nOffset [IN]word offset to the drambase of risc parameter
  \param  nValue  [IN]word value to set (16 bit)
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   
 */
UINT32 DSP_SetRiscParam0(UINT16 nOffset,UINT16 nValue);

/*!
  \brief  get single specific risc parameter by offset
  \par    detail description:\n
  \param  nOffset [IN]word offset to the drambase of risc parameter
  \return value (16-bit)
  \note   
 */
UINT16 DSP_GetRiscParam0(UINT16 nOffset);

/*!
  \brief  Get single dsp parameter by offset
  \par    detail description:\n
  \param  nOffset [IN]word offset to the drambase of dsp parameter
  \return word value (16 bit)
  \note     
 */
UINT16 DSP_GetDspParam0(UINT16 nOffset);

/*!
  \brief  Dump risc parameters
  \par    detail description:\n
  \note     
 */
void DSP_DumpRiscParams0(void);

/*!
  \brief  Dump dsp parameters
  \par    detail description:\n
  \note     
 */
void DSP_DumpDspParams0(void);

/*!
  \brief  Dump codec parameters
  \par    detail description:\n
  \param  codecBankID [IN]initial which codecBank, allowd enum value:\n
          CODEC_BANK_DEC1:Primary decoder\n
          CODEC_BANK_DEC2:Secondary decoder\n
          CODEC_BANK_ENC :Encoder\n
  \note     
 */
void DSP_DumpCodecParams(UINT8 codecBankID);

void DSP_DumpMemmap(void);

//UINT32 DSP_SetMasterGAIN(void);

UINT32 DSP_InitDSPMailbox(void);
/*!
  \brief  initial dsp parameters for SPV9200 ATV
  \par    detail description:\n
  \param  DSP_ERROR_NONE: succeed initial dsp parameters\n
          others: failed  
  \note     
 */
UINT16 DSP_InitRiscParams1(void);

/*!
  \brief  initial dsp parameters for SPV9200 ATV
  \par    detail description:\n
  \return DSP_ERROR_NONE: succeed initial dsp parameters\n
          others: failed  
  \note   
 */
UINT16 DSP_InitDspParams1(void);

/*!
  \brief  set single specific risc parameter by offset
  \par    detail description:\n
  \param  nOffset [IN]word offset to the drambase of risc parameter
  \param  nValue  [IN]word value to set (16 bit)
  \return DSP_ERROR_NONE: succeed and system is running\n
          others: failed  
  \note   
 */
UINT16 DSP_SetRiscParam1(UINT16 nOffset,UINT16 nValue);


/*!
  \brief  get single specific risc parameter by offset on 9200
  \par    detail description:\n
  \param  nOffset [IN]word offset to the drambase of risc parameter
  \return value (16-bit)
  \note   
 */
UINT16 DSP_GetRiscParam1(UINT16 nOffset);

/*!
  \brief  Get single dsp parameter by offset on 9200
  \par    detail description:\n
  \param  nOffset [IN]word offset to the drambase of dsp parameter
  \return word value (16 bit)
  \note     
 */
UINT16 DSP_GetDspParam1(UINT16 nOffset);

/*!
  \brief  read data from SSD with bit operation.
  \par    detail description:\n
  \param  nOffset [IN]SSD DTL Address
  \param  dSSDDataBitMask [IN]SSD data mask
  \param  dSSDDataBitIndex [IN]lowest bit index of SSD data (0..15)
  \return SSD data (16 bit)
  \note     
 */
UINT16 DSP_GetSSDBit(UINT16 nOffset, UINT32 dSSDDataBitMask, UINT8 dSSDDataBitIndex);

/*!
  \brief  write data to SSD with bit operation.
  \par    detail description:\n
  \param  nOffset [IN]SSD DTL Address
  \param  dSSDDataBitMask [IN]SSD data mask
  \param  dSSDDataBitIndex [IN]lowest bit index of SSD data (0..15)
  \return SSD data (16 bit)
  \note     
 */
void DSP_SetSSDBit(UINT16 nOffset, UINT32 dSSDDataBitMask, UINT8 bSSDDataBitIndex, UINT16 dSSDData);

/*!
  \brief  get single specific risc_param1 with bit operation by offset
  \par    detail description:\n
  \param  nOffset       [IN]risc_param1 offset
  \param  dDataBitHi    [IN]the high bit of specific risc_param1[nOffset]
  \param  dDataBitLo    [IN]the low bit of specific risc_param1[nOffset]  
  \return data (unsigned 16 bit)
  \note     
 */
UINT16 DSP_GetBitMaskRiscParam1(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo);

/*!
  \brief  set single specific risc_param1 with bit operation by offset
  \par    detail description:\n
  \param  nOffset       [IN]risc_param1 offset
  \param  dDataBitHi    [IN]the high bit of specific risc_param1[nOffset]
  \param  dDataBitLo    [IN]the low bit of specific risc_param1[nOffset]  
  \param  dMaskedData   [IN]the data we want to write to risc_param1[nOffset] (high bit ~ low bit)
  \return none
  \note     
 */
void DSP_SetBitMaskRiscParam1(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo, UINT16 dMaskedData);

/*!
  \brief  get single specific risc_param0 with bit operation by offset
  \par    detail description:\n
  \param  nOffset       [IN]risc_param0 offset
  \param  dDataBitHi    [IN]the high bit of specific risc_param0[nOffset]
  \param  dDataBitLo    [IN]the low bit of specific risc_param0[nOffset]  
  \return data (unsigned 16 bit)
  \note     
 */
UINT16 DSP_GetBitMaskRiscParam0(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo);

/*!
  \brief  set single specific risc_param0 with bit operation by offset
  \par    detail description:\n
  \param  nOffset       [IN]risc_param0 offset
  \param  dDataBitHi    [IN]the high bit of specific risc_param0[nOffset]
  \param  dDataBitLo    [IN]the low bit of specific risc_param0[nOffset]  
  \param  dMaskedData   [IN]the data we want to write to risc_param0[nOffset] (high bit ~ low bit)
  \return none
  \note     
 */
void DSP_SetBitMaskRiscParam0(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo, UINT16 dMaskedData);


/*!
  \brief  get single specific signed risc_param1 with bit operation by offset
  \par    detail description:\n
  \param  nOffset       [IN]risc_param1 offset
  \param  dDataBitHi    [IN]the high bit of specific risc_param1[nOffset]
  \param  dDataBitLo    [IN]the low bit of specific risc_param1[nOffset]  
  \return data (unsigned 16 bit)
  \note     
 */
 INT16 DSP_GetSignedBitMaskRiscParam1(UINT16 nOffset, UINT16 dDataBitHi, UINT16 dDataBitLo);
#endif //__DSP_MEM_H__
