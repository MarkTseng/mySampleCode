/*
 * Sunplus DSP ioctl command test program
 * Author : Mark Tseng
 * DOCUMENT: 
 *		1. Create dsp device node
 *		# mknod /dev/dsp c 42 0
 *		2. execute dsp_ioctl
 * $Id: dsp_ioctl.c,v 0.1 2011/11/21 Mark Tseng Exp $
 */
static char const rcsid[] = "$Id: playback, v0.0.0.1 " __DATE__ " " __TIME__ " Mark Exp $";

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

// ffmpeg header
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>


#include "types.h"
#include "wave_fmt.h"
#include "plf_av_source.h"
#include "drv_ssd.h"
#include "tvfe_api.h"
#include "tvfe_audio.h"
#include "dsp_ioctl.h"

#include "cyg-profile.h"

#if 0
typedef struct SampleFmtInfo {
    const char *name;
    int bits;
} SampleFmtInfo;

/**
 * all in native-endian format
 */
enum AVSampleFormat {
    AV_SAMPLE_FMT_NONE = -1,
    AV_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    AV_SAMPLE_FMT_S16,         ///< signed 16 bits
    AV_SAMPLE_FMT_S32,         ///< signed 32 bits
    AV_SAMPLE_FMT_FLT,         ///< float
    AV_SAMPLE_FMT_DBL,         ///< double
    AV_SAMPLE_FMT_NB           ///< Number of sample formats. DO NOT USE if linking dynamically
};



/** this table gives more information about formats */
static const SampleFmtInfo sample_fmt_info[AV_SAMPLE_FMT_NB] = {
    [AV_SAMPLE_FMT_U8]  = { .name = "u8",  .bits = 8 },
    [AV_SAMPLE_FMT_S16] = { .name = "s16", .bits = 16 },
    [AV_SAMPLE_FMT_S32] = { .name = "s32", .bits = 32 },
    [AV_SAMPLE_FMT_FLT] = { .name = "flt", .bits = 32 },
    [AV_SAMPLE_FMT_DBL] = { .name = "dbl", .bits = 64 },
};

const char *av_get_sample_fmt_name(enum AVSampleFormat sample_fmt)
{
    if (sample_fmt < 0 || sample_fmt >= AV_SAMPLE_FMT_NB)
        return NULL;
    return sample_fmt_info[sample_fmt].name;
}

#endif

#define	 DSP_DEVNAME	"/dev/dsp"
#define	 DSP_MAJOR_NR	42
#define	 DSP_MINOR_NR	0
int DSPfd;


int DSPDEV_OPEN()
{
	int ret = 0;

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
	return ret;
}


int DSPDEV_CLOSE()
{
	int ret = 0;
	ret = close(DSPfd);
	return ret;
}

int SetAudioWavefmt(int AudioCodec)
{
	int ret;
	WAVFORMATMACRO wfmt;
	// DSP_AUDIF_Get_DumpPtr test
	memset(&wfmt,0,sizeof(WAVFORMATMACRO));

	switch (AudioCodec)
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
	//wfmt.wfx.wFormatTag      = WAVE_FORMAT_MPEGLAYER3; /* format type */
	//wfmt.wfx.wFormatTag      = WAVE_FORMAT_DOLBY_AC3; /* format type */
	wfmt.wfx.nChannels       = 0x05; /* number of channels (i.e. mono, stereo...) */
	//wfmt.wfx.nSamplesPerSec  = 44100; /* sample rate */
	wfmt.wfx.nSamplesPerSec  = 48000; /* sample rate */
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
	return 0;
}

int CleanAudioBuf()
{
	DSP_IOCTL_CMD dspCMD;
	// DSP DSP_AUDIF_Clean_Audio_Buf test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ioctl(DSPfd,DSP_AUDIF_Clean_Audio_Buf,&dspCMD);

	// DSP DSP_AUDIF_Set_Play test
	memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
	dspCMD.argument_1 = 0x1;
	ioctl(DSPfd,DSP_AUDIF_Set_Play,&dspCMD);


	return 0;
}

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


AVFormatContext *pFormatCtx;
int             i,videoCount,audioCount, videoStream,audioStream;
AVCodecContext  *pCodecCtx;
AVCodec         *pCodec;
AVFrame         *pFrame; 
AVFrame         *pFrameRGB;
AVPacket        packet;
AVStream		*video_st,*audio_st;
int64_t			TotalPacketSize = 0;
const char		*codec_name;
uint8_t         *buffer;
unsigned int	AudioFrameNum = 0 ,VideoFrameNum = 0;
int64_t			AudioPacketSize = 0 ,VideoPacketSize = 0;

int PlayMp3_BSFeedByFrame(char *FileName)
{
	int read_byte=0,write_byte=0,ret;
	unsigned char Buf[BSFeed40K]; 
	DSP_IOCTL_CMD dspCMD;
	unsigned int barN,BSFreeSize;
	struct timeval elapsed1, elapsed2;
	int dusecs;
	float usecs2 = 0;

#if 1
	// ffmpeg log level to DEBUG
	av_log_set_level(AV_LOG_DEBUG);
#endif

	ret = av_open_input_file(&pFormatCtx, FileName, NULL, 0, NULL);
	printf("av_open_input_file ret: %d\n",ret);
	if(ret != 0)
		return -1;

	// Retrieve stream information
	pFormatCtx->probesize = 64;
	if(av_find_stream_info(pFormatCtx)<0)
		return -1; // Couldn't find stream information
	printf("av_find_stream_info\n");

	// Dump information about file onto standard error
	//dump_format(pFormatCtx, 0, FileName, 0);
	//printf("dump_format\n");


	videoStream = -1;
	audioStream = -1;

	for(i=0; i<pFormatCtx->nb_streams; i++) 
	{
		//printf("Codec_type: %d\n",pFormatCtx->streams[i]->codec->codec_type);
	
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioStream = i;
		}

		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = i;
		}
	}

	av_log(NULL, AV_LOG_WARNING, "videoStream:%d\n",videoStream);
	av_log(NULL, AV_LOG_WARNING, "audioStream:%d\n",audioStream);

	if( videoStream != -1){
		// Get a pointer to the codec context for the video stream
		pCodecCtx=pFormatCtx->streams[videoStream]->codec;

		printf("videoStream info\n");
		// Find the decoder for the video stream
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
		if(pCodec==NULL) {
			fprintf(stderr, "Unsupported codec!\n");
			return -1; // Codec not found
		}else{
			printf("Codec ID:%d\n",pCodecCtx->codec_id);
		}

		printf("Video stream bitrate:%d\n",pCodecCtx->bit_rate);
		printf("Video stream codec:%s\n",pCodec->name);
		printf("width:%d, height:%d\n",
				pCodecCtx->width,
				pCodecCtx->height
				);
	}

	if(audioStream != -1){
		// Get a pointer to the codec context for the video stream
		pCodecCtx=pFormatCtx->streams[audioStream]->codec;
		audio_st = pFormatCtx->streams[audioStream];

		printf("audioStream info\n");
		// Find the decoder for the video stream
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
		if(pCodec==NULL) {
			fprintf(stderr, "Unsupported codec!\n");
			return -1; // Codec not found
		}else{
			printf("Codec ID:%d\n",pCodecCtx->codec_id);
		}
		printf("Duration(sec):%lld\n",pFormatCtx->duration / AV_TIME_BASE);
		if (pFormatCtx->timestamp) {
			printf("timestamp:%lld \n",pFormatCtx->timestamp);
		}
		if (pFormatCtx->start_time) {
			printf("start_time:%lld \n",pFormatCtx->start_time );
		}
		if (pFormatCtx->file_size) {
			printf("file_size:%lld byte\n",pFormatCtx->file_size);
		}
		if (pFormatCtx->bit_rate) {
			printf("contain bitrate:%d kb/s\n",pFormatCtx->bit_rate / 1000);
		}
		if (pFormatCtx->packet_size) {
			printf("packet_size:%d \n",pFormatCtx->packet_size);
		}
		if (pFormatCtx->nb_programs) {
			printf("nb_programs:%d \n",pFormatCtx->nb_programs);
		}

#if 0
		printf("sample_rate:%d, channels:%d, fmt:%s\n",
				pCodecCtx->sample_rate,
				pCodecCtx->channels,
				av_get_sample_fmt_name(pCodecCtx->sample_fmt)
				);
#endif
		printf("sample_rate:%d, channels:%d \n",
				pCodecCtx->sample_rate,
				pCodecCtx->channels
				);


		printf("Audio stream bitrate:%d\n",pCodecCtx->bit_rate);
		printf("Audio stream codec:%s\n",pCodec->name);

		if(!strcmp(pCodec->name,"mp3"))
			SetAudioWavefmt(WAVE_FORMAT_MPEGLAYER3);
		
		if(!strcmp(pCodec->name,"ac3"))
			SetAudioWavefmt(WAVE_FORMAT_DOLBY_AC3);
	}


	i=0;
	while(1)
	{
		// DSP DSP_AUDIF_Get_audBSbuf_free test
		
		memset(&dspCMD,0,sizeof(DSP_IOCTL_CMD));
		dspCMD.argument_1 = 0x1;
		ioctl(DSPfd,DSP_AUDIF_Get_audBSbuf_free,&dspCMD);
		printf("DSP_AUDIF_Get_audBSbuf_free size:0x%x\n",dspCMD.argument_1);

		BSFreeSize = dspCMD.argument_1;

		if(BSFreeSize > BSFeed512K )
		{

			gettimeofday(&elapsed1, NULL);
			// Read frames and save first five frames to disk
			if(av_read_frame(pFormatCtx, &packet)>=0) 
			{
				gettimeofday(&elapsed2, NULL);
				dusecs = (elapsed2.tv_sec - elapsed1.tv_sec)*1000000 + (elapsed2.tv_usec - elapsed1.tv_usec);
				usecs2 += dusecs;        
				i++;
				if(packet.stream_index == videoStream)
				{
					printf("Video Frame: %d,Packet Size: %d\n",i,packet.size);
					//printf("read video frame time: %d usecs\n",dusecs);
				}

				if(packet.stream_index == audioStream)
				{
					//printf("Audio Frame: %d,Duration:%d, PTS:%lld,DTS:%lld\n",i,packet.duration,packet.pts,packet.dts);
					printf("Audio Frame: %d,Packet Size: %d\n",i,packet.size);
					//printf("read audio frame time: %d usecs\n",dusecs);
					write(DSPfd,packet.data,packet.size);

					/*
					for(audioCount = 0 ; audioCount < packet.size ; audioCount++)
					{

						printf("Data 0x%x:0x%x\n",audioCount,*(packet.data + audioCount));
					}
					*/
				}
				av_free_packet(&packet);
			}else{
				break;
			}

			usleep(10000);
		}
		else
		{
			//printf("BS Free Size le : %d\n",BSFeed640K);
			sleep(1);
		}
	}
	// Close the video file
	av_close_input_file(pFormatCtx);

	printf("\n%d frames parsed, average %.2f us per frame\n", i, usecs2/i);



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

	ret = DSPDEV_OPEN();

#if 0
	//function trace log
	printf ("Logfile: %s\n", cygprofile_getfilename ());
    cygprofile_enable ();
#endif

	CleanAudioBuf();
	//SetAudioWavefmt(&wfmt);

	if(argc < 2) {
		printf("Please provide a movie file\n");
		return -1;
	}

	// Register all formats and codecs
	av_register_all();
	printf("av_register_all\n");



	//PlayMp3_BSFeed(argv[1]);
	//perror("PlayMp3_BSFeed\n");

	PlayMp3_BSFeedByFrame(argv[1]);
	perror("PlayMp3_BSFeedByFrame\n");

	DSPDEV_CLOSE();

	return 0;
}


