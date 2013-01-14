/*!
 *************************************************************************
 * \file    auddrv.h
 *
 * \brief   auddrv header file
 *
 *
 * \date    2009/03/12
 *
 * \author  - jason.su        <jason.su@sunplus.com>
 *************************************************************************
 */


//#include "types.h"

#ifndef __AUDDRV_H__
#define __AUDDRV_H__

#include "wave_fmt.h"
#include "dsp_mem.h"
#include "dsp_api.h"
///#include "config_sys.h"

/******************************************************************************
	Defines
 *****************************************************************************/

#ifdef AUDDRV_GLOBAL_HERE
	#undef	EXTERN
	#define	EXTERN
#else
	#undef	EXTERN
	#define	EXTERN extern
#endif

#define AUDIO_MUTE_BY_PLAT_LAYER_REG
//#define SUPPORT_NEO6          // joylee: use this define to test NEO6 API

//----- Sample Rate -----//
#define FS_8K                   0x5000
#define FS_11K                  0x6000
#define FS_12K                  0x7000
#define FS_16K                  0x1000
#define FS_22K                  0x2000
#define FS_24K                  0x4000
#define FS_32K                  0x0001
#define FS_44K                  0x0002
#define FS_48K                  0x0004
#define FS_64K                  0x0010
#define FS_88K                  0x0020
#define FS_96K                  0x0040
#define FS_128K                 0x0100
#define FS_176K                 0x0200
#define FS_192K                 0x0400
#define FS_MAX                  0x8000

#define FS_DEFAULT              FS_48K

#define FS_MASK_48K             0x0007
#define FS_MASK_96K             0x0067
#define FS_MASK_192K            0x0667

//----- Switch Control -----//
#define STATUS_OFF                  0
#define STATUS_ON                   1

//----- MIC Support Setting related -----//
#define MIC_NOT_SUPPORT             0
#define MIC_SUPPORT                 1

//----- SPDIF Setting related -----//
#define SPDIF_OFF                   0
#define SPDIF_PCM                   1
#define SPDIF_RAW                   2
#define SPDIF_REENC                 3
#define SPDIF_IECRX                 4   //dylan : spdif bypass mode

//----- HDMI RAW support -----//
#define HDMI_OFF                    0
#define HDMI_PCM                    1
#define HDMI_RAW                    2
#define HDMI_REENC                  3
#define HDMI_RAWHD                  4
#define HDMI_PCM8CH                 5
//#define HDMI_MUTE                 3 // hcsu::added?

//cmhuang: 2009.12.14 HDMI setting flow
#define HDMI_RAWMODE_NONE           0
#define HDMI_RAWMODE_LEGACY         1
#define HDMI_RAWMODE_HD             2
#define HDMI_RAWMODE_HDHBR          3

//----- SPDIF In format -----//
#define SPDIFIN_PCM_MODE            0
#define SPDIFIN_AC3_MODE            1
#define SPDIFIN_DTS_MODE            2
#define SPDIFIN_DTSCD16_MODE        3
#define SPDIFIN_DTSCD14_MODE        4
#define SPDIFIN_NO_SIGNAL_MODE      7

//----- ProLogicII related -----//
//initial setting
#define PROLOGICII_MOVIE_DSP_INIT   0x80670730 
#define PROLOGICII_MUSIC_DSP_INIT   0x80470730 
#define PROLOGICII_OFF_DSP_INIT     0x00670730 

//command set : PLII enable
#define PLII_OFF                    0x10
#define PLII_ON                     0x11

//command set : decoder mode select
#define PLII_MODE_EMU               0x20
#define PLII_MODE_VIRTUAL           0x21
#define PLII_MODE_MUSIC             0x22
#define PLII_MODE_MOVIE             0x23
#define PLII_MODE_MATRIX            0x24
#define PLII_MODE_CUSTOM            0x27

//command set : output channel configuration
#define PLII_CH_3_0                 0x33
#define PLII_CH_2_2                 0x36
#define PLII_CH_3_2                 0x37

//command set : dimension control settings
//  0x40[-3]~0x46[+3], only for Music Mode
//  0x40(most surround) -> 0x43(neutral setting) -> 0x46(most center)
//  8203X DSP : 
//  0x40(most surround) -> 0x43(neutral setting) -> 0x46(most center)
#define PLII_DIMENSION              0x40
#define PLII_DIMENSION_MAX          0xA
#define PLII_DIMENSION_NEUTRAL      0x07
#define PLII_DIMENSION_MIN          0x04

//command set : center width control setting
//  0x50[0 degree]~0x57[90 degree], only for Music Mode
//  0x50(No center width spreading) -> 0x57(Phantom Center)
#define PLII_CENTER_WIDTH           0x50
#define PLII_CENTER_WIDTH_MAX       0x07
#define PLII_CENTER_WIDTH_MIN       0x00

//command set : panorama mode setting
#define PLII_PANORAMA_OFF           0x60
#define PLII_PANORAMA_ON            0x61

//function mask
#define PLII_FUNC_ENABLE            0X10
#define PLII_FUNC_MODE              0x20
#define PLII_FUNC_CHANNEL           0x30
#define PLII_FUNC_DIMENSION         0x40
#define PLII_FUNC_CENTER_WIDTH      0x50
#define PLII_FUNC_PANORAMA          0x60
#define PLII_FUNC_MASK              0Xf0


//operation bit
#define PLII_ENABLE_BIT             31
#define PLII_MODE_BIT               21
#define PLII_CHANNEL_BIT            16
#define PLII_DIMENSION_BIT          8
#define PLII_CENTER_WIDTH_BIT       4
#define PLII_PANORAMA_BIT           3

//----- Mute Status -----//
#define MUTE_STATUS_ON              1
#define MUTE_STATUS_OFF             0

//----- Ramp Status -----//
#define RAMP_STATUS_ON              1
#define RAMP_STATUS_OFF             0
//----- Equalizer(EQ) command set -----//
//eq display command
#define EQ_SPECTRUM_DISPLAY         0xf

//eq command type
#define EQ_SATNDARD_TYPE            0x00
#define EQ_EFFECT_TYPE              0x01
#define EQ_DISPLAY_TYPE             0x02

//----- Equalizer(EQ) band number/type -----//
#ifdef SUPPORT_MULTI_CHANNEL_5_BAND_EQ
#define EQ_BAND_MAX                 5
#else
#define EQ_BAND_MAX                 7
#endif
//#define EQ_TYPE_MAX               5	
#define EQ_TYPE_MAX                 7 //hcsu:7 or 5?? (temp fix for build error)
#define EQ_MIN_LEVEL                1
#define EQ_MAX_LEVEL                25

//----- LPCM decoder mode -----//
#define	LPCM_MODE_AOB               0x00
#define	LPCM_MODE_VOB               0x20
#define	LPCM_MODE_WAVE              0x40
#define	LPCM_MODE_CDDA              0x60
#define	LPCM_MODE_ADPCM             0x80 //add by Roger 06-04-14 for support ASF IMA ADPCM
#define	LPCM_MODE_MASK              0xE0

//----- AAC decoder bitsream format -----//
#define AAC_MODE_RAW                0x00
#define AAC_MODE_ADTS               0x01

//----- CDDA format -----//
#define NO_HEADER                   0xff
#define CDDA_HEADER                 0
#define DTS_HEADER14                1
#define DTS_HEADER16                2
#define AC3_HEADER                  3

//----- CODING_MODE mapping to codecID -----//
#define CODING_MODE_SYSTEM          ID_MAIN 			   // 0x00     
#define CODING_MODE_PCM	            ID_DECODER_PCM		   // 0x01
#define CODING_MODE_LPCM            ID_DECODER_PCM  
#define CODING_MODE_SPDIFINCD       ID_DECODER_PCM             
#define CODING_MODE_DDP	            ID_DECODER_DDP		   // 0x02
#define CODING_MODE_AC3             ID_DECODER_DDP  
#define CODING_MODE_SPDIFINAC3      ID_DECODER_DDP         
#define CODING_MODE_TRUEHD          ID_DECODER_TRUEHD	   // 0x03
#define CODING_MODE_PPCM            ID_DECODER_TRUEHD   
#define CODING_MODE_DTSLBR          ID_DECODER_DTSLBR	   // 0x04
#define CODING_MODE_DTSHD           ID_DECODER_DTSHD	   // 0x05
#define CODING_MODE_DTS             ID_DECODER_DTSHD
#define CODING_MODE_DTSCD           ID_DECODER_DTSHD      
#define CODING_MODE_SPDIFINDTS      ID_DECODER_DTSHD   
#define CODING_MODE_MPEG            ID_DECODER_MPEG		   // 0x06
#define CODING_MODE_MP1             ID_DECODER_MPEG       
#define CODING_MODE_MP2             ID_DECODER_MPEG    
#define CODING_MODE_MP3             ID_DECODER_MPEG    
#define CODING_MODE_MP3_6CH         ID_DECODER_MPEG    
#define CODING_MODE_AAC             ID_DECODER_AAC		   // 0x07  
#define CODING_MODE_WMA             ID_DECODER_WMA		   // 0x08
#define CODING_MODE_RA              ID_DECODER_RA		   // 0x09
#define CODING_MODE_OGG             ID_DECODER_OGG		   // 0x0a
#define CODING_MODE_EMU             ID_DECODER_EMU		   // 0x0b
#define CODING_MODE_WMAPRO          ID_DECODER_WMAPRO      // 0x0c
#define CODING_MODE_FLAC            ID_DECODER_FLAC        // 0x0d
#define CODING_MODE_AMR             ID_DECODER_AMR         // 0x0e              
#define CODING_MODE_MIDI			ID_DECODER_MIDI        // 0x0f
#define CODING_MODE_APE  			ID_DECODER_APE         // 0x13
#define CODING_MODE_ALAC            ID_DECODER_ALAC        // 0x14
#define CODING_MODE_AVS  			ID_DECODER_AVS         // 0x15

#define CODING_MODE_MPEG_ENC        ID_ENCODER_MPEG		   // 0x10(not ready)
#define CODING_MODE_AC3_ENC         ID_ENCODER_AC3		   // 0x11(not ready)
#define CODING_MODE_DTS_ENC         ID_ENCODER_DTS		   // 0x12(not ready)

// unsupported codecs
//#define CODING_MODE_JPG                           
//#define CODING_MODE_SDDS                
//#define CODING_MODE_MIDI                
//#define CODING_MODE_NES                 
//#define CODING_MODE_SPDIF                
//#define CODING_MODE_TUNER               
//#define CODING_MODE_NOISE               
//#define CODING_MODE_CD_MP3_ENCODER			
//#define CODING_MODE_MP3_MIX             
//#define CODING_MODE_VCD_MIC           

#define  AUDIF_CODING_MODE_NONE			0  //tyc1090
#define  AUDIF_CODING_MODE_SYSTEM		(1<<CODING_MODE_SYSTEM     )    // 0x00000001(ID:0x00)  
#define  AUDIF_CODING_MODE_PCM			(1<<CODING_MODE_PCM	     )    // 0x00000002(ID:0x01)
#define  AUDIF_CODING_MODE_LPCM			(1<<CODING_MODE_LPCM       )    // 0x00000002(ID:0x01)
#define  AUDIF_CODING_MODE_SPDIFINCD	(1<<CODING_MODE_SPDIFINCD  )    // 0x00000002(ID:0x01)
#define  AUDIF_CODING_MODE_DDP			(1<<CODING_MODE_DDP	     )    // 0x00000004(ID:0x02)
#define  AUDIF_CODING_MODE_AC3			(1<<CODING_MODE_AC3        )    // 0x00000004(ID:0x02)
#define  AUDIF_CODING_MODE_SPDIFINAC3	(1<<CODING_MODE_SPDIFINAC3 )    // 0x00000004(ID:0x02)
#define  AUDIF_CODING_MODE_TRUEHD		(1<<CODING_MODE_TRUEHD     )    // 0x00000008(ID:0x03)
#define  AUDIF_CODING_MODE_PPCM			(1<<CODING_MODE_PPCM       )    // 0x00000008(ID:0x03)
#define  AUDIF_CODING_MODE_DTSLBR		(1<<CODING_MODE_DTSLBR     )    // 0x00000010(ID:0x04)
#define  AUDIF_CODING_MODE_DTSHD		(1<<CODING_MODE_DTSHD      )    // 0x00000020(ID:0x05)
#define  AUDIF_CODING_MODE_DTS			(1<<CODING_MODE_DTS        )    // 0x00000020(ID:0x05)
#define  AUDIF_CODING_MODE_DTSCD		(1<<CODING_MODE_DTSCD      )    // 0x00000020(ID:0x05)
#define  AUDIF_CODING_MODE_SPDIFINDTS	(1<<CODING_MODE_SPDIFINDTS )    // 0x00000020(ID:0x05)
#define  AUDIF_CODING_MODE_MPEG			(1<<CODING_MODE_MPEG       )    // 0x00000040(ID:0x06)
#define  AUDIF_CODING_MODE_MP1			(1<<CODING_MODE_MP1        )    // 0x00000040(ID:0x06)
#define  AUDIF_CODING_MODE_MP2			(1<<CODING_MODE_MP2        )    // 0x00000040(ID:0x06)
#define  AUDIF_CODING_MODE_MP3			(1<<CODING_MODE_MP3        )    // 0x00000040(ID:0x06)
#define  AUDIF_CODING_MODE_MP3_6CH		(1<<CODING_MODE_MP3_6CH    )    // 0x00000040(ID:0x06)
#define  AUDIF_CODING_MODE_AAC			(1<<CODING_MODE_AAC        )    // 0x00000080(ID:0x07)
#define  AUDIF_CODING_MODE_WMA			(1<<CODING_MODE_WMA        )    // 0x00000100(ID:0x08)
#define  AUDIF_CODING_MODE_RA			(1<<CODING_MODE_RA         )    // 0x00000200(ID:0x09)
#define  AUDIF_CODING_MODE_OGG			(1<<CODING_MODE_OGG        )    // 0x00000400(ID:0x0A)
#define  AUDIF_CODING_MODE_EMU			(1<<CODING_MODE_EMU        )    // 0x00000800(ID:0x0B)
#define  AUDIF_CODING_MODE_WMAPRO		(1<<CODING_MODE_WMAPRO     )    // 0x00001000(ID:0x0C)
#define  AUDIF_CODING_MODE_FLAC			(1<<CODING_MODE_FLAC       )    // 0x00002000(ID:0x0D)                                                                          
#define AUDIF_CODING_MODE_AMR			(1<<CODING_MODE_AMR        )    // 0x00004000(ID:0x0E)
#define AUDIF_CODING_MODE_MIDI			(1<<CODING_MODE_MIDI       )    // 0x00008000(ID:0x0F)


#define  AUDIF_CODING_MODE_MPEG_ENC		(1<<CODING_MODE_MPEG_ENC   )    // 0x00010000(ID:0x10)
#define  AUDIF_CODING_MODE_AC3_ENC		(1<<CODING_MODE_AC3_ENC    )    // 0x00020000(ID:0x11)
#define  AUDIF_CODING_MODE_DTS_ENC		(1<<CODING_MODE_DTS_ENC    )    // 0x00040000(ID:0x12)

#define  AUDIF_CODING_MODE_APE			(1<<CODING_MODE_APE        )    // 0x00080000(ID:0x13)
#define  AUDIF_CODING_MODE_ALAC			(1<<CODING_MODE_ALAC        )    // 0x00100000(ID:0x14)
#define  AUDIF_CODING_MODE_AVS			(1<<CODING_MODE_AVS        )    // (ID:0x15)
#define  AUDIF_CODING_MODE_DECMAX		AUDIF_CODING_MODE_AVS                                                                          
#define  AUDIF_CODING_MODE_MAX			AUDIF_CODING_MODE_AVS

// unsupported codecs                         
//#define AUDIF_CODING_MODE_JPG               (1<<CODING_MODE_JPG           )             
//#define AUDIF_CODING_MODE_SDDS              (1<<CODING_MODE_SDDS          )   
//#define AUDIF_CODING_MODE_MIDI              (1<<CODING_MODE_MIDI          )   
//#define AUDIF_CODING_MODE_NES               (1<<CODING_MODE_NES           )   
//#define AUDIF_CODING_MODE_SPDIF             (1<<CODING_MODE_SPDIF         )    
//#define AUDIF_CODING_MODE_TUNER             (1<<CODING_MODE_TUNER         )   
//#define AUDIF_CODING_MODE_NOISE             (1<<CODING_MODE_NOISE         )   
//#define AUDIF_CODING_MODE_CD_MP3_ENCODER	  (1<<CODING_MODE_CD_MP3_ENCODER)
//#define AUDIF_CODING_MODE_MP3_MIX           (1<<CODING_MODE_MP3_MIX       )   
//#define AUDIF_CODING_MODE_VCD_MIC           (1<<CODING_MODE_VCD_MIC       ) 


#define AUDIF_CODING_MODE_DEFAULT        		AUDIF_CODING_MODE_MPEG      // default (mainprogram)

#define IS_AUDIF_CODING_MODE_AC3        		(AUDIF_CODING_MODE_AC3 == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_PPCM       		(AUDIF_CODING_MODE_PPCM == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_MP1        		(AUDIF_CODING_MODE_MP1 == aud_param.coding_mode_now)    
#define IS_AUDIF_CODING_MODE_MP2        		(AUDIF_CODING_MODE_MP2 == aud_param.coding_mode_now)    
#define IS_AUDIF_CODING_MODE_LPCM       		(AUDIF_CODING_MODE_LPCM == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_DTS        		(AUDIF_CODING_MODE_DTS == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_SDDS       		(AUDIF_CODING_MODE_SDDS == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_MP3        		(AUDIF_CODING_MODE_MP3 == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_PCM        		(AUDIF_CODING_MODE_PCM == aud_param.coding_mode_now)    
//#define IS_AUDIF_CODING_MODE_JPG        		(AUDIF_CODING_MODE_JPG == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_MIDI       		(AUDIF_CODING_MODE_MIDI == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_NES        		(AUDIF_CODING_MODE_NES == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_SPDIF      		(AUDIF_CODING_MODE_SPDIF == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_DTSCD      		(AUDIF_CODING_MODE_DTSCD == aud_param.coding_mode_now)  
#define IS_AUDIF_CODING_MODE_WMA        		(AUDIF_CODING_MODE_WMA == aud_param.coding_mode_now)    
//#define IS_AUDIF_CODING_MODE_SPDIFINCD    	(AUDIF_CODING_MODE_SPDIFINCD == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_SPDIFINAC3   	(AUDIF_CODING_MODE_SPDIFINAC3 == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_SPDIFINDTS   	(AUDIF_CODING_MODE_SPDIFINDTS == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_TUNER        	(AUDIF_CODING_MODE_TUNER == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_NOISE        	(AUDIF_CODING_MODE_NOISE == aud_param.coding_mode_now) 
//#define IS_AUDIF_CODING_MODE_CD_MP3_ENCODER	(AUDIF_CODING_MODE_CD_MP3_ENCODER == aud_param.coding_mode_now) 
#define IS_AUDIF_CODING_MODE_OGG          		(AUDIF_CODING_MODE_OGG == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_MP3_MIX      	(AUDIF_CODING_MODE_MP3_MIX == aud_param.coding_mode_now)
#define IS_AUDIF_CODING_MODE_AAC       			(AUDIF_CODING_MODE_AAC == aud_param.coding_mode_now)
//#define IS_AUDIF_CODING_MODE_VCD_MIC      	(AUDIF_CODING_MODE_VCD_MIC==aud_param.coding_mode_now)
      		
#define IS_AUDIF_CODING_MODE_DUAL_DSP (1)       //[kiwi 20091214] 


//----- Output Mode -----//
//Downmix mode 0x0700
#define OUTPUT_DOWNMIX_MODE       0x00
#define OUT_51Channel             0x00
#define OUT_51DownmixL0R0         0x01
#define OUT_51DownmixLtRt         0x02
#define OUT_51DownmixLsRs         0x03

#define OUTPUT_2CH_MODE           0x10

//Specific AC3 output setting 
//output mode  : 0x0A00
#define OUT_RFModeK3              0x20
#define OUT_LineModeK2            0x21

//AC3 karaoke mode : 0x0A01
#define OUT_AC3KaraMode           0x30

//AC3 karaoke melody channel : 0x0A02
#define ENABLE_MELODY_CHANNEL     0x40
#define DISABLE_MELODY_CHANNEL    0x41

//Dynamic compression setting
#define OUT_CustomModeB_DigitalDialnormK1     0x50

//AC3 dual mono mode : 0x0A04
#define OUT_Dual_Mono_STEREO      0x60
#define OUT_Dual_Mono_LL          0x61
#define OUT_Dual_Mono_RR          0x62
#define OUT_Dual_Mono_MIX         0x63

//Specific MIDI melody chanel switch 
#define OUT_MIDI_Melody_ch1_on    0x70
#define OUT_MIDI_Melody_ch1_off   0x71
#define OUT_MIDI_Melody_ch2_on    0x72
#define OUT_MIDI_Melody_ch2_off   0x73
#define OUT_MIDI_Melody_ch4_on    0x74
#define OUT_MIDI_Melody_ch4_off   0x75
#define OUT_MIDI_Melody_Disable   0x76
#define OUT_MIDI_Melody_Enable    0x77

//spdif in
#define SPDIFIN_HW_PERIOD_BUF_LEN 16

/******************************************************************************
    Enums
 *****************************************************************************/
 
// -----CodeBank ID----- //
typedef enum
{
	AUDIF_BANK_MAIN = 0,
	AUDIF_BANK_DEC1 = 1,
	AUDIF_BANK_DEC2 = 2,
	AUDIF_BANK_ENC  = 3,
	AUDIF_BANK_DEC1_EXT  = 4,
	AUDIF_ERR_Codec_BankID,
}AUDIF_BANK_e;

typedef enum _AUDIF_DSP_Init_e
{
    AUDIF_System_Boot_init	= 0x0,
    AUDIF_Reset_DSP_init	= 0x1,
}AUDIF_DSP_Init_e;

typedef enum _AUDIF_DAC_Type_e
{
    INT_DAC_1500	= 0x0,
    EXT_DAC_8203D	= 0x1,
}AUDIF_DAC_Type_e;

typedef enum _AUDIF_XCK_FS_e
{
    SUPPORT_128_XCK	= 0x0,
    SUPPORT_256_XCK	= 0x1,
}AUDIF_XCK_FS_e;

typedef enum _AUDIF_GM7_e
{
    AUDIF_GM7_OFF    = 0x0,
	AUDIF_GM5_MOVIE  = 0x1,		
    AUDIF_GM5_MUSIC  = 0x2,    
    AUDIF_GM7_Mode3  = 0x3,
    AUDIF_GM7_Mode4  = 0x4,
}AUDIF_GM7_e;

//----- DTS Neo6 command set -----//
typedef enum
{
    AUDIF_Neo6_Music    = 0x01,
    AUDIF_Neo6_Cinema   = 0x00,
    
    AUDIF_Neo6_Ch3      = 0x03,
    AUDIF_Neo6_Ch4      = 0x04,
    AUDIF_Neo6_Ch5      = 0x05,
    AUDIF_Neo6_Ch6      = 0x06,
    AUDIF_Neo6_Ch7      = 0x07,
    
    AUDIF_Neo6_G1p0     = 0x03,
    AUDIF_Neo6_G0p5     = 0x02,
    AUDIF_Neo6_G0p3     = 0x01,
    AUDIF_Neo6_G0p0     = 0x00,    
}AUDIF_Neo6_e;

//----- SRS TruVol command set -----//
typedef enum
{
    AUDIF_TruVol_Light  = 0x04,
    AUDIF_TruVol_Normal = 0x05,
    AUDIF_TruVol_Heavy  = 0x06,
    AUDIF_TruVol_Off    = 0x00,            
}AUDIF_TruVol_e;


//----- generic error codes -----//
typedef enum _AUDIF_Status_e
{
	/* generic error codes */
	AUDIF_ERR_Download_Failed = -6,
	AUDIF_ERR_Unknown_CodecID = -5,
	AUDIF_ERR_UnUsed = -4,
	AUDIF_ERR_INVALID_PARAM = -3,
	AUDIF_ERR_FIFOSpmCntMode = -2,
	AUDIF_ERR_BankID = -1,
	
	AUDIF_SUCCESS = 0x0,
	AUDIF_FAILURE,
	AUDIF_TIMEOUT,

	AUDIF_ERR_INVALID_OP,
	AUDIF_ERR_MEMORY_ALLOC,
	AUDIF_ERR_INVALID_USED,
	AUDIF_ERR_INVALID_State,

	AUDIF_ERR_BSBUF_FEED,
	AUDIF_ERR_BSBUF_ID,
	AUDIF_ERR_BS_WAIT,

	AUDIF_Warning_NonSupport_HWRamp,
} AUDIF_Status_e;

typedef enum _AUDIF_BS_Ending_e
{
	AUDIF_BS_Ending_Not_yet = 0,
	AUDIF_BS_Ending_done = 1,	
}AUDIF_BS_Ending_e;

typedef enum _AUDIF_FIFO_CNT_e
{
	AUDIF_FIFOCNT_Not_Empty = 0,
	AUDIF_FIFOCNT_Empty = 1,	
}AUDIF_FIFO_CNT_e;

typedef enum _AUDIF_FIFO_Type_e
{
	FIFO_PCM = 0,
	FIFO_SPDIF = 1,
	FIFO_HDMI =2,
	
}AUDIF_FIFO_Type_e;

typedef enum _AUDIF_FIFOSmpCnt_e
{
	FIFOSmpCnt_Init = 0,
	FIFOSmpCnt_Enable,
	FIFOSmpCnt_Disable,
	FIFOSmpCnt_Reset,
}AUDIF_FIFOSmpCnt_e;

typedef enum _AUDIF_CCA_Type
{
	unlimited_copy	= 0x0,
	//no_use		= 0x1,	
	one_copy_allowed= 0x2,
	no_copy_allowed	= 0x3,
	
}AUDIF_CCA_Type;

typedef enum _AUDIF_CDtype_e
{
	CDtype_CDDA = 0,
	CDtype_HDCD = 1,
	
} AUDIF_CDtype_e;

typedef enum _AUDIF_SetupMenuItem_e
{
	AUDIF_SETUP_SPDIF = 0x0,
	AUDIF_SETUP_HDMIAudio,
	AUDIF_SETUP_DownSampling,
	AUDIF_SETUP_Surround,
	AUDIF_SETUP_EQ,
	AUDIF_SETUP_DRC,
	AUDIF_SETUP_KaraokeEcho,
	AUDIF_SETUP_KaraokeKeyShift,
	AUDIF_SETUP_AudioDownMix,
	AUDIF_SETUP_GM5,
	AUDIF_SETUP_AudioSize,
	AUDIF_SETUP_AudioDistance,
	AUDIF_SETUP_AudioLevel,	
	AUDIF_SETUP_Mic,
} AUDIF_SetupMenuItem_e;

//----- MPEG channel status for STB -----//
typedef enum
{
	Stereo			= 0,
	Joint_stereo	= 1,
	Dual_channel	= 2,
	Single_channel	= 3,
}MPEG_chnStatus_e;

//----- Dual DSP command set only for STB -----//
typedef enum
{
	DualDSP_mode_Disable	= 0,
	DualDSP_mode_Enable		= 1,
}DualDSP_mode_e;
            
//----- Key shift command set -----//
typedef enum
{
    KS_POS_6            = 0x06,
    KS_POS_5            = 0x05,
    KS_POS_4            = 0x04,
    KS_POS_3            = 0x03,
    KS_POS_2            = 0x02,
    KS_POS_1            = 0x01,
    KS_0                = 0x00,	//default
    KS_NEG_1            = 0xFF,
    KS_NEG_2            = 0xFE,
    KS_NEG_3            = 0xFD,
    KS_NEG_4            = 0xFC,
    KS_NEG_5            = 0xFB,
    KS_NEG_6            = 0xFA,
}AUDIF_AppKs_e;

//----- Key shift time scale command set -----//
typedef enum
{
    KT_POS_4            = 0x04,
    KT_POS_3            = 0x03,
    KT_POS_2            = 0x02,
    KT_POS_1            = 0x01,
    KT_0                = 0x00, //default
    KT_NEG_1            = 0xFF,
    KT_NEG_2            = 0xFE,
    KT_NEG_3            = 0xFD,
    KT_NEG_4            = 0xFC,
}AUDIF_AppKt_e;

//----- 3D Surround command set -----//
typedef enum
{
    SURROUND_SATNDARD   = 0x00,
    SURROUND_CONCERT    = 0x01,
    SURROUND_CHURCH     = 0x02,
    SURROUND_PASSIVE    = 0x03,
    SURROUND_WIDE       = 0x04,
    SURROUND_LIVE       = 0x05,    
    SURROUND_ARENA      = 0x06,
    SURROUND_HALL       = 0x07,
    SURROUND_BATHROOM   = 0x08,
    SURROUND_CAVE       = 0x09,
}AUDIF_App3D_e;

//----- Equalizer (EQ) command set -----//
typedef enum
{
    EQ_SATNDARD     = 0x00,
    EQ_CLASSIC      = 0x01,
    EQ_ROCK         = 0x02,
    EQ_JAZZ         = 0x03,
    EQ_POP          = 0x04,
    EQ_DANCE        = 0x05,
    EQ_TECHNO       = 0x06,
    EQ_SOFT         = 0x07,
    EQ_LIVE         = 0x08,
    EQ_USER_DEFINED = 0x10,
    
    EQ_BAND_0       = 0x20,
    EQ_BAND_1       = 0x21,
    EQ_BAND_2       = 0x22,
    EQ_BAND_3       = 0x23,
    EQ_BAND_4       = 0x24,
    EQ_BAND_5       = 0x25,
    EQ_BAND_6       = 0x26,    
}AUDIF_AppEq_e;

//----- Bass Management (BM) command set -----//
typedef enum
{
    BM_SPK_LF       = 0x10,
    BM_SPK_RF       = 0x20,
    BM_SPK_LS       = 0x30,
    BM_SPK_RS       = 0x40,
    BM_SPK_LB       = 0x50,
    BM_SPK_RB       = 0x60,
    BM_SPK_C        = 0x70,
    BM_SPK_SUB      = 0x80,
    BM_SPK_LMRM     = 0xC0,
    BM_SPK_FRONT    = 0x90,
    BM_SPK_SURR     = 0xA0,
    BM_SPK_BACK     = 0xB0,
    BM_SPK_LARGE    = 0x00,
    BM_SPK_SMALL    = 0x01,
    BM_SPK_OFF      = 0x02,
    BM_SPK_ON       = 0x01,    
}AUDIF_AppBm_e;


typedef enum
{
    MASTER_GAIN = 0x0,
    LM_SPKGAIN  = 0x1,
    RM_SPKGAIN  = 0x2,
    LF_SPKGAIN  = 0x3,
    RF_SPKGAIN  = 0x4,
    LS_SPKGAIN  = 0x5,
    RS_SPKGAIN  = 0x6,
    C_PKGAIN    = 0x7,
    SW_SPKGAIN  = 0x8,
    LB_SPKGAIN  = 0x9,
    RB_SPKGAIN  = 0xA,
}AUDIF_SPK_e;

typedef enum
{
	nonDTSCD	= 0x0,
	DTSCD_16bit	= 0x1,
	DTSCD_14bit	= 0x2,
	Error_DTSCD_type = 0x3,
} disc_DTSCDtype_e;

typedef enum
{
	RF_outmode = 0x0,
	Line_outmode = 0x1,
} AC3_outmode_e;

typedef enum
{
	CRC_check_off	= 0x0,
	CRC_check_on	= 0x1,
}MPEG_CRC_Check_e;

//----- Audio interface config -----// 
typedef enum
{
	DAC_I2S24 = 0x4D,
	DAC_RJ24  = 0x5E,
	DAC_LJ24  = 0x5C,	
} DAC_format_e;

//----- DAC config -----//
typedef enum
{
	DAC_off,
	DAC_2ch_output,
	DAC_6ch_output,
	DAC_8ch_output,
} DAC_OutputCfg_e;

typedef enum
{
	pcm_POSITIVE = 0x1,
	pcm_NEGATIVE = 0x0,
} pcm_priority_e;

typedef enum
{
	mic_off	= 0x0,
	mic_on	= 0x7,
	mic_mix_i2s = 0x1,
	mic_mix_spdif0 = 0x2,
	mic_mix_spdif1 = 0x4,
	mic_mix_i2s_iec0 = 0x3,
} mic_switch_e;

typedef enum
{
	mic_gain_default	= 0x8000,
	mic_gain_max		= 0x8000,
	mic_gain_min		= 0x0,
} mic_gain_e;

//----- RISC set ADC gain for customer request (range: 1~5) -----//
typedef enum
{	
	AD_level_1 = 1,
	AD_level_2 = 2,
	AD_level_3 = 2,
	AD_level_4 = 4,
	AD_level_5 = 5,	
} AD_gain_e;

//----- InitRISCParam(initial phase) -----//
typedef enum
{
	EQ_STANDARD_40HI = 0x0c30,
	EQ_CLASSIC_40HI  = 0x0c30,
	EQ_ROCK_40HI     = 0x0628,
	EQ_JAZZ_40HI	 = 0x1120,
	EQ_POP_40HI      = 0x1430,
	EQ_DANCE_40HI    = 0x0c24,
	EQ_TECHNO_40HI   = 0x0e30,
	EQ_SOFT_40HI     = 0x1030,
	EQ_LIVE_40HI     = 0x0a31,

	EQ_STANDARD_40LO = 0xc30c,
	EQ_CLASSIC_40LO  = 0xc30c,
	EQ_ROCK_40LO     = 0xe514,
	EQ_JAZZ_40LO	 = 0xf494,
	EQ_POP_40LO      = 0x650e,
	EQ_DANCE_40LO    = 0x9411,
	EQ_TECHNO_40LO   = 0xb30b,
	EQ_SOFT_40LO     = 0x9350,
	EQ_LIVE_40LO     = 0x0452,

	EQ_STANDARD_65   = 0x830c,
	EQ_CLASSIC_65    = 0x0089,
	EQ_ROCK_65       = 0x058a,
	EQ_JAZZ_65       = 0x0588,	
	EQ_POP_65        = 0x028e,
	EQ_DANCE_65      = 0x0492,
	EQ_TECHNO_65     = 0x0490,
	EQ_SOFT_65       = 0x0492,
	EQ_LIVE_65       = 0x024a,
	
} EQ_type_e;

typedef enum
{
	dwmix_off  = 0x0,
	dwmix_L0R0 = 0x1,
	dwmix_LtRt = 0x2,
	dwmix_LsRs = 0x3,	
} downmix_e;

typedef enum
{	
	outmode_spkFF , // raw mode(default)
	outmode_spk10 = 0x1,
	outmode_spk20 = 0x2,
	outmode_spk30 = 0x3,
	outmode_spk22 = 0x6,
	outmode_spk32 = 0x7,
	outmode_spk33 = 0x8,
	outmode_spk34 = 0x9,
} SpkOutmode_e;


typedef enum
{	
	//support with mpeg,lpcm,wma
	OUT_STEREO           = 0x1,
	OUT_LL               = 0x2,
	OUT_RR               = 0x3,
	OUT_LL_AutoADDetect  = 0x4,
	OUT_RR_AutoADDetect  = 0x5,
	OUT_MixMono          = 0x6,
	//support with mpeg,lpcm,wma,ac3  
	OUT_VocalCancel_Auto = 0x7,
	OUT_VocalCancel      = 0x8,
	OUT_Karaoke_MAX     = 0x8,
} ch_mode_e;

typedef enum
{	
	OUT_NoVocal    = 0x0,
	OUT_LeftVocal  = 0x10,
	OUT_LeftVocal_AD  = 0x11,
	OUT_RightVocal = 0x20,
	OUT_RighVocal_AD  = 0x21,
	OUT_BothVocal  = 0x30,
	OUT_BothVocal_AD  = 0x31,
	AC3_Karaoke_Max	  = 0x7,
} AC3_Karaoke_e;

typedef enum
{	
	real_mode  = 0,	// default
	fast_mode,
	lineIn_mode,
	CDwithlineIn,
	CDmixlineIn,
	CDmixMIC,
} cdrip_mode_e;

typedef enum
{	
	ext_ADC = 0,	// default
	int_ADC,
} ADC_line_mode_e;

typedef enum
{	
	Disable = 0,	// default
	Enable  = 1,
} switch_e;

typedef enum
{	
	vocal_off = 0x0,	// default
	vocal_L   = 0x1,
	vocal_R   = 0x2,
} karaoke_vocal_e;

typedef enum
{	
	swapp_off    = 0x0,
	en_prologic2 = 0x1,
	en_neo6      = 0x2,
	en_gm7       = 0x3,
} swapp_e;

typedef enum
{	
	KSE_OFF	= 0x0,	// default
	KSE_flanger = 0x10,
	KSE_chorus  = 0x20,
	KSE_tremolo = 0x30,
	KSE_vibrato = 0x40,
} karaoke_sound_eft_e;


//----- Fast Forward command set -----//
typedef enum
{	
	// FF control by SWAPP
	FF_1x	= 0x0,
	FF_1p25x= 0x0100,
	FF_1p5x	= 0x0200,
	FF_1p75x= 0x0300,
	FF_2x	= 0x0400,
	FF_0p875= 0x1100,
	FF_0p75	= 0x1200,
	FF_0p625= 0x1300,
	FF_0p5	= 0x1400,
#if 0 // FF control by decoder
	FF_1x	= 0x0,	// default
	FF_2x	= 0x1,
	FF_4x	= 0x2,
	FF_8x	= 0x3,
	FF_16x	= 0x4,
	FF_32x	= 0x5,
#endif	
} FastForward_e;

//----- Output phase config -----//
typedef enum
{	
	OUTPUT_PHASE_0      = 0x0,	// default
	OUTPUT_PHASE_180    = 0x1,
} output_phase_e;

/******************************************************************************
    Structure
 *****************************************************************************/
typedef struct t_spdifin_info
{
    BYTE	bs_fmt;
    UINT32	fs;
    UINT8   cgms_type;
    UINT8   category;
}spdifin_info;

typedef struct t_spdifin_hw_period
{
    UINT32  period;    
    UINT32  buf[SPDIFIN_HW_PERIOD_BUF_LEN];
    INT16   idx;
    INT16   cnt;
}spdifin_hw_period;

typedef struct t_spdifin_fs_cnt
{
    UINT16 hi;
    UINT16 mid;
    UINT16 lo;
}_spdifin_fs_cnt;

typedef struct t_spdifin_hw_clk
{
    UINT32  sysclk;
    _spdifin_fs_cnt fs_32k;
    _spdifin_fs_cnt fs_44k;    
    _spdifin_fs_cnt fs_48k;    
    _spdifin_fs_cnt fs_88k;
    _spdifin_fs_cnt fs_96k;    
    _spdifin_fs_cnt fs_176k;    
    _spdifin_fs_cnt fs_192k;        
}_spdifin_hw_clk;

typedef struct t_output_mode
{
	BYTE	karaoke_mode;//2-ch karaoke mode
	BYTE 	ac3_karaoke_mode;//ac3 karaoke mode
	BYTE 	downmix_mode;//downmix mode
	BYTE 	ac3_op_mode;//ac3 output mode
	UINT16 	ac3_dyncmprs_mode;//ac3 dynamic compression setting
	BYTE 	ac3_dualmono_mode;//ac3 dual mono mode
	BYTE 	ac3_melody_on;//ac3 melody ch on/off
} output_mode;

typedef struct t_smprate_info
{
	UINT16	smp_rate_now;
	UINT16	frequency_mask;
	UINT8	frequency_multiple;
	UINT16	hw_smprate; // record audio hw sample rate
} smprate;

typedef struct t_dec_info
{
	UINT32 lpcm_frame_size;
	UINT32 lpcm_mode;
	UINT32 aac_mode;
} dec_info;



typedef struct t_spk_type
{
	BYTE front;
	BYTE center;
	BYTE surround;
	BYTE subwoofer;
	BYTE back;
	BYTE LF;
	BYTE RF;
	BYTE LS;
	BYTE RS;
	BYTE LB;
	BYTE RB;
} spk_type;


typedef struct t_mspk_type
{
	BYTE LF;//front left
	BYTE RF;//front right
	BYTE LS;//surround left
	BYTE RS;//surround right
	BYTE LB;//back left
	BYTE RB;//back right
	BYTE C;//center
	BYTE SUB;//subeoofer
	BYTE LMRM;//downmix channels
} mspk_type;

/*!
	\struct	t_channel_gain
	\brief	set each channel gain value
*/
typedef struct t_channel_gain
{
	UINT32	MG;
	UINT16	Lm;	/*!< channel select*/
	UINT16	Rm;
	UINT16	A0; // LF/RF
	UINT16	A1; // LS/RS
	UINT16	A2; // C/LFE
	UINT16	A3;	// LB/RB
} channel_gain;

/*!
	\struct	t_DAC_property
	\brief	DAC related setting
*/
typedef struct t_DAC_property
{
	UINT16	fs_limit; /*!< on top of sample rate that the system supported */
	UINT16	format;
	UINT16	pcm_priority;	
} DAC_property;

//
// Sound effect (temp. added)
//
typedef enum 
{	
	ID_SND_EFF_STEREO    = 0,
	ID_SND_EFF_MONO_CH1  = 1,
	ID_SND_EFF_MONO_CH2  = 2,
	ID_SND_EFF_MONO_CH3  = 3,
	ID_SND_EFF_MONO_CH4  = 4,
	ID_SND_EFF_MONO_CH5  = 5,	
	ID_SND_EFF_MONO_CH6  = 6,		
	ID_SND_EFF_MONO_CH7  = 7,
	ID_SND_EFF_MONO_CH8  = 8,	
	ID_SND_EFF_MAX       = 8,
} SndEffectChannelID;

typedef struct _SndEffect                                                         
{         
	SndEffectChannelID  id;
	UINT32 snd_address;
	UINT32 snd_sampleCnt;
    UINT16 MixGainLf;		                                                                 
    UINT16 MixGainRf;                                                                        
    UINT16 MixGainLs;   		                                                             
    UINT16 MixGainRs;		                                                                 
    UINT16 MixGainC;                                                                         
    UINT16 MixGainLfe;                                                                       
    UINT16 MixGainLb;                                                                        
    UINT16 MixGainRb;                                                                        
    UINT16 MixGainLm;                                                                        
    UINT16 MixGainRm;                                                                        
}SndEffect;      

typedef enum 
{
	SndEFF_BD = 0,
	SndEFF_game = 1,
}SndeffectMode_e;

/*!
	\struct	t_MIC_info
	\brief	MIC related setting
*/
typedef struct t_MIC_setting
{
	UINT16	mic_switch;    // mic_switch_e
	UINT16	gain0;	       // mic_gain_e
	UINT16	gain1;         // mic_gain_e
	UINT16	upper_th;      // (upper_theshold)	default: 0x100
	UINT16	presensity;	   // default: 0x40
	UINT16	aftersensity;  // default: 0x9
	UINT16	lower_th; // default: 0x55
} MIC_setting;

typedef struct t_AVS_DS
{
	UINT16	bs_type;
	UINT16	output_channel_mode;
	UINT16	nRaw_Block;
} AVS_DS;

typedef enum 
{
	AVS_BsType_RAW  = 0,
	AVS_BsType_AASF = 1,
	AVS_BsType_AATF = 2,
}AVS_Info_BsType;

typedef enum 
{
	AVS_Mono_mode = 1,
	AVS_Dual_mode = 2,
}AVS_Info_Channel_Mode;

typedef enum _AUDIF_Decoder_OPT
{
	// bit0: 0=aac enable, 1=aac disable
	// bit1: 0=heaac enable, 1=heaac disable
	INIT_OPT               = 0x0,	
	Support_LCAAC_HEAAC    = 0x0,
	Support_LCAAC_only     = 0x2,
	Support_HEAAC_only     = 0x1,
	NotSupport_LCAAC_HEAAC = 0x3,
	NotSupport_HEAAC       = 0x2,
}AUDIF_Decoder_OPT_e;

typedef struct t_auddrv_param
{
	UINT32      coding_mode_now;
	UINT16      encoding_mode_now;	    
	BYTE 		volume;
	mspk_type 	vol_mspk;
	BYTE 		mute;
	BYTE		mute_owner;
	BYTE 		srnd_type;
	BYTE 		eq_type;
	BYTE 		eq_gain[7];
	BYTE 		tone_key;
	output_mode out_mode;
	BYTE 		spdif_mode;
	BYTE        hdmi_mode; 	// hcsu:added
	BYTE 		hdmi_raw_mode;
	UINT16 		mic_mode;
	BYTE 		mic0_gain;	// mic1_gain->mic0_gain
	BYTE 		mic1_gain;	// mic2_gain->mic1_gain
	BYTE 		echo_level;
	BYTE        echo_decay;
	UINT16      echo_delay;
	UINT32 		pl2_init_val;
	BYTE 		pl2_mode;
	UINT32      neo6_cgain;
	BYTE        neo6_ch;
	BYTE        neo6_mode;
	UINT16      truvol_gain;
	BYTE        truvol_mode;
	spk_type 	speaker_info;
	UINT16 		gBass_Config;
	mspk_type   mspk_delay;
	BYTE        speaker_sw;
	BYTE 		center_spk_delay;
	BYTE 		srnd_spk_delay;
	smprate		smprate_info;
	UINT32 		encoder_sample_rate;
	UINT32 		encoder_bit_rate;
	BYTE        encoder_mode;
	BYTE 		encoder_speed;
	dec_info  	dec_param;	// hcsu:may needs to expand
	BYTE 		cdda_type;
	BYTE		testtone;
	BYTE		FIFO_Enable_Flag;	
	UINT16		output_channel_cfg; // use to choose spk on/off
	BYTE		DTSCD_type;
	UINT16		MPEG_CRC_Check;
	BYTE		DAC_Output_Cfg;
	UINT16		PCM_CFG;
	INT32       aux1_volume;
	INT32       aux2_volume;
	INT32       master_volume;
	UINT8       aux1_mute_glag;
	UINT8       aux2_mute_glag;
	UINT8       master_mute_glag;	
	UINT8       internal_mute_flag;
    UINT16		DDP_Converter_Mode;
    AVS_DS      AVS_Info;
    UINT8       pcm_output_phase;
	UINT16		Decoder_OPT;
    UINT16      Decoder_RunTimeOPT;
	UINT16      DDP_LFF_Channel_Mode;
} auddrv_param;

typedef struct t_AUDCLK_InitParam
{
	UINT32		xck_clk_fs_table[13];
	UINT16		Is_xck_clk_fs_setting;
}AUDCLK_param;

EXTERN AUDCLK_param audHW_param;
EXTERN auddrv_param aud_param;

// for init parameter from setup-menu
typedef struct t_auddrv_InitRiscParam
{
	channel_gain	chn_gain;		// default: 0x80(max)
	UINT16			disc_type;		// disc_type_e
	UINT16			spdif_mode;		// SPDIF_OFF/SPDIF_PCM/SPDIF_RAW
	UINT16			hdmi_mode;		// HDMI_OFF/HDMI_PCM/HDMI_RAW
	DAC_property	DAC_info;		// DAC_format_e & pcm_priority_e
	UINT16			echo_delay;
	UINT16			echo_decay;
	MIC_setting		mic_info;		// mic_switch_e & mic_gain_e 
	UINT16			AD_gain;		// AD_gain_e
	UINT16			keyshift;		// AUDIF_AppKs_e, 0xhh01 where hh is a variable for shiftscale
									// AUDIF_AppKt_e, 0xhh00 where hh is a variable for timescale
	UINT16			EQ_gain40HI;	// EQ_type_e
	UINT16			EQ_gain40LO;	// EQ_type_e
	UINT16			EQ_gain65;		// EQ_type_e
	UINT16			Surround_3D;	// AUDIF_App3D_e
	UINT16			bass_mgt;		// default: 0x0
	UINT16			bass_mgt_cfg;	// default: 0x0
	UINT16 			spk_delay_fnt;  // default: 0x0
	UINT16 			spk_delay_srud;	// default: 0x0
	UINT16 			spk_delay_back;	// default: 0x0
	UINT16 			app_cmode;		// default: 0x8888
	UINT16 			app_control;	// default: 0x063f
	UINT16			downmix_mode;	// downmix_e
	UINT16			ch_mode;		// ch_mode_e
	UINT16			cdrip_mode;		// cdrip_mode_e
	UINT16			ADC_line_mode;	// ADC_line_mode_e
	UINT16			karaoke_sound_eft; // karaoke_sound_eft_e
	UINT16			sound_effect;	// switch_e
	UINT16			sound_eft_gain;	// deault: 0x0
	UINT16			karaoke_vocal;	// karaoke_vocal_e
	UINT16			swapp;			// swapp_e	
} auddrv_InitRiscParam;

/*******IEC0 Delay command set only for STB*******/
typedef enum
{
	IEC0Delay_mode_Disable	= 0,
	IEC0Delay_mode_Enable	= 1,
}IEC0Delay_mode_e;

EXTERN auddrv_InitRiscParam aud_InitRiscParam;


/*!
	\brief	Wave format related
	\var		UINT32	aud_frequency
*/
extern UINT32           aud_frequency;

/*!
	\brief	Wave format related
	\var		UINT32	aud_frame_size
*/
extern UINT32           aud_frame_size;

/******************************************************************************
	Function headers
 *****************************************************************************/

/************************************************************/
/*                                                          */
/*  download dsp related APIs                               */
/*                                                          */
/************************************************************/

/*!
	\brief	Audio memory allocation controled by upper layer
	\param	dAudioAnchor [IN] audio memory address
	\return	error status enum
	\note	shoulld be un-cache dram_A or dram_B
*/
AUDIF_Status_e Audio_MemoryInit(UINT32 dAudioAnchor);

/*!
	\brief	Used to report dsp message by polling, also warning of discontinue samples
	\param	*pParam [IN]
	\return	None
	\note	100ms polling is used
*/
int Audio_MainRoutine( void *pParam );

/*!
  \brief initial audio dsp system to default and boot up mainprogram system\n
  \par detail description:\n
  internally AUDIF_Init_Audio wll invoke the following funtions:\n
  1. aud_InitAudParams(): initial auddrv_param static globals\n           
  2. dsp_Initial(): initial static globals of dsp API layers\n
  3. dspInitSystem(): download dsp mainprogram system and set to RUN state\n 
  4. AUDIF_Set_SampleRate(): set default sample rate (FS_DEFAULT)\n 
  5. initialize speaker gain and configuration\n.
  \param	None
  \return	None
  \note	    
 */
void AUDIF_Init_Audio(void);


/*!
	\brief	force to re-download current dsp code
	\param	None
	\return	None
	\note	
*/
void AUDIF_Reset_DSP(void);


/*!
	\brief	Download a specific dsp code
	\param	coding_mode	[IN] (refer auddrv.h)
	\return	None
	\note	DSP Download Procedule related API
*/
void AUDIF_Set_CodingMode(UINT32 coding_mode);

/*!
	\brief	Download a second specific dsp code
	\param	coding_mode	[IN] (refer auddrv.h)
	\return	None
	\note	DSP Download Procedule related API
*/
INT32 AUDIF_Set_Dec2CodingMode(UINT32 coding_mode);

/*!
	\brief	to get mainprogram ID and version for angle key
	\param	None
	\return	ID, year, and date
	\note	DSP version related API
*/
void AUDIF_Get_Bank0Ver( UINT32 *dspID );

/*!
	\brief	to get dec1 ID and version for angle key
	\param	None
	\return	ID, year, and date
	\note	DSP version related API
*/
void AUDIF_Get_Dec1Ver( UINT32 *dspID );

/*!
	\brief	Shown dsp ID and version
	\param	None
	\return	None
	\note	DSP version related API
*/
void AUDIF_Get_Decoder_Mode( void );


/*!
	\brief	Get dsp ID
	\param	None
	\return	coding_mode
	\note	DSP version related API
*/
UINT32 AUDIF_Get_DSPCodingMode( void );

/*!
	\brief	shut down audio module, include dsp and audio hw
	\param	None
	\return	None
	\note	1. Audio Initialization related API\n
			2. only stop behavior at this time
*/
void AUDIF_Audio_Shut_Down(void);

/*!
	\brief	Set audio hardware clock
	\par detail description:\n
	1. We will do up-sampling if sample rate is lower than 32KHz \n
	2. We will do down-sampling if frequence is not supported \n
	3. it will set DAC/IEC0/IEC1 clock at the same time 
	\param	smp_rate [IN] samples/sec, e.g., FS_32K
	\return	None
	\note	Audio sampling rate setting related API \n			
*/
void AUDIF_Set_SampleRate(int smp_rate);

/*!
	\brief	Set IEC0 clock
	\par detail description:\n
	1. We will do up-sampling if sample rate is lower than 32KHz \n
	2. We will do down-sampling if frequence is not supported \n
	\param	smp_rate [IN] samples/sec, e.g., FS_32K
	\return	None
	\note	Audio sampling rate setting related API \n			
*/
void AUDIF_Set_SPDIF_SmpRate(INT32 smp_rate);

/*!
	\brief	Set IEC1 clock
	\par detail description:\n
	1. We will do up-sampling if sample rate is lower than 32KHz \n
	2. We will do down-sampling if frequence is not supported \n
	\param	smp_rate [IN] samples/sec, e.g., FS_32K
	\return	None
	\note	Audio sampling rate setting related API \n			
*/
void AUDIF_Set_HDMI_SmpRate(INT32 smp_rate,UINT8 pathmode);

/*!
	\brief	check if IEC1 clock is supported
	\par detail description:\n
	\param	
	\return	
	\note	
*/
void AUD_IS_Support_HDMI_SmpRate(UINT32 *downSampling);

/*!
	\brief	Runtime update sample rate reported by decoder through mailbox[20]
	\param	void
	\return	None
	\note	Audio sampling rate setting related API
*/
void AUDIF_Update_Dec1_SampleRate(void);

/*!
	\brief	To get dec1 sample rate
	\param	void
	\return	None
	\note	Audio sampling rate setting related API
*/
UINT16 AUDIF_Get_SampleRate( void );

/*!
	\brief	Check FS valid range which limit the max sampling rate
	\param	mask [IN]\n
		0: 48K\n
		1: 96K\n
		2: 192K
	\return	frequency_mask
	\note	Audio sampling rate setting related API
*/
UINT16 AUDIF_Set_FrqMsk(UINT16 mask);

/*!
	\brief	A mapping of sample rate from dsp ID to system index
	\param	samplerateID [IN] e.g., K_DSP_SAMPLE_RATE_ID_32K
	\return	sample rate index, e.g., FS_32K
	\note	Audio sampling rate setting related API
*/
UINT16 aud_map_SmpRate_IDtoFS(UINT16 samplerateID);

/*!
	\brief	A mapping of sample rate from system index to dsp ID
	\param	smp_fs [IN] system fs index, e.g., FS_32K
	\return	sample rate ID, e.g., K_DSP_SAMPLE_RATE_ID_32K
	\note	Audio sampling rate setting related API
*/
UINT16 aud_map_SmpRate_FStoID(UINT16 smp_fs );

/*!
	\brief	A mapping of sample rate from system index to digital value
	\param	smp_in [IN] system fs index, e.g., FS_32K
	\return	sample rate represented by the digital value
	\note	Audio sampling rate setting related API
*/
INT32 aud_map_SmpRate_FStoDigital( UINT16 smp_in );


/************************************************************/
/*                                                          */
/*  Media Initialization APIs                               */
/*                                                          */
/************************************************************/

/*!
	\brief	Do some audio initialization according to "waveformat" header information
	\param	*wfmt	[IN] a pointer to waveformat header
	\return	1: success\n
			0: fail
	\note	1. DSP Download Procedule related API\n
	        2. Include download specific dsp and related decoder initialization /set sampling rate\n
	        3. waveformat header structure is defined in wave_fmt.h
*/
INT32 AUDIF_Init_Wave_Audio(WAVFORMATMACRO *wfmt);

/*!
	\brief	Get LPCM audio bistream fram size used for audio bistream buffer endpoint calculation
	\param	void
	\return	lpcm frame size
	\see	AUDIF_LPCM_Set_Buf_EndPoint()
	\note	LPCM format related API
*/
UINT32 AUDIF_Get_LPCM_Fsize(void);

/*!
	\brief	Set LPCM audio bistream fram size used for audio bistream buffer endpoint calculation
	\param	size [IN] frame size
	\return	AUDIF_Status_e
	\note	LPCM format related API
*/
AUDIF_Status_e AUDIF_Set_LPCM_Fsize(UINT32 size);

/*!
	\brief	Set LPCM audio bistream fram size used for audio bistream buffer endpoint calculation
	\par detail description:\n
	1. LPCM audio playback Initialization according to "waveformat" header information and decoder mode selection.\n
	\param	waveformat [IN] 
	\return	None
	\note	LPCM format related API
*/
void AUDIF_LPCM_Init_Parm(UINT32 mode, WAVFORMATMACRO *wfmt1);

/*!
	\brief	Set LPCM audio bistream fram size used for audio bistream buffer endpoint calculation
	\param	size [IN] frame size
	\return	AUDIF_Status_e
	\note	LPCM format related API
*/
void AUDIF_LPCM_Set_Buf_EndPoint(UINT32 fsize);

/*!
	\brief	Set MPEG audio CRC mode
	\param	switch_mode [IN] 0:CRC_check_off, 1:CRC_check_on
	\return	NONE
	\note	
*/
void AUDIF_Set_MPEG_CRC_Check( UINT16 switch_mode );

/*!
	\brief	Get MPEG audio CRC mode
	\param	void
	\return	MPEG_CRC_Check_e
	\note	
*/
UINT16 AUDIF_Get_MPEG_CRC_Check( void );

//============================================
//     Encode(MPEG/AC3/DTS) related APIs
//============================================

/*!
	\brief	download encoder dsp
	\param	endcode_mode [IN]
	\return	none
	\note	Encode related API\n
*/
void AUDIF_Set_EncoderDSP(UINT16 endcode_mode);

/*!
	\brief	Encode Mode selection
	\param	uCmdType	[IN] encode mode
	\return	1: success\n
			0: fail
	\note	Encode related API
*/  
void AUDIF_Set_Encode_Mode(UINT8 uCmdType);

/*!
	\brief	Encode bitrate setting
	\param	kBitrate [IN] encode kbitrate 
	\return	1: success\n
			0: fail
	\note	1. Encode related API\n
			2. We now support 128/160/192/256/320 kbps \n
			3. shoule removed.
*/  
void AUDIF_Set_Encode_Bitrate(UINT32 kBitrate);

/*!
	\brief	Encode sampling rate setting
	\param	samprate [IN] encode sampling rate 
	\return	1: success\n
			0: fail
	\note	1. Encode related API\n
			2. We now support 32K, 44K, 48K \n
			3. shoule removed.
*/  
void AUDIF_Set_Encode_Fs(UINT32 samprate);

void AUDIF_Set_Encode_Speed(UINT8 uCmdType);

INT32 AUDIF_Enable_AC3TransCoding( void );

int AUDIF_Updata_AAC_Param( UINT32 val );

INT32 AUDIF_AVS_Init_Param( UINT8 BankID,  AVS_DS AVS_Dec_Info );
	
INT32 AUDIF_Set_Decoder_OPT( UINT16 wavefmt , UINT16 opt_val );
	
/************************************************************/
/*                                                          */
/*  DSP FLOW CONTROL APIs                                   */
/*                                                          */
/************************************************************/

/*!
	\brief	Send Ready command to DSP
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API\n			
*/
int AUDIF_Set_Ready( UINT8 BankID );

/*!
	\brief	Send Stop command to DSP
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
int AUDIF_Set_Stop( UINT8 BankID );

/*!
	\brief	Send Stop command to DSP and NO wait to PCM ramp-down
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
INT32 AUDIF_Set_Stop_NoWait( UINT8 BankID );

/*!
	\brief	Send Play command to DSP
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
int AUDIF_Set_Play( UINT8 BankID );


/*!
	\brief	Send Fastforward command to DSP
	\param	1. BankID [IN] \n
			2. speed [IN]
	\return	AUDIF_Status_e
	\note	1. DSP FLOW CONTROL API\n
			2. the feature is accomplished by SWAPP
*/
int AUDIF_Set_FastForward( UINT8 BankID , UINT16 speed);

/*!
	\brief	Send Pause command to DSP
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
int AUDIF_Set_Pause( UINT8 BankID );

/*!
	\brief	Enable FIFO
	\param	None
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
INT32 AUDIF_Set_FIFOEnable( void );

/*!
	\brief	pause FIFO and will not clean FIFO
	\param	None
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
INT32 AUDIF_Set_FIFOPAUSE( void );

/*!
	\brief	Reset and clean FIFO
	\param	None
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
INT32 AUDIF_Set_FIFOReset( void );

/*!
	\brief	Check FIFO state to avoid decoder transition hang up
	\param	None
	\return	AUDIF_Status_e
	\note	DSP FLOW CONTROL API
*/
INT32 AUD_Check_FIFOState( void );

INT32 AUDF_IS_FIFO_PAUSE(void);
INT32 AUDIF_FIFO_Empty_Status( void );
INT32 AUDIF_Get_FIFO_Depth( UINT8 FIFO_type );
INT32 AUDIF_Set_FIFOSampleCnt( UINT16 mode );
UINT32 AUDIF_Get_FIFOSampleCnt( void );
/*!
	\brief	Send bit stream end command to DSP
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	1. DSP FLOW CONTROL API\n
			2. aud stop cmd related API\n
*/
INT32 AUDIF_Set_BsBufEnd(UINT8 BankID);

/*!
	\brief	Wait ending done reported by dsp
	\param	None
	\return	AUDIF_Status_e
	\note	1. DSP FLOW CONTROL API
			2. aud stop cmd related API\n
*/
INT32 AUDIF_Wait_EndingDone(void);


INT32 AUDIF_Is_EndingDone(void);

//*******************************************************
//	temp API for 8600
//*******************************************************
int AUDIF_DSP3_Get_abv_used(void);
BYTE* AUDIF_Get_abv_start(void);
UINT16 AUDIF_Get_abv_size(void);
int AUDIF_DSP3_Get_abv_free(void);
void AUDIF_Set_BytePtr(UINT32 val );
UINT32 AUDIF_Get_BytePtr(void);

/************************************************************/
/*                                                          */
/*  Audio decode/encode Buffer Control APIs                 */
/*                                                          */
/************************************************************/

/*!
	\brief	Get audio decode bistream buffer "free space"
	\param	BankID [IN]
	\return	bs buffer free size
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
int AUDIF_Get_audBSbuf_free(UINT8 BankID);

/*!
	\brief	Get audio decode bistream buffer "used space"
	\param	BankID [IN]
	\return	bs buffer used size
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
int AUDIF_Get_audBSbuf_used(UINT8 BankID);

/*!
	\brief	Get audio decode bistream buffer "start base"
	\param	BankID [IN]
	\return	DSP_GetBsBufStart()
	\note	1. Audio decode/encode Buffer Control related API\n
*/
BYTE* AUDIF_Get_audBSbuf_start(UINT8 BankID);

/*!
	\brief	Get audio decode bistream buffer "buffer len"
	\param	BankID [IN]
	\return	DSP_GetBsBufSize()
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
INT32 AUDIF_Get_audBSbuf_size(UINT8 BankID);

/*!
	\brief	Reset audio decode bistream buffer
	\param	BankID [IN]
	\return	AUDIF_Status_e
	\note	Audio decode/encode Buffer Control related API
*/
AUDIF_Status_e AUDIF_Reset_Audio_Buf(UINT8 BankID);
INT32 AUDIF_Clean_Audio_Buf(UINT8 BankID);
UINT32 AUDIF_BS_BufFeed( UINT8* pBsSrcBuf, UINT32 aFeedSize );

/*!
	\brief	Reset mp3 encode bistream buffer
	\param	None
	\return	None
	\note	Audio decode/encode Buffer Control related API
*/
void AUDIF_Reset_Audio_Encode_Buf(void);

/*!
	\brief	A mapping of codec bank ID from AUDIF to DSPAPI
	\param	BankID [IN] e.g., AUDIF_BANK_DEC1
	\return	dsp_bankID
	\note
*/
UINT8 AUDIF_Check_CodecBankID(UINT8 BankID);

/*!
	\brief	A mapping of bit stream bank ID from AUDIF to DSPAPI
	\param	BankID [IN] e.g., AUDIF_BANK_DEC1
	\return	DSP_BSBufID
	\note
*/
UINT8 AUDIF_Check_BSBufBankID( UINT8 BankID );

/************************************************************/
/*                                                          */
/*  API for DSP Runtime Control Register                    */
/*                                                          */
/************************************************************/

/*!
	\brief	Get audio decode bistream buffer dump pointer
	\param	BankID [IN]
	\return	dump_ptr
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
UINT32  AUDIF_Get_DumpPtr(UINT8 BankID);

/*!
	\brief	Set audio decode bistream buffer dump pointer
	\param	1. BankID [IN]\n
			2. val [IN]
	\return	None
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
void  AUDIF_Set_DumpPtr(UINT8 BankID,UINT32 val);

/*!
	\brief	Get audio decode bistream buffer barrier pointer
	\param	BankID [IN]
	\return	barrier_ptr
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
UINT32  AUDIF_Get_BarrierPtr(UINT8 BankID);

/*!
	\brief	Set audio decode bistream buffer barrier pointer
	\param	1. BankID [IN]\n
			2. val [IN]
	\return	None
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
void  AUDIF_Set_BarrierPtr(UINT8 BankID,UINT32 val);

/*!
	\brief	Get audio Encode bistream buffer "start base"
	\param	None
	\return	DSP_GetBsBufStart()
	\note	1. Audio decode/encode Buffer Control related API\n
*/
BYTE* AUDIF_Get_Encode_BSbuf_start(void);

/*!
	\brief	Get audio encode bistream buffer dump pointer
	\param	None
	\return	encode_dump_ptr
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
UINT32  AUDIF_Get_Encode_DumpPtr(void);

/*!
	\brief	Set audio encode bistream buffer dump pointer
	\param	val	[IN]
	\return	None
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
void  AUDIF_Set_Encode_DumpPtr(UINT32 val);

/*!
	\brief	Get audio encode bistream buffer barrier pointer
	\param	None
	\return	encode_barrier_ptr
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
UINT32  AUDIF_Get_Encode_BarrierPtr(void);

/*!
	\brief	Set audio encode bistream buffer barrier pointer
	\param	val	[IN]
	\return	None
	\note	1. Audio decode/encode Buffer Control related API\n
			2. return value unit: bytes
*/
void  AUDIF_Set_Encode_BarrierPtr(UINT32 val);

/************************************************************/
/*                                                          */
/*  Get info. or r/w data from DSP                          */
/*                                                          */
/************************************************************/

/*!
	\brief	Get dsp state
	\param	BankID [IN]
	\return	0: TASK_INIT\n
			1: TASK_READY\n
			2: TASK_STOP\n
			3: TASK_PLAY\n
			4: TASK_FF\n
			5: TASK_PAUSE\n
	\note	get/set information from DSP related API
*/
UINT32  AUDIF_Get_DSPState( UINT8 BankID );

/*!
	\brief	Get DSP Codec ID Information
	\param	*pBuf [IN] codec id stream pointer
	\param	id	[IN] id = 0, show main program dsp code information\n
					 id = 1 and up, show decoder dsp code information
	\return	None
	\note	get/set information from DSP related API
*/
void AUDIF_DSP_GetCodecID( char* pBuf , UINT8 id);

/*!
	\brief	Get DSP Codec Version Information
	\param	*pBuf	[IN] codec version stream pointer
	\param	id	[IN] UNUSED_ARGUMENT
	\return	None
	\note	get/set information from DSP related API
*/
void AUDIF_DSP_GetCodecVer( char* pBuf , UINT8 id);

/*!
	\brief	Get the CDDA Type from DSP (HDCD/CDDA)
	\param	None
	\return	1: HDCD \n
			0: CDDA
	\note	get/set information from DSP related API
*/
INT32 AUDIF_Get_CDDAType( void);

/*!
	\brief	Set DTSCD Type
	\param	disc_DTSCDtype_e (0:nonDTSCD, 1:16-bit DTSCD, 2:14-bit DTSCD)
	\return
	\note
*/
void AUDIF_Set_DTSCD_Type(BYTE val);

/*!
	\brief	Get bitrate information from mpeg/aac dsp
	\param	None
	\return
	\note	unit: kbitrate
*/
UINT32 AUDIF_Get_MPEG_AAC_Bitrate(void);



/************************************************************/
/*                                                          */
/*  AUDIO I/O CONTROL APIs                                  */
/*                                                          */
/************************************************************/

/*!
	\brief	Audio DSP MIC control command
	\param	mode [IN] mic_switch_e
	\return	1
	\note	1. MIC Control related API\n
			2. The paramter \a mode is determined by input signal threshold power
*/
int AUDIF_Set_MICStatus(UINT16 mode);

/*!
	\brief	Audio DSP MIC Vol(gain) control command
	\param	tblval	[IN] 0:off ~ 8:max
	\return	1: success\n
			0: fail
	\note	MIC Control related API
*/
int AUDIF_Set_MICVol(UINT8 tblval);

/*!
	\brief	Audio DSP MIC2 Vol(gain) control command
	\param	tblval	[IN] 0:off ~ 8:max
	\return	1: success\n
			0: fail
	\note	MIC Control related API
*/
int AUDIF_Set_MICVol2(UINT8 tblval);

/*!
	\brief	Get mic on/off status from dsp
	\param	None
	\return	HWREG_R(dsp24_port[4])
	\note		MIC Control related API
*/
UINT32 AUDIF_Get_MICState(void);

/*!
	\brief	Audio DSP SPDIF control command
	\param	mode [IN] 0:OFF, 1:PCM, 2:BITSTREAM
	\return
	\note	SPDIF IN/OUT related APIs
*/
INT32 AUDIF_Set_SPDIF(UINT8 mode);

INT32 AUDIF_Set_DigitalOutTable( UINT16 LookupTab[][3], UINT16 ROW, UINT16 COL );
INT32 AUDIF_Set_SPDIF_Capability( UINT16 AudioFormatTag, UINT16 LookupTab[][3] );

/*!
	\brief	Audio DSP HDMI control command
	\param	mode [IN] 0:OFF, 1:PCM, 2:BITSTREAM
	\return
	\note	HDMI IN/OUT related APIs
*/
INT32 AUDIF_Set_HDMI(UINT8 mode);

/*!
	\brief	Audio DSP HDMI control command
	\param	mode	[IN] 0:OFF, 1:PCM, 2:BITSTREAM
	\return
	\note	HDMI IN/OUT related APIs
*/
int AUDIF_SetAudParam_to_HDMI( UINT8 hdmi_output_mode );


#ifdef SUPPORT_QAE377_SPDIF_IN

/*!
	\brief	SPDIF-In Initialization
	\param	[IN] mode 1:SPDIF source form HDMI   0:SPDIF source form Ext. SPDIF
	\return	None
	\note	1. SPDIF IN/OUT related APIs\n
			2. Global variable initialization, pin-mux setting\n
*/
void AUDIF_Init_SpdifIn(UINT16 mode);

/*!
	\brief	SPDIF-In Un-Initialization
	\param	
	\return	None
	\note	1. SPDIF IN/OUT related APIs\n
			2. Unset runMode\n
*/
void AUDIF_UnInit_SpdifIn(void);

/*!
	\brief	SPDIF-In period update
	\param	None
	\return	None
	\note	1. SPDIF IN/OUT related APIs\n
			2. Update fs and bitstream format periodically \n
*/
void AUDIF_Update_SpdifIn(void);


/*!
	\brief	Get SPDIF-In information
	\param	info [IN] a point to the struct of _spdifin_info
	\return	None
	\note	SPDIF IN/OUT related APIs
*/
void AUDIF_Get_SpdifIn_Info(spdifin_info* info);

/*!
	\brief	Get SPDIF-In data format
	\param	None
	\return	spdifin_info.spdifin_mode
	\note	1. SPDIF IN/OUT related APIs\n
			2. The format is defined in auddef.h => SPDIF In format
*/
INT32 AUDIF_Get_SpdifIn_Type(void);

/*!
	\brief	Get sampling rate of SPDIF-In data
	\param	None
	\return	spdifin_info.sample_rate (unit: ksample/sec)
	\note		SPDIF IN/OUT related APIs
*/
	INT32 AUDIF_Get_SpdifIn_Fs(void);

#endif  // SUPPORT_QAE377_SPDIF_IN


//#ifdef SUPPORT_SOUND_EFFECT
/*!
	\brief	DSP mix PCM Data stored in memory
	\param	*PCMptr	[IN] a point to byte-alignment SDRAM address of PCM data
	\param	PCMlen	[IN] byte-alignment PCM data length
	\return	1: success\n
			0: fail
	\note	1. Audio Sound Effect API\n
			2. The SDRAM address must be aligned to 32 words(96 bytes) boundary
*/
//int AUDIF_Set_SoundEffect(UINT8 * PCMptr, UINT32 PCMlen);

/*!
	\brief	Control the sound effect volume
	\param	vol_h	[IN] the high byte of the 24-bit volume
	\param	vol_l	[IN] the lower word of the 24-bit volume
	\return	1: success\n
			0: fail
	\note	Audio Sound Effect API
*/
//int AUDIF_Set_SoundEffect_Vol(UINT8 vol_h, UINT16 vol_l);

/*!
	\brief	DSP mix PCM Data stored in memory
	\param	*pSndEffect	[IN]
	\return	0: success\n
	\note	1. Audio Sound Effect API\n
*/
UINT32 AUDIF_SetSndEffect(SndEffect* pSndEffect);

/*!
	\brief	check if sound effect done
	\param	SndEffectID	[IN] enum SndEffectChannelID
	\return	TRUE/FALSE\n
	\note	1. Audio Sound Effect API\n
*/
UINT32 AUDIF_CheckSndEffectDone(SndEffectChannelID SndEffectID);

/*!
	\brief	Get "unused" sound effect channel ID
	\param	None
	\return	free channel ID
	\note	1. Audio Sound Effect API\n
*/
UINT32 AUDIF_GetFreeMonoSndEffectID(void);

/*!
	\brief	control sound effect mode
	\param	mode	[IN] enum SndeffectMode_e
	\return	none
	\note	1. Audio Sound Effect API\n
*/
void AUDIF_Set_SndEffect_mode( UINT8 mode );

/*!
	\brief	stop sound effect by ID
	\param	SndEffectID	[IN] enum SndEffectChannelID
	\return	TRUE/FALSE\n
	\note	1. Audio Sound Effect API\n
*/
UINT32 AUDIF_StopSndEffect(SndEffectChannelID SndEffectID);

//#endif

/************************************************************/
/*                                                          */
/*  DUAL DSP CONTROL APIs                        */
/*                                                          */
/************************************************************/

/*
	\brief	Set Dec1 MixerGain while enbale Dual Mpeg
	\param	gain	[IN] Decoder 1 volume (from 0~77)
	\param	fade	[IN] fading control parameter (from 0~256)
	\return	AUDIF_FAILURE
	\note	DUAL DSP CONTROL API
*/
UINT32 AUDIF_Set_DualMpegDec1MixerGain(UINT16 gain, UINT16 fade);

/*
	\brief	Set Dec2 MixerGain while enbale Dual Mpeg
	\param	gain	[IN] Decoder 2 volume (from 0~77)
	\param	fade	[IN] panning control parameter (from 0~0xFF)
	\return	AUDIF_FAILURE
	\note	DUAL DSP CONTROL API
*/
UINT32 AUDIF_Set_DualMpegDec2MixerGain(UINT16 gain, UINT16 pan);

/*
	\brief	Set mixer mode while enbale Dual Mpeg
	\param	mode [IN] 0->mixer output\n
						1->output dec1\n
						2->output dec2
	\return
	\note	DUAL DSP CONTROL API
*/
void AUDIF_Set_DualMpegMixerMode(UINT8 mode);

/*
	\brief	Enable decoder2 mode if support for STB
	\param	mode [IN] 0:DualDSP_mode_Disable \n
						1:DualDSP_mode_Enable
	\return AUDIF_FAILURE
	\note	1. DUAL DSP CONTROL API \n
			2. NEED download 2nd dsp after EnableDualDSP
*/
UINT32 AUDIF_EnableDualDSP( UINT8 mode );

/*
	\brief	Get MPEG channel status report by dsp
	\param	void
	\return 0: Stereo\n
			1: Joint_stereo\n
			2: Dual_channel\n
			3: Single_channel
	\note	only used on STB
*/
UINT16 AUDIF_Get_ChannelStatus( void );


INT32 AUDIF_Get_SampleCnt( UINT8 BankID );

/************************************************************/
/*                                                          */
/*  DSP POST-PROCESSING CONTROL APIs                        */
/*                                                          */
/************************************************************/


//======================================
//    Audio Output Mode Control API
//======================================

/*!
	\brief	Audio DSP Output Mode control command
	\param	outmode [IN] UINT8 type
	\param	outparam [IN] UINT16 type
	\return	1: success\n
					0: fail
	\note		1. Audio Output Mode Control API\n
					2. the parameter \a outmode is described as below:\n
					[2-channel channel switch] \n
					0x01:Stereo(L/R) \n
					0x02:L/L \n
					0x03:R/R \n
					0x04:Auto-L \n
					0x05:Auto-R \n
					0x0A:OUT_MixMono \n\n
					[2-channel  vocal cancelation]  \n
					0X0B:OUT_VocalCancel_Auto  \n
					0X0C:OUT_VocalCancel \n\n
					[Downmix mode] \n
					0x06:5.1 channel \n
					0x07:2 channel \n
					0x08:DownMix Lx,Rx(for ProLogic) \n
					0X09:OUT_51DownmixLsRs \n\n
					[Specific AC3 output setting] \n
					0x10:RF_MODE-K3 \n
					0x20:LINE_MODE-K2 \n\n
					[AC3 karaoke mode]  \n
					0x30:no vocal \n
					0x40:left vocal \n
					0x50:right vocal \n
					0x60:both vocal \n\n
					[CUSTOM_MODE_B digital dialnorm -K1(Dynamic compression setting)] \n
					0x80:OUT_CustomModeB_DigitalDialnormK1 \n\n
					[AC3 dual mono mode] \n
					0x90:OUT_Dual_Mono_STEREO \n
					0x91:OUT_Dual_Mono_LL \n
					0x92:OUT_Dual_Mono_RR \n
					0x93:OUT_Dual_Mono_MIX
*/
int AUDIF_Set_OutputMode(UINT8 outmode, UINT16 outparam);

/*!
	\brief	Set test tone mode
	\param	mode [IN] \n
             0x00: sine gen  \n
	         0x01: WHITE NOISE GEN \n
	         0x02: PINK NOISE GEN
	\return	AUDIF_Status_e
	\note	should download emu dsp
*/
INT32 AUDIF_TestTone_mode( UINT8 mode );

/*!
	\brief	Control which speaker output noise generated by emu dsp
	\param	spktype [in]\n
   			 LF_SPKGAIN  = 0x3 \n
   			 RF_SPKGAIN  = 0x4 \n
   			 LS_SPKGAIN  = 0x5 \n
   			 RS_SPKGAIN  = 0x6 \n
   			 C_PKGAIN    = 0x7 \n
   			 SW_SPKGAIN  = 0x8 \n
   			 LB_SPKGAIN  = 0x9 \n
   			 RB_SPKGAIN  = 0xA \n
	\return	AUDIF_Status_e
	\note	should download emu dsp
*/
INT32 AUDIF_TestTone_SpkType( UINT8 spktype );

/*!
	\brief	record value of setup menu item
	\param	type [IN] setup menu setting item
	\param	value [IN] setup menu setting value
	\param	param [IN] setup menu setting param
	\return	None
	\note	1. write value to dsp dram
*/
void AUDIF_SetupMenu_Param( UINT16 type, UINT16 value , BYTE* param );

//======================================
//    Bass Management Related APIs
//======================================

/*!
	\brief	Execute AC3 BASSMANAGEMENT according to 5.1 channel speaker config
	\param	None
	\return	None
	\note		1. Bass Management Related API\n
				2. before using AUDIF_Set_BassConfig(), need to call AUDIF_Init_Speaker_Config()\n
				   or AUDIF_Set_Speaker_Config() first to config all 5.1 channel speaker size
*/
	void AUDIF_Set_BassConfig(void);

/*!
	\brief	Get bass management initialization config
	\param	None
	\return	gBass_Config
	\note		Bass Management Related API
*/
	UINT16 AUDIF_Get_BassConfig(void);

/*!
	\brief	Config speaker size for front/center/surround/subwoofer
	\param	spktype	[IN] enum AUDIF_AppBm_e
	\param	setupvalue	[IN] enum AUDIF_AppBm_e
	\return	None
	\note		1. Bass Management Related API\n
				2. The input parameters are described as below:\n
					[spktype]: [setupvalue] \n
					BM_SPK_FRONT : BM_SPK_LARGE/BM_SPK_SMALL \n
					BM_SPK_SURR  : BM_SPK_LARGE/BM_SPK_SMALL/BM_SPK_OFF \n
					BM_SPK_C     : BM_SPK_LARGE/BM_SPK_SMALL/BM_SPK_OFF \n
					BM_SPK_SUB   : BM_SPK_ON/BM_SPK_OFF \n

*/
	void AUDIF_Set_Speaker_Config(UINT8 spktype , UINT8 setupvalue);

/*!
	\brief	Get speaker size config for front/center/surround/subwoofer
	\param	spktype	[IN] enum AUDIF_AppBm_e
	\return	speaker size(BM_SPK_LARGE\BM_SPK_SMALL\BM_SPK_OFF\BM_SPK_ON): success
	        3: fail
	\note	Bass Management Related API
*/
	UINT8 AUDIF_Get_Speaker_Config(UINT8 spktype);

/*!
	\brief	Initialize speaker size to default value for front/center/surround/subwoofer
	\param	None
	\return	None
	\note		Bass Management Related API
*/
	void AUDIF_Init_Speaker_Config(void);

/*!
	\brief	Subwoofer Speaker switch command
	\param	ustatus	[IN] 0:STATUS_OFF, 1:STATUS_ON
	\return	None
*/
void AUDIF_Set_SubWoof(UINT8 ustatus);


//=======================================
//    Speaker gain control Related APIs
//=======================================

/*!
	\brief	Audio main volume(master gain) control command
	\param	x	[IN] the range of vol \a x is 0~16 (default case)
	\return	1
	\note	1. Speaker gain control Related API\n
			2. volume table is defined in VolTab_qae377
*/
int AUDIF_Set_Volume(UINT8 x);

#ifdef AUDIO_MUTE_BY_PLAT_LAYER_REG
/*!
	\brief	Audio demute command according to "playform layer mute/demute status register" information
	\param	plat_ctrlayer_mute_id	[IN] playform layer mute control id , define in status_reg_def.h(PSREG_STAT_AudioMuteCtrl->MuteID)
	\return	1: success \n
			0: fail
	\note	1. Speaker gain control Related API\n
			2. to demute main volume demute, also mic\5.1-speaker gain demute if needed
*/
INT32 AUDIF_Demute_PLAT_REG(UINT32 plat_ctrlayer_mute_id);
/*!
	\brief	Audio mute command  according to "playform layer mute/demute status register" information
	\param	plat_ctrlayer_mute_id	[IN] playform layer mute control id , define in status_reg_def.h(PSREG_STAT_AudioMuteCtrl->MuteID)
	\return	1: success \n
			0: fail
	\note	1. Speaker gain control Related API\n
			2. to mute main volume, also mic gain mute if needed
*/
INT32 AUDIF_Mute_PLAT_REG(UINT32 plat_ctrlayer_mute_id);
#endif
/*!
	\brief	Audio demute command
	\param	None
	\return	enum AUDIF_Status_e
	\note	1. Speaker gain control Related API\n
			2. to demute main volume demute, also mic\5.1-speaker gain demute if needed
*/
int AUDIF_Demute(void);

/*!
	\brief	Audio mute command
	\param	None
	\return	enum AUDIF_Status_e
	\note	1. Speaker gain control Related API\n
			2. to mute main volume, also mic gain mute if needed
*/
INT32 AUDIF_Mute(void);

/*!
	\brief	get audio volume
	\param	None
	\return	the range of vol is 0~16
	\note	Speaker gain control Related API\n

*/
INT32 AUDIF_Get_volume( void );
/*!
	\brief	get mude/demute status
	\param	None
	\return	1:mute ; 0:demute
	\note	Speaker gain control Related API\n
*/
INT32 AUDIF_Get_MuteStatus(void);

/*!
	\brief	Audio DSP 7 Speaker Gain control command
	\param	spktype	[IN] speaker_type
	\param	val	[IN] gain
	\return	1
	\note	1. Speaker gain control Related API\n
			2. to demute main volume demute, also mic\5.1-speaker gain demute if needed \n
            3. speaker_type: LM_SPKGAIN, RM_SPKGAIN, LF_SPKGAIN, RF_SPKGAIN, LS_SPKGAIN,
               RS_SPKGAIN, C_PKGAIN , SW_SPKGAIN, LB_SPKGAIN, RB_SPKGAIN
			4. gain: 0:off ~ 16:max
*/
int AUDIF_Set_SPKGain(UINT8 spktype, UINT8 val);

/*!
	\brief	Initial each speaker gain value
	\param	value	[IN] gain
	\return	None
	\note	1. Speaker gain control Related API
			2. default max
*/
void AUDIF_Init_SPKGain(UINT8 value);

//============================================
//    Speaker delay& echo control Related APIs
//============================================

/*!
	\brief	Audio DSP Speaker Delay control command
	\param	none
	\return	1: success \n
			0: fail
	\note	1. Speaker delay& echo control Related API\n
			2. Before calling this API, AUDIF_Set_SpkDelay_Config \n
			   should be called first to set delay for each channel \n
*/
int AUDIF_Set_SpkDelay(void);

/*!
	\brief	Audio DSP Speaker Delay control command
	\param	spk	[IN] enum AUDIF_AppBm_e
	\param	delay	[IN] delay time(ms)
	\return	1: success \n
			0: fail
	\note	1. Speaker delay& echo control Related API\n
			2.  The input parameters are described as below:\n
				[spk]       : [delay] \n
				BM_SPK_LF   : 0~5 (ms) \n
				BM_SPK_RF   : 0~5 (ms) \n
				BM_SPK_LS   : 0~25(ms) \n
				BM_SPK_RS   : 0~25(ms) \n
				BM_SPK_LB   : 0~35(ms) \n
				BM_SPK_RB   : 0~35(ms) \n
				BM_SPK_C    : 0~10(ms) \n
				BM_SPK_SUB  : 0~5 (ms) \n
*/
int AUDIF_Set_SpkDelay_Config(UINT8 spk, UINT16 delay);

/*!
	\brief	Initial Speaker Delay value
	\param	None
	\return	None
	\note	1. Speaker delay& echo control Related API
*/
void AUDIF_Init_SpkDelay_Config(void);

/*!
	\brief	set output channel configuration
	\param	None
	\return	AUDIF_Status_e
	\note	1. A downmix mode according to speaker status(on/off) \n
*/
UINT32 AUDIF_Set_SpkOutMode( void );

/*!
	\brief	Initial Speaker Delay value
	\param	A mapping of each speaker switch mode(on/off) \n
			The sequence of return value "spk_cfg" is defined \n
			as follow: (MSB)Rb Lb LEF C Rs Ls Rf Lf \n
			The bit value set to 1 if speaker is on,\n
			Otherwise, set to zero if speaker is off.
	\return	spk_cfg
	\note
*/
UINT8 AUDIF_Get_SpkStatus_Map( void );

/*!
	\brief	Audio DSP ECHO control command
	\param	x	[IN] 0:echo off, 1:light echo ~ 8:heavy echo
	\return	1
	\note	1. Speaker delay& echo control Related API\n
			2. delay: \n
				for Center Channel: 0~5 ms (step: 1ms)\n
				for Surround Channel(Ls,Rs): 0~15 ms (step: 3ms)
*/
int AUDIF_Set_EchoPara(UINT8 x);

/*!
	\brief	Audio DSP ECHO control command
	\param	delay	[IN] 0 ~ 12000 samples
	\param	decay	[IN] 0:small_decay ~ 127:heavy_decay
	\return	1
	\note	1. Speaker delay& echo control Related API\n
			2. decay speed EX: 63 => (63/128) => 0.5 decay speed
*/
int AUDIF_Set_Echo(UINT8 delay, UINT16 decay);


//============================================
//    PPROLOGICII Effect Control Related APIs
//============================================

/*!
	\brief	Audio ProLogicII control
	\param	mode	[IN] prologic II command set
	\return	1: success\n
			0: fail
	\note	PPROLOGICII Effect Control Related API
*/
	int AUDIF_Set_ProLogicII(UINT8 mode);

/*!
	\brief	Determine if prologic II is in specific decoder mode or not
	\param	mode	[IN] prologic II Decoder Mode Select command set
	\return	1: yes\n
			0: no
	\note	PPROLOGICII Effect Control Related API
*/
	UINT8 AUDIF_IS_PLII_MODE(UINT8 mode);

/*!
	\brief	Get prologic II initial configuration word
	\param	None
	\return
	\note		PPROLOGICII Effect Control Related API
*/
	UINT32 AUDIF_Get_PLII_Init_Config(void);

/*!
	\brief	Set prologic II initial configuration word
	\param	val [IN]
	\return	None
	\note		PPROLOGICII Effect Control Related API
*/
	void AUDIF_Set_PLII_Init_Config(UINT32 val);

/*!
	\brief	Set prologicII decoder mode of initial configuration word
	\param	mode	[IN] prologic II Decoder Mode Select command set(ref auddef.h)
	\return	None
	\note		PPROLOGICII Effect Control Related API
*/
	void AUDIF_Set_PLII_Init_Mode(UINT8 mode);


//============================================
//    PPROLOGICII Effect Control Related APIs
//============================================

/*!
	\brief	Audio GM7 control
	\param	mode [IN] GM5_MODE_OFF/GM5_MODE_MUSIC/GM5_MODE_MOIVE
	\return	1: success\n
			0: fail
	\note	GM5 Effect Control Related API
*/
int AUDIF_Set_GM7(UINT16 mode);



//============================================
//    DTS Neo6 Effect Control Related APIs
//============================================

/*!
	\brief	Audio Neo6 initial
	\param	void
	\return	1
	\note	Neo6 Effect Control Related API
*/
    UINT8 AUDIF_Init_Neo6(void);

/*!
	\brief	Audio Neo6 set channel config
	\param	mode [IN] channel (enum AUDIF_Neo6_e)
	\return	1: success\n
			0: fail
	\note	Neo6 Effect Control Related API
*/
    UINT8 AUDIF_Set_Neo6_Ch(UINT8 ch);

/*!
	\brief	Audio Neo6 set mode
	\param	mode	[IN] mode (enum AUDIF_Neo6_e)
	\return	1: success\n
			0: fail
	\note	Neo6 Effect Control Related API
*/
    UINT8 AUDIF_Set_Neo6_Mode(UINT8 mode);

/*!
	\brief	Audio Neo6 set center gain
	\param	mode	[IN] gain (enum AUDIF_Neo6_e)
	\return	1: success\n
			0: fail
	\note	Neo6 Effect Control Related API
*/
    UINT8 AUDIF_Set_Neo6_Gain(UINT8 gain);

//============================================
//    SRS TruVolume Control Related APIs
//============================================
/*!
	\brief	Audio TruVol initial
	\param	void
	\return	1
	\note	TruVol Effect Control Related API
*/
    UINT8 AUDIF_Init_TruVol(void);

/*!
	\brief	Audio TruVolume set mode
	\param	mode	[IN] mode (enum AUDIF_TruVol_e)
	\return	1: success\n
			0: fail
	\note	TruVolume Effect Control Related API
*/
    UINT8 AUDIF_Set_TruVol_Mode(UINT8 mode);

/*!
	\brief	Audio TruVolume set output gain
	\param	mode	[IN] gain (0~20)
	\return	1: success\n
			0: fail
	\note	TruVolume Effect Control Related API
*/
    UINT8 AUDIF_Set_TruVol_OutGain(UINT8 gain);

//============================================
//    3-Dimension
//	& Equalizier
//	& Key Shift Effect Control Related APIs
//============================================

/*!
	\brief	Audio 3D Surround control command
	\param	stype	[IN] enum AUDIF_App3D_e
	\return	1: success\n
			0: fail
	\note	Audio Post-Processing Control API
*/
int AUDIF_Set_Surround(UINT8 stype);

/*!
	\brief	Audio EQ control command
	\param	eqtype	[IN] enum AUDIF_AppEq_e
	        *p      [IN] the pointer of the gains for 7 bands
	                     the range of gain: 0~24 map to -12dB~+12dB
	                     p[0]: the lowest band; p[6]: the highest band
	\return	1: success\n
			0: fail
	\note	1. Audio Post-Processing Control API\n
			2. eqtype description:\n
			   in case of EQ_USER_DEFINED, be sure that *p is set properly
			   in other cases, *p is useless
*/
int AUDIF_Set_EQType(UINT8 eqtype, BYTE *p);




/*!
	\brief	Get specific band of EQ spectrum gain value
	\param	band	[IN] enum AUDIF_AppEq_e \n
	\                    EQ_BAND_0 ~ EQ_BAND_6
	\return
	\note		1. Audio Post-Processing Control API\n
*/
void AUDIF_Get_EQ_Spectrum(UINT16 *p_eq);

/*!
	\brief	Get specific band of EQ spectrum gain value from MIC source
	\param	band	[IN] (0~7)specific band of EQ spectrum gain value needed to get back
	\return
	\note		1. Audio Post-Processing Control API\n
					2. Before using it, need to call AUDIF_Set_EQ_Show_Spectrum() first\n
					3. We now only support 5-band or 7-band EQ
*/
UINT32 AUDIF_Get_Mic_EQ_Spectrum(UINT8 band);

/*!
	\brief	Audio key-shift control command
	\param	keyshift	[IN] enum AUDIF_AppKs_e
	\return	1: success\n
			0: fail
	\note	1. Audio Post-Processing Control API\n
*/
int AUDIF_Set_Key(UINT8 keyshift);

void AUDIF_APP_ChannelNum( void );

//============================================
//     Scoring Related APIs
//============================================

/*!
	\brief	Karaoke Vocal Scoring
	\param	uCmdType	[IN] the channel need to scroe
	\return	1: success\n
					0: fail
	\note		1. Scoring Related API\n
					2. parameter \a uCmdType description\n
				vocal_off(0x00): off\n
				vocal_L(0x01): Left Channel\n
				vocal_R(0x02): Right Channel\n
*/
INT32 AUDIF_Set_VocalScoring(UINT16 uCmdType);
INT32 AUDIF_Set_AC3Converter( UINT16 mode );
UINT32 AUDIF_Set_Karaoke_SndEffect( UINT16 mode );

/*!
	\brief	Get karaoke microphone scoring
	\param	None
	\return	0~100
	\note		Scoring Related API
*/
UINT32 AUDIF_Get_Final_Score(void);



//============================================
//   Polling DSP Debug Messeage related APIs
//============================================

/*!
	\brief	Get karaoke channel switch mode
	\param	None
	\return	[BYTE]channel switch mode, ref ch_mode_e emun
	\note
*/
BYTE AUDIF_Get_ChnSwitch_Mode( void );

/*!
	\brief	Get AC3 karaoke mode
	\param	None
	\return	[BYTE]channel switch mode, ref AC3_Karaoke_e emun
	\note
*/
BYTE AUDIF_Get_AC3Karaoke_Mode( void );

BYTE AUDIF_Get_Karaoke_MAXNum( void );

/************************************************************/
/*                                                          */
/*  Other APIs                                              */
/*                                                          */
/************************************************************/

/*!
	\brief	DSP watch dog mechanism
	\param	void
	\return	None
	\note	Other API
*/
void AUDIF_Watchdog(void);

/*!
	\brief	Ramp Up PCM output path of audio harware
	\param	void
	\return	None
	\see	AUDIF_WaitRampDown()
	\note	1. Audio HW Ramp Control API\n
			2. should group AUDIF_WaitRampDown to make sure rampdown fully complete
*/
void AUDIF_RampDown_PCM_Out(void);

/*!
	\brief	Ramp Down PCM output path of audio harware
	\param	void
	\return	None
	\note	Audio HW Ramp Control API\n
*/
void AUDIF_RampUp_PCM_Out(void);

/*!
	\brief	make sure ramp down fully complete
	\param	void
	\return	None
	\see	AUDIF_RampDown_PCM_Out()
	\note	1. Audio HW Ramp Control API\n
			2. according to FIFO count and sample rate to calculate the delay time
*/
UINT32 AUDIF_WaitRampDown(void);

/*!
  \def      AUDIF_DSP3_Decompress(rom,dst)
  \brief    De-compressed gzip compressed binary to dedicated memory spapce
*/
#define AUDIF_DSP3_Decompress(rom,dst) dsp3_decompress(rom,dst)


/*!
	\brief	Set copyright protection related information(CGMS) to dsp(L bit/CP bit)
	\param	CCA_type [IN] unlimited_copy/one_copy_allowed/no_copy_allowed
	\return	None
	\note	Audio channel & CCA setting related API
*/
void AUDIF_Set_CCA(UINT8 CCA_type);

/*!
	\brief	Set copyright protection related information(CGMS) to dsp(L bit/CP bit)
	\param	CGMS_type	[IN]
	\param	bit_res		[IN]
	\param	disc_type	[IN]
	\return	None
	\note	Audio channel & CCA setting related API
*/
void AUDIF_Set_Spdif_ChannelStatus(UINT8 CGMS_type , UINT8 bit_res, UINT8 disc_type);


/************************************************************/
/*                                                          */
/* Function name redefine                                   */
/* (for thoese the same interfaces between AUDIF & dspAPIs) */
/*                                                          */
/************************************************************/

/*!
	\brief	Get dsp message
	\param	*dspMsg [IN]
	\see	dspGetMessage()
	\note
*/
UINT32 AUDIF_Get_DspMessage(UINT32* dspMsg);

/*!
	\brief	Get dsp message
	\param	void
	\see	dspGetMessage()
	\note
*/
UINT32 AUDIF_Get_DBGMsg(void);


/*!
    \fn	AUDIF_Get_DBGFlag
	\brief	Get dsp debug messeage event flag
	\param	void
	\see	dspGetMessageFlag()
	\note
*/
UINT32 AUDIF_Get_DBGFlag(void);


/*!
    \fn	AUDIF_Set_DBGFlag
	\brief	Set dsp debug messeage event flag
	\param	void
	\see	dspSetMessageFlag()
	\note
*/
void AUDIF_Set_DBGFlag(void);


/*!
    \fn	AUDIF_Get_DspParam
	\brief	get dsp parameters from dsp
	\param	nOffset [IN]
	\see	DSP_GetDspParam0()
	\note
*/
UINT16 AUDIF_Get_DspParam(UINT16 nOffset);

/*!
    \fn	AUDIF_Set_RiscParams
	\brief	Set risc parameters to dsp
	\param	nOffset [IN]
	\param	nValue [IN]
	\see	DSP_SetRiscParam0()
	\note
*/
UINT32 AUDIF_Set_RiscParam(UINT16 nOffset,UINT16 nValue);

/*!
    \fn	AUDIF_Get_DspInfo
	\brief	Generic API fr get dsp internal information (globals/memory address)
	\param	Info [IN]
	\see	dspGetInfo()
	\note
*/
UINT32 AUDIF_Get_DspInfo(UINT32 Info);




//============================================
//    NES GAME Flow Control &
//    Initialization related APIs
//============================================

#ifdef SUPPORT_NESGAME //Jeff 20020521

/*!
	\brief	set play command for NES game
	\param	None
	\return	None
	\note	NES GAME Flow Control API
*/
	void AUDIF_NES_Set_Play(void);

/*!
	\brief	set stop command for NES game
	\param	None
	\return	None
	\note	NES GAME Flow Control API
*/
	void AUDIF_NES_Set_Stop(void);

/*!
	\brief	set spdif output on/off command for NES game
	\param	mode	[IN] 0: off,  1:on
	\return	None
	\note	NES GAME Flow Control API
*/
	void AUDIF_NES_Set_SPDIF(UINT8 mode);

/*!
	\brief	set dac output on/off command for NES game
	\param	mode	[IN] 0: off,  1:on
	\param	type_dac	[IN] dac format configuration
	\param	playbackrate	[IN]1: 32k, 2: 44.1k
	\return	None
	\note	NES GAME Flow Control API
*/
	void AUDIF_NES_Set_DAC(UINT8 mode , UINT8 type_dac , UINT8 playbackrate);

#endif

//============================================
//  DVD Audio Watermark Check related APIs
//============================================

#ifdef DVD_AUDIO    //ycwen 2005/5/9 DVD AUDIO Watermark
/*!
	\brief	Check DVD Audio Watermark
	\param	None
	\return	1: Watermark exist \n
					0: no Watermark information
	\note		DVD Audio Watermark Check related API
*/
	UINT8 AUDIF_Check_DVD_Audio_WaterMark(void);

/*!
	\brief	Initialize DVD Audio Watermark
	\param	None
	\return	None
	\note		DVD Audio Watermark Check related API
*/
	void AUDIF_Initialize_DVD_Audio_WaterMark(void);

#endif


#if defined(SUPPORT_MIDI)
/*!
	\brief	MIDI Flow Control command
	\param	mode	[IN] control mode
	\param	param	[IN] audio only flag 1:audio only 0:not audio only
	\return	None
	\note		1. Other API\n
					2. the parameter \a mode is only supported as follows: \n
							AUDIOPLAY \n
							AUDIOFF1_25X \n
							AUDIOFF1_50X \n
							AUDIOFF1_75X \n
							AUDIOFF2_00X \n
							AUDIOSF1_25X \n
							AUDIOSF1_50X \n
							AUDIOSF1_75X \n
							AUDIOSF2_00X \n
*/
	void AUDIF_MIDI_Set_Flow_Ctrl(UINT8 mode , UINT16 param);
#endif

#ifdef SUPPORT_REP_READ//Maoyong 2004.05.26
/*!
	\brief	DSP command for record and repeat reading control
	\param	uCmdType	[IN] type of repeat reading DSP command
	\return	1: success \n
					0: fail
	\note		Other API
*/
	int AUDIF_Set_RepRead(UINT8 uCmdType);
#endif


/*!
	\brief	Turn OFF DAC
	\param	None
	\return	None
	\note	Provide to AP layer
*/
void AUDIF_PowerDown_DAC( void );

void AUDIF_Set_DAC_Output( UINT8 DAC_cfg );
UINT8 AUDIF_Get_DAC_Output( void );
void AUDIF_Set_DAC_Type( UINT8 DAC_Type , UINT16 num_output_ch );
void AUDIF_Set_PCM_CFG( UINT16 type );
INT32 AUDIF_Set_PSREG_ChnNum( void );
INT32 AUDIF_Get_Channel_Mask(UINT8 BankID, UINT8 *dsp_chnnum );
INT32 AUDIF_Get_Channel_Number(UINT8 BankID);
INT32 AUDIF_Get_BitRate(UINT8 BankID);

//*******************************************************
//	 Audio IEC0 spdif delay related APIs
//*******************************************************
UINT16 AUDIF_Get_SPDIF_Fs(void);
UINT32 AUDIF_Get_IEC0_Delay(void);
void AUDIF_Set_IEC0_Delay(UINT16 msec);

/*******IEC0 Delay command set only for STB*******/
UINT32 AUDIF_EnableIEC0Delay( UINT8 mode );

//
// dylan: 9200 related codes
//

#define AUD_AUDIO_LEVEL_ADJUST_VALUE_MIN     (-16)     //-16 dB
#define AUD_AUDIO_LEVEL_ADJUST_VALUE_MAX     (15)      //+15 dB

#define AUD_AUDIO_MASTER_VOLUME_VALUE_MIN    (-672)    //-84 dB
#define AUD_AUDIO_MASTER_VOLUME_VALUE_MAX    (192)     //+24 dB

#define AUD_AUDIO_MAIN_VOLUME_VALUE_MIN      (-84)     //-84 dB
#define AUD_AUDIO_MAIN_VOLUME_VALUE_MAX      (24)      //+24 dB

#define AUD_AUDIO_AUX_VOLUME_VALUE_MIN       (-84)     //-84 dB
#define AUD_AUDIO_AUX_VOLUME_VALUE_MAX       (24)      //+24 dB

#define AUD_AUDIO_MAIN_BASS_VALUE_MIN        (-16)     //-16 dB
#define AUD_AUDIO_MAIN_BASS_VALUE_MAX        (15)      //+15 dB

#define AUD_AUDIO_CENTER_BASS_VALUE_MIN      (-16)     //-16 dB
#define AUD_AUDIO_CENTER_BASS_VALUE_MAX      (15)      //+15 dB

#define AUD_AUDIO_SURROUND_BASS_VALUE_MIN    (-16)     //-16 dB
#define AUD_AUDIO_SURROUND_BASS_VALUE_MAX    (15)      //+15 dB

#define AUD_AUDIO_MAIN_TREBLE_VALUE_MIN      (-16)     //-16 dB
#define AUD_AUDIO_MAIN_TREBLE_VALUE_MAX      (15)      //+15 dB

#define AUD_AUDIO_CENTER_TREBLE_VALUE_MIN    (-16)     //-16 dB
#define AUD_AUDIO_CENTER_TREBLE_VALUE_MAX    (15)      //+15 dB

#define AUD_AUDIO_SURROUND_TREBLE_VALUE_MIN  (-16)     //-16 dB
#define AUD_AUDIO_SURROUND_TREBLE_VALUE_MAX  (15)      //+15 dB

#define AUD_AUDIO_EQUALIZER_VALUE_MIN        (-12)     //-12 dB
#define AUD_AUDIO_EQUALIZER_VALUE_MAX        (12)      //+12 dB

#define AUD_AUDIO_LIP_SYNC_VALUE_MIN         (0)       // 0 ms
#define AUD_AUDIO_LIP_SYNC_VALUE_MAX         (63)      //252 ms

/***************************************************************************
	Type definition
 ***************************************************************************/

/*Default parameters for Aud_AudioSetLipsSync function*/
#define Aud_AUDIO_LIP_SYNC_DELAY_SWITCH_DEFAULT                          DTV_FALSE   ////DTV_TRUE : enable lip sync function, DTV_FALSE : disable lip sync function
#define AUD_AUDIO_LIP_SYNC_DELAY_DEFAULT                                 25          //0 ~ 63(1 step is 4 ms)

/*Bass management default values*/
//#define PL_AUDIO_BASS_MANAGEMENT_MODE_DEFAULT                           DTV_DRV_AUD_BASSMANAGEMENT_OFF
#define AUD_AUDIO_BASS_MANAGEMENT_FILTER_CHARACTERISTIC_DEFAULT          0
#define AUD_AUDIO_BASS_MANAGEMENT_SUBWOOFER_FILTER_SWITCH_DEFAULT        DTV_TRUE

#define SPAL_AUDIO_AVL_MODE_OFF                                      	Aud_AUDIO_AVL_MODE_OFF
#define SPAL_AUDIO_AVL_MODE_ON                                       	Aud_AUDIO_AVL_MODE_02S
#define SPAL_AUDIO_AVL_REFERENCE_LEVEL_DEFAULT                          7          //0 (-6 dBFS)~ 15(-36 dBFS)
#define SPAL_AUDIO_AVL_WEIGHT_FILTER_SWITCH_DEFAULT                     DTV_FALSE  //DTV_TRUE : enable weight filter, DTV_FALSE : disable weight filter
#define SPAL_AUDIO_AVL_PEAKMODE_WITH_CLIP_MANAGEMENT_SWITCH_DEFAULT     DTV_FALSE  //DTV_TRUE : enable peak mode with clip management, DTV_FALSE : disable peak mode with clip management

#define AUD_NICAM_ERROR_MIN         	(0)
#define AUD_NICAM_ERROR_MAX         	(255)

#define AUD_SIF_AGC_MIN	                (-6)
#define AUD_SIF_AGC_MAX	                (25)

/***************************************************************************
	Enum definition
 ***************************************************************************/
#if 0
typedef enum
{
   /* common status values */
   AUD_SUCCESS,						/* successful outcome */
   AUD_ERR_INVALID_PARAM,			/* invalid parameter */
   AUD_ERR_INVALID_OP,				/* requested operation is invalid */
   AUD_ERR_MEMORY_ALLOC,			/* problem allocating memory */
   AUD_ERR_BUFFER_UNDERFLOW,		/* not enough stream data is available to decode the next video frame */
   AUD_ERR_RESOURCE_UNAVAILABLE,	/* hardware resource is in use or otherwise unavailable */
   AUD_ERR_FAILURE,
   AUD_WARN_NO_ACTION,				/* the function completed successfully, but no action was taken */
   AUD_WARN_PARAM_CLIPPED,			/* the function completed successfully, though a parameter was clipped to within a valid range.*/
   AUD_WARN_BUFFER_EMPTY,			/* buffer became empty before the requested amount of data could be read */
   AUD_WARN_BUFFER_FULL,			/* buffer became full before the requested amount of data could be written */
} Aud_Status_t;
#endif

typedef enum
{
   /* common status values */
   AUD_ERR_INVALID_PARAM        = -6,   /* invalid parameter */
   AUD_ERR_INVALID_OP           = -5,   /* requested operation is invalid */
   AUD_ERR_MEMORY_ALLOC         = -4,   /* problem allocating memory */
   AUD_ERR_BUFFER_UNDERFLOW     = -3,   /* not enough stream data is available to decode the next video frame */
   AUD_ERR_RESOURCE_UNAVAILABLE = -2,   /* hardware resource is in use or otherwise unavailable */
   AUD_ERR_FAILURE              = -1,

   AUD_SUCCESS                  =  0,	/* successful outcome */

   AUD_WARN_NO_ACTION           =  1,	/* the function completed successfully, but no action was taken */
   AUD_WARN_PARAM_CLIPPED       =  2,	/* the function completed successfully, though a parameter was clipped to within a valid range.*/
   AUD_WARN_BUFFER_EMPTY        =  3,	/* buffer became empty before the requested amount of data could be read */
   AUD_WARN_BUFFER_FULL         =  4,	/* buffer became full before the requested amount of data could be written */
} AUDIF_Status_t;

typedef enum
{
    AUD_AUDIO_EFFECT_MODE_NORMAL								= 0,
    /*only one effect can be chosen at one time*/
	AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL,
    AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX,
    AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL,
	AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX,
    AUD_AUDIO_EFFECT_MODE_NOISE_SEQUENCING,
} Aud_AudioEffectMode_t;

typedef enum
{
    AUD_AUDIO_SPATIAL_EFFECT_NORMAL			= 0,
    /*only one effect can be chosen at one time*/
    AUD_AUDIO_SPATIAL_EFFECT_ESS,            //I-Stereo
    AUD_AUDIO_SPATIAL_EFFECT_EPS,            //I-Mono
    AUD_AUDIO_SPATIAL_EFFECT_SRS_WOW,
    AUD_AUDIO_SPATIAL_EFFECT_AUTO_ESS_EPS,
    AUD_AUDIO_SPATIAL_EFFECT_MAX,
} Aud_AudioSpatialEffect_t;

typedef enum
{
    AUD_AUDIO_BAKCEND_EFFECT_NORMAL			= 0,
    /*only one effect can be chosen at one time*/
    AUD_AUDIO_BAKCEND_EFFECT_DVB,
    AUD_AUDIO_BAKCEND_EFFECT_DBB,
    AUD_AUDIO_BAKCEND_EFFECT_DVB_SUBWOOFER,
    AUD_AUDIO_BAKCEND_EFFECT_DBB_SUBWOOFER,
} Aud_AudioBackendEffect_t;

typedef enum
{
    /*only one effect can be chosen at one time*/
    AUD_AUDIO_SUBWOOFER_SOURCE_NONE		= 0,
    AUD_AUDIO_SUBWOOFER_SOURCE_RLC_TYPE = 1, //redirect right channel¡Bleft channel and center channel to subwoofer channel
    AUD_AUDIO_SUBWOOFER_SOURCE_RL_TYPE  = 2,  //redirect right channel and left channel to subwoofer channel
    AUD_AUDIO_SUBWOOFER_SOURCE_RLC_ENLARGE_TYPE = 3,
    AUD_AUDIO_SUBWOOFER_SOURCE_MAX_TYPE = 3,
} Aud_AudioSubwooferSource_t;

typedef enum
{
    AUD_AUDIO_NORMAL_SUBWOOFER_MODE		= 0,
    AUD_AUDIO_ECONOMIC_SUBWOOFER_MODE   = 1,
    AUD_AUDIO_MAX_SUBWOOFER_MODE        = 1,    
} Aud_AudioSubwooferMode_t;

typedef enum
{
    Cufoff_50Hz = 0,
    Cufoff_60Hz,
    Cufoff_70Hz,
    Cufoff_80Hz,
    Cufoff_90Hz,    
    Cufoff_100Hz,
    Cufoff_110Hz,
    Cufoff_120Hz,
    Cufoff_130Hz,
    Cufoff_140Hz,
    Cufoff_150Hz,
    Cufoff_200Hz,
    Cufoff_250Hz,
    Cufoff_300Hz,
    Cufoff_350Hz,    
    Cufoff_400Hz,    
} Aud_AudioSubwooferCutoff_t;

typedef enum
{
    AUD_AUDIO_AVL_MODE_OFF    = 0,
    AUD_AUDIO_AVL_MODE_20MS,
    AUD_AUDIO_AVL_MODE_02S,
    AUD_AUDIO_AVL_MODE_04S,
    AUD_AUDIO_AVL_MODE_08S,
    AUD_AUDIO_AVL_MODE_16S,
} Aud_AudioAVLMode_t;

typedef enum
{
    AUD_AUDIO_ISOUND_INTENSITY_EFFECT_0    = 0,
    AUD_AUDIO_ISOUND_INTENSITY_EFFECT_1,
    AUD_AUDIO_ISOUND_INTENSITY_EFFECT_2,
    AUD_AUDIO_ISOUND_INTENSITY_EFFECT_3,
    AUD_AUDIO_ISOUND_INTENSITY_EFFECT_4,
    AUD_AUDIO_ISOUND_INTENSITY_EFFECT_5,
} Aud_AudioISoundIntensityEffect_t;

typedef enum
{
    AUD_AUDIO_ISOUND_FADETIME_01_1_SEC   = 0,
    AUD_AUDIO_ISOUND_FADETIME_02_2_SEC,
    AUD_AUDIO_ISOUND_FADETIME_03_3_SEC,
    AUD_AUDIO_ISOUND_FADETIME_04_5_SEC,
    AUD_AUDIO_ISOUND_FADETIME_06_8_SEC,
    AUD_AUDIO_ISOUND_FADETIME_09_0_SEC,
    AUD_AUDIO_ISOUND_FADETIME_14_5_SEC,
    AUD_AUDIO_ISOUND_FADETIME_19_0_SEC,
    AUD_AUDIO_ISOUND_FADETIME_136_MSEC,
    AUD_AUDIO_ISOUND_FADETIME_272_MSEC,
    AUD_AUDIO_ISOUND_FADETIME_410_MSEC,
    AUD_AUDIO_ISOUND_FADETIME_546_MSEC,
    AUD_AUDIO_ISOUND_FADETIME_820_MSEC,
    AUD_AUDIO_ISOUND_FADETIME_01_8_SEC,
    AUD_AUDIO_ISOUND_FADETIME_02_4SEC,
} Aud_AudioISoundFadeTime_t;

typedef enum
{
    AUD_AUDIO_ISOUND_DETECTORTIME_01SEC   = 0,
    AUD_AUDIO_ISOUND_DETECTORTIME_05SEC,
    AUD_AUDIO_ISOUND_DETECTORTIME_08SEC,
    AUD_AUDIO_ISOUND_DETECTORTIME_14SEC,
} Aud_AudioISoundDetectorTime_t;

typedef enum _DDP_Converter_Mode_e
{
	DDP_Converter_Disable	= 0x0,
	DDP_Converter_Enable	= 0x1,
}DDP_Converter_Mode_e;
typedef enum
{
    AUD_AUDIO_ACC_PRIMARY_PATH       = 0,
    AUD_AUDIO_ACC_PRIMARY_CENTER,
    AUD_AUDIO_ACC_PRIMARY_SURROUND,
} Aud_AudioACCPath_t;

typedef enum
{
    AUD_AUDIO_CLIP_MANAGEMENT_OFF                     = 0,
    AUD_AUDIO_CLIP_MANAGEMENT_STATIC_VOLUME_MODE,
    AUD_AUDIO_CLIP_MANAGEMENT_STATIC_CONTROL_MODE,
    AUD_AUDIO_CLIP_MANAGEMENT_DYNAMIC_CONTROL_MODE,
    AUD_AUDIO_CLIP_MANAGEMENT_DYNAMIC_VOLUME_MODE,
} Aud_AudioClipManagementMode_t;

typedef enum
{
    AUD_AUDIO_DVM_PEAK_DETECTION_SELECT_MAIN                  = 0,
    AUD_AUDIO_DVM_PEAK_DETECTION_SELECT_MAIN_CENTER_SUBWOOFER,
} Aud_AudioDVMPeakDetectionSelection_t;

typedef enum
{
    AUD_AUDIO_PATH_PRIMARY			= 0,
    AUD_AUDIO_PATH_AUX1,
    AUD_AUDIO_PATH_AUX2,
    AUD_AUDIO_PATH_AUX3,
    AUD_AUDIO_PATH_CENTER_SURROUND,
    AUD_AUDIO_PATH_CENTER_SUBWOOFER,
    AUD_AUDIO_PATH_SURROUND_SUBWOOFER,
} Aud_AudioPath_t;



typedef enum
{
    AUDIF_ADC_SOURCE_ADC0   = 0x0,  /*!scart*/
    AUDIF_ADC_SOURCE_ADC1   = 0x5,  /*!default*/
    AUDIF_ADC_SOURCE_ADC2   = 0xA,
    AUDIF_ADC_SOURCE_ADC3   = 0xF,
    AUDIF_ADC_SOURCE_MAX    = 0xF,
} AUDIF_AdcSourceSelect_e;

typedef enum
{
    AUDIF_DAC_MAP_PCM0_L    = 0x0,
    AUDIF_DAC_MAP_PCM0_R    = 0x1,
    AUDIF_DAC_MAP_PCM1_L    = 0x2,
    AUDIF_DAC_MAP_PCM1_R    = 0x3,
    AUDIF_DAC_MAP_PCM2_L    = 0x4,
    AUDIF_DAC_MAP_PCM2_R    = 0x5,
    AUDIF_DAC_MAP_PCM_MASK  = 0x7,
} AUDIF_DacMapping_e;

typedef enum
{
    AUDIF_MAIN_PATH_MIXER       = 0x0,
    AUDIF_MAIN_PATH_SIF         = 0x1,
    AUDIF_MAIN_PATH_AUX         = 0x2,    
} AUDIF_MainPath_e;

typedef enum
{
    AUDIF_AUX1_PATH_MIXER     = 0x0,
    AUDIF_AUX1_PATH_SIF       = 0x1,    
    AUDIF_AUX1_PATH_AUX       = 0x2,    
    AUDIF_AUX1_PATH_OFF       = 0x3,        
    AUDIF_AUX1_PATH_LFE       = 0x4,            
    AUDIF_AUX1_PATH_MAX       = 0x4,                
} AUDIF_Aux1Path_e;

typedef enum
{
    AUDIF_AUX2_PATH_MIXER     = 0x0,
    AUDIF_AUX2_PATH_SIF       = 0x1,    
    AUDIF_AUX2_PATH_AUX       = 0x2,    
    AUDIF_AUX2_PATH_OFF       = 0x3,        
    AUDIF_AUX2_PATH_MAX       = 0x3,                    
} AUDIF_Aux2Path_e;

typedef enum
{
    AUD_AUDIO_MAINLOUDNESS_LEVEL_MINUS_15DB    = 0,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_MINUS_12DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_MINUS_09DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_MINUS_06DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_MINUS_03DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_00DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_03DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_06DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_09DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_12DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_15DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_18DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_21DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_24DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_27DB,
    AUD_AUDIO_MAINLOUDNESS_LEVEL_PLUS_30DB,
} Aud_AudioMainLoudnessVolumeLevel_t;


typedef enum
{
    AUD_REPORT_INDIRECT	= 0,//report from audio manager task
    AUD_REPORT_DIRECT,			//direct report from hardware status, for standard report when channel scanning
} Aud_ReportMethod_e;

// dsp_param1[0] (bit 4:0) : dsp report standard detection
typedef enum
{
    AUD_SIF_STANDARD_FAIL	= 0,
    AUD_SIF_STANDARD_BG,
    AUD_SIF_STANDARD_DK,
    AUD_SIF_STANDARD_M,
    AUD_SIF_STANDARD_BG_A2,
    AUD_SIF_STANDARD_BG_NICAM,
    AUD_SIF_STANDARD_DK_A2_1,
    AUD_SIF_STANDARD_DK_A2_2,
    AUD_SIF_STANDARD_DK_A2_3,
    AUD_SIF_STANDARD_DK_NICAM,
    AUD_SIF_STANDARD_L_NICAM,
    AUD_SIF_STANDARD_I_NICAM,
    AUD_SIF_STANDARD_M_KOREA,
    AUD_SIF_STANDARD_M_BTSC,
    AUD_SIF_STANDARD_M_EIAJ,
    AUD_SIF_STANDARD_FM_10_7_IF_50_US,
    AUD_SIF_STANDARD_FM_10_7_IF_75_US,
    AUD_SIF_STANDARD_FM_SELECTABLE_IF_50_US,        /* Need to check ?? */
    AUD_SIF_STANDARD_FM_SELECTABLE_IF_75_US,        /* Need to check ?? */
    AUD_SIF_STANDARD_BUSY = 31,
} Aud_SIFStandard_e;

// dsp report available SIF mode
typedef enum
{
    AUD_SIF_AVAIL_BUSY                 = 0x00000000,
    AUD_SIF_AVAIL_ANALOG_MONO          = 0x00000001,
    AUD_SIF_AVAIL_NICAM_MONO           = 0x00000002,
    AUD_SIF_AVAIL_STEREO               = 0x00000004,
    AUD_SIF_AVAIL_DUAL                 = 0x00000008,
    AUD_SIF_AVAIL_SAP                  = 0x00000010,
    AUD_SIF_AVAIL_NICAM                = 0x00000020,
    AUD_SIF_AVAIL_FORCED_MONO          = 0x00000040,
    AUD_SIF_AVAIL_BTSC_PILOT_OVERFLOW  = 0x80000000,
} Aud_SIFAvailableMode_e;

// auddrv set standard mask to dsp (_Demdec_Set_SifStandardMask)
typedef enum
{
    AUD_SIF_STANDARD_ASD_MODE_MASK_BG       = (0x1),	// B/G(H) (2 possible stereo standards)  5.5 MHz
    AUD_SIF_STANDARD_ASD_MODE_MASK_DK       = (0x1<<1),	// D/K/K'(4 possible stereo standards)   6.5 MHz
    AUD_SIF_STANDARD_ASD_MODE_MASK_L        = (0x1<<2),	// L/L'                                  6.5 MHz
    AUD_SIF_STANDARD_ASD_MODE_MASK_I        = (0x1<<3),	// I                                     6.0 MHz
    AUD_SIF_STANDARD_ASD_MODE_MASK_M        = (0x1<<4),	// M(Korea A2 BTSC or EIAJ)              4.5 MHz
    AUD_SIF_STANDARD_ASD_MODE_MASK_ALL		= (0x1F),   // Mask bit4:0
    AUD_SIF_STANDARD_SSS_MODE_BG,
    AUD_SIF_STANDARD_SSS_MODE_DK,
    AUD_SIF_STANDARD_SSS_MODE_M,
    AUD_SIF_STANDARD_SSS_MODE_BG_A2,
    AUD_SIF_STANDARD_SSS_MODE_BG_NICAM,
    AUD_SIF_STANDARD_SSS_MODE_DK_A2_1,
    AUD_SIF_STANDARD_SSS_MODE_DK_A2_2,
    AUD_SIF_STANDARD_SSS_MODE_DK_A2_3,
    AUD_SIF_STANDARD_SSS_MODE_DK_NICAM,
    AUD_SIF_STANDARD_SSS_MODE_L_NICAM,
    AUD_SIF_STANDARD_SSS_MODE_I_NICAM,
    AUD_SIF_STANDARD_SSS_MODE_M_KOREA,
    AUD_SIF_STANDARD_SSS_MODE_M_BTSC,
    AUD_SIF_STANDARD_SSS_MODE_M_EIAJ,
    AUD_SIF_STANDARD_SSS_MODE_FM_10_7_IF_50_US,
    AUD_SIF_STANDARD_SSS_MODE_FM_10_7_IF_75_US,
    AUD_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_50_US,        /* Need to check ?? */
    AUD_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_75_US,        /* Need to check ?? */
} Aud_SIFStandardMask_e;

typedef enum _AUDIF_SRSTSHD_Switch_e
{
	SRSTSHD_OFF = 0x0,
	SRSTSHD_ON = 0x1,
}AUDIF_SRSTSHD_Switch_e;

typedef enum _AUDIF_SRSTSHD_Control_e
{
	SRSTSHD_CONTROL_OFF = 0x0,
	SRSTSHD_CONTROL_0P0 = 0x0,
	SRSTSHD_CONTROL_0P3,
	SRSTSHD_CONTROL_0P4,
	SRSTSHD_CONTROL_0P5,
	SRSTSHD_CONTROL_0P6,
	SRSTSHD_CONTROL_0P7,
	SRSTSHD_CONTROL_0P8,	
	SRSTSHD_CONTROL_1P0
}AUDIF_SRSTSHD_Control_e;

typedef enum _AUDIF_SRSTSHD_FrontMode_e
{
	SRSTSHD_MONO = 0x0,
	SRSTSHD_STEREO = 0x1
}AUDIF_SRSTSHD_FrontMode_e;

typedef enum _AUDIF_SRSTSHD_SpkSize_e
{
	SRSTSHD_SPK_40HZ,
	SRSTSHD_SPK_60HZ,
	SRSTSHD_SPK_100HZ,
	SRSTSHD_SPK_150HZ,
	SRSTSHD_SPK_200HZ,
	SRSTSHD_SPK_250HZ,
	SRSTSHD_SPK_300HZ,
	SRSTSHD_SPK_400HZ
}AUDIF_SRSTSHD_SpkSize_e;

typedef enum _AUDIF_SRSTSHD_Crossover_e
{
	SRSTSHD_CROSSOVER_80HZ,
	SRSTSHD_CROSSOVER_120HZ,
	SRSTSHD_CROSSOVER_160HZ,
	SRSTSHD_CROSSOVER_200HZ
}AUDIF_SRSTSHD_Crossover_e;

typedef enum _AUDIF_SRSTSHD_3dBitrate_e
{
	SRSTSHD_LOW = 0x0,
	SRSTSHD_HIGH = 0x1,
}AUDIF_SRSTSHD_3dBitrate_e;

typedef enum _AUDIF_SRSTSHD_3dMode_e
{
	SRSTSHD_3DSTEREO = 0x0,
	SRSTSHD_EXTREME = 0x1,
}AUDIF_SRSTSHD_3dMode_e;


// order is important!
// must match with typedef eSource_Type_t in customer.h
typedef enum {
	AUD_SOURCE_ATV = 0,
	AUD_SOURCE_DTV,
	AUD_SOURCE_RADIO,
	AUD_SOURCE_SCART,
	AUD_SOURCE_SCART1,
	AUD_SOURCE_AV,
	AUD_SOURCE_AV1,
	AUD_SOURCE_AV2,
	AUD_SOURCE_SVIDEO,
	AUD_SOURCE_SVIDEO1,
	AUD_SOURCE_SVIDEO2,
	AUD_SOURCE_YPBPR,
	AUD_SOURCE_YPBPR1,
	AUD_SOURCE_YPBPR2,
	AUD_SOURCE_HDMI,
	AUD_SOURCE_HDMI1,
	AUD_SOURCE_HDMI2,
	AUD_SOURCE_PC,
	AUD_SOURCE_MEDIA,
	AUD_SOURCE_KARAOKE,
	AUD_SOURCE_NET,
	AUD_SOURCE_DVD,
	AUD_SOURCE_MAX,
} AUDIF_Source_Type_e;

/***************************************************************************
	Structure definition
 ***************************************************************************/
typedef struct
{
    UINT8 DacMapping_Data0_L;
    UINT8 DacMapping_Data0_R;
    UINT8 DacMapping_Data1_L;
    UINT8 DacMapping_Data1_R;
    UINT8 DacMapping_Data2_L;
    UINT8 DacMapping_Data2_R;
}AUDIF_DacMapping_t;

typedef struct
{
    INT8 bPGA_gain;	//!PGA gain control value : -25(mute) ~ 6(6 dB),(1 step is 1 dB)
    UINT32 dLipSyncMemoryStarAddress;//!memory star address for lip sync (dely line unit)
	UINT32 dLipSyncMemorySize;		//!memory size for lip sync (dely line unit)
} Aud_AudioInitialConfig_t;

typedef struct
{
	Aud_AudioSubwooferSource_t eSubwooferSource;
	Aud_AudioSubwooferMode_t eSubwooferMode;
    Aud_AudioSubwooferCutoff_t eSubwooferLP;
    Aud_AudioSubwooferCutoff_t eSubwooferHP;    
} Aud_AudioSubwooferConfig_t;

typedef struct
{
	INT8 bDECAdjustLevel;
    INT8 bMonoAdjustLevel;
    INT8 bNICAMAdjustLevel;
    INT8 bSAPAdjustLevel;
    INT8 bADCAdjustLevel;
    INT8 bDigitalAdjustLevel;
} Aud_AudioLevelAdjust_t;

typedef struct
{
	Aud_AudioPath_t eChannel;
	Boolean fEnSilenceDetection;
  	UINT8 bThreshold;
  	UINT8 bDelay;
} Aud_AudioAutoSilence_t;

typedef struct
{
    Aud_AudioAVLMode_t eMode;
    INT8 bReference_level;
    Boolean fEnWeightFilter;
    Boolean fEnPeakModeWithClipManagement;
	Boolean Enable_AVL_Func;
} Aud_AudioAVLConfig_t;

typedef struct
{
    Boolean fEn;
	UINT8 bDelay;
} Aud_AudioLipSyncConfig_t;

typedef struct
{
    Boolean fEnBypass;
    INT8 bMainEQ_Band1_0100HZ;
    INT8 bMainEQ_Band2_0300HZ;
    INT8 bMainEQ_Band3_1000HZ;
    INT8 bMainEQ_Band4_3000HZ;
    INT8 bMainEQ_Band5_8000HZ;
} Aud_AudioEqualizerConfig_t;

typedef struct
{
	Aud_AudioClipManagementMode_t eClipManagementMode;
	Aud_AudioDVMPeakDetectionSelection_t eDVMPeakDetectSelection;
    UINT8 bDVMRelease_range_upper_limit;
    UINT8 bDVMRelease_range_lower_limit;
    UINT8 bDVMAttack_speed;
    UINT8 bDVMRelease_speed;
} Aud_AudioClipManagement_t;

typedef struct
{
	Aud_AudioISoundIntensityEffect_t eIntensityEffect;
	Aud_AudioISoundFadeTime_t eFadeTime;
	Aud_AudioISoundDetectorTime_t eDetectorTime;
}Aud_AudioISound_t;

typedef struct
{
    Boolean fEnBypass;
    INT8 bMainEQ_Band1_0100HZ;
    INT8 bMainEQ_Band2_0300HZ;
    INT8 bMainEQ_Band3_1000HZ;
    INT8 bMainEQ_Band4_3000HZ;
    INT8 bMainEQ_Band5_8000HZ;
} Aud_EqualizerConfig_t;

typedef struct
{
	Boolean fEnMainLoudness;
    Boolean fEnExtraBass;
    Aud_AudioMainLoudnessVolumeLevel_t eNoneAttackVolumeLevel;
} Aud_MainLoudnessConfig_t;

//----- auddrv report demdec HW and FW status -----//
typedef struct
{
    UINT8 En;
    UINT8 HysteresisFastMode;
    INT8  Gain;
} Aud_SIFAGCConfig_t;

typedef struct
{
	Aud_ReportMethod_e	eMethod;
    Aud_SIFStandard_e   eRetStandard;
    Aud_SIFAvailableMode_e eRetMode;
    UINT16	bRetSIFAGCGain;
	UINT16	bNICAMError;
} Aud_FrontEndParam_t;

// jyciou 2011/04/13
typedef enum _AUDIF_SUNSUR_Switch_e
{
	SUNSUR_OFF = 0x0,
	SUNSUR_ON 	= 0x1,
}AUDIF_SUNSUR_Switch_e;

// jyciou 2011/04/13
typedef enum _AUDIF_SUNSUR_Gain_e
{
	SUNSUR_GAIN_0P0,
	SUNSUR_GAIN_0P4,
	SUNSUR_GAIN_0P5,
	SUNSUR_GAIN_0P6,
	SUNSUR_GAIN_0P7,
	SUNSUR_GAIN_0P8,
	SUNSUR_GAIN_0P9,
	SUNSUR_GAIN_1P0
} AUDIF_SUNSUR_Gain_e;

typedef struct
{
    AUDIF_SUNSUR_Gain_e ambiance_gain;
    AUDIF_SUNSUR_Gain_e reverb_gain;
    AUDIF_SUNSUR_Gain_e direct_gain;
} Aud_SUNSUR_Param_t;

/******************************************************************************
       Funciton Prototype
 *****************************************************************************/
//AUDIF_Status_t AUDIF_InitConfig(Aud_AudioInitialConfig_t *pInitialParam);
AUDIF_Status_t AUDIF_Set_EffectMode(Aud_AudioEffectMode_t eEffectMode);
AUDIF_Status_t AUDIF_Set_SpatialEffect(Aud_AudioSpatialEffect_t eSpatialEffect);
AUDIF_Status_t AUDIF_Set_BackEndEffect(Aud_AudioBackendEffect_t eBackendEffect);
AUDIF_Status_t AUDIF_Set_Subwoofer(Aud_AudioSubwooferConfig_t *pSubwoofer_config);
AUDIF_Status_t AUDIF_Set_LevelAdjust(Aud_AudioLevelAdjust_t *pLevelAdjust_config);
AUDIF_Status_t AUDIF_Set_AVL(Aud_AudioAVLConfig_t *pAVL_config);
AUDIF_Status_t AUDIF_Set_AutoSilence(Aud_AudioAutoSilence_t *pAutoSilence_config);
AUDIF_Status_t AUDIF_Set_ClipManagement(Aud_AudioClipManagement_t *pClipManagement_config);
//AUDIF_Status_t AUDIF_Set_SRSTruSurroundXT(Aud_AudioSRSTrusurroundXT_t *pSRSTruSurroundXT_config);
AUDIF_Status_t AUDIF_Set_ISound(Aud_AudioISound_t *pISound_config);
//AUDIF_Status_t AUDIF_Set_3DSound(Aud_AudioSRS3DSound_t *pSRS3DSound_config);
//AUDIF_Status_t AUDIF_Set_SRSWOW(Aud_AudioSRSWOW_t *pWOW_config);

AUDIF_Status_t AUDIF_Set_Bass(INT32 dBass);
AUDIF_Status_t AUDIF_Get_Bass(INT32 *pdRetBass);
AUDIF_Status_t AUDIF_Set_CenterBass(INT32 dCenterBass);
AUDIF_Status_t AUDIF_Get_CenterBass(INT32 *pdRetCenterBass);
AUDIF_Status_t AUDIF_Set_SurroundBass(INT32 dSurroundBass);
AUDIF_Status_t AUDIF_Get_SurroundBass(INT32 *pdRetSurroundBass);
AUDIF_Status_t AUDIF_Set_Treble(INT32 dTreble);
AUDIF_Status_t AUDIF_Get_Treble(INT32 *pdRetTreble);
AUDIF_Status_t AUDIF_Set_CenterTreble(INT32 dCenterTreble);
AUDIF_Status_t AUDIF_Get_CenterTreble(INT32 *pdRetCenterTreble);
AUDIF_Status_t AUDIF_Set_SurroundTreble(INT32 dSurroundTreble);
AUDIF_Status_t AUDIF_Get_SurroundTreble(INT32 *pdRetSurroundTreble);
AUDIF_Status_t AUDIF_Set_MainEqulizer(Aud_EqualizerConfig_t *pEQ_config);
AUDIF_Status_t AUDIF_Get_MainEqulizer(Aud_EqualizerConfig_t *pRetEQ_config);
AUDIF_Status_t AUDIF_Set_CenterEqulizer(Aud_EqualizerConfig_t *pEQ_config);
AUDIF_Status_t AUDIF_Get_CenterEqulizer(Aud_EqualizerConfig_t *pRetEQ_config);
AUDIF_Status_t AUDIF_Set_MainLoudness(Aud_MainLoudnessConfig_t *pMainLoudness_config);
AUDIF_Status_t AUDIF_Set_SUNSUR_OnOff(AUDIF_SUNSUR_Switch_e dVal);


/*!
	\brief	Audio ADC source select
	\param	dSelect	[IN] enum Aud_DemodAgcCtrl_e
	\return	AUDIF_Status_t\n
	\note	none\n
*/
AUDIF_Status_t AUDIF_Set_AdcSourceSelect(UINT8 dSelect);
/*!
	\brief	Audio ADC source select
	\param	pdSelect	[OUT] enum AUDIF_AdcSourceSelect_e
	\return	AUDIF_Status_t\n
	\note	none\n
*/
AUDIF_Status_t AUDIF_Get_AdcSourceSelect(UINT8 *pdSelect);
/*!
	\brief	set channel map config
	\param	pdSelect	[IN] enum AUDIF_DacMapping_e
	\return	AUDIF_Status_t\n
	\note	none\n
*/
AUDIF_Status_t AUDIF_Set_ChannelMap(AUDIF_DacMapping_t *pChannelMap);
/*!
	\brief	get channel map config
	\param	pdSelect	[OUT] enum AUDIF_DacMapping_e
	\return	AUDIF_Status_t\n
	\note	none\n
*/
AUDIF_Status_t AUDIF_Get_ChannelMap(AUDIF_DacMapping_t *pRetChannelMap);

/*!
	\brief	get spdif input fs locked
	\param	
	\return	spdifin_setting.fs\n
	\note	none\n					
*/
INT32 AUDIF_Get_SpdifIn_Fs(void);

/*!
	\brief	ATV-SIF in Initialization
	\param	None
	\return	None
	\note	1. SIF in related APIs\n
			2. Global variable initialization, pin-mux setting\n			
*/
void AUDIF_Init_SifIn(void);

/*!
	\brief	Set main path output
	\param	mode    [IN] AUDIF_MainPath_e
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_MainChannelOutput(UINT8 mode);

/*!
	\brief	Set  Aux1 & Aux2 paths output
	\param	mode    [IN] AUDIF_Aux1Path_e, AUDIF_Aux2Path_e
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_AuxChannelOutput(UINT8 aux1mode, UINT8 aux2mode);

/*!
	\brief	Set master volume
	\param	mode    [IN] dMasterVolume -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_MasterVolume(INT32 dMasterVolume);

/*!
	\brief	Get master volume
	\param	mode    [IN] pdRetMasterVolume:the master volume value
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Get_MasterVolume(INT32 *pdRetMasterVolume);


/*!
	\brief	Set Aux1 volume
	\param	mode    [IN] dAux1Volume -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_Aux1Volume(INT32 dAux1Volume);

/*!
	\brief	Get Aux1 volume
	\param	mode    [IN] pdRetAux1Volume:the Aux1 volume value
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Get_Aux1Volume(INT32 *pdRetAux1Volume);

/*!
	\brief	Set Aux2 volume
	\param	mode    [IN] dAux2Volume -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_Aux2Volume(INT32 dAux2Volume);

/*!
	\brief	Get Aux2 volume
	\param	mode    [IN] pdRetAux2Volume:the Aux2 volume value
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Get_Aux2Volume(INT32 *pdRetAux2Volume);

/*!
	\brief	Set LR balance (L,R channels main volume)
	\param	mode    [IN] dLeftVolume -84(-84 dB,mute) ~ 24(+24 dB),(1 step, 1 dB)
	                     dRightVolume -84(-84 dB,mute) ~ 24(+24 dB),(1 step, 1 dB)
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_Balance(INT32 dLeftVolume, INT32 dRightVolume);

/*!
	\brief	Get LR balance (L,R channels main volume)
	\param	mode    [IN] pdRetLeftVolume : L channel main volume value
	                     pdRetRightVolume: R channel main volume value
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Get_Balance(INT32 *pdRetLeftVolume, INT32 *pdRetRightVolume);

/*!
	\brief	Set SW APP2(NXP APP) switch on/off
	\param	mode    [IN] mode: 0=turn off(bypass all APP except volume ctrl), 1=turn on
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_SwApp2Switch(UINT8 mode);

/*!
	\brief	Set HDMI input sample rate
	\param	mode    [IN] smp_rate
	\return	AUDIF_Status_t\n
	\note	none \n
*/
AUDIF_Status_t AUDIF_Set_HDMIInput_SmpRate(UINT32 smp_rate);


/*!
    \brief    Set Aux1 output mute or un-mute
    \param    mute_flag : MUTE_STATUS_ON->mute, else set volume to original value
    \return   AUD_SUCCESS if successful
*/
AUDIF_Status_t AUDIF_Set_Aux1MuteStatus(UINT8 mute_flag);

/*!
    \brief    Set Aux2 output mute or un-mute
    \param    mute_flag : MUTE_STATUS_ON->mute, else set volume to original value
    \return   AUD_SUCCESS if successful
*/
AUDIF_Status_t AUDIF_Set_Aux2MuteStatus(UINT8 mute_flag);

/*!
    \brief    Set Master output mute or un-mute
    \param    mute_flag : MUTE_STATUS_ON->mute, else set volume to original value
    \return   AUD_SUCCESS if successful
*/
AUDIF_Status_t AUDIF_Set_MasterMuteStatus(UINT8 mute_flag);

/*!
    \brief    Set ATV change channel ramp-down or ramp-up
    \param    ramp_flag : RAMP_STATUS_ON->ramp-down, else ramp-up
    \return   AUD_SUCCESS if successful
*/
AUDIF_Status_t AUDIF_Set_AtvRamp(UINT8 ramp_flag);

AUDIF_Status_t AUDIF_Set_LipSync_DelayTime(UINT16 ms);
AUDIF_Status_t AUDIF_Set_PcmOutputPhase(UINT8 main_phase, UINT8 aux1_phase, UINT8 aux2_phase);
AUDIF_Status_t AUDIF_Set_SUNSUR_Param(Aud_SUNSUR_Param_t *sunsur_param);
AUDIF_Status_t AUDIF_Get_SUNSUR_Param(Aud_SUNSUR_Param_t *sunsur_param);

/********************* SRS TruSurround HD/HD4 Control Related APIs ***********************/
AUDIF_Status_t AUDIF_Set_SRSTSHD_OnOff(AUDIF_SRSTSHD_Switch_e dVal);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SurroundOnOff(AUDIF_SRSTSHD_Switch_e dVal);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SurroundControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_DialogControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_DefinitionFrontControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_DefinitionCenterControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassFrontMode(AUDIF_SRSTSHD_FrontMode_e dMode);
AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassFrontControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassSubControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassFrontSize(AUDIF_SRSTSHD_SpkSize_e dSize);
AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassSubSize(AUDIF_SRSTSHD_SpkSize_e dSize);
AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassSubCrossover(AUDIF_SRSTSHD_Crossover_e dSize);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dOnOff(AUDIF_SRSTSHD_Switch_e dVal);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dBitrate(AUDIF_SRSTSHD_3dBitrate_e dVal);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dMode(AUDIF_SRSTSHD_3dMode_e dVal);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dCenterControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dSpaceControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_FocusControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_LimiterOnOff(AUDIF_SRSTSHD_Switch_e dVal);
AUDIF_Status_t AUDIF_Set_SRSTSHD_LimiterControl(AUDIF_SRSTSHD_Control_e dGain);
AUDIF_Status_t AUDIF_Set_SRSTSHD_ChannelMode(UINT8 mode);
void AUDIF_Update_Spdifin_SampleRate(void);
AUDIF_Status_t AUDIF_Get_MasterMuteStatus(UINT8 *mute_status);
AUDIF_Status_t AUDIF_Get_Aux1MuteStatus(UINT8 *aux1_mute_status);
AUDIF_Status_t AUDIF_Get_Aux2MuteStatus(UINT8 *aux2_mute_status);
INT32 AUDIF_Set_InternalMuteStatus(UINT8 mute_flag);

/*!
    \brief    External DAC XCK setting
    \param    XCK_FS : a multiple of sampling-rate
    \return   NONE
*/
void AUDIF_Set_Ext_XCK_FS( UINT16 XCK_FS );
/************************* 8203D prototype API ******************************/
INT32 AUDIF_Get_MPEG_BSType( UINT8 BankID, INT16 *val );
INT32 AUDIF_Init_Wave_Dec2_Audio(WAVFORMATMACRO *wfmt1);
INT32 AUDIF_Is_DecEndingDone(UINT8 BankID);
INT32 AUDIF_Wait_DecEndingDone(UINT8 BankID);
void AUDIF_Set_XCK_FS( UINT16 XCK_FS );
void AUDIF_Set_I2SBypass( UINT16 mode );
AUDIF_Status_t AUDIF_Set_SourceType(AUDIF_Source_Type_e mode);
AUDIF_Status_t AUDIF_Get_DdpFmt( UINT16 *type );
    
#endif		// __AUDDRV_H__
