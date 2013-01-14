#ifndef __AUDSTATIC_H__
#define __AUDSTATIC_H__


#include "aud_global.h"
 /******************************************************************************
    globals
 *****************************************************************************/ 
extern WAVEFORMATEX2 gWmaParams; //tyc1090: change EXTERN to extern

#ifdef VOLUMN_TAB_80
#define VOLUME_SCALE	 82
#elif defined (VOLUMN_TAB_50)
#define VOLUME_SCALE	(50+1)
#else
#define VOLUME_SCALE	(16+1)
#endif


#define MIC_GAIN_SCALE	9

//Dual-Mpeg mixer related define
#define FADE_SCALE        256
#define DB2LINEAR_SCALE   78
#define FULL_SCALE_GAIN   0x7FFF

/*======================================*/
/*=====  SPDIF In related     =====*/
/*======================================*/

#define IS_SPDIFIN_PERIOD_LEGAL(x, lo, hi)  ((x>=lo) & (x<=hi))
#define IEC_RX_32K_PERIOD   1
#define IEC_RX_44K_PERIOD   2
#define IEC_RX_48K_PERIOD   3
#define IEC_RX_88K_PERIOD   4       
#define IEC_RX_96K_PERIOD   5       
#define IEC_RX_176K_PERIOD  6       
#define IEC_RX_192K_PERIOD  7 

//inline UINT16 aud_GetDecID( UINT32 uiCodingMode );
//inline UINT16 aud_GetEncID( UINT32 uiCodingMode );
void aud_InitAudParams(void);
void aud_SetCodecParams(UINT32 coding_mode);
void aud_SetDec2CodecParams(UINT32 coding_mode);
void aud_ResetCodecPara(UINT32 coding_mode);
UINT32 SampleRate_Mapping( UINT32 smp_in );

#if defined(SCALER_6500)||defined(HDMI_SOC)||defined(HDMI_13)
static void Disable_HDMI(void);
static void Enable_HDMI(void);
#else
void Disable_HDMI(void); 
void Enable_HDMI(void);
#endif



#endif //#ifndef __AUDSTATIC_H__

