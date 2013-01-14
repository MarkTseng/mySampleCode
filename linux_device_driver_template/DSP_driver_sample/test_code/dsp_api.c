#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>		/* ioctl */
#include <pthread.h>
#include <signal.h>
//#include <mcheck.h>  // check memory leak

#include "dsp_api.h"

#define	 DSP_DEVNAME	"/dev/dsp"
#define	 DSP_MAJOR_NR	42
#define	 DSP_MINOR_NR	0

// return FD
int DSPDEV_OPEN()
{
	int DSPfd,ret;

	DSPfd = open(DSP_DEVNAME,O_RDWR);
	if(DSPfd < 0)
	{
		/* auto create dsp device node */
		ret = mknod(DSP_DEVNAME,S_IFCHR,makedev(DSP_MAJOR_NR,DSP_MINOR_NR));
		if(ret)
		{
			perror("mknod fail");
			return -ENODEV;
		}
		DSPfd = open(DSP_DEVNAME,O_RDWR);
		if(DSPfd < 0)
			return -ENODEV;
	}
	return DSPfd;
}

// return successful?
int DSPDEV_CLOSE(int DSPfd)
{
	int ret = 0;
	ret = close(DSPfd);
	return ret;
}
 
// to be modified
int DSPDEV_SetAudioWavefmt(int DSPfd,int AudioCodecType)
{
	int ret;
	WAVFORMATMACRO wfmt;

	memset(&wfmt,0,sizeof(WAVFORMATMACRO));

	switch (AudioCodecType)
	{
		case WAVE_FORMAT_MPEGLAYER3:
			wfmt.wfx.wFormatTag      = WAVE_FORMAT_MPEGLAYER3; /* format type */
			break;

		case WAVE_FORMAT_DOLBY_AC3:
			wfmt.wfx.wFormatTag      = WAVE_FORMAT_DOLBY_AC3; /* format type */
			break;
		default:
			printf("no this codec type\n");
			break;
	}

	wfmt.wfx.nChannels       = 0x05; /* number of channels (i.e. mono, stereo...) */
	wfmt.wfx.nSamplesPerSec  = 44100; /* sample rate */
	wfmt.wfx.nAvgBytesPerSec = 0x0;/* for buffer estimation */
	wfmt.wfx.nBlockAlign     = 0x0; /* block size of data */
	wfmt.wfx.wBitsPerSample  = 0x0; //chnnels = frame_bits/smaple_bits!!
	wfmt.wfx.cbSize          = 0x0;/* The count in bytes of the size of extra information (after cbSize) */

	ret = ioctl(DSPfd,DSP_AUDIF_Init_Wave_Audio,&wfmt);

	//ioctl(DSPfd,DSP_AUDIF_Set_FIFOEnable,NULL);
	return ret;
}

int DSPDEV_GetAudioWavefmt(int DSPfd,WAVFORMATMACRO *wfmt)
{

	return 0;
}

int DSPDEV_CleanAudioBuf(int DSPfd)
{
	DSP_IOCTL_CMD dspCMD;
	int ret = AUDIF_SUCCESS;
	// DSP DSP_AUDIF_Clean_Audio_Buf test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Clean_Audio_Buf,&dspCMD);
	if(ret != AUDIF_SUCCESS)
		return ret;
	
	// DSP DSP_AUDIF_Set_Play test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Set_Play,&dspCMD);
	if(ret != AUDIF_SUCCESS)
		return ret;

	return AUDIF_SUCCESS;
}

int DSPDEV_GetBSFreeSize(int DSPfd)
{
	DSP_IOCTL_CMD dspCMD;
	int ret = AUDIF_SUCCESS;
	
	// DSP DSP_AUDIF_Get_audBSbuf_free test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ret = ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_free,&dspCMD);
	//printf("DSP_AUDIF_Get_audBSbuf_free size:%d\n",dspCMD.argument_1);
	if(ret != AUDIF_SUCCESS)
		return ret;
	
	return dspCMD.argument_1;
}

