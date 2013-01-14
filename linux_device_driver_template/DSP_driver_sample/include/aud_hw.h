#ifndef __AUD_HW_H__
#define __AUD_HW_H__

/***********************************************************************
 *  2009.06 / chuancheng.yang
 *  This file is all about audio harware register macro define for IC_8203D
 ***********************************************************************/

/***********************************************************************
 *  Declaration Session
 ***********************************************************************/
#include <asm/mach-spv9200/types.h>
#include "gpio_pin_map.h"

void PLLA_Clock_Setting(int index);

// ccyang: add here for enable the following define, they will move to proper place
#define INTERNAL_AUDIO_DAC
#ifndef AUD_9200_CLOCK
#define HW_SUPPORT_AUDHW_USE_DIGITAL_PLL
#endif
#define HW_SUPPORT_HDMI_I2S_SPDIF

void AUDHW_Init_AudHW_CLK(void);
void AUDHW_Set_AudHW_SmpRate(unsigned fs);
void AUDHW_Set_IEC0_CLK(unsigned fs);
void AUDHW_Set_IEC1_CLK(unsigned fs);
void AUDHW_Set_IntDAC_CLK(unsigned fs);

#define GPIO_AMUTE_PIN  PIN_W22
void AUDHW_Enable_Mute(int pin_number);
void AUDHW_Disable_Mute(int pin_number);
#ifdef AUDIOTEAM_Module_Test
void TurnON_i2c_otp( void );
int read_i2c_reg(int offset);
void write_i2c_reg(int offset, int data);
#endif

#ifdef INTERNAL_AUDIO_DAC
void internal_dac_setting( void );
void internal_dac_reset( void );
#endif
void AUDHW_AutoCalibration(void);

#define	XCKCFG					HWREG_R(aud_ext_dac_xck_cfg)
#define	AUD_SET_XCK(x)			HWREG_W(aud_ext_dac_xck_cfg, (x))
#define	AUD_GET_XCK()			(HWREG_R(aud_ext_dac_xck_cfg))
#ifdef EMULATION
#define	AUD_SET_BCK(x)			(HWREG_R(aud_ext_dac_bck_cfg) = HWREG_R(aud_ext_dac_bck_cfg))
#else
#define	AUD_SET_BCK(x)			HWREG_W(aud_ext_dac_bck_cfg, (x))
#endif
#define	AUD_GET_BCK()			(HWREG_R(aud_ext_dac_bck_cfg))

#define	AUD_SET_IEC(x)			HWREG_W(aud_iec0_bclk_cfg, (x))
#define	AUD_SET_IEC_1(x)		HWREG_W(aud_iec1_bclk_cfg, (x))

#define	AUD_SET_ADC_XCK(x)		HWREG_W(aud_ext_adc_xck_cfg, (x))
#define	AUD_GET_ADC_XCK()		(HWREG_R(aud_ext_adc_xck_cfg))
#define	AUD_SET_ADC_BCK(x)		HWREG_W(aud_ext_adc_bck_cfg, (x))
#define	AUD_GET_ADC_BCK()		(HWREG_R(aud_ext_adc_bck_cfg))

// ccyang: where is idac ctrl?
//#define	REG_WT_IDAC_CRTL(x)	(HWREG_R(g62_reserved_1) = (x)) //82xx: g31.15
//#define	REG_RD_IDAC_CRTL()		(HWREG_R(g62_reserved_1)) //82xx: g31.15
/*
#define	REG_WT_IDAC_CRTL_g0(x)	(HWREG_R(g124_reserved[0]) = (x) )
#define	REG_WT_IDAC_CRTL_g1(x)	(HWREG_R(g124_reserved[1]) = (x) )
#define	REG_RD_IDAC_CRTL_g0()	(HWREG_R(g124_reserved[0]))
#define	REG_RD_IDAC_CRTL_g1()	(HWREG_R(g124_reserved[1]))
*/
#define	REG_WT_IDAC_CRTL_g0(x)	HWREG_W(otp_i2c_kp_otp_slave_addr, (x))
#define	REG_WT_IDAC_CRTL_g1(x)	HWREG_W(otp_i2c_macro_slave_addr, (x))
#define	REG_RD_IDAC_CRTL_g0()	(HWREG_R(otp_i2c_kp_otp_slave_addr))
#define	REG_RD_IDAC_CRTL_g1()	(HWREG_R(otp_i2c_macro_slave_addr))

#define	REG_WT_IDAC_XCK(x)		HWREG_W(aud_int_dac_xck_cfg, (x))
#define	REG_RD_IDAC_XCK()		(HWREG_R(aud_int_dac_xck_cfg))
#define	REG_WT_IDAC_BCK(x)		HWREG_W(aud_int_dac_bck_cfg, (x))
#define	REG_RD_IDAC_BCK()		(HWREG_R(aud_int_dac_bck_cfg))

// ccyang: do we have internal ADC ? Ans: No.
#if 0
#define	REG_WT_IADC_CRTL(x)	(HWREG_R(g62_reserved_1) = (x)) //82xx: g31.5
#define	REG_RD_IADC_CRTL()		(HWREG_R(g62_reserved_1)) //82xx: g31.5
#define	REG_WT_IADC_XCK(x)	(HWREG_R(g62_reserved_1) = (x)) //82xx: g31.26
#define	REG_RD_IADC_XCK()		(HWREG_R(g62_reserved_1)) //82xx: g31.26
#define	REG_WT_IADC_BCK(x)	(HWREG_R(g62_reserved_1) = (x)) //82xx: g31.27
#define	REG_RD_IADC_BCK()		(HWREG_R(g62_reserved_1)) //82xx: g31.27
#endif

#define	AUD_GET_I2S_MUTE_FLAG()	(HWREG_R(aud_ext_dac_bck_cfg) & 0x8000)

// ccyang: where are the pll registers defined? Ans: use DPLLA3 in group3
#ifdef HW_SUPPORT_AUDHW_USE_DIGITAL_PLL
#define	REG_WT_DPLLA_REMAIN(x)	HWREG_W(dpll3_remainder, (x))
#define	REG_WT_DPLLA_DENOM(x)	HWREG_W(dpll3_denominator, (x))
#define	REG_WT_DPLLA_DIV(x)		HWREG_W(dpll3_divider, (x))
#define	REG_WT_DPLLA_CTRL(x)	HWREG_W(dpll3_ctrl, (x))
// ccyang: no more need in QAE377
#define	REG_USE_DPLLA()			//(HWREG_R(sft_cfg35)  |= (1<<13))
#endif

#define	AUD_ENABLE_XCK_CLK()			HWREG_1(aud_ext_dac_xck_cfg, AUDCLK_XCK_ENABLE)
#define	AUD_DISABLE_XCK_CLK()			HWREG_0(aud_ext_dac_xck_cfg, ~AUDCLK_XCK_ENABLE)

#define	AUD_ENABLE_XCK_PAD()			HWREG_1(aud_ext_dac_xck_cfg, AUDCLK_XCKPAD_ENABLE)
#define	AUD_DISABLE_XCK_PAD()			HWREG_0(aud_ext_dac_xck_cfg, ~AUDCLK_XCKPAD_ENABLE)

#define	SET_AUDHWYA(x)	(HWREG_R(aud_audhwya) = (x)) //group 61.20
#define	SET_APPHWYA(x)	(HWREG_R(app_apphwya) = (x)) //group 72.1

#define	GET_SPDIF_IN_PERIOD() 		(HWREG_R(iec0_rx_debug_info)) //bit 9:0 RX0 sampe Rate Detect

#ifdef HW_SUPPORT_RAMP_DSP
// reg g67.13 for PCM ramp up/down
#define	SET_AUD_PCM_RAMP_DELTA(x)		HWREG_W(aud_grm_delta_ramp_pcm, ( (x<<8) & 0x0000ff00) )
#define	SET_AUD_PCM_RAMP_Num_Sample(x)	HWREG_W(aud_grm_delta_ramp_pcm, ( x & 0x000000ff) )
// bit0 controls PCM ramp up/down (set value: 1/0), ref g67.5
#define	SET_AUD_PCM_RAMP_CFG(x)			HWREG_W(aud_grm_gain_control_4, ( x & 0x00000001) )
#else
#define	SET_AUD_PCM_RAMP_DELTA(x)
#define	SET_AUD_PCM_RAMP_Num_Sample(x)
#define	SET_AUD_PCM_RAMP_CFG(x)
#endif

// ccyang: need to refine each bit value
#define	SET_AUD_AUDHW_RESET(x)		HWREG_W(audif_ctrl, (x))  //group 60.0

// ccyang: need to refine each bit value
#define	SET_AUD_AUDHW_ENABLE(x)		HWREG_W(aud_enable, (x)) //group 60.1

#endif

