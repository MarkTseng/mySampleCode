#ifndef __AUDPOSTPROC_H__
#define __AUDPOSTPROC_H__
#include "aud_global.h"
#include "dsp_mem.h"

//
// defines
//
#define BIT_0					0
#define BIT_1					1
#define BIT_2					2
#define BIT_3					3
#define BIT_4					4
#define BIT_5					5
#define BIT_6					6
#define BIT_7					7
#define BIT_8					8
#define BIT_9					9
#define BIT_10					10
#define BIT_11					11
#define BIT_12					12
#define BIT_13					13
#define BIT_14					14
#define BIT_15					15

//----- SIF AGC HWREG related -----//
// AUD_SIFADC_MUX_ADR[sifadc_mux] (bank6-0x20, G305.0 bit0)
#define HW_SSD_SIFADC_MUX_V_SIFAIN0				0
#define HW_SSD_SIFADC_MUX_V_SIFAIN1				1
#define HW_SSD_SIFADC_MUX_V_MAX				    1
#define HW_SSD_SIFADC_MUX_V_MIN				    0

// AUD_AGC_OFF_ADR[agc_hysteresis] (bank6-0x24, G305.4 bit1)
#define HW_SSD_AGC_HYSTERESIS_V_SLOW_MODE	    0
#define HW_SSD_AGC_HYSTERESIS_V_FAST_MODE	    1
#define HW_SSD_AGC_HYSTERESIS_V_MAX		        1
#define HW_SSD_AGC_HYSTERESIS_V_MIN		        0

// AUD_AGC_OFF_ADR[agc_off] (bank6-0x24, G305.4 bit0)
#define HW_SSD_AGC_OFF_V_AGC_NOT_OFF		    0
#define HW_SSD_AGC_OFF_V_AGC_OFF			    1
#define HW_SSD_AGC_OFF_V_MAX				    1
#define HW_SSD_AGC_OFF_V_MIN				    0

// AUD_AGC_CTL_ADR[agc_ctl] (bank6-0x25, G305.5 bit4:0)
#define HW_SSD_AGC_CTL_V_MAX				    31
#define HW_SSD_AGC_CTL_V_MIN				    0

// AUD_AGC_STATUS_ADR[agc_status] (bank6-0x26, G305.6 bit4:0)
#define HW_SSD_AGC_STATUS_V_MAX			        31
#define HW_SSD_AGC_STATUS_V_MIN			        0

//----- DEMDEC DSP related -----//
// risc_param1[RISC_DDEP_CTRL_L] bit7:0 STDSEL ASD mode
#define DSP_STDSEL_ASD_MODE_B_G                 (0x1<<0)
#define DSP_STDSEL_ASD_MODE_D_K                 (0x1<<1)
#define DSP_STDSEL_ASD_MODE_L_L                 (0x1<<2)
#define DSP_STDSEL_ASD_MODE_I                   (0x1<<3)
#define DSP_STDSEL_ASD_MODE_M                   (0x1<<4)

// risc_param1[RISC_DDEP_CTRL_L] bit7:0 STDSEL both SSS mode and ASD mode
#define DSP_STDSEL_VALUE_FAIL                                        0
#define DSP_STDSEL_VALUE_B_G                                         1
#define DSP_STDSEL_VALUE_D_K                                         2
#define DSP_STDSEL_VALUE_M                                           3

#define DSP_STDSEL_VALUE_B_G_A2                                      4
#define DSP_STDSEL_VALUE_B_G_NICAM                                   5
#define DSP_STDSEL_VALUE_D_K_A2_1                                    6
#define DSP_STDSEL_VALUE_D_K_A2_2                                    7
#define DSP_STDSEL_VALUE_D_K_A2_3                                    8
#define DSP_STDSEL_VALUE_D_K_NICAM                                   9
#define DSP_STDSEL_VALUE_L_NICAM_L                                   10
#define DSP_STDSEL_VALUE_I_NICAM                                     11
#define DSP_STDSEL_VALUE_M_KOREA                                     12
#define DSP_STDSEL_VALUE_M_BTSC                                      13
#define DSP_STDSEL_VALUE_M_EIAJ                                      14
#define DSP_STDSEL_VALUE_FM_RADIO_IF_10_7_MHZ_50_US_DEEMPHASIS       15
#define DSP_STDSEL_VALUE_FM_RADIO_IF_10_7_MHZ_75_US_DEEMPHASIS       16
#define DSP_STDSEL_VALUE_FM_RADIO_SELECTABLE_IF_50_US_DEEMPHASIS     17
#define DSP_STDSEL_VALUE_FM_RADIO_SELECTABLE_IF_75_US_DEEMPHASIS     18


#define AUD_POSTPROC_CHK_RANGE(value,min,max)\
{\
	if((value < min)\
		||(value > max))\
	{\
		return AUDIF_ERR_INVALID_PARAM;\
	}\
}\

/******************************************************************************
    Demdec related
 *****************************************************************************/
// auddrv global variables to save dsp dsp_param1[0,1] report
typedef struct
{
    union
    {
        UINT32 _INF_DEV_STA;
        struct
        {
            UINT32 STDRES    :5;
            UINT32 GST       :1;
            UINT32 GDU       :1;
            UINT32 RESERVED0 :1;
            UINT32 ADU       :1;
            UINT32 AST       :1;
            UINT32 AAMUT     :1;
            UINT32 BPILOT    :1;
            UINT32 SAPDET    :1;
            UINT32 BAMUT     :1;
            UINT32 SAMUT     :1;
            UINT32 VDSP_C    :1;
            UINT32 NICST_C   :1;
            UINT32 NICDU_C   :1;
            UINT32 NAMUT     :1;
            UINT32 RSSF      :1;
            UINT32 INITSTAT  :1;
            UINT32 RESERVED1 :3;
            UINT32 RESERVED2 :8;
        } INF_DEV_STA;
    };

    union
    {
        UINT32 _INF_NIC_STA;
        struct
        {
            UINT32 ERR_OUT   :8;
            UINT32 CFC       :1;
            UINT32 CO_LOCKED :1;
            UINT32 NACB      :4;
            UINT32 VDSP      :1;
            UINT32 NICST     :1;
            UINT32 NICDU     :1;
            UINT32 RESERVED0 :7;
            UINT32 RESERVED1 :8;
        } INF_NIC_STA;
    };

    union
    {
        UINT32 _INF_NIC_ADD;
        struct
        {
            UINT32 ADW       :11;
            UINT32 RESERVED0 :6;
            UINT32 RESERVED1 :7;
            UINT32 RESERVED2 :8;
        } INF_NIC_ADD;
    };

    union
    {
        UINT32 _INF_LEV_MON;
        struct
        {
            UINT32 MONLEVEL  :24;
            UINT32 RESERVED0 :8;
        } INF_LEV_MON;
    };

    union
    {
        UINT32 _INF_MPX_LEV;
        struct
        {
            UINT32 HW_AST    :1;
            UINT32 HW_ADU    :1;
            UINT32 FI_AST    :1;
            UINT32 FI_ADU    :1;
            UINT32 RESERVED0 :2;
            UINT32 MPXPLEV   :18;
            UINT32 RESERVED1 :8;
        } INF_MPX_LEVEL;
    };

    union
    {
        UINT32 _INF_NOISELEV;
        struct
        {
            UINT32 NDETCH_STAT       :1;
            UINT32 NDETPB_STAT       :1;
            UINT32 NOISELEVEL        :22;
            UINT32 RESERVED0         :8;
        } INF_NOISELEV;
    };

    union
    {
        UINT32 _INF_OVMADAPT;
        struct
        {
            UINT32 FILTBW_STAT       :2;
            UINT32 RESERVED0         :10;
            UINT32 OVM_SCALE_STAT    :12;
            UINT32 RESERVED1         :8;
        } INF_OVMADAPT;
    };
} Aud_DemDecInfo_t;

typedef struct
{
    Aud_SIFStandard_e           eStandard;
    Aud_SIFAvailableMode_e      eMode;    
    UINT8                       bSIFAGCGain;
    UINT8                       bNICAMError;
} Aud_FrontEndStatus_t;

typedef struct
{
	Aud_SIFAGCConfig_t  FM_AGCConfigParam;
    Aud_SIFAGCConfig_t  AM_AGCConfigParam;
} Aud_FrontEndControl_t;

// auddrv global variable to save demdec related status and dsp report
typedef struct
{
    Aud_FrontEndStatus_t Status;
    Aud_FrontEndControl_t Control;
} Aud_FrontEndInfo_t;


#endif
