/******************************************************************************/
/**
 *
 * @file    plf_av_source.h
 *
 * @brief    This file specifies the Audio/Video signal pins
 *
 * @note    Copyright (c) 2010 Sunplus Technology Co., Ltd. \n
 *            All rights reserved.
 *
 * @author  002130
 *
 * $Id$
 ******************************************************************************/
#ifndef __PLF_AV_SOURCE_H__
#define __PLF_AV_SOURCE_H__

typedef enum {
	PLF_INPUT_TYPE_NULL      = 0,
	PLF_INPUT_PIN_TYPE_ATV_TUNER,
	PLF_INPUT_PIN_TYPE_CVBS,
	PLF_INPUT_PIN_TYPE_SVIDEO,
	PLF_INPUT_PIN_TYPE_YPBPR,
	PLF_INPUT_PIN_TYPE_RGB,
	PLF_INPUT_PIN_TYPE_HDMI,
	PLF_INPUT_PIN_TYPE_DVI,
	PLF_INPUT_PIN_TYPE_SCART_AV,
	PLF_INPUT_PIN_TYPE_SCART_SV,
	PLF_INPUT_PIN_TYPE_SCART_AV_RGB,
	PLF_INPUT_PIN_TYPE_DTV,
	PLF_INPUT_PIN_TYPE_STORAGE,
	PLF_INPUT_PIN_TYPE_CCIR656,
	PLF_INPUT_PIN_TYPE_VIF,
	PLF_INPUT_PIN_TYPE_SIF,
	PLF_INPUT_PIN_TYPE_AUDIO_ADC,
	PLF_INPUT_PIN_TYPE_I2S,
	PLF_INPUT_PIN_TYPE_SPDIF,
	PLF_INPUT_PIN_TYPE_MAX,
} PLF_AVType_e;


typedef enum {
	PLF_NULL_PIN        = 0,
	PLF_INPUT_PIN_CVBS0,

	PLF_INPUT_PIN_CVBS1,

	PLF_INPUT_PIN_CVBS2,

	PLF_INPUT_PIN_CVBS3,

	PLF_INPUT_PIN_CVBS4,

	PLF_INPUT_PIN_C1,

	PLF_INPUT_PIN_C2,

	PLF_INPUT_PIN_RIN,
	PLF_INPUT_PIN_GIN,
	PLF_INPUT_PIN_BIN,
	PLF_INPUT_PIN_ADC_SOG,

	PLF_INPUT_PIN_PR1,
	PLF_INPUT_PIN_Y1,
	PLF_INPUT_PIN_PB1,
	PLF_INPUT_PIN_ADC_SOY1,

	PLF_INPUT_PIN_PR2,
	PLF_INPUT_PIN_Y2,
	PLF_INPUT_PIN_PB2,
	PLF_INPUT_PIN_ADC_SOY2,

	PLF_INPUT_PIN_SARADC0,

	PLF_INPUT_PIN_SARADC1,

	PLF_INPUT_PIN_SARADC2,

	PLF_INPUT_PIN_HDMI0,

	PLF_INPUT_PIN_HDMI1,

	PLF_INPUT_PIN_HDMI2,

	PLF_INPUT_PIN_DTV, /*including parallel/Serial/VIF depends on tuner driver*/

	PLF_INPUT_PIN_STORAGE,

	PLF_INPUT_PIN_AUD_SIF,

	PLF_INPUT_PIN_AUD_ADC0,

	PLF_INPUT_PIN_AUD_ADC1,

	PLF_INPUT_PIN_AUD_ADC2,

	PLF_INPUT_PIN_AUD_ADC3,

	PLF_INPUT_PIN_AUD_HDMI0,

	PLF_INPUT_PIN_AUD_HDMI1,

	PLF_INPUT_PIN_AUD_HDMI2,

	PLF_INPUT_PIN_AUD_DTV,

	PLF_INPUT_PIN_AUD_STORAGE,

	PLF_INPUT_PIN_AUD_SPDIFIN,

	PLF_OUTPUT_PIN_AUD_ADC0,

	PLF_OUTPUT_PIN_AUD_ADC1,

	PLF_OUTPUT_PIN_AUD_ADC2,

	PLF_OUTPUT_PIN_AUD_SPDIFOUT,
} PLF_AVPin_e;

/********************************************************************
	Structure
********************************************************************/

/* Input pin config */
typedef union {
	struct {
		PLF_AVPin_e cvbs_pin;
		PLF_AVPin_e r_pin;
		PLF_AVPin_e g_pin;
		PLF_AVPin_e b_pin;
		PLF_AVPin_e fs_pin;
		PLF_AVPin_e fb_pin;
	} scart_av_rgb;
	struct {
		PLF_AVPin_e cvbs_pin;
		PLF_AVPin_e fs_pin;
	} scart_av;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e c_pin;
		PLF_AVPin_e fs_pin;
	} scart_sv;
	struct {
		PLF_AVPin_e pin;
	} cvbs;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e c_pin;
	} svideo;
	struct {
		PLF_AVPin_e r_pin;
		PLF_AVPin_e g_pin;
		PLF_AVPin_e b_pin;
		PLF_AVPin_e sog_pin;
	} rgb;
	struct {
		PLF_AVPin_e y_pin;
		PLF_AVPin_e pb_pin;
		PLF_AVPin_e pr_pin;
		PLF_AVPin_e soy_pin;
	} ypbpr;
	struct {
		PLF_AVPin_e pin;
	} hdmi;
	struct {
		PLF_AVPin_e pin;
	} digital;
} PLF_VideoConf_st;

typedef struct {
	PLF_AVType_e 		eType;
	PLF_VideoConf_st 	video;
	PLF_AVPin_e 		eAudioPri;
} PLF_AVSourcePinConfig_st;
#endif


/*****************************************************************************
** $Rev$
**
*****************************************************************************/

