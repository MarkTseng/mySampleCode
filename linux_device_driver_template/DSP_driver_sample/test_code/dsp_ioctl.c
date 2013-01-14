/*
 * Sunplus DSP ioctl command test program
 * Author : Mark Tseng
 * DOCUMENT: 
 *		1. Create dsp device node
 *		# mknod /dev/dsp c 42 0
 *		2. execute dsp_ioctl
 * $Id: dsp_ioctl.c,v 0.1 2011/11/21 Mark Tseng Exp $
 */

static char const rcsid[] = "$Id: dsp_ioctl, v0.0.0.1 " __DATE__ " " __TIME__ " Mark Exp $";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>		/* ioctl */

#include "types.h"
#include "wave_fmt.h"
#include "plf_av_source.h"
#include "drv_ssd.h"
#include "tvfe_api.h"
#include "tvfe_audio.h"
#include "dsp_ioctl.h"

#define	 DSP_DEVNAME	"/dev/dsp"
#define	 DSP_MAJOR_NR	42
#define	 DSP_MINOR_NR	0
int DSPfd;

unsigned char TestArray[] = {0,1,2,3,4,5,7,8,9,10,11,12,13,14,15,16,17,18,19,
							0,1,2,3,4,5,7,8,9,10,11,12,13,14,15,16,17,18,19,
							0,1,2,3,4,5,7,8,9,10,11,12,13,14,15,16,17,18,19,
							0,1,2,3,4,5,7,8,9,10,11,12,13,14,15,16,17,18,19,
												0,1,2,3,4,5,7,8,9};
#define BSFeed32K	32000
#define BSFeed40K	40000
#define BSFeed48K	48000
#define BSFeed64K	64000
#define BSFeed128K	128000
#define BSFeed224K	224000
#define BSFeed256K	256000
#define BSFeed384K	384000
#define BSFeed512K	512000
#define BSFeed640K	640000
#define MP3FILENAME	"/usb/sda1/test.mp3"

int PlayMp3_BSFeed(char *FileName)
{
	int MP3_FD, read_byte=0,write_byte=0;
	unsigned char Buf[BSFeed40K]; 
	DSP_IOCTL_CMD dspCMD;
	unsigned int barN,BSFreeSize;

	MP3_FD = open(FileName,O_RDWR);

	if(MP3_FD == -1)
	{
		perror("open mp3 fail\n");	
		return ENOENT;
	}

	while(1)
	{
		// DSP DSP_AUDIF_Get_audBSbuf_free test
		
		memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		dspCMD.argument_1 = 0x1;
		ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_free,&dspCMD);
		//printf("DSP_AUDIF_Get_audBSbuf_free size:0x%x\n",dspCMD.argument_1);

		BSFreeSize = dspCMD.argument_1;

		if(BSFreeSize > BSFeed40K )
		{
			read_byte = read(MP3_FD,Buf,BSFeed40K);
			//printf("read_byte: 0x%x\n",read_byte);

			if(read_byte == 0)
				break;

			write_byte = write(DSPfd,Buf,read_byte);
			//printf("write_byte: 0x%x\n",write_byte);
#if 0
			memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
			dspCMD.argument_1 = 0x1;
			ioctl(DSPfd,DSP_AUDIF_Get_BarrierPtr,&dspCMD);
			barN = dspCMD.argument_1;
			printf("barN: 0x%x\n",barN);
#endif

			sleep(1);
		}
		else
		{
			printf("BS Free Size le : %d\n",BSFeed40K);
			sleep(3);
		}
	}

	close(MP3_FD);

	return 0;
}


int main (int argc, char **argv)
{
	int ret = 0;
	DSP_IOCTL_CMD dspCMD;
	Aud_EqualizerConfig_t dspEQ;
	TVFE_Audio_CMD_st TVFE_CMD;
	TVFE_Audio_Equalizer_Config_t TVFE_EQ;	
	WAVFORMATMACRO wfmt;

	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	memset(&dspEQ,0,sizeof(Aud_EqualizerConfig_t));
	memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
	memset(&TVFE_EQ,0,sizeof(TVFE_Audio_Equalizer_Config_t));
	memset(&wfmt,0,sizeof(WAVFORMATMACRO));


	DSPfd = open(DSP_DEVNAME,O_RDWR);
	if(DSPfd < 0)
	{
		/* auto create dsp device node */
		ret = mknod(DSP_DEVNAME,S_IFCHR,makedev(DSP_MAJOR_NR,DSP_MINOR_NR));
		if(ret)
		{
			perror("mknod fail\n");
			exit(1);	
		}
		DSPfd = open(DSP_DEVNAME,O_RDWR);
	}

#if 0
	dspCMD.argument_1 = 100;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_MasterVolume,&dspCMD);
	if(ret != 0)
	{
		perror("DSP_AUDIF_Set_MasterVolume Fail\n");
	}	

	dspCMD.argument_1 = AUDIF_MAIN_PATH_AUX;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_MainChannelOutput,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_MainChannelOutput Fail\n");
	}	

	dspCMD.argument_1 = AUDIF_AUX1_PATH_AUX;
	dspCMD.argument_2 = AUDIF_AUX1_PATH_LFE;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_AuxChannelOutput,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_AuxChannelOutput Fail\n");
	}	


	dspCMD.argument_1 = AUDIF_ADC_SOURCE_ADC2;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_AdcSourceSelect,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_AdcSourceSelect Fail\n");
	}	

	dspCMD.argument_1 = 0;
	ret = ioctl(DSPfd,DSP_SET_MUTE,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_SET_MUTE Fail\n");
	}	

	dspCMD.argument_1 = 0;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_Bass,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_Bass Fail\n");
	}	

	dspCMD.argument_1 = 0;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_Treble,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_Treble Fail\n");
	}	

#endif

#if 1
	// EQ setting
	dspEQ.fEnBypass = 0;
	dspEQ.bMainEQ_Band1_0100HZ = 10;
	dspEQ.bMainEQ_Band2_0300HZ = 10;
	dspEQ.bMainEQ_Band3_1000HZ = 10;
	dspEQ.bMainEQ_Band4_3000HZ = 10;
	dspEQ.bMainEQ_Band5_8000HZ = 10;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_MainEqulizer,&dspEQ);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_MainEqulizer Fail\n");
	}

	// Get EQ config
	memset(&dspEQ,0,sizeof(Aud_EqualizerConfig_t));
	ret = ioctl(DSPfd,DSP_AUDIF_Get_MainEqulizer,&dspEQ);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_MainEqulizer Fail: %d\n",ret);
	}
	printf("fEnBypass :%d\n",dspEQ.fEnBypass);
	printf("bMainEQ_Band1_0100HZ :%d\n",dspEQ.bMainEQ_Band1_0100HZ);
	printf("bMainEQ_Band2_0300HZ :%d\n",dspEQ.bMainEQ_Band2_0300HZ);
	printf("bMainEQ_Band3_1000HZ :%d\n",dspEQ.bMainEQ_Band3_1000HZ);
	printf("bMainEQ_Band4_3000HZ :%d\n",dspEQ.bMainEQ_Band4_3000HZ);
	printf("bMainEQ_Band5_8000HZ :%d\n",dspEQ.bMainEQ_Band5_8000HZ);

	// DSP_AUDIF_Get_MasterMuteStatus	
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	ret = ioctl(DSPfd,DSP_AUDIF_Get_MasterMuteStatus,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_MasterMuteStatus Fail: %d\n",ret);
	}	
	printf("DSP_AUDIF_Get_MasterMuteStatus :%d\n",dspCMD.argument_1);

	// DSP_AUDIF_TestTone_mode
	dspCMD.argument_1 = 0;
	ret = ioctl(DSPfd,DSP_AUDIF_TestTone_mode,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_TestTone_mode Fail\n");
	}	

	dspCMD.argument_1 = AUDIF_ADC_SOURCE_ADC0;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_AdcSourceSelect,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_AdcSourceSelect Fail\n");
	}	

	dspCMD.argument_1 = AUDIF_ADC_SOURCE_ADC2;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_AdcSourceSelect,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_AdcSourceSelect Fail\n");
	}	

	ret = ioctl(DSPfd,DSP_AUDIF_Set_FIFOReset,NULL);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_FIFOReset Fail\n");
	}	

	ret = ioctl(DSPfd,DSP_AUDIF_Set_FIFOEnable,NULL);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_FIFOEnable Fail\n");
	}	


	// TVFE argument test
	TVFE_CMD.Set_Effect_Mode.Effect_Mode = TVFE_AUDIO_EFFECT_MODE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX;
	ret = ioctl(DSPfd,DSP_TVFE_Audio_Set_Effect_Mode,&TVFE_CMD);
	if(ret != 0)
	{
		printf("DSP_TVFE_Audio_Set_Effect_Mode Fail\n");
	}	

	// TVFE get test
	memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
	ret = ioctl(DSPfd,DSP_TVFE_Audio_Get_Main_Equlizer_Config,&TVFE_CMD);
	if(ret != 0)
	{
		printf("DSP_TVFE_Audio_Get_Main_Equlizer_Config Fail\n");
	}else{
		printf("Enable_Bypass :%d\n",TVFE_CMD.Get_Main_Equlizer_Config.EQ.Enable_Bypass);
		printf("Band1_0100HZ :%d\n",TVFE_CMD.Get_Main_Equlizer_Config.EQ.Band1_0100HZ);
		printf("Band2_0300HZ :%d\n",TVFE_CMD.Get_Main_Equlizer_Config.EQ.Band2_0300HZ);
		printf("Band3_1000HZ :%d\n",TVFE_CMD.Get_Main_Equlizer_Config.EQ.Band3_1000HZ);
		printf("Band4_3000HZ :%d\n",TVFE_CMD.Get_Main_Equlizer_Config.EQ.Band4_3000HZ);
		printf("Band5_8000HZ :%d\n",TVFE_CMD.Get_Main_Equlizer_Config.EQ.Band5_8000HZ);
	}

	// TVFE Get Master mute value
	memset(&TVFE_CMD,0,sizeof(TVFE_Audio_CMD_st));
	ret = ioctl(DSPfd,DSP_TVFE_Audio_Get_MasterMuteStatus,&TVFE_CMD);
	if(ret != 0)
	{
		printf("DSP_TVFE_Audio_Get_MasterMuteStatus Fail\n");
	}else{
		printf("DSP_TVFE_Audio_Get_MasterMuteStatus :%d\n",TVFE_CMD.Get_MasterMuteStatus.mute);
	}

	// DSP demute test
	ret = ioctl(DSPfd,DSP_AUDIF_Demute,NULL);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Demute Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Demute :%d\n",ret);
	}

	// DSP DSP_AUDIF_Get_audBSbuf_start test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_start,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_audBSbuf_start Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Get_audBSbuf_start :%d\n",ret);
	}

	// DSP DSP_AUDIF_Get_audBSbuf_free test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_free,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_audBSbuf_free Fail:%d,size:0x%x\n",ret,dspCMD.argument_1);
	}else{
		printf("DSP_AUDIF_Get_audBSbuf_free :%d,size:0x%x\n",ret,dspCMD.argument_1);
	}

	// DSP DSP_AUDIF_Get_audBSbuf_size test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_size,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_audBSbuf_size Fail:%d,size:0x%x\n",ret,dspCMD.argument_1);
	}else{
		printf("DSP_AUDIF_Get_audBSbuf_size :%d,size:0x%x\n",ret,dspCMD.argument_1);
	}

#endif
#if 0
	// DSP_AUDIF_Get_DumpPtr test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_DumpPtr,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_DumpPtr Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Get_DumpPtr :%d\n",ret);
	}


	// DSP DSP_AUDIF_Set_DumpPtr test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	dspCMD.argument_2 = 0x2000;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_DumpPtr,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_DumpPtr Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Set_DumpPtr :%d\n",ret);
	}

	// DSP DSP_AUDIF_Get_audBSbuf_free test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_free,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_audBSbuf_free Fail:%d,size:0x%x\n",ret,dspCMD.argument_1);
	}else{
		printf("DSP_AUDIF_Get_audBSbuf_free :%d,size:0x%x\n",ret,dspCMD.argument_1);
	}

	// DSP_AUDIF_Get_DumpPtr test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_DumpPtr,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_DumpPtr Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Get_DumpPtr :%d\n",ret);
	}


#endif
#if 0
	// DSP_AUDIF_Get_DumpPtr test
	memset(&wfmt,0,sizeof(WAVFORMATMACRO));
	wfmt.wfx.wFormatTag      = WAVE_FORMAT_MPEGLAYER3; /* format type */
	wfmt.wfx.nChannels       = 0x0; /* number of channels (i.e. mono, stereo...) */
	wfmt.wfx.nSamplesPerSec  = 44100; /* sample rate */
	wfmt.wfx.nAvgBytesPerSec = 0x0;/* for buffer estimation */
	wfmt.wfx.nBlockAlign     = 0x0; /* block size of data */
	wfmt.wfx.wBitsPerSample  = 0x0; //chnnels = frame_bits/smaple_bits!!
	wfmt.wfx.cbSize          = 0x0;/* The count in bytes of the size of extra information (after cbSize) */

	ret = ioctl(DSPfd,DSP_AUDIF_Init_Wave_Audio,&wfmt);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Init_Wave_Audio Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Init_Wave_Audio :%d\n",ret);
	}

	//ioctl(DSPfd,DSP_AUDIF_Set_FIFOEnable,NULL);
#endif

	// DSP DSP_AUDIF_Get_audBSbuf_start test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_start,&dspCMD);
	
	ioctl(DSPfd,DSP_AUDIF_Update_Dec1_SampleRate,NULL);

	// DSP DSP_AUDIF_Clean_Audio_Buf test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ioctl(DSPfd,DSP_AUDIF_Clean_Audio_Buf,&dspCMD);

	// DSP DSP_AUDIF_Set_Play test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ioctl(DSPfd,DSP_AUDIF_Set_Play,&dspCMD);


	//ret = PlayMp3();
	//perror("PlayMP3\n");

	//PlayMp3_BSFeed(argv[1]);
	//perror("PlayMp3_BSFeed\n");

#if 0
	// DSP DSP_AUDIF_Set_Play test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_Play,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_Play Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Set_Play :%d\n",ret);
	}


	// DSP DSP_AUDIF_Set_HDMI test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x0;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_HDMI,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_HDMI Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Set_HDMI :%d\n",ret);
	}

	// DSP_AUDIF_Set_Pause test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x01;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_Pause,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_Pause Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Set_Pause :%d\n",ret);
	}

	// DSP DSP_AUDIF_Set_Stop test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_Stop,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Set_Stop Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Set_Stop :%d\n",ret);
	}

	// DSP_AUDIF_Get_BarrierPtr  test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_BarrierPtr,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_BarrierPtr Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Get_BarrierPtr :%d\n",ret);
	}

	// DSP_AUDIF_Get_DumpPtr test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_DumpPtr,&dspCMD);
	if(ret != 0)
	{
		printf("DSP_AUDIF_Get_DumpPtr Fail:%d\n",ret);
	}else{
		printf("DSP_AUDIF_Get_DumpPtr :%d\n",ret);
	}
#endif
	// write test
	//ret = write(DSPfd,TestArray,sizeof(TestArray));
	//printf("write byte:%d\n",ret);

	close(DSPfd);

	return 0;
}
