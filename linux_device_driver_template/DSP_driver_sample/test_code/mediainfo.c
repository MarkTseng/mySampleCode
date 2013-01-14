// tutorial01.c
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use
//
// gcc -o tutorial01 tutorial01.c -lavutil -lavformat -lavcodec -lz
//
// to build (assuming libavformat and libavcodec are correctly installed
// your system).
//
// Run using
//
// tutorial01 myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM
// format.

static char const rcsid[] = "$Id: mediainfo, v0.0.0.1 " __DATE__ " " __TIME__ " Mark Exp $";

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>

#include <stdio.h>
#include "cyg-profile.h"

int main(int argc, char *argv[]) {
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
	int ret,arg_count;

	if(argc < 2) {
		printf("Please provide a movie file\n");
		return -1;
	}

	//function trace log
	printf ("Logfile: %s\n", cygprofile_getfilename ());
    cygprofile_enable ();

	// out argv
	for(arg_count=0; arg_count<argc; arg_count++)
		printf("arg[%d]: %s\n",arg_count,argv[arg_count]);

	// Register all formats and codecs
	av_register_all();
	printf("av_register_all\n");

	av_log_set_level(AV_LOG_DEBUG);
	
	// Open video file
	//if(av_open_input_file(&pFormatCtx, argv[1], NULL, 0, NULL)!=0)
	//	return -1; // Couldn't open file
	ret = av_open_input_file(&pFormatCtx, argv[1], NULL, 0, NULL);
	printf("av_open_input_file ret: %d\n",ret);
	if(ret != 0)
		return -1;

	// Retrieve stream information
	pFormatCtx->probesize = 64;
	if(av_find_stream_info(pFormatCtx)<0)
		return -1; // Couldn't find stream information
	printf("av_find_stream_info\n");

	// Dump information about file onto standard error
	dump_format(pFormatCtx, 0, argv[1], 0);
	printf("dump_format\n");

// enable to see all infomation
#if 1
	// Find the first video stream
	videoStream = -1;
	audioStream = -1;

	for(i=0; i<pFormatCtx->nb_streams; i++) 
	{
		printf("Codec_type: %d\n",pFormatCtx->streams[i]->codec->codec_type);
#if 1
		if(pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO) {
			audioStream = i;
		}

		if(pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
			videoStream = i;
		}
#endif
	}

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

		printf("sample_rate:%d, channels:%d, fmt:%s\n",
				pCodecCtx->sample_rate,
				pCodecCtx->channels,
				av_get_sample_fmt_name(pCodecCtx->sample_fmt)
				);
		printf("Audio stream bitrate:%d\n",pCodecCtx->bit_rate);
		printf("Audio stream codec:%s\n",pCodec->name);

	}


	// Read frames and save first five frames to disk
	i=0;
	while(av_read_frame(pFormatCtx, &packet)>=0) {

		i++;
		TotalPacketSize += packet.size;
#if 0
		printf("Audio Frame: %d,Packet Size: %d\n",i,packet.size);
		printf("Audio Frame: %d,Packet duration: %d\n",i,packet.duration);
		printf("Audio Frame: %d,Packet pos: %lld\n",i,packet.pos);
#endif
#if 1
		if(packet.stream_index == videoStream)
		{
			//printf("Video Frame: %d,Duration:%d, PTS:%lld,DTS:%lld\n",i,packet.duration,packet.pts,packet.dts);
			//printf("Video Frame: %d,Packet Size: %d\n",i,packet.size);
			VideoFrameNum++;
			VideoPacketSize += packet.size;
		}
#endif
#if 1
		if(packet.stream_index == audioStream)
		{
			//printf("Audio Frame: %d,Duration:%d, PTS:%lld,DTS:%lld\n",i,packet.duration,packet.pts,packet.dts);
			//printf("Audio Frame: %d,Packet Size: %d\n",i,packet.size);
			/*
			for(audioCount = 0 ; audioCount < packet.size ; audioCount++)
			{
				printf("Data 0x%x:0x%x\n",audioCount,*(packet.data + audioCount));
			}
			*/
			AudioFrameNum++;
			AudioPacketSize += packet.size;
		}
#endif
		av_free_packet(&packet);
	}

	printf("Total frame number:%d\n",i);
	printf("Total video frame number:%d\n",VideoFrameNum);
	printf("Total audio frame number:%d\n",AudioFrameNum);
	printf("Total packet size:%lld\n",TotalPacketSize);
	printf("Total Video packet size:%lld\n",VideoPacketSize);
	printf("Total Audio packet size:%lld\n",AudioPacketSize);
	printf("AV_TIME_BASE:%d\n",AV_TIME_BASE);
#endif
	// Close the video file
	av_close_input_file(pFormatCtx);

	return 0;
}
