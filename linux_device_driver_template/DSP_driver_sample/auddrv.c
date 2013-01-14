/******************************************************************************
 * Filename    : auddrv.c
 *
 * Description : auddrv implementation file
 * Language    : C Language
 *
 * Revision History:
 * ------------------
 * [090119]hcsu Initial Draft
 *
 * -----------------------------------------------------------------------
 * Copyright (C) 2007-2009 Sunplus Technology Co., Ltd.
 * All rights reserved. The information contained herein is the exclusive
 * property of SUNPLUS TECHNOLOGY CO., LTD. and shall not be distributed,
 * reproduced, or disclosed in whole or in part without prior written
 * permission of SUNPLUS TECHNOLOGY CO., LTD.
 *****************************************************************************/

#define AUDDRV_GLOBAL_HERE		// allocate auddrv globals here

/******************************************************************************
    Include files
 *****************************************************************************/
///#include "setting.h"
//#include "stypes.h"                 // need to check types.def
///#include "types.h"
///#include <asm/mach-sphe8600/types.h>

//#include "gsl/gl_api.h"
//#include "platform/spi_interface.h"
//#include "regmap.h"

#include <asm/mach-spv9200/spv9200_memlayout.h>
#include <asm/mach-spv9200/types.h>
#include <linux/time.h>

#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/io.h>

#include "include/aud_global.h"

#include "include/wave_fmt.h"
#include "include/auddrv.h"
#include "include/aud_hw.h"
#include "include/dsp_api.h"
#include "include/dsp_mem.h"

#include "include/aud_static.h"

#include "include/aud_postproc.h" //dylan:add for define 9200 post-proc algorithms

#include "include/status_reg.h"
#include "include/gl_modctrl.h"
#include "include/gl_i2c.h"
#include "include/audclk.h"
//#ifndef AUDIOTEAM_Module_Test
//#include "status_reg.h"
//#include "status_reg_def.h"
//#endif

/******************************************************************************
    Local Defines
 *****************************************************************************/
#define DEBUG_AUDDRV_MODULE

#ifdef DEBUG_AUDDRV_MODULE
#endif

//#define ENABLE_INLINE
#define UNUSED_ARGUMENT(x) (void)x

#ifdef	ENABLE_INLINE
#ifdef 	ARM_EVM
#define	INLINE	__inline
#else
#define	INLINE	inline
#endif  // ARM_EVM
#else
#define	INLINE
#endif  // ENABLE_INLINE

#ifdef AUD_SUPPORT_HDMI
#include "drivers/drv_hdmitx.h"
HdmiAudConfig_t hdmi_audcfg;	//ccyang
#endif

// [jyciou 20110527]for hdmi clock setting
#define orion_hdmi_clk
/******************************************************************************
    global variables
 *****************************************************************************/
auddrv_param   aud_param;
auddrv_param   aud_param_dec2;
WAVFORMATMACRO gWavFmt;
WAVFORMATMACRO gWavFmt_dec2;
extern WAVEFORMATEX2 gWmaParams;
// the parameter of audio memory alloc address
unsigned long __iomem *gAUDBufAddr;

// the parameter of init audio thread
#ifdef USE_RISC_AUDIOHandle
//GL_Task_t	AudioTaskHandle=NULL; //audio thread
//GL_Semaphore_t gAudioSemaphore = NULL;
#endif

// for AUDIF_Get_BytePtr
UINT32 audBS_byte_ptr;
UINT32 Dec2audBS_byte_ptr;

//frequence mask:48/96/192K
#define MASKTBL_MAXSIZE 3
UINT16 MaskTable[MASKTBL_MAXSIZE]={FS_MASK_48K, FS_MASK_96K, FS_MASK_192K};

// watchdog parameter
#ifdef DumpBar_Watchdog
static UINT32 prior_DSPbarrier=0;
static UINT32 prior_DSPdump=0;
static UINT32 dspWatchDog_cnt=0;
static UINT32 FIFOWatchDog_cnt=0;
static UINT32 prior_A5cnt=0;
#endif
static UINT16 dsp_freerun_cnt=0;
static UINT16 freerun_resetdsp_cnt=0;

// for spdifin
#ifdef SUPPORT_QAE377_SPDIF_IN
spdifin_info spdifin_setting;
spdifin_hw_period spdifin_hw_info;
_spdifin_hw_clk spdifin_hw_clk;
UINT32 spdifin_clk_sum = 0;
UINT32 hdmiin_default_fs = 48000;
WAVFORMATMACRO wfmt_spdifin;
#endif

// for IC VerB. DAC work around
static UINT8 dac_calibration_flag = 0;
static UINT8 dac_clk_source = 0;

// Backup aux mode and set aux mode to "off" while fast-forward mode
static UINT16 auxmode_backup = 0;
static UINT32 fifo_a0_cnt_last = 0;
static UINT32 fifo_a1_cnt_last = 0;
static UINT32 fifo_a2_cnt_last = 0;
static UINT8 fifo_watchdog_cnt = 0;

static UINT8 currSourceType = 0;
/******************************************************************************
    extern variables
 *****************************************************************************/
extern const BYTE VolTab_qae377[VOLUME_SCALE];
extern UINT16  MICVolTab_qae377[MIC_GAIN_SCALE];
extern const UINT16 echo_para_tbl[9][2];

#if 1//def DUAL_MPEG_MIXER
#define X16(n) (((((n)>>15)+1)>>1) - ((((n)>>15)+1)>>16))
extern const INT32 dB2Linear_Tab[DB2LINEAR_SCALE];
extern const BYTE Fade2dB_Tab[FADE_SCALE];
#endif

/******************************************************************************
    Function Implementations
 *****************************************************************************/
// Digital Output Configuration
#define DigiOutTab_ROW	9
#define DigiOutTab_COL	3
UINT16 DigitalOutputDefaultTab[DigiOutTab_ROW][DigiOutTab_COL]={
	{WAVE_FORMAT_DOLBY_AC3,             SPDIF_RAW,  HDMI_RAW}, // AC3
	{WAVE_FORMAT_DOLBY_DIGITAL_PLUS,    SPDIF_RAW,  HDMI_RAW}, // DDP
	{WAVE_FORMAT_DOLBY_DTS,             SPDIF_RAW,  HDMI_RAW}, // DTS
	{WAVE_FORMAT_DTSHD,                 SPDIF_RAW,  HDMI_RAW}, // DTSHD
	{WAVE_FORMAT_DTSHD_MASTER_AUDIO,    SPDIF_RAW,  HDMI_RAW}, // DTSHDMA
	{WAVE_FORMAT_TRUEHD,                SPDIF_RAW,  HDMI_RAW}, // TrueHD	
	{WAVE_FORMAT_AAC,                   SPDIF_PCM,  HDMI_PCM}, // AAC
	{WAVE_FORMAT_MPEG2AAC,              SPDIF_PCM,  HDMI_PCM}, // AAC
	{WAVE_FORMAT_MPEG,                  SPDIF_RAW,  HDMI_RAW}, // MPEG
};

//---------------------------------------------------------------------
//    download dsp related APIs
//---------------------------------------------------------------------
#if 0
void Init_Audio_Thread( void )
{
	#if 1 //william need to update
#ifdef USE_RISC_AUDIOHandle
	GL_Status_t dReturnValue;

	AudioTaskHandle = GL_TaskPoolGetHandle("Audio Thread");
	dReturnValue = GL_TaskPoolActivate(AudioTaskHandle);

	if (dReturnValue != GL_SUCCESS)
	{
		AUDDRV_PRINT("Audio_Main Task create failed.\n");
		GL_QueueDelete(AudioTaskHandle);

		return ;
	}

	//GL_TaskPoolSelfDelete();
#endif
#endif
}
#endif


void _Audio_OutputFifo_watchdog(void)
{
	UINT32 a0_cnt, a1_cnt, a2_cnt;

	a0_cnt = HWREG_R(aud_a0_cnt);
	a1_cnt = HWREG_R(aud_a1_cnt);
	a2_cnt = HWREG_R(aud_a2_cnt);	

    // if all FIFO counts are GT than 0, return
	if ((a0_cnt != 0) && (a1_cnt != 0) && (a2_cnt != 0))
	{
		fifo_a0_cnt_last = a0_cnt;
		fifo_a1_cnt_last = a1_cnt;
		fifo_a2_cnt_last = a2_cnt;		
		
		fifo_watchdog_cnt = 0;
		return;
	}

    // if all FIFO counts equal 0, return
	if ((a0_cnt == 0) && (a1_cnt == 0) && (a2_cnt == 0))
	{
		fifo_a0_cnt_last = a0_cnt;
		fifo_a1_cnt_last = a1_cnt;
		fifo_a2_cnt_last = a2_cnt;		
		
		fifo_watchdog_cnt = 0;
		return;
	}
    
	// at least one FIFO count empty.
	if ((a0_cnt==fifo_a0_cnt_last) || 
		(a1_cnt==fifo_a1_cnt_last) ||
		(a2_cnt==fifo_a2_cnt_last))
	{		
        // at least one FIFO count exploded
        if (((a0_cnt + 0x1000) > HWREG_R(aud_a0_length))||
            ((a1_cnt + 0x1000) > HWREG_R(aud_a1_length))||
            ((a2_cnt + 0x1000) > HWREG_R(aud_a2_length)))
        {
		fifo_watchdog_cnt++;
	}
	else
	{
		fifo_watchdog_cnt = 0;
	}
	}
	else
	{
		fifo_watchdog_cnt = 0;
	}

	fifo_a0_cnt_last = a0_cnt;
	fifo_a1_cnt_last = a1_cnt;
	fifo_a2_cnt_last = a2_cnt;

	if (fifo_watchdog_cnt >= 10)
	{
		fifo_watchdog_cnt = 0;
		AUDIF_Reset_DSP();
	}
	
}

int Audio_MainRoutine( void *pParam )
{
    // Used to receive dsp msg
	UINT32 dspMessage;

#ifdef AUD_ShowFIFOStatus
	// Used to prevent printing msg all the time, but print msg when buffer is empty first time.
	static bool _bIsPrintDACMsg = true;
	static bool _bIsPrintSPDIFMsg = true;
#endif

	while (1)
	{
		// receive dsp msg
		while(DSP_ERROR_NO_MESSAGE != AUDIF_Get_DspMessage(&dspMessage));

		AUDIF_Update_Dec1_SampleRate();
		AUDIF_Set_PSREG_ChnNum();

        if (SPDIF_IECRX == aud_param.spdif_mode)
        {
            AUDIF_Update_Spdifin_SampleRate();
        }

#ifdef AUD_Enable_TimeoutProtect
		// check if dsp wait state timeout
		if( 1 == DSP_Get_Timeout_Flag() )
		{
			//if 1, we will force to reset dsp
			DSP_Set_Timeout_Flag(0);
			AUDIF_Reset_DSP();
		}
		// check if sendcmd timeout or not
		if( 1 == DSP_Get_SendCmdTimeout_Flag() )
		{
			//if 1, we will force to reset dsp
			DSP_Set_SendCmdTimeout_Flag(0);
			AUDIF_Reset_DSP();
		}
#endif
		// to avoid fifo hangup issue.
		_Audio_OutputFifo_watchdog();
		
#ifdef AUD_Enable_MainDead_Watchdog
		if( (HWREG_R(dsp_port[12])& 0xf) == MAIN_STATE_DEAD )
		{
			AUDIF_Reset_DSP();
		}
#endif
#ifdef AUD_Enable_Watchdog
		// dsp watchdog
		AUDIF_Watchdog();
#endif

	#ifdef AUD_ShowFIFOStatus
		if( AUDIF_Get_DSPState( AUDIF_BANK_DEC1 )==TASK_PLAY )
		{
			// show aud fifo
			if( HWREG_R(aud_a0_cnt) == 0 )
			{
				if ( _bIsPrintDACMsg == true )
				{
					AUDDRV_PRINT("[DAC FIFO] CDEF\n");
					_bIsPrintDACMsg = false;
				}
			}
			else
			{
				_bIsPrintDACMsg = true;
			}

			if( HWREG_R(aud_a5_cnt) == 0 )
			{
				if ( _bIsPrintSPDIFMsg == true )
				{
					AUDDRV_PRINT("[SPDIF FIFO] CDEF\n");
					_bIsPrintSPDIFMsg = false;
				}
			}
			else
			{
				_bIsPrintSPDIFMsg = true;
			}

			//if( HWREG_R(aud_a6_cnt) == 0 )
			//	AUDDRV_PRINT("[HDMI FIFO] CDEF\n");
		}
	#endif
		DSP_Waitloop(100);

		// exit kthead
		if (kthread_should_stop())
			break;

	}

	return 0;
}

AUDIF_Status_e Audio_MemoryInit(UINT32 dAudioAnchor)
{
	gAUDBufAddr = dAudioAnchor;
	return AUDIF_SUCCESS;
}

#define DSP 3
#define INPUT_FREQ_KHZ	(27*1000)
#define KHZ		1000
#ifndef _BIT
#define _BIT(n)			(1<<(n))
#endif
static int gSysBusFreqFPGA = -1;
int GL_GetSysBusFreq(void)
{
	unsigned int cfg, ns, m, freq;

	cfg = HWREG_R(sspll_cfg);

	if (cfg & _BIT(31))
		return (int)INPUT_FREQ_KHZ*KHZ;

	ns = cfg & 0xff;
	if (ns<66 && ns >=3)
		ns += 2;
	m = (cfg >> 8) & 0x1f;
	if (m == 0) {
		return gSysBusFreqFPGA;
	}

	freq = 2 * INPUT_FREQ_KHZ * ns / m;
	freq *= KHZ;
	if (cfg & _BIT(16))
		freq  /= 2;
	if (cfg & _BIT(30))
		freq /= 2;

	return (int)freq;
}

void AUDIF_Init_Audio(void)
{

	//AUDIO_SEMAPHORE_CREATE();
	//DSPCMD_MUTEX_CREATE();

	AUDDRV_PRINT("AUDIF_Init_Audio::audio module initial setup\n");
	/*Add by william @20110208*/
	GL_moduleClockOn(DSP);
	GL_moduleResetOff(DSP);
	/*endadd*/
	// should initial aud_param
	aud_InitAudParams();

	//AUDIF_Set_BytePtr(0);  //hcsu:fixeme

	// initial dsp variables and allocate memory
    // 1. initiial dsp APIs internal static global variables
    // 2. initiial dsp code&data memory allocation and set to hardware releated registers
    // 3. initial risc parameters
    // 4. initial dsp memory layout
    AUDDRV_PRINT("\nAUDIO memory addr base: 0x%x\n\n", gAUDBufAddr);
    DSP_Initial(gAUDBufAddr , AUDIF_System_Boot_init);

    // initial dsp system  (download mainprogram)
    if (DSP_ERROR_NONE != DSP_InitSystem())
    {
    	//AUDIO_SEMAPHORE_GIVE();
        diag_printf("AUDIF_Init_Audio::e:DSP_InitSystem failed\n");
        return;
    }

	UINT32 mainID;
	mainID = (DSP_GetDspParam0(DSP_MAIN_ID)<<16)|DSP_GetDspParam0(DSP_MAIN_VERSION);

	DSP_PRINT("\nAUDIF_Init_Audio::mainID:0x%08x\n",mainID);

	AUDDRV_PRINT("AUDIF_Init_Audio::no needs to download a default coding mode??\n");
	// Needs to download a default codec?
	// AUDIF_Set_CodingMode(AUDIF_CODING_MODE_DEFAULT);

	// Semaphore post
	//AUDIO_SEMAPHORE_GIVE();

    //
    // set default channel mapping
    //
#if 0 //Leon
    AUDIF_DacMapping_t mapping;
	mapping.DacMapping_Data0_L = AUDIF_DAC_MAP_PCM0_L;
	mapping.DacMapping_Data0_R = AUDIF_DAC_MAP_PCM0_R;
	mapping.DacMapping_Data1_L = AUDIF_DAC_MAP_PCM1_L;
	mapping.DacMapping_Data1_R = AUDIF_DAC_MAP_PCM1_R;
	mapping.DacMapping_Data2_L = AUDIF_DAC_MAP_PCM2_L;
	mapping.DacMapping_Data2_R = AUDIF_DAC_MAP_PCM2_R;
    AUDIF_Set_ChannelMap(&mapping);
#endif
    // set default ADC pin-mux
    AUDIF_Set_AdcSourceSelect(AUDIF_ADC_SOURCE_ADC1);

    // init audio clock
    AUDHW_Init_AudHW_CLK();   

#ifdef SAMPLE_RATE_DEFAULT_BY_ID
	UINT16 R_SmpRate;
	R_SmpRate = aud_map_SmpRate_IDtoFS(K_PCM_OUTPUT_SAMPLE_RATE_ID);
	AUDIF_Set_SampleRate(R_SmpRate);
#else
	AUDIF_Set_SampleRate(FS_DEFAULT);
#endif

	AUDIF_Init_Speaker_Config(); // ccyang add
	AUDIF_Init_SpkDelay_Config();
	AUDIF_Init_SPKGain(0x80);
#ifdef QAE468_CERTI_BOARD
    // dylan: new certi-board pinmux config
    // clean bit 6:5
    HWREG_W(sft_cfg_1, HWREG_R(sft_cfg_1)&= ~(0x60));
    //HWREG_W(gpio_first[0], 0x0000); // set Ext-DAC GPIO
#endif

// 9200 DAC work-around : need to check IC version
//#define Write_MPEG_Reg(group, addr, Val)    (*((volatile UINT32 *)(0xbffe0000 + (((group<<5) + addr)<<2))) = (Val))
//#define Read_MPEG_Reg(group, addr)          (*((volatile UINT32 *)(0xbffe0000 + (((group<<5) + addr)<<2))))

    UINT8 bDVersion = 0;
    static UINT8 bDVersionCheckflag=0;//check IC version ONCE after system is initial...
    diag_printf("\n bDVersionCheckflag %x\n", bDVersionCheckflag);
    if((bDVersionCheckflag==0)&&(HWREG_R(stamp) == 0x331))
    {
        bDVersionCheckflag = 1;
#if 1
        // that is, IC VerB, C and D
        //enter_critical_section(0);
        UINT32 dwVal, dwTimeout=0xffffff;
        //Write_MPEG_Reg(205, 0x11, 0x6001);
		HWREG_W(dtv_cps_otp_ctrl, 0x6001);
        //dwVal = Read_MPEG_Reg(205, 0x13) & ~(0x14);     //clear OTP_CMD_ST_READ_DONE & OTP_CMD_ST_RELOAD_DONE
        dwVal = HWREG_R(dtv_cps_otp_cmd_st) & ~(0x14);     //clear OTP_CMD_ST_READ_DONE & OTP_CMD_ST_RELOAD_DONE
        //Write_MPEG_Reg(205, 0x13, dwVal);
		HWREG_W(dtv_cps_otp_cmd_st, dwVal);
        //Write_MPEG_Reg(205, 0x14, 0xEF8);
		HWREG_W(dtv_cps_otp_read_addr, 0xEF8);
        //Write_MPEG_Reg(205, 0x12, 0x3E02);
		HWREG_W(dtv_cps_otp_cmd, 0x3E02);
        //while (0 == (Read_MPEG_Reg(205, 0x13) & 0x4)) { //wait OTP_CMD_ST_RELOAD_DONE
        while (0 == (HWREG_R(dtv_cps_otp_cmd_st) & 0x4)) { //wait OTP_CMD_ST_RELOAD_DONE
            // do nothing, but timeout
            if((--dwTimeout)==0)
                break;
        }
        //Write_MPEG_Reg(205, 0x12, 0x3E04);
		HWREG_W(dtv_cps_otp_cmd, 0x3E04);
        //while (0 == (Read_MPEG_Reg(205, 0x13) & 0x10)){ //wait OTP_CMD_ST_READ_DONE
        while (0 == (HWREG_R(dtv_cps_otp_cmd_st) & 0x10)){ //wait OTP_CMD_ST_READ_DONE
            // do nothing, but timeout
            if((--dwTimeout)==0)
                break;
        }
        //bDVersion = Read_MPEG_Reg(205, 0x15);
        bDVersion = HWREG_R(dtv_cps_otp_read_data);
        //dwVal = Read_MPEG_Reg(205, 0x13) & ~(0x10);     //clear OTP_CMD_ST_READ_DONE
        dwVal = HWREG_R(dtv_cps_otp_cmd_st) & ~(0x10);     //clear OTP_CMD_ST_READ_DONE
        //Write_MPEG_Reg(205, 0x13, dwVal);
		HWREG_W(dtv_cps_otp_cmd_st, dwVal);

        //leave_critical_section(0);

		diag_printf("\nVersion %x\n", bDVersion);
#else
        bDVersion = 0;
#endif        
        if(bDVersion&0x1)
	    {
            // IC Ver.D
            diag_printf("#################### AUDIO version IC Ver.D\n");
            // set bck inverse
            HWREG_W(clk_sel_atv_dtv, (HWREG_R(clk_sel_atv_dtv)|(0x1 << 12)));
	        //HWREG_W(gpio_first[0], HWREG_R(gpio_first[0])|(1<<12));
	        //HWREG_W(gpio_oe[0], HWREG_R(gpio_oe[0])|(1<<12));
	        //HWREG_W(gpio_out[0], HWREG_R(gpio_out[0])|(1<<12));
        }
		else
        {
            // IC Ver.B&C
            if (dac_calibration_flag==0)
            {
                //UINT32 timestamp1, timestamp2;
				struct timeval timestamp1, timestamp2;
				do_gettimeofday(&timestamp1);
                //timestamp1 = GL_GetTimestamp();
                AUDIF_Set_MasterVolume(0);

                // backup avl setting and turn off avl
                UINT16 avl = DSP_GetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, 7,4);
                DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, 7,4,0);

                // turn off all sw app
                AUDIF_Set_SwApp2Switch(0);

                // set spk layout                
                UINT16 spk = DSP_GetRiscParam0(RISC_MAIN_SPK_OUTPUT);
                DSP_SetRiscParam0(RISC_MAIN_SPK_OUTPUT, 0x3ff);
	            DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
                // set emu mode = DC
                aud_param.testtone = 0x4;   //set emu mode = DC
                AUDIF_Set_CodingMode(AUDIF_CODING_MODE_EMU);
                
                AUDIO_SEMAPHORE_TAKE();
                AUDHW_AutoCalibration();
                AUDIO_SEMAPHORE_GIVE();
                
                AUDIF_Set_Stop( AUDIF_BANK_DEC1 );
                //timestamp2 = GL_GetTimestamp();
				do_gettimeofday(&timestamp2);
                diag_printf("#################### waste %d us to check AUDIO phase.[%d/%d]\n", timestamp2.tv_usec -timestamp1.tv_usec, timestamp1.tv_usec, timestamp2.tv_usec);
                dac_calibration_flag = 1;

                // set avl value
                DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, 7,4,avl);
                DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);                
                // set spk layout
                DSP_SetRiscParam0(RISC_MAIN_SPK_OUTPUT, spk);
	            DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
            }

        }
    }

	AUDIF_Set_FIFOSampleCnt(FIFOSmpCnt_Init);
	AUDIF_Set_FIFOSampleCnt(FIFOSmpCnt_Enable);


    // 20110614 temp add here
    // for spdif bypass mode setting

    spdifin_hw_info.idx = 0;
    spdifin_hw_info.cnt = 0;
    spdifin_hw_info.period = 0;

    int i = 0;
    for (i=0; i<SPDIFIN_HW_PERIOD_BUF_LEN; i++)
        spdifin_hw_info.buf[i] = 0;


    wfmt_spdifin.wfx.wFormatTag = WAVE_FORMAT_BigEndian_LPCM;
    wfmt_spdifin.wfx.nChannels = 2;
    wfmt_spdifin.wfx.wBitsPerSample = 16;
    wfmt_spdifin.wfx.nSamplesPerSec = 44100;
    
    
    // init clk related global variables
	spdifin_hw_clk.sysclk = GL_GetSysBusFreq();	
    
    spdifin_hw_clk.fs_192k.mid = spdifin_hw_clk.sysclk/(16*192000);        
    
    spdifin_hw_clk.fs_176k.mid = spdifin_hw_clk.sysclk/(16*176400);    
    spdifin_hw_clk.fs_96k.mid = spdifin_hw_clk.sysclk/(16*96000);        
    spdifin_hw_clk.fs_88k.mid = spdifin_hw_clk.sysclk/(16*88200);
    spdifin_hw_clk.fs_48k.mid = spdifin_hw_clk.sysclk/(16*48000);
    spdifin_hw_clk.fs_44k.mid = spdifin_hw_clk.sysclk/(16*44100);    
    spdifin_hw_clk.fs_32k.mid = spdifin_hw_clk.sysclk/(16*32000);

    spdifin_hw_clk.fs_192k.hi = (spdifin_hw_clk.fs_192k.mid + spdifin_hw_clk.fs_176k.mid)/2;
    spdifin_hw_clk.fs_176k.hi = (spdifin_hw_clk.fs_176k.mid + spdifin_hw_clk.fs_96k.mid)/2;
    spdifin_hw_clk.fs_96k.hi = (spdifin_hw_clk.fs_96k.mid + spdifin_hw_clk.fs_88k.mid)/2;    
    spdifin_hw_clk.fs_88k.hi = (spdifin_hw_clk.fs_88k.mid + spdifin_hw_clk.fs_48k.mid)/2;    
    spdifin_hw_clk.fs_48k.hi = (spdifin_hw_clk.fs_48k.mid + spdifin_hw_clk.fs_44k.mid)/2;            
    spdifin_hw_clk.fs_44k.hi = (spdifin_hw_clk.fs_44k.mid + spdifin_hw_clk.fs_32k.mid)/2;        
    spdifin_hw_clk.fs_32k.hi = spdifin_hw_clk.fs_32k.mid*(1+(1/100));

    spdifin_hw_clk.fs_192k.lo = spdifin_hw_clk.fs_192k.mid*(1-(1/100));    
    spdifin_hw_clk.fs_176k.lo = spdifin_hw_clk.fs_192k.hi+1;    
    spdifin_hw_clk.fs_96k.lo = spdifin_hw_clk.fs_176k.hi+1;
    spdifin_hw_clk.fs_88k.lo = spdifin_hw_clk.fs_96k.hi+1;
    spdifin_hw_clk.fs_48k.lo = spdifin_hw_clk.fs_88k.hi+1;    
    spdifin_hw_clk.fs_44k.lo = spdifin_hw_clk.fs_48k.hi+1;    
    spdifin_hw_clk.fs_32k.lo = spdifin_hw_clk.fs_44k.hi+1;    

    //AUDIO_SEMAPHORE_TAKE();
    AUDDRV_PRINT("[AUDDRV] sysclk = %d\n", spdifin_hw_clk.sysclk);
    AUDDRV_PRINT("[AUDDRV]32k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_32k.lo, spdifin_hw_clk.fs_32k.mid, spdifin_hw_clk.fs_32k.hi);
    AUDDRV_PRINT("[AUDDRV]44k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_44k.lo, spdifin_hw_clk.fs_44k.mid, spdifin_hw_clk.fs_44k.hi);
    AUDDRV_PRINT("[AUDDRV]48k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_48k.lo, spdifin_hw_clk.fs_48k.mid, spdifin_hw_clk.fs_48k.hi);    
    AUDDRV_PRINT("[AUDDRV]88k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_88k.lo, spdifin_hw_clk.fs_88k.mid, spdifin_hw_clk.fs_88k.hi);    
    AUDDRV_PRINT("[AUDDRV]96k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_96k.lo, spdifin_hw_clk.fs_96k.mid, spdifin_hw_clk.fs_96k.hi);    
    AUDDRV_PRINT("[AUDDRV]176k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_176k.lo, spdifin_hw_clk.fs_176k.mid, spdifin_hw_clk.fs_176k.hi);    
    AUDDRV_PRINT("[AUDDRV]192k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_192k.lo, spdifin_hw_clk.fs_192k.mid, spdifin_hw_clk.fs_192k.hi);

#ifdef DEBUG_DDCO
    AUDIF_Enable_AC3TransCoding();
#endif
}

UINT16 aud_map_SmpRate_IDtoFS(UINT16 samplerateID)
{
	UINT16 sampleRate = FS_48K;
	switch(samplerateID)
	{

	case K_DSP_SAMPLE_RATE_ID_44K:
		sampleRate = FS_44K;
		break;
	case K_DSP_SAMPLE_RATE_ID_88K:
		sampleRate = FS_88K;
		break;
	case K_DSP_SAMPLE_RATE_ID_22K:
		sampleRate = FS_22K;
		break;
	case K_DSP_SAMPLE_RATE_ID_176K:
		sampleRate = FS_176K;
		break;
	case K_DSP_SAMPLE_RATE_ID_11K:
		sampleRate = FS_11K;
		break;
//	case 0x5:
//		sampleRate = 5512;
//		break;
	case K_DSP_SAMPLE_RATE_ID_48K:
		sampleRate = FS_48K;
		break;
	case K_DSP_SAMPLE_RATE_ID_96K:
		sampleRate = FS_96K;
		break;
	case K_DSP_SAMPLE_RATE_ID_24K:
		sampleRate = FS_24K;
		break;
	case K_DSP_SAMPLE_RATE_ID_192K:
		sampleRate = FS_192K;
		break;
	case K_DSP_SAMPLE_RATE_ID_12K:
		sampleRate = FS_12K;
		break;
//	case 0x15:
//		sampleRate = 6000;
//		break;
	case K_DSP_SAMPLE_RATE_ID_32K:
		sampleRate = FS_32K;
		break;
	case K_DSP_SAMPLE_RATE_ID_64K:
		sampleRate = FS_64K;
		break;
	case K_DSP_SAMPLE_RATE_ID_16K:
		sampleRate = FS_16K;
		break;
	case K_DSP_SAMPLE_RATE_ID_128K:
		sampleRate = FS_128K;
		break;
	case K_DSP_SAMPLE_RATE_ID_8K:
		sampleRate = FS_8K;
		break;
//	case 0x25:
//		sampleRate = 6000;
//		break;
//	case 0x30:
//		sampleRate = 768000;
//		break;
	default:
		//DSP_PRINT("aud_SetPCMSampleRate::FS_DEFAULT(FS_48K)\n");
		sampleRate = FS_DEFAULT;
	}

	return sampleRate;
}

void aud_SetIEC0SampleRate(UINT16 samplerateID)
{
	UINT16 sampleRate = FS_48K;
	switch(samplerateID)
	{

	case 0x0:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_44k\n");
		sampleRate = FS_44K;
		break;
	case 0x1:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_88K\n");
		sampleRate = FS_88K;
		break;
	case 0x2:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_22K\n");
		sampleRate = FS_22K;
		break;
	case 0x3:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_176K\n");
		sampleRate = FS_176K;
		break;
	case 0x4:
		DSP_PRINT("aud_SetPCMSampleRate::FS_11K\n");
		sampleRate = FS_11K;
		break;
//	case 0x5:
//		sampleRate = 5512;
//		break;
	case 0x10:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_48K\n");
		sampleRate = FS_48K;
		break;
	case 0x11:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_96K\n");
		sampleRate = FS_96K;
		break;
	case 0x12:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_24K\n");
		sampleRate = FS_24K;
		break;
	case 0x13:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_192K\n");
		sampleRate = FS_192K;
		break;
	case 0x14:
		sampleRate = FS_12K;
		DSP_PRINT("aud_SetPCMSampleRate::FS_12K\n");
		break;
//	case 0x15:
//		sampleRate = 6000;
//		break;
	case 0x20:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_32K\n");
		sampleRate = FS_32K;
		break;
	case 0x21:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_64K\n");
		sampleRate = FS_64K;
		break;
	case 0x22:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_16K\n");
		sampleRate = FS_16K;
		break;
	case 0x23:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_128K\n");
		sampleRate = FS_128K;
		break;
	case 0x24:
		DSP_PRINT("aud_SetPCMSampleRate::FS_8K\n");
		sampleRate = FS_8K;
		break;
//	case 0x25:
//		sampleRate = 6000;
//		break;
//	case 0x30:
//		sampleRate = 768000;
//		break;
	default:
		DSP_PRINT("aud_SetIEC0SampleRate::FS_DEFAULT(FS_48K)\n");
		sampleRate = FS_DEFAULT;
	}
	AUDHW_Set_IEC0_CLK(sampleRate);
}

void AUDIF_Reset_DSP(void)
{
	INT32 MasterVolume;

	//AUDIF_Get_MasterVolume(&MasterVolume);	// to keep volume after reset
	MasterVolume = aud_param.master_volume;

	AUDIO_SEMAPHORE_TAKE();

	//force to re-load same DSP audio decoder
	diag_printf("AUDIF_Reset_DSP::force to reload dsp codec again\n");

	DSP_Initial(gAUDBufAddr,AUDIF_Reset_DSP_init);
//	DSP_Initial(gAUDBufAddr);
	DSP_InitSystem();

	AUDIF_Set_SampleRate(aud_param.smprate_info.smp_rate_now);

	HWREG_W(dsp_port[2], 0);
	HWREG_W(dsp_port[3], 0);

	aud_SetCodecParams(aud_param.coding_mode_now);

	if (DSP_ERROR_NONE != DSP_InitCodec(CODEC_BANK_DEC1))
	{
		ERROR_PRINT("AUDIF_Set_CodingMode::e:DSP_InitCodec(CODEC_BANK_DEC1) failed\n");
		AUDIO_SEMAPHORE_GIVE();
		return;
	}

	UINT32 dec1ID;
	dec1ID = (DSP_GetDspParam0(DSP_DEC1_ID)<<16)|DSP_GetDspParam0(DSP_DEC1_VERSION);
	DSP_PRINT("\nAUDIF_Set_CodingMode::dec1ID:0x%08x\n\n",dec1ID);

	AUDIO_SEMAPHORE_GIVE();

	AUDIF_Set_MasterVolume(MasterVolume);

}

void AUDIF_Set_CodingMode(UINT32 coding_mode)
{
	AUDIO_SEMAPHORE_TAKE();

    AUDDRV_PRINT("AUDIF_Set_CodingMode::set coding_mode:0x%x, coding_mode_now:0x%x\n",coding_mode,aud_param.coding_mode_now);

	HWREG_W(dsp_port[2], 0);
	HWREG_W(dsp_port[3], 0);

	// always reset coding mode to make updated parameters effect

	if (coding_mode == aud_param.coding_mode_now)
	{
		// no reset dsp memory layout

		AUDDRV_PRINT("AUDIF_Set_CodingMode::coding mode the same, reset codec parameters\n");
		aud_ResetCodecPara(coding_mode);
		DSP_SetSampleRate(0x00FFFFFF); // initial sample rate of dec1

		AUDIO_SEMAPHORE_GIVE();
		return;
	}

	// Check if decoder enabled?
	// AUDIF layer, use coding_mode instead of codec ID
	//	UINT16 decID = aud_GetDecID(coding_mode);
	//	UINT16 encID = aud_GetEncID(coding_mode);
	//   AUDDRV_PRINT("AUDIF_Set_CodingMode::decID:0x%x\n",decID);

	// Update risc parameters according (should update to latest menu selection)??
	// hcsu: add related update function: void DSP_UpdateRiscParams(void) later

	// Download decoder if needed
	if (AUDIF_CODING_MODE_SYSTEM != coding_mode)
	{
		aud_SetCodecParams(coding_mode);

	    if (DSP_ERROR_NONE != DSP_InitCodec(CODEC_BANK_DEC1))
	    {
    	    ERROR_PRINT("AUDIF_Set_CodingMode::e:DSP_InitCodec(CODEC_BANK_DEC1) failed\n");
			AUDIO_SEMAPHORE_GIVE();
        	return;
	    }
		//AUDIF_Set_Channel();
	}

	// Download encoder if needed
	/*
	if (AUDIF_CODING_MODE_SYSTEM != encID)
	{
		aud_SetCodecParams(encID);

	    if (DSP_ERROR_NONE != DSP_InitCodec(CODEC_BANK_ENC))
	    {
    	    ERROR_PRINT("AUDIF_Set_CodingMode::e:DSP_InitCodec(CODEC_BANK_ENC) failed\n");
        	return;
	    }
	}
	*/

#if 0

        //Steven 2006/12/26 turn off mic when dsp decoder switch to solve the pop noise problem ,
        //because of mic-mix harware initialized not well(buffer is not completed clear).
        AUDIF_Set_MICStatus(STATUS_OFF);

        AUDIF_Set_Stop();

        //Steven 2008/4/22 , turn off audio path of HDMI before reload dsp to reduce pop noise
        Disable_HDMI();

        AUDKRNL_Download_Codec(coding_mode);

        dsp3_wait_dsp_port0_5(0,1,DSPSTOPSTATE,dsp_wait_port5_counter);

        buf_barrier_size = ((coding_mode==AUDIF_CODING_MODE_LPCM) ? 1024:8);
        AUDIF_Set_Play();//2004AUDDRV AudioSetPlay();

        //Steven 2008/5/14 , determine to enable HDMI Audio or not by SPDIF state variable
        Enable_HDMI();
#endif


	// finished downloading, set the channel?
	//AUDIF_Set_Channel();

	UINT32 dec1ID;
	dec1ID = (DSP_GetDspParam0(DSP_DEC1_ID)<<16)|DSP_GetDspParam0(DSP_DEC1_VERSION);
	DSP_PRINT("\nAUDIF_Set_CodingMode::dec1ID:0x%08x\n\n",dec1ID);

	aud_param.coding_mode_now = coding_mode;

	AUDIO_SEMAPHORE_GIVE();
}


INT32 AUDIF_Set_Dec2CodingMode(UINT32 coding_mode)
{
	AUDIO_SEMAPHORE_TAKE();

	HWREG_W(dsp_port[4], 0);
	HWREG_W(dsp_port[5], 0);

	if (coding_mode == aud_param_dec2.coding_mode_now)
	{
		// no reset dsp memory layout
		AUDDRV_PRINT("AUDIF_Set_Dec2CodingMode::coding mode the same, reset codec parameters\n");
		aud_SetDec2CodecParams(coding_mode);
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);		
		//DSP_SetSampleRate(0x00FFFFFF); // initial sample rate of dec2

		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_SUCCESS;
	}
	
	// always redownload
	if (AUDIF_CODING_MODE_SYSTEM != coding_mode)
	{
		aud_SetDec2CodecParams(coding_mode);

	    if (DSP_ERROR_NONE != DSP_InitCodec(CODEC_BANK_DEC2))
	    {
    	    ERROR_PRINT("AUDIF_Set_Dec2CodingMode::e:DSP_InitCodec(CODEC_BANK_DEC2) failed\n");
			AUDIO_SEMAPHORE_GIVE();
			return AUDIF_ERR_Download_Failed;
	    }
	}

	UINT32 dec2ID;
	dec2ID = (DSP_GetDspParam0(DSP_DEC2_ID)<<16)|DSP_GetDspParam0(DSP_DEC2_VERSION);
	DSP_PRINT("\nAUDIF_Set_CodingMode::dec2ID:0x%08x\n\n",dec2ID);

	aud_param_dec2.coding_mode_now = coding_mode;

	AUDIO_SEMAPHORE_GIVE();

	return AUDIF_SUCCESS;
}
#if 0 //william removed, for major release2 code merge
UINT32 AUDIF_Get_Bank0Ver( void )
{
	UINT32 dspID;

	dspID = (DSP_GetDspParam0(DSP_MAIN_ID)<<16)|DSP_GetDspParam0(DSP_MAIN_VERSION);

	//AUDDRV_PRINT("[MAIN]0x%08x\n",dspID);

	return dspID;
}
#else
void AUDIF_Get_Bank0Ver( UINT32 *dspID )
{
	*dspID = (DSP_GetDspParam0(DSP_MAIN_ID)<<16)|DSP_GetDspParam0(DSP_MAIN_VERSION);

	//AUDDRV_PRINT("[MAIN]0x%08x\n",dspID);

	return;
}
#endif

void AUDIF_Get_Dec1Ver( UINT32 *dspID )
{
	*dspID = (DSP_GetDspParam0(DSP_DEC1_ID)<<16)|DSP_GetDspParam0(DSP_DEC1_VERSION);

	//diag_printf("[DEC1]0x%08x\n",*dspID);

	return ;

}

void AUDIF_Get_Decoder_Mode( void )
{

	UINT16 dec1ID;
	UINT32 dec1Ver;

	dec1ID = DSP_GetDspParam0(DSP_MAIN_ID);
	if( (dec1ID>>8)== ID_MAIN )
		diag_printf("DSP CODEC ID:[MAIN] ");
	dec1Ver = (((dec1ID&0xff)<<16) | DSP_GetDspParam0(DSP_MAIN_VERSION));
	diag_printf("%06x\n",dec1Ver);

	dec1ID=0;
	dec1Ver=0;

	dec1ID = DSP_GetDspParam0(DSP_DEC1_ID);

	switch((dec1ID>>8))
	{
		case ID_DECODER_PCM:
			diag_printf("DSP CODEC ID:[PCM] ");
			break;
		case ID_DECODER_DDP:
			diag_printf("DSP CODEC ID:[DDP] ");
			break;
		case ID_DECODER_TRUEHD:
			diag_printf("DSP CODEC ID:[TRUEHD] ");
			break;
		case ID_DECODER_DTSLBR:
			diag_printf("DSP CODEC ID:[DTSLBR] ");
			break;
		case ID_DECODER_DTSHD:
			if( aud_param.coding_mode_now == AUDIF_CODING_MODE_DTS )
				diag_printf("DSP CODEC ID:[DTS] ");
			else if( aud_param.coding_mode_now == AUDIF_CODING_MODE_DTSHD)
			diag_printf("DSP CODEC ID:[DTSHD] ");
			else
				diag_printf("DSP CODEC ID:[Err] ");
			break;
		case ID_DECODER_MPEG:
			diag_printf("DSP CODEC ID:[MPEG] ");
			break;
		case ID_DECODER_AAC:
			diag_printf("DSP CODEC ID:[AAC] ");
			break;
		case ID_DECODER_WMA:
			diag_printf("DSP CODEC ID:[WMA] ");
			break;
		case ID_DECODER_RA:
			diag_printf("DSP CODEC ID:[RA] ");
			break;
		case ID_DECODER_OGG:
			diag_printf("DSP CODEC ID:[OGG] ");
			break;
		case ID_DECODER_EMU:
			diag_printf("DSP CODEC ID:[EMU] ");
			break;
		case ID_DECODER_WMAPRO:
			diag_printf("DSP CODEC ID:[WMAPRO] ");
			break;
		case ID_DECODER_APE:
			diag_printf("DSP CODEC ID:[APE] ");
			break;
		case ID_DECODER_AVS:
			diag_printf("DSP CODEC ID:[AVS] ");
			break;
		default:
			diag_printf("DSP CODEC ID:[unknown] ");
			break;
	}

	dec1Ver = (((dec1ID&0xff)<<16) | DSP_GetDspParam0(DSP_DEC1_VERSION));
	diag_printf("%06x\n",dec1Ver);

}

UINT32 AUDIF_Get_DSPCodingMode( void )
{
	AUDDRV_PRINT("audio coding mode now: 0x%x\n",aud_param.coding_mode_now);
	return aud_param.coding_mode_now;
}

void AUDIF_Audio_Shut_Down(void)
{
	AUDDRV_PRINT("AUDIF_Audio_Shut_Down::fixed me\n");

    AUDIF_Set_Stop( AUDIF_BANK_DEC1 );
    //delay_srv_10ms(20);
    //delay_1ms(200);       // wait FIFO audio data have already clean up?
    ///AUD_RESET();         // AUD_RESET? check
}

void AUDIF_Init_Audio_CLK(void)
{
	AUDIO_SEMAPHORE_TAKE();
	AUDDRV_PRINT("AUDIF_Init_Audio_CLK::  audio clock initial setup\n");

//    AUDHW_Init_AudHW_CLK();
	AUDIO_SEMAPHORE_GIVE();
}

//*******************************************************
//	temp API for 8600
//*******************************************************

int AUDIF_DSP3_Get_abv_used(void)
{
	return 0;	// no used anymore
}

BYTE* AUDIF_Get_abv_start(void)
{
	return 0;	// no used anymore
}
UINT16 AUDIF_Get_abv_size(void)
{
	return 0;	// no used anymore

}
int AUDIF_DSP3_Get_abv_free(void)
{
	return 0;	// no used anymore
}

void AUDIF_Set_BytePtr(UINT32 val )
{
	AUDIO_SEMAPHORE_TAKE();

	audBS_byte_ptr= val;

	AUDIO_SEMAPHORE_GIVE();
}

UINT32 AUDIF_Get_BytePtr(void)
{
	return audBS_byte_ptr;	// should not used anymore
}
#if 0
void AUDIF_Set_Dec2BytePtr(UINT32 val )
{
	AUDIO_SEMAPHORE_TAKE();

	Dec2audBS_byte_ptr= val;

	AUDIO_SEMAPHORE_GIVE();
}

UINT32 AUDIF_Get_Dec2BytePtr(void)
{
	return Dec2audBS_byte_ptr;

}
#endif
//*******************************************************
//	Audio decode/encode Buffer Control APIs
//*******************************************************

INT32 AUDIF_Get_audBSbuf_free( UINT8 BankID )
{
	INT32  bsbufFree = 0;
	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_audBSbuf_free::AUDIF_ERR_Codec_BankID\n");
		return 0;
	}

	bsbufFree = (INT32)DSP_GetBsBufFree(DSP_BSBufID);

	//AUDDRV_PRINT("AUDIF_Get_audBSbuf_free::0x%x(%d)\n",bsbufFree,bsbufFree);

	return bsbufFree;

}

INT32 AUDIF_Get_audBSbuf_used(UINT8 BankID)
{
	INT32 bsbufUsed = 0;
	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_audBSbuf_used::AUDIF_ERR_Codec_BankID\n");
		return 0;
	}

	bsbufUsed = (INT32)DSP_GetBsBufUsed(DSP_BSBufID);

	//AUDDRV_PRINT("AUDIF_Get_audBSbuf_used::0x%x(%d)\n",bsbufUsed,bsbufUsed);

	return bsbufUsed;
}

BYTE* AUDIF_Get_audBSbuf_start(UINT8 BankID)
{
	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_audBSbuf_start::AUDIF_ERR_Codec_BankID\n");
		return 0;
	}

	return (BYTE*)DSP_GetBsBufStart(DSP_BSBufID);

}

INT32 AUDIF_Get_audBSbuf_size(UINT8 BankID)
{
//	AUDIO_SEMAPHORE_TAKE();

	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_audBSbuf_size::AUDIF_ERR_Codec_BankID\n");
//	AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

	INT32  bsbufSize = (INT32)DSP_GetBsBufSize(DSP_BSBufID);

	//AUDDRV_PRINT("AUDIF_Get_audBSbuf_size::0x%x(%d)\n", bsbufSize,bsbufSize);

//	AUDIO_SEMAPHORE_GIVE();

	return bsbufSize;
}

AUDIF_Status_e AUDIF_Reset_Audio_Buf(UINT8 BankID)
{
	UINT32 DSP_State = 0;
	UINT8 DSP_BSBufID = 0;

	DSP_State = AUDIF_Get_DSPState( BankID );

	// check if bank ID is legal or not
	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Reset_Audio_Buf::AUDIF_ERR_Codec_BankID\n");
		return AUDIF_ERR_Codec_BankID;
	}

	if( DSP_State == TASK_STOP )
	{
		AUDDRV_PRINT("AUDIF_Reset_Audio_Buf::BSBUF ID %d\n",DSP_BSBufID);
		AUDIF_Set_DumpPtr(DSP_BSBufID,0);
		AUDIF_Set_BarrierPtr(DSP_BSBufID,0);
	}
	else
	{
		AUDDRV_PRINT("AUDIF_Reset_Audio_Buf failed(non_stop_state)\n");
		return AUDIF_ERR_INVALID_State;
	}

	return AUDIF_SUCCESS;

}

INT32 AUDIF_Clean_Audio_Buf(UINT8 BankID)
{
/******************************************************
*	clean audio buffer to zero
******************************************************/

	UINT8 DSP_BSBufID = 0;

	// check if bank ID is legal or not
	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Clean_Audio_Buf::AUDIF_ERR_Codec_BankID\n");
		return AUDIF_ERR_BankID;
	}

	// stop decoder
	DSP_SetCodecStop(DSP_BSBufID);

	// reset dump and barrier
	DSP_SetDumpPtr(DSP_BSBufID, 0 );
	DSP_SetBarrierPtr(DSP_BSBufID, 0 );

	// clean buf
	INT32 bsbufSize = (INT32)DSP_GetBsBufSize(DSP_BSBufID);
	BYTE *BS_start = (BYTE*)DSP_GetBsBufStart(DSP_BSBufID);
	memset(BS_start, 0, bsbufSize);

	AUDDRV_PRINT("AUDIF_Clean_Audio_Buf::BSBUF ID %d\n",DSP_BSBufID);
	return AUDIF_SUCCESS;

}

UINT32 AUDIF_BS_BufFeed( UINT8* pBsSrcBuf, UINT32 aFeedSize )
{
	UINT32 return_msg;

	return_msg = DSP_BsBufFeed_Cached( AUDIF_BANK_DEC1 , pBsSrcBuf , aFeedSize );

	if( return_msg == DSP_ERROR_BSBUF_FEED )
	{
		diag_printf("AUDIF_BS_BufFeed::e:bitstream buffer source is null\n");
		diag_printf("AUDIF_BS_BufFeed::e:bs address is null\n");
		return AUDIF_ERR_BSBUF_FEED;
	}
	else if( return_msg == DSP_ERROR_BSBUF_ID )
	{
		diag_printf("AUDIF_BS_BufFeed::e:dsp bitstream buffer not enabled\n");
		return AUDIF_ERR_BSBUF_ID;
	}
	else if ( return_msg == DSP_ERROR_BS_WAIT )
	{
		diag_printf("AUDIF_BS_BufFeed::e:wait dsp consume data!\n");
		return AUDIF_ERR_BS_WAIT;
	}
	else
		return AUDIF_SUCCESS;


}

BYTE AUDIF_Get_ChnSwitch_Mode( void )
{
	// plz refer ch_mode_e in auddrv.h
	// return karaoke channel switch mode
	return aud_param.out_mode.karaoke_mode;
}

BYTE AUDIF_Get_AC3Karaoke_Mode( void )
{
	// plz refer AC3_Karaoke_e in auddrv.h
	// return AC3 karaoke mode
	if( aud_param.out_mode.ac3_karaoke_mode == OUT_NoVocal )
		return OUT_NoVocal;
	else if( aud_param.out_mode.ac3_karaoke_mode == OUT_LeftVocal)
		return OUT_LeftVocal;
	else if( aud_param.out_mode.ac3_karaoke_mode == OUT_RightVocal)
		return OUT_RightVocal;
	else if( aud_param.out_mode.ac3_karaoke_mode == OUT_BothVocal)
		return OUT_BothVocal;
	else
		return AUDIF_FAILURE;

}


void AUDIF_Set_Karaoke_Encode_Mode(INT32 mode)
{

	//AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	AUDIO_SEMAPHORE_TAKE();

	// add codes here
	UNUSED_ARGUMENT(mode);

	AUDIO_SEMAPHORE_GIVE();

}

//*******************************************************
//	DSP Flow Control APIs
//*******************************************************
UINT8 AUDIF_Check_CodecBankID(UINT8 BankID)
{
	UINT8 DSP_BankID = 0;
	switch( BankID )
	{
		case AUDIF_BANK_MAIN:
			DSP_BankID = CODEC_BANK_SYSTEM;
			break;
		case AUDIF_BANK_DEC1:
			DSP_BankID = CODEC_BANK_DEC1;
			break;
		case AUDIF_BANK_DEC2:
			DSP_BankID = CODEC_BANK_DEC2;
			break;
		case AUDIF_BANK_ENC:
			DSP_BankID = CODEC_BANK_ENC;
			break;
		default:
			diag_printf("[Err]AUDIF_Check_SystemBankID:: Error Codec ID %d\n", BankID);
			return AUDIF_ERR_Codec_BankID;
	}

	return DSP_BankID;

}

INT32 AUDIF_Set_FastForward( UINT8 BankID, UINT16 speed)
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	UINT8 DSP_BankID = 0;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_FastForward:: Bank ID %d\n", BankID );

#if 0
	if( speed > FF_32x )
	{
		AUDDRV_PRINT("[Err]AUDIF_Set_FastForward::FAILURE\n");
		return AUDIF_FAILURE;
	}
#endif

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Set_FastForward::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	// Check FIFO state to avoid decoder transition hang up
	AUD_Check_FIFOState();

    // 9200 PCM output FIFO no aux output on FF mode
    auxmode_backup = DSP_GetRiscParam1(RISC_MAIN_AUXOUTPUT);
    AUDDRV_PRINT("auxmode = %d\n", auxmode_backup);

#if 0
    UINT16  aux1mode = auxmode_backup&0xFF;
    UINT16  aux2mode = (auxmode_backup>>8)&0xFF;    

    if (aux1mode == AUDIF_AUX1_PATH_MIXER)
    {
        aux1mode = AUDIF_AUX1_PATH_OFF;
    }
    
    if (aux2mode == AUDIF_AUX2_PATH_MIXER)
    {
        aux1mode = AUDIF_AUX1_PATH_OFF;
    }
    
    DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, ((aux2mode<<8)|aux1mode));
#else
    DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, 0x0303);
#endif
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    
	if (DSP_ERROR_NONE != DSP_SetCodecFastForward(DSP_BankID,speed))
	{
		eStatus = AUDIF_FAILURE;
	}

	// ramp up
	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x00_RAMP_UP,0,0);
    AUDIF_RampUp_PCM_Out();

#if 0 // FF control by decoder
#ifdef USE_RISC_PARSER_PARAM
	switch( DSP_BankID )
	{
		case CODEC_BANK_DEC1:
			DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, speed );
			break;
		case CODEC_BANK_DEC2:
			AUDDRV_PRINT("AUDIF_Set_Stop::fix me\n");
			//DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, speed );
			break;
		default:
			diag_printf("[Err]AUDIF_Set_Stop:: Error Codec ID %d for FF_RATIO", DSP_BankID );
			eStatus = AUDIF_ERR_Codec_BankID;
			AUDIO_SEMAPHORE_GIVE();
			return eStatus;
	}
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
#endif
#endif
	AUDIO_SEMAPHORE_GIVE();
	return eStatus;
}


INT32 AUDIF_Set_Pause( UINT8 BankID )
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	UINT8 DSP_BankID = 0;
    UINT8 isDecTransFromFFState = 0;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_Pause:: Bank ID %d\n", BankID );

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Set_Pause::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	// Check FIFO state to avoid decoder transition hang up
	AUD_Check_FIFOState();

    // Ramp down    
    AUDIF_RampDown_PCM_Out();
    
    if (TASK_FF==DSP_GetState(DSP_BankID))
    {
        isDecTransFromFFState = 1;
    }    

	if (DSP_ERROR_NONE != DSP_SetCodecPause(DSP_BankID))
	{
		eStatus = AUDIF_FAILURE;
	}

#ifdef USE_RISC_PARSER_PARAM
	switch( DSP_BankID )
	{
		case CODEC_BANK_DEC1:
			DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, 0 );
			break;
		case CODEC_BANK_DEC2:
			AUDDRV_PRINT("AUDIF_Set_Pause::fix me\n");
			//DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, 0 );
			break;
		default:
			diag_printf("[Err]AUDIF_Set_Pause:: Error Codec ID %d for FF_RATIO", DSP_BankID );
			eStatus = AUDIF_ERR_Codec_BankID;
			AUDIO_SEMAPHORE_GIVE();
			return eStatus;
	}
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
#endif

	//GL_TaskSleep(90);
    msleep(90);
    if (isDecTransFromFFState)
    {
        // to avoid fifo hangup
        AUDDRV_PRINT("AUDIF_Set_Pause::trans state from FF, reset auxmode %d\n", auxmode_backup);
    DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, auxmode_backup);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    }
    
	AUDIO_SEMAPHORE_GIVE();
	return eStatus;
}



INT32 AUDIF_Set_Play( UINT8 BankID )
{

	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	UINT8 DSP_BankID = 0;
    UINT8 isDecTransFromFFState = 0;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_Play:: Bank ID %d\n", BankID );

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Set_Play::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	// Enable FIFO
	if( aud_param.FIFO_Enable_Flag == Disable )
	{
		AUDDRV_PRINT("AUDIF_Set_Play call FIFO enable\n");
		DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xffff,0x0);
		aud_param.FIFO_Enable_Flag = Enable;
	}

    if (TASK_FF==DSP_GetState(DSP_BankID))
    {
        isDecTransFromFFState = 1;
    }

	if (DSP_ERROR_NONE != DSP_SetCodecPlay(DSP_BankID))
	{
		eStatus = AUDIF_FAILURE;
	}

	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x00_RAMP_UP,0,0);
    AUDIF_RampUp_PCM_Out();

    if (isDecTransFromFFState)
    {
        // to avoid fifo hangup
        AUDDRV_PRINT("AUDIF_Set_Stop::trans state from FF, reset auxmode %d\n", auxmode_backup);
	DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, auxmode_backup);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);    
    }
	AUDIO_SEMAPHORE_GIVE();
	return eStatus;
}


INT32 AUDIF_Set_Ready( UINT8 BankID )
{

	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	UINT8 DSP_BankID = 0;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_Ready:: Bank ID %d\n", BankID );

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Set_Ready::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	if (DSP_ERROR_NONE != DSP_SetCodecReady(DSP_BankID))
	{
		eStatus = AUDIF_FAILURE;
	}

	AUDIO_SEMAPHORE_GIVE();
	return eStatus;
}
INT32 __AUDIF_CleanPcmFifo(void)
{
    // check if need to clean PCM buffer? (FIFO A0~A2)
    
    BYTE *address = (BYTE*)DSP_GetInfo(INFO_AUD_FIFO_BASE);

    INT32 fifo_a0_len = HWREG_R(aud_a0_length);
    INT32 fifo_a1_len = HWREG_R(aud_a1_length);
    INT32 fifo_a2_len = HWREG_R(aud_a2_length);    

    if (AUDIF_MAIN_PATH_MIXER == DSP_GetRiscParam1(RISC_MAIN_MAINOUTPUT))
    {
        AUDDRV_PRINT("__AUDIF_CleanPcmFifo::clean A0 FIFO addr 0x%08x len 0x%04x\n", (UINT32)address, fifo_a0_len);
        memset(address, 0, fifo_a0_len);
    }

    address += fifo_a0_len;
    
    if (AUDIF_AUX1_PATH_MIXER == DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT, 7, 0))
    {
        AUDDRV_PRINT("__AUDIF_CleanPcmFifo::clean A1 FIFO addr 0x%08x len 0x%04x\n", (UINT32)address, fifo_a1_len);        
        memset(address, 0, fifo_a1_len);
    }    

    address += fifo_a1_len;
    
    if (AUDIF_AUX2_PATH_MIXER == DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT,15, 8))
    {
        AUDDRV_PRINT("__AUDIF_CleanPcmFifo::clean A2 FIFO addr 0x%08x len 0x%04x\n", (UINT32)address, fifo_a2_len);        
        memset(address, 0, fifo_a2_len);
    }
    
    return AUDIF_SUCCESS;
}

INT32 AUDIF_Set_Stop( UINT8 BankID )
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;

	UINT8 DSP_BankID = 1;
    UINT8 isDecTransFromFFState = 0;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_Stop:: Bank ID %d\n", BankID );

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Set_Stop::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	// Check FIFO state to avoid decoder transition hang up
	AUD_Check_FIFOState();

	// Reset Bitstream Buffer
	// bitstream buffer will be flush when decoder state PLAY->STOP transition
	//DSP_FlushBsBuf(BSBUF_ID_DEC1);

    AUDIF_RampDown_PCM_Out();

    if (TASK_FF==DSP_GetState(DSP_BankID))
    {
        isDecTransFromFFState = 1;
    }

	if (DSP_ERROR_NONE != DSP_SetCodecStop(DSP_BankID))
	{
		eStatus = AUDIF_FAILURE;
	}

#ifdef USE_RISC_PARSER_PARAM

	switch( DSP_BankID )
	{
		case CODEC_BANK_DEC1:
	DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, 0 );
			break;
		case CODEC_BANK_DEC2:
			AUDDRV_PRINT("AUDIF_Set_Stop::fix me\n");
			//DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, 0 );
			break;
		default:
			diag_printf("[Err]AUDIF_Set_Stop:: Error Codec ID %d for FF_RATIO", DSP_BankID );
			eStatus = AUDIF_ERR_Codec_BankID;
			AUDIO_SEMAPHORE_GIVE();
			return eStatus;
	}
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
#endif
#if 0  
	//AUDIF_WaitRampDown();
	//GL_TaskSleep(90);
    msleep(90);
#else
	//GL_TaskSleep(40);
    msleep(90);
    __AUDIF_CleanPcmFifo();
#endif

    if (isDecTransFromFFState)
    {
        // to avoid fifo hangup
        AUDDRV_PRINT("AUDIF_Set_Stop::trans state from FF, reset auxmode %d\n", auxmode_backup);
    DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, auxmode_backup);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    }
    
	AUDIO_SEMAPHORE_GIVE();

	// Note: main decoder(01/29) remove FIFO reset cmd to audio hw
	// Note: main decoder(4/2) add FIFO reset cmd to audio-hw
	// AUDIF_Set_FIFOReset();
	return eStatus;
}

INT32 AUDIF_Set_Stop_NoWait( UINT8 BankID )
{    
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;

	UINT8 DSP_BankID = 1;
    UINT8 isDecTransFromFFState = 0;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_Stop_NoWait:: Bank ID %d\n", BankID );

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Set_Stop_NoWait::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	// Check FIFO state to avoid decoder transition hang up
	AUD_Check_FIFOState();

    AUDIF_RampDown_PCM_Out();

    if (TASK_FF==DSP_GetState(DSP_BankID))
    {
        isDecTransFromFFState = 1;
    }    

	if (DSP_ERROR_NONE != DSP_SetCodecStop(DSP_BankID))
	{
		eStatus = AUDIF_FAILURE;
	}

#ifdef USE_RISC_PARSER_PARAM

	switch( DSP_BankID )
	{
		case CODEC_BANK_DEC1:
	        DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, 0 );
			break;
		case CODEC_BANK_DEC2:
			AUDDRV_PRINT("AUDIF_Set_Stop_NoWait::fix me\n");
			//DSP_SetRiscParam0(RISC_DEC1_FF_RATIO, 0 );
			break;
		default:
			diag_printf("[Err]AUDIF_Set_Stop_NoWait:: Error Codec ID %d for FF_RATIO", DSP_BankID );
			eStatus = AUDIF_ERR_Codec_BankID;
			AUDIO_SEMAPHORE_GIVE();
			return eStatus;
	}
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
#endif

    if (isDecTransFromFFState)
    {
        // to avoid fifo hangup
        AUDDRV_PRINT("AUDIF_Set_Stop_NoWait::trans state from FF, reset auxmode %d\n", auxmode_backup);
        DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, auxmode_backup);
        DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    }   
	AUDIO_SEMAPHORE_GIVE();	
	return eStatus;
}


INT32 AUDIF_Set_FIFOEnable( void )
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
#if 0 //ndef AUD_SUPPORT_FIFORESET
	AUDDRV_PRINT("NOT execute AUDIF_Set_FIFOEnable\n");
	return eStatus;
#endif

	AUDIO_SEMAPHORE_TAKE();

	if( DSP_ERROR_NONE != DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xffff,0x0) )
	{
		eStatus = AUDIF_FAILURE;
	}

	AUDDRV_PRINT("AUDIF_Set_FIFOEnable\n");

	aud_param.FIFO_Enable_Flag = Enable;

	AUDIO_SEMAPHORE_GIVE();

	return eStatus;
}

INT32 AUDIF_Set_FIFOPAUSE( void )
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
#ifndef AUD_SUPPORT_FIFORESET
	AUDDRV_PRINT("NOT execute AUDIF_Set_FIFOPAUSE\n");
	return eStatus;
#endif

	AUDIO_SEMAPHORE_TAKE();
    AUDIF_RampDown_PCM_Out();
	AUDIF_WaitRampDown();

	if( DSP_ERROR_NONE != DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x0,0x0) )
	{
		eStatus = AUDIF_FAILURE;
	}

	aud_param.FIFO_Enable_Flag = Disable;

	AUDDRV_PRINT("AUDIF_Set_FIFOPAUSE\n");

	AUDIO_SEMAPHORE_GIVE();

	return eStatus;

}

INT32 AUDIF_Set_FIFOReset( void )
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
#ifndef AUD_SUPPORT_FIFORESET
	AUDDRV_PRINT("NOT execute AUDIF_Set_FIFOReset\n");
	return eStatus;
#endif

	AUDIO_SEMAPHORE_TAKE();

	DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x01_FIFO_RESET,0x0,0x0);

	AUDDRV_PRINT("AUDIF_Set_FIFOReset\n");

	//UINT64 timer_rtc = GL_GetRtc64();
	struct timeval timer_rtc, timer_stamp;
	do_gettimeofday(&timer_rtc);

	while(1)
	{
		if(HWREG_R(aud_a0_cnt) == 0 && HWREG_R(aud_a5_cnt) == 0 && HWREG_R(aud_a6_cnt) == 0)
			break;

		do_gettimeofday(&timer_stamp);
		if( (timer_stamp.tv_usec - timer_rtc.tv_usec) >= 300000 ) // 300ms
		{
			diag_printf("\n== AUDIF_Set_FIFOReset TimeOut==\n");
			eStatus = AUDIF_TIMEOUT;
			break;
		}
	}

	AUDIO_SEMAPHORE_GIVE();

	return eStatus;
}

INT32 AUD_Check_FIFOState( void )
{
/*********************************************************************
*	Decoder state transition should NOT be in the FIFO pause state,
*   hence, we would reset FIFO, otherwise, decoder may hang up.
*********************************************************************/
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
#ifndef AUD_SUPPORT_FIFORESET
	AUDDRV_PRINT("NOT execute AUD_Check_FIFOState\n");
	return eStatus;
#endif

	UINT32 Master_Gain = 0;

	if( aud_param.FIFO_Enable_Flag == Disable )
	{
		// mute
		DSP_SendCmd(0x08, 0x00, 0x0000, 0);
		// FIFO Reset
		DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x01_FIFO_RESET,0x0,0x0);
		aud_param.FIFO_Enable_Flag = Enable;
		// demute
		if( aud_param.mute != 1) // it means system is Not in mute state
		{
			if( aud_param.volume == 0 )
				Master_Gain = 0;
			else if( aud_param.volume == (VOLUME_SCALE-1) )
				Master_Gain = ((VolTab_qae377[aud_param.volume]<<24));
			else
       			Master_Gain = ((VolTab_qae377[aud_param.volume]<<24)| 0x00FFFF00);
			DSP_SendCmd(0x08, 0x00, 0x0000, Master_Gain);
		}
	}
	return eStatus;
}

INT32 AUDF_IS_FIFO_PAUSE(void)
{
	UINT32 fifo_pause_flag = 0;
	if( aud_param.FIFO_Enable_Flag == Disable )
		fifo_pause_flag = 1;

	return fifo_pause_flag;
}

INT32 AUDIF_FIFO_Empty_Status( void )
{
/*********************************************************************
*	check FIFO cnt
*********************************************************************/
	AUDIF_FIFO_CNT_e eStatus;

	if( HWREG_R(aud_a0_cnt)==0 && HWREG_R(aud_a5_cnt)==0 )
		eStatus = AUDIF_FIFOCNT_Empty;
	else
		eStatus = AUDIF_FIFOCNT_Not_Empty;

	return eStatus;
}

INT32 AUDIF_Get_FIFO_Depth( UINT8 FIFO_type )
{
	UINT16 depth_cfg=0;
	INT32 FIFO_depth=0;

	depth_cfg = DSP_GetRiscParam0(RISC_MAIN_FIFO_DEPTH_CFG);

	switch( depth_cfg )
	{
		case FIFO_DEPTH_COMPACT:
			if( FIFO_type == FIFO_PCM || FIFO_type == FIFO_SPDIF )
				FIFO_depth = 2048;
			else if( FIFO_type == FIFO_HDMI )
				FIFO_depth = 65536;
			else
				FIFO_depth = 0;
			break;

		case FIFO_DEPTH_NORMAL:
			if( FIFO_type == FIFO_PCM || FIFO_type == FIFO_SPDIF )
				FIFO_depth = 4096;
			else if( FIFO_type == FIFO_HDMI )
				FIFO_depth = 65536;
			else
				FIFO_depth = 0;
			break;

		case FIFO_DEPTH_LARGE:
			if( FIFO_type == FIFO_PCM || FIFO_type == FIFO_SPDIF )
				FIFO_depth = 8192;
			else if( FIFO_type == FIFO_HDMI )
				FIFO_depth = 65536;
			else
				FIFO_depth = 0;
			break;

		case FIFO_DEPTH_EXTREME:
			if( FIFO_type == FIFO_PCM || FIFO_type == FIFO_SPDIF )
				FIFO_depth = 16384;
			else if( FIFO_type == FIFO_HDMI )
				FIFO_depth = 65536;
			else
				FIFO_depth = 0;
			break;

		default:
			FIFO_depth = 0;
			break;
	}

	return FIFO_depth;

}

INT32 AUDIF_Set_FIFOSampleCnt( UINT16 mode )
{
/************************************************************
*	[mode] ref AUDIF_FIFOSmpCnt_e
*	[Note] You should init FIFO SampleCnt before enable it!
************************************************************/

	INT32 rVal = AUDIF_SUCCESS;
	
	if( mode == FIFOSmpCnt_Init ||  mode == FIFOSmpCnt_Disable )
	{	// initialize
		HWREG_W(aud_apt_reset,1);
		HWREG_W(aud_apt_parameter,0x01000010 ); // SampleNum=256, Delta=1.0
	}
	else if( mode == FIFOSmpCnt_Enable )
	{	// enable
		HWREG_W(aud_apt_reset,0);
	}
	else if( mode == FIFOSmpCnt_Reset )
	{	// reset
		HWREG_W(aud_apt_reset,1);
		HWREG_W(aud_apt_parameter,0x01000010 ); // SampleNum=256, Delta=1.0
		HWREG_W(aud_apt_reset,0);
	}
	else
	{
		rVal = AUDIF_ERR_FIFOSpmCntMode;
		diag_printf("AUDIF_Set_FIFOSampleCnt Error mode\n");
	}

	return rVal;
	
}

UINT32 AUDIF_Get_FIFOSampleCnt( void )
{
	UINT32 val=0;
	val = HWREG_R(aud_apt_data);

	//diag_printf("AUDIF_Get_FIFOSampleCnt: 0x%x\n", val);
	
	return val;
}

INT32 AUDIF_Set_BsBufEnd(UINT8 BankID)
{
	AUDIO_SEMAPHORE_TAKE();
	UINT8 DSP_BSBufID = 0;
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Set_BsBufEnd::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

	if (DSP_ERROR_NONE != DSP_SendCmd(RISC_CMD0x02_BSBUF, CODE0x01_BSBUF_ENDING, DSP_BSBufID, 0x0) )
	{
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_FAILURE;
	}

	AUDIO_SEMAPHORE_GIVE();

	return eStatus;
}

INT32 AUDIF_Wait_EndingDone(void)
{
	/*****************************************
	* DSP_MAIN_ENDING_CTRL_DONE:
	*		bit0:dec1 done
	*		bit1:dec2 done
	******************************************/

	AUDIO_SEMAPHORE_TAKE();

	AUDIF_Status_e eStatus = AUDIF_SUCCESS;

	UINT16 EndCtrlDone = 0x0;
	//UINT64 timer_rtc = GL_GetRtc64();

	struct timeval timer_rtc, timer_stamp;
	do_gettimeofday(&timer_rtc);

	while(1)
	{
		EndCtrlDone = DSP_GetDspParam0(DSP_MAIN_ENDING_CTRL_DONE);

		DSP_Waitloop(1);

		do_gettimeofday(&timer_stamp);
		if( (timer_stamp.tv_usec - timer_rtc.tv_usec) >= 3000 ) // 3ms
		//if( (GL_GetRtc64() - timer_rtc)/1000 >= 3 )
		{
			diag_printf("== AUDIF_Wait_EndingDone Timeout ==\n");
			eStatus = AUDIF_TIMEOUT;
			break;
		}
	}

	AUDIO_SEMAPHORE_GIVE();

	return eStatus;
}

INT32 AUDIF_Is_EndingDone(void)
{
	/*****************************************
	* DSP_MAIN_ENDING_CTRL_DONE:
	*		bit0:dec1 done
	*		bit1:dec2 done
	******************************************/
	AUDIF_BS_Ending_e eStatus;
	UINT16 EndCtrlDone = 0x0;

	EndCtrlDone = DSP_GetDspParam0(DSP_MAIN_ENDING_CTRL_DONE);

	if(EndCtrlDone&0x1)
		eStatus = AUDIF_BS_Ending_done;
	else
		eStatus = AUDIF_BS_Ending_Not_yet;

	return eStatus;
}

#if 0

INT32 AUDIF_Set_Continue(void)
{
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;

	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_Continue::Set Continue (play) command\n");

	if (DSP_ERROR_NONE != DSP_SetCodecPlay(CODEC_BANK_DEC1))
	{
		eStatus = AUDIF_FAILURE;
	}

	AUDIO_SEMAPHORE_GIVE();
	return eStatus;

}
#endif


//*******************************************************
//   DSP message protocol
//*******************************************************
UINT32 AUDIF_Get_DspMessage(UINT32* dspMsg)
{
	AUDIO_SEMAPHORE_TAKE();

	UINT32 dFlag = DSP_GetMessage(dspMsg);

	AUDIO_SEMAPHORE_GIVE();

	return dFlag;
}


UINT32 AUDIF_Get_DBGMsg(void)
{
	AUDIO_SEMAPHORE_TAKE();

	UINT32 dMessage = DSP_GetNewMessage();

	AUDIO_SEMAPHORE_GIVE();

	return dMessage;
}

UINT32 AUDIF_Get_DBGFlag(void)
{
	AUDIO_SEMAPHORE_TAKE();

	UINT32 dFlag = DSP_GetMessageFlag();

	AUDIO_SEMAPHORE_GIVE();

	return dFlag;

}

void AUDIF_Set_DBGFlag(void)
{
	AUDIO_SEMAPHORE_TAKE();

	DSP_SetMessageFlag();

	AUDIO_SEMAPHORE_GIVE();

}


//*******************************************************
//   API for DSP Runtime Control Register
//*******************************************************
UINT8 AUDIF_Check_BSBufBankID( UINT8 BankID )
{
	UINT8 DSP_BSBufID = 0;

	switch( BankID )
	{
		case AUDIF_BANK_DEC1:
			DSP_BSBufID = BSBUF_ID_DEC1;
			break;
		case AUDIF_BANK_DEC2:
			DSP_BSBufID = BSBUF_ID_DEC2;
			break;
		case AUDIF_BANK_ENC:
			DSP_BSBufID = BSBUF_ID_ENC;
			break;
		default:
			diag_printf("[Err]AUDIF_Check_BSBufBankID:: Error Codec ID %d\n", BankID);
			return AUDIF_ERR_Codec_BankID;
	}

	return DSP_BSBufID;

}


UINT32 AUDIF_Get_DumpPtr( UINT8 BankID )
{
	//AUDIO_SEMAPHORE_TAKE();
	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_DumpPtr::e:AUDIF_ERR_Codec_BankID\n");
		//AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(DSP_BSBufID);


	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Get_DumpPtr::e:pBsBufInfo NULL\n");
		//AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

//	AUDDRV_PRINT("AUDIF_Get_DumpPtr::0x%x(%d)\n",pBsBufInfo->dumpPtr,pBsBufInfo->dumpPtr);

	//AUDIO_SEMAPHORE_GIVE();
	return (pBsBufInfo->dumpPtr);
}

void AUDIF_Set_DumpPtr( UINT8 BankID ,UINT32 val)
{
//	AUDIO_SEMAPHORE_TAKE();

	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Set_DumpPtr::e:AUDIF_ERR_Codec_BankID\n");
//		AUDIO_SEMAPHORE_GIVE();
		return;
	}

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(DSP_BSBufID);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Set_DumpPtr::e:pBsBufInfo NULL\n");
//		AUDIO_SEMAPHORE_GIVE();
		return;
	}

//	AUDDRV_PRINT("AUDIF_Set_DumpPtr::dump:0x%x, val:0x%x\n",pBsBufInfo->dumpPtr,val);
	DSP_SetDumpPtr(DSP_BSBufID, val);

//	AUDIO_SEMAPHORE_GIVE();
	return;

}


UINT32 AUDIF_Get_BarrierPtr(UINT8 BankID)
{
//	AUDIO_SEMAPHORE_TAKE();

	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_BarrierPtr::e:AUDIF_ERR_Codec_BankID\n");
//		AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(DSP_BSBufID);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Get_BarrierPtr::e:pBsBufInfo NULL\n");
//		AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

//	AUDDRV_PRINT("AUDIF_Get_BarrierPtr::0x%x(%d)\n",pBsBufInfo->barrPtr,pBsBufInfo->barrPtr);

//	AUDIO_SEMAPHORE_GIVE();
	return (pBsBufInfo->barrPtr);
}

void AUDIF_Set_BarrierPtr(UINT8 BankID,UINT32 val)
{
//	AUDIO_SEMAPHORE_TAKE();

	UINT8 DSP_BSBufID = 0;

	DSP_BSBufID = AUDIF_Check_BSBufBankID(BankID);

	if( AUDIF_ERR_Codec_BankID == DSP_BSBufID )
	{
		ERROR_PRINT("AUDIF_Get_BarrierPtr::e:AUDIF_ERR_Codec_BankID\n");
//		AUDIO_SEMAPHORE_GIVE();
		return;
	}

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(DSP_BSBufID);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Set_BarrierPtr::e:pBsBufInfo NULL\n");
//		AUDIO_SEMAPHORE_GIVE();
		return;
	}

//	AUDDRV_PRINT("AUDIF_Set_DumpPtr::dump:0x%x, val:0x%x\n",pBsBufInfo->dumpPtr,val);
	DSP_SetBarrierPtr(DSP_BSBufID, val);

//	AUDIO_SEMAPHORE_GIVE();
	return;
}

UINT32 AUDIF_Get_DSPState( UINT8 BankID )
{
	AUDIO_SEMAPHORE_TAKE();

	UINT8 DSP_BankID = 0;
	UINT8 dsp_state;

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Get_DSPState::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_Codec_BankID;
	}

	dsp_state = DSP_GetState(DSP_BankID);

	AUDIO_SEMAPHORE_GIVE();

	return ((UINT32)dsp_state);

}


UINT32 AUDIF_Get_AC3_CheckBonding(void)
{
	return 0;
}


#if 0
UINT32 AUDIF_Get_Karaoke_Encode_BarrierPtr(void)
{
	return 0;
}

UINT32 AUDIF_Get_Karaoke_Encode_DumpPtr(void)
{
	return 0;
}

UINT32 AUDIF_Get_Karaoke_Encode_State(void)
{
	return 0;
}

UINT32 AUDIF_Get_MICState(void)
{
	// note: not work in 820xR platform
	return 1;
}

void AUDIF_Set_Karaoke_Encode_BarrierPtr(UINT32 val)
{
	UNUSED_ARGUMENT(val);
}

void AUDIF_Set_Karaoke_Encode_DumpPtr(UINT32 val)
{
	UNUSED_ARGUMENT(val);
}

#endif

//*******************************************************
//	Get DSP parameters
//*******************************************************
UINT16 AUDIF_Get_DspParam(UINT16 nOffset)
{
	AUDIO_SEMAPHORE_TAKE();

	UINT16 wDspParam = DSP_GetDspParam0(nOffset);

	AUDIO_SEMAPHORE_GIVE();
	return wDspParam;
}


UINT32 AUDIF_Set_RiscParam(UINT16 nOffset,UINT16 nValue)
{
	AUDIO_SEMAPHORE_TAKE();

	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	if ( DSP_ERROR_NONE != DSP_SetRiscParam0(nOffset,nValue))
	{
		eStatus = AUDIF_FAILURE;
	}

	AUDIO_SEMAPHORE_GIVE();
	return eStatus;
}


UINT32 AUDIF_Get_DspInfo(UINT32 Info)
{
	AUDIO_SEMAPHORE_TAKE();

	UINT32 dInfo = DSP_GetInfo(Info);

	AUDIO_SEMAPHORE_GIVE();

	return dInfo;
}


//*******************************************************
//	Audio I/O Control APIs
//*******************************************************

INT32 AUDIF_Set_MICStatus(UINT16 mode)
{
	// mode: ref mic_switch_e in auddrv.h
	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("AUDIF_Set_MICStatus: 0x%x\n", mode);
	DSP_SendCmd(0x09, 0x00, mode, 0x0 );
	DSP_SetRiscParam0(RISC_MAIN_MIC_SWITCH, mode );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	aud_param.mic_mode = mode;

	AUDIO_SEMAPHORE_GIVE();
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Set_MICVol(UINT8 tblval)
{
	AUDIO_SEMAPHORE_TAKE();

	DSP_SendCmd(0x09, 0x01, MICVolTab_qae377[tblval], 0x0 );
	DSP_SetRiscParam0(RISC_MAIN_MIC0_GAIN, MICVolTab_qae377[tblval] );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	aud_param.mic0_gain = tblval;

	AUDIO_SEMAPHORE_GIVE();
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Set_MICVol2(UINT8 tblval)
{
	AUDIO_SEMAPHORE_TAKE();

	DSP_SendCmd(0x09, 0x02, MICVolTab_qae377[tblval], 0x0 );
	DSP_SetRiscParam0(RISC_MAIN_MIC1_GAIN, MICVolTab_qae377[tblval] );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	aud_param.mic1_gain = tblval;

	AUDIO_SEMAPHORE_GIVE();
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Set_OutputMode(UINT8 outmode, UINT16 outparam)
{
	UINT16 wOutputParam = 0;
	UINT8  bOutCmd = 0x7;
	UINT8  bOutCmd2 = 0;

	AUDIO_SEMAPHORE_TAKE();
	
	bOutCmd2 = (outmode & 0xf0);
	if(bOutCmd2 > OUTPUT_2CH_MODE)
	    bOutCmd = 0xA;
	
	switch(bOutCmd2)
	{
	    // 0x0700yyyy
		case OUTPUT_DOWNMIX_MODE:
			bOutCmd2 = 0;
			wOutputParam = (UINT16)(outparam & 0xff);
			DSP_SetRiscParam0(RISC_MAIN_DOWNMIX_MODE, (outparam & 0xf) );
			DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);			
			DSP_SendCmd(bOutCmd, bOutCmd2, wOutputParam, 0x00000000 );
			break;
			
	    // 0x0701yyyy
		case OUTPUT_2CH_MODE:
			bOutCmd2 = 1;
			wOutputParam = (UINT16)(outparam & 0xff);
			aud_param.out_mode.karaoke_mode = (BYTE)(outparam & 0xf);
			DSP_SetRiscParam0(RISC_MAIN_2CH_MODE, (outparam & 0xf) );
			DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
			break;
			
	    //the following settings are AC3 Output Control	(0x0Ayy)
		case OUT_RFModeK3:
			aud_param.out_mode.ac3_op_mode = (BYTE)(outparam & 0xff);
			if( aud_param.coding_mode_now == AUDIF_CODING_MODE_AC3 )
			{
				bOutCmd2 = 0;
				wOutputParam = (UINT16)(outparam & 0xff);
				DSP_SetRiscParam0(RISC_DEC1_DDP_OutMode, (outparam & 0xff) );
				DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
				DSP_SendCmd(bOutCmd, bOutCmd2, wOutputParam, 0x00000000 );
			}
			break;
			
		case OUT_AC3KaraMode: //case OUT_NoVocal:
			aud_param.out_mode.ac3_karaoke_mode = (BYTE)(outparam & 0xff);
			if( aud_param.coding_mode_now == AUDIF_CODING_MODE_AC3 )
			{
				bOutCmd2 = 1;
				wOutputParam = (UINT16)(outparam & 0xff);
				DSP_SetRiscParam0(RISC_DEC1_DDP_Karaoke_Mode,(outparam & 0xff));
				DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
				DSP_SendCmd(bOutCmd, bOutCmd2, wOutputParam, 0x00000000 );
			}
			break;
			
		case OUT_CustomModeB_DigitalDialnormK1:	// AC3 DRC
			aud_param.out_mode.ac3_dyncmprs_mode = (UINT32)(outparam & 0xffff);
			if( aud_param.coding_mode_now == AUDIF_CODING_MODE_AC3 )
			{
				bOutCmd2 = 3;
				wOutputParam = (UINT16)(outparam & 0xffff);
				DSP_SendCmd(bOutCmd, bOutCmd2, wOutputParam, 0x00000000 );
			}
			break;
			
		case ENABLE_MELODY_CHANNEL:
			if( aud_param.coding_mode_now == AUDIF_CODING_MODE_AC3 )
			{
				bOutCmd2 = 2;
				wOutputParam = (UINT16)(outparam & 0xff);
				DSP_SendCmd(bOutCmd, bOutCmd2, wOutputParam, 0x00000000 );
			}
			break;
			
		case OUT_Dual_Mono_STEREO:
			if( aud_param.coding_mode_now == AUDIF_CODING_MODE_AC3 )
			{
				wOutputParam = (UINT16)(outparam & 0xff);
				DSP_SendCmd(bOutCmd, bOutCmd2, wOutputParam, 0x00000000 );
				bOutCmd2 = 4;
			}
			break;
	}
	
	AUDIO_SEMAPHORE_GIVE();
	return 1;
}


UINT8 AUDIF_Get_SpkStatus_Map( void )
{
/********************************************************
* A mapping of each speaker switch mode (on/off)
* the bit value set to 1 if speaker is on;
* Otherwise, set to zero if speaker is off.
*		MSB-----------------LSB
*		Rb Lb LFE C Rs Ls Rf Lf
*********************************************************/

	UINT8 spk_cfg = 0x0;
	// LF
	if( aud_param.speaker_info.LF != BM_SPK_OFF )
		spk_cfg |= 0x1;
	else
		spk_cfg &= 0xFE;
	// RF
	if( aud_param.speaker_info.RF != BM_SPK_OFF )
		spk_cfg |= 0x2;
	else
		spk_cfg &= 0xFD;
	// LS
	if( aud_param.speaker_info.LS != BM_SPK_OFF )
		spk_cfg |= 0x4;
	else
		spk_cfg &= 0xFB;
	// RS
	if( aud_param.speaker_info.RS != BM_SPK_OFF )
		spk_cfg |= 0x8;
	else
		spk_cfg &= 0xF7;
	// Center
	if( aud_param.speaker_info.center!= BM_SPK_OFF )
		spk_cfg |= 0x10;
	else
		spk_cfg &= 0xEF;
	// LFE
	if( aud_param.speaker_info.subwoofer!= BM_SPK_OFF )
		spk_cfg |= 0x20;
	else
		spk_cfg &= 0xDF;
	// LB
	if( aud_param.speaker_info.LB != BM_SPK_OFF )
		spk_cfg |= 0x40;
	else
		spk_cfg &= 0xBF;
	//RB
	if( aud_param.speaker_info.RB != BM_SPK_OFF )
		spk_cfg |= 0x80;
	else
		spk_cfg &= 0x7F;

	//AUDDRV_PRINT("AUDIF_Get_SpkStatus_Map: 0x%x\n",spk_cfg);

	return spk_cfg;
}

UINT32 AUDIF_Set_SpkOutMode( void )
{
/********************************************************
*	output channel configuration
*	[IN] spk_cfg: each bit represents to a speaker
*		 the bit value set to 1 if speaker is on;
*		 Otherwise, set to zero if speaker is off.
*		MSB-----------------LSB
*		Rb Lb LFE C Rs Ls Rf Lf
*********************************************************/

	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	UINT16 channel_mode;
	UINT16 dwmix_val;
	UINT8 spk_cfg;

	spk_cfg = AUDIF_Get_SpkStatus_Map();

	// mark back speaker
	spk_cfg &= 0x3f;

	dwmix_val = DSP_GetRiscParam0(RISC_MAIN_DOWNMIX_MODE);

	switch(gWavFmt.wfx.wFormatTag)
	{
	case WAVE_FORMAT_DOLBY_DIGITAL_PLUS:
	case WAVE_FORMAT_DOLBY_AC3:
		if( dwmix_val == dwmix_L0R0)
		{
			aud_param.output_channel_cfg = outmode_spk20;
			aud_param.out_mode.downmix_mode = 2;
			aud_param.DDP_LFF_Channel_Mode = 0; // off
		}
		else if( dwmix_val == dwmix_LtRt)
		{
			aud_param.output_channel_cfg = outmode_spk20;
			aud_param.out_mode.downmix_mode = 1;
			aud_param.DDP_LFF_Channel_Mode = 0; // off
		}
		else
		{
			aud_param.DDP_LFF_Channel_Mode = 0; // off
			switch( spk_cfg )
			{
				case 0x03:
				case 0x23:
					channel_mode = outmode_spk20;
					break;
				case 0x13:
				case 0x33:
					channel_mode = outmode_spk30;
					break;
				case 0x0f:
				case 0x2f:
					channel_mode = outmode_spk22;
					break;
				case 0x1f:
					channel_mode = outmode_spk32;
					break;
				default:
					channel_mode = 0xFFFF;
					aud_param.DDP_LFF_Channel_Mode = 1; // on
					break;
			}

			aud_param.output_channel_cfg = channel_mode;
			aud_param.out_mode.downmix_mode = dwmix_off;

			DSP_SetRiscParam0(RISC_DEC1_DDP_OutputChnMode,channel_mode);
		}

		//DSP_SetRiscParam0(RISC_MAIN_DOWNMIX_MODE,aud_param.out_mode.downmix_mode);

		//AUDDRV_PRINT("AUDIF_Set_SpkOutMode::output_channel_cfg: 0x%x\n",aud_param.output_channel_cfg);
		//AUDDRV_PRINT("AUDIF_Set_SpkOutMode::downmix_mode: 0x%x\n",aud_param.out_mode.downmix_mode);
		break;

	case WAVE_FORMAT_DOLBY_DTS:
	case WAVE_FORMAT_DTSHD:
	case WAVE_FORMAT_DTSHD_MASTER_AUDIO:

		if( dwmix_val == dwmix_LtRt )
		{
			channel_mode = 5;
		}
		else if( dwmix_val == dwmix_L0R0 )
		{
			channel_mode = 0x3;
		}
		else // downmix off
		{
			switch( spk_cfg )
			{
				case 0x10:
					channel_mode = 1;
					break;
				case 0x30:
					channel_mode = 2;
					break;
				case 0x03:
					channel_mode = 3;
					break;
				case 0x23:
					channel_mode = 4;
					break;
				case 0x13:
					channel_mode = 6;
					break;
				case 0x33:
					channel_mode = 7;
					break;
				case 0x0f:
					channel_mode = 12;
					break;
				case 0x2f:
					channel_mode = 13;
					break;
				case 0x3f:
					channel_mode = 14;
					break;
				default:
					channel_mode = 0x0;
					break;
			}
		}

		aud_param.output_channel_cfg = channel_mode;
		DSP_SetRiscParam0(RISC_DEC1_DTSHD_OutputChnMode,channel_mode);
		break;

	default:
		if( dwmix_val != dwmix_LtRt && dwmix_val != dwmix_L0R0 )
		{
			DSP_SetRiscParam0(RISC_MAIN_SPK_OUTPUT, spk_cfg );
		}
		else
		{
			DSP_SetRiscParam0(RISC_MAIN_SPK_OUTPUT, 0x303 );
		}

		break;
	}

	UINT16 SWAPP_mode = DSP_GetRiscParam0(RISC_SWAPP_ID);
	UINT16 SWAPP_PARAM0_val = DSP_GetRiscParam0(RISC_SWAPP_PARAM0);

	switch( SWAPP_mode )
	{
		case en_prologic2:
			if( (SWAPP_PARAM0_val>>15) == 1 ) //enable
			{
				switch( spk_cfg )
				{
					case 0x13:
						channel_mode = outmode_spk30;
						break;
					case 0x0f:
						channel_mode = outmode_spk22;
						break;
					case 0x1f:
						channel_mode = outmode_spk32;
						break;
					case 0xdf:
						channel_mode = outmode_spk34;
						break;
					default:
						channel_mode = 0x0;
						break;
				}
				DSP_SetRiscParam0(RISC_SWAPP_PARAM0, (SWAPP_PARAM0_val&0xfff0)|channel_mode );
			}
			break;

		case en_neo6:
			if( (SWAPP_PARAM0_val>>15) == 1 ) //enable
			{
				switch( spk_cfg )
				{
					case 0x13:
						channel_mode = 0x8;
						break;
					case 0x0f:
						channel_mode = 0x10;
						break;
					case 0x1f:
						channel_mode = 0x20;
						break;
					case 0xdf:
						channel_mode = 0x80;
						break;
					default:
						channel_mode = 0x0;
						break;
				}
				DSP_SetRiscParam0(RISC_SWAPP_PARAM0, (SWAPP_PARAM0_val&0xff00)|channel_mode );
			}
			break;
		case en_gm7:
			if( (SWAPP_PARAM0_val>>15) == 1 ) //enable
			{
				switch( spk_cfg )
				{
					case 0x13:
						channel_mode = 0x8;
						break;
					case 0x0f:
						channel_mode = 0x10;
						break;
					case 0x1f:
						channel_mode = 0x20;
						break;
					case 0x5f: //use LB instead of Cs
						channel_mode = 0x40;
						break;
					case 0xdf:
						channel_mode = 0x80;
						break;
					default:
						channel_mode = 0x20;
						break;
				}
				DSP_SetRiscParam0(RISC_SWAPP_PARAM0, (SWAPP_PARAM0_val&0xff00)|channel_mode );
			}
			break;

		default:
			break;

	}

// mantis 99274
//	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return eStatus;
}

INT32 AUDIF_TestTone_mode( UINT8 mode )
{
	/* mode: 0x00: sine gen
	         0x02: PINK NOISE GEN
	         0x01: WHITE NOISE GEN
	*/

	aud_param.testtone = mode;

	DSP_SetRiscParam0(RISC_MAIN_SPK_OUTPUT, 0x3ff );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	AUDIF_Set_CodingMode( AUDIF_CODING_MODE_EMU );
	AUDIF_Set_Play(AUDIF_BANK_DEC1);

	return AUDIF_SUCCESS;
}

INT32 AUDIF_TestTone_SpkType( UINT8 spktype )
{
/*spktype:
  LF_SPKGAIN  = 0x3, RF_SPKGAIN  = 0x4,  LS_SPKGAIN  = 0x5, RS_SPKGAIN  = 0x6,
  C_PKGAIN    = 0x7, SW_SPKGAIN  = 0x8,  LB_SPKGAIN  = 0x9, RB_SPKGAIN  = 0xA
*/

	UINT8 MAX_TARGET_GAIN = 0x80;
	//UINT8 gainLm = 0;
	//UINT8 gainRm = 0;
	UINT8 gainLf = 0;
	UINT8 gainRf = 0;
	UINT8 gainLs = 0;
	UINT8 gainRs = 0;
	UINT8 gainC  = 0;
	UINT8 gainSw = 0;
	UINT8 gainLb = 0;
	UINT8 gainRb = 0;


	switch(spktype)
	{
	/*
	case LM_SPKGAIN:
		gainLm = MAX_TARGET_GAIN;
		break;
	case RM_SPKGAIN:
		gainRm = MAX_TARGET_GAIN;
		break;
	*/
	case LF_SPKGAIN:
		gainLf = MAX_TARGET_GAIN;
		break;
	case RF_SPKGAIN:
		gainRf = MAX_TARGET_GAIN;
		break;
	case LS_SPKGAIN:
		gainLs = MAX_TARGET_GAIN;
		break;
	case RS_SPKGAIN:
		gainRs = MAX_TARGET_GAIN;
		break;
	case C_PKGAIN:
		gainC = MAX_TARGET_GAIN;
		break;
	case SW_SPKGAIN:
		gainSw = MAX_TARGET_GAIN;
		break;
	case LB_SPKGAIN:
		gainLb = MAX_TARGET_GAIN;
		break;
	case RB_SPKGAIN:
		gainRb = MAX_TARGET_GAIN;
		break;
	}

	//AUDIF_Set_SPKGain(LM_SPKGAIN,gainLm);
	//AUDIF_Set_SPKGain(RM_SPKGAIN,gainRm);
	AUDIF_Set_SPKGain(LF_SPKGAIN,gainLf);
	AUDIF_Set_SPKGain(RF_SPKGAIN,gainRf);
	AUDIF_Set_SPKGain(LS_SPKGAIN,gainLs);
	AUDIF_Set_SPKGain(RS_SPKGAIN,gainRs);
	AUDIF_Set_SPKGain(C_PKGAIN,gainC);
	AUDIF_Set_SPKGain(SW_SPKGAIN,gainSw);
	AUDIF_Set_SPKGain(LB_SPKGAIN,gainLb);
	AUDIF_Set_SPKGain(RB_SPKGAIN,gainRb);

	DSP_SetRiscParam0(RISC_MAIN_A0_GAIN, gainLf<<8|gainRf);
	DSP_SetRiscParam0(RISC_MAIN_A1_GAIN, gainLs<<8|gainRs);
	DSP_SetRiscParam0(RISC_MAIN_A2_GAIN, gainSw<<8|gainC);
	DSP_SetRiscParam0(RISC_MAIN_A3_GAIN, gainLb<<8|gainRb);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return AUDIF_SUCCESS;
}

#define IEC0_MODE 			0x00	//SPDIF
#define IEC1_MODE 			0x01	//HDMI
INT32 AUDIF_Set_SPDIF(UINT8 mode)
{
#ifdef DEBUG_DDCO
    mode = SPDIF_REENC;
#endif
	int iRes=1;

    if(aud_param.spdif_mode == mode)
    {
        return iRes;     
    }    

	AUDIO_SEMAPHORE_TAKE();
	AUDDRV_PRINT("AUDIF_Set_SPDIF::aud_param.spdif_mode:0x%x, mode:%x\n",aud_param.spdif_mode,mode);
	DSP_SendCmd(RISC_CMD0x06_IEC_MODE,IEC0_MODE,mode,0x0);
	DSP_SetRiscParam0(RISC_MAIN_SPDIF_MODE, (UINT16)mode );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	aud_param.spdif_mode = mode;
	AUDIO_SEMAPHORE_GIVE();

	//AUDIF_Set_SPDIF_Capability( gWavFmt.wfx.wFormatTag , DigitalOutputDefaultTab );

    if (SPDIF_IECRX == mode)
    {
        // enable SPDIF RX0
        HWREG_W(sft_cfg_2, HWREG_R(sft_cfg_2) | (0x1<<2));        

        // SPDIF RX pinmux selection : 0=ext. SPDIF
		HWREG_W(g304_reserved[12], HWREG_R(g304_reserved[12])&(~(0x1)));

        // DSP will queue input data until buffer full
        DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x00_SPDIF_INPUT,0x0,0x0);
    }
	
    return iRes;

}


INT32 AUDIF_Set_DigitalOutTable( UINT16 LookupTab[][3], UINT16 ROW, UINT16 COL )
{
	AUDIO_SEMAPHORE_TAKE();
	
	UINT16 i=0,j=0;
	UINT16 IsFind_fmtTag = 0;

	if( ROW > DigiOutTab_ROW )
	{
		diag_printf("[Err] the number of ROW(%d) cannot bigger than %d\n",ROW,DigiOutTab_ROW);
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_INVALID_PARAM;
	}
	if( COL > DigiOutTab_COL )
	{
		diag_printf("[Err] the number of COL(%d) cannot bigger than %d\n",COL,DigiOutTab_COL);
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_INVALID_PARAM;
	}

	for( i=0; i<ROW ; i++ )
	{
		IsFind_fmtTag = 0;
		for( j=0; j<ROW ; j++ )
		{
			if( DigitalOutputDefaultTab[j][0] == LookupTab[i][0] )
			{
				DigitalOutputDefaultTab[j][1] = LookupTab[i][1];
				DigitalOutputDefaultTab[j][2] = LookupTab[i][2];
				IsFind_fmtTag = 1;
				break;
			}
		}
		if( IsFind_fmtTag == 0 )
			diag_printf("[Warning] AUDIF_Set_DigitalOutTable cannot find the mapping wavefmt tag: 0x%x\n",LookupTab[i][0] );
	}

	AUDDRV_PRINT("AUDIF_Set_DigitalOutTable SUCCESS\n");

	AUDIO_SEMAPHORE_GIVE();
	
	return AUDIF_SUCCESS;
}


INT32 AUDIF_Set_SPDIF_Capability( UINT16 AudioFormatTag, UINT16 LookupTab[][3] )
{		
	AUDIO_SEMAPHORE_TAKE();

  	UINT16 index=0, IsFind_fmtTag=0, table_SPDIF_mode=0;

	for( index=0; index<DigiOutTab_ROW ; index++)
	{
		if( LookupTab[index][0] == AudioFormatTag )
		{
			table_SPDIF_mode = LookupTab[index][1];
			IsFind_fmtTag = 1;
			break;
		}
	}

	if( IsFind_fmtTag!=1 )
	{		
		AUDDRV_PRINT("AUDIF_Set_SPDIF_Capability::Without any changes for spdif mode");
		AUDDRV_PRINT("because do not find the mapping wavefmt tag\n");
		
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_SUCCESS;
	}
	else
	{
		// SPDIF Setting
		if( aud_param.spdif_mode == SPDIF_PCM || aud_param.spdif_mode == SPDIF_OFF )
		{
			// no change
			AUDDRV_PRINT("AUDIF_Set_SPDIF_Capability::Spdif mode: 0x%x\n", aud_param.spdif_mode);
		}
		else
		{			
			// Re-Enc mode is limited under the group of 48KHz
			if( table_SPDIF_mode == SPDIF_REENC )
			{
				UINT16 t_smprate = aud_param.smprate_info.smp_rate_now;
				if( (t_smprate != FS_12K) && (t_smprate != FS_24K) && 
					(t_smprate != FS_48K) && (t_smprate != FS_96K ) && 
					(t_smprate != FS_192K ) )
				{
					table_SPDIF_mode = SPDIF_PCM;
				}
			}
			
			DSP_SendCmd(RISC_CMD0x06_IEC_MODE,IEC0_MODE,table_SPDIF_mode,0x0);
			DSP_SetRiscParam0(RISC_MAIN_SPDIF_MODE, table_SPDIF_mode );
			DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
			AUDDRV_PRINT("AUDIF_Set_SPDIF_Capability::Set SPDIF mode: 0x%x\n",table_SPDIF_mode);
		}
	}

	AUDIO_SEMAPHORE_GIVE();
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Set_HDMI(UINT8 mode)
{
	AUDIO_SEMAPHORE_TAKE();

	int iRes=1;

	AUDDRV_PRINT("AUDIF_Set_HDMI::aud_param.hdmi_mode:0x%x, mode:%x\n",aud_param.hdmi_mode,mode);

	//if (mode != aud_param.hdmi_mode)	//cmhuang: 2010.08.17 use aud_param.hdmi_mode to record setup menu setting, remove aud_param.hdmi_mode related code here
	//{
		DSP_SendCmd(RISC_CMD0x06_IEC_MODE,IEC1_MODE,mode,0x0);  // always succeed
		DSP_SetRiscParam0(RISC_MAIN_HDMI_MODE, (UINT16)mode );	// update RISC param
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
		//aud_param.hdmi_mode = mode;
	//}

	AUDIO_SEMAPHORE_GIVE();
    return iRes;
}


#ifdef AUD_SUPPORT_HDMI
// ccyang: 2009.08.04
int AUDIF_SetAudParam_to_HDMI( UINT8 hdmi_output_mode )
{
#if (PRODUCT_CAT != DTV)
	//cmhuang: 2009.12.14 HDMI setting flow

	int ret_val=0;
	int ret_check=0;
	UINT8 RetSupported=0;
	int HDMI_Audio_Type=0;
	int Audio_Raw_Format=0;
	int HDMI_Set_Type=0;
	HdmiAudLimitByVideo_st HDMIAudioLimit;
	ShortAudioForm_t HDMIAudDetail;



	if (hdmi_output_mode == HDMI_REENC)
	{										//if (hdmi_output_mode == HDMI_REENC), need to check re-encode type, not original codec type
		HDMI_Audio_Type = HDMI_ACF_DTS;		// assume re-encode type is DTS
		Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
	}
	else
	{
		switch ( gWavFmt.wfx.wFormatTag )
		{
			case WAVE_FORMAT_DOLBY_AC3:
				HDMI_Audio_Type = HDMI_ACF_AC3;
				Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
				break;
			case WAVE_FORMAT_DOLBY_DTS:
				HDMI_Audio_Type = HDMI_ACF_DTS;
				Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
				break;
			case WAVE_FORMAT_DOLBY_DIGITAL_PLUS:
				if (hdmi_output_mode == HDMI_RAWHD)
	{
					HDMI_Audio_Type = HDMI_ACF_DDPlus;
					Audio_Raw_Format = HDMI_RAWMODE_HD;
				}
				else
				{
					HDMI_Audio_Type = HDMI_ACF_AC3;
					Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
				}
				break;
			case WAVE_FORMAT_TRUEHD:
				if (hdmi_output_mode == HDMI_RAWHD)
				{
					HDMI_Audio_Type = HDMI_ACF_MAT;
					Audio_Raw_Format = HDMI_RAWMODE_HDHBR;
				}
				else
				{
					HDMI_Audio_Type = HDMI_ACF_AC3;
		Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
	}
				break;
			case WAVE_FORMAT_DTSHD:
				if (hdmi_output_mode == HDMI_RAWHD)
				{
					HDMI_Audio_Type = HDMI_ACF_DtsHd;
					Audio_Raw_Format = HDMI_RAWMODE_HD;
				}
	else
	{
					HDMI_Audio_Type = HDMI_ACF_DTS;
					Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
				}
				break;
			case WAVE_FORMAT_DTSHD_MASTER_AUDIO:
				if (hdmi_output_mode == HDMI_RAWHD)
				{
		HDMI_Audio_Type = HDMI_ACF_ExDtsHdMaster;
		Audio_Raw_Format = HDMI_RAWMODE_HDHBR;
	}
				else
				{
					HDMI_Audio_Type = HDMI_ACF_DTS;
					Audio_Raw_Format = HDMI_RAWMODE_LEGACY;
				}
				break;
			default:
				HDMI_Audio_Type = HDMI_ACF_LPCM;
				Audio_Raw_Format = HDMI_RAWMODE_NONE;
				break;
		}
	}


	// if device EDID ok RetSupported = 1; else RetSupported = 0;
	ret_check = HDMI_IF_AudioCodeChecked( HDMI_Audio_Type , &RetSupported );
	if( ret_check != HDMI_OPERATION_SUCCESS )
	{
		RetSupported = 0;
	}
	if (hdmi_output_mode == HDMI_PCM8CH)
	{
		ret_check = HDMI_IF_AudioCodeChecked( HDMI_ACF_LPCM , &RetSupported );
		if( ret_check != HDMI_OPERATION_SUCCESS )
		{
			RetSupported = 0;
		}

		// check supported PCM channel num
		ret_check = HDMI_IF_AudioCodeDetail(HDMI_ACF_LPCM, &HDMIAudDetail);
		if( ret_check != HDMI_OPERATION_SUCCESS )
	{
		RetSupported = 0;
	}
		if( HDMIAudDetail.bMaxChannels != HDMI_AUD_8Ch)
		{
			RetSupported = 0;
		}
	}


	if(RetSupported)
	{
		// if video resolution ok RetSupported = 1; else RetSupported = 0;

		ret_check = HDMI_IF_Get_AudioLimitation(&HDMIAudioLimit);
		if( ret_check != HDMI_OPERATION_SUCCESS )
		{
			RetSupported = 0;
		}

		if (hdmi_output_mode == HDMI_PCM8CH)
		{
			switch(aud_param.smprate_info.smp_rate_now)
			{
				case FS_88K:
					if (HDMIAudioLimit.eMaxFsIn8Ch < HDMI_AUD_FS_88K )
					{
						RetSupported = 0;
					}
					break;
				case FS_96K:
					if (HDMIAudioLimit.eMaxFsIn8Ch < HDMI_AUD_FS_96K )
					{
						RetSupported = 0;
					}
					break;
				case FS_176K:
					if (HDMIAudioLimit.eMaxFsIn8Ch < HDMI_AUD_FS_176K )
					{
						RetSupported = 0;
					}
					break;
				case FS_192K:
					if (HDMIAudioLimit.eMaxFsIn8Ch < HDMI_AUD_FS_192K )
					{
						RetSupported = 0;
					}
					break;
				default:
					break;
			}
		}
		else if (Audio_Raw_Format == HDMI_RAWMODE_HDHBR)
		{
			if (HDMIAudioLimit.eMaxFrIn2Ch != HDMI_AUD_FR_768)
			{
				RetSupported = 0;
			}
		}
	}


	// set HDMI sampling rate here, may need to change below
	// use audio hw sample rate
	AUDIF_Set_HDMI_SmpRate(aud_param.smprate_info.hw_smprate, HDMI_AUD_IN_SPDIF);
	AUDIF_Set_HDMI_SmpRate(aud_param.smprate_info.hw_smprate, HDMI_AUD_IN_I2S);

	// it should use audio hw sample rate
	switch(aud_param.smprate_info.hw_smprate)
	{
		case FS_32K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_32K;
			break;
		case FS_44K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_44K;
			break;
		case FS_48K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_48K;
			break;
		case FS_88K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_88K;
			break;
		case FS_96K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_96K;
			break;
		case FS_176K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_176K;
			break;
		case FS_192K:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_192K;
			break;
		default:
			hdmi_audcfg.eSampleRate = HDMI_AUD_FS_48K;
			break;
	}


	// HDMI default setting, may need to change below
	hdmi_audcfg.eInput = HDMI_AUD_IN_SPDIF; //HDMI Audio Input by SPDIF
	hdmi_audcfg.eOutput = HDMI_AUD_OUT_PCM2; //HDMI Audio Output 2 channels PCM
	hdmi_audcfg.eChannels = HDMI_AUD_2Ch;


	// Assume system behavior is: If the selected Audio format can not be output, always output only PCM-2ch
	if (hdmi_output_mode == HDMI_RAWHD)
	{
		if (Audio_Raw_Format == HDMI_RAWMODE_HDHBR)
		{
			//if EDID and Video Resolution Supported
			if (RetSupported)
			{
				// set the HDMI audio data format
				HDMI_Set_Type = HDMI_RAWHD;

				// set HDMI sampling rate to 192K
				AUDIF_Set_HDMI_SmpRate(FS_192K, HDMI_AUD_IN_I2S);

				// HDMIIF set the HDMI audio related setting
				hdmi_audcfg.eSampleRate = HDMI_AUD_FS_192K; //Sammple rate 192k
				hdmi_audcfg.eInput = HDMI_AUD_IN_I2S; //HDMI Audio Input by I2S
				hdmi_audcfg.eOutput = HDMI_AUD_OUT_HBR; //HDMI Audio Output high bit rate(HBR)
				hdmi_audcfg.eChannels = HDMI_AUD_8Ch;
			}
			else
			{
				HDMI_Set_Type = HDMI_PCM;
			}
		}
		else if (Audio_Raw_Format == HDMI_RAWMODE_HD)
		{
			//if EDID and Video Resolution Supported
			if (RetSupported)
			{
				HDMI_Set_Type = HDMI_RAWHD;

				// set HDMI sampling rate to 192K
				AUDIF_Set_HDMI_SmpRate(FS_192K, HDMI_AUD_IN_SPDIF);

				// HDMIIF set the HDMI audio related setting
				hdmi_audcfg.eSampleRate = HDMI_AUD_FS_192K; //Sammple rate 192k
			}
			else
			{
				HDMI_Set_Type = HDMI_PCM;
			}
		}
		else if (Audio_Raw_Format == HDMI_RAWMODE_LEGACY)
		{
			//if EDID and Video Resolution Supported
			if (RetSupported)
			{
				HDMI_Set_Type = HDMI_RAW;
			}
			else
			{
				HDMI_Set_Type = HDMI_PCM;
			}
		}
		else
		{
				HDMI_Set_Type = HDMI_PCM;
		}
	}
	else if (hdmi_output_mode == HDMI_REENC)
	{
			//if EDID and Video Resolution Supported
			if (RetSupported)
			{
				HDMI_Set_Type = HDMI_REENC;
			}
			else
			{
				HDMI_Set_Type = HDMI_PCM;
			}
	}
	else if (hdmi_output_mode == HDMI_RAW)
	{
		if (Audio_Raw_Format == HDMI_RAWMODE_NONE)
		{
				HDMI_Set_Type = HDMI_PCM;
		}
		else
		{
			//if EDID and Video Resolution Supported
			if (RetSupported)
			{
				HDMI_Set_Type = HDMI_RAW;
			}
			else
			{
				HDMI_Set_Type = HDMI_PCM;
			}
		}
	}
	else if (hdmi_output_mode == HDMI_PCM8CH)
	{
			//if EDID and Video Resolution Supported
			if (RetSupported)
			{
				HDMI_Set_Type = HDMI_PCM8CH;

				// HDMIIF set the HDMI audio related setting
				hdmi_audcfg.eInput = HDMI_AUD_IN_I2S; //HDMI Audio Input by I2S
				hdmi_audcfg.eOutput = HDMI_AUD_OUT_PCMx; //HDMI Audio Output n channels PCM(2<n<=8)
				hdmi_audcfg.eChannels = HDMI_AUD_8Ch;
				//hdmi_audcfg.eChannels = HDMI_AUD_6Ch;
			}
			else
			{
				HDMI_Set_Type = HDMI_PCM;
			}
	}
	else if (hdmi_output_mode == HDMI_PCM)
	{
				HDMI_Set_Type = HDMI_PCM;
	}
	else if (hdmi_output_mode == HDMI_OFF)
	{
				HDMI_Set_Type = HDMI_OFF;
	}

	AUDIF_Set_HDMI( HDMI_Set_Type );

	//ret_val = HDMI_IF_Set_Audio( hdmi_audcfg );
	UINT8 bTimeCnt = 0;
	while(HDMI_IF_Set_Audio( hdmi_audcfg ) != HDMI_OPERATION_SUCCESS)
	{
		AUDDRV_PRINT("[AUD]HDMI_INIT (ongoing)!\n");
		if(bTimeCnt > 9)
		{
			AUDDRV_PRINT("[AUD]HDMI_INIT (TimeOut)!\n");
			break;
		}
		//GL_TaskSleep(10);
		msleep(10);
		//cyg_thread_delay(10);
		bTimeCnt++;
	}

	return ret_val;
#else
	return 0;
#endif
}
#endif

#ifdef SUPPORT_QAE377_SPDIF_IN
void AUDIF_Get_SpdifIn_Info(spdifin_info* info)
{
    memcpy(info, &spdifin_setting, sizeof(spdifin_info));
	return;
}

INT32 AUDIF_Get_SpdifIn_Type(void)
{
	return spdifin_setting.bs_fmt;
}

INT32 AUDIF_Get_SpdifIn_Fs(void)
{
	return spdifin_setting.fs;
}

void AUDIF_UnInit_SpdifIn(void)
{
	AUDIF_Set_Stop(AUDIF_BANK_DEC1);
    AUDIO_SEMAPHORE_TAKE();
    UINT16 runMode = DSP_GetRiscParam0(RISC_MAIN_MODE);

    AUDDRV_PRINT("AUDIF_UnInit_SpdifIn::runMode now = 0x%08x\n", runMode);

    runMode &= ~(1<<MODE_SPDIFIN_ENABLE_BIT);	
	DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
    
    // fix bug of no sound as switching to AV from HDMI mode
	// because spdifin has called ramp down(stop)
    AUDIF_RampUp_PCM_Out();
	
	// 20110519 check if mclk from hdmi rx
	dac_clk_source	= 0;		
    AUDIO_SEMAPHORE_GIVE();
}

void AUDIF_Init_SpdifIn(UINT16 mode)
{
    AUDIO_SEMAPHORE_TAKE();
	
    int i;

    // initial set fifo disable here!
    // to avoid hang up issue
	aud_param.FIFO_Enable_Flag = Disable;

    //---------------------------------------------------------------------------
    // init global variables
    //---------------------------------------------------------------------------
    spdifin_setting.bs_fmt = SPDIFIN_AC3_MODE;
    spdifin_setting.fs     = FS_48K;
    spdifin_setting.cgms_type = unlimited_copy;
    spdifin_setting.category = 1;    

    spdifin_hw_info.cnt = 0;
    spdifin_hw_info.idx = 0;
    spdifin_hw_info.period = 0;

    for (i=0; i<SPDIFIN_HW_PERIOD_BUF_LEN; i++)
        spdifin_hw_info.buf[i] = 0;


    wfmt_spdifin.wfx.wFormatTag = WAVE_FORMAT_BigEndian_LPCM;
    wfmt_spdifin.wfx.nChannels = 2;
    wfmt_spdifin.wfx.wBitsPerSample = 16;
    wfmt_spdifin.wfx.nSamplesPerSec = 44100;


    // init clk related global variables
	spdifin_hw_clk.sysclk = GL_GetSysBusFreq();	
    
    spdifin_hw_clk.fs_192k.mid = spdifin_hw_clk.sysclk/(16*192000);        
    
    spdifin_hw_clk.fs_176k.mid = spdifin_hw_clk.sysclk/(16*176400);    
    spdifin_hw_clk.fs_96k.mid = spdifin_hw_clk.sysclk/(16*96000);        
    spdifin_hw_clk.fs_88k.mid = spdifin_hw_clk.sysclk/(16*88200);
    spdifin_hw_clk.fs_48k.mid = spdifin_hw_clk.sysclk/(16*48000);
    spdifin_hw_clk.fs_44k.mid = spdifin_hw_clk.sysclk/(16*44100);    
    spdifin_hw_clk.fs_32k.mid = spdifin_hw_clk.sysclk/(16*32000);

    spdifin_hw_clk.fs_192k.hi = (spdifin_hw_clk.fs_192k.mid + spdifin_hw_clk.fs_176k.mid)/2;
    spdifin_hw_clk.fs_176k.hi = (spdifin_hw_clk.fs_176k.mid + spdifin_hw_clk.fs_96k.mid)/2;
    spdifin_hw_clk.fs_96k.hi = (spdifin_hw_clk.fs_96k.mid + spdifin_hw_clk.fs_88k.mid)/2;    
    spdifin_hw_clk.fs_88k.hi = (spdifin_hw_clk.fs_88k.mid + spdifin_hw_clk.fs_48k.mid)/2;    
    spdifin_hw_clk.fs_48k.hi = (spdifin_hw_clk.fs_48k.mid + spdifin_hw_clk.fs_44k.mid)/2;            
    spdifin_hw_clk.fs_44k.hi = (spdifin_hw_clk.fs_44k.mid + spdifin_hw_clk.fs_32k.mid)/2;        
	spdifin_hw_clk.fs_32k.hi = spdifin_hw_clk.fs_32k.mid*(1+(1/100));
    spdifin_hw_clk.fs_192k.lo = spdifin_hw_clk.fs_192k.mid*(1-(1/100));    
    spdifin_hw_clk.fs_176k.lo = spdifin_hw_clk.fs_192k.hi+1;    
    spdifin_hw_clk.fs_96k.lo = spdifin_hw_clk.fs_176k.hi+1;
    spdifin_hw_clk.fs_88k.lo = spdifin_hw_clk.fs_96k.hi+1;
    spdifin_hw_clk.fs_48k.lo = spdifin_hw_clk.fs_88k.hi+1;    
    spdifin_hw_clk.fs_44k.lo = spdifin_hw_clk.fs_48k.hi+1;    
    spdifin_hw_clk.fs_32k.lo = spdifin_hw_clk.fs_44k.hi+1;    

    //AUDIO_SEMAPHORE_TAKE();
    AUDDRV_PRINT("[AUDDRV] sysclk = %d\n", spdifin_hw_clk.sysclk);
    AUDDRV_PRINT("[AUDDRV]32k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_32k.lo, spdifin_hw_clk.fs_32k.mid, spdifin_hw_clk.fs_32k.hi);
    AUDDRV_PRINT("[AUDDRV]44k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_44k.lo, spdifin_hw_clk.fs_44k.mid, spdifin_hw_clk.fs_44k.hi);
    AUDDRV_PRINT("[AUDDRV]48k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_48k.lo, spdifin_hw_clk.fs_48k.mid, spdifin_hw_clk.fs_48k.hi);    
    AUDDRV_PRINT("[AUDDRV]88k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_88k.lo, spdifin_hw_clk.fs_88k.mid, spdifin_hw_clk.fs_88k.hi);    
    AUDDRV_PRINT("[AUDDRV]96k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_96k.lo, spdifin_hw_clk.fs_96k.mid, spdifin_hw_clk.fs_96k.hi);    
    AUDDRV_PRINT("[AUDDRV]176k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_176k.lo, spdifin_hw_clk.fs_176k.mid, spdifin_hw_clk.fs_176k.hi);    
    AUDDRV_PRINT("[AUDDRV]192k::lo %d mid %d hi %d\n", spdifin_hw_clk.fs_192k.lo, spdifin_hw_clk.fs_192k.mid, spdifin_hw_clk.fs_192k.hi);
    //AUDIO_SEMAPHORE_GIVE();
    //---------------------------------------------------------------------------
    // dylan : in product version, we should remove the following config form auddrv
	// pin-mux setting:
	//  sft_cfg_2     : bit2=RX0, bit3=RX1, bit4=RX2
	//  aud_misc_ctrl : set bit2 to use RX2 (instead of record fifo)
    //---------------------------------------------------------------------------
#ifdef QAE468_CERTI_BOARD
	HWREG_W(sft_cfg_2, HWREG_R(sft_cfg_2) | ((0x1<<2) | (0x1<<3) | (0x1<<4)));
	HWREG_W(aud_misc_ctrl, HWREG_R(aud_misc_ctrl) | (0x1 << 2)); //select SPDIF RX2 intead of Record RX
#else
    HWREG_W(sft_cfg_2, HWREG_R(sft_cfg_2) | (0x1<<2));
#endif

    //---------------------------------------------------------------------------
	// pin-mux setting
    //---------------------------------------------------------------------------    
    // 1.SPDIF RX0 mux setting : g304.12 bit0 0:ext spdif in  1:hdmi spdif in
    // 2.SPDIF RX1 mux setting : g304.12 bit1 0:ext spdif in  1:ssd sif spdif in
    if (mode == 1)
    {
        HWREG_W(g304_reserved[12], HWREG_R(g304_reserved[12])|0x1);
       dac_clk_source	= 1;
    }
    else
    {
        HWREG_W(g304_reserved[12], HWREG_R(g304_reserved[12])&(~(0x1)));
       dac_clk_source	= 0;		
    }

    AUDDRV_PRINT("AUDIF_Init_SpdifIn::pinmux setting\n", HWREG_R(sft_cfg_2));

    //---------------------------------------------------------------------------
	// main program related settings
    //---------------------------------------------------------------------------
    UINT16 runMode = DSP_GetRiscParam0(RISC_MAIN_MODE);
    UINT16 fifo    = DSP_GetRiscParam0(RISC_MAIN_FIFO_CFG_L);
	
    UINT8 isDec1Enable=0, isSpdifinEnable=0, isFifoEnable=0;

    if(runMode&(1<<MODE_DEC1_ENABLE_BIT))
        isDec1Enable = 1;

    if(runMode&(1<<MODE_SPDIFIN_ENABLE_BIT))
        isSpdifinEnable = 1;

    if(fifo & (0x1<<13))
        isFifoEnable = 1;

    if(isDec1Enable && isSpdifinEnable && isFifoEnable)
    {
    	AUDIO_SEMAPHORE_GIVE();

        AUDIF_Set_Stop(CODEC_BANK_DEC1);

        //AUDIO_SEMAPHORE_TAKE();
        //DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x00_SPDIF_INPUT,0x0,0x0);
        //AUDIO_SEMAPHORE_GIVE();
        return;
    }
    else if(isDec1Enable && isFifoEnable)
    {
        runMode |= (1<<MODE_SPDIFIN_ENABLE_BIT);
        DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);

        AUDIO_SEMAPHORE_GIVE();
        AUDIF_Set_Stop(CODEC_BANK_DEC1);


        AUDIO_SEMAPHORE_TAKE();	            
	    DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);
        DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
        DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x00_SPDIF_INPUT,0x0,0x0);
    
        // fix bug of no sound as switching to AV from HDMI mode
	    // because spdifin has called ramp down(stop)
        AUDIF_RampUp_PCM_Out();
        
        AUDIO_SEMAPHORE_GIVE();
        return;        
    }
    else
    {
        fifo |=  (0x1<<13);
        runMode |= ((1<<MODE_DEC1_ENABLE_BIT) | (1<<MODE_SPDIFIN_ENABLE_BIT));
	DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);
        

	DSP_SetRiscParam0(RISC_MAIN_FIFO_CFG_L, fifo);
	DSP_SetRiscParam0(RISC_MAIN_FIFO_DEPTH_CFG, FIFO_DEPTH_NORMAL);
	DSP_SetRiscParam0(RISC_MAIN_SPDIF_MODE, aud_param.spdif_mode); // jy.ciou

	// make mainprogram update configurations
	DSP_SetSystemIdle();
	DSP_SetSystemReady();
	DSP_SetSystemIdle();
	DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x7E8,0x0);        
	DSP_SetSystemRun();
  	DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x7FF,0x0);

	AUDIO_SEMAPHORE_GIVE();
	AUDIF_Set_SampleRate(FS_44K);
	
	// avoid to acess wrong romtable address
	AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
	
    // set default coding mode??
    AUDDRV_PRINT("AUDIF_Init_SpdifIn::set default coding mode 0x%02x\n", spdifin_setting.bs_fmt);
    AUDDRV_PRINT("AUDIF_Init_SpdifIn::set default sample rate 0x%02x\n", spdifin_setting.fs);
    AUDIF_Set_Stop(CODEC_BANK_DEC1);
    }


}

void AUDIF_Update_Spdifin_SampleRate(void)
{
    UINT32 regval, iec_period_8t, period = 0;
    UINT32 sum; 
    INT16  idx, cnt, k;
    INT8 dtscd_flag = 0;
    BYTE bs_fmt = (BYTE)(HWREG_R(dsp_port[21]) & 0x7);;

    //-----------------------------------------------------------------------------
    // Calculate sample rate by averaging iec0_period_8t (iec0_rx_debug_info[10:0])
    // iec0_period_8t = system_clk/(fs*128/8)
    // We could obtain fs if the range of iec0_period_8t was ensured.
    //-----------------------------------------------------------------------------

    // check if error occurred (bit17==1 or bit18==1)
    regval = HWREG_R(iec0_rx_debug_info);
    if ((regval>>17)&0x3)
        iec_period_8t = 0; // error occurred! set an invalid data
    else
        iec_period_8t = regval&0x7FF;
    
    // if bitstream format changed, reset the averaging buffer
	if(bs_fmt != spdifin_setting.bs_fmt)
	{
        AUDDRV_PRINT("AUDIF_Update_SpdifIn::bs_fmt:old %d  new %d\n", bs_fmt, spdifin_setting.bs_fmt);
        //spdifin_hw_info.cnt = 0;
        //spdifin_hw_info.idx = 0;
	}    
    
    idx = spdifin_hw_info.idx;
    cnt = spdifin_hw_info.cnt;

    // check if iec_period_8t is in the valid data range
    if (IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_32k.lo, spdifin_hw_clk.fs_32k.hi) ||
        IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_44k.lo, spdifin_hw_clk.fs_44k.hi) ||
        IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_48k.lo, spdifin_hw_clk.fs_48k.hi) ||
        IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_88k.lo, spdifin_hw_clk.fs_88k.hi) ||
        IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_96k.lo, spdifin_hw_clk.fs_96k.hi) ||
        IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_176k.lo, spdifin_hw_clk.fs_176k.hi) ||
        IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_192k.lo, spdifin_hw_clk.fs_192k.hi))
	{
        spdifin_hw_info.buf[idx] = iec_period_8t;
        idx++;
        cnt++;
        if (idx == SPDIFIN_HW_PERIOD_BUF_LEN)  idx = 0;
        if (cnt > SPDIFIN_HW_PERIOD_BUF_LEN)   cnt = SPDIFIN_HW_PERIOD_BUF_LEN;
    }

    spdifin_hw_info.idx = idx;
    spdifin_hw_info.cnt = cnt;

    // sum up the average buffer
	sum = 0;
	for(k=0; k<cnt; k++)
        sum += spdifin_hw_info.buf[k];

    // check frequency range
    if (((sum>(spdifin_hw_clk.fs_32k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_32k.hi*cnt))))
        period = IEC_RX_32K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_44k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_44k.hi*cnt))))
        period = IEC_RX_44K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_48k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_48k.hi*cnt))))
        period = IEC_RX_48K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_88k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_88k.hi*cnt))))
        period = IEC_RX_88K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_96k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_96k.hi*cnt))))
        period = IEC_RX_96K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_176k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_176k.hi*cnt))))
        period = IEC_RX_176K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_192k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_192k.hi*cnt))))
        period = IEC_RX_192K_PERIOD;
    
    if ((spdifin_hw_info.period != period) || (spdifin_setting.bs_fmt != bs_fmt))
    {
        switch(period)
        {
            case IEC_RX_32K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 32000;
                break;
            case IEC_RX_44K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 44100;
                break;
            case IEC_RX_48K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 48000;
                break;
            case IEC_RX_88K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 88200;
                break;
            case IEC_RX_96K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 96000;
		        break;
            case IEC_RX_176K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 176400;
                break;
            case IEC_RX_192K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 192000;
		        break;
	        default:
                AUDDRV_PRINT("AUDIF_Update_SpdifIn::un-known period %d\n",period);
                wfmt_spdifin.wfx.nSamplesPerSec = 48000;

	}

#ifdef DAC_OUTPUT_48K
        DSP_SetRiscParam1(RISC_MAIN_IEC_RX0_FS, (UINT16)SampleRate_Mapping(wfmt_spdifin.wfx.nSamplesPerSec));
        DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

        //if(wfmt_spdifin.wfx.nSamplesPerSec == 96000)
        //    wfmt_spdifin.wfx.nSamplesPerSec = 48000;
#endif        
    spdifin_setting.fs = aud_map_SmpRate_IDtoFS((UINT16)SampleRate_Mapping(wfmt_spdifin.wfx.nSamplesPerSec));
        spdifin_setting.bs_fmt = bs_fmt;

        spdifin_hw_info.period = period;        
	}

#ifdef DAC_OUTPUT_48K
     if ((spdifin_setting.bs_fmt == SPDIFIN_DTSCD16_MODE) || (spdifin_setting.bs_fmt == SPDIFIN_DTSCD14_MODE))
    {
        //diag_printf("[dylan] DTSCD set Fs = 44kHz?\n");
        dtscd_flag = 1;
    } 

    if (dtscd_flag == 1)
        {
        spdifin_setting.fs = FS_44K;
    }
    else
    {
        if (spdifin_setting.fs != FS_96K)
            spdifin_setting.fs = FS_48K;
    }
#endif        
    
    if(aud_param.smprate_info.smp_rate_now != spdifin_setting.fs)
    {
		// temprary solution!! need to fix
		// This is for avoiding SPDIF output fifo empty, because follow
		// bellow flow, could cause it happen
		if (dtscd_flag == 1)
		{
        AUDIF_Set_SampleRate(spdifin_setting.fs);
			return;
		}    	
        //
        // mute DAC PCM output and SPDIF before change sample rate
        //
        
        UINT8 spdif_mode = aud_param.spdif_mode;    // backup SPDIF mode
        
        AUDIF_Set_SPDIF(SPDIF_OFF);
        
        AUDIO_SEMAPHORE_TAKE();
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x03_A0_RAMP_DOWN,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x05_A1_RAMP_DOWN,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x07_A2_RAMP_DOWN,0,0);        
        AUDIO_SEMAPHORE_GIVE();                
        
        //GL_TaskSleep(40);
		msleep(40);

        AUDIF_Set_SampleRate(spdifin_setting.fs);

        //GL_TaskSleep(40);
		msleep(40);

        AUDIF_Set_SPDIF(spdif_mode);

        AUDIO_SEMAPHORE_TAKE();
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x02_A0_RAMP_UP,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x04_A1_RAMP_UP,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x06_A2_RAMP_UP,0,0);                        
        AUDIO_SEMAPHORE_GIVE();        
    }    
}

#ifdef ENA_9202_DEMDEC
// dylan : for project 9202, new SIF control flow
void AUDIF_Init_SifIn(void)
{
    //---------------------------------------------------------------------------
	//  source selection
	//  g307_reserved[4] bit0 : 0=SSD-DSP   1=CEVA
    //---------------------------------------------------------------------------
    //HWREG_W(g307_reserved[4], HWREG_R(g307_reserved[4])|0x1);

    //---------------------------------------------------------------------------
	// main program related settings
    //---------------------------------------------------------------------------
	UINT16 runMode = DSP_GetRiscParam0(RISC_MAIN_MODE);
    UINT16 fifo    = DSP_GetRiscParam0(RISC_MAIN_FIFO_CFG_L);
#if 0
    // don't re-init if enable bit and fifo had been set.
    if ((runMode & (1<<MODE_SIFIN_ENABLE_BIT)) && (fifo & (0<<12)))
    {
        AUDDRV_PRINT("AUDIF_Init_SifIn::w:SIF-in mode had been initialized already\n");
        return;
    }
#endif
    runMode |= (1<<MODE_SIFIN_ENABLE_BIT);	// enable sif input mode
    fifo |=  (0x1<<12);                     // enable i2s-in fifo

    AUDIF_Set_Stop(CODEC_BANK_DEC1);

	DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);
	DSP_SetRiscParam0(RISC_MAIN_FIFO_CFG_L, fifo);
    DSP_SetRiscParam0(RISC_MAIN_FIFO_DEPTH_CFG, FIFO_DEPTH_NORMAL);
    DSP_SetRiscParam0(RISC_MAIN_SPDIF_MODE, SPDIF_PCM);

    DSP_SetRiscParam1(RISC_MAIN_MAINOUTPUT, 1);

	// make mainprogram update configurations
	DSP_SetSystemIdle();
	DSP_SetSystemReady();
	DSP_SetSystemIdle();
    DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x7E8,0x0);
	DSP_SetSystemRun();

    //----------------------------------------------------------------
	// enable FIFO (temp. add here since download flow won't re-init hardware)
	DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x7FF,0x0);

    AUDIF_Set_SampleRate(FS_32K);
    HWREG_W(clk_sel_atv_dtv, (HWREG_R(clk_sel_atv_dtv)& (~0x30)));
  	HWREG_W(ext_adc_cfg, 0x4D); //SSD I2S format is 0x4D

}
#else
void AUDIF_Init_SifIn(void)
{
    // initial set fifo disable here!
    // to avoid hang up issue
	//aud_param.FIFO_Enable_Flag = Disable;

    //---------------------------------------------------------------------------
	//  enable IEC RX1:
	//  sft_cfg_2     : bit2=RX0, bit3=RX1, bit4=RX2
    //---------------------------------------------------------------------------
    HWREG_W(sft_cfg_2, HWREG_R(sft_cfg_2) | (0x1<<3));
    AUDDRV_PRINT("AUDIF_Init_SifIn::pinmux setting\n", HWREG_R(sft_cfg_2));

    //---------------------------------------------------------------------------
	//  source selection
	//  1.SPDIF RX0 mux setting : g304.12 bit0 0:ext spdif in  1:hdmi spdif in
    //  2.SPDIF RX1 mux setting : g304.12 bit1 0:ext spdif in  1:ssd sif spdif in
    //---------------------------------------------------------------------------
    HWREG_W(g304_reserved[12], HWREG_R(g304_reserved[12])|(0x1<<1));

    //---------------------------------------------------------------------------
	// main program related settings
    //---------------------------------------------------------------------------
	UINT16 runMode = DSP_GetRiscParam0(RISC_MAIN_MODE);
    UINT16 fifo    = DSP_GetRiscParam0(RISC_MAIN_FIFO_CFG_L);

    UINT8 isSifinEnable=0, isFifoEnable=0;

    if(runMode&(1<<MODE_SIFIN_ENABLE_BIT))
        isSifinEnable = 1;

    if(fifo & (0x1<<14))
        isFifoEnable = 1;

    if(isSifinEnable && isFifoEnable)
    {
        AUDDRV_PRINT("AUDIF_Init_SifIn::already init Sifin\n");
        //DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xffff,0x0);
			aud_param.FIFO_Enable_Flag = Enable;
        DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xffff,0x0);
        return;
    }
    else if(isFifoEnable)
    {
        AUDDRV_PRINT("AUDIF_Init_SifIn::init run mode only\n");        
        runMode |= (1<<MODE_SIFIN_ENABLE_BIT);	// enable sif input mode
        DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);
        DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
        DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xffff,0x0);    
        AUDIF_RampUp_PCM_Out();
        return;
    }
    else
    {
        AUDDRV_PRINT("AUDIF_Init_SifIn::init main program\n");                
    runMode |= (1<<MODE_SIFIN_ENABLE_BIT);	// enable sif input mode
    fifo |=  (0x1<<14);                     // enable spdif input fifo : A13=RX0, A14=RX1, A12=RX2

	DSP_SetRiscParam0(RISC_MAIN_MODE, runMode);
	DSP_SetRiscParam0(RISC_MAIN_FIFO_CFG_L, fifo);
    DSP_SetRiscParam0(RISC_MAIN_FIFO_DEPTH_CFG, FIFO_DEPTH_NORMAL);
        //DSP_SetRiscParam0(RISC_MAIN_SPDIF_MODE, SPDIF_PCM);

	// make mainprogram update configurations
	DSP_SetSystemIdle();
	DSP_SetSystemReady();
	DSP_SetSystemIdle();
    DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0x7E8,0x0);
	DSP_SetSystemRun();

		DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x00_FIFO_ENABLE,0xffff,0x0);
		aud_param.FIFO_Enable_Flag = Enable;
    }


}
#endif
void AUDIF_Update_SpdifIn(void)
{
	UINT16 runMode = DSP_GetRiscParam0(RISC_MAIN_MODE);

	if ((runMode & (1<<MODE_SPDIFIN_ENABLE_BIT))==0)
	{
		return;
	}
	
    BYTE   bs_fmt;
    UINT32 regval, iec_period_8t, period = 0;
    UINT32 sum = 0; 
    INT16  cnt = 0;
    int i;

    //-----------------------------------------------------------------------------
    // the bitstream format is reported by DSP via dsp_port[21]
    // 0:pcm    1:ac3   2:dts   3:dtscd     7:no signal
    //-----------------------------------------------------------------------------
    bs_fmt = (BYTE)(HWREG_R(dsp_port[21]) & 0x7);

    //-----------------------------------------------------------------------------
    // Calculate sample rate by averaging iec0_period_8t (G60.17 iec0_rx_debug_info[10:0])
    // iec0_period_8t = system_clk/(fs*128/8)
    // We could obtain fs if the range of iec0_period_8t was ensured.
    //-----------------------------------------------------------------------------

	for(i=0; i<SPDIFIN_HW_PERIOD_BUF_LEN; i++)
	{
    // check if error occurred (bit17==1 or bit18==1)
    regval = HWREG_R(iec0_rx_debug_info);
        
    if ((regval>>17)&0x3)
        iec_period_8t = 0; // error occurred! set an invalid data
    else
        iec_period_8t = regval&0x7FF;

    // check if iec_period_8t is in the valid data range
    	if (IS_SPDIFIN_PERIOD_LEGAL(iec_period_8t, spdifin_hw_clk.fs_192k.lo, spdifin_hw_clk.fs_32k.hi))
	{
            sum += iec_period_8t;           
        cnt++;
    	}
    }

    // if SPDIF clock draft +-0.2%, flush buffer
    if((sum > spdifin_clk_sum*102/100) || (sum < spdifin_clk_sum*98/100))
    {
        AUDIF_Set_Stop(CODEC_BANK_DEC1);
        AUDIF_Clean_Audio_Buf(CODEC_BANK_DEC1);
        AUDIF_Set_Play(CODEC_BANK_DEC1);
        spdifin_clk_sum = sum;
    }

    // check frequency range
    if (((sum>(spdifin_hw_clk.fs_32k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_32k.hi*cnt))))
        period = IEC_RX_32K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_44k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_44k.hi*cnt))))
        period = IEC_RX_44K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_48k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_48k.hi*cnt))))
        period = IEC_RX_48K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_88k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_88k.hi*cnt))))
        period = IEC_RX_88K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_96k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_96k.hi*cnt))))
        period = IEC_RX_96K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_176k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_176k.hi*cnt))))
        period = IEC_RX_176K_PERIOD;
    if (((sum>(spdifin_hw_clk.fs_192k.lo*cnt))&&(sum<=(spdifin_hw_clk.fs_192k.hi*cnt))))
        period = IEC_RX_192K_PERIOD;
    
    if (spdifin_hw_info.period != period)
    {
        AUDIO_SEMAPHORE_TAKE();
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x03_A0_RAMP_DOWN,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x05_A1_RAMP_DOWN,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x07_A2_RAMP_DOWN,0,0);        
        AUDIO_SEMAPHORE_GIVE(); 

        switch(period)
        {
            case IEC_RX_32K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 32000;
                break;
            case IEC_RX_44K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 44100;
                break;
            case IEC_RX_48K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 48000;
                break;
            case IEC_RX_88K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 88200;
                break;
            case IEC_RX_96K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 96000;
		        break;
            case IEC_RX_176K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 176400;
                break;
            case IEC_RX_192K_PERIOD:
                wfmt_spdifin.wfx.nSamplesPerSec = 192000;
		        break;
	        default:
                AUDDRV_PRINT("AUDIF_Update_SpdifIn::un-known period %d\n",period);
                wfmt_spdifin.wfx.nSamplesPerSec = 48000;
                AUDIF_Set_Stop(CODEC_BANK_DEC1);
		}        

        spdifin_setting.fs = aud_map_SmpRate_IDtoFS((UINT16)SampleRate_Mapping(wfmt_spdifin.wfx.nSamplesPerSec));
        spdifin_hw_info.period = period;

        if((SPDIFIN_PCM_MODE == spdifin_setting.bs_fmt) || (aud_param.coding_mode_now == AUDIF_CODING_MODE_SPDIFINCD))
        {
            memcpy(&gWavFmt, &wfmt_spdifin, sizeof(wfmt_spdifin));
            AUDIF_Set_Stop(CODEC_BANK_DEC1);
            AUDIF_Init_Wave_Audio(&wfmt_spdifin);

            // DSP will queue input data until buffer full
            DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x00_SPDIF_INPUT,0x0,0x0);
            
            AUDIF_Set_Play(CODEC_BANK_DEC1);
            spdifin_setting.bs_fmt = SPDIFIN_PCM_MODE;
        }

        AUDDRV_PRINT("AUDIF_Update_SpdifIn::change fs form %d to %d\n",aud_param.smprate_info.smp_rate_now, spdifin_setting.fs);
        AUDIO_SEMAPHORE_TAKE();
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x02_A0_RAMP_UP,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x04_A1_RAMP_UP,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x06_A2_RAMP_UP,0,0);                        
        AUDIO_SEMAPHORE_GIVE();
    } 

#ifdef DAC_OUTPUT_48K
        spdifin_setting.fs = FS_48K;
#endif        
    
    if(aud_param.smprate_info.smp_rate_now != spdifin_setting.fs)
    {     
        //
        // mute DAC PCM output and SPDIF before change sample rate
        //
        
        UINT8 spdif_mode = aud_param.spdif_mode;    // backup SPDIF mode
        
        AUDIF_Set_SPDIF(SPDIF_OFF);
        
        AUDIO_SEMAPHORE_TAKE();
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x03_A0_RAMP_DOWN,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x05_A1_RAMP_DOWN,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x07_A2_RAMP_DOWN,0,0);        
        AUDIO_SEMAPHORE_GIVE();                
        
        //GL_TaskSleep(40);
		msleep(40);

        AUDIF_Set_SampleRate(spdifin_setting.fs);

        //GL_TaskSleep(40);
		msleep(40);

        AUDIF_Set_SPDIF(spdif_mode);

        AUDIO_SEMAPHORE_TAKE();
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x02_A0_RAMP_UP,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x04_A1_RAMP_UP,0,0);        
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x06_A2_RAMP_UP,0,0);                        
        AUDIO_SEMAPHORE_GIVE(); 
    }

    //-------------------------------------------------------------------------------
    // Download the corresponding codec
    //-------------------------------------------------------------------------------
	switch(bs_fmt)
    {
    case SPDIFIN_PCM_MODE:
		if((SPDIFIN_PCM_MODE != spdifin_setting.bs_fmt)|| (aud_param.coding_mode_now != AUDIF_CODING_MODE_SPDIFINCD))
        {
            AUDIF_Set_Stop(CODEC_BANK_DEC1);
            memcpy(&gWavFmt, &wfmt_spdifin, sizeof(wfmt_spdifin));
            AUDIF_Init_Wave_Audio(&wfmt_spdifin);
        	DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x00_SPDIF_INPUT,0x0,0x0);
            AUDIF_Set_Play(CODEC_BANK_DEC1);
			spdifin_setting.bs_fmt = bs_fmt;
        }
		break;

	case SPDIFIN_AC3_MODE:
		if((SPDIFIN_AC3_MODE != spdifin_setting.bs_fmt) || (aud_param.coding_mode_now != AUDIF_CODING_MODE_SPDIFINAC3))
        {
            AUDIF_Set_Stop(CODEC_BANK_DEC1);
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_SPDIFINAC3);
            AUDIF_Set_Play(CODEC_BANK_DEC1);
			spdifin_setting.bs_fmt = bs_fmt;
        }
		break;

	case SPDIFIN_DTS_MODE:
	case SPDIFIN_DTSCD16_MODE:
	case SPDIFIN_DTSCD14_MODE:        
		if((SPDIFIN_DTS_MODE != spdifin_setting.bs_fmt) || (SPDIFIN_DTSCD16_MODE != spdifin_setting.bs_fmt)
            || (aud_param.coding_mode_now != AUDIF_CODING_MODE_SPDIFINDTS) || (SPDIFIN_DTSCD14_MODE != spdifin_setting.bs_fmt))
        {
            AUDIF_Set_Stop(CODEC_BANK_DEC1);
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_SPDIFINDTS);
            AUDIF_Set_Play(CODEC_BANK_DEC1);
			spdifin_setting.bs_fmt = bs_fmt;
        }
		break;

	default:
		if(SPDIFIN_NO_SIGNAL_MODE != spdifin_setting.bs_fmt)
{
            AUDDRV_PRINT("AUDIF_Update_SpdifIn::un-known bs_fmt %d\n",bs_fmt);
            //AUDIF_Mute();
            AUDIF_Set_Stop(CODEC_BANK_DEC1);
            spdifin_setting.bs_fmt = SPDIFIN_NO_SIGNAL_MODE;
}

	}

	return;
}

#endif  // SUPPORT_QAE377_SPDIF_IN


//*******************************************************
//	 Audio sampling rate setting related APIs
//*******************************************************

UINT16 AUDIF_Get_FrqMsk(void)
{
	return aud_param.smprate_info.frequency_mask;
}

UINT16 AUDIF_Set_FrqMsk(UINT16 mask)
{
/*******************************************
* [Param IN] mask:
*    48K = 0 ; 96K = 1 ; 192K = 2
********************************************/

    if( mask < MASKTBL_MAXSIZE )
	mask = MaskTable[mask];
    else
    {
    	AUDDRV_PRINT("frequence mask is illegal\n");
        return AUDIF_FAILURE;
    }

    if (aud_param.smprate_info.frequency_mask == mask)
    {
    	AUDDRV_PRINT("frequence mask is the same\n");
        return AUDIF_SUCCESS;
    }

	aud_param.smprate_info.frequency_mask = mask;

    return AUDIF_SUCCESS;

}

void AUDIF_Set_SampleRate(INT32 smp_rate)
{
// ND Sampling Rate ID:
// 44.1k: 0x0000 88.2k: 0x0001 22.05k: 0x0002 176.4k: 0x0003 11.025k: 0x0004 5.5125k: 0x0005
// 48  k: 0x0010 96  k: 0x0011 24   k: 0x0012 192  k: 0x0013 12    k: 0x0014      6k: 0x0015
// 32  k: 0x0020 64  k: 0x0021 16   k: 0x0022 128  k: 0x0023  8    k: 0x0024      4k: 0x0025
// 768 k: 0x0030 Not indicated: 0x0031
// K_DSP_SAMPLE_RATE_ID_44K

/***********************************************
*	smp_rate: (ref auddrv.h)
*		FS_32K, FS_44K, FS_48K, FS_96K, FS_192K
************************************************/

    // 9200 restriction : set Fs = 48kHz except DVD and DTV modes
    if ((currSourceType==AUD_SOURCE_DVD) || (currSourceType==AUD_SOURCE_DTV))
    {
        AUDDRV_PRINT("AUDIF_Set_SampleRate::source=%d, smp_rate=%d\n", currSourceType, smp_rate);
    }
    else
    {
        smp_rate = FS_48K;
    }

	AUDDRV_PRINT("AUDIF_Set_SampleRate::smp_rate:0x%x\n",smp_rate);

	UINT32 downSampling = smp_rate;
	UINT32 freqMask_multiple = 1;
#ifndef orion_hdmi_clk
	UINT32 clk_setting = 0;
#endif

	// up-sampling
	switch(smp_rate)
	{
		case FS_8K:
			downSampling = FS_32K;
			break;
		case FS_11K:
			downSampling = FS_44K;
			break;
		case FS_12K:
			downSampling = FS_48K;
			break;
		case FS_16K:
			downSampling = FS_32K;
			break;
		case FS_22K:
			downSampling = FS_44K;
			break;
		case FS_24K:
			downSampling = FS_48K;
			break;
	}

	//mute_line_out();
	//set_line_out_demute_flag(1);

	// frequency_mask={0x0007, 0x0067, 0x0667}
	while ((downSampling & aud_param.smprate_info.frequency_mask) == 0)
	{
		freqMask_multiple <<= 1;
		downSampling >>= 4;
		if (downSampling <= 0)
			break;
	}

	// judge supported frequency or not
	if ((downSampling & aud_param.smprate_info.frequency_mask) == 0)
		downSampling = FS_DEFAULT;

	if (freqMask_multiple != aud_param.smprate_info.frequency_multiple )
	{
		aud_param.smprate_info.frequency_multiple = freqMask_multiple;
		//Init_NewDSPCodec(coding_mode_now);
		//AUDIF_Set_CodingMode(aud_param.coding_mode_now);

	}

// [jyciou] HDMI setting issue	
#ifdef orion_hdmi_clk 
	AUDHW_Set_AudHW_SmpRate(downSampling);
#else
	if (dac_clk_source) // HDMI clk setting
	{	
		clk_setting = HWREG_R(clk_sel_atv_dtv);
		clk_setting &= (~(0x3F<<17));	//clean bit22:17
		clk_setting &= (~(0x3F<<6));	//clean bit11:6
		clk_setting &= (~(0x1<<1));		//clean bit1
		clk_setting &= (~(0x3<<4));	         //clean bit5:4
		clk_setting &= (~(0x1<<16));         //clean bit16
		clk_setting |= 0x1<<17;				
		clk_setting |= 0x1<<6;
		clk_setting |= 0x1<<1;
		clk_setting |= 0x2<<4;
		clk_setting |= 0x1<<16;
		HWREG_W(clk_sel_atv_dtv, clk_setting);
		
	}
	else
	{
		clk_setting = HWREG_R(clk_sel_atv_dtv);
		clk_setting &= (~(0x3<<16));
		clk_setting &= (~(0x2));
		clk_setting &= (~(0x3<<5));
		clk_setting |= 0x1<<4;
		HWREG_W(clk_sel_atv_dtv, clk_setting);
		AUDHW_Set_AudHW_SmpRate(downSampling);
	}
#endif

	// record decoder sample rate
	aud_param.smprate_info.smp_rate_now = smp_rate;

	// set sample rate to main dsp
	DSP_SetRiscParam0(RISC_MAIN_DAC_FS, aud_map_SmpRate_FStoID(downSampling) );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	AUDDRV_PRINT("AUDIF_Set_SampleRate::smp_rate_now=0x%x  frequency_multiple=0x%x\n",aud_param.smprate_info.smp_rate_now,aud_param.smprate_info.frequency_multiple);
	AUDDRV_PRINT("AUDIF_Set_SampleRate::audhw set Sample Rate:0x%x\n",downSampling);

}

void AUDIF_Set_SPDIF_SmpRate(INT32 smp_rate)
{
	/***********************************************
    * 2011/07/04
    * 9200 DAC/IEC0/IEC1 run on same Fs
    * NO need to set sample rate again
    ************************************************/    
    return;
    
	/***********************************************
	*	smp_rate: (ref auddrv.h)
	*		FS_32K, FS_44K, FS_48K, FS_96K, FS_192K
	************************************************/

	AUDDRV_PRINT("AUDIF_Set_SampleRate_IEC0::smp_rate:0x%x\n",smp_rate);

	UINT32 downSampling = smp_rate;

	// up-sampling
	switch(smp_rate)
	{
		case FS_8K:
			downSampling = FS_32K;
			break;
		case FS_11K:
			downSampling = FS_44K;
			break;
		case FS_12K:
			downSampling = FS_48K;
			break;
		case FS_16K:
			downSampling = FS_32K;
			break;
		case FS_22K:
			downSampling = FS_44K;
			break;
		case FS_24K:
			downSampling = FS_48K;
			break;
	}

	// frequency_mask={0x0007, 0x0067, 0x0667}
	while ((downSampling & aud_param.smprate_info.frequency_mask) == 0)
	{
		downSampling >>= 4;
		if (downSampling <= 0)
			break;
	}

	// judge supported frequency or not
	if ((downSampling & aud_param.smprate_info.frequency_mask) == 0)
		downSampling = FS_DEFAULT;

	DSP_SetRiscParam0(RISC_MAIN_IEC0_FS, aud_map_SmpRate_FStoID(downSampling) );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	AUDHW_Set_IEC0_CLK(downSampling);	// SPDIF clock setting

}

#ifdef AUD_SUPPORT_HDMI
void AUDIF_Set_HDMI_SmpRate(INT32 smp_rate,UINT8 pathmode)
{
	/***********************************************
    * 2011/07/04
    * 9200 DAC/IEC0/IEC1 run on same Fs
    * NO need to set sample rate again
    ************************************************/
    return;
    
	/***********************************************
	*	smp_rate: (ref auddrv.h)
	*		FS_32K, FS_44K, FS_48K, FS_96K, FS_192K
	************************************************/

	AUDDRV_PRINT("AUDIF_Set_SampleRate_IEC1::smp_rate:0x%x\n",smp_rate);

	UINT32 downSampling = smp_rate;

	// up-sampling
	switch(smp_rate)
	{
		case FS_8K:
			downSampling = FS_32K;
			break;
		case FS_11K:
			downSampling = FS_44K;
			break;
		case FS_12K:
			downSampling = FS_48K;
			break;
		case FS_16K:
			downSampling = FS_32K;
			break;
		case FS_22K:
			downSampling = FS_44K;
			break;
		case FS_24K:
			downSampling = FS_48K;
			break;
	}

	// frequency_mask={0x0007, 0x0067, 0x0667}
	while ((downSampling & aud_param.smprate_info.frequency_mask) == 0)
	{
		downSampling >>= 4;
		if (downSampling <= 0)
			break;
	}

	// judge supported frequency or not
	if ((downSampling & aud_param.smprate_info.frequency_mask) == 0)
		downSampling = FS_DEFAULT;

	DSP_SetRiscParam0(RISC_MAIN_IEC1_FS, aud_map_SmpRate_FStoID(downSampling) );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

#ifndef QAE377_FPGA_MODE
	switch( pathmode )
	{
		case HDMI_AUD_IN_SPDIF:
			AUDHW_Set_IEC1_CLK(downSampling);	// SPDIF clock setting
			AUDHW_Set_IntDAC_CLK(downSampling); // I2S clock setting
			break;
		case HDMI_AUD_IN_I2S:
			AUDHW_Set_IntDAC_CLK(downSampling); // I2S clock setting
			break;
		default:
			break;
	}
#endif

}

void AUD_IS_Support_HDMI_SmpRate(UINT32 *downSampling)
{
#if (PRODUCT_CAT != DTV)
//cmhuang: 2010.03.17 check HDMI EDID for supported sampling rate,
//         if high sampling rate not support, need to do down-sampling
	int ret_check=0;
	UINT8 RetSupported;
	ShortAudioForm_t HDMIAudDetail;

	ret_check = HDMI_IF_AudioCodeChecked( HDMI_ACF_LPCM , &RetSupported );
	if( ret_check == HDMI_OPERATION_SUCCESS )
	{

		// check supported PCM sampling rate
		ret_check = HDMI_IF_AudioCodeDetail(HDMI_ACF_LPCM, &HDMIAudDetail);
		if( ret_check == HDMI_OPERATION_SUCCESS )
		{
			switch(*downSampling)
			{
				// assume FS_32K, FS_44K, FS_48K always supported
				case FS_88K:
					if( ( HDMIAudDetail.bSampleRateInfo & (1<<HDMI_AUD_FS_88K) ) == 0 )
						*downSampling = FS_44K;
					break;
				case FS_96K:
					if( ( HDMIAudDetail.bSampleRateInfo & (1<<HDMI_AUD_FS_96K) ) == 0 )
						*downSampling = FS_48K;
					break;
				case FS_176K:
					if( ( HDMIAudDetail.bSampleRateInfo & (1<<HDMI_AUD_FS_176K) ) == 0 )
					{
						if( ( HDMIAudDetail.bSampleRateInfo & (1<<HDMI_AUD_FS_88K) ) == 0 )
							*downSampling = FS_44K;
						else
							*downSampling = FS_88K;
					}
					break;
				case FS_192K:
					if( ( HDMIAudDetail.bSampleRateInfo & (1<<HDMI_AUD_FS_192K) ) == 0 )
					{
						if( ( HDMIAudDetail.bSampleRateInfo & (1<<HDMI_AUD_FS_96K) ) == 0 )
							*downSampling = FS_48K;
						else
							*downSampling = FS_96K;
					}
					break;
				default:
					break;
			}
		}
	}
#endif
}
#endif

void AUDIF_Update_Dec1_SampleRate(void)
{
	UINT16 pcm_smp=0;
  	UINT16 codecID = 0;

	pcm_smp = (UINT16)(DSP_GetSampleRate()&0xff );
	if( pcm_smp == 0xff )
	{
        // if DSP return sample rate == 0xFF ---> "not ready"
		return;
	}
		
	codecID = (DSP_GetDspParam0(DSP_DEC1_ID)>>8);
    if (codecID == 0)
    {
        // if codec ID == 0 ---> no decoder is running, return
        return;
    }

    // update sample rate config only on "DTV" and "MEDIA"
    if (!((currSourceType == AUD_SOURCE_DTV) || \
        (currSourceType == AUD_SOURCE_MEDIA)))
    {
        return;
    }
    
#ifdef DAC_OUTPUT_48K	
	// 9200 restriction : if 48kHz group, set 48kHz
	if ((pcm_smp&0xFFF0)==K_DSP_SAMPLE_RATE_ID_48K)
	{
		pcm_smp = K_DSP_SAMPLE_RATE_ID_48K;
	}
#endif
	pcm_smp = aud_map_SmpRate_IDtoFS( pcm_smp );

	if( aud_param.smprate_info.smp_rate_now == pcm_smp )
	{
		return;
	}
	else
	{
		AUDIF_Set_SampleRate(pcm_smp);
	}
}

UINT16 AUDIF_Get_SampleRate( void )
{
	UINT16 pcm_smp,hw_smp;
	pcm_smp = aud_param.smprate_info.smp_rate_now;
	hw_smp = aud_param.smprate_info.hw_smprate;

	switch(pcm_smp)
	{
		case FS_8K:
			diag_printf("AUDIF_Get_SampleRate:: 8KHz\n");
			break;
		case FS_11K:
			diag_printf("AUDIF_Get_SampleRate:: 11KHz\n");
			break;
		case FS_12K:
			diag_printf("AUDIF_Get_SampleRate:: 12KHz\n");
			break;
		case FS_16K:
			diag_printf("AUDIF_Get_SampleRate:: 16KHz\n");
			break;
		case FS_22K:
			diag_printf("AUDIF_Get_SampleRate:: 22KHz\n");
			break;
		case FS_24K:
			diag_printf("AUDIF_Get_SampleRate:: 24KHz\n");
			break;
		case FS_32K:
			diag_printf("AUDIF_Get_SampleRate:: 32KHz\n");
			break;
		case FS_44K:
			diag_printf("AUDIF_Get_SampleRate:: 44KHz\n");
			break;
		case FS_48K:
			diag_printf("AUDIF_Get_SampleRate:: 48KHz\n");
			break;
		case FS_64K:
			diag_printf("AUDIF_Get_SampleRate:: 64KHz\n");
			break;
		case FS_88K:
			diag_printf("AUDIF_Get_SampleRate:: 88KHz\n");
			break;
		case FS_96K:
			diag_printf("AUDIF_Get_SampleRate:: 96KHz\n");
			break;
		case FS_176K:
			diag_printf("AUDIF_Get_SampleRate:: 176KHz\n");
			break;
		case FS_192K:
			diag_printf("AUDIF_Get_SampleRate:: 192KHz\n");
			break;
	}

	switch(hw_smp)
	{
		case FS_8K:
			diag_printf("Audio HW SampleRate:: 8KHz\n");
			break;
		case FS_11K:
			diag_printf("Audio HW SampleRate:: 11KHz\n");
			break;
		case FS_12K:
			diag_printf("Audio HW SampleRate:: 12KHz\n");
			break;
		case FS_16K:
			diag_printf("Audio HW SampleRate:: 16KHz\n");
			break;
		case FS_22K:
			diag_printf("Audio HW SampleRate:: 22KHz\n");
			break;
		case FS_24K:
			diag_printf("Audio HW SampleRate:: 24KHz\n");
			break;
		case FS_32K:
			diag_printf("Audio HW SampleRate:: 32KHz\n");
			break;
		case FS_44K:
			diag_printf("Audio HW SampleRate:: 44KHz\n");
			break;
		case FS_48K:
			diag_printf("Audio HW SampleRate:: 48KHz\n");
			break;
		case FS_64K:
			diag_printf("Audio HW SampleRate:: 64KHz\n");
			break;
		case FS_88K:
			diag_printf("Audio HW SampleRate:: 88KHz\n");
			break;
		case FS_96K:
			diag_printf("Audio HW SampleRate:: 96KHz\n");
			break;
		case FS_176K:
			diag_printf("Audio HW SampleRate:: 176KHz\n");
			break;
		case FS_192K:
			diag_printf("Audio HW SampleRate:: 192KHz\n");
			break;
	}

	return pcm_smp;

}

INT32 aud_map_SmpRate_FStoDigital( UINT16 smp_in )
{
	INT32 smp_out = 0;
	switch( smp_in )
	{
		case FS_32K:
			smp_out = 32000;
			break;
		case FS_44K:
			smp_out = 44100;
			break;
		case FS_48K:
			smp_out = 48000;
			break;
		case FS_88K:
			smp_out = 88200;
			break;
		case FS_96K:
			smp_out = 96000;
			break;
		case FS_176K:
			smp_out = 176400;
			break;
		case FS_192K:
			smp_out = 192000;
			break;
		default:
			smp_out = 32000;
			break;
	}

	return smp_out;

}

UINT16 aud_map_SmpRate_FStoID(UINT16 smp_fs )
{
	UINT16 smp_ID = K_DSP_SAMPLE_RATE_ID_48K;

	switch(smp_fs)
	{
	case FS_8K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_8K;
		break;
	case FS_11K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_11K;
		break;
	case FS_12K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_12K;
		break;
	case FS_16K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_16K;
		break;
	case FS_22K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_22K;
		break;
	case FS_24K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_24K;
		break;
	case FS_32K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_32K;
		break;
	case FS_44K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_44K;
		break;
	case FS_48K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_48K;
		break;
	case FS_64K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_64K;
		break;
	case FS_88K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_88K;
		break;
	case FS_96K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_96K;
		break;
	case FS_128K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_128K;
		break;
	case FS_176K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_176K;
		break;
	case FS_192K:
		smp_ID = K_DSP_SAMPLE_RATE_ID_192K;
		break;
	default:
		AUDDRV_PRINT("default sample rate\n");
		smp_ID = K_DSP_SAMPLE_RATE_ID_48K;
		break;
	}

	return smp_ID;

}

//*******************************************************
//	Audio Post-Processing Control APIs
//*******************************************************
// this function can't use  AUDIO_SEMAPHORE_TAKE() & AUDIO_SEMAPHORE_GIVE()

#ifdef AUDIO_MUTE_BY_PLAT_LAYER_REG
//Demute according to "playform layer mute/demute status register" information
//plat_ctrlayer_mute_id is define in status_reg_def.h(PSREG_STAT_AudioMuteCtrl -> MuteID)
INT32 AUDIF_Demute_PLAT_REG(UINT32 plat_ctrlayer_mute_id)
{
    	int iRes = AUDIF_FAILURE;
   	UINT32 mute_status = 0;

	PLAT_REG_AddAudioMuteCtrl(PSREG_AUDIO_DEMUTE,plat_ctrlayer_mute_id);
	PLAT_REG_GetAudioMuteStatus(&mute_status);

	if(mute_status == PSREG_AUDIO_MUTE_OFF)
	{
		AUDDRV_PRINT("AUDIF_Demute\n");
		iRes = AUDIF_Demute();
	}
	else if(mute_status == PSREG_AUDIO_MUTE_ON)
	{
		AUDDRV_PRINT("AUDIF_Demute is NOT allowed\n");
		AUDDRV_PRINT("Mute flag still pull by module 0x%x",PSREG_VALUE(PSREG_STAT_AudioMuteCtrl));
		iRes = AUDIF_FAILURE;
	}

    return iRes;
}


//Mute according to "playform layer mute/demute status register" information
//plat_ctrlayer_mute_id is define in status_reg_def.h(PSREG_STAT_AudioMuteCtrl -> MuteID)
INT32 AUDIF_Mute_PLAT_REG(UINT32 plat_ctrlayer_mute_id)
{
    int iRes;
	int is_allow_mute=0; // 0: prohibit; 1: allow
   	UINT32 mute_status = 0;

	PLAT_REG_AddAudioMuteCtrl(PSREG_AUDIO_MUTE,plat_ctrlayer_mute_id);
	PLAT_REG_GetAudioMuteStatus(&mute_status);

	if(mute_status == PSREG_AUDIO_MUTE_ON)
	{
		is_allow_mute = 1;
	}
	else if(mute_status == PSREG_AUDIO_MUTE_OFF)
	{
		is_allow_mute = 0;
	}

	if( is_allow_mute == 1 )
	{
		AUDDRV_PRINT("AUDIF_Mute\n");
    		iRes = AUDIF_Mute();
	}
	else
	{
		AUDDRV_PRINT("AUDIF_Mute is NOT allowed\n");
		iRes = AUDIF_FAILURE;
	}

	AUDDRV_PRINT("Mute flag pulled by 0x%x",PSREG_VALUE(PSREG_STAT_AudioMuteCtrl));

    return iRes;
}

INT32 AUDIF_Set_InternalMuteStatus(UINT8 mute_flag)
{
	AUDIO_SEMAPHORE_TAKE();

	if (mute_flag == MUTE_STATUS_ON)
	{
		DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, -672);
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	}
	else
	{
		if (aud_param.master_mute_glag == MUTE_STATUS_OFF)
		{
			DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, aud_param.master_volume);
			DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
		}
	}

	aud_param.internal_mute_flag = mute_flag;

	AUDIO_SEMAPHORE_GIVE();

	return AUDIF_SUCCESS;
}
#else
INT32 AUDIF_Demute_PLAT_REG(UINT32 plat_ctrlayer_mute_id)
{
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Mute_PLAT_REG(UINT32 plat_ctrlayer_mute_id)
{
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Set_InternalMuteStatus(UINT8 mute_flag)
{
	return AUDIF_SUCCESS;
}
#endif


INT32 AUDIF_Demute(void)
{
    AUDIO_SEMAPHORE_TAKE();
	
	AUDDRV_PRINT("AUDIF_Demute\n");
#if 0
    UINT32 Master_Gain = 0x80000000;    

    DSP_SendCmd(0x08, 0x00, 0x0000, Master_Gain);
    DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_H, Master_Gain>>16);
    DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_L, Master_Gain&0xffff);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	aud_param.mute = 0;
#else
    // MUTE_STATUS_ON:mute, MUTE_STATUS_OFF:de-mute
    DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, aud_param.master_volume);
	aud_param.master_mute_glag = MUTE_STATUS_OFF;
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0); 
#endif
    AUDIO_SEMAPHORE_GIVE();

    return 1;
}


INT32 AUDIF_Mute(void)
{
    AUDIO_SEMAPHORE_TAKE();
	
	AUDDRV_PRINT("AUDIF_Mute\n");
#if 0
    UINT32 Master_Gain = 0;

    DSP_SendCmd(0x08, 0x00, 0x0000, Master_Gain);
    DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_H, Master_Gain>>16);
    DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_L, Master_Gain&0xffff);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

    aud_param.mute = 1;
#else
    // MUTE_STATUS_ON:mute, MUTE_STATUS_OFF:de-mute
    DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, -672);
	aud_param.master_mute_glag = MUTE_STATUS_ON;
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0); 
#endif
	
    AUDIO_SEMAPHORE_GIVE();

    return 1;
}


INT32 AUDIF_Get_volume( void )
{
	/*if( aud_param.mute == 1 )
		return 0;
	else*/
	return aud_param.volume;
}

INT32 AUDIF_Get_MuteStatus(void)
{
	// 1:mute ; 0:demute
	return aud_param.mute;
}

/******************************************************************************/
/**
* @brief    Set Aux1 output mute or un-mute
* @param    mute_flag : MUTE_STATUS_ON->mute, else set volume to original value
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Aux1MuteStatus(UINT8 mute_flag)
{
    AUDIO_SEMAPHORE_TAKE();

    // MUTE_STATUS_ON:mute, MUTE_STATUS_OFF:de-mute
    if (MUTE_STATUS_ON == mute_flag)
    {
        DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX1_VOL, BIT_11,BIT_0, -672);
    }   
    else
    {
        DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX1_VOL, BIT_11,BIT_0, aud_param.aux1_volume);
    }

	aud_param.aux1_mute_glag = mute_flag;

    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);    

    AUDIO_SEMAPHORE_GIVE();
    return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set Aux2 output mute or un-mute
* @param    mute_flag : MUTE_STATUS_ON->mute, else set volume to original value
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Aux2MuteStatus(UINT8 mute_flag)
{
    AUDIO_SEMAPHORE_TAKE();

    // MUTE_STATUS_ON:mute, MUTE_STATUS_OFF:de-mute
    if (MUTE_STATUS_ON == mute_flag)
    {
        DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX2_VOL, BIT_11,BIT_0, -672);
    }   
    else
    {
        DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX2_VOL, BIT_11,BIT_0, aud_param.aux2_volume);
    }

	aud_param.aux2_mute_glag = mute_flag;
	
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);        

    AUDIO_SEMAPHORE_GIVE();
    return AUD_SUCCESS;    
}

/******************************************************************************/
/**
* @brief    Set Master output mute or un-mute
* @param    mute_flag : MUTE_STATUS_ON->mute, else set volume to original value
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_MasterMuteStatus(UINT8 mute_flag)
{
    AUDIO_SEMAPHORE_TAKE();

    // MUTE_STATUS_ON:mute, MUTE_STATUS_OFF:de-mute
    if (MUTE_STATUS_ON == mute_flag)
    {
        DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, -672);
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    }   
    else
    {
		if (aud_param.internal_mute_flag == MUTE_STATUS_OFF)	// wait internal demute
		{
        DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, aud_param.master_volume);
			DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
		}
    } 

	aud_param.master_mute_glag = mute_flag;

    AUDIO_SEMAPHORE_GIVE();

    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Get_MasterMuteStatus(UINT8 *mute_status)
{
/*************************************************
 [param]mute_status:
	MUTE_STATUS_ON   : 1
	MUTE_STATUS_OFF  : 0
**************************************************/
    AUDIO_SEMAPHORE_TAKE();

    *mute_status = aud_param.master_mute_glag;    

    AUDIO_SEMAPHORE_GIVE();
    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Get_Aux1MuteStatus(UINT8 *aux1_mute_status)
{
/*************************************************
 [param]mute_status:
	MUTE_STATUS_ON   : 1
	MUTE_STATUS_OFF  : 0
**************************************************/
    AUDIO_SEMAPHORE_TAKE();
    *aux1_mute_status = aud_param.aux1_mute_glag;    
    AUDIO_SEMAPHORE_GIVE();
    return AUD_SUCCESS;   
}

AUDIF_Status_t AUDIF_Get_Aux2MuteStatus(UINT8 *aux2_mute_status)
{
/*************************************************
 [param]mute_status:
	MUTE_STATUS_ON   : 1
	MUTE_STATUS_OFF  : 0
**************************************************/
    AUDIO_SEMAPHORE_TAKE();
    *aux2_mute_status = aud_param.aux2_mute_glag;    
    AUDIO_SEMAPHORE_GIVE();
    return AUD_SUCCESS; 
}

void AUDIF_SetupMenu_Param( UINT16 type, UINT16 value , BYTE* param )
{
	UINT16 eq65,BM_ref;
	UINT32 eqHL;
	UINT16 spk_f,spk_c,spk_r,spk_b,spk_s;
	UINT16 dly_LF,dly_RF,dly_LS,dly_RS,dly_C,dly_SUB,dly_LB,dly_RB,dly_LMRM;
	UINT16 key;

	//provide setup menu to write RISC paramter to DRAM
	switch(type)
	{
		case AUDIF_SETUP_SPDIF:
			DSP_SetRiscParam0(RISC_MAIN_SPDIF_MODE, value );
			break;
		case AUDIF_SETUP_HDMIAudio:
			// cmhuang: 2010.08.17 RISC Param will be set in AUDIF_Set_HDMI()
			// only use aud_param.hdmi_mode to record setup menu setting here
			//DSP_SetRiscParam0(RISC_MAIN_HDMI_MODE, value );
			aud_param.hdmi_mode = value;
			break;
		case AUDIF_SETUP_DownSampling:
			//AUDDRV_PRINT("no use\n");
			break;
		case AUDIF_SETUP_Surround:
			DSP_SetRiscParam0(RISC_MAIN_3D_SURROUND, value );
			break;
		case AUDIF_SETUP_EQ:
			switch(value)
			{
		    case EQ_CLASSIC:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_CLASSIC_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_CLASSIC_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_CLASSIC_65 );
		        break;
	        case EQ_ROCK:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_ROCK_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_ROCK_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_ROCK_65 );
		        break;
		    case EQ_JAZZ:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_JAZZ_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_JAZZ_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_JAZZ_65 );
		        break;
		    case EQ_POP:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_POP_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_POP_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_POP_65 );
		        break;
		    case EQ_DANCE:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_DANCE_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_DANCE_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_DANCE_65 );
		        break;
		    case EQ_TECHNO:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_TECHNO_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_TECHNO_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_TECHNO_65 );
		        break;
		    case EQ_SOFT:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_SOFT_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_SOFT_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_SOFT_65 );
		        break;
		    case EQ_LIVE:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_LIVE_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_LIVE_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_LIVE_65 );
		        break;
		    case EQ_SATNDARD:
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, EQ_STANDARD_40HI );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, EQ_STANDARD_40LO );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, EQ_STANDARD_65 );
		        break;
		    case EQ_USER_DEFINED:
		        eq65 = (((UINT16)param[6])<<6) + (UINT16)param[5];
		        eqHL = (((UINT32)param[4])<<24) + (((UINT32)param[3])<<18) + (((UINT32)param[2])<<12) + (((UINT32)param[1])<<6) + ((UINT32)param[0]);
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_HI, (UINT16)(eqHL>>16) );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_4_0_LO, (UINT16)(eqHL&0xffff) );
				DSP_SetRiscParam0(RISC_MAIN_EQ_GAIN_6_5, eq65 );
		        break;
			}
			break;
		case AUDIF_SETUP_DRC:
			// should be set again in DDP parameter setting when download decoder
			DSP_SetRiscParam0(RISC_DEC1_PARAMS_START+4, aud_param.out_mode.ac3_dyncmprs_mode );
			break;
		case AUDIF_SETUP_KaraokeEcho:
			//delay
			//DSP_SetRiscParam0(RISC_MAIN_ECHO_DELAY, echo_para_tbl[aud_param.echo_level][1] );
			//decay(gain)
			DSP_SetRiscParam0(RISC_MAIN_ECHO_GAIN, echo_para_tbl[aud_param.echo_level][0] );
			break;
		case AUDIF_SETUP_KaraokeKeyShift:
			//AUDDRV_PRINT("\n[yang]SETUP_EVENT_KaraokeKeyShift: %d\n",value);
			key = 0x0001; // 1: for keyshift mode
			key |= (value << 8);
			DSP_SetRiscParam0(RISC_MAIN_KEY_SHIFT, key );
			break;
		case AUDIF_SETUP_AudioDownMix:
			DSP_SetRiscParam0(RISC_MAIN_DOWNMIX_MODE, value );
			break;
		case AUDIF_SETUP_GM5:
			// move to AUDIF_Set_GM5()
			//DSP_SetRiscParam0(RISC_SWAPP_ID, en_gm5 );
			//DSP_SetRiscParam0(RISC_SWAPP_PARAM0, value );
			//DSP_SetRiscParam0(RISC_SWAPP_PARAM1, 0 );
			//DSP_SetRiscParam0(RISC_SWAPP_PARAM2, 0 );
			break;
		case AUDIF_SETUP_AudioSize:
			//Speaker Config/size
			spk_f = (UINT16)aud_param.speaker_info.front;
			spk_c = (UINT16)aud_param.speaker_info.center;
			spk_r = (UINT16)aud_param.speaker_info.surround;
			spk_b = (UINT16)aud_param.speaker_info.back;
			spk_s = (UINT16)aud_param.speaker_info.subwoofer;

			value = spk_s | spk_b<<3 | spk_r<<6 | spk_c<< 9 | spk_f<<12;
			DSP_SetRiscParam0(RISC_MAIN_BASS_MANAGEMENT_SPK, value );

			BM_ref = spk_f*18 + spk_c*6 + spk_r*2 + spk_s;
			DSP_SetRiscParam0(RISC_MAIN_BASS_MANAGEMENT_CFG, BM_ref );
			break;
		case AUDIF_SETUP_AudioDistance:
			//SpkDelay
			dly_LF = (UINT16)aud_param.mspk_delay.LF;
			dly_RF = (UINT16)aud_param.mspk_delay.RF;
			dly_LS = (UINT16)aud_param.mspk_delay.LS;
			dly_RS = (UINT16)aud_param.mspk_delay.RS;
			dly_C = (UINT16)aud_param.mspk_delay.C;
			dly_SUB = (UINT16)aud_param.mspk_delay.SUB;
			dly_LB = (UINT16)aud_param.mspk_delay.LB;
			dly_RB = (UINT16)aud_param.mspk_delay.RB;
			dly_LMRM = (UINT16)aud_param.mspk_delay.LMRM;

		// Version B
			value = dly_LMRM | dly_LF<<5 | dly_RF<<10;
			DSP_SetRiscParam0(RISC_MAIN_SPK_TIME_DELAY_FRONT, value );
			value = dly_LS | dly_RS<<5 | dly_LB<<10;
			DSP_SetRiscParam0(RISC_MAIN_SPK_TIME_DELAY_SURROUND, value );
			value = dly_C | dly_SUB<<5 | dly_RB<<10;
			DSP_SetRiscParam0(RISC_MAIN_SPK_TIME_DELAY_BACK, value );
#if 0	// Version A
			value = dly_LF | dly_LS<<8;
			DSP_SetRiscParam0(RISC_MAIN_SPK_TIME_DELAY_FRONT, value );
			value = dly_C | dly_LB<<8;
			DSP_SetRiscParam0(RISC_MAIN_SPK_TIME_DELAY_SURROUND, value );
#endif
			break;

		case AUDIF_SETUP_AudioLevel:
			//SPKGain
			switch( *param )
			{
				case LF_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A0_GAIN, (value<<8)|aud_param.vol_mspk.RF ); //LF
					break;
				case RF_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A0_GAIN, (aud_param.vol_mspk.LF<<8)|value ); //RF
					break;
				case LS_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A1_GAIN, (value<<8)|aud_param.vol_mspk.RS ); //LS
					break;
				case RS_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A1_GAIN, (aud_param.vol_mspk.LS<<8)|value ); //RS
					break;
				case C_PKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A2_GAIN, (value<<8)|aud_param.vol_mspk.SUB); //C
					break;
				case SW_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A2_GAIN, (aud_param.vol_mspk.C<<8)|value ); //LEF
					break;
				case LB_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A3_GAIN, (value<<8)|aud_param.vol_mspk.RB ); //LB
					break;
				case RB_SPKGAIN:
					DSP_SetRiscParam0(RISC_MAIN_A3_GAIN, (aud_param.vol_mspk.LB<<8)|value ); //RB
					break;
			}
			break;
		case AUDIF_SETUP_Mic:   //lptan add for maintis 94477 2010-7-5
			DSP_SetRiscParam0(RISC_MAIN_MIC_SWITCH, value );
			break;
		default:
			AUDDRV_PRINT("Error Set RISC PARAM tos DRAM");
			break;
	}

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

}

UINT16 AUDIF_Get_BassConfig(void)
{
	return (aud_param.gBass_Config);
}

void AUDIF_Get_EQ_Spectrum(UINT16 *p_eq)
{
    AUDIO_SEMAPHORE_TAKE();

	p_eq[0] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND0);
	p_eq[1] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND1);
	p_eq[2] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND2);
	p_eq[3] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND3);
	p_eq[4] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND4);
	p_eq[5] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND5);
	p_eq[6] = DSP_GetDspParam0(DSP_MAIN_EQ_BAND6);

#if 0//def DEBUG_ENABLE_AUDDRV_PRINT
	int i=0;
	for(i=0;i<EQ_BAND_MAX;i++)
	{
		AUDDRV_PRINT("EQ[%d]: 0x%x\n",i, p_eq[i]);
	}
#endif

	AUDIO_SEMAPHORE_GIVE();

}

UINT32 AUDIF_Get_PLII_Init_Config(void)
{
	return aud_param.pl2_init_val;
}

UINT8 AUDIF_Get_Speaker_Config(UINT8 spktype)
{
    AUDIO_SEMAPHORE_TAKE();
	UINT8 spkvalue = 0x3;

	switch (spktype)
	{
	    case BM_SPK_FRONT:
	        spkvalue = aud_param.speaker_info.front;
	        break;
	    case BM_SPK_SURR:
	        spkvalue = aud_param.speaker_info.surround;
	        break;
	    case BM_SPK_C:
	        spkvalue = aud_param.speaker_info.center;
	        break;
	    case BM_SPK_SUB:
	        spkvalue = aud_param.speaker_info.subwoofer;
	        break;
	}
	AUDIO_SEMAPHORE_GIVE();
	return spkvalue;
}

void AUDIF_Init_Speaker_Config(void)
{
    AUDIO_SEMAPHORE_TAKE();
    aud_param.speaker_info.front = BM_SPK_LARGE;
    aud_param.speaker_info.surround = BM_SPK_LARGE;
    aud_param.speaker_info.back = BM_SPK_LARGE;
    aud_param.speaker_info.center = BM_SPK_LARGE;
    aud_param.speaker_info.subwoofer = BM_SPK_ON;
    aud_param.speaker_info.LF= BM_SPK_ON;
	aud_param.speaker_info.RF= BM_SPK_ON;
	aud_param.speaker_info.LS= BM_SPK_ON;
	aud_param.speaker_info.RS= BM_SPK_ON;
	aud_param.speaker_info.LB= BM_SPK_OFF;
	aud_param.speaker_info.RB= BM_SPK_OFF;

    AUDIO_SEMAPHORE_GIVE();
}

UINT8 AUDIF_IS_PLII_MODE(UINT8 mode)
{
	AUDIO_SEMAPHORE_TAKE();

	UINT8 pl2_mode_true;

	pl2_mode_true = ((aud_param.pl2_init_val & ((PLII_ON & 0xf)<<PLII_ENABLE_BIT)) && ((mode & PLII_FUNC_MASK)== PLII_FUNC_MODE) && (aud_param.pl2_init_val & ((mode& 0xf)<<PLII_MODE_BIT)));


	AUDDRV_PRINT("AUDDRV : Is PLII Mode[%x]? : %d\n",mode,pl2_mode_true);

	AUDIO_SEMAPHORE_GIVE();

	return pl2_mode_true;
}

// name = AUDIF_Set_5SPKGain in 8203R platform
INT32 AUDIF_Set_SPKGain(UINT8 spktype, UINT8 val)
{
	UINT16 wVal = (UINT16)val;
	if(spktype > RB_SPKGAIN)
	   return 0;
    if(spktype == MASTER_GAIN)
	return 0;
    AUDIO_SEMAPHORE_TAKE();

    // Lm & Rm resolution 16 bits
    if((spktype == LM_SPKGAIN) | (spktype == RM_SPKGAIN))
    {
        wVal = wVal << 8;
    }
    DSP_SendCmd(0x08, spktype, wVal, 0x00000000);

	switch(spktype)
	{
		case LF_SPKGAIN:
			aud_param.vol_mspk.LF = val;
			break;
		case RF_SPKGAIN:
			aud_param.vol_mspk.RF = val;
			break;
		case LS_SPKGAIN:
			aud_param.vol_mspk.LS = val;
			break;
		case RS_SPKGAIN:
			aud_param.vol_mspk.RS = val;
			break;
		case C_PKGAIN:
			aud_param.vol_mspk.C = val;
			break;
		case SW_SPKGAIN:
			aud_param.vol_mspk.SUB = val;
			break;
		case LB_SPKGAIN:
			aud_param.vol_mspk.LB = val;
			break;
		case RB_SPKGAIN:
			aud_param.vol_mspk.RB = val;
			break;
	}

	AUDIO_SEMAPHORE_GIVE();
	return 1;
}

void AUDIF_Init_SPKGain(UINT8 value)
{
	AUDIF_Set_SPKGain(LF_SPKGAIN,value);
	AUDIF_Set_SPKGain(RF_SPKGAIN,value);
	AUDIF_Set_SPKGain(LS_SPKGAIN,value);
	AUDIF_Set_SPKGain(RS_SPKGAIN,value);
	AUDIF_Set_SPKGain(C_PKGAIN,value);
	AUDIF_Set_SPKGain(SW_SPKGAIN,value);
	AUDIF_Set_SPKGain(LB_SPKGAIN,value);
	AUDIF_Set_SPKGain(RB_SPKGAIN,value);

	DSP_SetRiscParam0(RISC_MAIN_A0_GAIN, (aud_param.vol_mspk.LF<<8)|aud_param.vol_mspk.RF );
	DSP_SetRiscParam0(RISC_MAIN_A1_GAIN, (aud_param.vol_mspk.LS<<8)|aud_param.vol_mspk.RS );
	DSP_SetRiscParam0(RISC_MAIN_A2_GAIN, (aud_param.vol_mspk.SUB<<8)|aud_param.vol_mspk.C);
	DSP_SetRiscParam0(RISC_MAIN_A3_GAIN, (aud_param.vol_mspk.LB<<8)|aud_param.vol_mspk.RB );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
}

void AUDIF_Set_BassConfig(void)
{
    AUDIO_SEMAPHORE_TAKE();
    UINT16 BM_ref;

	BM_ref = (UINT16)aud_param.speaker_info.front*18 +
	         (UINT16)aud_param.speaker_info.center*6 +
	         (UINT16)aud_param.speaker_info.surround*2 +
	         (UINT16)aud_param.speaker_info.subwoofer;

	if (BM_ref<=36)
	{
	    DSP_SendCmd(0x10,0x03,BM_ref,0x00000000);
	    aud_param.gBass_Config = BM_ref;
	}
	else
	    DSP_SendCmd(0x10,0x03,0x0000,0x00000000);

	AUDIO_SEMAPHORE_GIVE();
}

INT32 AUDIF_Set_CSPKGain(UINT8 val)
{
	UNUSED_ARGUMENT(val);
	return 0;
}

INT32 AUDIF_Set_Echo(UINT8 delay, UINT16 decay)
{
	AUDIO_SEMAPHORE_TAKE();

	UNUSED_ARGUMENT(delay);
	DSP_SendCmd(0x09, 0x03, decay, 0x0);
	DSP_SetRiscParam0(RISC_MAIN_ECHO_GAIN, echo_para_tbl[aud_param.echo_level][0] );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	AUDIO_SEMAPHORE_GIVE();
    return 1;
}

INT32 AUDIF_Set_EchoPara(UINT8 x)
{
    int res;
    //AUDIO_SEMAPHORE_TAKE();
	aud_param.echo_level = x;
    res = AUDIF_Set_Echo(echo_para_tbl[x][1], echo_para_tbl[x][0] );
    //AUDIO_SEMAPHORE_GIVE();
    return res;
}

INT32 AUDIF_Set_EQ_Level(BYTE *p, UINT8 num_band)
{
	UNUSED_ARGUMENT(p);
	UNUSED_ARGUMENT(num_band);
	return 0;
}

INT32 AUDIF_Set_EQ_Show_Spectrum(UINT8 eq_std_flg)
{
	UNUSED_ARGUMENT(eq_std_flg);
	return 0;
}

INT32 AUDIF_Set_EQType(UINT8 eqtype, BYTE *p)
{
    AUDIO_SEMAPHORE_TAKE();

    UINT16 eqH;
    UINT32 eqL;

	switch (eqtype)
	{
	    case EQ_CLASSIC:
	        DSP_SendCmd(0x10,0x01,0x0089,0x0C30C30C);
	        break;
        case EQ_ROCK:
	        DSP_SendCmd(0x10,0x01,0x058A,0x0628E514);
	        break;
	    case EQ_JAZZ:
	        DSP_SendCmd(0x10,0x01,0x0588,0x1120F494);
	        break;
	    case EQ_POP:
	        DSP_SendCmd(0x10,0x01,0x028E,0x1430650E);
	        break;
	    case EQ_DANCE:
	        DSP_SendCmd(0x10,0x01,0x0492,0x0c249411);
	        break;
	    case EQ_TECHNO:
	        DSP_SendCmd(0x10,0x01,0x0490,0x0e30b30b);
	        break;
	    case EQ_SOFT:
	        DSP_SendCmd(0x10,0x01,0x0492,0x10309350);
	        break;
	    case EQ_LIVE:
	        DSP_SendCmd(0x10,0x01,0x024a,0x0a310452);
	        break;
	    case EQ_SATNDARD:
	        DSP_SendCmd(0x10,0x01,0x830C,0x0C30C30C);
	        break;
	    case EQ_USER_DEFINED:
	        eqH = (((UINT16)p[6])<<6) + (UINT16)p[5];
	        eqL = (((UINT32)p[4])<<24) + (((UINT32)p[3])<<18) + (((UINT32)p[2])<<12) + (((UINT32)p[1])<<6) + ((UINT32)p[0]);
	        DSP_SendCmd(0x10,0x01,eqH,eqL);
	        break;
	    default:
	        AUDIO_SEMAPHORE_GIVE();
	        return 0;
	}

	AUDIO_SEMAPHORE_GIVE();
	return 1;
}

INT32 AUDIF_Set_FSPKGain(UINT8 val)
{
	UNUSED_ARGUMENT(val);
	return 0;
}

INT32 AUDIF_Set_Key(UINT8 keyshift)
{

	if ((keyshift>KS_POS_6)&&(keyshift<KS_NEG_6))
	{
		return 0;
	}
	else
	{
		AUDIO_SEMAPHORE_TAKE();
        UINT16 key = 0x0001;         // 1: for keyshift mode
		key |= (((UINT16)keyshift) << 8);
		aud_param.tone_key = keyshift;
		DSP_SendCmd(0x10,0x00,key,0x0);
		DSP_SetRiscParam0(RISC_MAIN_KEY_SHIFT, key);	//lptan add,we need update key shift word 2010-7-5
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

		AUDIO_SEMAPHORE_GIVE();
        	return 1;
	}
}

void AUDIF_APP_ChannelNum( void )
{
	//********************************************
	// check channel number mode for APP
	//********************************************

 	// check the behavior for speaker
	if( aud_param.speaker_info.surround == BM_SPK_OFF )
	{
		DSP_SendCmd( 0x08, LS_SPKGAIN , 0x0, 0x0);
		DSP_SendCmd( 0x08, RS_SPKGAIN , 0x0, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A1_GAIN, 0x0 );
	}
	else
	{
		DSP_SendCmd( 0x08, LS_SPKGAIN , 0x80, 0x0);
		DSP_SendCmd( 0x08, RS_SPKGAIN , 0x80, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A1_GAIN, 0x8080 );
	}

	if( aud_param.speaker_info.center == BM_SPK_OFF )
	{
		DSP_SendCmd( 0x08, C_PKGAIN , 0x0, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A2_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A2_GAIN)&0xff00 );
	}
	else
	{
		DSP_SendCmd( 0x08, C_PKGAIN , 0x80, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A2_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A2_GAIN)|0x0080 );
	}

	if( aud_param.speaker_info.subwoofer == BM_SPK_OFF )
	{
		DSP_SendCmd( 0x08, SW_SPKGAIN , 0x0, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A2_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A2_GAIN)&0x00ff );
	}
	else
	{
		DSP_SendCmd( 0x08, SW_SPKGAIN , 0x80, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A2_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A2_GAIN)|0x8000 );
	}

	if( aud_param.speaker_info.LB == BM_SPK_OFF )
	{
		DSP_SendCmd( 0x08, LB_SPKGAIN , 0x0, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A3_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A3_GAIN)&0x00ff );
	}
	else
	{
		DSP_SendCmd( 0x08, LB_SPKGAIN , 0x80, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A3_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A3_GAIN)|0x8000 );
	}

	if( aud_param.speaker_info.RB == BM_SPK_OFF )
	{
		DSP_SendCmd( 0x08, RB_SPKGAIN , 0x0, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A3_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A3_GAIN)&0xff00 );
	}
	else
	{
		DSP_SendCmd( 0x08, RB_SPKGAIN , 0x80, 0x0);
		DSP_SetRiscParam0(RISC_MAIN_A3_GAIN,  DSP_GetRiscParam0(RISC_MAIN_A3_GAIN)|0x0080 );
	}

}

UINT32 AUDIF_Set_Karaoke_SndEffect( UINT16 mode )
{
/**********************************************************
*	mode [IN]:
*	karaoke_OFF	= 0x0,	// default
*	KSE_flanger = 0x10,
*	KSE_chorus  = 0x20,
*	KSE_tremolo = 0x30,
*	KSE_vibrato = 0x40,
**********************************************************/

	AUDIO_SEMAPHORE_TAKE();

	// enable KSE
	if( mode != KSE_OFF )
		mode |= 0x1;

	DSP_SendCmd(0x09, 0x05, mode, 0x0);
	DSP_SetRiscParam0(RISC_MAIN_KARAOKE_SOUND_EFFECT, mode );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	AUDIO_SEMAPHORE_GIVE();
	return AUDIF_SUCCESS;
}

void AUDIF_Set_PLII_Init_Config(UINT32 val)
{
	AUDIO_SEMAPHORE_TAKE();

	aud_param.pl2_init_val = val;

	AUDDRV_PRINT("AUDDRV : Set PLII Initial Configuration:%x\n",aud_param.pl2_init_val);

	AUDIO_SEMAPHORE_GIVE();

	return;

}

void AUDIF_Set_PLII_Init_Mode(UINT8 mode)
{
	AUDIO_SEMAPHORE_TAKE();

	aud_param.pl2_init_val |= (1<<PLII_ENABLE_BIT);
	aud_param.pl2_init_val &= (~(0x0f<<PLII_MODE_BIT));
	aud_param.pl2_init_val |= ((mode & 0xf)<<PLII_MODE_BIT);

	AUDDRV_PRINT("AUDDRV : Set PLII Initial Config[%x] for Decode mode[%x]\n",aud_param.pl2_init_val,mode);

	AUDIO_SEMAPHORE_GIVE();

	return;

}

INT32 AUDIF_Set_ProLogicII(UINT8 mode)
{

	AUDIO_SEMAPHORE_TAKE();

	UINT8 pl2_func;


	AUDDRV_PRINT("AUDDRV : Set ProLogicII\n");


	pl2_func = mode & PLII_FUNC_MASK;

	switch(pl2_func)
	{
		case PLII_FUNC_ENABLE:
	 		if(PLII_OFF)
				aud_param.pl2_init_val = PROLOGICII_OFF_DSP_INIT;     // set to initial value
			else if(PLII_ON)
				aud_param.pl2_init_val |= (1<<PLII_ENABLE_BIT);       // set enable
			break;
		case PLII_FUNC_MODE:
			aud_param.pl2_init_val &= (~(0x0f<<PLII_MODE_BIT));    //clear mode
			aud_param.pl2_init_val |= ((mode & 0x0f)<<PLII_MODE_BIT);      // set init-val to movie mode
			break;
		case PLII_FUNC_DIMENSION:
			aud_param.pl2_init_val &= (~(0x0f<<PLII_DIMENSION_BIT));   //clear dimension
			aud_param.pl2_init_val |= ((mode & 0x0f)<<PLII_DIMENSION_BIT);    //set dimension
			break;
		case PLII_FUNC_CENTER_WIDTH:
			aud_param.pl2_init_val &= (~(0x07<<PLII_CENTER_WIDTH_BIT));   //clear center width
			aud_param.pl2_init_val |= ((mode & 0x07)<<PLII_CENTER_WIDTH_BIT);     // set center width
			break;
		case PLII_FUNC_PANORAMA:
			aud_param.pl2_init_val &= (~(0x01<<PLII_PANORAMA_BIT));  //clear panorama bit
			aud_param.pl2_init_val |= ((mode & 0x01)<<PLII_PANORAMA_BIT);  ////set panorama bit
			break;
		case PLII_FUNC_CHANNEL:
			aud_param.pl2_init_val &= (~(0x0f<<PLII_CHANNEL_BIT));   //clear channel config
			aud_param.pl2_init_val |= ((mode & 0x0f)<<PLII_CHANNEL_BIT);     //set channel config
			break;
		default:
			break;
	}

	AUDDRV_PRINT("AUDDRV : pl2_init_val :%x\n",aud_param.pl2_init_val);

	if (DSP_SendCmd(RISC_CMD0x11_SW_APP,CODE0x01_NEO6_CMD,0x0000,aud_param.pl2_init_val) == DSP_ERROR_NONE)
	{
		AUDIO_SEMAPHORE_GIVE();
		return 1;
	}
	else
	{
		AUDIO_SEMAPHORE_GIVE();
		return 0;
	}
}


INT32 AUDIF_Set_GM7(UINT16 mode)
{
	AUDIO_SEMAPHORE_TAKE();

	AUDIF_Status_e eStatus = AUDIF_SUCCESS;
	UINT16 SWAPP_ID,SWAPP_PARAM0;

	AUDDRV_PRINT("AUDDRV : Set GM7 mode :%x\n",mode);

	SWAPP_ID = DSP_GetRiscParam0(RISC_SWAPP_ID);
	SWAPP_PARAM0 = DSP_GetRiscParam0(RISC_SWAPP_PARAM0);

	switch(mode)
	{
		case AUDIF_GM7_OFF:
			SWAPP_PARAM0 &= 0x7fff;
			break;
		case AUDIF_GM5_MOVIE:
			SWAPP_PARAM0 |= 0x8000;
			SWAPP_PARAM0 &= 0xfeff;
			break;
		case AUDIF_GM5_MUSIC:
			SWAPP_PARAM0 |= 0x8100;
			break;
		case AUDIF_GM7_Mode3:
			SWAPP_PARAM0 |= 0x8000;
			SWAPP_PARAM0 &= 0xfdff;
			break;
		case AUDIF_GM7_Mode4:
			SWAPP_PARAM0 |= 0x8200;
			break;
	}

	//[ccyang]Enable SWAPP should be in the system ready state
	if( SWAPP_ID != en_gm7 )
	{
		DSP_SetCodecStop(CODEC_BANK_DEC1);
		DSP_SetSystemIdle();
		DSP_SetSystemReady();
		DSP_SetRiscParam0(RISC_SWAPP_ID, en_gm7);
		DSP_SetRiscParam0(RISC_SWAPP_PARAM0, SWAPP_PARAM0 );
		DSP_SetRiscParam0(RISC_SWAPP_PARAM1, 0 );
		DSP_SetRiscParam0(RISC_SWAPP_PARAM2, 0 );
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
		DSP_SetSystemIdle();
		DSP_SetSystemRun();
	}
	else
	{
		DSP_SetRiscParam0(RISC_SWAPP_PARAM0, SWAPP_PARAM0 );
		DSP_SetRiscParam0(RISC_SWAPP_PARAM1, 0 );
		DSP_SetRiscParam0(RISC_SWAPP_PARAM2, 0 );
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	}

	if ( DSP_ERROR_NONE != DSP_SendCmd(RISC_CMD0x11_SW_APP,CODE0x02_GM5_CMD,SWAPP_PARAM0,0x00000000) )
	{
		eStatus = AUDIF_FAILURE;
		AUDDRV_PRINT("[FAIL]AUDDRV:: Set GM5 mode:%x FAIL!\n",mode);
	}

	AUDIO_SEMAPHORE_GIVE();
	return eStatus;

}


UINT8 AUDIF_Init_Neo6(void)
{
    AUDIO_SEMAPHORE_TAKE();
    aud_param.neo6_ch = (1 << AUDIF_Neo6_Ch7);
    aud_param.neo6_mode = AUDIF_Neo6_Cinema;
    aud_param.neo6_cgain = 0x7FFFFFFF;
    AUDIO_SEMAPHORE_GIVE();
    return 1;
}

UINT8 AUDIF_Set_Neo6_Ch(UINT8 ch)
{
    UINT16 config;

    if ((ch < AUDIF_Neo6_Ch3) | (ch > AUDIF_Neo6_Ch7))
        return 0;

    AUDIO_SEMAPHORE_TAKE();
    config = (((UINT16) aud_param.neo6_mode)<<8) | (1 << ch);
    DSP_SendCmd(RISC_CMD0x11_SW_APP,CODE0x01_NEO6_CMD,config,aud_param.neo6_cgain);
    AUDIO_SEMAPHORE_GIVE();
    return 1;
}

UINT8 AUDIF_Set_Neo6_Mode(UINT8 mode)
{
    UINT16 config;

    if ((mode!=AUDIF_Neo6_Music) & (mode!=AUDIF_Neo6_Cinema))
        return 0;

    AUDIO_SEMAPHORE_TAKE();
    config = (((UINT16) mode)<<8) | ((UINT16) aud_param.neo6_ch);
    DSP_SendCmd(RISC_CMD0x11_SW_APP,CODE0x01_NEO6_CMD,config,aud_param.neo6_cgain);
    AUDIO_SEMAPHORE_GIVE();
    return 1;
}

UINT8 AUDIF_Set_Neo6_Gain(UINT8 gain)
{
    UINT16 config;

    AUDIO_SEMAPHORE_TAKE();
    config = (((UINT16) aud_param.neo6_mode)<<8) | ((UINT16) aud_param.neo6_ch);

    switch (gain)
    {
        case AUDIF_Neo6_G1p0:
            aud_param.neo6_cgain = 0x7FFFFFFF;
            break;
        case AUDIF_Neo6_G0p5:
            aud_param.neo6_cgain = 0x40000000;
            break;
        case AUDIF_Neo6_G0p3:
            aud_param.neo6_cgain = 0x26666666;
            break;
        case AUDIF_Neo6_G0p0:
            aud_param.neo6_cgain = 0x00000000;
            break;
        default:
            AUDIO_SEMAPHORE_GIVE();
            return 0;
    }

    DSP_SendCmd(RISC_CMD0x11_SW_APP,CODE0x01_NEO6_CMD,config,aud_param.neo6_cgain);
    AUDIO_SEMAPHORE_GIVE();
    return 1;
}

UINT8 AUDIF_Init_TruVol(void)
{
	// move to aud_InitAudParams()
	aud_param.truvol_gain = 0x80;
	aud_param.truvol_mode = AUDIF_TruVol_Off;

	return 1;
}

UINT8 AUDIF_Set_TruVol_Mode(UINT8 mode)
{
	// [mode] ref: AUDIF_TruVol_e
	UINT16 value;

	aud_param.truvol_mode = mode;
	value = ((UINT16) mode)<<13 | aud_param.truvol_gain;

	AUDIO_SEMAPHORE_TAKE();
	DSP_SetRiscParam0(RISC_TRUVOL_PARAM0, value );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	AUDIO_SEMAPHORE_GIVE();

	return 1;
}

UINT8 AUDIF_Set_TruVol_OutGain(UINT8 gain)
{
	// gain range: 0~20(max)
	UINT16 value;

	if( gain > 20 )
	gain = 20;
	if( gain < 0 )
	gain = 0;

	value = (UINT16)((128*gain)/10);           // Q5.8(0.05)=12.8

	aud_param.truvol_gain = value;

	value = ((UINT16) aud_param.truvol_mode)<<13 | value;

	AUDIO_SEMAPHORE_TAKE();
	DSP_SetRiscParam0(RISC_TRUVOL_PARAM0, value );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	AUDIO_SEMAPHORE_GIVE();

	return 1;
}

INT32 AUDIF_Set_VocalScoring(UINT16 uCmdType)
{
	// uCmdType [IN]: refer karaoke_vocal_e in auddrv.h

 	AUDIO_SEMAPHORE_TAKE();

	DSP_SendCmd(0x09, 0x04, uCmdType, 0x0);
	DSP_SetRiscParam0(RISC_MAIN_KARAOKE_VOCAL_SCORE, uCmdType );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	AUDIO_SEMAPHORE_GIVE();

	return 1;
}

INT32 AUDIF_Set_AC3Converter( UINT16 mode )
{
	// Note: this API NEED to be called before download DDP decoder
	AUDIO_SEMAPHORE_TAKE();
	
	switch( mode )
	{
		case DDP_Converter_Disable:
			aud_param.DDP_Converter_Mode = mode;
			AUDDRV_PRINT("AUDIF_Set_AC3Converter: Disable\n");
			break;
		case DDP_Converter_Enable:
			aud_param.DDP_Converter_Mode = mode;
			AUDDRV_PRINT("AUDIF_Set_AC3Converter: Enable\n");
			break;
		default:
			diag_printf("[Err]AUDIF_Set_AC3Converter unknown\n");
			AUDIO_SEMAPHORE_GIVE();
			return AUDIF_ERR_INVALID_PARAM;
	}

	AUDIO_SEMAPHORE_GIVE();
	
	return AUDIF_SUCCESS;
}

void AUDIF_Set_Speaker_Config(UINT8 spktype , UINT8 setupvalue)
{
    AUDIO_SEMAPHORE_TAKE();
	switch (spktype)
	{
	    case BM_SPK_FRONT:
	        aud_param.speaker_info.front = setupvalue;
	        aud_param.speaker_info.LF= setupvalue;
	        aud_param.speaker_info.RF= setupvalue;
	        break;
	    case BM_SPK_SURR:
	        aud_param.speaker_info.surround = setupvalue;
	        aud_param.speaker_info.back = setupvalue;
	        aud_param.speaker_info.LS= setupvalue;
	        aud_param.speaker_info.RS= setupvalue;
	        aud_param.speaker_info.LB= setupvalue;
	        aud_param.speaker_info.RB= setupvalue;
	        break;
	    case BM_SPK_C:
	        aud_param.speaker_info.center = setupvalue;
	        break;
	    case BM_SPK_SUB:
	        aud_param.speaker_info.subwoofer = setupvalue;
	        break;
	    case BM_SPK_LF:
	        aud_param.speaker_info.LF= setupvalue;
	        break;
	    case BM_SPK_RF:
	        aud_param.speaker_info.RF= setupvalue;
	        break;
	    case BM_SPK_LS:
	        aud_param.speaker_info.LS= setupvalue;
	        break;
	    case BM_SPK_RS:
	        aud_param.speaker_info.RS= setupvalue;
	        break;
	    case BM_SPK_LB:
	        aud_param.speaker_info.LB= setupvalue;
	        break;
	    case BM_SPK_RB:
	        aud_param.speaker_info.RB= setupvalue;
	        break;
	}
	AUDIO_SEMAPHORE_GIVE();
}
INT32 AUDIF_Set_SpkDelay(void)
{
    AUDIO_SEMAPHORE_TAKE();
	UINT32 delayL;
	UINT16 delayS;

	// Version B
	delayS = ((UINT16)aud_param.mspk_delay.RB<<10) + ((UINT16)aud_param.mspk_delay.SUB<<5) + (UINT16)aud_param.mspk_delay.C;
	delayL = (((UINT32)aud_param.mspk_delay.LB)<<25) +
	         (((UINT32)aud_param.mspk_delay.RS)<<20) +
	         (((UINT32)aud_param.mspk_delay.LS)<<15) +
	         (((UINT32)aud_param.mspk_delay.RF)<<10) +
	         (((UINT32)aud_param.mspk_delay.LF)<<5) +
	         ((UINT32)aud_param.mspk_delay.LMRM);
#if 0       // Version A
    delayS = 0;
    delayL = (((UINT32)aud_param.mspk_delay.LB)<<24) +
	         (((UINT32)aud_param.mspk_delay.C)<<16) +
	         (((UINT32)aud_param.mspk_delay.LS)<<8) +
	         ((UINT32)aud_param.mspk_delay.LF);
#endif

	DSP_SendCmd(0x10,0x04,delayS,delayL);
	AUDIO_SEMAPHORE_GIVE();
	return 1;
}

INT32 AUDIF_Set_SpkDelay_Config(UINT8 spk, UINT16 delay)
{
    AUDIO_SEMAPHORE_TAKE();
	switch (spk)
	{
	    case BM_SPK_LF:
	        if (delay>17)
	            return 0;
	        aud_param.mspk_delay.LF = delay;
	        break;
	    case BM_SPK_RF:
	        if (delay>17)
	            return 0;
	        aud_param.mspk_delay.RF = delay;
	        break;
	    case BM_SPK_LS:
	        if (delay>27)
	            return 0;
	        aud_param.mspk_delay.LS = delay;
	        break;
	    case BM_SPK_RS:
	        if (delay>27)
	            return 0;
	        aud_param.mspk_delay.RS = delay;
	        break;
	    case BM_SPK_LB:
	        if (delay>37)
	            return 0;
	        aud_param.mspk_delay.LB = delay;
	        break;
	    case BM_SPK_RB:
	        if (delay>37)
	            return 0;
	        aud_param.mspk_delay.RB = delay;
	        break;
	    case BM_SPK_C:
	        if (delay>17)
	            return 0;
	        aud_param.mspk_delay.C = delay;
	        break;
	    case BM_SPK_SUB:
	        if (delay>17)
	            return 0;
	        aud_param.mspk_delay.SUB = delay;
	        break;
	    case BM_SPK_LMRM:
	        if (delay>17)
	            return 0;
	        aud_param.mspk_delay.LMRM = delay;
	        break;
	    default:
	        AUDIO_SEMAPHORE_GIVE();
	        return 0;
	}
	AUDIO_SEMAPHORE_GIVE();
	return 1;
}

void AUDIF_Init_SpkDelay_Config(void)
{
	AUDIO_SEMAPHORE_TAKE();
	aud_param.mspk_delay.LF = 0;
	aud_param.mspk_delay.RF = 0;
	aud_param.mspk_delay.LS = 0;
	aud_param.mspk_delay.RS = 0;
	aud_param.mspk_delay.C = 0;
	aud_param.mspk_delay.SUB = 0;
	aud_param.mspk_delay.LB = 0;
	aud_param.mspk_delay.RB = 0;
	aud_param.mspk_delay.LMRM = 0;
	AUDIO_SEMAPHORE_GIVE();
}

INT32 AUDIF_Set_SSPKGain(UINT8 val)
{
	UNUSED_ARGUMENT(val);
	return 0;
}

void AUDIF_Set_SubWoof(UINT8 ustatus)
{
    AUDIO_SEMAPHORE_TAKE();
    UINT8 state;

    state = ustatus;
    if (state == STATUS_OFF)
        state = BM_SPK_OFF;
	AUDIF_Set_Speaker_Config(BM_SPK_SUB,state);
	AUDIO_SEMAPHORE_GIVE();
}

INT32 AUDIF_Set_Surround(UINT8 stype)
{
	if (stype>9)        // support 9 modes
	    return 0;

	AUDIO_SEMAPHORE_TAKE();
	DSP_SendCmd(0x10,0x02,(UINT16)stype,0x0);
	AUDIO_SEMAPHORE_GIVE();
	return 1;
}

INT32 AUDIF_Set_Volume(UINT8 x)
{
#if 1 // remove hw gain control for 9200
	return 1;
#else
    UINT32 Master_Gain;
    AUDIO_SEMAPHORE_TAKE();

#ifdef VOLUMN_TAB_50
    // [dylan] 20101214 there are 100 steps of volumn control on AP for 9200 tv usage
    // so a 50-tapes volumn table is created in auddrv
    x = x>>1;
#endif

    aud_param.volume = x;
    // Master_Gain is 24bits, VolTab[x] is 8 bits(0~127)
    if(aud_param.mute==1)
    {
        Master_Gain = 0;
    }
    else
    {
    	if( x<=0 )
		    Master_Gain = 0;
        else if( x >= (VOLUME_SCALE-1) )
		    Master_Gain = ((VolTab_qae377[VOLUME_SCALE-1]<<24));
		else
       	    Master_Gain = ((VolTab_qae377[x]<<24)| 0x00FFFF00);
    }

    DSP_SendCmd(0x08, 0x00, 0x0000, Master_Gain);

	// ccyang: set RISC parameter to DRAM
#ifdef USE_RISC_PARSER_PARAM
	DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_H, Master_Gain>>16);
	DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_L, Master_Gain&0xffff);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
#endif

    AUDIO_SEMAPHORE_GIVE();
    return 1;
#endif	
}


static const char* sgCodecStringTbl[] =
{
	"MAIN",        // 0x00 : CODECID: ID_MAIN
	"PCM",         // 0x01 : CODECID: ID_DECODER_PCM
	"DD+",         // 0x02 : CODECID: ID_DECODER_DDP
	"TRUEHD",      // 0x03 : CODECID: ID_DECODER_TRUEHD
	"DTSLBR",      // 0x04 : CODECID: ID_DECODER_DTSLBR
    "DTSHD",       // 0x05 : CODECID: ID_DECODER_DTSHD
	"MPEG",        // 0x06 : CODECID: ID_DECODER_MPEG
	"AAC",         // 0x07 : CODECID: ID_DECODER_AAC
	"WMA",         // 0x08 : CODECID: ID_DECODER_WMA
	"RA",          // 0x09 : CODECID: ID_DECODER_RA
	"OGG",         // 0x0A : CODECID: ID_DECODER_OGG
	"EMU",         // 0x0B : CODECID: ID_DECODER_EMU
	"WMAPRO",      // 0x0C : CODECID: ID_DECODER_WMAPRO
	"???",		   // 0x0D :
	"???",		   // 0x0E :
	"???",		   // 0x0F :
	"MPEG ENC",    // 0x10 : CODECID: ID_ENCODER_MPEG
	"AC3 ENC",     // 0x11 : CODECID: ID_ENCODER_AC3
	"DTS ENC",     // 0x12 : CODECID: ID_ENCODER_DTS
};



void AUDIF_DSP_GetCodecID( char* pBuf , UINT8 id)
{
	if (id <= ID_MAX_CODECID)
		strcpy( pBuf, sgCodecStringTbl[id]);
	else
		strcpy( pBuf, "???");
		return;

}

void AUDIF_DSP_GetCodecVer( char* pBuf , UINT8 id)
{
	UNUSED_ARGUMENT(id);

	UINT32 mainID;
	UINT32 dec1ID;
	UINT32 codecVersion;

	mainID = (DSP_GetDspParam0(DSP_MAIN_ID)<<16)|DSP_GetDspParam0(DSP_MAIN_VERSION);
	dec1ID = (DSP_GetDspParam0(DSP_DEC1_ID)<<16)|DSP_GetDspParam0(DSP_DEC1_VERSION);
	codecVersion = dec1ID & 0xFFFFFF;

    AUDDRV_PRINT("AUDIF_DSP_GetCodecVer::mainID:%08x, dec1ID:%08x, codecIndex:%08x\n",mainID,dec1ID,codecVersion);

	sprintf(pBuf, "%06x", codecVersion);

    AUDDRV_PRINT("AUDIF_DSP_GetCodecVer::%s\n",pBuf);

}

INT32	AUDIF_Get_CDDAType(void)
{

    AUDDRV_PRINT("AUDIF_Get_CDDAType::\n");

	UINT16  idCDDA;

	// AUDIF_CODING_MODE_PCM

	if (AUDIF_CODING_MODE_PCM != aud_param.coding_mode_now)
	{
		    AUDDRV_PRINT("AUDIF_Get_CDDAType::wrong coding mode:%x\n",aud_param.coding_mode_now);
			return 0;
	}

	idCDDA = AUDIF_Get_DspParam(DSP_DEC1_PCM_HDCD_FLAG);

    AUDDRV_PRINT("AUDIF_Get_CDDAType::idCDDA:%x\n",idCDDA);

	//#define K_HDCD_ID		0x0102 // 0x7E0FA0

	if (idCDDA == 1)
		return CDtype_HDCD;
	else
		return CDtype_CDDA;
}

void AUDIF_Set_DTSCD_Type(BYTE val)
{
	// refer disc_DTSCDtype_e
    AUDDRV_PRINT("AUDIF_Set_DTSCD_Type::%d\n", val);

	if( val >= Error_DTSCD_type )
	{
		AUDDRV_PRINT("[Err]AUDIF_Set_DTSCD_Type::Error type\n");
		return;
	}
	else
	{
		aud_param.DTSCD_type = val;
		DSP_SetRiscParam0(RISC_DEC1_DTSCD_Type, val );
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	}
}

UINT32 AUDIF_Get_Final_Score(void)
{
	UINT32 score;
	score = (UINT32)DSP_GetDspParam0(DSP_MAIN_SCORE_FINAL);
	return score;
}

UINT32 AUDIF_Get_Mic_Buf_Fullratio(void)
{
	return 0;
}

UINT32 AUDIF_Get_Mic_EQ_Spectrum(UINT8 band)
{
	UNUSED_ARGUMENT(band);
	return 0;
}

UINT32 AUDIF_Get_MPEG_AAC_Bitrate(void)
{

	UINT16 bitrate = AUDIF_Get_DspParam(DSP_DEC1_BITRATE);

	AUDDRV_PRINT("AUDIF_Get_MPEG_AAC_Bitrate::bitrate:%d kbps\n",bitrate);
	return bitrate;

}


//*******************************************************
//	Media Initialization API
//*******************************************************
INT32 AUDIF_Init_Wave_Audio(WAVFORMATMACRO *wfmt1)
{
	AUDDRV_PRINT("AUDIF_Init_Wave_Audio::Init_Wave_Audio\n");

    memcpy(&gWavFmt, wfmt1, sizeof(gWavFmt));

#if 0 // remove hw gain control for 9200
	//DSP_InitRiscParams0();
	UINT32 Mgain= DSP_SetMasterGAIN();
	DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_H, Mgain>>16);
	DSP_SetRiscParam0(RISC_MAIN_MASTER_GAIN_L, Mgain&0xffff);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
#endif

	// set channel output mode configuration
	AUDIF_Set_SpkOutMode();
	AUDIF_APP_ChannelNum();

	AUDDRV_PRINT("AUDIF_Init_Wave_Audio::wFormatTag     :0x%x\n",gWavFmt.wfx.wFormatTag);
	AUDDRV_PRINT("AUDIF_Init_Wave_Audio::nChannels      :0x%x\n",gWavFmt.wfx.nChannels);
    AUDDRV_PRINT("AUDIF_Init_Wave_Audio::nSamplesPerSec :0x%x\n",gWavFmt.wfx.nSamplesPerSec);
    AUDDRV_PRINT("AUDIF_Init_Wave_Audio::nAvgBytesPerSec:0x%x\n",gWavFmt.wfx.nAvgBytesPerSec);
    AUDDRV_PRINT("AUDIF_Init_Wave_Audio::nBlockAlign    :0x%x\n",gWavFmt.wfx.nBlockAlign);
    AUDDRV_PRINT("AUDIF_Init_Wave_Audio::wBitsPerSample :0x%x\n",gWavFmt.wfx.wBitsPerSample);
    AUDDRV_PRINT("AUDIF_Init_Wave_Audio::cbSize         :0x%x\n",gWavFmt.wfx.cbSize);

#if 0	// ccyang
	// set sample rate to Audio HW
	R_SmpRate= aud_map_SmpRate_IDtoFS((UINT16)SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec));
	AUDIF_Set_SampleRate(R_SmpRate);
#endif

#ifdef AUD_SUPPORT_HDMI
	AUDIF_SetAudParam_to_HDMI(HDMI_PCM);
#endif

	switch (gWavFmt.wfx.wFormatTag)
	{
		case WAVE_FORMAT_BD_LPCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::lpcm vob audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_PCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::pcm audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			AUDIF_LPCM_Init_Parm(WAVE_FORMAT_PCM,wfmt1);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_BigEndian_LPCM: // big endian lpcm
		case WAVE_FORMAT_ULaw_LPCM://uLaw
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::pcm audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_DVD_LPCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::DVD LPCM audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_CD_LPCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::CD LPCM audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			AUDIF_LPCM_Init_Parm(WAVE_FORMAT_CD_LPCM,wfmt1);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_IMA_ADPCM:	//for support ASF IMA ADPCM
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::ima-adpcm audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_ADPCM:	//for support MS ADPCM
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::MS ADPCM audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_LPCM);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_LPCM);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_MPEG:
		case WAVE_FORMAT_MPEGLAYER3:
            		AUDDRV_PRINT("AUDIF_Init_Wave_Audio::mpg audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_MP3);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_MP3);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_DOLBY_DIGITAL_PLUS:
		case WAVE_FORMAT_DOLBY_AC3:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::ac3 audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_AC3);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_AC3);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_DOLBY_DTS:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::dts audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_DTS);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_DTS);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_WMA_STEREO:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::wma audio\n");
            AUDIF_Set_CodingMode(AUDIF_CODING_MODE_WMA);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_WMA);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_MPEG2AAC:
        case WAVE_FORMAT_AAC:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::aac audio\n");
			aud_param.dec_param.aac_mode = AAC_MODE_RAW;
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_AAC);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_AAC);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_WMA_PRO:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::wma pro\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_WMAPRO);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_WMA);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_DTSHD:
		case WAVE_FORMAT_DTSHD_MASTER_AUDIO:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::dts hd\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_DTSHD);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_DTS);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_TRUEHD:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::truehd\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_TRUEHD);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_DolbyTrueHD);
			PLAT_REG_SetAudioFormatValid();
			break;
	 	case WAVE_FORMAT_OGG_VORBIS:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::ogg vorbis\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_OGG);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_OGG);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_REAL_AUDIO:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::real audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_RA);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_RA);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_NOISE_GEN:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::noise generator\n");
			AUDIF_TestTone_mode(0); // 0:sine/ 1:pink/ 2:white
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_EMU);
			//PSR setting
			PLAT_REG_SetAudioFormat(PSREG_AUDIO_MODE_EMU);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_FLAC:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::FLAC audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_FLAC);
			break;
		case WAVE_FORMAT_APE:
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::APE audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_APE);
			PLAT_REG_SetAudioFormatValid();
			break;
		case WAVE_FORMAT_AVS:
			AUDDRV_PRINT("AUDIF_Init_Wave_Audio::AVS audio\n");
			AUDIF_Set_CodingMode(AUDIF_CODING_MODE_AVS);
			PLAT_REG_SetAudioFormatValid();
			break;

		default:
            AUDDRV_PRINT("AUDIF_Init_Wave_Audio::Unknown audio format\n");
			return AUDIF_ERR_Unknown_CodecID;
	}

#if (PRODUCT_CAT != DTV)
// mantis 99274
#ifdef AUD_SUPPORT_HDMI
	HDMI_IF_Set_AudioDisabled();
#endif

#endif
	UINT16 R_SmpRate;
	// set sample rate to Audio HW
	R_SmpRate= aud_map_SmpRate_IDtoFS((UINT16)SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec));
	AUDIF_Set_SampleRate(R_SmpRate);
#if (PRODUCT_CAT != DTV)
#ifdef AUD_SUPPORT_HDMI
	AUDIF_SetAudParam_to_HDMI(aud_param.hdmi_mode);
	GL_DelayUs(5000); // fix mantis 94852
	HDMI_IF_Set_AudioEnabled();
#endif
#endif
	AUDIF_Set_SPDIF(aud_param.spdif_mode);
	return AUDIF_SUCCESS;

}

void AUDIF_RA_Init_Parm( RAWAVEFORMAT *wRAfmt)
{

	UINT16 codecParams[16];
	int i=0;

	AUDDRV_PRINT("#####AUDIF_RA_Init_Parm ######\n\n");
	
	AUDIF_Set_Stop( AUDIF_BANK_DEC1 );

	AUDIO_SEMAPHORE_TAKE();

	memcpy(&(gWavFmt.wfx), &(wRAfmt->wfx) , sizeof(gWavFmt.wfx));

	// Prepare codec parameters (no need to call WORD16 to translate to BE)
	for (i=0; i<K_CODEC_PARAMS; i++)
	{
		codecParams[i] = 0;
	}

	if( aud_param.coding_mode_now == AUDIF_CODING_MODE_RA )
	{
		codecParams[0] =  ID_DECODER_RA;
		codecParams[1] =  wRAfmt->nSamples;
		codecParams[2] =  wRAfmt->nFrameBits;
		codecParams[3] =  gWavFmt.wfx.nSamplesPerSec;
		codecParams[4] =  gWavFmt.wfx.nChannels;
		codecParams[5] =  wRAfmt->nRegions;
		codecParams[6] =  wRAfmt->cplStart;
		codecParams[7] =  wRAfmt->cplQbits;
		codecParams[15] = 0x8;

		DSP_WriteCodecParams(CODEC_BANK_DEC1, codecParams);
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	}
	else
	{
		codecParams[0] =  ID_DECODER_RA;
		codecParams[1] =  wRAfmt->nSamples;
		codecParams[2] =  wRAfmt->nFrameBits;
		codecParams[3] =  gWavFmt.wfx.nSamplesPerSec;
		codecParams[4] =  gWavFmt.wfx.nChannels;
		codecParams[5] =  wRAfmt->nRegions;
		codecParams[6] =  wRAfmt->cplStart;
		codecParams[7] =  wRAfmt->cplQbits;
		codecParams[15] = 0x8;

		DSP_WriteCodecParams(CODEC_BANK_DEC1, codecParams);
		DSP_InitCodec(CODEC_BANK_DEC1);
		aud_param.coding_mode_now = AUDIF_CODING_MODE_RA;
	}

	AUDIO_SEMAPHORE_GIVE();

	UINT16 R_SmpRate;
	R_SmpRate= aud_map_SmpRate_IDtoFS((UINT16)SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec));
	AUDIF_Set_SampleRate(R_SmpRate);

	AUDIF_Set_Play( AUDIF_BANK_DEC1 );

}

INT32 AUDIF_AVS_Init_Param( UINT8 BankID,  AVS_DS AVS_Dec_Info )
{
	AUDIO_SEMAPHORE_TAKE();

	AUDDRV_PRINT("\n##### AUDIF_AVS_Init_Param ######\n");

	switch( BankID )
	{
		case AUDIF_BANK_DEC1:
			aud_param.AVS_Info.bs_type             = AVS_Dec_Info.bs_type;
			aud_param.AVS_Info.output_channel_mode = AVS_Dec_Info.output_channel_mode;
			aud_param.AVS_Info.nRaw_Block          = AVS_Dec_Info.nRaw_Block;
			break;
		case AUDIF_BANK_DEC2:
			//aud_param.AVS_Info.bs_type             = AVS_Dec_Info.bs_type;
			//aud_param.AVS_Info.output_channel_mode = AVS_Dec_Info.output_channel_mode;
			//aud_param.AVS_Info.nRaw_Block          = AVS_Dec_Info.nRaw_Block;
			break;
		default:
			aud_param.AVS_Info.bs_type             = AVS_BsType_RAW;
			aud_param.AVS_Info.output_channel_mode = AVS_Dual_mode;
			aud_param.AVS_Info.nRaw_Block          = 0;
			break;			
	}

	AUDIO_SEMAPHORE_GIVE();

	return AUDIF_SUCCESS;
}

int AUDIF_Updata_AAC_Param( UINT32 val )
{
	// val: the value of sample rate(ex: 44100)
	
	AUDIF_Status_e eStatus = AUDIF_SUCCESS;

	AUDIO_SEMAPHORE_TAKE();	

	if( aud_param.coding_mode_now == AUDIF_CODING_MODE_AAC )
	{
		UINT16 nsmprate;

		gWavFmt.wfx.nSamplesPerSec = val;

		nsmprate = (UINT16)SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec);

		DSP_SetRiscParam0( RISC_DEC1_PARAMS_START+2, nsmprate );
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	}
	else
	{
		eStatus = AUDIF_ERR_Unknown_CodecID;
	}

	AUDIO_SEMAPHORE_GIVE();

	return eStatus;

}


void AUDIF_Set_EncoderDSP(UINT16 endcode_mode)
{
//RISC_ENC_BITRATE_ID¡G
// mpeg1: 0x01 ~ 0x0E
// 128(default),32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 kbps
// mpeg2: 0x10 ~ 0x1D
// 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160   kbps

	UINT16 codecParams[16];
	UINT16 i=0;

	//memset(codecParams,0,sizeof(codecParams)*sizeof(UINT16));

	for( i=0; i<16 ; i++)
		codecParams[i] = 0;

	switch(endcode_mode)
	{
		case WAVE_FORMAT_Encode_MPEG:
			codecParams[0] = ID_ENCODER_MPEG;
			codecParams[1] = 0;//encoder_mode;
			codecParams[2] = SampleRate_Mapping(aud_param.encoder_sample_rate);
			codecParams[3] = aud_param.encoder_bit_rate;
			codecParams[4] = 0;//encode_chn_num;
			codecParams[5] = 0;//MPEG_layer;
			codecParams[6] = aud_param.encoder_speed;//recoding_mode;
			break;

		case WAVE_FORMAT_Encode_AC3:
			codecParams[0] = ID_ENCODER_AC3;
            codecParams[1] = 1;//encoder_mode = Mixer_PCMInfo;
			break;
		case WAVE_FORMAT_Encode_DTS:
			codecParams[0] = ID_ENCODER_DTS;
			break;
	}

	codecParams[15] = 0x8; // frame cnt

	// download dsp code
	DSP_WriteCodecParams(CODEC_BANK_ENC,codecParams);
	DSP_InitCodec(CODEC_BANK_ENC);

	UINT32 encID=0;
	encID = (DSP_GetDspParam0(DSP_ENC_ID)<<16)|DSP_GetDspParam0(DSP_ENC_VERSION);
	DSP_PRINT("InitEncoderMode::encID:0x%08x\n\n",encID);


}

void AUDIF_Set_Encode_Bitrate(UINT32 kBitrate)
{
	//AUDDRV_PRINT("AUDIF_Set_Encode_Bitrate::kBitrate:0x%x\n",kBitrate);
    aud_param.encoder_bit_rate = kBitrate;

	return;
}

void AUDIF_Set_Encode_Fs(UINT32 samprate)
{
    //AUDDRV_PRINT("AUDIF_Set_Encode_Fs::samprate:0x%x\n",samprate);
    aud_param.encoder_sample_rate = samprate;

	return;
}

void AUDIF_Set_Encode_Mode(UINT8 uCmdType)
{
	UNUSED_ARGUMENT(uCmdType);
    //AUDDRV_PRINT("AUDIF_Set_Encode_Mode::uCmdType:0x%x\n",uCmdType);
    //aud_param.encoder_mode = uCmdType;
	return;
}

void AUDIF_Set_Encode_Speed(UINT8 uCmdType)
{
	// 0: normal (1x)
	// 1: fast (4x)
	AUDDRV_PRINT("AUDIF_Set_Encode_Speed::uCmdType:0x%x\n",uCmdType);

	DSP_SetRiscParam0(RISC_ENC_SPEED_MODE, (UINT16)uCmdType );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	aud_param.encoder_speed = uCmdType;
	return;
}

BYTE* AUDIF_Get_Encode_BSbuf_start(void)
{
	return (BYTE*)DSP_GetBsBufStart(BSBUF_ID_ENC);
}

void AUDIF_Reset_Audio_Encode_Buf(void)
{
	AUDDRV_PRINT("AUDIF_Set_Encode_Mode::fix me\n");
	AUDIF_Set_Encode_BarrierPtr(0);
	AUDIF_Set_Encode_DumpPtr(0);
}

UINT32 AUDIF_Get_Encode_BarrierPtr(void)
{
	AUDIO_SEMAPHORE_TAKE();
	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(BSBUF_ID_ENC);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Get_Encode_BarrierPtr::e:pBsBufInfo NULL\n");
		AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

//	AUDDRV_PRINT("AUDIF_Get_Encode_BarrierPtr::0x%x(%d)\n",pBsBufInfo->barrPtr,pBsBufInfo->barrPtr);

	AUDIO_SEMAPHORE_GIVE();
	return (pBsBufInfo->barrPtr);
}

UINT32 AUDIF_Get_Encode_DumpPtr(void)
{
	AUDIO_SEMAPHORE_TAKE();

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(BSBUF_ID_ENC);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Get_Encode_DumpPtr::e:pBsBufInfo NULL\n");
		AUDIO_SEMAPHORE_GIVE();
		return 0;
	}

//	AUDDRV_PRINT("AUDIF_Get_Encode_DumpPtr::0x%x(%d)\n",pBsBufInfo->dumpPtr,pBsBufInfo->dumpPtr);

	AUDIO_SEMAPHORE_GIVE();
	return (pBsBufInfo->dumpPtr);
}

void AUDIF_Set_Encode_BarrierPtr(UINT32 val)
{
	AUDIO_SEMAPHORE_TAKE();

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(BSBUF_ID_ENC);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Set_Encode_BarrierPtr::e:pBsBufInfo NULL\n");
		AUDIO_SEMAPHORE_GIVE();
		return;
	}

//	AUDDRV_PRINT("AUDIF_Set_Encode_BarrierPtr::dump:0x%x, val:0x%x\n",pBsBufInfo->dumpPtr,val);
	DSP_SetBarrierPtr(BSBUF_ID_ENC, val);

	AUDIO_SEMAPHORE_GIVE();
	return;
}

void AUDIF_Set_Encode_DumpPtr(UINT32 val)
{
	AUDIO_SEMAPHORE_TAKE();

	BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(BSBUF_ID_ENC);

	if (NULL == pBsBufInfo)
	{
		ERROR_PRINT("AUDIF_Set_Encode_DumpPtr::e:pBsBufInfo NULL\n");
		AUDIO_SEMAPHORE_GIVE();
		return;
	}

//	AUDDRV_PRINT("AUDIF_Set_Encode_DumpPtr::dump:0x%x, val:0x%x\n",pBsBufInfo->dumpPtr,val);
	DSP_SetDumpPtr(BSBUF_ID_ENC, val);

	AUDIO_SEMAPHORE_GIVE();
	return;
}

INT32 AUDIF_Enable_AC3TransCoding( void )
{
	// init encoder mode to avoid download encoder failed 
	// when mainprogram state changed.
	aud_param.encoding_mode_now = 0;
    aud_param.Decoder_RunTimeOPT = 1; //not support AAC SBR mode
	
	AUDIF_Set_EncoderDSP(WAVE_FORMAT_Encode_AC3);
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Get_SampleCnt( UINT8 BankID )
{
	INT32 rSampleCnt=0;

	switch(BankID)
	{
		case AUDIF_BANK_DEC1:
			rSampleCnt = (INT32)HWREG_R(dsp_port[15]);
			break;
		case AUDIF_BANK_DEC2:
			rSampleCnt = (INT32)HWREG_R(dsp_port[16]);
			break;
		case AUDIF_BANK_ENC:
			rSampleCnt = (INT32)HWREG_R(dsp_port[17]);
			break;
		default:
			rSampleCnt = AUDIF_ERR_BankID;
			break;
	}

	return rSampleCnt;

}

//*******************************************************
//
//	1500 STB related Control
//
//*******************************************************
UINT16 AUDIF_Get_ChannelStatus( void )
{
#if 0
	AUDIO_SEMAPHORE_TAKE();

	UINT16 val=0;

	val = DSP_GetDspParam(DSP_DEC1_MPEG_CHAN_MODE);

	//AUDDRV_PRINT("AUDIF_Get_ChannelStatus:: %d\n",val);

	AUDIO_SEMAPHORE_GIVE();

	if( val == 0 )
		return Stereo;
	else if( val == 1 )
		return Joint_stereo;
	else if( val == 2 )
		return Dual_channel;
	else if( val == 3 )
		return Single_channel;
	else
		return AUDIF_FAILURE;
#endif

	AUDIO_SEMAPHORE_TAKE();
	
	UINT16 stereo_mode = DSP_GetDspParam0(DSP_DEC1_CHANNEL_MASK);
	UINT16 val = 0;
	
	switch( stereo_mode )
	{
		case 0x0003:
			val = Stereo;
			break;
			
		case 0x1003:
			val = Joint_stereo;
			break;
			
		case 0x2003:
			val = Dual_channel;
			break;	
					
		case 0x0001:
			val = Single_channel; // mono
			break;
			
		default:
			val = Stereo;
			break;
	}
	
	AUDIO_SEMAPHORE_GIVE();
	
	return val;

}
//*******************************************************
//	 Audio IEC0 spdif delay related APIs
//*******************************************************
UINT16 AUDIF_Get_SPDIF_Fs(void)
{
	UINT16 spdif_smp_rate=48000;
	UINT16 iec_fs=DSP_GetRiscParam0(RISC_MAIN_IEC0_FS);

	switch(iec_fs)
	{
		case 0x10 :
			spdif_smp_rate=48000;
			break;
	}
	AUDDRV_PRINT("AUDIF_Get_SPDIF_Fs::spdif smpRate:%d\n",spdif_smp_rate);
	return spdif_smp_rate;
}

UINT32 AUDIF_EnableIEC0Delay( UINT8 mode )
{
/********************************************************
*	Enable iec0 delay feature for STB
*	mode: IEC0Delay_mode_Enable = 1
*		  IEC0Delay_mode_Disable = 0
*********************************************************/
	UINT16 val;

	// bit0 is used for mixer mode
	// bit1 is used for iec0 delay mode
	val = DSP_GetRiscParam0(RISC_MAIN_MIXER_SELECT);

	if( mode == IEC0Delay_mode_Enable )
	{
		val = ( val | (1<<1) );
		DSP_SetRiscParam0( RISC_MAIN_MIXER_SELECT , val );
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

		// update RISC parameter by change bank0 state
		DSP_SetSystemIdle();
		DSP_SetSystemReady();
		DSP_SetSystemIdle();
		DSP_SetSystemRun();

		// reset dec1 for update dsp internal flow
		aud_SetCodecParams(aud_param.coding_mode_now);
		DSP_InitCodec(CODEC_BANK_DEC1);

		if((val & 1) == 1)
		{
			aud_SetDec2CodecParams(aud_param.coding_mode_now);
			DSP_InitCodec(CODEC_BANK_DEC2);
		}
	}
	else
	{
		val = ( val & ~(1<<1) );

		DSP_SetRiscParam0( RISC_MAIN_MIXER_SELECT , val );
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
		// update RISC parameter by change bank0 state
		DSP_SetSystemIdle();
		DSP_SetSystemReady();
		DSP_SetSystemIdle();
		DSP_SetSystemRun();

		// reset dec1 for update dsp internal flow
		aud_SetCodecParams(aud_param.coding_mode_now);
		DSP_InitCodec(CODEC_BANK_DEC1);

		if((val & 1) == 1)
		{
			aud_SetDec2CodecParams(aud_param.coding_mode_now);
			DSP_InitCodec(CODEC_BANK_DEC2);
		}
	}
	return AUDIF_SUCCESS;
}
UINT32 AUDIF_Get_IEC0_Delay(void)
{
	UINT16 spdif_smp_rate=AUDIF_Get_SPDIF_Fs();
	UINT32 iec_delay=DSP_GetRiscParam0(RISC_MAIN_IEC0_DELAY_SAMPLE);
	//AUDDRV_PRINT("iec_delay:%d\n",iec_delay);
	iec_delay=(iec_delay*1000/spdif_smp_rate);
	AUDDRV_PRINT("AUDIF_Get_IEC0_Delay::delay time(ms):%d\n",iec_delay);
	return iec_delay;
}

void AUDIF_Set_IEC0_Delay(UINT16 msec)
{
	AUDDRV_PRINT("AUDIF_Set_IEC0_Delay::delay time(ms):%d\n",msec);

	UINT32 smp_rate=48000;
	UINT32 smp_delay=0;

	if(msec > 250)
	{
		return;
	}
	else
	{
		smp_rate=AUDIF_Get_SPDIF_Fs(); //get spdif smp rate
		if(msec >= AUDIF_Get_IEC0_Delay())
		{
			smp_delay = msec*smp_rate / 1000;
		}
		else if(msec < AUDIF_Get_IEC0_Delay())
		{
			//FIFO reset
			if(AUDIF_SUCCESS == AUDIF_Set_FIFOReset())
				smp_delay = msec*smp_rate / 1000;
		}
	}
	if(smp_delay > 0x2f00)
		smp_delay=0x2f00;

	DSP_SetRiscParam0(RISC_MAIN_IEC0_DELAY_SAMPLE, smp_delay);
	AUDDRV_PRINT("AUDIF_Set_IEC0_Delay::smp_delay=%x\n", smp_delay);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return;
}


//*******************************************************
//
//	Daul DSP Mixer related Control
//
//*******************************************************
UINT32 AUDIF_EnableDualDSP( UINT8 mode )
{
/********************************************************
*	Enable decoder2 mode if support for STB
*	mode: DualDSP_mode_Enable = 1
*		  DualDSP_mode_Disable = 0
*	Note: NEED download 2nd dsp after EnableDualDSP
*********************************************************/
	UINT16 val,vMixVal;

	val = DSP_GetRiscParam0(RISC_MAIN_MODE);

	// bit0 is used for mixer mode
	// bit1 is used for iec0 delay mode
	vMixVal = DSP_GetRiscParam0(RISC_MAIN_MIXER_SELECT);

	if( mode == DualDSP_mode_Enable )
		val = ( val | (1<<MODE_DEC2_ENABLE_BIT) );
	else
	{
		val = ( val & ~(1<<MODE_DEC2_ENABLE_BIT) );
	}

	if( DSP_ERROR_NONE != DSP_SetRiscParam0( RISC_MAIN_MODE , val ) )
	{
		return AUDIF_FAILURE;
	}

//	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	if( mode == DualDSP_mode_Enable )
	{
		DSP_SetRiscParam0(RISC_MAIN_DUALMPEG_DEC1_GAIN,0x3fff);
		DSP_SetRiscParam0(RISC_MAIN_DUALMPEG_DEC2_GAIN_H,0x3fff);
		DSP_SetRiscParam0(RISC_MAIN_DUALMPEG_DEC2_GAIN_L,0x3fff);
		DSP_SetRiscParam0(RISC_MAIN_MIXER_SELECT,vMixVal|0x1);   //0x0:BD mixer  0x1:DualMpeg mixer
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

		// update RISC parameter by change bank0 state
		DSP_SetSystemIdle();
		DSP_SetSystemReady();
		DSP_SetSystemIdle();
		DSP_SetSystemRun();

		// reset dec1 for update dsp internal flow
		aud_SetCodecParams(aud_param.coding_mode_now);
		DSP_InitCodec(CODEC_BANK_DEC1);
	}
	else
	{
		DSP_SetRiscParam0(RISC_MAIN_MIXER_SELECT,vMixVal&0xFFFE );   //0x0:BD mixer  0x1:DualMpeg mixer
		DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

		AUDIF_Reset_DSP();
	}

	return AUDIF_SUCCESS;
}

UINT32 AUDIF_Set_DualMpegDec1MixerGain(UINT16 gain, UINT16 fade)
{

/********************************************************
*	gain = Decoder 1 volume (from 0~77)
*	fade = fading control parameter (from 0~256)
*********************************************************/

    // mixer gain = user gain * fade
    int target_gain;

    if (gain<0 || gain>=DB2LINEAR_SCALE || fade<0 || fade>=FADE_SCALE)
        return AUDIF_FAILURE;

    BYTE idx = Fade2dB_Tab[fade];

	int fade_gain = X16(dB2Linear_Tab[idx]);
	int user_gain = X16(dB2Linear_Tab[gain]);

	if (user_gain==FULL_SCALE_GAIN && fade_gain==FULL_SCALE_GAIN)
		target_gain = FULL_SCALE_GAIN;
	else if (user_gain==FULL_SCALE_GAIN)
		target_gain = fade_gain;
	else if (fade_gain==FULL_SCALE_GAIN)
		target_gain = user_gain;
	else
		target_gain = (user_gain*fade_gain)>>15;

	DSP_SendCmd(RISC_CMD0x14_MIXER_GAIN, CODEC0X00_DEC1_GAIN, target_gain, 0x0);

    return AUDIF_SUCCESS;
}


UINT32 AUDIF_Set_DualMpegDec2MixerGain(UINT16 gain, UINT16 pan)
{

/********************************************************
*	gain = Decoder 2 volume (from 0~77)
*	fade = panning control parameter (from 0~0xFF)
*********************************************************/

    // mixer gain = user gain * pan
    BYTE idx;
	int i, user_gain, target_gain;
	int temp[2], pan_gain[2];

    if (gain<0 || gain>=DB2LINEAR_SCALE || pan<0 || pan>=0xFF)
        return AUDIF_FAILURE;

	user_gain = X16(dB2Linear_Tab[gain]);

	if (pan<=0x7F)
	{
		idx = (pan>0x15)?0x15:pan;

		pan_gain[0] = X16(dB2Linear_Tab[idx]);
		pan_gain[1] = X16(dB2Linear_Tab[0]);
	}
	else
	{
		idx = (pan<0xEB)?0xEB:pan;
		idx = 0x100 - pan;

		pan_gain[0] = X16(dB2Linear_Tab[idx]);
		pan_gain[1] = X16(dB2Linear_Tab[0]);
	}

	for (i=0; i<2; i++)
	{
		if (user_gain==FULL_SCALE_GAIN && pan_gain[i]==FULL_SCALE_GAIN)
			temp[i] = FULL_SCALE_GAIN;
		else if (user_gain==FULL_SCALE_GAIN)
			temp[i] = pan_gain[i];
		else if (pan_gain[i]==FULL_SCALE_GAIN)
			temp[i] = user_gain;
		else
			temp[i]  = (user_gain*pan_gain[i])>>15;
	}

	target_gain = temp[0]<<16 | temp[1];

	DSP_SendCmd(RISC_CMD0x14_MIXER_GAIN, CODEC0X01_DEC2_GAIN,  0x0, target_gain);

    return AUDIF_SUCCESS;
}

void AUDIF_Set_DualMpegMixerMode(UINT8 mode)
{
/**********************************
*	mode: 0->mixer output
*		  1->output dec1
*		  2->output dec2
***********************************/
    DSP_SendCmd(RISC_CMD0x14_MIXER_GAIN, CODEC0X02_MIXER_MODE,  mode, 0x0);
}



//*******************************************************
//	Others API
//*******************************************************

UINT8 AUDF_Get_I2S_Data_Toggle_Status(void)
{
	return 0;
}

UINT8 AUDIF_Check_DVD_Audio_WaterMark(void)
{
	return 0;
}


void AUDIF_Watchdog(void)
{
	if( DSP_GetState(DSP_STATE_DEC1) == TASK_PLAY )
	{

#ifdef DumpBar_Watchdog
		UINT16 freset;
		BsBufInfo* pBsBufInfo = DSP_UpdateBsBufInfo(BSBUF_ID_DEC1);
		UINT32 bsbufFree = DSP_GetBsBufFree(BSBUF_ID_DEC1);

		freset=0;

		// no playback, but system on standby mode
		if( pBsBufInfo->dumpPtr == 0 && pBsBufInfo->barrPtr == 0 )
			freset = 1;

		// dump no move
		if( prior_DSPdump == pBsBufInfo->dumpPtr )
			freset = 1;

		// record dump value
		prior_DSPdump	= pBsBufInfo->dumpPtr;

		// dump without moving caused by barrier no move first
		if( prior_DSPbarrier == pBsBufInfo->barrPtr && bsbufFree<1024 )
			freset = 0;

		if( (prior_DSPbarrier != pBsBufInfo->barrPtr)|| freset )
		{
			dspWatchDog_cnt = 0;
			prior_DSPbarrier = pBsBufInfo->barrPtr;
		}
		else
		{
			dspWatchDog_cnt++;
			prior_DSPbarrier = pBsBufInfo->barrPtr;

			if( dspWatchDog_cnt > 10 )
			{
				dspWatchDog_cnt = 0;
				diag_printf("**********************************************\n");
				diag_printf("*\tcall AUDIF_Watchdog\n");
				diag_printf("*\tdump:(0x%x) bar:(0x%x)\n",pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);
				diag_printf("*\tdsp state:0x%08x\n",HWREG_R(dsp_port[12]));
				diag_printf("**********************************************\n");
				//extern void audDumpInfo(void);
				//audDumpInfo();
				AUDIF_Reset_DSP();
			}
		}

		// FIFO watchdog
		if( aud_param.coding_mode_now == AUDIF_CODING_MODE_MP3 ||
			aud_param.coding_mode_now == AUDIF_CODING_MODE_AC3||
			aud_param.coding_mode_now == AUDIF_CODING_MODE_DTS )
		{
			UINT16 A5cnt = HWREG_R(aud_a5_cnt);

			if( (prior_A5cnt != A5cnt) || (A5cnt == 0) )
			{
				FIFOWatchDog_cnt = 0;
				prior_A5cnt = A5cnt;
			}
			else
			{
				FIFOWatchDog_cnt++;
				prior_A5cnt = A5cnt;

				if( FIFOWatchDog_cnt>10 && (HWREG_R(aud_a0_cnt)==0 ))
				{
					FIFOWatchDog_cnt = 0;
					diag_printf("**********************************************\n");
					diag_printf("*\tcall FIFO_Watchdog\n");
					diag_printf("*\tdump:(0x%x) bar:(0x%x)\n",pBsBufInfo->dumpPtr,pBsBufInfo->barrPtr);
					diag_printf("*\tA0 cnt:(0x%x) A5 cnt:(0x%x)\n",HWREG_R(aud_a0_cnt),HWREG_R(aud_a5_cnt));
					diag_printf("**********************************************\n");
					DSP_SendCmd(RISC_CMD0x01_FIFO_CTRL,CODE0x01_FIFO_RESET,0x0,0x0);
				}
			}
		}
#endif
		// dsp report counter
		if( AUDF_IS_FIFO_PAUSE() != TRUE )
		{
			UINT16 dsp_freerun_cnt_now = HWREG_R( dsp_port[13] );

			if( dsp_freerun_cnt != dsp_freerun_cnt_now)
			{
				dsp_freerun_cnt = dsp_freerun_cnt_now;
				freerun_resetdsp_cnt = 0;
			}
			else
			{
				freerun_resetdsp_cnt++;
				dsp_freerun_cnt = dsp_freerun_cnt_now;

				if( freerun_resetdsp_cnt > 10)
				{
					freerun_resetdsp_cnt = 0;
					diag_printf("****************************************************************\n");
					diag_printf("*\tcall AUDIF_Watchdog caused by dsp freerun counter no move\n");
					diag_printf("****************************************************************\n");
					AUDIF_Reset_DSP();
				}
			}
		}

	}
}

void AUDIF_Initialize_DVD_Audio_WaterMark(void)
{
}

UINT32 AUDIF_IS_DTS_Decoder(void)
{
	return 0;
}

INT32 AUDIF_Is_WAM_Low_Rate(void)
{
	return 0;
}

#if 0
void AUDIF_MIDI_Set_Flow_Ctrl(UINT8 mode , UINT16 param)
{
	UNUSED_ARGUMENT(mode);
	UNUSED_ARGUMENT(param);
}


void AUDIF_NES_Set_DAC(UINT8 mode , UINT8 type_dac , UINT8 playbackrate)
{
	UNUSED_ARGUMENT(mode);
	UNUSED_ARGUMENT(type_dac);
	UNUSED_ARGUMENT(playbackrate);
}

void AUDIF_NES_Set_Play(void)
{
}

void AUDIF_NES_Set_SPDIF(UINT8 mode)
{
	UNUSED_ARGUMENT(mode);
}

void AUDIF_NES_Set_Stop(void)
{
}
#endif

void AUDIF_RampDown_PCM_Out(void)
{
    UINT16 aux1_current = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT, 7, 0);
    UINT16 aux2_current = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT,15, 8);
    
	//DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x01_RAMP_DOWN,0,0);
	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x03_A0_RAMP_DOWN,0,0);

    if ((AUDIF_AUX1_PATH_LFE == aux1_current) || (AUDIF_AUX1_PATH_MIXER == aux1_current))
    {
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x05_A1_RAMP_DOWN,0,0);
    }    
    
    if (AUDIF_AUX2_PATH_MIXER == aux2_current)
    {
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x07_A2_RAMP_DOWN,0,0);
    }        
}

void AUDIF_RampUp_PCM_Out(void)
{
    UINT16 aux1_current = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT, 7, 0);
    UINT16 aux2_current = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT,15, 8);
    
	//DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x00_RAMP_UP,0,0);
	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x02_A0_RAMP_UP,0,0);
    if ((AUDIF_AUX1_PATH_LFE == aux1_current) || (AUDIF_AUX1_PATH_MIXER == aux1_current))
    {
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x04_A1_RAMP_UP,0,0);
    }    

    if (AUDIF_AUX2_PATH_MIXER == aux2_current)
    {
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x06_A2_RAMP_UP,0,0);
    }            
}

UINT32 AUDIF_WaitRampDown(void)
{
	// wait ramp down
	UINT32 FIFO_CNT_A0 = HWREG_R(aud_a0_cnt) / 3; // FIFO sample number

	if( FIFO_CNT_A0 != 0)
	{
		int delaytime = 0;
		int smp_val = aud_map_SmpRate_FStoDigital(aud_param.smprate_info.hw_smprate);
		int RampSampleNum = 0;
		int index = 0;

		while( FIFO_CNT_A0 >=32 )
		{
			FIFO_CNT_A0 >>= 1;
			index++;
			if( index >= 7)
				break;
		}
		if( index == 0 )
			RampSampleNum = (1 << 5);
		else
			RampSampleNum = (1 << (index+4));

		delaytime = ((RampSampleNum*1000)/smp_val)+1;

		DSP_Waitloop(delaytime);
		//GL_DelayUs(40000); // busy waiting
	}

	return AUDIF_SUCCESS;

}

INT32 AUDIF_Set_RepRead(UINT8 uCmdType)
{
	UNUSED_ARGUMENT(uCmdType);
	return 0;
}

//*******************************************************
//	Other property
//*******************************************************
void AUDIF_Set_Spdif_ChannelStatus(UINT8 CGMS_type , UINT8 bit_res, UINT8 disc_type)
{
/*************************************************************
*	Bit 1~ bit 0
*	Copy Generation Management System(CGMS)
*	CGMS | IEC 61937
*	            |_bit2_bit15____________________
*	0 0       |   1        x   unlimited copies allowed
*	0 1       |   -         -    not used
*	1 0       |   0        0   one copy allowed
*	1 1       |   0        1   no copy allowed
*
*	bit2
*	0: SPDIF/PCM output 16-bit
*	1: SPDIF/PCM output 24-bit
*
*	bit3: byte1 category code
*	0: set CD
*	1: set DVD
*************************************************************/
#if 0
    UINT8 curr_CGMS_type, curr_bit_res, curr_disc_type;

    curr_CGMS_type = HWREG_R(dsp_port[14])&0x3;
    curr_bit_res = (HWREG_R(dsp_port[14])>>2)&0x1;
    curr_disc_type = (HWREG_R(dsp_port[14])>>3)&0x1;

    if ((CGMS_type == curr_CGMS_type) && (bit_res == curr_bit_res) && (disc_type == curr_disc_type))
    {
        return;
    }
#endif

	//AUDDRV_PRINT("AUDIF_Set_Spdif_ChannelStatus:: CGMS:%x\n",CGMS_type);
	//AUDDRV_PRINT("AUDIF_Set_Spdif_ChannelStatus:: BitResolution:%x\n",bit_res);
	//AUDDRV_PRINT("AUDIF_Set_Spdif_ChannelStatus:: Category:%x\n",disc_type);

	HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) & 0xfffffff0);	//clear last four bits

	// set CGMS type
	switch(CGMS_type)
	{
		case unlimited_copy:
			HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) | unlimited_copy );
			//cmhuang: 2010.05.31 HDMI HBR CGMS setting, not test yet, temporal disabled
			//HDMI_IF_Set_AudioGMS(CGMS_UnlimitCopy);
			break;
		case one_copy_allowed:
			HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) | one_copy_allowed );
			//HDMI_IF_Set_AudioGMS(CGMS_OneCopyAllowed);
			break;
		case no_copy_allowed:
			HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) | no_copy_allowed );
			//HDMI_IF_Set_AudioGMS(CGMS_NoCopyAllowed);
			break;
		default:
			HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) | no_copy_allowed );
			//HDMI_IF_Set_AudioGMS(CGMS_NoCopyAllowed);
			break;
	}

	// set bit resolution
	HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) | bit_res << 2);

	// set category code
	HWREG_W(dsp_port[14], HWREG_R(dsp_port[14]) | disc_type << 3);
}


void AUDIF_Set_CCA(UINT8 CCA_type)
{
    return;
}

void AUDIF_Set_Mic_Buf_Fullratio(UINT32 val)
{
    UNUSED_ARGUMENT(val);
    AUDDRV_PRINT("AUDIF_Set_Mic_Buf_Fullratio::not needed\n");
}

void AUDIF_LPCM_Set_Buf_EndPoint(UINT32 fsize)
{
	AUDDRV_PRINT("Set LPCM Decode Buf => Fsize:%x",fsize);

    UINT32 asize, res;

    asize = AUDIF_Get_audBSbuf_size(AUDIF_BANK_DEC1);
    if(fsize > asize)
        res = 0;
    else
    {
        aud_param.dec_param.lpcm_frame_size = fsize;
        res = asize % fsize;
    }
    //AudioSetEndPoint(asize-res);
	HWREG_W(dsp_port[13], (asize-res)); //LPCM bitstream buffer length (bytes)

    AUDDRV_PRINT("Set LPCM Decode Buf => Endpoint:%x\n",(asize-res));

}

UINT32 AUDIF_Get_LPCM_Fsize(void)
{
	return aud_param.dec_param.lpcm_frame_size;
}

AUDIF_Status_e AUDIF_Set_LPCM_Fsize(UINT32 param)
{
	aud_param.dec_param.lpcm_frame_size = param;
	return AUDIF_SUCCESS;
}

void AUDIF_Set_LPCM_Flag(UINT32 val)
{
	UNUSED_ARGUMENT(val);
	  AUDDRV_PRINT("AUDIF_Set_LPCM_Flag::dummy func??\n");
}

void AUDIF_LPCM_Init_Parm(UINT32 mode, WAVFORMATMACRO *wfmt1)
{
	UINT16 codecParams[16];
	int i=0;

	union {
		UINT32 reserve;
		WAVFORMATMACRO	*fmt;
	}g_ADPCMfmt;

	AUDIF_Set_Stop( AUDIF_BANK_DEC1 );

	AUDIO_SEMAPHORE_TAKE();

	memcpy(&gWavFmt, wfmt1, sizeof(gWavFmt));

	// Prepare codec parameters (no need to call WORD16 to translate to BE)
	for (i=0; i<K_CODEC_PARAMS; i++)
	{
		codecParams[i] = 0;
	}

	gWavFmt.wfx.wFormatTag = (UINT16)mode;

	g_ADPCMfmt.fmt = &gWavFmt;
	PCMWAVEFORMAT *ADPCMfmtParam;
	ADPCMfmtParam = (PCMWAVEFORMAT *)g_ADPCMfmt.reserve;

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

	codecParams[0] =  ID_DECODER_PCM ;
	codecParams[2] =  gWavFmt.wfx.nChannels;
	codecParams[3] =  gWavFmt.wfx.wBitsPerSample;
	codecParams[4] =  SampleRate_Mapping(gWavFmt.wfx.nSamplesPerSec);
	codecParams[15] = 0x8;	// frame counts message: 2^4 = 16 (frames)

	DSP_WriteCodecParams(CODEC_BANK_DEC1, codecParams);

	#if 0
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.wFormatTag %d\n", gWavFmt.wfx.wFormatTag);
		 AUDDRV_PRINT("[ccyang] codecParams[1] %d\n", codecParams[1]);
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.nChannels %d\n", gWavFmt.wfx.nChannels);
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.wBitsPerSample %d\n", gWavFmt.wfx.wBitsPerSample);
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.nSamplesPerSec %d\n", gWavFmt.wfx.nSamplesPerSec);
		 AUDDRV_PRINT("[ccyang] gWavFmt.wfx.nBlockAlign %d\n", gWavFmt.wfx.nBlockAlign);
		 AUDDRV_PRINT("[ccyang] codecParams[6] %d\n", codecParams[6]);
	#endif

	AUDIO_SEMAPHORE_GIVE();

	UINT16 R_SmpRate;
	R_SmpRate= aud_map_SmpRate_IDtoFS(codecParams[4]);

	if( aud_param.smprate_info.smp_rate_now != R_SmpRate )
	{
		// set sample rate to Audio HW
		AUDIF_Set_SampleRate(R_SmpRate);
		// set sample rate to main dsp
		//DSP_SetRiscParam0(RISC_MAIN_DAC_FS, codecParams[4] );
		//DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	}

	AUDIF_Set_Play( AUDIF_BANK_DEC1 );

	return;

}

void AUDIF_LPCM_Check_Param(BYTE* pType)
{
	UNUSED_ARGUMENT(pType);
  AUDDRV_PRINT("AUDIF_LPCM_Check_Param::dummy func??\n");
}

void AUDIF_AAC_Init_Freq(void)
{
  AUDDRV_PRINT("AUDIF_AAC_Init_Freq::dummy func??\n");
}

INT32 AUDIF_Set_Decoder_OPT( UINT16 wavefmt , UINT16 opt_val )
{
	AUDIF_Status_e rVal = AUDIF_SUCCESS;

	switch(wavefmt)
	{
		case WAVE_FORMAT_MPEG2AAC:
		case WAVE_FORMAT_AAC:
			// bit0: 0=aac enable, 1=aac disable
			// bit1: 0=heaac enable, 1=heaac disable
			aud_param.Decoder_OPT = opt_val;
#ifdef DEBUG_DDCO            
            aud_param.Decoder_OPT = 0;
#endif
			break;

		default:
			aud_param.Decoder_OPT = 0;
			rVal = AUDIF_ERR_Unknown_CodecID;
			AUDDRV_PRINT("[Err]AUDIF_Set_Decoder_OPT::Wrong Parameter\n");
			break;
	}

	return rVal;
}

void AUDIF_PowerDown_DAC( void )
{
	GL_WriteI2cReg(0xa,0x00);
	GL_WriteI2cReg(0x3,0xff);
	AUDDRV_PRINT("Power down DAC\n");
}

void AUDIF_Set_XCK_FS( UINT16 XCK_FS )
{
    // dylan : 9200 interal DAC setting
    //  XCK = N*Fs
    //  BCK = XCK/2 = (N/2)*Fs
    //  LRCK = XCK/128  = (N/128)*Fs
    //  Since LRCK must be 1*Fs--->N must be 128 so that XCK = 128*Fs
    //  !!!!!NO need to set XCK for INTERNAL DAC!!!!!
    
	AUDIO_SEMAPHORE_TAKE();
	UNUSED_ARGUMENT(XCK_FS);
	AUDIO_SEMAPHORE_GIVE();    
}

void AUDIF_Set_Ext_XCK_FS( UINT16 XCK_FS )
{
    switch(XCK_FS)
    {
        case SUPPORT_128_XCK:
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
            break;

        case SUPPORT_256_XCK:
            audHW_param.xck_clk_fs_table[0] = AUDCLK_256XCK_08192;
            audHW_param.xck_clk_fs_table[1] = AUDCLK_256XCK_11290;
            audHW_param.xck_clk_fs_table[2] = AUDCLK_256XCK_12288;
        	audHW_param.xck_clk_fs_table[3] = 0;
        	audHW_param.xck_clk_fs_table[4] = 0;
        	audHW_param.xck_clk_fs_table[5] = AUDCLK_256XCK_22579;
        	audHW_param.xck_clk_fs_table[6] = AUDCLK_256XCK_24576;
        	audHW_param.xck_clk_fs_table[7] = 0;
        	audHW_param.xck_clk_fs_table[8] = 0;
        	audHW_param.xck_clk_fs_table[9] = AUDCLK_256XCK_45158;
        	audHW_param.xck_clk_fs_table[10] = AUDCLK_256XCK_49152;
        	audHW_param.xck_clk_fs_table[11] = 0;
        	audHW_param.xck_clk_fs_table[12] = 0;
            break;

        default:
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
            break;
    }
    
    audHW_param.Is_xck_clk_fs_setting = 1;    
}

void AUDIF_Set_I2SBypass( UINT16 mode )
{
    AUDIO_SEMAPHORE_TAKE();
	UNUSED_ARGUMENT(mode);
	AUDIO_SEMAPHORE_GIVE();   
}

void AUDIF_Set_PCM_CFG( UINT16 type )
{
	// type [in] refer DAC_format_e
	aud_param.PCM_CFG = type;
    HWREG_W(pcm_cfg,aud_param.PCM_CFG);    
	AUDDRV_PRINT("AUDIF_Set_PCM_CFG: 0x%x\n",type);
}
void AUDIF_Set_DAC_Type( UINT8 DAC_Type , UINT16 num_output_ch )
{
	// DAC_Type [IN] ref AUDIF_DAC_Type_e in auddrv.h
	// num_output_ch [IN] ref DAC_OutputCfg_e in auddrv.h

	if( DAC_Type == INT_DAC_1500 )
	{
	 	// 2-ch int DAC ctrl on Main die
	 	// disable auto-sleep
		HWREG_W(adac_qae377_ctrl, 0x2c);
	}
	else
	{
		AUDIF_Set_DAC_Output(num_output_ch);
	}
}

void AUDIF_Set_DAC_Output( UINT8 DAC_cfg )
{
	// DAC_cfg [IN] ref: DAC_OutputCfg_e

	switch( DAC_cfg )
	{
		case DAC_off:
			aud_param.DAC_Output_Cfg = 0xff;
			AUDDRV_PRINT("DAC_off\n");
			break;
		case DAC_2ch_output:
			aud_param.DAC_Output_Cfg = 0xfc;
			AUDDRV_PRINT("DAC_2ch_output\n");
			break;
		case DAC_6ch_output:
			aud_param.DAC_Output_Cfg = 0xc0;
			AUDDRV_PRINT("DAC_6ch_output\n");
			break;
		case DAC_8ch_output:
			aud_param.DAC_Output_Cfg = 0x0;
			AUDDRV_PRINT("DAC_8ch_output\n");
			break;
		default:
			aud_param.DAC_Output_Cfg = 0x0;
			AUDDRV_PRINT("DAC_8ch_output\n");
			break;
	}

	//GL_WriteI2cReg(0xa, ~aud_param.DAC_Output_Cfg );
	//GL_WriteI2cReg(0x3, aud_param.DAC_Output_Cfg );
}

UINT8 AUDIF_Get_DAC_Output( void )
{
	int DAC_cfg = GL_ReadI2cReg(0x3);
	UINT8 rVal=0;

	switch( DAC_cfg )
	{
		case 0xff:
			rVal = DAC_off;
			AUDDRV_PRINT("DAC_off\n");
			break;
		case 0xfc:
			rVal = DAC_2ch_output;
			AUDDRV_PRINT("DAC_2ch_output\n");
			break;
		case 0xc0:
			rVal = DAC_6ch_output;
			AUDDRV_PRINT("DAC_6ch_output\n");
			break;
		case 0x0:
			rVal = DAC_8ch_output;
			AUDDRV_PRINT("DAC_8ch_output\n");
			break;
	}
	return rVal;

}

void AUDIF_Set_MPEG_CRC_Check( UINT16 switch_mode )
{
/************************************************************
*	You MUST set CRC_Check mode before download mpeg dsp
*	switch_mode[IN]:
*		CRC_check_off	= 0x0,
*		CRC_check_on	= 0x1,
************************************************************/
#if 0
	if( (gWavFmt.wfx.wFormatTag != WAVE_FORMAT_MPEG) &&
		(gWavFmt.wfx.wFormatTag != WAVE_FORMAT_MPEGLAYER3) )
	{
		AUDDRV_PRINT("[Invalid CRC mode]NOT MPEG audio\n");
		return;
	}
#endif
	if( switch_mode == CRC_check_off)
		AUDDRV_PRINT("AUDIF Set MPEG CRC check [OFF]\n");
	else if( switch_mode == CRC_check_on )
		AUDDRV_PRINT("AUDIF Set MPEG CRC mode [ON]\n");
	else
	{
		AUDDRV_PRINT("[Invalid CRC mode] %d\n",switch_mode);
		return;
	}

	aud_param.MPEG_CRC_Check = switch_mode;

}

UINT16 AUDIF_Get_MPEG_CRC_Check( void )
{
	return aud_param.MPEG_CRC_Check;
}


UINT32 AUDIF_SetSndEffect(SndEffect* pSndEffect)
{

#define K_DSP_MEMMAP_DRAMBASE		0x01000000
#define K_DSP_MAX_DMA_RANGE         (16*1024*1024) // 16M bytes

	if (pSndEffect->id > ID_SND_EFF_MAX)
	{
		AUDDRV_PRINT("AUDIF_SetSndEffect::Unsupported effect ID:%d\n",pSndEffect->id);
		return 1;
	}

	//UINT32 DataBaseFromDramBank = DSP_GetInfo(INFO_DATA_BANK_BASE) - SDRAM_B_BASE_UNCACHED;
	UINT32 DataBaseFromDramBank = DSP_GetInfo(INFO_DATA_BANK_BASE);
	UINT32 SndAddress = pSndEffect->snd_address - DataBaseFromDramBank;

	//
	// Check if sound effect address is valid (for dram address)
	//

	if (SndAddress < 0)
	{
		AUDDRV_PRINT("AUDIF_SetSndEffect::e:sndAddress should be below Dsp databank\n");
		AUDDRV_PRINT("AUDIF_SetSndEffect::i:SndAddress:0x%x,DatabankAddress:0x%x!\n",pSndEffect->snd_address,DataBaseFromDramBank);
		return 1;
	}
	if (SndAddress >= K_DSP_MAX_DMA_RANGE)
	{
		AUDDRV_PRINT("AUDIF_SetSndEffect::e:SndAddress(offset) should be no more than 16M bytes\n");
		AUDDRV_PRINT("AUDIF_SetSndEffect::i:SndAddress:0x%x,DatabankAddress:0x%x!\n",pSndEffect->snd_address,DataBaseFromDramBank);
		return 1;
	}
	if (SndAddress%128 != 0)
	{
		AUDDRV_PRINT("AUDIF_SetSndEffect::e:SndAddress(offset) should be 128 bytes aligned\n");
		AUDDRV_PRINT("AUDIF_SetSndEffect::i:SndAddress:0x%x\n",pSndEffect->snd_address);
		return 1;
	}

	UINT32 GainChanA0 = (pSndEffect->MixGainLf <<16) | (pSndEffect->MixGainRf);
	UINT32 GainChanA1 = (pSndEffect->MixGainLs <<16) | (pSndEffect->MixGainRs);
	UINT32 GainChanA2 = (pSndEffect->MixGainC <<16) | (pSndEffect->MixGainLfe);
	UINT32 GainChanA3 = (pSndEffect->MixGainLb <<16) | (pSndEffect->MixGainRb);
	UINT32 GainChanA4 = (pSndEffect->MixGainLm <<16) | (pSndEffect->MixGainRm);


	AUDDRV_PRINT("AUDIF_SetSndEffect::ID:%d, address:0x%x, samples:%d\n",pSndEffect->id,SndAddress|K_DSP_MEMMAP_DRAMBASE,pSndEffect->snd_sampleCnt);
	if (GainChanA0)	AUDDRV_PRINT("AUDIF_SetSndEffect::gain:A0:0x%08x\n",GainChanA0);
	if (GainChanA1)	AUDDRV_PRINT("AUDIF_SetSndEffect::gain:A1:0x%08x\n",GainChanA1);
	if (GainChanA2)	AUDDRV_PRINT("AUDIF_SetSndEffect::gain:A2:0x%08x\n",GainChanA2);
	if (GainChanA3)	AUDDRV_PRINT("AUDIF_SetSndEffect::gain:A3:0x%08x\n",GainChanA3);
	if (GainChanA4)	AUDDRV_PRINT("AUDIF_SetSndEffect::gain:A4:0x%08x\n",GainChanA4);

	AUDIO_SEMAPHORE_TAKE();
	DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x00_RAMP_UP,0,0); // ramp up
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x00_EFFECT_ADDRESS,pSndEffect->id,SndAddress|K_DSP_MEMMAP_DRAMBASE);
	//hcsu:DSP code should protect
	if (pSndEffect->id != ID_SND_EFF_STEREO)
	{
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x02_EFFECT_MIXGAIN_A0,pSndEffect->id,GainChanA0);
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x03_EFFECT_MIXGAIN_A1,pSndEffect->id,GainChanA1);
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x04_EFFECT_MIXGAIN_A2,pSndEffect->id,GainChanA2);
    DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x05_EFFECT_MIXGAIN_A3,pSndEffect->id,GainChanA3);
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x06_EFFECT_MIXGAIN_A4,pSndEffect->id,GainChanA4);
	}

	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x01_EFFECT_SAMPLE_CNT,pSndEffect->id,pSndEffect->snd_sampleCnt);
	AUDIO_SEMAPHORE_GIVE();

	return 0;
}


UINT32 AUDIF_CheckSndEffectDone(SndEffectChannelID SndEffectID)
{

	UINT32 sampleLeft = 0;

	if (SndEffectID > ID_SND_EFF_MAX)
	{
		AUDDRV_PRINT("AUDIF_SetSndEffect::Unsupported effect ID:%d\n",SndEffectID);
		return 1;
	}


	AUDIO_SEMAPHORE_TAKE();
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x07_EFFECT_CHECK_DONE,SndEffectID,0x0);
	AUDIO_SEMAPHORE_GIVE();

    sampleLeft = GET_CMD_PARAM_PORT();
	AUDDRV_PRINT("AUDIF_SetSndEffect::ID:%d, sample left:%d\n",SndEffectID,sampleLeft);

	if (sampleLeft) return FALSE;  // if sampleLeft != 0, return FALSE

	return TRUE;

}

UINT32 AUDIF_GetFreeMonoSndEffectID(void)
{

	UINT32 i = ID_SND_EFF_MONO_CH1;
	UINT32 freeID = ID_SND_EFF_MONO_CH1;
	UINT32 sampleLeft = 0;

	AUDIO_SEMAPHORE_TAKE();
	for(i = ID_SND_EFF_MONO_CH1; i<ID_SND_EFF_MAX; i++)
	{
		DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x07_EFFECT_CHECK_DONE,i,0x0);
  	    sampleLeft = GET_CMD_PARAM_PORT();
		if (0 == sampleLeft)
		{
			freeID = i;
			break;
		}
	}

   	AUDIO_SEMAPHORE_GIVE();
	return freeID;
}


INT32 AUDIF_Set_PSREG_ChnNum( void )
{
	if( DSP_GetState(DSP_STATE_DEC1) == TASK_PLAY )
	{
		//UINT32 channel_num = DSP_ReadData(DSP_READ_SHORT , 0x20a36 );
		UINT16 channel_num = DSP_GetDspParam0(DSP_DEC1_CHANNEL_MASK)&0xff;
		UINT32 PSREG_ChnNum, temp_ChnNum;
		
		PLAT_REG_GetAudioChn(&PSREG_ChnNum);

		switch( channel_num )
		{
			case 0x0:
				temp_ChnNum = PSREG_AUDIO_CHN_NONE;
				break;
			case 0x01:
			case 0x02:			
				temp_ChnNum = PSREG_AUDIO_CHN_MONO;
				break;
			case 0x03:
				temp_ChnNum = PSREG_AUDIO_CHN_STEREO;
				break;
			case 0x13:
				temp_ChnNum = PSREG_AUDIO_CHN_2_1;
				break;
			case 0x0f:
				temp_ChnNum = PSREG_AUDIO_CHN_4;
				break;
			case 0x1f:
				temp_ChnNum = PSREG_AUDIO_CHN_5;
				break;
			case 0x3f:
				temp_ChnNum = PSREG_AUDIO_CHN_6;
				break;
			case 0x7f:
				temp_ChnNum = PSREG_AUDIO_CHN_7;
				break;
			case 0xff:
				temp_ChnNum = PSREG_AUDIO_CHN_8;
				break;
			default:
				temp_ChnNum = PSREG_AUDIO_CHN_STEREO;
				break;
		}

		if( temp_ChnNum != PSREG_ChnNum )
		{
			PLAT_REG_SetAudioChnInvalid();	
			
			switch( channel_num )
			{
				case 0x0:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_NONE);
					break;
				case 0x01:
				case 0x02:			
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_MONO);
					break;
				case 0x03:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_STEREO);
					break;
				case 0x13:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_2_1);
					break;
				case 0x0f:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_4);
					break;
				case 0x1f:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_5);
					break;
				case 0x3f:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_6);
					break;
				case 0x7f:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_7);
					break;
				case 0xff:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_8);
					break;
				default:
					PLAT_REG_SetAudioChn(PSREG_AUDIO_CHN_STEREO);
					break;
			}

			PLAT_REG_SetAudioChnValid();
			//aud_param.Dec1_ChnNum = channel_num;
			//diag_printf("AUDIF_Set_PSREG_ChnNum: 0x%x\n",channel_num);
		}
	}
	return AUDIF_SUCCESS;
}

INT32 AUDIF_Get_Channel_Mask(UINT8 BankID, UINT8 *dsp_chnnum )
{
/*********************************************************	
*	[IN] spk_cfg: each bit represents to a speaker
*		 the bit value set to 1 if speaker is on;
*		 Otherwise, set to zero if speaker is off.
*		MSB-----------------LSB
*		Rb Lb LFE C Rs Ls Rf Lf
*********************************************************/

	AUDIO_SEMAPHORE_TAKE();
	
	UINT8 DSP_BankID = 1;
	
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Get_Channel_Mask::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_BankID;
	}
	
	switch(DSP_BankID)
	{
		case CODEC_BANK_DEC1:
			*dsp_chnnum = (UINT8)(DSP_GetDspParam0(DSP_DEC1_CHANNEL_MASK)&0xff);
			break;
		case CODEC_BANK_DEC2:
			*dsp_chnnum = (UINT8)(DSP_GetDspParam0(DSP_DEC2_CHANNEL_MASK)&0xff);
			break;
		default:
			*dsp_chnnum = (UINT8)(DSP_GetDspParam0(DSP_DEC1_CHANNEL_MASK)&0xff);
			break;
	}

	AUDDRV_PRINT("AUDIF_Get_Channel_Mask: 0x%x\n", *dsp_chnnum);

	AUDIO_SEMAPHORE_GIVE();	
	return AUDIF_SUCCESS;

}

INT32 AUDIF_Get_Channel_Number(UINT8 BankID)
{
	UINT8 DSP_BankID = 1;
	UINT16 dsp_chnnum = 0;
	INT32 channel_num = 0;	
	int i=0;

	AUDIO_SEMAPHORE_TAKE();

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Get_Channel_Number::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_BankID;
	}

	switch(DSP_BankID)
	{
		case CODEC_BANK_DEC1:
			dsp_chnnum = DSP_GetDspParam0(DSP_DEC1_CHANNEL_MASK);
			break;
		case CODEC_BANK_DEC2:
			dsp_chnnum = DSP_GetDspParam0(DSP_DEC2_CHANNEL_MASK);
			break;
		default:
			dsp_chnnum = DSP_GetDspParam0(DSP_DEC1_CHANNEL_MASK);
			break;
	}

	for( i=0 ; i<8 ; i++ )
	{
		if( (dsp_chnnum & 0x1)== 1 )
			channel_num+=1;
		dsp_chnnum = dsp_chnnum>>1;
	}

	AUDDRV_PRINT("AUDIF_Get_Channel_Number: %d\n",channel_num);

	AUDIO_SEMAPHORE_GIVE();
	
	return channel_num;

}

INT32 AUDIF_Get_BitRate(UINT8 BankID)
{
	UINT8 DSP_BankID = 1;
	UINT16 BitRate_LW = 0;
	UINT16 BitRate_HW = 0;
	INT32 BitRate = 0;

	AUDIO_SEMAPHORE_TAKE();

	// check if Bank ID is legal or not
	DSP_BankID = AUDIF_Check_CodecBankID(BankID);
	if( AUDIF_ERR_Codec_BankID == DSP_BankID )
	{
		AUDDRV_PRINT("AUDIF_Get_BitRate::e:AUDIF_ERR_Codec_BankID\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUDIF_ERR_BankID;
	}

	switch(DSP_BankID)
	{
		case CODEC_BANK_DEC1:
			BitRate_LW = DSP_GetDspParam0(DSP_DEC1_BIT_RATE_LW);
			BitRate_HW = DSP_GetDspParam0(DSP_DEC1_BIT_RATE_HW);
			break;
		case CODEC_BANK_DEC2:
			BitRate_LW = DSP_GetDspParam0(DSP_DEC2_BIT_RATE_LW);
			BitRate_HW = DSP_GetDspParam0(DSP_DEC2_BIT_RATE_HW);
			break;
		default:
			BitRate_LW = DSP_GetDspParam0(DSP_DEC1_BIT_RATE_LW);
			BitRate_HW = DSP_GetDspParam0(DSP_DEC1_BIT_RATE_HW);
			break;
	}

	BitRate = (BitRate_HW<<16)|BitRate_LW;
	
	//AUDDRV_PRINT("AUDIF_Get_BitRate: %d\n",BitRate);

	//BitRate = (INT32)DSP_ReadData( DSP_READ_LONG, 0x20a42);
	//diag_printf("DSP 0x20A42 BitRate: %d\n",BitRate);
	
	AUDIO_SEMAPHORE_GIVE();
	
	return BitRate;

}
void AUDIF_Set_SndEffect_mode( UINT8 mode )
{
/***********************************************************
*	mode [IN] SndeffectMode_e:
*		SndEFF_BD = 0,
*		SndEFF_game = 1,
***********************************************************/
	AUDIO_SEMAPHORE_TAKE();

	UINT16 MAIN_MODE = DSP_GetRiscParam0(RISC_MAIN_MODE);

	if( mode == SndEFF_game )
	{
		// enable game sound effect mode
		//MAIN_MODE |= WORD16(1<<MODE_SNDEFFECT_ENABLE_BIT);
		MAIN_MODE |= 0x100;

		// disable BD sound effect mode
		//MAIN_MODE &= ~(WORD16(1<<MODE_EFF_ENABLE_BIT));
		MAIN_MODE &= 0xfffb;
	}
	else if( mode == SndEFF_BD )
	{
		// disable game sound effect mode
		MAIN_MODE &= 0xfeff;

		// Enable BD sound effect mode
		MAIN_MODE |= 0x4;
	}
	else
	{
		AUDDRV_PRINT("[Err]Set_SndEffect_mode failed!\n");
		AUDIO_SEMAPHORE_GIVE();
		return;
	}

	DSP_SetSystemIdle();
	DSP_SetSystemReady();
	DSP_SetRiscParam0(RISC_MAIN_MODE, MAIN_MODE );
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	DSP_SetSystemIdle();
	DSP_SetSystemRun();

	AUDIO_SEMAPHORE_GIVE();
	return;
}

UINT32 AUDIF_StopSndEffect(SndEffectChannelID SndEffectID)
{
	if (SndEffectID > ID_SND_EFF_MAX)
	{
		AUDDRV_PRINT("AUDIF_StopSndEffect::Unsupported effect ID:%d\n",SndEffectID);
		return AUDIF_FAILURE;
	}

	AUDIO_SEMAPHORE_TAKE();
	DSP_SendCmd(RISC_CMD0x12_EFFECT_AUDIO,CODE0x01_EFFECT_SAMPLE_CNT,SndEffectID,0);
	AUDIO_SEMAPHORE_GIVE();

	return AUDIF_SUCCESS;
}
//
// temp for sound effect gain convertion
//

#if 0

#define MAXV			0x7FFFFFFF
#define	MINV			0x80000000
#define SAT(flt_num)	((int)(flt_num >= (double)0x80000000?MAXV:flt_num))
#define Q31(flt_num)	(SAT((double)0x80000000 * (flt_num) +0.5))

#define	Q_XY			29		// Depth_Value and LR_Value [-0x20000000,0x20000000]
#define	Q_SIN			6		// sin table size (2^Q_SIN+1)
#define	TABLE_SIZE		65		// (2^Q_SIN+1)

int sin_table[TABLE_SIZE] = {

Q31(0.000000000000),
Q31(0.024541228523),
Q31(0.049067674327),
Q31(0.073564563600),
Q31(0.098017140330),
Q31(0.122410675199),
Q31(0.146730474455),
Q31(0.170961888760),
Q31(0.195090322016),
Q31(0.219101240157),
Q31(0.242980179903),
Q31(0.266712757475),
Q31(0.290284677254),
Q31(0.313681740399),
Q31(0.336889853392),
Q31(0.359895036535),
Q31(0.382683432365),
Q31(0.405241314005),
Q31(0.427555093430),
Q31(0.449611329655),
Q31(0.471396736826),
Q31(0.492898192230),
Q31(0.514102744193),
Q31(0.534997619887),
Q31(0.555570233020),
Q31(0.575808191418),
Q31(0.595699304492),
Q31(0.615231590581),
Q31(0.634393284164),
Q31(0.653172842954),
Q31(0.671558954847),
Q31(0.689540544737),
Q31(0.707106781187),
Q31(0.724247082951),
Q31(0.740951125355),
Q31(0.757208846506),
Q31(0.773010453363),
Q31(0.788346427627),
Q31(0.803207531481),
Q31(0.817584813152),
Q31(0.831469612303),
Q31(0.844853565250),
Q31(0.857728610000),
Q31(0.870086991109),
Q31(0.881921264348),
Q31(0.893224301196),
Q31(0.903989293123),
Q31(0.914209755704),
Q31(0.923879532511),
Q31(0.932992798835),
Q31(0.941544065183),
Q31(0.949528180593),
Q31(0.956940335732),
Q31(0.963776065795),
Q31(0.970031253195),
Q31(0.975702130039),
Q31(0.980785280403),
Q31(0.985277642389),
Q31(0.989176509965),
Q31(0.992479534599),
Q31(0.995184726672),
Q31(0.997290456679),
Q31(0.998795456205),
Q31(0.999698818696),
Q31(1.000000000000)


};



INT64 f_mult(int a, int b, int PS32)
{
	return (((INT64) a * (INT64) b) >> PS32);
}


int f_sat(INT64 a)
{
	INT64 maxV = MAXV;
	INT64 minV = MINV;

	minV = -minV;

	if (a > maxV)
		a = maxV;
	else if (a < minV)
		a = minV;

	return ((int) a);
}

/********************************************************
	x: [-0x20000000,0x20000000], mapped to [-1,1]

	return sin(pi/2*|x|)
********************************************************/

int f_sin(int x)
{
	int integer, fraction, one, y;
	int ps32;
	INT64 acc;

	if (x < 0)
		x = -x;

	ps32 = Q_XY-Q_SIN;
	one = 1 << ps32;

	integer = x >> ps32;
	fraction = x - (integer<<ps32);

	acc = f_mult(sin_table[integer],one-fraction,ps32);
	if (integer != (TABLE_SIZE-1))
		acc += f_mult(sin_table[integer+1],fraction,ps32);
	y = f_sat(acc);

	return y;
}

/********************************************************
	x: [-0x20000000,0x20000000], mapped to [-1,1]

	return cos(pi/2*x)
********************************************************/
int f_cos(int x)
{
	int integer, fraction, one, y;
	int ps32;
	INT64 acc;

	if (x < 0)
		x = -x;

	ps32 = Q_XY-Q_SIN;
	one = 1 << ps32;

	integer = x >> ps32;
	fraction = x - (integer<<ps32);

	integer = (TABLE_SIZE-1) - integer;

	acc = f_mult(sin_table[integer],one-fraction,ps32);
	if (integer != 0)
		acc += f_mult(sin_table[integer-1],fraction,ps32);
	y = f_sat(acc);

	return y;
}

int Calculate_MixGainValue(UINT16 dGainValue, INT32 dDepthValue, INT32 dLRValue, SndEffect *pSndEffect)
{
	INT32 temp, tempY, gain;
	INT32 one, ps32;

	ps32 = 31;
	one = 1 << Q_XY;
	gain = (UINT32)dGainValue;

	// calculate L/R
	tempY = f_sin((dDepthValue+one)>>1);
	temp = f_sat(f_mult(f_sin(dLRValue),tempY,ps32));
	temp = f_sat(f_mult(gain,temp,ps32));
	if (dLRValue<=0)
	{
		pSndEffect->MixGainLf = (UINT16)temp;
		pSndEffect->MixGainRf = 0;
	}
	else
	{
		pSndEffect->MixGainLf = 0;
		pSndEffect->MixGainRf = (UINT16)temp;
	}

	// calculate C
	temp = f_sat(f_mult(f_cos(dLRValue),tempY,ps32));
	temp = f_sat(f_mult(gain,temp,ps32));
	pSndEffect->MixGainC = (UINT16)temp;

	// calculate LS
	tempY = f_cos((dDepthValue+one)>>1);
	temp = f_sat(f_mult(f_cos((dLRValue+one)>>1),tempY,ps32));
	temp = f_sat(f_mult(gain,temp,ps32));
	pSndEffect->MixGainLs = (UINT16)temp;

	// calculate RS
	temp = f_sat(f_mult(f_sin((dLRValue+one)>>1),tempY,ps32));
	temp = f_sat(f_mult(gain,temp,ps32));
	pSndEffect->MixGainRs = (UINT16)temp;

	// copy LS/RS to LB/RB and set LFE unity
	pSndEffect->MixGainLb = pSndEffect->MixGainLs;
	pSndEffect->MixGainRb = pSndEffect->MixGainRs;
	pSndEffect->MixGainLfe = 0xFFFF;

	// calculate Lm
	temp = f_cos((dLRValue+one)>>1);
	temp = f_sat(f_mult(gain,temp,ps32));
	pSndEffect->MixGainLm = (UINT16)temp;

	// calculate Rm
	temp = f_sin((dLRValue+one)>>1);
	temp = f_sat(f_mult(gain,temp,ps32));
	pSndEffect->MixGainRm = (UINT16)temp;

	return 0;
}

#endif


/*
**********************************************************
*
*   9200 related APIs
*
**********************************************************
*/
#if 0
/******************************************************************************/
/**
* @brief    Initial AUDIO.
*           The function shall be executed one time at application starting.
* @param    Aud_AudioInitialConfig_t *pInitialParam
			pInitialParam->PGA_gain : PGA gain control value : -25(mute) ~ 6(6 dB),(1 step is 1 dB)
* @return   PL_SUCCESS if successful
			PL_ERR_INVALID_PARAM if the initial parameter is wrong
******************************************************************************/
static Boolean gfAudioInitFlag = FALSE;
AUDIF_Status_t AUDIF_InitConfig(Aud_AudioInitialConfig_t *pInitialParam)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;
	DSP_DRV_AudInitialConfig_t InitialParam;

	// Initialize 9200 RISC parameters
	DSP_InitRiscParams1();

	// Initialize 9200 DSP parameters
	DSP_InitDspParams1();

	if(pInitialParam == NULL)
	{
		return AUD_ERR_INVALID_PARAM;
	}

    if(gfAudioInitFlag==FALSE)
    {
        /*Variables initial*/
		InitialParam.dLipSyncMemoryStarAddress = pInitialParam->dLipSyncMemoryStarAddress;
		InitialParam.dLipSyncMemorySize = pInitialParam->dLipSyncMemorySize;

      // dylan will cover this part
        DSP_DRV_AudI2SOutputCrossbarInit();

		/*set default I2S output format*/
        DSP_DRV_AudSetI2SOutputFormat(DTV_FALSE,DSP_DRV_AUD_SSD_OUTPUT_I2S,DSP_DRV_AUD_I2S_OUTPUT_PHILIPS24BIT);

        /**************************************************************************/
    	/* create mutex for protecting platform internal variables                */
    	/**************************************************************************/
    	cyg_mutex_init(&gPrimaryPathInfo.mutexID);
    	cyg_mutex_init(&gAudioPrimaryPath.mutexID);
    	cyg_mutex_init(&gAudioAUX1Path.mutexID);
		cyg_mutex_init(&gAudioAUX2Path.mutexID);
		cyg_mutex_init(&gAudioAUX3Path.mutexID);
    	cyg_mutex_init(&gAudioCenterSurroundPath.mutexID);
    	cyg_mutex_init(&gAudioCenterSubwooferPath.mutexID);
    	cyg_mutex_init(&gAudioSurroundSubwooferPath.mutexID);

		if(DSP_DRV_SUCCESS != DSP_DRV_AudSetPFAGain(pInitialParam->bPGA_gain))
		{
			return AUD_ERR_INVALID_PARAM;
		}

        DSP_DRV_AudSetDelayLineUnitConfig(AUD_AUDIO_LIP_SYNC_DELAY_SWITCH_DEFAULT,
										  AUD_AUDIO_LIP_SYNC_DELAY_DEFAULT);


		//DSP_DRV_AudSetEffectMode(DSP_DRV_AUD_EFFECT_MODE_DEFAULT); //GJ 090622 add for AVL setting
        DSP_SetSSDParam(REG_Ssd_sndmod, DSP_DRV_AUD_EFFECT_MODE_DEFAULT);

        /*unmute all output path*/
		//DSP_DRV_AudSetDEMDECMute(FALSE);
		DSP_SetSSDParam(REG_Ssd_ddmute, REG_SSD_DDMUTE_VALUE_NO_MUTE);

        //DSP_DRV_AudSetMutePath(DSP_DRV_AUD_MUTE_PATH_ALL,FALSE);
        DSP_SetSSDBit(REG_Ssd_mainmut_OFFSET, 0x1FF, REG_Ssd_mainmut_BIT_IDX, 0x0);

		/*IC workaround, mantis ID is 69087, begin*/
		//DSP_DRV_AudSetMasterVolume(0);

        //DSP_DRV_AudSetOutputPath(DSP_DRV_AUD_SSD_OUTPUT_DAC0,DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_L,DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_R);
		//DSP_DRV_AudSetOutputPath(DSP_DRV_AUD_SSD_OUTPUT_DAC1,DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_L,DSP_DRV_AUD_SSD_OUTPUT_SOURCE_MAIN_R);
		/*IC workaround, mantis ID is 69087, end*/
        /*Variables initial*/
        gfAudioInitFlag=TRUE;
   }
    else
    {
        aud_ret = AUD_WARN_NO_ACTION;
    }


    return aud_ret;
}
#endif
/******************************************************************************/
/**
* @brief    Configure audio effect mode. Some of sound effects in this path need cooperate with the other path.
*           This is application's responsibility to configure correct actions for those path.
* @param    Aud_AudioEffectMode_t eEffectMode:
* @return   PL_SUCCESS if successful
*           PL_ERR_INVALID_PARAM if parameters is unknown or pointer variable is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Set_EffectMode(Aud_AudioEffectMode_t eEffectMode)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;
    DSP_DRV_AudEffectMode_t mode;

    switch(eEffectMode)
    {
        case AUD_AUDIO_EFFECT_MODE_NORMAL:
            mode = DSP_DRV_AUD_EFFECT_MODE_DEFAULT;
            break;

		case AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL:
            mode = DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_XT_DPL;
            break;

        case AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX:
            mode = DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX;
            break;

		case AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL:
            mode = DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_HD_DPL;
            break;

        case AUD_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX:
            mode = DSP_DRV_AUD_EFFECT_MODE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX;
            break;

        default :
            mode = DSP_DRV_AUD_EFFECT_MODE_DEFAULT;
            aud_ret = AUD_ERR_INVALID_PARAM;
            break;
    }

    DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_3, BIT_0, mode);
  	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

    return aud_ret;
}

/******************************************************************************/
/**
* @brief  		Configure audio effect mode. Some of sound effects in this path need cooperate with the other path.
*           		This is application's responsibility to configure correct actions for those path.
* @param    	Aud_AudioEffectMode_t eEffectMode:
* @return   	PL_SUCCESS if successful
*           		PL_ERR_INVALID_PARAM if parameters is unknown or pointer variable is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Set_SUNSUR_OnOff(AUDIF_SUNSUR_Switch_e dVal)
{    
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SUNSUR_OFF,SUNSUR_ON);

    AUDIO_SEMAPHORE_TAKE();
	DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_15, BIT_15, (UINT16)dVal);

	// set fixed gain values temporarily
	// bit[10:8] Ambiance gain
	// bit[6:4] reverb gain
	// bit[2:0] direct gain
	//DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_10, BIT_8, (UINT16)SUNSUR_GAIN_0P9);
	//DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_6, BIT_4, (UINT16)SUNSUR_GAIN_0P7);
	//DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_2, BIT_0, (UINT16)SUNSUR_GAIN_0P7);

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	
    AUDIO_SEMAPHORE_GIVE();
	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SUNSUR_Param(Aud_SUNSUR_Param_t *sunsur_param)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;
    
    AUD_POSTPROC_CHK_RANGE(sunsur_param->ambiance_gain, SUNSUR_GAIN_0P0,SUNSUR_GAIN_1P0);
    AUD_POSTPROC_CHK_RANGE(sunsur_param->reverb_gain, SUNSUR_GAIN_0P0,SUNSUR_GAIN_1P0);
    AUD_POSTPROC_CHK_RANGE(sunsur_param->direct_gain, SUNSUR_GAIN_0P0,SUNSUR_GAIN_1P0);
    
    AUDIO_SEMAPHORE_TAKE();        
	DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_10, BIT_8, (UINT16)(sunsur_param->ambiance_gain));
	DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_6, BIT_4, (UINT16)(sunsur_param->reverb_gain));
	DSP_SetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_2, BIT_0, (UINT16)(sunsur_param->direct_gain));    

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
    AUDIO_SEMAPHORE_GIVE();
    
	return aud_ret;    
}

AUDIF_Status_t AUDIF_Get_SUNSUR_Param(Aud_SUNSUR_Param_t *sunsur_param)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
    
    (sunsur_param->ambiance_gain) = DSP_GetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_10, BIT_8);
    (sunsur_param->reverb_gain) = DSP_GetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_6, BIT_4);
    (sunsur_param->direct_gain) = DSP_GetBitMaskRiscParam0(RISC_SUNSUR_CTRL, BIT_2, BIT_0);

    AUDDRV_PRINT("AUDIF_Get_SUNSUR_Param:ambiance_gain  %d\n",sunsur_param->ambiance_gain);
    AUDDRV_PRINT("AUDIF_Get_SUNSUR_Param:reverb_gain    %d\n",sunsur_param->reverb_gain);
    AUDDRV_PRINT("AUDIF_Get_SUNSUR_Param:direct_gain    %d\n",sunsur_param->direct_gain);    
    
	return aud_ret;    
}
/******************************************************************************/
/**
* @brief    Configure spatial effect.
* @param    Aud_AudioSpatialEffect_t eSpatialEffect:
* @return   Aud_SUCCESS if successful
*           Aud_ERR_INVALID_PARAM if parameters is unknown or pointer variable is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Set_SpatialEffect(Aud_AudioSpatialEffect_t eSpatialEffect)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;
	DSP_DRV_AudSpatialEffect_t mode;

	switch(eSpatialEffect)
	{
	    case AUD_AUDIO_SPATIAL_EFFECT_NORMAL:
	        mode = DSP_DRV_AUD_SPATIAL_EFFECT_OFF;
	        break;

	    case AUD_AUDIO_SPATIAL_EFFECT_ESS:
	        mode = DSP_DRV_AUD_SPATIAL_EFFECT_I_STEREO;
	        break;

	    case AUD_AUDIO_SPATIAL_EFFECT_EPS:
	        mode = DSP_DRV_AUD_SPATIAL_EFFECT_I_MONO;
	        break;

	    case AUD_AUDIO_SPATIAL_EFFECT_SRS_WOW:
	        mode = DSP_DRV_AUD_SPATIAL_EFFECT_SRS_SPATIALIZER;
	        break;

	    case AUD_AUDIO_SPATIAL_EFFECT_AUTO_ESS_EPS:
	        mode = DSP_DRV_AUD_SPATIAL_EFFECT_AUTO_I_STEREO_I_MONO;
	        break;

	    default :
	        mode = DSP_DRV_AUD_SPATIAL_EFFECT_OFF;
	        aud_ret = AUD_ERR_INVALID_PARAM;
	        break;
	}

	if(mode == DSP_DRV_AUD_SPATIAL_EFFECT_AUTO_I_STEREO_I_MONO)
	{
	    DSP_SetSSDParam(REG_Ssd_isoundmode, REG_SSD_ISOUNDMODE_VALUE_AUTOMATIC_MODE);
	}
	else
	{
	    DSP_SetSSDParam(REG_Ssd_isoundmode, REG_SSD_ISOUNDMODE_VALUE_MANUAL_MODE);
	    DSP_SetSSDParam(REG_Ssd_somoctrl, mode);
	}

    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

    return aud_ret;
}

/******************************************************************************/
/**
* @brief    Configure backend effect. Some of sound effects in this path need cooperate with the other path.
*           This is application's responsibility to configure correct actions for those path.
* @param    Aud_AudioBackendEffect_t eBackendEffect:
* @return   Aud_SUCCESS if successful
*           Aud_ERR_INVALID_PARAM if parameters is unknown or pointer variable is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Set_BackEndEffect(Aud_AudioBackendEffect_t eBackendEffect)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	DSP_DRV_AudBassFeatureControl_t mode;
	switch(eBackendEffect)
	{
			case AUD_AUDIO_BAKCEND_EFFECT_NORMAL:
			mode = DSP_DRV_AUD_BASS_FEATURECONTROL_OFF;
			    break;

			case AUD_AUDIO_BAKCEND_EFFECT_DVB:
			    mode = DSP_DRV_AUD_BASS_FEATURECONTROL_DVB_MAIN;
			    break;

			case AUD_AUDIO_BAKCEND_EFFECT_DVB_SUBWOOFER:
			    mode = DSP_DRV_AUD_BASS_FEATURECONTROL_DVB_SUBWOOFER;
			    break;

			case AUD_AUDIO_BAKCEND_EFFECT_DBB:
			    mode = DSP_DRV_AUD_BASS_FEATURECONTROL_DBB_MAIN;
			    break;

			case AUD_AUDIO_BAKCEND_EFFECT_DBB_SUBWOOFER:
			    mode = DSP_DRV_AUD_BASS_FEATURECONTROL_DBB_SUBWOOFER;
			    break;

			default :
			    mode = DSP_DRV_AUD_BASS_FEATURECONTROL_OFF;
			    return AUD_ERR_INVALID_PARAM;
			    break;
	}

	DSP_SetSSDParam(REG_Ssd_bassfeaturectrl, mode);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Configure subwoofer.
* @param    pSubwoofer_config->eSubwooferSource : specify the subwoofer source
* @param    pSubwoofer_config->eSubwooferMode : specify the subwoofer mode
* @return   Aud_SUCCESS if successful
*           Aud_ERR_INVALID_PARAM if parameters is unknown or pointer variable is DTV_NULL
*           Aud_ERR_RESOURCE_UNAVAILABLE if the output source is not available over this
*			platform or is not match output type
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Subwoofer(Aud_AudioSubwooferConfig_t *pSubwoofer_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
    UINT8 mode, mode_subwoofer;   

	if(pSubwoofer_config->eSubwooferSource > AUD_AUDIO_SUBWOOFER_SOURCE_MAX_TYPE)
	{
        mode = AUD_AUDIO_SUBWOOFER_SOURCE_NONE;
	        aud_ret = AUD_ERR_INVALID_PARAM;
	}
    else
	{
        mode = pSubwoofer_config->eSubwooferSource;
    }    

    /*
    Bass Management Filtercharacteristics (50 - 400Hz Cornerfrequency)
    $0 = 50 Hz      $1 = 60 Hz      $2 = 70 Hz      $3 = 80 Hz
    $4 = 90 Hz      $5 = 100 Hz     $6 = 110 Hz     $7 = 120 Hz
    $8 = 130 Hz     $9 = 140 Hz     $A = 150 Hz     $B = 200 Hz
    $C = 250 Hz     $D = 300 Hz     $E = 350 Hz     $F = 400 Hz
    */

    // bit 15:12 low pass filter for LFE
    DSP_SetBitMaskRiscParam1(RISC_DOL_CTRL, BIT_15, BIT_12, pSubwoofer_config->eSubwooferLP);

    // bit 7:4 high pass filter for front
    DSP_SetBitMaskRiscParam1(RISC_DOL_CTRL, BIT_7, BIT_4, pSubwoofer_config->eSubwooferHP);

    if (pSubwoofer_config->eSubwooferMode > AUD_AUDIO_MAX_SUBWOOFER_MODE)
  {
        mode_subwoofer = AUD_AUDIO_NORMAL_SUBWOOFER_MODE;
        aud_ret = AUD_ERR_INVALID_PARAM;	    
  }
  else
  {
        mode_subwoofer = pSubwoofer_config->eSubwooferMode;
  }

    DSP_SetBitMaskRiscParam1(RISC_DOL_CTRL, BIT_9, BIT_8, mode);//set BMT mode 1/2/off
    DSP_SetBitMaskRiscParam1(RISC_DOL_CTRL, BIT_10, BIT_10, 1); //set LFE on
    DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_8, BIT_8, mode_subwoofer);
    AUDIO_SEMAPHORE_TAKE();
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    AUDIO_SEMAPHORE_GIVE();

	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set level adjust configuration
* @param    pLevelAdjust_config->bDECAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
* @param    pLevelAdjust_config->bMonoAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
* @param    pLevelAdjust_config->bNICAMAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
* @param    pLevelAdjust_config->bSAPAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
* @param    pLevelAdjust_config->bADCAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
* @param    pLevelAdjust_config->bDigitalAdjustLevel : -16(-16 dB, mute) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_LevelAdjust(Aud_AudioLevelAdjust_t *pLevelAdjust_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pLevelAdjust_config == NULL)
		return AUD_ERR_INVALID_PARAM;

    DSP_SetSSDParam(REG_Ssd_declev, pLevelAdjust_config->bDECAdjustLevel);
    DSP_SetSSDParam(REG_Ssd_monolev, pLevelAdjust_config->bMonoAdjustLevel);
    DSP_SetSSDParam(REG_Ssd_niclev, pLevelAdjust_config->bNICAMAdjustLevel);
    DSP_SetSSDParam(REG_Ssd_saplev, pLevelAdjust_config->bSAPAdjustLevel);
    DSP_SetSSDParam(REG_Ssd_adclev, pLevelAdjust_config->bADCAdjustLevel);
    DSP_SetSSDParam(REG_Ssd_diginlev, pLevelAdjust_config->bDigitalAdjustLevel);

    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set AVL configuration
* @param    pAVL_config->eMode : specify the decay time
* @param    pAVL_config->bReference_level : 0 =  -6 dB
*  										    1 =  -8 dB
*  										    2 = -10 dB, high threshold | (-10 dBFS), small reduction ("daytime mode")
*  							     		    3 = -12 dB
*  									        4 = -14 dB
*  										    5 = -16 dB
*  										    6 = -18 dB
*  										    7 = -20 dB, medium threshold | (-20 dBFS), medium reduction ("evening mode")
*  								            8 = -22 dB
*  										    9 = -24 dB
*  										   10 = -26 dB
*  										   11 = -28 dB
*  										   12 = -30 dB, low threshold | (-30 dBFS), strong reduction ("night mode")
*  										   13 = -32 dB
*  										   14 = -34 dB
*  										   15 = -36 dB
*
* @param    pAVL_config->fEnWeightFilter : Enable/Disable weight filter
* @param    pAVL_config->fEnPeakModeWithClipManagement : Enable/Disable Peak Mode With Clip Management
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_AVL(Aud_AudioAVLConfig_t *pAVL_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pAVL_config == NULL)
		return AUD_ERR_INVALID_PARAM;

	/*Set enable AVL */
	DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_6,BIT_6,pAVL_config->Enable_AVL_Func);
	/*Set AVL mode*/
	DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_7,BIT_4,pAVL_config->eMode);
	/*Set AVL reference volume level*/
	DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_12,BIT_8,pAVL_config->bReference_level);
	/*Set AVL weight filter enable/disable*/
	/*Set AVL peak mode with clip management enable/disable*/
	if(pAVL_config->fEnWeightFilter == TRUE)
	{
		DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_1,BIT_1,0x1);
	    	DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_2,BIT_2,0x1);
	}
	else
	{
		DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_1,BIT_1,0x0);
	    	DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_2,BIT_2,0x0);
	}

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	//AUDIF_Set_Stop(AUDIF_BANK_DEC1);
    //AUDIF_Set_Play(AUDIF_BANK_DEC1);

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set auto silence configuration
* @param    pAutoSilence_config->eChannel : AUD_AUDIO_PATH_PRIMARY/AUD_AUDIO_PATH_AUX1
* @param    pAutoSilence_config->fEnSilenceDetection : DTV_TRUE : enable auto silence detection
*													   DTV_FALSE : disable auto silence detection
* @param    pAutoSilence_config->bThreshold : 0 ~ 30(-30 - 3*bThreshold dBFS)
* @param    pAutoSilence_config->bDelay :  0 =  2^5 = 32 samples
*										   1 =  2^6 = 64 samples
*										   2 =  2^7 = 128 samples
*										   3 =  2^8 = 256 samples
*										   4 =  2^9 = 512 samples
*										   5 = 2^10 = 1024 samples
*										   6 = 2^11 = 2048 samples
*										   7 = 2^12 = 4096 samples
*										   8 = 2^13 = 8192 samples
*										   9 = 2^14 = 16384 samples
*										  10 = 2^15 = 32768 samples
*										  11 = 2^16 = 65536 samples
*										  12 = 2^17 = 131072 samples
*										  13 = 2^18 = 262144 samples
*										  14 = 2^19 = 524288 samples
*										  15 = 2^20 = 1048576 samples
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_AutoSilence(Aud_AudioAutoSilence_t *pAutoSilence_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	DSP_DRV_AudSilenceDetectionChannel_t mode;

	if(pAutoSilence_config == NULL)
		return AUD_ERR_INVALID_PARAM;

	if(pAutoSilence_config->eChannel == AUD_AUDIO_PATH_PRIMARY)
	{
		mode = DSP_DRV_AUD_SILENCE_DETECTION_CHANNEL_MAIN;
	}
	else if(pAutoSilence_config->eChannel == AUD_AUDIO_PATH_AUX1)
	{
		mode = DSP_DRV_AUD_SILENCE_DETECTION_CHANNEL_AUX1;
	}
	else
	{
		return AUD_ERR_INVALID_PARAM;
	}

	switch (mode)
	{
        case DSP_DRV_AUD_SILENCE_DETECTION_CHANNEL_MAIN:
            if (pAutoSilence_config->fEnSilenceDetection == TRUE)
            {
                DSP_SetSSDParam(REG_Ssd_sild_en_main, REG_SSD_SILD_EN_MAIN_VALUE_ENABLED);
			    DSP_SetSSDParam(REG_Ssd_sild_thr_main, pAutoSilence_config->bThreshold);
			    DSP_SetSSDParam(REG_Ssd_sild_delay_main, pAutoSilence_config->bDelay);
            }
            else
            {
			    DSP_SetSSDParam(REG_Ssd_sild_en_main, REG_SSD_SILD_EN_MAIN_VALUE_DISABLED);
			}
			break;

        case DSP_DRV_AUD_SILENCE_DETECTION_CHANNEL_AUX1:
            if (pAutoSilence_config->fEnSilenceDetection == TRUE)
			{
                DSP_SetSSDParam(REG_Ssd_sild_en_aux1, REG_SSD_SILD_EN_AUX1_VALUE_ENABLED);
			    DSP_SetSSDParam(REG_Ssd_sild_thr_aux1, pAutoSilence_config->bThreshold);
			    DSP_SetSSDParam(REG_Ssd_sild_delay_aux1, pAutoSilence_config->bDelay);
            }
			else
			{
			    DSP_SetSSDParam(REG_Ssd_sild_en_aux1, REG_SSD_SILD_EN_AUX1_VALUE_DISABLED);
			}
			break;

        default:
			aud_ret = DSP_DRV_ERR_INVALID_PARAM;
			break;
	}

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set clip management configuration
* @param    pClipManagement_config->eClipManagementMode : specify the clip management mode
* @param    pClipManagement_config->eDVMPeakDetectSelection : specify the channel for measurement
* @param    pClipManagement_config->bDVMRelease_range_upper_limit : 0 = -3,5dBFS
*																	1 = -6dBFS
*																	2 = -9dBFS
*																	3 = -12dBFS
*
* @param    pClipManagement_config->bDVMRelease_range_lower_limit : 0 = -40dBFS
*																	1 = -50dBFS
*																	2 = -60dBFS
*
* @param    pClipManagement_config->bDVMAttack_speed : 0 = 4.8dB/s
*													   1 = 9.6dB/s
*
* @param    pClipManagement_config->bDVMRelease_speed : 0 = 0.06dB/s
*														1 = 0.12dB/s
*														2 = 0.24dB/s
*														3 = 0.48dB/s
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if handler isn't registered
******************************************************************************/
AUDIF_Status_t AUDIF_Set_ClipManagement(Aud_AudioClipManagement_t *pClipManagement_config)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pClipManagement_config == NULL)
        return AUD_ERR_INVALID_PARAM;

	if(pClipManagement_config->eClipManagementMode > DSP_DRV_AUD_CLIP_MANAGEMENT_DYNAMIC_VOLUME_MODE)
    {
        aud_ret = AUD_ERR_INVALID_PARAM;
    }
    else
    {
        DSP_SetSSDParam(REG_Ssd_clipmanage, pClipManagement_config->eClipManagementMode);
    }

    if((pClipManagement_config->bDVMRelease_range_upper_limit > REG_Ssd_dvm_rr_upper_MAX)
        ||(pClipManagement_config->bDVMRelease_range_lower_limit > REG_Ssd_dvm_rr_lower_MAX)
        ||(pClipManagement_config->bDVMAttack_speed > REG_Ssd_dvm_attspeed_MAX)
        ||(pClipManagement_config->bDVMRelease_speed > REG_Ssd_dvm_relspeed_MAX))
        aud_ret = AUD_ERR_INVALID_PARAM;

    if(pClipManagement_config->eDVMPeakDetectSelection == DSP_DRV_AUD_DVM_PEAK_DETECTION_SELECT_MAIN)
    {
	    DSP_SetSSDParam(REG_Ssd_dvm_peak_sel, REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_MAIN_L_R);
    }
	else if(pClipManagement_config->eDVMPeakDetectSelection == DSP_DRV_AUD_DVM_PEAK_DETECTION_SELECT_MAIN_CENTER_SUBWOOFER)
    {
        DSP_SetSSDParam(REG_Ssd_dvm_peak_sel, REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_L_R_C_SUB);
    }
    else
    {
        aud_ret = AUD_ERR_INVALID_PARAM;
    }

    DSP_SetSSDParam(REG_Ssd_dvm_rr_upper, pClipManagement_config->bDVMRelease_range_upper_limit);
    DSP_SetSSDParam(REG_Ssd_dvm_rr_lower, pClipManagement_config->bDVMRelease_range_lower_limit);
    DSP_SetSSDParam(REG_Ssd_dvm_attspeed, pClipManagement_config->bDVMAttack_speed);
    DSP_SetSSDParam(REG_Ssd_dvm_relspeed, pClipManagement_config->bDVMRelease_speed);

    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set I sound configuration
* @param    pISound_config->eIntensityEffect : intensity selection for I sound
* @param    pISound_config->eFadeTime : ISound Fade Time
* @param    pISound_config->eDetectorTime : ISound Detector Time
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if handler isn't registered
******************************************************************************/
AUDIF_Status_t AUDIF_Set_ISound(Aud_AudioISound_t *pISound_config)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;

    UINT32 mapping_table[][2] =
	{
        {DSP_DRV_AUD_ISOUND_FADETIME_01_1_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_1_1SEC_136MSEC	},
        {DSP_DRV_AUD_ISOUND_FADETIME_02_2_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_2_2SEC_272MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_03_3_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_3_3SEC_410MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_04_5_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_4_5SEC_546MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_06_8_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_6_8SEC_820MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_09_0_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_9_0SEC_1_1SEC    	},
        {DSP_DRV_AUD_ISOUND_FADETIME_14_5_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_14_5SEC_1_8SEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_19_0_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_19_0SEC_2_4SEC   	},
        {DSP_DRV_AUD_ISOUND_FADETIME_136_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_1_1SEC_136MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_272_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_2_2SEC_272MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_410_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_3_3SEC_410MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_546_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_4_5SEC_546MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_820_MSEC,   REG_SSD_ISOUNDFADETIME_VALUE_6_8SEC_820MSEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_01_8_SEC,   REG_SSD_ISOUNDFADETIME_VALUE_14_5SEC_1_8SEC    },
        {DSP_DRV_AUD_ISOUND_FADETIME_02_4SEC,   REG_SSD_ISOUNDFADETIME_VALUE_19_0SEC_2_4SEC   	},
    };

    if((pISound_config->eIntensityEffect > DSP_DRV_AUD_ISOUND_INTENSITY_EFFECT_5)
        ||(pISound_config->eFadeTime > DSP_DRV_AUD_ISOUND_FADETIME_02_4SEC)
	    ||(pISound_config->eDetectorTime > DSP_DRV_AUD_ISOUND_DETECTORTIME_14SEC))
        return AUD_ERR_INVALID_PARAM;

	if((pISound_config->eFadeTime >= DSP_DRV_AUD_ISOUND_FADETIME_01_1_SEC)
	    && (pISound_config->eFadeTime <= DSP_DRV_AUD_ISOUND_FADETIME_19_0_SEC))
	{/*Set I sound Fader table selector to table1*/
	    DSP_SetSSDParam(REG_Ssd_isoundfadetimesel, REG_SSD_ISOUNDFADETIMESEL_VALUE_TABLE_1_1_1SEC_19SEC);
	}
	else if((pISound_config->eFadeTime >= DSP_DRV_AUD_ISOUND_FADETIME_136_MSEC)
	    && (pISound_config->eFadeTime <= DSP_DRV_AUD_ISOUND_FADETIME_02_4SEC))
	{/*Set I sound Fader table selector to table2*/
	    DSP_SetSSDParam(REG_Ssd_isoundfadetimesel, REG_SSD_ISOUNDFADETIMESEL_VALUE_TABLE_2_136MSEC_2_4SEC);
	}
	else
	{
	    return AUD_ERR_INVALID_PARAM;
	}

	/*Set I sound fade time*/
	DSP_SetSSDParam(REG_Ssd_isoundfadetime, mapping_table[pISound_config->eFadeTime][1]);
	DSP_SetSSDParam(REG_Ssd_insoef,pISound_config->eIntensityEffect);
	DSP_SetSSDParam(REG_Ssd_isounddettime,pISound_config->eDetectorTime);

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set Main Bass
* @param    INT32 dBass : bass value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if bass is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Bass(INT32 dBass)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dBass, AUD_AUDIO_MAIN_BASS_VALUE_MIN,AUD_AUDIO_MAIN_BASS_VALUE_MAX);

	// set risc_param1[RISC_MAIN_TON_CTRL] bit[4:0]
	DSP_SetBitMaskRiscParam1(RISC_MAIN_TON_CTRL, BIT_4, BIT_0, (UINT16)dBass);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Get Bass value
* @param    INT32 *pdRetBass: Bass value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pRetBass is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_Bass(INT32 *pdRetBass)
{
    INT16 bass;
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pdRetBass == NULL)
		return AUD_ERR_INVALID_PARAM;

	// get risc_param1[RISC_MAIN_TON_CTRL] bit[4:0] bith sign-extention
    bass = DSP_GetSignedBitMaskRiscParam1(RISC_MAIN_TON_CTRL, BIT_4, BIT_0);
    *pdRetBass = (INT32)bass;

    return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set Center Bass
* @param    INT32 dCenterBass : bass value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if dCenterBass is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_CenterBass(INT32 dCenterBass)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dCenterBass, AUD_AUDIO_CENTER_BASS_VALUE_MIN,AUD_AUDIO_CENTER_BASS_VALUE_MAX);

	// set risc_param1[RISC_CENTER_TON_CTRL] bit[4:0]
	DSP_SetBitMaskRiscParam1(RISC_CENTER_TON_CTRL, BIT_4, BIT_0, (UINT16)dCenterBass);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Get Center Bass value
* @param    INT32 *pdRetCenterBass: Bass value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pdRetCenterBass is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_CenterBass(INT32 *pdRetCenterBass)
{
    INT16 center_bass;

	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pdRetCenterBass == NULL)
        return AUD_ERR_INVALID_PARAM;

	// get risc_param1[RISC_CENTER_TON_CTRL] bit[4:0] with sign-extention
    center_bass = DSP_GetSignedBitMaskRiscParam1(RISC_CENTER_TON_CTRL, BIT_4, BIT_0);
    *pdRetCenterBass = (INT32)center_bass;

    return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set Surround Bass
* @param    INT32 dSurroundBass : bass value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if dSurroundBass is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_SurroundBass(INT32 dSurroundBass)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dSurroundBass, AUD_AUDIO_SURROUND_BASS_VALUE_MIN,AUD_AUDIO_SURROUND_BASS_VALUE_MAX);

    // set risc_param1[RISC_SURR_TON_CTRL] bit[4:0]
	DSP_SetBitMaskRiscParam1(RISC_SURR_TON_CTRL, BIT_4, BIT_0, (UINT16)dSurroundBass);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Get Surround Bass value
* @param    INT32 *pdRetSurroundBass: Bass value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pdRetSurroundBass is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_SurroundBass(INT32 *pdRetSurroundBass)
{
    INT16 surround_bass;
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pdRetSurroundBass == NULL)
		return AUD_ERR_INVALID_PARAM;

    // set risc_param1[RISC_SURR_TON_CTRL] bit[4:0] with sign-extention
    surround_bass = DSP_GetSignedBitMaskRiscParam1(RISC_SURR_TON_CTRL, BIT_4, BIT_0);

    *pdRetSurroundBass = (INT32)surround_bass;

    return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set Main treble
* @param    INT32 dTreble : treble value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if treble is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Treble(INT32 dTreble)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;

    AUD_POSTPROC_CHK_RANGE(dTreble, AUD_AUDIO_MAIN_TREBLE_VALUE_MIN,AUD_AUDIO_MAIN_TREBLE_VALUE_MAX);

    // set risc_param1[RISC_MAIN_TON_CTRL] bit12~8
    DSP_SetBitMaskRiscParam1(RISC_MAIN_TON_CTRL, BIT_12, BIT_8, (UINT16)dTreble);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

    return aud_ret;
}

/******************************************************************************/
/**
* @brief    Get Main Treble value
* @param    INT32 *pdRetTreble: Treble value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pdRetTreble is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_Treble(INT32 *pdRetTreble)
{
    INT16 treble;
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pdRetTreble == NULL)
		return AUD_ERR_INVALID_PARAM;

    // get risc_param1[RISC_MAIN_TON_CTRL] bit12~8  with sign-extention
	treble = DSP_GetSignedBitMaskRiscParam1(RISC_MAIN_TON_CTRL, BIT_12, BIT_8);

    *pdRetTreble = (INT32)treble;

    return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set Center treble
* @param    INT32 dCenterTreble : treble value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if treble is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_CenterTreble(INT32 dCenterTreble)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

    AUD_POSTPROC_CHK_RANGE(dCenterTreble, AUD_AUDIO_CENTER_TREBLE_VALUE_MIN,AUD_AUDIO_CENTER_TREBLE_VALUE_MAX);

    // set risc_param1[RISC_CENTER_TON_CTRL] bit12~8
	DSP_SetBitMaskRiscParam1(RISC_CENTER_TON_CTRL, BIT_12, BIT_8, (UINT16)dCenterTreble);

    return aud_ret;
}
/******************************************************************************/
/**
* @brief    Get Center Treble value
* @param    INT32 *pdRetCenterTreble: Treble value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pdRetCenterTreble is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_CenterTreble(INT32 *pdRetCenterTreble)
{
    INT16 center_treble;
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pdRetCenterTreble == NULL)
		return AUD_ERR_INVALID_PARAM;

    // get risc_param1[RISC_CENTER_TON_CTRL] bit12~8 with sign-extention
    center_treble = DSP_GetSignedBitMaskRiscParam1(RISC_CENTER_TON_CTRL, BIT_12, BIT_8);

    *pdRetCenterTreble = (INT32)center_treble;

    return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set Surround treble
* @param    INT32 dSurroundTreble : treble value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if treble is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_SurroundTreble(INT32 dSurroundTreble)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

    AUD_POSTPROC_CHK_RANGE(dSurroundTreble, AUD_AUDIO_SURROUND_TREBLE_VALUE_MIN,AUD_AUDIO_SURROUND_TREBLE_VALUE_MAX);

    // set risc_param1[RISC_SURR_TON_CTRL] bit12~8
    DSP_SetBitMaskRiscParam1(RISC_SURR_TON_CTRL, BIT_12, BIT_8, (UINT16)dSurroundTreble);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

  return aud_ret;
}
/******************************************************************************/
/**
* @brief    Get Surround Treble value
* @param    INT32 *pdRetdSurroundTreble: Treble value : -16(-16 dB) ~ 15(+15 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pdRetSurroundTreble is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_SurroundTreble(INT32 *pdRetSurroundTreble)
{
	INT16 surround_treble;
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pdRetSurroundTreble == NULL)
		return AUD_ERR_INVALID_PARAM;

    // get risc_param1[RISC_SURR_TON_CTRL] bit12~8 with sign-extention
	surround_treble = DSP_GetSignedBitMaskRiscParam1(RISC_SURR_TON_CTRL, BIT_12, BIT_8);

	*pdRetSurroundTreble = (INT32)surround_treble;

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set main equalizer value
* @param    pEQ_config->fEnBypass : Enable/Disable EQ process bypass
* @param    pEQ_config->bMainEQ_Band1_0100HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band2_0300HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band3_1000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band4_3000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band5_8000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if EQ is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_MainEqulizer(Aud_EqualizerConfig_t *pEQ_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pEQ_config == NULL)
		return AUD_ERR_INVALID_PARAM;

	if(pEQ_config->fEnBypass == TRUE)
    {
        DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_9, BIT_9, 0x1);
    }
    else
    {
        DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_9, BIT_9, 0x0);
    }

	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band1_0100HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band2_0300HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band3_1000HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band4_3000HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band5_8000HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);

    DSP_SetBitMaskRiscParam1(RISC_EQMAIN1_TON_CTRL, BIT_4, BIT_0, pEQ_config->bMainEQ_Band1_0100HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQMAIN1_TON_CTRL, BIT_9, BIT_5, pEQ_config->bMainEQ_Band2_0300HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQMAIN1_TON_CTRL, BIT_14,BIT_10,pEQ_config->bMainEQ_Band3_1000HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQMAIN2_TON_CTRL, BIT_4, BIT_0, pEQ_config->bMainEQ_Band4_3000HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQMAIN2_TON_CTRL, BIT_12,BIT_8, pEQ_config->bMainEQ_Band5_8000HZ);

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Get main equalizer
* @param    *pRetEQ_config->fEnBypass : Enable/Disable EQ process bypass
* @param    *pRetEQ_config->bMainEQ_Band1_0100HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band2_0300HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band3_1000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band4_3000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band5_8000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pRetEQ_config is DTV_NULL
******************************************************************************/

AUDIF_Status_t AUDIF_Get_MainEqulizer(Aud_EqualizerConfig_t *pRetEQ_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pRetEQ_config == NULL)
		return AUD_ERR_INVALID_PARAM;

	pRetEQ_config->fEnBypass = DSP_GetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_9, BIT_9);

    pRetEQ_config->bMainEQ_Band1_0100HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQMAIN1_TON_CTRL, BIT_4, BIT_0);
    pRetEQ_config->bMainEQ_Band2_0300HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQMAIN1_TON_CTRL, BIT_9, BIT_5);
    pRetEQ_config->bMainEQ_Band3_1000HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQMAIN1_TON_CTRL, BIT_14, BIT_10);
    pRetEQ_config->bMainEQ_Band4_3000HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQMAIN2_TON_CTRL, BIT_4, BIT_0);
    pRetEQ_config->bMainEQ_Band5_8000HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQMAIN2_TON_CTRL, BIT_12, BIT_8);

	return aud_ret;
}
/******************************************************************************/
/**
* @brief    Set center equalizer value
* @param    pEQ_config->fEnBypass : Enable/Disable EQ process bypass
* @param    pEQ_config->bMainEQ_Band1_0100HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band2_0300HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band3_1000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band4_3000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    pEQ_config->bMainEQ_Band5_8000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if EQ is out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_CenterEqulizer(Aud_EqualizerConfig_t *pEQ_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pEQ_config == NULL)
		return AUD_ERR_INVALID_PARAM;

	if(pEQ_config->fEnBypass == TRUE)
    {
        DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_9, BIT_9, 0x1);
    }
    else
    {
        DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_9, BIT_9, 0x0);
    }

	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band1_0100HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band2_0300HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band3_1000HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band4_3000HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);
	AUD_POSTPROC_CHK_RANGE(pEQ_config->bMainEQ_Band5_8000HZ,AUD_AUDIO_EQUALIZER_VALUE_MIN,AUD_AUDIO_EQUALIZER_VALUE_MAX);

    DSP_SetBitMaskRiscParam1(RISC_EQCENTER1_TON_CTRL, BIT_4, BIT_0, pEQ_config->bMainEQ_Band1_0100HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQCENTER1_TON_CTRL, BIT_9, BIT_5, pEQ_config->bMainEQ_Band2_0300HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQCENTER1_TON_CTRL, BIT_14,BIT_10,pEQ_config->bMainEQ_Band3_1000HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQCENTER2_TON_CTRL, BIT_4, BIT_0, pEQ_config->bMainEQ_Band4_3000HZ);
    DSP_SetBitMaskRiscParam1(RISC_EQCENTER2_TON_CTRL, BIT_12,BIT_8, pEQ_config->bMainEQ_Band5_8000HZ);

    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Get center equalizer
* @param    *pRetEQ_config->fEnBypass : Enable/Disable EQ process bypass
* @param    *pRetEQ_config->bMainEQ_Band1_0100HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band2_0300HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band3_1000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band4_3000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @param    *pRetEQ_config->bMainEQ_Band5_8000HZ : -12(-12 dB) ~ 12(+12 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pRetEQ_config is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_CenterEqulizer(Aud_EqualizerConfig_t *pRetEQ_config)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	if(pRetEQ_config == NULL)
		return AUD_ERR_INVALID_PARAM;

	pRetEQ_config->fEnBypass = DSP_GetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_9, BIT_9);

    pRetEQ_config->bMainEQ_Band1_0100HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQCENTER1_TON_CTRL, BIT_4, BIT_0);
    pRetEQ_config->bMainEQ_Band2_0300HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQCENTER1_TON_CTRL, BIT_9, BIT_5);
    pRetEQ_config->bMainEQ_Band3_1000HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQCENTER1_TON_CTRL, BIT_14, BIT_10);
    pRetEQ_config->bMainEQ_Band4_3000HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQCENTER2_TON_CTRL, BIT_4, BIT_0);
    pRetEQ_config->bMainEQ_Band5_8000HZ = DSP_GetSignedBitMaskRiscParam1(RISC_EQCENTER2_TON_CTRL, BIT_12, BIT_8);


	return aud_ret;
}

/******************************************************************************/
/**
* @brief    Set master volume according assigned volume gain
* @param    dMasterVolume : Master volume value : -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_MasterVolume(INT32 dMasterVolume)
{
	AUDIO_SEMAPHORE_TAKE();
	//AUD_POSTPROC_CHK_RANGE(dMasterVolume, -AUD_AUDIO_MASTER_VOLUME_VALUE_MIN, AUD_AUDIO_MASTER_VOLUME_VALUE_MAX);
	if ((dMasterVolume>AUD_AUDIO_MASTER_VOLUME_VALUE_MAX) || (dMasterVolume<AUD_AUDIO_MASTER_VOLUME_VALUE_MIN))
	{
		AUDDRV_PRINT("AUDIF_Set_MasterVolume::e:invalid param, should be +192~-672\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUD_ERR_INVALID_PARAM;
	}

	aud_param.master_volume = dMasterVolume;

	if( aud_param.master_mute_glag == MUTE_STATUS_ON )
	{
		DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, -672);
	}
	else
	{
		DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0, (UINT16)dMasterVolume);
	}

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	AUDIO_SEMAPHORE_GIVE();
	return AUD_SUCCESS;
}
/******************************************************************************/
/**
* @brief    Get master volume value
* @param    pdRetMasterVolume : Report master volume value : -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_FAILURE if pointer is NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_MasterVolume(INT32 *pdRetMasterVolume)
{
    if (NULL == pdRetMasterVolume)
    {
        AUDDRV_PRINT("AUDIF_Get_MasterVolume::e:NULL param\n");
        return AUD_ERR_FAILURE;
    }

    *pdRetMasterVolume = DSP_GetSignedBitMaskRiscParam1(RISC_MASTER_VOL, BIT_11,BIT_0);

    return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set Aux1 volume according assigned volume gain
* @param    dMasterVolume : Aux1 volume value : -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Aux1Volume(INT32 dAux1Volume)
{
	AUDIO_SEMAPHORE_TAKE();
	//AUD_POSTPROC_CHK_RANGE(dMasterVolume, -AUD_AUDIO_MASTER_VOLUME_VALUE_MIN, AUD_AUDIO_MASTER_VOLUME_VALUE_MAX);
	if ((dAux1Volume>AUD_AUDIO_MASTER_VOLUME_VALUE_MAX) || (dAux1Volume<AUD_AUDIO_MASTER_VOLUME_VALUE_MIN))
	{
		AUDDRV_PRINT("AUDIF_Set_Aux1Volume::e:invalid param, should be +192~-672\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUD_ERR_INVALID_PARAM;
	}
	AUDDRV_PRINT("AUDIF_Set_Aux1Volume::%d\n", dAux1Volume);
    
	aud_param.aux1_volume = dAux1Volume;

	if( aud_param.aux1_mute_glag == MUTE_STATUS_ON )
	{
		DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX1_VOL, BIT_11,BIT_0, -672);
	}
	else
	{
		DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX1_VOL, BIT_11,BIT_0, (UINT16)dAux1Volume);
	}    
    
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	AUDIO_SEMAPHORE_GIVE();
	return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get AUX1 volume value
* @param    pdRetMasterVolume : Report Aux1 volume value : -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_FAILURE if pointer is NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_Aux1Volume(INT32 *pdRetAux1Volume)
{
    if (NULL == pdRetAux1Volume)
    {
        AUDDRV_PRINT("AUDIF_Get_Aux1Volume::e:NULL param\n");
        return AUD_ERR_FAILURE;
    }

    *pdRetAux1Volume = DSP_GetSignedBitMaskRiscParam1(RISC_MAIN_AUX1_VOL, BIT_11,BIT_0);

    return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set Aux1 volume according assigned volume gain
* @param    dMasterVolume : Aux1 volume value : -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Aux2Volume(INT32 dAux2Volume)
{
	AUDIO_SEMAPHORE_TAKE();
	//AUD_POSTPROC_CHK_RANGE(dMasterVolume, -AUD_AUDIO_MASTER_VOLUME_VALUE_MIN, AUD_AUDIO_MASTER_VOLUME_VALUE_MAX);
	if ((dAux2Volume>AUD_AUDIO_MASTER_VOLUME_VALUE_MAX) || (dAux2Volume<AUD_AUDIO_MASTER_VOLUME_VALUE_MIN))
	{
		AUDDRV_PRINT("AUDIF_Set_Aux2Volume::e:invalid param, should be +192~-672\n");
		AUDIO_SEMAPHORE_GIVE();
		return AUD_ERR_INVALID_PARAM;
	}

	AUDDRV_PRINT("AUDIF_Set_Aux2Volume::%d\n", dAux2Volume);

	aud_param.aux2_volume = dAux2Volume;

	if( aud_param.aux2_mute_glag == MUTE_STATUS_ON )
	{
		DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX2_VOL, BIT_11,BIT_0, -672);
	}
	else
	{
		DSP_SetBitMaskRiscParam1(RISC_MAIN_AUX2_VOL, BIT_11,BIT_0, (UINT16)dAux2Volume);
	}
	
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	AUDIO_SEMAPHORE_GIVE();
	return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get AUX2 volume value
* @param    pdRetMasterVolume : Report Aux2 volume value : -672(-84 dB,mute) ~ 192(+24 dB),(1 step, 0.125 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_FAILURE if pointer is NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_Aux2Volume(INT32 *pdRetAux2Volume)
{
    if (NULL == pdRetAux2Volume)
    {
        AUDDRV_PRINT("AUDIF_Get_Aux2Volume::e:NULL param\n");
        return AUD_ERR_FAILURE;
    }

    *pdRetAux2Volume = DSP_GetSignedBitMaskRiscParam1(RISC_MAIN_AUX2_VOL, BIT_11,BIT_0);

    return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set balance according assigned left/right volume
* @param    dLeftVolume : Left channel volume value : -84(-84 dB,mute) ~ 24(+24 dB),(1 step, 1 dB)
* @param    dRightVolume : Right channel volume value : -84(-84 dB,mute) ~ 24(+24 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if parameters is unknown or out of range
******************************************************************************/
AUDIF_Status_t AUDIF_Set_Balance(INT32 dLeftVolume, INT32 dRightVolume)
{
    AUDIO_SEMAPHORE_TAKE();
    //AUD_POSTPROC_CHK_RANGE(dLeftVolume,AUD_AUDIO_MAIN_VOLUME_VALUE_MIN,AUD_AUDIO_MAIN_VOLUME_VALUE_MAX);
    //AUD_POSTPROC_CHK_RANGE(dRightVolume,AUD_AUDIO_MAIN_VOLUME_VALUE_MIN,AUD_AUDIO_MAIN_VOLUME_VALUE_MAX);
    if ((dLeftVolume>AUD_AUDIO_MAIN_VOLUME_VALUE_MAX) || (dLeftVolume<AUD_AUDIO_MAIN_VOLUME_VALUE_MIN))
    {
        AUDDRV_PRINT("AUDIF_Set_MasterVolume::e:invalid param, should be +192~-672\n");
        AUDIO_SEMAPHORE_GIVE();
        return AUD_ERR_INVALID_PARAM;
    }
    if ((dRightVolume>AUD_AUDIO_MAIN_VOLUME_VALUE_MAX) || (dRightVolume<AUD_AUDIO_MAIN_VOLUME_VALUE_MIN))
    {
        AUDDRV_PRINT("AUDIF_Set_MasterVolume::e:invalid param, should be +192~-672\n");
        AUDIO_SEMAPHORE_GIVE();
        return AUD_ERR_INVALID_PARAM;
    }
    DSP_SetBitMaskRiscParam1(RISC_MAIN_VOL, BIT_7, BIT_0, (UINT8)(dLeftVolume&0xFF));
    DSP_SetBitMaskRiscParam1(RISC_MAIN_VOL, BIT_15, BIT_8, (UINT8)(dRightVolume&0xFF));
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
    AUDIO_SEMAPHORE_GIVE();
    return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Get balance left channel and right channel volume value
* @param    pdRetLeftVolume : Report left channel volume value : -84(-84 dB,mute) ~ 24(+24 dB),(1 step, 1 dB)
* @param    pdRetRightVolume : Report right channel volume value : -84(-84 dB,mute) ~ 24(+24 dB),(1 step, 1 dB)
* @return   AUD_SUCCESS if successful
*           AUD_ERR_FAILURE if pointer is NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Get_Balance(INT32 *pdRetLeftVolume, INT32 *pdRetRightVolume)
{

    INT16 left_volume,right_volume;

	if((NULL == pdRetLeftVolume) ||(NULL == pdRetRightVolume))
    {
		return AUD_ERR_FAILURE;
    }

    left_volume = DSP_GetSignedBitMaskRiscParam1(RISC_MAIN_VOL, BIT_7, BIT_0);
    right_volume = DSP_GetSignedBitMaskRiscParam1(RISC_MAIN_VOL, BIT_15, BIT_8);
    *pdRetLeftVolume = left_volume;
    *pdRetRightVolume = right_volume;

    return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set main loudness configuration
* @param    pMainLoudness_config->fEnMainLoudness : DTV_TRUE : enable mainloudness process
*                                  				    DTV_FALSE : disable mainloudness process
* @param    pMainLoudness_config->fEnExtraBass : DTV_TRUE : filter character is 1000 Hz,
*											     DTV_FALSE : filter character is 500 Hz
* @param    pMainLoudness_config->eNoneAttackVolumeLevel : 0 = -15 dB
*  														   1 = -12 dB
*  														   2 =  -9 dB
*  														   3 =  -6 dB
*  														   4 =  -3 dB
*  														   5 =   0 dB
*  														   6 =  +3 dB
*  														   7 =  +6 dB
*  														   8 =  +9 dB
*  														   9 = +12 dB
*  														  10 = +15 dB
*  														  11 = +18 dB
*  														  12 = +21 dB
*  														  13 = +24 dB
*  														  14 = +27 dB
*  														  15 = +30 dB
* @return   AUD_SUCCESS if successful
*           AUD_ERR_INVALID_PARAM if pMainLoudness_config is DTV_NULL
******************************************************************************/
AUDIF_Status_t AUDIF_Set_MainLoudness(Aud_MainLoudnessConfig_t *pMainLoudness_config)
{
    AUDIF_Status_t aud_ret = AUD_SUCCESS;

	UINT32 mapping_table[][2] =
    {
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_15DB,    REG_SSD_MAINLONA_VALUE_MINUS_15DB_VOLUME_9DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_12DB,    REG_SSD_MAINLONA_VALUE_MINUS_12DB_VOLUME_12DB_VOLUME    },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_09DB,    REG_SSD_MAINLONA_VALUE_MINUS_9DB_VOLUME_15DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_06DB,    REG_SSD_MAINLONA_VALUE_MINUS_6DB_VOLUME_18DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_03DB,    REG_SSD_MAINLONA_VALUE_MINUS_3DB_VOLUME_21DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_00DB,     REG_SSD_MAINLONA_VALUE_0DB_VOLUME_24DB_VOLUME           },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_03DB,     REG_SSD_MAINLONA_VALUE_3DB_VOLUME_27DB_VOLUME           },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_06DB,     REG_SSD_MAINLONA_VALUE_6DB_VOLUME_30DB_VOLUME           },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_09DB,     REG_SSD_MAINLONA_VALUE_MINUS_15DB_VOLUME_9DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_12DB,     REG_SSD_MAINLONA_VALUE_MINUS_12DB_VOLUME_12DB_VOLUME    },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_15DB,     REG_SSD_MAINLONA_VALUE_MINUS_9DB_VOLUME_15DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_18DB,     REG_SSD_MAINLONA_VALUE_MINUS_6DB_VOLUME_18DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_21DB,     REG_SSD_MAINLONA_VALUE_MINUS_3DB_VOLUME_21DB_VOLUME     },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_24DB,     REG_SSD_MAINLONA_VALUE_0DB_VOLUME_24DB_VOLUME           },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_27DB,     REG_SSD_MAINLONA_VALUE_3DB_VOLUME_27DB_VOLUME           },
          {DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_30DB,     REG_SSD_MAINLONA_VALUE_6DB_VOLUME_30DB_VOLUME           },
    };

    if(pMainLoudness_config == NULL)
        return AUD_ERR_INVALID_PARAM;

    /*Set main loudness volume level*/
    DSP_SetSSDParam(REG_Ssd_mainlona, mapping_table[pMainLoudness_config->eNoneAttackVolumeLevel][1]);

    /*Set main loudness volume level*/
    if((pMainLoudness_config->eNoneAttackVolumeLevel >= DSP_DRV_AUD_MAINLOUDNESS_VOLUME_MINUS_15DB)
        &&(pMainLoudness_config->eNoneAttackVolumeLevel <= DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_06DB))
    {/*Set main loudness volume level extension to set1*/
        DSP_SetSSDParam(REG_Ssd_mainlonaext, REG_SSD_MAINLONAEXT_VALUE_SET_1_MINUS_15DB_6DB);
    }
    else if((pMainLoudness_config->eNoneAttackVolumeLevel >= DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_09DB)
        &&(pMainLoudness_config->eNoneAttackVolumeLevel <= DSP_DRV_AUD_MAINLOUDNESS_VOLUME_PLUS_30DB))
    {/*Set main loudness volume level extension to set2*/
        DSP_SetSSDParam(REG_Ssd_mainlonaext, REG_SSD_MAINLONAEXT_VALUE_SET_2_9DB_30DB);
    }
    else
    {
        return AUD_ERR_INVALID_PARAM;
    }

    /*Set mainloudness enable/disable*/
    if(pMainLoudness_config->fEnMainLoudness == TRUE)
    {
        DSP_SetSSDParam(REG_Ssd_mainloud, REG_SSD_MAINLOUD_VALUE_ON);
    }
    else
    {
        DSP_SetSSDParam(REG_Ssd_mainloud, REG_SSD_MAINLOUD_VALUE_OFF);
    }

    /*Set mainloudness extra bass enable/disable*/
    if(pMainLoudness_config->fEnExtraBass == TRUE)
    {
        DSP_SetSSDParam(REG_Ssd_mainloch, REG_SSD_MAINLOCH_VALUE_EXTRA_BASS_1000HZ);
    }
    else
	{
        DSP_SetSSDParam(REG_Ssd_mainloch, REG_SSD_MAINLOCH_VALUE_STANDARD_500HZ);
    }

	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

    return aud_ret;
}


AUDIF_Status_t AUDIF_Set_AdcSourceSelect(UINT8 dSelect)
{
    if (dSelect>AUDIF_ADC_SOURCE_MAX)
        return AUD_ERR_INVALID_PARAM;

	HWREG_W(sft_cfg_40_dtv, (0x60000 | dSelect));
    //HWREG_W(sft_cfg_40_dtv, (0xC0000 | dSelect));

    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Get_AdcSourceSelect(UINT8 *pdSelect)
{
    if(NULL == pdSelect)
		return AUD_ERR_INVALID_PARAM;

    *pdSelect = (HWREG_R(sft_cfg_40_dtv) & 0xF);

    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Set_ChannelMap(AUDIF_DacMapping_t *pChannelMap)
{
    UINT32 mapping = 0;
    AUDIF_Status_t retstatus = AUD_ERR_INVALID_PARAM;

    if (NULL == pChannelMap)
    {
        /*if NULL, set default value*/
        mapping = 0x00543210;
        retstatus = AUD_WARN_NO_ACTION;
    }
    else
    {
        mapping  = (pChannelMap->DacMapping_Data0_L)&AUDIF_DAC_MAP_PCM_MASK;
        mapping |= ((pChannelMap->DacMapping_Data0_R)&AUDIF_DAC_MAP_PCM_MASK)<<4;
        mapping |= ((pChannelMap->DacMapping_Data1_L)&AUDIF_DAC_MAP_PCM_MASK)<<8;
        mapping |= ((pChannelMap->DacMapping_Data1_R)&AUDIF_DAC_MAP_PCM_MASK)<<12;
        mapping |= ((pChannelMap->DacMapping_Data2_L)&AUDIF_DAC_MAP_PCM_MASK)<<16;
        mapping |= ((pChannelMap->DacMapping_Data2_R)&AUDIF_DAC_MAP_PCM_MASK)<<20;

        AUDDRV_PRINT("AUDIF_Set_ChannelMap::data0L:0x%02x\n",pChannelMap->DacMapping_Data0_L);
        AUDDRV_PRINT("AUDIF_Set_ChannelMap::data0R:0x%02x\n",pChannelMap->DacMapping_Data0_R);
        AUDDRV_PRINT("AUDIF_Set_ChannelMap::data1L:0x%02x\n",pChannelMap->DacMapping_Data1_L);
        AUDDRV_PRINT("AUDIF_Set_ChannelMap::data1R:0x%02x\n",pChannelMap->DacMapping_Data1_R);
        AUDDRV_PRINT("AUDIF_Set_ChannelMap::data2L:0x%02x\n",pChannelMap->DacMapping_Data2_L);
        AUDDRV_PRINT("AUDIF_Set_ChannelMap::data2R:0x%02x\n",pChannelMap->DacMapping_Data2_R);

        retstatus = AUD_SUCCESS;
    }

    AUDDRV_PRINT("AUDIF_Set_ChannelMap::mapping:0x%08x\n",mapping);

    HWREG_W(aud_grm_switch_0, mapping);
    HWREG_W(aud_grm_switch_int, mapping);

    return retstatus;
}

AUDIF_Status_t AUDIF_Get_ChannelMap(AUDIF_DacMapping_t *pRetChannelMap)
{
    UINT32 mapping;

    if (NULL == pRetChannelMap)
		return AUD_ERR_INVALID_PARAM;

    mapping = HWREG_R(aud_grm_switch_0);

    pRetChannelMap->DacMapping_Data0_L = mapping&AUDIF_DAC_MAP_PCM_MASK;
    pRetChannelMap->DacMapping_Data0_R = (mapping>>4)&AUDIF_DAC_MAP_PCM_MASK;
    pRetChannelMap->DacMapping_Data1_L = (mapping>>8)&AUDIF_DAC_MAP_PCM_MASK;
    pRetChannelMap->DacMapping_Data1_R = (mapping>>12)&AUDIF_DAC_MAP_PCM_MASK;
    pRetChannelMap->DacMapping_Data2_L = (mapping>>16)&AUDIF_DAC_MAP_PCM_MASK;
    pRetChannelMap->DacMapping_Data2_R = (mapping>>20)&AUDIF_DAC_MAP_PCM_MASK;
    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Set_MainChannelOutput(UINT8 mode)
{
    AUDIF_Status_t retstatus = AUD_SUCCESS;
  	AUDIO_SEMAPHORE_TAKE();
    UINT16 mainmode_curr, aux1mode_curr, aux2mode_curr;

    if (mode > 2)
    {
        AUDDRV_PRINT("AUDIF_Set_MainChannelOutput::e:invalid mode! set default value 0\n");
        mode = 0;
        retstatus = AUD_ERR_INVALID_PARAM;
    }

    mainmode_curr = DSP_GetRiscParam1(RISC_MAIN_MAINOUTPUT);        
    aux1mode_curr = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT, 7, 0);
    aux2mode_curr = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT,15, 8);

	//AUDDRV_PRINT("mainmode_curr : %d\n",mainmode_curr);
	//AUDDRV_PRINT("aux1mode_curr : %d\n",aux1mode_curr);
	//AUDDRV_PRINT("aux2mode_curr : %d\n",aux2mode_curr);

    if(mainmode_curr != mode)
    {
        // call A0 rampdown??
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x03_A0_RAMP_DOWN,0,0);
    }

    switch(mode)
    {            
        case AUDIF_MAIN_PATH_SIF:
            if ((mainmode_curr != AUDIF_MAIN_PATH_SIF) &&
                (aux1mode_curr != AUDIF_AUX1_PATH_SIF) &&
                (aux2mode_curr != AUDIF_AUX2_PATH_SIF))
            {
                DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x01_SIF_INPUT,0x0,0x0);
            }
            break;

        case AUDIF_MAIN_PATH_AUX:
            if ((mainmode_curr != AUDIF_MAIN_PATH_AUX) &&
                (aux1mode_curr != AUDIF_AUX1_PATH_AUX) &&
                (aux2mode_curr != AUDIF_AUX2_PATH_AUX))
            {
                DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x02_AUX_INPUT,0x0,0x0);
            }            
            break;
            
        case AUDIF_MAIN_PATH_MIXER:    
        default:
            break;            
    }

    DSP_SetRiscParam1(RISC_MAIN_MAINOUTPUT, mode);    
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

    // call A0 rampup ??
    DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x02_A0_RAMP_UP,0,0);
    
   	AUDIO_SEMAPHORE_GIVE();
    return retstatus;
}

AUDIF_Status_t AUDIF_Set_AuxChannelOutput(UINT8 aux1mode, UINT8 aux2mode)
{
    AUDIF_Status_t retstatus = AUD_SUCCESS;
    UINT16 mainmode_curr, aux1mode_curr, aux2mode_curr;
    
   	AUDIO_SEMAPHORE_TAKE();

    if (aux1mode > AUDIF_AUX1_PATH_MAX)
    {
        AUDDRV_PRINT("AUDIF_Set_AuxOutput::e:invalid aux1 mode! set default OFF\n");
        aux1mode = AUDIF_AUX1_PATH_OFF;
        retstatus = AUD_ERR_INVALID_PARAM;
    }

    if (aux2mode > AUDIF_AUX2_PATH_MAX)
    {
        AUDDRV_PRINT("AUDIF_Set_AuxOutput::e:invalid aux2 mode! set default OFF\n");
        aux2mode = AUDIF_AUX2_PATH_OFF;
        retstatus = AUD_ERR_INVALID_PARAM;
    }

    mainmode_curr = DSP_GetRiscParam1(RISC_MAIN_MAINOUTPUT);            
    aux1mode_curr = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT, 7, 0);
    aux2mode_curr = DSP_GetBitMaskRiscParam1(RISC_MAIN_AUXOUTPUT,15, 8);    
    
    if(aux1mode_curr != aux1mode)
    {
        //call A1 rampdown?
        DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x05_A1_RAMP_DOWN,0,0);
            }
            
    switch(aux1mode)
            {
        case AUDIF_AUX1_PATH_SIF:
            AUDDRV_PRINT("AUDIF_Set_AuxChannelOutput::Aux1 output SIF\n");
            if ((mainmode_curr != AUDIF_MAIN_PATH_SIF) &&
                (aux1mode_curr != AUDIF_AUX1_PATH_SIF) &&
                (aux2mode_curr != AUDIF_AUX2_PATH_SIF))
            {
                AUDDRV_PRINT("AUDIF_Set_AuxChannelOutput::reset SIF input buffer\n");
                DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x01_SIF_INPUT,0x0,0x0);
            }
            break;

        case AUDIF_AUX1_PATH_AUX:
            AUDDRV_PRINT("AUDIF_Set_AuxChannelOutput::Aux1 output AUX\n");
            if ((mainmode_curr != AUDIF_MAIN_PATH_AUX) &&
                (aux1mode_curr != AUDIF_AUX1_PATH_AUX) &&
                (aux2mode_curr != AUDIF_AUX2_PATH_AUX))
            {
                AUDDRV_PRINT("AUDIF_Set_AuxChannelOutput::reset AUX input buffer\n");
                DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x02_AUX_INPUT,0x0,0x0);
            }            
            break;
            
        case AUDIF_AUX1_PATH_LFE:
            AUDDRV_PRINT("AUDIF_Set_AuxChannelOutput::Aux1 output MAIN LFE\n");
            break;
            
        case AUDIF_AUX1_PATH_MIXER:                        
        case AUDIF_AUX1_PATH_OFF:
        default:
            break;            
    }

    if(aux2mode_curr != aux2mode)
            {
        //call A2 rampdown?
            }            
            
    switch(aux2mode)
            {
        case AUDIF_AUX2_PATH_SIF:
            if ((mainmode_curr != AUDIF_MAIN_PATH_SIF) &&
                (aux1mode_curr != AUDIF_AUX1_PATH_SIF) &&
                (aux2mode_curr != AUDIF_AUX2_PATH_SIF))
            {
                DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x01_SIF_INPUT,0x0,0x0);
            }
            break;

        case AUDIF_AUX2_PATH_AUX:
            if ((mainmode_curr != AUDIF_MAIN_PATH_AUX) &&
                (aux1mode_curr != AUDIF_AUX1_PATH_AUX) &&
                (aux2mode_curr != AUDIF_AUX2_PATH_AUX))
            {
                DSP_SendCmd(RISC_CMD0x18_PCM_INPUT_REINIT,CODE0x02_AUX_INPUT,0x0,0x0);
            }            
            break;
        case AUDIF_AUX2_PATH_MIXER:            
        case AUDIF_AUX2_PATH_OFF:
        default:
            break;            
    }

	auxmode_backup = ((aux2mode<<8)|aux1mode);    
    AUDDRV_PRINT("AUDIF_Set_AuxOutput::aux mode 0x%02x 0x%02x\n", aux1mode, aux2mode);
    
    DSP_SetRiscParam1(RISC_MAIN_AUXOUTPUT, ((aux2mode<<8)|aux1mode));
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
   	AUDIO_SEMAPHORE_GIVE();

    //GL_TaskSleep(90);    
	msleep(90);

   	AUDIO_SEMAPHORE_TAKE();    
    DSP_SendCmd(RISC_CMD0x03_RAMP,CODE0x04_A1_RAMP_UP,0,0);    
   	AUDIO_SEMAPHORE_GIVE();    
    return retstatus;
}

AUDIF_Status_t AUDIF_Set_LipSync_DelayTime(UINT16 ms)
{
    	UINT16 samples;
	UINT32 smp_rate;
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	smp_rate= aud_map_SmpRate_FStoDigital(AUDIF_Get_SampleRate()); //get DAC smp rate
	samples = ms*smp_rate/1000;                                                                                
	AUD_POSTPROC_CHK_RANGE(samples, 0, 8064);   // max length 252 ms * 32 kHz sample rate
	//DSP_SetBitMaskRiscParam1(RISC_SOU_APP_MOD_L, BIT_7, BIT_7, 0x1); // 0: lip sync disable; 1: lip sync enable
	DSP_SetBitMaskRiscParam1(RISC_MAIN_LIPSYNC, BIT_15, BIT_0, (UINT16)samples);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
	return aud_ret;
}
AUDIF_Status_t AUDIF_Set_SwApp2Switch(UINT8 mode)
{
    //0=disable, 1=enable
    if ((mode == 0) || (mode == 1))
    {
        AUDIO_SEMAPHORE_TAKE();
	    DSP_SetBitMaskRiscParam1(RISC_MAIN_SOU_EFF, BIT_0, BIT_0, mode);
        DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);
        AUDIO_SEMAPHORE_GIVE();
        return AUD_SUCCESS;
    }
    else
    {
        return AUD_ERR_INVALID_PARAM;
    }
}

AUDIF_Status_t AUDIF_Set_HDMIInput_SmpRate(UINT32 smp_rate)
{
    //hdmiin_default_fs = smp_rate;
    //AUDDRV_PRINT("[AUDDRV] HDMI input smp-rate:%d, %d\n", smp_rate, hdmiin_default_fs);
	//AUDIF_Update_SpdifIn();
    
    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_OnOff(AUDIF_SRSTSHD_Switch_e dVal)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SRSTSHD_OFF,SRSTSHD_ON);

	AUDIO_SEMAPHORE_TAKE();
	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_15, BIT_15, (UINT16)dVal);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
    AUDIO_SEMAPHORE_GIVE();

	/*    
    AUDIO_SEMAPHORE_TAKE();
    AUDIF_Set_SRSTSHD_SurroundOnOff(SRSTSHD_ON);
    AUDIF_Set_SRSTSHD_SurroundControl(SRSTSHD_CONTROL_0P6);
    AUDIF_Set_SRSTSHD_DialogControl(SRSTSHD_CONTROL_0P5);
    AUDIF_Set_SRSTSHD_DefinitionFrontControl(SRSTSHD_CONTROL_0P6);
    AUDIF_Set_SRSTSHD_DefinitionCenterControl(SRSTSHD_CONTROL_0P6);
    AUDIF_Set_SRSTSHD_TrubassFrontMode(SRSTSHD_STEREO);
    AUDIF_Set_SRSTSHD_TrubassFrontControl(SRSTSHD_CONTROL_0P3);
    AUDIF_Set_SRSTSHD_TrubassSubControl(SRSTSHD_CONTROL_0P3);
    AUDIF_Set_SRSTSHD_TrubassFrontSize(SRSTSHD_SPK_150HZ);
    AUDIF_Set_SRSTSHD_TrubassSubSize(SRSTSHD_SPK_40HZ);
    AUDIF_Set_SRSTSHD_TrubassSubCrossover(SRSTSHD_CROSSOVER_80HZ);
    AUDIF_Set_SRSTSHD_SRS3dOnOff(SRSTSHD_ON);
    AUDIF_Set_SRSTSHD_SRS3dBitrate(SRSTSHD_HIGH);
    AUDIF_Set_SRSTSHD_SRS3dMode(SRSTSHD_3DSTEREO);
    AUDIF_Set_SRSTSHD_SRS3dCenterControl(SRSTSHD_CONTROL_0P5);
    AUDIF_Set_SRSTSHD_SRS3dSpaceControl(SRSTSHD_CONTROL_0P8);
    AUDIF_Set_SRSTSHD_FocusControl(SRSTSHD_CONTROL_0P4);
    AUDIF_Set_SRSTSHD_LimiterOnOff(SRSTSHD_OFF);
    AUDIF_Set_SRSTSHD_LimiterControl(SRSTSHD_CONTROL_0P7);
    AUDIO_SEMAPHORE_GIVE();
    */
	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SurroundOnOff(AUDIF_SRSTSHD_Switch_e dVal)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SRSTSHD_OFF,SRSTSHD_ON);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_11, BIT_11, (UINT16)dVal);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SurroundControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_14, BIT_12, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_DialogControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_10, BIT_8, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_DefinitionFrontControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_6, BIT_4, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_DefinitionCenterControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_2, BIT_0, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassFrontMode(AUDIF_SRSTSHD_FrontMode_e dMode)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dMode, SRSTSHD_MONO,SRSTSHD_STEREO);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUBASS_CTRL, BIT_11, BIT_11, (UINT16)dMode);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassFrontControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUBASS_CTRL, BIT_14, BIT_12, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassSubControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUBASS_CTRL, BIT_7, BIT_5, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassFrontSize(AUDIF_SRSTSHD_SpkSize_e dSize)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dSize, SRSTSHD_SPK_40HZ,SRSTSHD_SPK_400HZ);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUBASS_CTRL, BIT_10, BIT_8, (UINT16)dSize);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassSubSize(AUDIF_SRSTSHD_SpkSize_e dSize)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dSize, SRSTSHD_SPK_40HZ,SRSTSHD_SPK_200HZ);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUBASS_CTRL, BIT_4, BIT_2, (UINT16)dSize);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_TrubassSubCrossover(AUDIF_SRSTSHD_Crossover_e dSize)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dSize, SRSTSHD_CROSSOVER_80HZ,SRSTSHD_CROSSOVER_200HZ);

	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUBASS_CTRL, BIT_1, BIT_0, (UINT16)dSize);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dOnOff(AUDIF_SRSTSHD_Switch_e dVal)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SRSTSHD_OFF,SRSTSHD_ON);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_15, BIT_15, (UINT16)dVal);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dBitrate(AUDIF_SRSTSHD_3dBitrate_e dVal)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SRSTSHD_LOW,SRSTSHD_HIGH);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_11, BIT_11, (UINT16)dVal);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dMode(AUDIF_SRSTSHD_3dMode_e dVal)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SRSTSHD_3DSTEREO,SRSTSHD_EXTREME);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_7, BIT_7, (UINT16)dVal);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dCenterControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_10, BIT_8, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_SRS3dSpaceControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_14, BIT_12, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_FocusControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_6, BIT_4, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_LimiterOnOff(AUDIF_SRSTSHD_Switch_e dVal)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;

	AUD_POSTPROC_CHK_RANGE(dVal, SRSTSHD_OFF,SRSTSHD_ON);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_3, BIT_3, (UINT16)dVal);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_LimiterControl(AUDIF_SRSTSHD_Control_e dGain)
{
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	
	AUD_POSTPROC_CHK_RANGE(dGain, SRSTSHD_CONTROL_OFF,SRSTSHD_CONTROL_1P0);

	DSP_SetBitMaskRiscParam0(RISC_SRS_WOW_CTRL, BIT_2, BIT_0, (UINT16)dGain);
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);

	return aud_ret;
}

AUDIF_Status_t AUDIF_Set_SRSTSHD_ChannelMode(UINT8 mode)
{
	// mode : 0x0=ordinary stereo    0x1=LtRt
	AUDIF_Status_t aud_ret = AUD_SUCCESS;
	UINT8 ch_mode = 0;

	if (mode==0)
	{
		ch_mode = 0;
	}
	else if(mode==1)
	{
		ch_mode = 1;
	}
	else
	{
		ch_mode = 0;
		aud_ret = AUD_ERR_INVALID_PARAM;
	}
	
	DSP_SetBitMaskRiscParam0(RISC_SRS_TRUHD_CTRL, BIT_3, BIT_3, ch_mode);

    AUDIO_SEMAPHORE_TAKE();	
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x02_SYNC_RISC_PARAM,0x0,0x0);
	AUDIO_SEMAPHORE_GIVE();	
	return aud_ret;
}
#if 0

/******************************************************************************
    Demdec related
 *****************************************************************************/
Aud_FrontEndInfo_t gAudioFrontEndInfo;
Aud_DemDecInfo_t gAudDemDecInfo;

void _Demdec_Get_DspInfo(void)
{
    UINT16 value_l, value_h;

    value_h = DSP_GetDspParam1(DSP_INF_DEV_STA_H);
    value_l = DSP_GetDspParam1(DSP_INF_DEV_STA_L);
    gAudDemDecInfo._INF_DEV_STA = ((value_l<<16) | value_h);

    value_h = DSP_GetDspParam1(DSP_INF_NIC_STA_H);
    value_l = DSP_GetDspParam1(DSP_INF_NIC_STA_L);
    gAudDemDecInfo._INF_NIC_STA = ((value_l<<16) | value_h);

    value_h = DSP_GetDspParam1(DSP_INF_NIC_ADD);
    gAudDemDecInfo._INF_NIC_ADD = value_h;

    value_h = DSP_GetDspParam1(DSP_INF_LEV_MON_H);
    value_l = DSP_GetDspParam1(DSP_INF_LEV_MON_L);
    gAudDemDecInfo._INF_LEV_MON = ((value_l<<16) | value_h);

    value_h = DSP_GetDspParam1(DSP_INF_MPX_LEV_H);
    value_l = DSP_GetDspParam1(DSP_INF_MPX_LEV_L);
    gAudDemDecInfo._INF_MPX_LEV = ((value_l<<16) | value_h);

    value_h = DSP_GetDspParam1(DSP_INF_NOISELEV_H);
    value_l = DSP_GetDspParam1(DSP_INF_NOISELEV_L);
    gAudDemDecInfo._INF_NOISELEV = ((value_l<<16) | value_h);

    value_h = DSP_GetDspParam1(DSP_INF_OVMADAPT_H);
    value_l = DSP_GetDspParam1(DSP_INF_OVMADAPT_L);
    gAudDemDecInfo._INF_OVMADAPT= ((value_l<<16) | value_h);

}

//
// determine the SIF mode depending on global variables (gAudDemDecInfo)
//
UINT16 _Demdec_Get_SifMode(void)
{
    UINT8 fsap = 0, fdual = 0, fstereo = 0, fnicam = 0;
    UINT16 mode = AUD_SIF_AVAIL_BUSY;

    // check SAP
    if(gAudDemDecInfo.INF_DEV_STA.SAPDET)
    {
        if(gAudDemDecInfo.INF_DEV_STA.SAMUT)
        {
            fsap = 0;
        }
        else
        {
            fsap = 1;
        }
    }

    // check dual
    if(gAudDemDecInfo.INF_DEV_STA.GDU)
    {
        fdual = 1;
    }

    // check stereo
    if(AUD_SIF_STANDARD_M_BTSC == gAudDemDecInfo.INF_DEV_STA.STDRES)
    {
        if ((gAudDemDecInfo.INF_DEV_STA.BPILOT)
            && (!gAudDemDecInfo.INF_DEV_STA.BAMUT))
        {
            fstereo = 1;
        }
    }
    else
    {
        if(gAudDemDecInfo.INF_DEV_STA.GST)
        {
            fstereo = 1;
        }
    }

    // check nicam
    if(gAudDemDecInfo.INF_DEV_STA.VDSP_C)
    {
        if(gAudDemDecInfo.INF_DEV_STA.NAMUT)
        {
            fnicam = 0;
        }
        else
        {
            fnicam = 1;
        }
    }

    // determine the standard
    if (fnicam == 1)
    {
        mode |= (AUD_SIF_AVAIL_NICAM | AUD_SIF_AVAIL_ANALOG_MONO);

        if (fdual == 1)
        {
            mode |= AUD_SIF_AVAIL_DUAL;
        }
        else if (fstereo == 1)
        {
            mode |= AUD_SIF_AVAIL_STEREO;
        }
        else
        {
            mode |= AUD_SIF_AVAIL_NICAM_MONO;
        }
    }
    else
    {
        if (fsap == 1)
        {
            mode |= AUD_SIF_AVAIL_SAP;
            if (fstereo == 1)
            {
                mode |= (AUD_SIF_AVAIL_ANALOG_MONO|AUD_SIF_AVAIL_STEREO);
            }
            else
            {
                mode |= AUD_SIF_AVAIL_ANALOG_MONO;
            }
        }
        else
        {
            if (fstereo == 1)
            {/*if get A2 standard, and dual or stereo is available, mono mode should be unavailable*/
                if ((AUD_SIF_STANDARD_BG_A2 == gAudDemDecInfo.INF_DEV_STA.STDRES)
                    ||(AUD_SIF_STANDARD_DK_A2_1 == gAudDemDecInfo.INF_DEV_STA.STDRES)
                    ||(AUD_SIF_STANDARD_DK_A2_2 == gAudDemDecInfo.INF_DEV_STA.STDRES)
                    ||(AUD_SIF_STANDARD_DK_A2_3 == gAudDemDecInfo.INF_DEV_STA.STDRES))
                {
                    mode |= AUD_SIF_AVAIL_STEREO;
                }
                else
                {
                    mode |= (AUD_SIF_AVAIL_STEREO | AUD_SIF_AVAIL_ANALOG_MONO);
                }
            }
            else
            {/*if get A2 standard, and dual or stereo is available, mono mode should be unavailable*/
                if (fdual == 1)
                {
                    if ((AUD_SIF_STANDARD_BG_A2 == gAudDemDecInfo.INF_DEV_STA.STDRES)
                        ||(AUD_SIF_STANDARD_DK_A2_1 == gAudDemDecInfo.INF_DEV_STA.STDRES)
                        ||(AUD_SIF_STANDARD_DK_A2_2 == gAudDemDecInfo.INF_DEV_STA.STDRES)
                        ||(AUD_SIF_STANDARD_DK_A2_3 == gAudDemDecInfo.INF_DEV_STA.STDRES))
                    {
                        mode |= AUD_SIF_AVAIL_DUAL;
                    }
                    else
                    {
                        mode |= (AUD_SIF_AVAIL_DUAL | AUD_SIF_AVAIL_ANALOG_MONO);
                    }
                }
                else
                {
                    mode |= AUD_SIF_AVAIL_ANALOG_MONO;
                }
            }
        }
    }

    return mode;
}

INT16 _Demdec_Get_SifAgcGain(void)
{
    INT16 offset, value;

    offset = HW_SSD_AGC_STATUS_V_MAX - AUD_SIF_AGC_MAX;
    value = HWREG_R(g305_reserved[6])&0x1F;

    return (value - offset);
}

AUDIF_Status_t _Demdec_Set_SifStandardMask(Aud_SIFStandardMask_e eStandardMask)
{
    AUDIF_Status_t retstatus = AUD_SUCCESS;

    AUDDRV_PRINT("_Demdec_Set_SifStandardMask:eStandardMask = %d\n", eStandardMask);

    if(eStandardMask <= AUD_SIF_STANDARD_ASD_MODE_MASK_ALL)
    {
        AUDDRV_PRINT("_Demdec_Set_SifStandardMask:Set ASD mode\n");
	    DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_9, BIT_8, 0); // Set EPMODE = ASD mode
	    DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, eStandardMask);
    }
    else
    {
        AUDDRV_PRINT("_Demdec_Set_SifStandardMask:Set SSS mode\n");
	    DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_9, BIT_8, 1); // Set EPMODE = SSS mode

        switch(eStandardMask)
        {
            case AUD_SIF_STANDARD_SSS_MODE_BG:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_B_G);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_DK:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_D_K);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_M:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_M);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_BG_A2:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_B_G_A2);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_BG_NICAM:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_B_G_NICAM);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_DK_A2_1:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_D_K_A2_1);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_DK_A2_2:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_D_K_A2_2);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_DK_A2_3:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_D_K_A2_3);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_DK_NICAM:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_D_K_NICAM);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_L_NICAM:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_L_NICAM_L);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_I_NICAM:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_I_NICAM);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_M_KOREA:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_M_KOREA);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_M_BTSC:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_M_BTSC);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_M_EIAJ:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_M_EIAJ);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_FM_10_7_IF_50_US:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_FM_RADIO_IF_10_7_MHZ_50_US_DEEMPHASIS);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_FM_10_7_IF_75_US:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_FM_RADIO_IF_10_7_MHZ_75_US_DEEMPHASIS);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_50_US:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_FM_RADIO_SELECTABLE_IF_50_US_DEEMPHASIS);
                break;

            case AUD_SIF_STANDARD_SSS_MODE_FM_SELECTABLE_IF_75_US:
                DSP_SetBitMaskRiscParam1(RISC_DDEP_CTRL_L, BIT_7, BIT_0, DSP_STDSEL_VALUE_FM_RADIO_SELECTABLE_IF_50_US_DEEMPHASIS);
                break;

            default:
                retstatus = AUD_ERR_INVALID_PARAM;
        }
    }

    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);

    return retstatus;

}
AUDIF_Status_t AUDIF_Get_FrontEndStatus(Aud_FrontEndParam_t *pFrontEndParam)
{
    AUDIF_Status_t retstatus = AUD_SUCCESS;

	if(NULL == pFrontEndParam)
    {
		return AUD_ERR_FAILURE;
    }

    if(pFrontEndParam->eMethod == AUD_REPORT_DIRECT)
    {
        _Demdec_Get_DspInfo();  //get dsp_param1

        // update global variables
        gAudioFrontEndInfo.Status.eStandard   = gAudDemDecInfo.INF_DEV_STA.STDRES;
        gAudioFrontEndInfo.Status.eMode       = _Demdec_Get_SifMode();
        gAudioFrontEndInfo.Status.bSIFAGCGain = _Demdec_Get_SifAgcGain();
        gAudioFrontEndInfo.Status.bNICAMError = gAudDemDecInfo.INF_NIC_STA.ERR_OUT;

        if (AUD_SIF_STANDARD_BUSY == gAudioFrontEndInfo.Status.eStandard)
        {
            pFrontEndParam->eRetStandard = AUD_SIF_STANDARD_FAIL;
        }
        else
        {
            pFrontEndParam->eRetStandard    = gAudioFrontEndInfo.Status.eStandard;
        }

        if(gAudioFrontEndInfo.Status.eMode & AUD_SIF_AVAIL_NICAM)
		{
			pFrontEndParam->bNICAMError = gAudioFrontEndInfo.Status.bNICAMError;
		}
		else
		{
			pFrontEndParam->bNICAMError = AUD_NICAM_ERROR_MAX;
		}

        pFrontEndParam->eRetMode        = gAudioFrontEndInfo.Status.eMode;
        pFrontEndParam->bRetSIFAGCGain  = gAudioFrontEndInfo.Status.bSIFAGCGain;


    }
    else
    {
        /*Get last global variables*/
        AUDDRV_PRINT("AUDIF_Get_FrontEndStatus:w::temp no indirect mode\n");
    }


    return retstatus;
}

AUDIF_Status_t AUDIF_Set_SifAgcConfig(UINT8 FMmodulation, Aud_SIFAGCConfig_t *pSIFAFCConfig)
{
    INT16 offset;
    INT32 mode;

    if (NULL == pSIFAFCConfig)
    {
        return AUD_ERR_FAILURE;
    }

    if (FMmodulation)
    {
        memcpy((Aud_SIFAGCConfig_t*)&gAudioFrontEndInfo.Control.FM_AGCConfigParam,(Aud_SIFAGCConfig_t*)pSIFAFCConfig,sizeof(Aud_SIFAGCConfig_t));
    }
    else
    {
        memcpy((Aud_SIFAGCConfig_t*)&gAudioFrontEndInfo.Control.FM_AGCConfigParam,(Aud_SIFAGCConfig_t*)pSIFAFCConfig,sizeof(Aud_SIFAGCConfig_t));
    }

    // set HW AGC config
    mode = HWREG_R(g305_reserved[4]);

    if (pSIFAFCConfig->HysteresisFastMode)
    {
        mode |= (0x1<<1);   //set bit 1 (agc_hysteresis = fast mode)
    }
    else
    {
        mode &= ~(0x1<<1);  //clr bit 1 (agc_hysteresis = slow mode)
    }

    if (pSIFAFCConfig->En)
    {
        mode &= ~(0x1);     //clr bit 0 (agc_off = agc not off)
    }
    else
    {
        mode |= 1;          //set bit 0 (agc_off = agc off)

        if ((pSIFAFCConfig->Gain <AUD_SIF_AGC_MIN) || (pSIFAFCConfig->Gain >AUD_SIF_AGC_MAX))
        {
            // set mode and do nothing else
            HWREG_W(g305_reserved[4], mode);
            return AUD_ERR_INVALID_PARAM;
        }
        else
        {
            offset = HW_SSD_AGC_CTL_V_MAX - AUD_SIF_AGC_MAX;
            HWREG_W(g305_reserved[5], (pSIFAFCConfig->Gain + offset)); // set agc_ctl
        }
    }

    HWREG_W(g305_reserved[4], mode);

    return AUD_SUCCESS;
}
#endif

/************************* 8203D prototype API ******************************/

INT32 AUDIF_Get_MPEG_BSType( UINT8 BankID, INT16 *val )
{	
	UNUSED_ARGUMENT(BankID);
	*val = 0;
	return AUD_SUCCESS;
}

INT32 AUDIF_Init_Wave_Dec2_Audio(WAVFORMATMACRO *wfmt1)
{
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::%d\n", __LINE__ );

	INT32 rVal=0;

	memcpy(&gWavFmt_dec2, wfmt1, sizeof(gWavFmt_dec2));
	//memcpy(&gWavFmt_dec2, &gWavFmt, sizeof(gWavFmt_dec2));

	// set channel output mode configuration
	// AUDIF_Set_SpkOutMode();

	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::wFormatTag     :0x%x\n",gWavFmt_dec2.wfx.wFormatTag);
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::nChannels      :0x%x\n",gWavFmt_dec2.wfx.nChannels);
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::nSamplesPerSec :0x%x\n",gWavFmt_dec2.wfx.nSamplesPerSec);
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::nAvgBytesPerSec:0x%x\n",gWavFmt_dec2.wfx.nAvgBytesPerSec);
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::nBlockAlign    :0x%x\n",gWavFmt_dec2.wfx.nBlockAlign);
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::wBitsPerSample :0x%x\n",gWavFmt_dec2.wfx.wBitsPerSample);
	AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::cbSize         :0x%x\n",gWavFmt_dec2.wfx.cbSize);

	switch (gWavFmt_dec2.wfx.wFormatTag)
	{
		case WAVE_FORMAT_BD_LPCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::lpcm vob audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_PCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::pcm audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_BigEndian_LPCM: // big endian lpcm
		case WAVE_FORMAT_ULaw_LPCM://uLaw
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::pcm audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_DVD_LPCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::DVD LPCM audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_CD_LPCM:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::CD LPCM audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_IMA_ADPCM:	//for support ASF IMA ADPCM
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::ima-adpcm audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_ADPCM:	//for support MS ADPCM
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::MS ADPCM audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_LPCM);
			break;
		case WAVE_FORMAT_MPEG:
		case WAVE_FORMAT_MPEGLAYER3:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::mpg audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_MP3);
			break;
		case WAVE_FORMAT_DOLBY_DIGITAL_PLUS:
		case WAVE_FORMAT_DOLBY_AC3:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::ac3 audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_AC3);
			break;
		case WAVE_FORMAT_DOLBY_DTS:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::dts audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_DTS);
			break;
		case WAVE_FORMAT_WMA_STEREO:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::wma audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_WMA);
			break;
		case WAVE_FORMAT_MPEG2AAC:
		case WAVE_FORMAT_AAC:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::aac audio\n");
			//aud_param.dec_param.aac_mode = AAC_MODE_RAW;
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_AAC);
			break;
		case WAVE_FORMAT_WMA_PRO:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::wma pro\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_WMAPRO);
			break;
		case WAVE_FORMAT_DTSHD:
		case WAVE_FORMAT_DTSHD_MASTER_AUDIO:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::dts hd\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_DTSHD);
			break;
		case WAVE_FORMAT_TRUEHD:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::truehd\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_TRUEHD);
			break;
	 	case WAVE_FORMAT_OGG_VORBIS:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::ogg vorbis\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_OGG);
			//AUDIF_Set_OggInitFlag(0);
			break;
		case WAVE_FORMAT_REAL_AUDIO:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::real audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_RA);
			break;
		case WAVE_FORMAT_NOISE_GEN:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::noise generator\n");
			//AUDIF_TestTone_mode(0); // 0:sine/ 1:pink/ 2:white
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_EMU);
			break;
		case WAVE_FORMAT_FLAC:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::FLAC audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_FLAC);
			break;
		case WAVE_FORMAT_APE:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::APE audio\n");
			rVal = AUDIF_Set_Dec2CodingMode(AUDIF_CODING_MODE_APE);
			break;

		default:
			AUDDRV_PRINT("AUDIF_Init_Wave_Dec2_Audio::Unknown audio format\n");
			return AUDIF_ERR_Unknown_CodecID;
	}

	return rVal;

}

INT32 AUDIF_Is_DecEndingDone(UINT8 BankID)
{
	UNUSED_ARGUMENT(BankID);
	return AUD_SUCCESS;
}

INT32 AUDIF_Wait_DecEndingDone(UINT8 BankID)
{
	UNUSED_ARGUMENT(BankID);
	return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set ATV change channel ramp-down or ramp-up
* @param    ramp_flag : RAMP_STATUS_ON->ramp-down, else ramp-up
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_AtvRamp(UINT8 ramp_flag)
{
    	AUDIO_SEMAPHORE_TAKE();

    	// MUTE_STATUS_ON:mute, MUTE_STATUS_OFF:de-mute
    	if (RAMP_STATUS_ON == ramp_flag)
    	{
    		DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_15,BIT_15, 1); //jy.ciou 20110502
        	}   
    	else
    	{
    		DSP_SetBitMaskRiscParam1(RISC_MASTER_VOL, BIT_15,BIT_15, 0);
    	} 
	
	DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);    

	AUDIO_SEMAPHORE_GIVE();
    	return AUD_SUCCESS;
}

/******************************************************************************/
/**
* @brief    Set PCM output phase control
* @param    index : phase
* @return   AUD_SUCCESS if successful
******************************************************************************/
AUDIF_Status_t AUDIF_Set_PcmOutputPhase(UINT8 main_phase, UINT8 aux1_phase, UINT8 aux2_phase)
{
    // please refer to output_phase_e
    UINT8 phase = aud_param.pcm_output_phase;

    if (OUTPUT_PHASE_180 == main_phase)
    {
        phase |= (OUTPUT_PHASE_180<<0);
    }
    else
    {
        phase &= ~(OUTPUT_PHASE_180<<0);
    }

    if (OUTPUT_PHASE_180 == aux1_phase)
    {
        phase |= (OUTPUT_PHASE_180<<1);
    }
    else
    {
        phase &= ~(OUTPUT_PHASE_180<<1);
    }    

    if (OUTPUT_PHASE_180 == aux2_phase)
    {
        phase |= (OUTPUT_PHASE_180<<2);
    }
    else
    {
        phase &= ~(OUTPUT_PHASE_180<<2);
    }    
    
    DSP_SetBitMaskRiscParam1(RISC_MAIN_PCMOUTPUT_CTRL, BIT_2,BIT_0, phase);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);    
    return AUD_SUCCESS;
}


AUDIF_Status_t AUDIF_Set_SourceType(AUDIF_Source_Type_e mode)
{        
    currSourceType = mode;

    DSP_SetBitMaskRiscParam1(RISC_MAIN_APPLICATION, BIT_15,BIT_0, currSourceType);
    DSP_SendCmd(RISC_CMD0x05_WRITE,CODE0x04_SYNC_RISC_PARAM1,0x0,0x0);       

    AUDDRV_PRINT("AUDIF_Set_AppSource::mode:%d\n", currSourceType);
    
    return AUD_SUCCESS;
}

AUDIF_Status_t AUDIF_Get_DdpFmt( UINT16 *type )
{
	// 0: AC3, 1: DDP
	if( aud_param.coding_mode_now == (1<<ID_DECODER_DDP) )
	{
		UINT16 DDP_type=0;
		DDP_type = (UINT16)( (HWREG_R(dsp_port[20])>>24)& 0x1 );		

		if( DDP_type == 1 )
			*type = WAVE_FORMAT_DOLBY_DIGITAL_PLUS;
		else
			*type = WAVE_FORMAT_DOLBY_AC3;
	}
	else
		*type = 0;
	
	return AUD_SUCCESS;
}
