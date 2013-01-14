/******************************************************************************
 * Filename    : aud_static.c
 * 
 * Description : auddrv static functions (auddrc internal used only)
 * Language    : C Language 
 *           
 * Revision History: 
 * ------------------
 * [090312]hcsu Initial Draft
 *
 * -----------------------------------------------------------------------
 * Copyright (C) 2007-2009 Sunplus Technology Co., Ltd. 
 * All rights reserved. The information contained herein is the exclusive 
 * property of SUNPLUS TECHNOLOGY CO., LTD. and shall not be distributed, 
 * reproduced, or disclosed in whole or in part without prior written 
 * permission of SUNPLUS TECHNOLOGY CO., LTD.   
 *****************************************************************************/
#include "include/aud_global.h"
#include "include/auddrv.h"
#include "include/aud_static.h"
#include "include/audclk.h"

//#include "string.h"
#include "include/dsp_api.h"
#include "include/wave_fmt.h"

  /******************************************************************************
    Local Defines
 *****************************************************************************/ 


 /******************************************************************************
    globals
 *****************************************************************************/ 
#ifdef USE_RISC_PARSER_PARAM
extern WAVFORMATMACRO gWavFmt;
#endif

#if 1//def DUAL_MPEG_MIXER
const INT32 dB2Linear_Tab[DB2LINEAR_SCALE] = 
{
	0x7fffffff,	0x721482bf,	0x65ac8c2e,	0x5a9df7aa,
	0x50c335d3,	0x47faccef,	0x4026e73c,	0x392ced8d,
	0x32f52cfe,	0x2d6a866f,	0x287a26c4,	0x241346f5,
	0x2026f30f,	0x1ca7d767,	0x198a1357,	0x16c310e3,
	0x144960c5,	0x12149a5f,	0x101d3f2d,	0x0e5ca14c,
	0x0ccccccc,	0x0b687379,	0x0a2adad1,	0x090fcbf7,
	0x08138561,	0x0732ae17,	0x066a4a52,	0x05b7b15a,
	0x0518847f,	0x048aa70b,	0x040c3713,	0x039b8718,
	0x0337184e,	0x02dd958a,	0x028dcebb,	0x0246b4e3,
	0x0207567a,	0x01cedc3c,	0x019c8651,	0x016fa9ba,
	0x0147ae14,	0x01240b8c,	0x01044914,	0x00e7facb,
	0x00cec089,	0x00b8449b,	0x00a43aa1,	0x00925e89,
	0x008273a6,	0x007443e7,	0x00679f1b,	0x005c5a4f,
	0x00524f3b,	0x00495bc1,	0x00416179,	0x003a4549,
	0x0033ef0c,	0x002e4939,	0x002940a1,	0x0024c42c,
	0x0020c49b,	0x001d345a,	0x001a074e,	0x001732ad,
	0x0014acda,	0x00126d42,	0x00106c43,	0x000ea30d,
	0x000d0b90,	0x000ba063,	0x000a5cb5,	0x00093c3b,
	0x00083b1f,	0x000755f9,	0x000689bf,	0x0005d3ba,
	0x00053181,	0x00000000,
};

const BYTE Fade2dB_Tab[FADE_SCALE] = 
{
	0,0,1,1,1,2,2,2,
	2,3,3,3,4,4,4,5,
	5,5,5,6,6,6,7,7,
	7,8,8,8,8,9,9,9,
	10,10,10,11,11,11,11,12,
	12,12,13,13,13,14,14,14,
	14,15,15,15,16,16,16,17,
	17,17,17,18,18,18,19,19,
	19,20,20,20,20,21,21,21,
	22,22,22,23,23,23,23,24,
	24,24,25,25,25,26,26,26,
	26,27,27,27,28,28,28,29,
	29,29,29,30,30,30,31,31,
	31,32,32,32,32,33,33,33,
	34,34,34,35,35,35,35,36,
	36,36,37,37,37,38,38,38,
	38,39,39,39,40,40,40,41,
	41,41,41,42,42,42,43,43,
	43,44,44,44,44,45,45,45,
	46,46,46,47,47,47,47,48,
	48,48,49,49,49,50,50,50,
	50,51,51,51,52,52,52,53,
	53,53,53,54,54,54,55,55,
	55,56,56,56,56,57,57,57,
	58,58,58,59,59,59,59,60,
	60,60,61,61,61,62,62,62,
	62,63,63,63,64,64,64,65,
	65,65,65,66,66,66,67,67,
	67,68,68,68,68,69,69,69,
	70,70,70,71,71,71,71,72,
	72,72,73,73,73,74,74,74,
	74,75,75,75,76,76,76,77,	
};
#endif

#if defined (VOLUMN_TAB_80)
const BYTE VolTab_qae377[VOLUME_SCALE] = {
    	0x00,                               //0
        0x01, 0x02, 0x03, 0x04, 0x05,		//01--05
        0x06, 0x07, 0x08, 0x09, 0x0a, 		//06--10
        0x0b, 0x0c, 0x0d, 0x0e, 0x0f,  		//11--15
        0x10, 0x11, 0x12, 0x13, 0x14,    	//16--20
        0x15, 0x16, 0x17, 0x18, 0x19,   	//21--25
        0x1a, 0x1b, 0x1c, 0x1d, 0x1f,   	//26--30
        0x20, 0x21, 0x22, 0x23, 0x24,   	//31--25
        0x25, 0x26, 0x27, 0x28, 0x29,   	//35--40
        0x2a, 0x2b, 0x2c, 0x2d, 0x30,   	//41--45
        0x31, 0x33, 0x35, 0x36, 0x37,   	//46--50
        0x39, 0x3a, 0x3b, 0x3c, 0x3d,   	//51--55
        0x3e, 0x3f, 0x40, 0x42, 0x44,   	//56--60
        0x46, 0x48, 0x4a, 0x4c, 0x4d,   	//61--65
        0x4f, 0x50, 0x51, 0x53, 0x55,   	//66--70
        0x58, 0x5a, 0x5d, 0x62, 0x67,   	//71--75
        0x6c, 0x70, 0x74, 0x78, 0x7f,   	//76--80
	    0x80                               //81
};
#elif defined (VOLUMN_TAB_50)
const BYTE VolTab_qae377[VOLUME_SCALE] = 
{
        0x00, 0x03, 0x05, 0x08,
        0x0A, 0x0D, 0x0F, 0x12,
        0x14, 0x17, 0x1A, 0x1C,
        0x1F, 0x21, 0x24, 0x26,
        0x29, 0x2C, 0x2E, 0x31,
        0x33, 0x36, 0x38, 0x3B,
        0x3D, 0x40, 0x43, 0x45,
        0x48, 0x4A, 0x4D, 0x4F,
        0x52, 0x54, 0x57, 0x5A,
        0x5C, 0x5F, 0x61, 0x64,
        0x66, 0x69, 0x6C, 0x6E,
        0x71, 0x73, 0x76, 0x78,
        0x7B, 0x7D, 0x80,
};
#else
const BYTE VolTab_qae377[VOLUME_SCALE] = {
	    0x00,   //0
        0x02,
        0x03,
        0x04,   //3
        0x05,   //4
        0x07,   //5
        0x09,   //6
        0x0c,   //7
        0x11,   //8
        0x16,   //9
        0x1e,   //10
        0x28,   //11
        0x36,   //12
        0x47,   //13
        0x5f,   //14
        0x7f,   //15
        0x80,   //16
};
#endif //VOL_80



const UINT16  MICVolTab_qae377[MIC_GAIN_SCALE] = {
		0x0000,   // 0
        0x0fff,   // 1 
        0x1600,   // 2
        0x2000,   // 3
        0x3600,   // 4
        0x4700,   // 5
        0x5700,   // 6
        0x6400,   // 7
        0x8000,   // 8
};

/*======================================*/
/*=====  ECHO Parameter related     =====*/
/*======================================*/
//{decay,delay}
// decay : 0 ~ 127
// delay : 0 ~ 12000
  #ifdef ECHO_SPECIAL_PARA
    const UINT16 echo_para_tbl[9][2]={
            {0,0},     {76,2976},   // decay : 76/128= 0.6  , delay: 2976/44100=67ms
            {79,3648}, {81,4320},
            {84,4992}, {87,5664},
            {89,6336}, {92,7008},
            {94,7680},
    };
  #elif defined(SUPPORT_MIDI) //Jeff 20040303
    const UINT16 echo_para_tbl[9][2]={
        {0,0},      {7,12000},
        {15,13000}, {23,12000},
        {31,12000}, {39,12000},
        {47,12000}, {55,12000},
        {63,12000},
    };
  #elif defined(ECHO_SPECIAL_PARA_ORITRON_HT220)
    const UINT16 echo_para_tbl[9][2]={
            {0,0},     {76,2976},   
            {79,3648}, {81,4320},
            {84,4992}, {87,5664},
            {89,6336}, {94,7008},
            {100,7680},
    };
  #else//ECHO_SPECIAL_PARA
    const UINT16 echo_para_tbl[9][2]={
        {0,0},      {7,10000},
        {15,10000}, {23,10000},
        {31,10000}, {39,10000},
        {47,10000}, {55,10000},
        {63,10000},
    };
  #endif//ECHO_SPECIAL_PARA

void aud_InitAudParams(void)
{

	memset(&aud_param,0,sizeof(aud_param));	
	
	aud_param.coding_mode_now					= 0x0;		
	aud_param.smprate_info.smp_rate_now			= FS_DEFAULT;     
	aud_param.smprate_info.frequency_mask		= FS_MASK_192K; //192K
	aud_param.smprate_info.frequency_multiple	= 0x1;
	aud_param.volume							= 16; // volume
	aud_param.out_mode.ac3_op_mode				= Line_outmode;
	aud_param.out_mode.ac3_karaoke_mode			= OUT_NoVocal; 
	aud_param.out_mode.ac3_dyncmprs_mode		= 0;
	aud_param.DDP_LFF_Channel_Mode              = 0;
	aud_param.FIFO_Enable_Flag					= Enable;
	aud_param.DTSCD_type						= nonDTSCD;
	aud_param.encoder_bit_rate					= 0x9; // default 128Kbps
	aud_param.encoder_sample_rate				= 44100;
	aud_param.encoder_speed						= 0; // normal(1x)
	aud_param.MPEG_CRC_Check					= 0; // default 0
	aud_param.spdif_mode                        = SPDIF_OFF;
	aud_param.hdmi_mode							= HDMI_PCM;
	aud_param.mic0_gain							=0x8;   //lptan add for maintis id :94709 2010-7-6
	aud_param.mic1_gain							=0x8;
	// TruVolume setting
	aud_param.truvol_gain                       = 0x80;
	aud_param.truvol_mode                       = AUDIF_TruVol_Off;
	aud_param.PCM_CFG                           = DAC_I2S24;    
	aud_param.master_mute_glag                  = MUTE_STATUS_OFF;
	aud_param.internal_mute_flag                = MUTE_STATUS_OFF;
	aud_param.aux1_mute_glag                    = MUTE_STATUS_OFF;
	aud_param.aux2_mute_glag                    = MUTE_STATUS_OFF;
	aud_param.DDP_Converter_Mode                = DDP_Converter_Enable;
	aud_param.AVS_Info.bs_type                  = AVS_BsType_RAW;
	aud_param.AVS_Info.output_channel_mode      = AVS_Dual_mode;
	aud_param.AVS_Info.nRaw_Block               = 0x0;
    aud_param.pcm_output_phase                  = ((OUTPUT_PHASE_0<<0) | (OUTPUT_PHASE_0<<1) | (OUTPUT_PHASE_0<<2));

	aud_param.Decoder_OPT                       = INIT_OPT;
    aud_param.Decoder_RunTimeOPT                = 0x0;

    if( audHW_param.Is_xck_clk_fs_setting != 1 )
	{	// default XCK = 128FS
        audHW_param.xck_clk_fs_table[0] = AUDCLK_128XCK_04096;
        audHW_param.xck_clk_fs_table[1] = AUDCLK_128XCK_05644;
        audHW_param.xck_clk_fs_table[2] = AUDCLK_128XCK_06144;
        audHW_param.xck_clk_fs_table[3] = 0;
        audHW_param.xck_clk_fs_table[4] = 0;
        audHW_param.xck_clk_fs_table[5] = AUDCLK_128XCK_11290;
        audHW_param.xck_clk_fs_table[6] = AUDCLK_128XCK_12288;
        audHW_param.xck_clk_fs_table[7] = 0;
        audHW_param.xck_clk_fs_table[8] = 0;
        audHW_param.xck_clk_fs_table[9] = AUDCLK_128XCK_22579;
        audHW_param.xck_clk_fs_table[10] = AUDCLK_128XCK_24576;
        audHW_param.xck_clk_fs_table[11] = 0;
        audHW_param.xck_clk_fs_table[12] = 0;
		audHW_param.Is_xck_clk_fs_setting       = 1;
	}


	AUDDRV_PRINT("aud_InitAudParams::aud_param:coding_mode_now:0x%x\n",aud_param.coding_mode_now);
	AUDDRV_PRINT("aud_InitAudParams::initial me!\n");

}

void aud_SetCodecParams(UINT32 coding_mode)
{
    UINT16 codecParams[16];
	UINT16 i;

#ifdef USE_RISC_PARSER_PARAM
	union {
		UINT32 addr;
		WAVFORMATMACRO	*fmt;
	}g_WMAfmt;

	union {
		UINT32 addr;
		WAVFORMATMACRO	*fmt;
	}g_AACfmt;	

	union {
		UINT32 reserve;
		WAVFORMATMACRO	*fmt;
	}g_ADPCMfmt;
	
	union {
		UINT32 addr;
		WAVFORMATMACRO	*fmt;
	}g_WMAProfmt;
	
	union {
		UINT32 addr;
		WAVFORMATMACRO	*fmt;
	}g_RAProfmt;
	
#endif

	// Prepare codec parameters (no need to call WORD16 to translate to BE)
    for (i=0; i<K_CODEC_PARAMS; i++)
    {
        codecParams[i] = 0;
    }
	
	codecParams[15] = 0x8;			// frame counts message: 2^4 = 16 (frames)

	switch(coding_mode)
	{
	//case AUDIF_CODING_MODE_AC3:    
    case AUDIF_CODING_MODE_DDP: 
		 codecParams[0] = ID_DECODER_DDP;
		 codecParams[1] = aud_param.out_mode.ac3_op_mode; // 0:RF, 1:line mode
		 codecParams[2] = aud_param.out_mode.ac3_karaoke_mode;
		 codecParams[3] = 0; // karaoke melody: 0:enable, 1:disable
		 codecParams[4] = aud_param.out_mode.ac3_dyncmprs_mode;
		 codecParams[5] = 0; // dual mono 0: stereo
		 codecParams[6] = aud_param.output_channel_cfg;
		 codecParams[7] = aud_param.out_mode.downmix_mode;
		 codecParams[8] = aud_param.DDP_LFF_Channel_Mode; // 0: off, 1: on
		 codecParams[9] = aud_param.DDP_Converter_Mode;
         break;        
	//case AUDIF_CODING_MODE_PPCM: 
    case AUDIF_CODING_MODE_TRUEHD:
		 codecParams[0] = ID_DECODER_TRUEHD;
		 codecParams[1] = 0x0016;
         break;        
	//case AUDIF_CODING_MODE_MP1:            
	//case AUDIF_CODING_MODE_MP2:            
	//case AUDIF_CODING_MODE_MP3:            
	//case AUDIF_CODING_MODE_MP3_6CH:
    case AUDIF_CODING_MODE_MPEG:
		 codecParams[0] = ID_DECODER_MPEG;		
		 codecParams[1] = aud_param.MPEG_CRC_Check;
         break;        
	//case AUDIF_CODING_MODE_LPCM:   
    case AUDIF_CODING_MODE_PCM:
#ifdef USE_RISC_PARSER_PARAM

		g_ADPCMfmt.fmt = &gWavFmt;		
		PCMWAVEFORMAT *ADPCMfmtParam;
		ADPCMfmtParam = (PCMWAVEFORMAT *)g_ADPCMfmt.reserve;
		
		 codecParams[0] =  ID_DECODER_PCM ;				
		 switch(gWavFmt.wfx.wFormatTag)
		 {
			case WAVE_FORMAT_PCM:
		 		codecParams[1] =  5 ;
				break;
			case WAVE_FORMAT_IMA_ADPCM:
				codecParams[1] =  2 ;
				codecParams[5] = gWavFmt.wfx.nBlockAlign;
				codecParams[6] = ADPCMfmtParam->nSamplesPerBlock;
				break;
			case WAVE_FORMAT_ADPCM: //MS_ADPCM
				codecParams[1] =  1 ;
				codecParams[5] = gWavFmt.wfx.nBlockAlign;
				codecParams[6] = ADPCMfmtParam->nSamplesPerBlock;
				break;
			case WAVE_FORMAT_CD_LPCM:
				codecParams[1] =  3 ;
				break;
			case WAVE_FORMAT_DVD_LPCM:
				codecParams[1] =  0 ;
				break;
			case WAVE_FORMAT_BD_LPCM:
				codecParams[1] =  4 ;
				break;
			case WAVE_FORMAT_ULaw_LPCM:
				codecParams[1] = 6;
				break;
			case WAVE_FORMAT_BigEndian_LPCM:
				codecParams[1] = 7;
				break;					
			default:
				codecParams[1] =  0 ;
				break;				
		 }	
		 codecParams[2] =  gWavFmt.wfx.nChannels;
		 codecParams[3] =  gWavFmt.wfx.wBitsPerSample;		
		 codecParams[4] =  SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec);

	#if 0
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.wFormatTag %d\n", gWavFmt.wfx.wFormatTag);
		 AUDDRV_PRINT("[ccyang] codecParams[1] %d\n", codecParams[1]);		 
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.nChannels %d\n", gWavFmt.wfx.nChannels);
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.wBitsPerSample %d\n", gWavFmt.wfx.wBitsPerSample);			
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.nSamplesPerSec %d\n", gWavFmt.wfx.nSamplesPerSec);		 
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.nBlockAlign %d\n", gWavFmt.wfx.nBlockAlign);		 
		 AUDDRV_PRINT("[ccyang] codecParams[6] %d\n", codecParams[6]);
	#endif
#else
		 codecParams[0] =  ID_DECODER_PCM ;	
		 codecParams[1] =  4 ;
		 codecParams[2] =  6;
		 codecParams[3] =  16;
		 codecParams[4] =  K_DSP_SAMPLE_RATE_ID_48K ;
#endif  //USE_RISC_PARSER_PARAM
		 break;        
	//case AUDIF_CODING_MODE_DTS:        
	//case AUDIF_CODING_MODE_DTSCD:  
    case AUDIF_CODING_MODE_DTSHD:
		 codecParams[0] =  ID_DECODER_DTSHD;	
		 codecParams[1] =  aud_param.output_channel_cfg;
		 codecParams[2] =  aud_param.DTSCD_type;		 
         break;        
	case AUDIF_CODING_MODE_WMA:
#ifdef USE_RISC_PARSER_PARAM
		g_WMAfmt.fmt = &gWavFmt;		
		WMAUDIO2WAVEFORMAT *wmafmtParam;
		wmafmtParam = (WMAUDIO2WAVEFORMAT *)g_WMAfmt.addr;

		 codecParams[0] =  ID_DECODER_WMA;	
		 codecParams[1] = gWavFmt.wfx.nSamplesPerSec;
		 codecParams[2] = gWavFmt.wfx.nAvgBytesPerSec;
		 codecParams[3] = gWavFmt.wfx.nBlockAlign;
		 codecParams[4] = gWavFmt.wfx.wBitsPerSample;
		 codecParams[5] = gWavFmt.wfx.nChannels;
         codecParams[6] = (UINT16)((wmafmtParam->dwSamplesPerBlock)&0xffff);
         codecParams[7] = (UINT16)((wmafmtParam->dwSamplesPerBlock)>>16);
         codecParams[8] = wmafmtParam->wEncodeOptions;
#else
	 codecParams[0] =  ID_DECODER_WMA;	
	 codecParams[1] = gWmaParams.nSamplesPerSec;
         codecParams[2] = gWmaParams.nAvgBytesPerSec;
         codecParams[3] = gWmaParams.nBlockAlign;   
         codecParams[4] = gWmaParams.wBitsPerSample;
         codecParams[5] = gWmaParams.nChannels;      
         codecParams[6] = gWmaParams.dwSamplesPerBlockL; 
         codecParams[7] = gWmaParams.dwSamplesPerBlockH;
         codecParams[8] = gWmaParams.wEncodeOptions;
#endif //USE_RISC_PARSER_PARAM
         break;            
	case AUDIF_CODING_MODE_AAC:
		#ifdef USE_RISC_PARSER_PARAM
		 g_AACfmt.fmt = &gWavFmt;
		 AACWAVEFORMAT *AACfmtParam;
		 AACfmtParam = (AACWAVEFORMAT *)g_AACfmt.addr;

		 codecParams[0] =  ID_DECODER_AAC;						
		 codecParams[1] = (AACfmtParam->BSType)&0x00ff;// Data format [0=ADTS] [1=raw data] 
		 codecParams[2] = SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec);
		 codecParams[4] = aud_param.Decoder_OPT;
		 codecParams[5] = aud_param.Decoder_RunTimeOPT;         
		#else
		 codecParams[0] =  ID_DECODER_AAC;						
		#endif
         break;        
	case AUDIF_CODING_MODE_OGG:
		 codecParams[0] =  ID_DECODER_OGG;
		 codecParams[1] =  1; // ogg gose into init flow
         break;                       
	case AUDIF_CODING_MODE_DTSLBR:
		 codecParams[0] =  ID_DECODER_DTSLBR;						
         break;        
	case AUDIF_CODING_MODE_RA:           
		 g_RAProfmt.fmt = &gWavFmt;		
		 RAWAVEFORMAT *RAfmtParam;
		 RAfmtParam = (RAWAVEFORMAT *)g_RAProfmt.addr;

		 codecParams[0] =  ID_DECODER_RA;
		 codecParams[1] =  RAfmtParam->nSamples;
		 codecParams[2] =  RAfmtParam->nFrameBits;
		 codecParams[3] =  gWavFmt.wfx.nSamplesPerSec;
		 codecParams[4] =  gWavFmt.wfx.nChannels;
		 codecParams[5] =  RAfmtParam->nRegions;
		 codecParams[6] =  RAfmtParam->cplStart;
		 codecParams[7] =  RAfmtParam->cplQbits;
		 codecParams[15] = 0x8;
	 	 break; 
		 
	case AUDIF_CODING_MODE_EMU:          
		 codecParams[0] =  ID_DECODER_EMU;											
	 	 codecParams[1] =  K_EMU_OUTPUT_PCM_BLOCK_SIZE;					// PCM 64*n
		 codecParams[2] =  K_EMU_OUTPUT_IEC0_BLOCK_SIZE;				// SPDIF 64*n
		 codecParams[3] =  K_EMU_OUTPUT_IEC1_BLOCK_SIZE;				// HDMI 64*n
		 codecParams[4] =  K_EMU_OUTPUT_PCM_SAMPLE_RATE_ID;		// PCM sample rate ID
		 codecParams[5] =  K_EMU_OUTPUT_IEC0_SAMPLE_RATE_ID;		// SPDIF sample rate ID		
		 codecParams[6] =  K_EMU_OUTPUT_IEC1_SAMPLE_RATE_ID;		// HDMI sample rate ID
#ifdef USE_RISC_PARSER_PARAM
		 codecParams[7] =  aud_param.testtone;				// Ouput Mode
#else
		 codecParams[7] =  K_EMU_OUTPUT_MODE;				// Ouput Mode
#endif
		 codecParams[15] = 8;			// frame counts message: 2^4 = 16 (frames)
	 	 break;
	case AUDIF_CODING_MODE_WMAPRO:       
		 g_WMAProfmt.fmt = &gWavFmt;		
		 WMAPROWAVEFORMAT *WMAProfmtParam;
		 WMAProfmtParam = (WMAPROWAVEFORMAT*)g_WMAProfmt.addr;
		
		 codecParams[0] =  ID_DECODER_WMAPRO;														
		 codecParams[1] =  SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec);
		 codecParams[2] =  (UINT16)((gWavFmt.wfx.nAvgBytesPerSec) >> 16);
		 codecParams[3] =  (UINT16)((gWavFmt.wfx.nAvgBytesPerSec) & 0xffff);
		 codecParams[4] =  0;
		 codecParams[5] =  gWavFmt.wfx.nBlockAlign;
		 codecParams[6] =  gWavFmt.wfx.wBitsPerSample;
		 codecParams[7] =  gWavFmt.wfx.nChannels;
		 codecParams[8] =  WMAProfmtParam->nEncodeOpt;
		 codecParams[9] =  WMAProfmtParam->nChannelMask;
		 codecParams[10] =  gWavFmt.wfx.wFormatTag;		 
	 	 break;       

	case AUDIF_CODING_MODE_FLAC:
		 codecParams[0] =  ID_DECODER_FLAC;																
 		 break;
	case AUDIF_CODING_MODE_AMR:
		 codecParams[0] =  ID_DECODER_AMR;																
 		 break;

	case AUDIF_CODING_MODE_MIDI:
		 codecParams[0] =  ID_DECODER_MIDI;																
 		 break;		 

	case AUDIF_CODING_MODE_APE:
		 codecParams[0] =  ID_DECODER_APE;																
 		 break;		 		 

	case AUDIF_CODING_MODE_AVS:
		 codecParams[0] =  ID_DECODER_AVS;
		 codecParams[1] =  aud_param.AVS_Info.bs_type;
		 codecParams[2] =  0x4;
		 codecParams[3] =  aud_param.AVS_Info.output_channel_mode;
		 codecParams[4] =  aud_param.AVS_Info.nRaw_Block;
 		 break;
		 
	default:
		 AUDDRV_PRINT("aud_SetCodecParams::unknown coding mode:0x%x!\n",coding_mode);
		 codecParams[0] =  0;														
         break;       
		
	}

	DSP_WriteCodecParams(CODEC_BANK_DEC1, codecParams);
}

//
// temp code to download dec2
//
void aud_SetDec2CodecParams(UINT32 coding_mode)
{
    UINT16 codecParams[16];
	UINT16 i;

	// Prepare codec parameters (no need to call WORD16 to translate to BE)
    for (i=0; i<K_CODEC_PARAMS; i++)
    {
        codecParams[i] = 0;
    }
	
	codecParams[15] = 0x4;			// frame counts message: 2^4 = 16 (frames)

	switch(coding_mode)
	{
	//case AUDIF_CODING_MODE_AC3:    
    case AUDIF_CODING_MODE_DDP: 
		 codecParams[0] = ID_DECODER_DDP;
		 codecParams[15] = 10;
         break;        
	//case AUDIF_CODING_MODE_PPCM: 
    case AUDIF_CODING_MODE_TRUEHD:
		 codecParams[0] = ID_DECODER_TRUEHD;
		 codecParams[1] = 0x0016;
         break;        
	//case AUDIF_CODING_MODE_MP1:            
	//case AUDIF_CODING_MODE_MP2:            
	//case AUDIF_CODING_MODE_MP3:            
	//case AUDIF_CODING_MODE_MP3_6CH:
    case AUDIF_CODING_MODE_MPEG:
		 codecParams[0] = ID_DECODER_MPEG;		
         break;        
	//case AUDIF_CODING_MODE_LPCM:   
    case AUDIF_CODING_MODE_PCM:
		 codecParams[0] =  ID_DECODER_PCM ;				
         break;        
	//case AUDIF_CODING_MODE_DTS:        
	//case AUDIF_CODING_MODE_DTSCD:  
    case AUDIF_CODING_MODE_DTSHD:
		 codecParams[0] =  ID_DECODER_DTSHD;
//		 codecParams[15] = 0;
         break;        
	case AUDIF_CODING_MODE_WMA:
		 codecParams[0] =  ID_DECODER_WMA;	
#ifndef USE_RISC_PARSER_PARAM		 
	     codecParams[1] = gWmaParams.nSamplesPerSec;
         codecParams[2] = gWmaParams.nAvgBytesPerSec;
         codecParams[3] = gWmaParams.nBlockAlign;   
         codecParams[4] = gWmaParams.wBitsPerSample;
         codecParams[5] = gWmaParams.nChannels;      
         codecParams[6] = gWmaParams.dwSamplesPerBlockL; 
         codecParams[7] = gWmaParams.dwSamplesPerBlockH;
         codecParams[8] = gWmaParams.wEncodeOptions;		 
#endif
         break;            
	case AUDIF_CODING_MODE_AAC:
		 codecParams[0] =  ID_DECODER_AAC;						
         break;        
	case AUDIF_CODING_MODE_OGG:
		 codecParams[0] =  ID_DECODER_OGG;						
         break;                       
	case AUDIF_CODING_MODE_DTSLBR:
		 codecParams[0] =  ID_DECODER_DTSLBR;						
         break;
	case AUDIF_CODING_MODE_RA:           
		 codecParams[0] =  ID_DECODER_RA;								
         break;        
	case AUDIF_CODING_MODE_EMU:          
		 codecParams[0] =  ID_DECODER_EMU;												
	 	 codecParams[1] =  1;					// PCM 64*n
		 codecParams[2] =  1;					// SPDIF 64*n
		 codecParams[3] =  1;					// HDMI 64*n
		 codecParams[4] =  0x10;				// PCM sample rate ID
		 codecParams[5] =  0x10;				// SPDIF sample rate ID		
		 codecParams[6] =  0x10;				// HDMI sample rate ID			 		 
 		 codecParams[7] =  0x0;					// Ouput Mode
         break;       
	case AUDIF_CODING_MODE_WMAPRO:       
		 codecParams[0] =  ID_DECODER_WMAPRO;														
         break;       
	case AUDIF_CODING_MODE_FLAC:
		 codecParams[0] =  ID_DECODER_FLAC;																
 		 break;

	case AUDIF_CODING_MODE_AMR:
		 codecParams[0] =  ID_DECODER_AMR;																
 		 break;

	case AUDIF_CODING_MODE_MIDI:
		 codecParams[0] =  ID_DECODER_MIDI;																
 		 break;		 

	case AUDIF_CODING_MODE_APE:
		 codecParams[0] =  ID_DECODER_APE;																
 		 break;		 	

	default:
		 AUDDRV_PRINT("aud_SetDec2CodecParams::unknown coding mode:0x%x!\n",coding_mode);
		 codecParams[0] =  0;														
         break;       
		
	}

	DSP_WriteCodecParams(CODEC_BANK_DEC2, codecParams);
}



void aud_ResetCodecPara(UINT32 coding_mode)
{
 //  Reset codec parameters when coding_mode == coding_mode_now
 //  DSP state change from STOP->PLAY 
	
	AUDDRV_PRINT("aud_ResetCodecPara::check me!\n");	

	aud_SetCodecParams(coding_mode);	

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);	
	
	DSP_SetCodecStop(CODEC_BANK_DEC1);		
	DSP_SetCodecPlay(CODEC_BANK_DEC1);	
}

UINT32 SampleRate_Mapping( UINT32 smp_in )
{
	UINT32 smp = 48000;

	switch(smp_in)
	{
		case 8000-500 ... 8000+500:
			smp = K_DSP_SAMPLE_RATE_ID_8K;
			break;
		case 11025-500 ... 11025+450:
			smp = K_DSP_SAMPLE_RATE_ID_11K;
			break;
		case 12000-500 ... 12000+500:
			smp = K_DSP_SAMPLE_RATE_ID_12K;
			break;			
		case 16000-500 ... 16000+500:
			smp = K_DSP_SAMPLE_RATE_ID_16K;
			break;
		case 22050-500 ... 22050+500:
			smp = K_DSP_SAMPLE_RATE_ID_22K;
			break;		
		case 24000-500 ... 24000+500:
			smp = K_DSP_SAMPLE_RATE_ID_24K;
			break;	
		case 32000-500 ... 32000+500:
			smp = K_DSP_SAMPLE_RATE_ID_32K;
			break;
		case 44100-500 ... 44100+500:
			smp = K_DSP_SAMPLE_RATE_ID_44K;
			break;
		case 48000-500 ... 48000+500:
			smp = K_DSP_SAMPLE_RATE_ID_48K;
			break;
		case 64000-500 ... 64000+500:
			smp = K_DSP_SAMPLE_RATE_ID_64K;
			break;			
		case 88200-500 ... 88200+500:
			smp = K_DSP_SAMPLE_RATE_ID_88K;
			break;			
		case 96000-500 ... 96000+500:
			smp = K_DSP_SAMPLE_RATE_ID_96K;
			break;
		case 128000-500 ... 128000+500:
			smp = K_DSP_SAMPLE_RATE_ID_128K;
			break;			
		case 176400-500 ... 176400+500:
			smp = K_DSP_SAMPLE_RATE_ID_176K;
			break;
		case 192000-500 ... 192000+500:
			smp = K_DSP_SAMPLE_RATE_ID_192K;
			break;			
		default:
			smp = K_DSP_SAMPLE_RATE_ID_UNKNOWN;
			break;
	}
	return smp;
}


#if defined(SCALER_6500)||defined(HDMI_SOC)||defined(HDMI_13)
//Steven 2008/4/22 , turn off  audio path of HDMI
static void Disable_HDMI(void)
{
  BYTE HDMI_data __UNUSED; // fix compiler warning by jonthan.hsu 12-08-2008

#ifdef HDMI_SOC//FOR 8203H
    if (HPD_State()==1)
    {
    //2008-10-28(2),fine-tune hdmi audio noise,Eric Lin,merge HDMI code from LG-8203A V1.32    	
    write_hdmi_reg(0,2,0,0x02); //0x72,0x05,HDMI Audio FIFO Reset
    delay_srv_10ms(5); //wait the HDMI FIFO is cleared 
    
    //2008-10-28(3),mark it,fine-tune hdmi audio noise,Eric Lin,merge HDMI code from LG-8203A V1.32
    //disable audio
    //HDMI_data = read_hdmi_reg(4,10,1);
    //HDMI_data = HDMI_data & 0xfd;//bit1=1
    //write_hdmi_reg(4,10,1,HDMI_data);

    //2008-10-28(4),mark it,fine-tune hdmi audio noise,Eric Lin,merge HDMI code from LG-8203A V1.32
    //disable packet clock
    //HDMI_data = read_hdmi_reg(4,0,0);
    //HDMI_data = HDMI_data & 0xfd;//bit1=1
    //write_hdmi_reg(4,0,0,HDMI_data);    
	
    AUDDRV_PRINT("AUDDRV : Disable_HDMI for HDMI_SOC\n");

    }
#elif defined(HDMI_13)//FOR 8203R
    if (HPD_State()==1)
    {
    //mute audio
    HDMI_data=read_hdmi13_reg(0x72,0x0d);
    HDMI_data=HDMI_data | 0x02; //bit1=1
    write_hdmi13_reg(0x72,0x0d,HDMI_data);

/*disable audio may cause pop noise
    //disable audio
    HDMI_data=read_hdmi13_reg(0x7a,0x14);
    HDMI_data=HDMI_data & 0xfc; //bit0,1=0
    write_hdmi13_reg(0x7a,0x14,HDMI_data);
*/

    //disable packet clock
    HDMI_data=read_hdmi13_reg(0x7a,0x01);
    HDMI_data=HDMI_data & 0xfd;
    write_hdmi13_reg(0x7a,0x01,HDMI_data);

/*reset audio fifo may cause pop noise
    //audio fifo reset
    HDMI_data=read_hdmi13_reg(0x72,0x05);
    HDMI_data=HDMI_data | 0x02; //bit1=1
    write_hdmi13_reg(0x72,0x05,HDMI_data);
*/    

    AUDDRV_PRINT("AUDDRV : Disable_HDMI for HDMI_13\n");

}

#elif defined(SCALER_6500)//FOR 6500
    if (HPD_State()==1)
    {
    //mute audio
    HDMI_data=read_i2c(0x72,0x0d);
    HDMI_data=HDMI_data | 0x02; //bit1=1
    write_i2c(0x72,0x0d,HDMI_data);

/*disable audio may cause pop noise
    //disable audio
    HDMI_data=read_i2c(0x7a,0x14);
    HDMI_data=HDMI_data & 0xfc; //bit0,1=0
    write_i2c(0x7a,0x14,HDMI_data);
*/

    //disable packet clock
    HDMI_data=read_i2c(0x7a,0x01);
    HDMI_data=HDMI_data & 0xfd;
    write_i2c(0x7a,0x01,HDMI_data);

/*reset audio fifo may cause pop noise
    //audio fifo reset
    HDMI_data=read_i2c(0x72,0x05);
    HDMI_data=HDMI_data | 0x02; //bit1=1
    write_i2c(0x72,0x05,HDMI_data);
*/    
    AUDDRV_PRINT("AUDDRV : Disable_HDMI for 6500\n");
}
#endif
}


//Steven 2008/4/22 , turn on  audio path of HDMI
static void Enable_HDMI(void)
{
  BYTE HDMI_data __UNUSED; // fix compiler warning by jonthan.hsu 12-08-2008

#ifdef HDMI_SOC//FOR 8203H
    if (HPD_State()==1)
    {
    //2008-10-28(5),mark it,fine-tune hdmi audio noise,Eric Lin,merge HDMI code from LG-8203A V1.32        
    write_hdmi_reg(0,2,0,0x00); //(0x72,0x05)HDMI Audio FIFO normal work;    	
    //2008-10-28(6),mark it,fine-tune hdmi audio noise,Eric Lin,merge HDMI code from LG-8203A V1.32    
    //enable packet clock
    //HDMI_data = read_hdmi_reg(4,0,0);
    //HDMI_data= HDMI_data | 0x02;
    //write_hdmi_reg(4,0,0,HDMI_data);    

    //2008-10-28(7),mark it,fine-tune hdmi audio noise,Eric Lin,merge HDMI code from LG-8203A V1.32
    //enable audio
    //HDMI_data = read_hdmi_reg(4,10,1);
    //HDMI_data= HDMI_data | 0x02;
    //write_hdmi_reg(4,10,1,HDMI_data);

    AUDDRV_PRINT("AUDDRV : Enable_HDMI for HDMI_SOC\n");
	
    }
#elif defined(HDMI_13)//FOR 8203R
    if (HPD_State()==1)
    {
/*    
    //audio fifo reset diable
    HDMI_data=read_hdmi13_reg(0x72,0x05);
    HDMI_data=HDMI_data & 0xfd; //bit1=0
    write_hdmi13_reg(0x72,0x05,HDMI_data);
*/    

    //enable packet clock
    HDMI_data=read_hdmi13_reg(0x7a,0x01);
    HDMI_data= HDMI_data | 0x02;
    write_hdmi13_reg(0x7a,0x01,HDMI_data);	

/*
    //enable audio
    HDMI_data=read_hdmi13_reg(0x7a,0x14);
    HDMI_data=HDMI_data | 0x03; //bit0,1=1 
    write_hdmi13_reg(0x7a,0x14,HDMI_data);
*/

    //demute audio 
    HDMI_data=read_hdmi13_reg(0x72,0x0d);
    HDMI_data=HDMI_data & 0xfd; //bit1=0
    write_hdmi13_reg(0x72,0x0d,HDMI_data);
	
    AUDDRV_PRINT("AUDDRV : Enable_HDMI for HDMI_13\n");

    }
    
#elif defined(SCALER_6500)//FOR ,6500
    if (HPD_State()==1)
    {
/*    
    //audio fifo reset diable
    HDMI_data=read_i2c(0x72,0x05);
    HDMI_data=HDMI_data & 0xfd; //bit1=0
    write_i2c(0x72,0x05,HDMI_data);
*/    

    //enable packet clock
    HDMI_data=read_i2c(0x7a,0x01);
    HDMI_data= HDMI_data | 0x02;
    write_i2c(0x7a,0x01,HDMI_data);	

/*
    //enable audio
    HDMI_data=read_i2c(0x7a,0x14);
    HDMI_data=HDMI_data | 0x03; //bit0,1=1 
    write_i2c(0x7a,0x14,HDMI_data);
*/

    //demute audio 
    HDMI_data=read_i2c(0x72,0x0d);
    HDMI_data=HDMI_data & 0xfd; //bit1=0
    write_i2c(0x72,0x0d,HDMI_data);

    AUDDRV_PRINT("AUDDRV : Enable_HDMI for 6500\n");
    }    
#endif

}
#else
void Disable_HDMI(void) 
{
	AUDDRV_PRINT("Disable_HDMI : empty, fix me!\n");
}
void Enable_HDMI(void) 
{
    AUDDRV_PRINT("Enable_HDMI :  empty, fix me!\n");
}
#endif 

