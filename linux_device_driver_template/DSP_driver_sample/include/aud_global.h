
#ifndef __AUD_GLOBAL_H__
#define __AUD_GLOBAL_H__

#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/io.h>

#include <asm/mach-spv9200/regmap_9200.h>
#include <asm/mach-spv9200/spv9200.h>
#include <asm/mach-spv9200/types.h>
#include "dsp_api.h"
//#include "playlist.h"

/********************************************************************************************

	audio dsp system run mode related defines

  ********************************************************************************************/
//#define USE_DRAM_A					// Use DRAM_A (32bit) of DRAM_B (16bit)
//#define DISABLE_DSPCODE_SCRAMBLE		// Disable scramble dsp code
//#define ENABLE_DSP_INTERRUPT			// enabled dsp->risc interrupt
//#define ENABLE_DSP_RUNTIME_MEMMAP
//#define ENABLE_DSP_LOCK_API
#define AUD_9200_CLOCK
#define DEBUG_TEST_IEC0_DELAY
//#define QAE468_CERTI_BOARD

//#if(IC_SIMPLE_TEST == 1 )				// by makefile
//#define AUDIOTEAM_Module_Test			// audio module test
//#endif

#define DEBUG_ENABLE_DEC1
//#define DEBUG_ENABLE_DEC2
//#define DEBUG_ENABLE_EFF
//#define DEBUG_ENABLE_APP
#define DEBUG_ENABLE_ENC
//#define DEBUG_ENABLE_MIC
#define DEBUG_ENABLE_AUXIN		// not ready
#define DEBUG_ENABLE_SIFIN
//#define DEBUG_ENABLE_SPDIFIN		// not ready
//#define DEBUG_ENABLE_SNDEFFECT	// for Native32 sound effect (11k)
#define DEBUG_ENABLE_FIFO_A5			// A5:S/PDIF FIFO enabled
//#define DEBUG_ENABLE_FIFO_A6			// A6:HDMI FIFO enabled
#ifdef DEBUG_ENABLE_MIC
#define DEBUG_ENABLE_FIFO_MIC			// A7 and A10 enabled
#endif
//
// FIFO_DEPTH_SETTING: FIFO_DEPTH_COMPACT/FIFO_DEPTH_NORMAL/FIFO_DEPTH_LARGE/FIFO_DEPTH_EXTREME
//
#define FIFO_DEPTH_SETTING			FIFO_DEPTH_NORMAL

// ---------------------------------------------------------------------------
// Sample rate related settings:
// 44.1k: 0x0000 88.2k: 0x0001 22.05k: 0x0002 176.4k: 0x0003 11.025k: 0x0004 5.5125k: 0x0005
// 48  k: 0x0010 96  k: 0x0011 24   k: 0x0012 192  k: 0x0013 12    k: 0x0014      6k: 0x0015
// 32  k: 0x0020 64  k: 0x0021 16   k: 0x0022 128  k: 0x0023  8    k: 0x0024      4k: 0x0025
// 768 k: 0x0030 Not indicated: 0x0031
// ---------------------------------------------------------------------------
#define SAMPLE_RATE_DEFAULT_BY_ID						// K_PCM_OUTPUT_SAMPLE_RATE_ID
#define AUTO_SET_DAC_SAMPLE_RATE 						// auto set DAC sample rate when sample rate changed
#define K_PCM_OUTPUT_SAMPLE_RATE_ID         0x0010		// DAC max
#define K_IEC0_OUTPUT_SAMPLE_RATE_ID        0x0010
#define K_IEC1_OUTPUT_SAMPLE_RATE_ID        0x0010

// ---------------------------------------------------------------------------
// S/PDIF input selection
// ---------------------------------------------------------------------------
#define SUPPORT_QAE377_SPDIF_IN
//#define DEBUG_SPDIF_IN_PCM  //enable PCM data throughout, download main-program only
                            //dsp spdif-in should change to SPDIFIN_VERIFY_MODE
//#define SPDIFIN_FORCE_44TO48                            

// ---------------------------------------------------------------------------
// 9202 Demdec related
// ---------------------------------------------------------------------------
//#define ENA_9202_DEMDEC  //get data from SSD-HW (I2S RX, A12)
#ifdef ENA_9202_DEMDEC
#ifndef DEBUG_ENABLE_SIFIN
#define DEBUG_ENABLE_SIFIN  // must turn on SIFIN
#endif

#ifdef DEBUG_ENABLE_AUXIN
#undef DEBUG_ENABLE_AUXIN   // must turn off AUXIN, because they shared A12 usage
#endif
#endif //ENA_9202_DEMDEC

/********************************************************************************************

 	QAE377 FPGA or IC settings (please modified env.conf to switch ON/OFF QAE377_FPGA_MODE)

 ********************************************************************************************/


#ifdef QAE377_FPGA_MODE
// ----------------------------------------------------------------------------------
// for QAE377 FPGA defines (auto config by makefile)
// ----------------------------------------------------------------------------------
#define DEBUG_ENABLE_DSP_MONITOR
#ifdef DEBUG_ENABLE_DSP_MONITOR
//#define DEBUG_ENABLE_PERFRAME_MONITOR		// monitor every frame monitor results
//#define DEBUG_CHECKSUM_VERIFICATION			// enable checksum verification
//#define CHECKSUM_ERROR_DSP_LOCK				// Lock DSP while checksum errors
#ifndef DEBUG_PRINT_DRAM_MESSAGE
#define DEBUG_PRINT_DRAM_MESSAGE 			// must enable DSP dram message (dont off)
#endif //DEBUG_PRINT_DRAM_MESSAGE
#endif //DEBUG_ENABLE_DSP_MONITOR

#define INFO_SHOW_DSP_MONITOR_FRAMECNT		0x100
//#define ENABLE_DUMMY_MASTER				// enable dummy master settings
//#define ENABLE_L2_CACHE
#define ENABLE_DSP_ROM_2T

#ifdef DEBUG_ENABLE_APP
//#define DEBUG_APP_VERIFICATION      // for APP verification
#endif

#define DSP_NEW_MEMORY_LAYOUT			// should enabled on QAE377A and disable on QAE377S
#else
// ----------------------------------------------------------------------------------
// for QAE377 IC defines (auto config by makefile)
// ----------------------------------------------------------------------------------
#define DEBUG_ENABLE_DSP_MONITOR
#ifdef DEBUG_ENABLE_DSP_MONITOR
//#define DEBUG_ENABLE_PERFRAME_MONITOR		// monitor every frame monitor results
//#define DEBUG_CHECKSUM_VERIFICATION			// enable checksum verification
//#define CHECKSUM_ERROR_DSP_LOCK				// Lock DSP while checksum errors
#ifndef DEBUG_PRINT_DRAM_MESSAGE
#define DEBUG_PRINT_DRAM_MESSAGE 			// must enable DSP dram message (dont off)
#endif //DEBUG_PRINT_DRAM_MESSAGE
#endif //DEBUG_ENABLE_DSP_MONITOR

#define INFO_SHOW_DSP_MONITOR_FRAMECNT		0x200
//#define ENABLE_DUMMY_MASTER					// enable dummy master settings
//#define ENABLE_L2_CACHE
//#define QAE377_Chip_SPDIFOUT_CERT_8CH		// enable 8-ch SPDIF OUT for certification related setting
//#define DEBUG_DOWN_FS_RATIO				// for I2S recording
//#define BOARD_8600A_511_SKT_A         	// for SKT board: dramB:0x92000000 - 0x96000000
//#define FILE_IO_BY_SDCARD					// enable SD-CARD for FILE I/O (if comment out FILE I/O by ATA)
#define HDMI_CHN_MAP_BY_HDMI_DRV		// HDMI channel mapping by HDMI Driver

#define ENABLE_DSP_ROM_2T
#define IC_RerA_AUD_Setting
//#define DSP_PLLH_594M
#define DSP_NEW_MEMORY_LAYOUT			// should enabled on QAE377A and disable on QAE377S
#ifdef AUDIOTEAM_Module_Test
//#define Record_I2S_Data
#endif

#ifndef AUDIOTEAM_Module_Test
#define USE_RISC_PARSER_PARAM
#define USE_RISC_AUDIOHandle
#endif

//#define AUD_SUPPORT_HDMI
#define AUD_SPEAKER_CONFIG
//#define AUD_ShowFIFOStatus //show DAC/SPDIF/HDMI for debug
#define SUPPORT_SOFTWARE_APP // Software APP

//#define VOLUMN_TAB_80
#define VOLUMN_TAB_50
#define DAC_OUTPUT_48K
//#define DEBUG_DDCO
#endif

//#define AUD_SUPPORT_FIFORESET	// ccyang
//#define AUD_Enable_Watchdog // ccyang
#define AUD_Enable_TimeoutProtect // ccyang
#define AUD_Enable_MainDead_Watchdog 
// ---------------------------------------------------------------------------
// software APP selection
// ---------------------------------------------------------------------------
#ifdef SUPPORT_SOFTWARE_APP
#define AUD_SUPPORT_GM7
//#define AUD_SUPPORT_NEO6
//#define SUPPORT_PROLOGIC_II
#endif

// ---------------------------------------------------------------------------
// emu decoder parameters
// ---------------------------------------------------------------------------
#define K_EMU_OUTPUT_PCM_BLOCK_SIZE			32
#define K_EMU_OUTPUT_IEC0_BLOCK_SIZE        32
#define K_EMU_OUTPUT_IEC1_BLOCK_SIZE        32
#define K_EMU_OUTPUT_PCM_SAMPLE_RATE_ID     0x0010
#define K_EMU_OUTPUT_IEC0_SAMPLE_RATE_ID    0x0010
#define K_EMU_OUTPUT_IEC1_SAMPLE_RATE_ID    0x0010
#define K_EMU_OUTPUT_MODE                   0x0			// 0x0:sinewave, 0x1:white noise, 0x2: pink noise

#ifdef AUDIO_TEST_AUTO_BATCH
#define DEBUG_ENABLE_PERFRAME_MONITOR		// monitor every frame monitor results
#endif


/******************************************************************************
	dsp messages
 *****************************************************************************/
//#define DEBUG_PRINT_DSP_MESSAGE
#ifdef DEBUG_PRINT_DSP_MESSAGE
#define DEBUG_PRINT_DRAM_MESSAGE		// show dram message
#define DEBUG_PRINT_DSP_MESSAGE			// show dsp message
#endif

#define MSG_FILTER_ID_MAIN          (1<<MSG_ID_MAIN  )
#define MSG_FILTER_ID_DEC1          (1<<MSG_ID_DEC1  )
#define MSG_FILTER_ID_DEC2          (1<<MSG_ID_DEC2  )
#define MSG_FILTER_ID_ENC           (1<<MSG_ID_ENC   )
#define MSG_FILTER_ID_DBG           (1<<MSG_ID_DBG   )
#define MSG_FILTER_ID_PRF           (1<<MSG_ID_PRF   )
#define MSG_FILTER_ID_CHKSUM        (1<<MSG_ID_CHKSUM)
#define MSG_FILTER_ID_DMA           (1<<MSG_ID_DMA   )
#define MSG_FILTER_ID_ISR           (1<<MSG_ID_ISR   )
#define MSG_FILTER_ID_APP           (1<<MSG_ID_APP   )
#define MSG_FILTER_ID_SWAPP         (1<<MSG_ID_SWAPP )
#define MSG_FILTER_ID_MIXER         (1<<MSG_ID_MIXER )
#define MSG_FILTER_ID_FPGA          (1<<MSG_ID_FPGA  )

//#define DEFAULT_DSP_MSG_ID_FILTER	    (MSG_FILTER_ID_DEC1 | MSG_FILTER_ID_DEC2 | MSG_FILTER_ID_DBG)
//#define DEFAULT_DSP_MSG_ID_FILTER	    (MSG_FILTER_ID_DEC1 | MSG_FILTER_ID_DMA | MSG_FILTER_ID_DEC2 | MSG_FILTER_ID_ENC |MSG_FILTER_ID_PRF | MSG_FILTER_ID_CHKSUM | MSG_FILTER_ID_DBG)
#define DEFAULT_DSP_MSG_ID_FILTER	    0xFFFF
#define DEFAULT_DSP_MSG_SUBID_FILTER 	0xFFFF


/******************************************************************************
   risc printf messages
 *****************************************************************************/
//#define DEBUG_ENABLE_PRINT_MESSAGE					// enable debug print

//#define DEBUG_ENABLE_DSPDEBUG_PRINT				// enable dsp debug print
#ifdef DEBUG_ENABLE_PRINT_MESSAGE					// if defined DEBUG_ENABLE_PRINT_MESSAGE
#define DEBUG_ENABLE_MONPRF_PRINT                   // enable MONPRF_PRINT
#define DEBUG_ENABLE_DSPAPI_PRINT					// enabled DSPAPI_PRINT
#define DEBUG_ENABLE_AUDDRV_PRINT					// enabled AUDDRV_PRINT
//#define DEBUG_ENABLE_SYSTASK_PRINT					// enabled SYSTASK_PRINT
//#define DEBUG_ENABLE_MONTASK_PRINT					// enabled MONTASK_PRINT
//#define DEBUG_ENABLE_AUDTASK_PRINT					// enabled AUDTASK_PRINT
//#define DEBUG_ENABLE_INFOTASK_PRINT					// enabled INFOTASK_PRINT
//#define DEBUG_ENABLE_ASF_PRINT					// enabled ASF_PRINT
//#define DEBUG_ENABLE_FILE_LOG						// enable file log (not implement yet)
//#define DEBUG_ENABLE_RMFF_PRINT				// enabled RMFF_PRINT
#endif

#ifdef DEBUG_CHECKSUM_VERIFICATION
#undef DEBUG_ENABLE_MONPRF_PRINT
#undef DEBUG_ENABLE_INFOTASK_PRINT
#endif

//
// don't modified the following defines
//
#define PRINT_ID_MONPRF         		(1<<0)
#define PRINT_ID_DSPAPI		    		(1<<1)
#define PRINT_ID_AUDDRV					(1<<2)
#define PRINT_ID_SYSTASK				(1<<4)
#define PRINT_ID_MONTASK				(1<<5)
#define PRINT_ID_AUDTASK				(1<<6)
#define PRINT_ID_INFOTASK       		(1<<7)
#define PRINT_ID_ASFPRINT       		(1<<8)
#define PRINT_ID_RMPRINT        		(1<<9)
#define PRINT_ID_MAX					(1<<31)

#ifdef DEBUG_ENABLE_DSPDEBUG_PRINT
#define DSP_PRINT(...)					diag_printf(__VA_ARGS__)
#define ERROR_PRINT(...)
#else
#define DSP_PRINT(...)					//
#define ERROR_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_MONPRF_PRINT
#define MONPRF_PRINT(x...)				DSP_Printf(PRINT_ID_MONPRF,##x)
#else
#define MONPRF_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_DSPAPI_PRINT
#define DSPAPI_PRINT(x...)				DSP_Printf(PRINT_ID_DSPAPI,##x)
#else
#define DSPAPI_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_AUDDRV_PRINT
#define AUDDRV_PRINT(x...)				DSP_Printf(PRINT_ID_AUDDRV,##x)
#else
#define AUDDRV_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_SYSTASK_PRINT
#define SYSTASK_PRINT(x...)				DSP_Printf(PRINT_ID_SYSTASK,##x)
#else
#define SYSTASK_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_MONTASK_PRINT
#define MONTASK_PRINT(x...)				DSP_Printf(PRINT_ID_MONTASK,##x)
#else
#define MONTASK_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_AUDTASK_PRINT
#define AUDTASK_PRINT(x...)				DSP_Printf(PRINT_ID_AUDTASK,##x)
#else
#define AUDTASK_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_INFOTASK_PRINT
#define INFOTASK_PRINT(x...)			DSP_Printf(PRINT_ID_INFOTASK,##x)
#else
#define INFOTASK_PRINT(...)				//
#endif

#ifdef DEBUG_ENABLE_ASF_PRINT
#define ASF_PRINT(x...)					DSP_Printf(PRINT_ID_ASFPRINT,##x)
#else
#define ASF_PRINT(...)					//
#endif

#ifdef DEBUG_ENABLE_RMFF_PRINT
#define RA_PRINT(x...)					DSP_Printf(PRINT_ID_RMPRINT,##x)
#else
#define RA_PRINT(...)							//
#endif

#ifdef DEBUG_ENABLE_FILE_LOG
#define FILE_LOG(x...)					DSP_FileLog(##x)
#else
#define FILE_LOG(x...)					//
#endif


/******************************************************************************
	ecos related defines
 *****************************************************************************/
//#define ENABLE_AUDIO_SEMAHPORE
//#define ENABLE_UART_MUTEX
//#define Enable_DSPCMD_MUTEX

#ifdef ENABLE_UART_MUTEX
extern GL_Mutex_t gUARTMutex;
#define UART_MUTEX_CREATE()				GL_MutexCreate("UART(dsp) mutex",&gUARTMutex)
#define UART_MUTEX_DELETE()				GL_MutexDelete(gUARTMutex)
#define UART_MUTEX_LOCK()				GL_MutexLock(gUARTMutex)
#define UART_MUTEX_UNLOCK()				GL_MutexUnlock(gUARTMutex)
#else
#define UART_MUTEX_CREATE()
#define UART_MUTEX_DELETE()
#define UART_MUTEX_LOCK()
#define UART_MUTEX_UNLOCK()
#endif

//
// Audio sempahore
// (currently only one semaphore to protect AUDIF from simultaneouly called by threads)
//
#ifdef ENABLE_AUDIO_SEMAHPORE
extern GL_Semaphore_t gAudioSemaphore;
#define AUDIO_SEMAPHORE_CREATE()		{if(gAudioSemaphore == NULL) GL_SemaphoreCreate("Audio Sempahore", 1, 1, &gAudioSemaphore);}
#define AUDIO_SEMAPHORE_DELETE()		GL_SemaphoreDelete(gAudioSemaphore)
#define AUDIO_SEMAPHORE_TAKE()			GL_SemaphoreTake(gAudioSemaphore,GL_INFINITE_WAIT)
#define AUDIO_SEMAPHORE_GIVE()			GL_SemaphoreGive(gAudioSemaphore)
#else
#define AUDIO_SEMAPHORE_CREATE()
#define AUDIO_SEMAPHORE_DELETE()
#define AUDIO_SEMAPHORE_TAKE()
#define AUDIO_SEMAPHORE_GIVE()
#endif

#ifdef Enable_DSPCMD_MUTEX
extern GL_Mutex_t gDspCmdMutex;
#define DSPCMD_MUTEX_CREATE()			{if(gDspCmdMutex == NULL) GL_MutexCreate("DspCmd", &gDspCmdMutex);}
#define DSPCMD_MUTEX_DELETE()			GL_MutexDelete(gDspCmdMutex)
#define DSPCMD_MUTEX_LOCK()				GL_MutexLock(gDspCmdMutex)
#define DSPCMD_MUTEX_UNLOCK()			GL_MutexUnlock(gDspCmdMutex)
#else
//extern GL_Mutex_t gDspCmdMutex;
#define DSPCMD_MUTEX_CREATE()
#define DSPCMD_MUTEX_DELETE()
#define DSPCMD_MUTEX_LOCK()
#define DSPCMD_MUTEX_UNLOCK()
#endif


//
//	Debug assert
//
/*
extern GL_Status_t GL_TaskSleep(UINT32 sdTimeout);
extern void GL_DebugAssert(char *pFileName, UINT32 dLineNumber, UINT32 dExpressionEvaluation, char *pConditionText);
#define ASSERT(cond,doc) 				do { \
	                                      	UART_MUTEX_LOCK(); \
	                                      	GL_DebugAssert(__FILE__, __LINE__, cond, doc);\
	                                      	UART_MUTEX_UNLOCK();\
										}while(0)
*/
/********************************************************************************************

 	AUDIO TEAM Module Test Codebase Related Setting

********************************************************************************************/

#ifdef AUDIOTEAM_Module_Test

/********************************************************************************************

 	Playlist related settings

 ********************************************************************************************/

#define IS_TEST_DEC1_CHECK_MODE     TRUE	// TRUE:test all codec with single file FALSE:NORMAL MODE
#define IS_TEST_DEC1_ALL_FILES		FALSE	// TRUE:test all files  FALSE:test single file
#define IS_TEST_DEC1_ALL_CODECS		TRUE	// TRUE:test all codecs FALSE:test single codec
#define IS_TEST_DEC1_REPEAT_FILE    FALSE	// TRUE:repeat single file
#define IS_TEST_DEC1_REPEAT_LIST    FALSE   // TRUE:repeat playlist

#define IS_TEST_DEC2_CHECK_MODE     FALSE    // TRUE:test all codec with single file FALSE:NORMAL MODE
#define IS_TEST_DEC2_ALL_FILES		FALSE	// TRUE:test all files  FALSE:test single file
#define IS_TEST_DEC2_ALL_CODECS		FALSE	// TRUE:test all codecs FALSE:test single codec
#define IS_TEST_DEC2_REPEAT_FILE    FALSE	// TRUE:repeat single file
#define IS_TEST_DEC2_REPEAT_LIST    FALSE   // TRUE:repeat playlist

#define DEFAULT_ENABLE_FIXED_CODEC_PARAMS

// ---------------------------------------------------------------------------
// Primary decoder
// ---------------------------------------------------------------------------
// Primary Codec selection
#ifdef DEBUG_ENABLE_DEC1
//#define DEBUG_DEC1_PCM
//#define DEBUG_DEC1_DDP
//#define DEBUG_DEC1_TRUEHD
//#define DEBUG_DEC1_DTSLBR
//#define DEBUG_DEC1_DTSHD
#define DEBUG_DEC1_MPEG
//#define DEBUG_DEC1_WMA
//#define DEBUG_DEC1_RA
//#define DEBUG_DEC1_OGG
//#define DEBUG_DEC1_EMU
//#define DEBUG_DEC1_WMAPRO
//#define DEBUG_DEC1_AAC
//#define DEBUG_DEC1_FLAC
//#define DEBUG_DEC1_APE
#endif

// ---------------------------------------------------------------------------
// Secondary decoder
// ---------------------------------------------------------------------------
#ifdef DEBUG_ENABLE_DEC2
//#define DEBUG_DEC2_PCM
//#define DEBUG_DEC2_DDP
//#define DEBUG_DEC2_TRUEHD
//#define DEBUG_DEC2_DTSLBR
//#define DEBUG_DEC2_DTSHD
#define DEBUG_DEC2_MPEG
//#define DEBUG_DEC2_WMA
//#define DEBUG_DEC2_RA
//#define DEBUG_DEC2_OGG
//#define DEBUG_DEC2_EMU
//#define DEBUG_DEC2_WMAPRO
//#define DEBUG_DEC2_AAC
//#define DEBUG_DEC2_FLAC
//#define DEBUG_DEC2_APE
#endif

// ---------------------------------------------------------------------------
// APP
// ---------------------------------------------------------------------------
#ifdef DEBUG_ENABLE_APP
//#define DEBUG_APP_VERIFICATION      // for APP verification
#endif

// ---------------------------------------------------------------------------
// Encoder
// ---------------------------------------------------------------------------
#ifdef DEBUG_ENABLE_ENC
#undef ENABLE_DSP_INTERRUPT		// dont know why DSP->RISC will crash system??
//#define DEBUG_ENC_MPEG
//#define DEBUG_ENC_AC3
//#define DEBUG_ENC_DTS
#endif
#define DUMP_ENCODER_OUTBS_TO_FILE

// ---------------------------------------------------------------------------
// emu decoder parameters
// ---------------------------------------------------------------------------
#ifdef DEBUG_DEC1_EMU
#undef AUTO_SET_DAC_SAMPLE_RATE
#undef ENABLE_DSP_INTERRUPT
#endif


//
//	 File I/O settings
//
#ifdef FILE_IO_BY_SDCARD
#define MOUNT_DEV   	"/dev/sde/"
#else
#define MOUNT_DEV   	"/dev/hda/1"
#endif
#define MOUNT_PNT   	"/"
//#define MOUNT_PNT   	"/usba0"
#define MOUNT_FS_FAT    "fatfs"
#define MOUNT_FS_NTFS   "ntfs"

//extern int ide_mount(const char *devname, const char *dir, const char *fsname);
//extern int ide_umount( const char *name);
extern int mount(const char *devname, const char *dir, const char *fsname);
extern int umount(const char *name);



/******************************************************************************
	systask related
 *****************************************************************************/
#define ENABLE_AUTO_START
typedef struct _SYSTASK_MODE
{
	UINT16 bAutoStart;
	UINT16 bEnableShowInfo;
	UINT16 bEnableDummyMaster;
	UINT16 bEnableL2Cache;
	SYSTEM_INFO* pDec1PlayInfo;
	SYSTEM_INFO* pDec2PlayInfo;
	UINT16* pRiscParams;
	UINT16* pDec1Params;
	UINT16* pDec2Params;
	UINT16* pEncParams;
}SYSTASK_MODE;


enum _SYSTASK_CMD
{
	SYSTASK_CMD_EXIT			   	= -1,
	SYSTASK_CMD_IDLE 			   	= 0,

	//SYSTASK_MENU_MAIN: MenuMain() command:
	SYSTASK_CMD_MAIN_SHOW_DEFAULT  	= 1,
	SYSTASK_CMD_MAIN_CHANGE_DEFAULT = 2,
	SYSTASK_CMD_MAIN_EMU_TEST_MODE  = 3,
	SYSTASK_CMD_MAIN_FILE_TEST_MODE = 4,
	SYSTASK_CMD_SPDIFIN_MODE        = 5,
	SYSTASK_CMD_DDP_CER_MODE        = 6,
	SYSTASK_CMD_DTSHD_CER_MODE      = 7,


	//SYSTASK_MENU_SHOW_DEFAULT:
	SYSTASK_CMD_SHOW_SYSTASK_MODE	= 1,
	SYSTASK_CMD_SHOW_RISC_PARAM     = 2,
	SYSTASK_CMD_SHOW_DEC1_MODE      = 3,
	SYSTASK_CMD_SHOW_DEC2_MODE      = 4,
	SYSTASK_CMD_SHOW_ENC_MODE       = 5,
	SYSTASK_CMD_SHOW_ALL            = 6,

	//SYSTASK_MENU_CHANGE_DEFAULT:
	SYSTASK_CMD_CHANGE_SYSTASK_MODE = 1,
	SYSTASK_CMD_CHANGE_RISC_PARAM   = 2,
	SYSTASK_CMD_CHANGE_DEC1_MODE    = 3,
	SYSTASK_CMD_CHANGE_DEC2_MODE    = 4,
	SYSTASK_CMD_CHANGE_ENC_MODE     = 5,
};

enum _MON_CMD
{
	MON_CMD 		= 0,
	MON_CMD_SYSTEM  = 1,
	MON_CMD_DSP  	= 2,
	MON_CMD_AUD 	= 3,
};



typedef enum _SYSTASK_MENU_LEVEL
{
	SYSTASK_MENU_MAIN			    = 0,
	SYSTASK_MENU_SHOW_DEFAULT       = 1,
	SYSTASK_MENU_CHANGE_DEFAULT     = 2,
}SYSTASK_MENU_LEVEL;

#define STR_YES_NO(x)			(x == 0)? ("no"):("yes")

extern SYSTASK_MODE gSystaskMode;
extern int gSystaskCmd;
extern SYSTASK_MENU_LEVEL gSystaskNowMenu;


#endif // AUDIOTEAM_Module_Test


#endif // __AUD_GLOBAL_H__

