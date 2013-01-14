/******************************************************************************/
/*! @addtogroup Audio
*  @{
******************************************************************************/
/******************************************************************************/
/*!
* @file    drv_audio_config.h
*
* @brief   This file specifies the APIs provided to control audio input device.
*
* @note     Copyright (c) 2007 Sunplus Technology Co., Ltd. \n
*          All rights reserved.
*
********************************************************************************/
#ifndef _DRV_AUDIO_CONFIG_H_
#define _DRV_AUDIO_CONFIG_H_

/*******************************************************************************
* Header include
******************************************************************************/

/*******************************************************************************
* constant
******************************************************************************/

/*===========================================================================*/
//default constant for path level adjust
/*===========================================================================*/
#if 0
/*Level adjust initial values*/
#define DTV_DRV_AUD_LEVEL_ADJUST_DEC_DEFAULT                            0   //range : -16(-16 dB,mute) ~ 15(+15 dB)
#define DTV_DRV_AUD_LEVEL_ADJUST_MONO_DEFAULT                           0   //range : -16(-16 dB,mute) ~ 15(+15 dB)
#define DTV_DRV_AUD_LEVEL_ADJUST_NICAM_DEFAULT                          0   //range : -16(-16 dB,mute) ~ 15(+15 dB)
#define DTV_DRV_AUD_LEVEL_ADJUST_SAP_DEFAULT                            0   //range : -16(-16 dB,mute) ~ 15(+15 dB)
#define DTV_DRV_AUD_LEVEL_ADJUST_ADC_DEFAULT                            0   //range : -16(-16 dB,mute) ~ 15(+15 dB)
#define DTV_DRV_AUD_LEVEL_ADJUST_I2S_DEFAULT                            0   //range : -16(-16 dB,mute) ~ 15(+15 dB)
#endif
/*===========================================================================*/
//default constant for DEMDEC(demodulator and decoder) part
/*===========================================================================*/
#if 0
/*magnitude detect default values*/
#define DTV_DRV_AUD_MXP_MAGNITUDE_DETECT_UP_DEFAULT                     3   //range : 0 ~ 15
#define DTV_DRV_AUD_MXP_MAGNITUDE_DETECT_LOW_DEFAULT                    9   //range : 0 ~ 15
#define DTV_DRV_AUD_SAP_MAGNITUDE_DETECT_UP_DEFAULT                     3   //range : 0 ~ 15
#define DTV_DRV_AUD_SAP_MAGNITUDE_DETECT_LOW_DEFAULT                    6   //range : 0 ~ 15
#define DTV_DRV_AUD_SOUND_CARRIER1_MAGNITUDE_DETECT_THRESHOLD_DEFAULT   -8  //range : -16 ~ 15

/*NMUTE default values*/
#define DTV_DRV_AUD_SAP_NMUTE_THRESHOLD_DEFAULT                         -6  //range : -16 ~ 15
#define DTV_DRV_AUD_SAP_NMUTE_HYSTERDZIS_DEFAULT                        4   //range : 0 ~ 15
#define DTV_DRV_AUD_FM_A2_SOUND_CARRIER2_NMUTE_THRESHOLD_DEFAULT        0   //range : -16 ~ 15
#define DTV_DRV_AUD_FM_A2_SOUND_CARRIER2_NMUTE_HYSTERDZIS_DEFAULT       4   //range : 0 ~ 15
#define DTV_DRV_AUD_BTSC_NMUTE_THRESHOLD_DEFAULT                        0   //range : -16 ~ 15
#define DTV_DRV_AUD_BTSC_NMUTE_HYSTERDZIS_DEFAULT                       4   //range : 0 ~ 15
#define DTV_DRV_AUD_FM_RADIO_NMUTE_THRESHOLD_DEFAULT                    0   //range : -16 ~ 15
#define DTV_DRV_AUD_FM_RADIO_NMUTE_HYSTERDZIS_DEFAULT                   4   //range : 0 ~ 15
#define DTV_DRV_AUD_EIAJ_NMUTE_THRESHOLD_DEFAULT                        0   //range : -16 ~ 15
#define DTV_DRV_AUD_EIAJ_NMUTE_HYSTERDZIS_DEFAULT                       4   //range : 0 ~ 15
#define DTV_DRV_AUD_EIAJ_SUB_CARRIER_NMUTE_THRESHOLD_UP_DEFAULT         8   //range : 0 ~ 15
#define DTV_DRV_AUD_EIAJ_SUB_CARRIER_NMUTE_THRESHOLD_LOW_DEFAULT        12  //range : 0 ~ 15
#define DTV_DRV_AUD_EIAJ_SUB_CARRIER_DETECT_SWITCH_DEFAULT              REG_SSD_EIAJ_CAR_DETECT_VALUE_DISABLED

/*NICAM default values*/
#define DTV_DRV_AUD_NICAM_ONLY_RELATED_SWITCH_DEFAULT                   REG_SSD_ONLY_RELATED_VALUE_FALSE
#define DTV_DRV_AUD_NICAM_FALL_BACK_EXTERANL_AM_SWITCH_DEFAULT          REG_SSD_EXTAM_VALUE_CHANNEL_1_OUTPUT
#define DTV_DRV_AUD_NICAM_DEEMPHASIS_SWITCH_DEFAULT                     REG_SSD_NICDEEM_VALUE_ON
#define DTV_DRV_AUD_NICAM_AUTO_MUTE_SWITCH_DEFAULT                      REG_SSD_NIC_AMUTE_VALUE_ON
#define DTV_DRV_AUD_NICAM_LOWER_ERROR_LIMIT_DEFAULT                     100 //range : 0 ~ 255
#define DTV_DRV_AUD_NICAM_UPPER_ERROR_LIMIT_DEFAULT                     200 //range : 0 ~ 255
#endif
/*overmodulation default values*/
#define DRV_AUD_OVERMODULATION_ADAPTION_SWITCH_DEFAULT              REG_SSD_OVMADAPT_VALUE_ENABLED
#define DRV_AUD_OVERMODULATION_LEVEL_THRESHOLD_DEFAULT              REG_SSD_OVMTHR_VALUE_6_DB_MINUS_9_DBFS

/*analog deemphasis default values*/
#define DRV_AUD_FIXED_DEEMPHASIS_ANALONG_SOUND_DEFAULT              REG_SSD_FMDEEM_VALUE_OFF_FLAT

/*DEMDEC easy program mode default values*/
#define DRV_AUD_FIXED_DEMDEC_EPMODE_DEFAULT                         REG_SSD_EPMODE_VALUE_ASD

/*===========================================================================*/
//default constant for audio process part
/*===========================================================================*/

#if 0//remove default values to TVFE_AUDIO_config.h and set these values at PL_AudioInit()
/*AVL initial values*/
#define DTV_DRV_AUD_AVL_MODE_DEFAULT                                    REG_SSD_AVLMOD_VALUE_OFF
#define DTV_DRV_AUD_AVL_LEVEL_DEFAULT                                   REG_SSD_AVLLEV_VALUE_MEDIUM_THRESHOLD
#define DTV_DRV_AUD_AVL_WEIGHT_FILTER_SWITCH_DEFAULT                    REG_SSD_AVLWEIGHT_VALUE_OFF
#define DTV_DRV_AUD_AVL_PEAKMODE_WITH_CLIP_MANAGEMENT_SWITCH_DEFAULT    REG_SSD_AVLPEAKMODE_VALUE_AVL_STANDALONE

/*MainLoudness initial values*/
#define DTV_DRV_AUD_MAINLOUDNESS_SWITCH_DEFAULT                         REG_SSD_MAINLOUD_VALUE_OFF
#define DTV_DRV_AUD_MAINLOUDNESS_NONE_ATTACK_VOLUME_LEVEL_DEFAULT       REG_SSD_MAINLONA_VALUE_MINUS_15DB_VOLUME_9DB_VOLUME
#define DTV_DRV_AUD_MAINLOUDNESS_FILTER_CHARACTERISTIC_DEFAULT          REG_SSD_MAINLOCH_VALUE_STANDARD_500HZ
//#define DTV_DRV_AUD_MAINLOUDNESS_NONE_ATTACK_VOLUME_EXTENSION_DEFAULT   REG_SSD_MAINLONAEXT_VALUE_SET_1_MINUS_15DB_6DB

/*Lip sync initial values*/
#define DTV_DRV_AUD_LIP_SYNC_SWITCH_DEFAULT                             REG_CTRL_DLU_EN_VALUE_DISABLED
#define DTV_DRV_AUD_LIP_SYNC_DELAY_DEFAULT                              REG_CTRL_DLU_DELAY_MIN

/*set clip management default config*/
#define DTV_DRV_AUD_CLIP_MANAGEMENT_MODE_DEFAULT                        REG_SSD_CLIPMANAGE_VALUE_CLIP_MANAGEMENT_OFF_DEFAULT

/*set clip management DVM default config*/
#define DTV_DRV_AUD_CLIP_MANAGEMENT_PEAK_DETECTION_SELECT_DEFAULT       REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_MAIN_L_R
#define DTV_DRV_AUD_CLIP_MANAGEMENT_RELEASE_RANGE_UPPER_LIMIT_DEFAULT   REG_SSD_DVM_RR_UPPER_VALUE_MINUS_3_5DBFS
#define DTV_DRV_AUD_CLIP_MANAGEMENT_RELEASE_RANGE_LOWER_LIMIT_DEFAULT   REG_SSD_DVM_RR_LOWER_VALUE_MINUS_60DBFS
#define DTV_DRV_AUD_CLIP_MANAGEMENT_ATTACK_SPEED_DEFAULT                REG_SSD_DVM_ATTSPEED_VALUE_9_6DB_S
#define DTV_DRV_AUD_CLIP_MANAGEMENT_RELEASE_SPEED_DEFAULT               REG_SSD_DVM_RELSPEED_VALUE_0_48DB_S

/*Bass management default values*/
#define DTV_DRV_AUD_BASS_MANAGEMENT_MODE_DEFAULT                        REG_SSD_BAMAMO_VALUE_OFF_WIDE_CENTRE_MODE
#define DTV_DRV_AUD_BASS_MANAGEMENT_FILTER_CHARACTERISTIC_DEFAULT       REG_SSD_BAMAFC_VALUE_50_HZ
#define DTV_DRV_AUD_BASS_MANAGEMENT_SUBWOOFER_FILTER_SWITCH_DEFAULT     REG_SSD_BAMASUB_VALUE_SUBWOOFER_FILTER_ON

/*VDSII default values*/
#define DTV_DRV_AUD_VDSII_MIX_LEVEL_DEFAULT                             REG_SSD_VDSMIXLEV_VALUE_80
#define DTV_DRV_AUD_VDSII_BALANCE_DEFAULT                               REG_SSD_VDSIIBALANCE_VALUE_LEFT

/*SRS Trusurround XT default values*/
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_MODE_DEFAULT                       REG_SSD_TSXT_K_MODE_VALUE_K2_0
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_DIALOGCLARITY_SWITCH_DEFAULT       REG_SSD_TSXT_DialogClarityEnable_VALUE_ON
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_TRUBASS_SWITCH_DEFAULT             REG_SSD_TSXT_TruBassEnable_VALUE_ON
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_HEADPHONE_MODE_SWITCH_DEFAULT      REG_SSD_TSXT_Headphone_ModeEnable_VALUE_OFF
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_CENTER_OUT_523_MODE_SWITCH_DEFAULT REG_SSD_TSXT_CenterOut_523ModeEnable_VALUE_OFF
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_SPEAKER_SIZE_DEFAULT               REG_SSD_TSXT_SpeakerSize_VALUE_60HZ
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_BASS_GAIN_DEFAULT                  REG_SSD_TSXT_TruBass_GAIN_VALUE_60
#define DTV_DRV_AUD_SRSTRUSURROUNDXT_DIALOGCLARITY_GAIN_DEFAULT         REG_SSD_TSXT_DialogClarity_GAIN_VALUE_67

/*ESS(I_STEREO) and EPS(I_MONO) default values*/
#define DTV_DRV_AUD_I_STEREO_I_MONO_EFFECT_DEFAULT                      REG_SSD_INSOEF_VALUE_4

/*auto switch ESS(I_STEREO) and EPS(I_MONO) default config*/
#define DTV_DRV_AUD_AUTO_I_STEREO_I_MONO_MODE_DEFAULT                   REG_SSD_ISOUNDMODE_VALUE_MANUAL_MODE
#define DTV_DRV_AUD_AUTO_I_STEREO_I_MONO_FADE_TIME_DEFAULT              REG_SSD_ISOUNDFADETIME_VALUE_3_3SEC
#define DTV_DRV_AUD_AUTO_I_STEREO_I_MONO_DETECT_TIME_DEFAULT            REG_SSD_ISOUNDDETTIME_VALUE_8SEC

/*3D sound default values*/
#define DTV_DRV_AUD_3D_SOUND_CENTERCONTROL_DEFAULT                      REG_SSD_SRS3DCENTER_VALUE_MINUS_14DB
#define DTV_DRV_AUD_3D_SOUND_SPACECONTROL_DEFAULT                       REG_SSD_SRS3DSPACE_VALUE_MINUS_4DB

/*WOW default values*/
#define DTV_DRV_AUD_WOW_BRIGHTNESS_DEFAULT                              REG_SSD_WOW_BRIGHTNESS_VALUE_HIGH
#define DTV_DRV_AUD_WOW_SPEAKERSIZE_DEFAULT                             REG_SSD_WOW_SPKSIZE_VALUE_60HZ
#define DTV_DRV_AUD_WOW_BASSLEVEL_DEFAULT                               REG_SSD_WOW_BASSLEVEL_VALUE_60
#define DTV_DRV_AUD_WOW_STEREOWIDTH_DEFAULT                             REG_SSD_WOW_STEREOWIDTH_VALUE_80

/*BBE default values*/
#define DTV_DRV_AUD_BBE_BASS_BOOST_100HZ_DEFAULT                        0//range : 0 ~ 15
#define DTV_DRV_AUD_BBE_TREBLE_BOOST_10KHZ_DEFAULT                      0//range : 0 ~ 15
#endif
/*******************************************************************************
* Structure
******************************************************************************/

/*******************************************************************************
* Variable
******************************************************************************/

/*******************************************************************************
* Marco
******************************************************************************/

/*******************************************************************************
* Program
******************************************************************************/

#endif /* __DRV_AUDIO_CONFIG_H__ */

/*!
    @} end of addtogroup Audio */

