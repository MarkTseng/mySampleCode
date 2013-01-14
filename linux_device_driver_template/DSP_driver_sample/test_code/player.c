#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>

#include "libavutil/avstring.h"
#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/opt.h"
#include "libswscale/swscale.h"

#define DECODED_AUDIO_BUFFER_SIZE            192000

struct options
{
	int streamId;
	int frames;
	int nodec;
	int bplay;
	int thread_count;
	int64_t lstart;
	char finput[256];
	char foutput1[256];
	char foutput2[256];
};

int parse_options(struct options *opts, int argc, char** argv)
{
	int optidx;
	char *optstr;

	if (argc < 2) return -1;

	opts->streamId = -1;
	opts->lstart = -1;
	opts->frames = -1;
	opts->foutput1[0] = 0;
	opts->foutput2[0] = 0;
	opts->nodec = 0;
	opts->bplay = 0;
	opts->thread_count = 0;
	strcpy(opts->finput, argv[1]);

	optidx = 2;
	while (optidx < argc)
	{
		optstr = argv[optidx++];
		if (*optstr++ != '-') return -1;
		switch (*optstr++)
		{
			case 's':  //< stream id
				opts->streamId = atoi(optstr);
				break;
			case 'f':  //< frames
				opts->frames = atoi(optstr);
				break;
			case 'k':  //< skipped
				opts->lstart = atoll(optstr);
				break;
			case 'o':  //< output
				strcpy(opts->foutput1, optstr);
				strcat(opts->foutput1, ".mpg");
				strcpy(opts->foutput2, optstr);
				strcat(opts->foutput2, ".raw");
				break;
			case 'n': //decoding and output options
				if (strcmp("dec", optstr) == 0)
					opts->nodec = 1;
				break;
			case 'p':
				opts->bplay = 1;
				break;
			case 't':
				opts->thread_count = atoi(optstr);
				break;
			default:
				return -1;
		}
	}

	return 0;
}

void show_help(char* program)
{
	printf("Simple FFMPEG test program\n");
	printf("Usage: %s inputfile [-sstreamid [-fframes] [-kskipped] [-ooutput_filename(without extension)] [-p] [-tthread_count]]\n", 
			program);
	return;
}

static void log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
	vfprintf(stdout, fmt, vl);
}

/*
 * audio renderer code (oss)
 */
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#define OSS_DEVICE "/dev/dsp0"

struct audio_dsp
{
	int audio_fd;
	int channels;
	int format;
	int speed;
};
int map_formats(enum SampleFormat format)
{
	switch(format)
	{
		case SAMPLE_FMT_U8:
			return AFMT_U8;
		case SAMPLE_FMT_S16:
			return AFMT_S16_LE;
		default:
			return AFMT_U8; 
	}
}
int set_audio(struct audio_dsp* dsp)
{
	if (dsp->audio_fd == -1)
	{
		printf("Invalid audio dsp id!\n");
		return -1;
	}    

	if (-1 == ioctl(dsp->audio_fd, SNDCTL_DSP_SETFMT, &dsp->format))
	{
		printf("Failed to set dsp format!\n");
		return -1;
	}

	if (-1 == ioctl(dsp->audio_fd, SNDCTL_DSP_CHANNELS, &dsp->channels))
	{
		printf("Failed to set dsp format!\n");
		return -1;
	}

	if (-1 == ioctl(dsp->audio_fd, SNDCTL_DSP_SPEED, &dsp->speed))
	{
		printf("Failed to set dsp format!\n");
		return -1;
	}    
	return 0;
}

int play_pcm(struct audio_dsp* dsp, unsigned char *buf, int size)
{
	if (dsp->audio_fd == -1)
	{
		printf("Invalid audio dsp id!\n");
		return -1;
	}

	if (-1 == write(dsp->audio_fd, buf, size))
	{
		printf("Failed to write audio dsp!\n");
		return -1;
	}

	return 0;
}
/* audio renderer code end */

/* video renderer code*/
#include <linux/fb.h>
#include <sys/mman.h>

#define FB_DEVICE "/dev/fb0"

enum pic_format
{
	eYUV_420_Planer,
};
struct video_fb
{
	int video_fd;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	unsigned char *fbp;
	AVFrame *frameRGB;
	struct 
	{
		int x;
		int y;
	} video_pos;
};

int open_video(struct video_fb *fb, int x, int y)
{
	int screensize;
	fb->video_fd = open(FB_DEVICE, O_WRONLY);
	if (fb->video_fd == -1) return -1;

	if (ioctl(fb->video_fd, FBIOGET_FSCREENINFO, &fb->finfo)) return -2;
	if (ioctl(fb->video_fd, FBIOGET_VSCREENINFO, &fb->vinfo)) return -2;


	printf("video device: resolution %dx%d, %dbpp\n", fb->vinfo.xres, fb->vinfo.yres, fb->vinfo.bits_per_pixel);
	screensize = fb->vinfo.xres * fb->vinfo.yres * fb->vinfo.bits_per_pixel / 8;
	fb->fbp = (unsigned char *) mmap(0, screensize, PROT_READ|PROT_WRITE, MAP_SHARED, fb->video_fd, 0);
	if (fb->fbp == -1) return -3;

	if (x >= fb->vinfo.xres || y >= fb->vinfo.yres)
	{
		return -4;
	}
	else
	{
		fb->video_pos.x = x;
		fb->video_pos.y = y;
	}

	fb->frameRGB = avcodec_alloc_frame();
	if (!fb->frameRGB) return -5;

	return 0;
}
#if 0
/* only 420P supported now */
int show_picture(struct video_fb *fb, AVFrame *frame, int width, int height, enum pic_format format)
{
	struct SwsContext *sws;
	int i;
	unsigned char *dest;
	unsigned char *src;

	if (fb->video_fd == -1) return -1;
	if ((fb->video_pos.x >= fb->vinfo.xres) || (fb->video_pos.y >= fb->vinfo.yres)) return -2;

	if (fb->video_pos.x + width > fb->vinfo.xres)
	{
		width = fb->vinfo.xres - fb->video_pos.x;
	}
	if (fb->video_pos.y + height > fb->vinfo.yres)
	{
		height = fb->vinfo.yres - fb->video_pos.y;
	}

	if (format == PIX_FMT_YUV420P)
	{
		sws = sws_getContext(width, height, format, width, height, PIX_FMT_RGB32, SWS_FAST_BILINEAR, NULL, NULL, NULL);
		if (sws == 0)
		{
			return -3;
		}
		if (sws_scale(sws, frame->data, frame->linesize, 0, height, fb->frameRGB->data, fb->frameRGB->linesize))
		{
			return -3;
		}

		dest = fb->fbp + (fb->video_pos.x+fb->vinfo.xoffset) * (fb->vinfo.bits_per_pixel/8) +(fb->video_pos.y+fb->vinfo.yoffset) * fb->finfo.line_length;
		for (i = 0; i < height; i++)
		{
			memcpy(dest, src, width*4);
			src += fb->frameRGB->linesize[0];
			dest += fb->finfo.line_length;
		}
	}
	return 0;
}
#endif
void close_video(struct video_fb *fb)
{
	if (fb->video_fd != -1) 
	{
		munmap(fb->fbp, fb->vinfo.xres * fb->vinfo.yres * fb->vinfo.bits_per_pixel / 8);
		close(fb->video_fd);
		fb->video_fd = -1;
	}
}
/* video renderer code end */

int main(int argc, char **argv)
{
	AVFormatContext* pCtx = 0;
	AVCodecContext *pCodecCtx = 0;
	AVCodec *pCodec = 0;
	AVPacket packet;
	AVFrame *pFrame = 0;
	FILE *fpo1 = NULL;
	FILE *fpo2 = NULL;
	int nframe;
	int err;
	int got_picture;
	int picwidth, picheight, linesize;
	unsigned char *pBuf;
	int16_t *pSndBuf = NULL;    
	int i;
	int64_t timestamp;
	struct options opt;
	int usefo = 0;
	struct audio_dsp dsp;
	int dusecs;
	float usecs1 = 0;
	float usecs2 = 0;
	struct timeval elapsed1, elapsed2;
	int decoded = 0;

	av_register_all();

	av_log_set_callback(log_callback);
	av_log_set_level(50);

	if (parse_options(&opt, argc, argv) < 0 || (strlen(opt.finput) == 0))
	{
		show_help(argv[0]);
		return 0;
	}


	err = av_open_input_file(&pCtx, opt.finput, 0, 0, 0);
	if (err < 0)
	{
		printf("\n->(av_open_input_file)\tERROR:\t%d\n", err);
		goto fail;
	}
	err = av_find_stream_info(pCtx);
	if (err < 0)
	{
		printf("\n->(av_find_stream_info)\tERROR:\t%d\n", err);
		goto fail;
	}
	if (opt.streamId < 0)
	{
		dump_format(pCtx, 0, pCtx->filename, 0);
		goto fail;
	}
	else
	{
		printf("\n extra data in Stream %d (%dB):", opt.streamId, pCtx->streams[opt.streamId]->codec->extradata_size);
		for (i = 0; i < pCtx->streams[opt.streamId]->codec->extradata_size; i++)
		{
			if (i%16 == 0) printf("\n");
			printf("%2x  ", pCtx->streams[opt.streamId]->codec->extradata[i]);
		}
	}
	/* try opening output files */
	if (strlen(opt.foutput1) && strlen(opt.foutput2))
	{
		fpo1 = fopen(opt.foutput1, "wb");
		fpo2 = fopen(opt.foutput2, "wb");
		if (!fpo1 || !fpo2)
		{
			printf("\n->error opening output files\n");
			goto fail;
		}
		usefo = 1;
	}
	else
	{
		usefo = 0;
	}

	if (opt.streamId >= pCtx->nb_streams)
	{
		printf("\n->StreamId\tERROR\n");
		goto fail;
	}

	if (opt.lstart > 0)
	{
		err = av_seek_frame(pCtx, opt.streamId, opt.lstart, AVSEEK_FLAG_ANY);
		if (err < 0)
		{
			printf("\n->(av_seek_frame)\tERROR:\t%d\n", err);
			goto fail;
		}
	}

	/* for decoder configuration */
	if (!opt.nodec)
	{
		/* prepare codec */
		pCodecCtx = pCtx->streams[opt.streamId]->codec;

		if (opt.thread_count <= 16 && opt.thread_count > 0 )
		{
			pCodecCtx->thread_count = opt.thread_count;
			pCodecCtx->thread_type = FF_THREAD_FRAME;
		}
		pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
		if (!pCodec)
		{
			printf("\n->can not find codec!\n");
			goto fail;
		}
		err = avcodec_open(pCodecCtx, pCodec);
		if (err < 0)
		{
			printf("\n->(avcodec_open)\tERROR:\t%d\n", err);
			goto fail;
		}
		pSndBuf = (int16_t *) malloc(DECODED_AUDIO_BUFFER_SIZE);
		if (!pSndBuf)
		{
			printf("\n->no memory\n");
			goto fail;
		}
		pFrame = avcodec_alloc_frame();        

		/* prepare device */
		if (opt.bplay)
		{
			/* audio devices */
			dsp.audio_fd = open(OSS_DEVICE, O_WRONLY);
			if (dsp.audio_fd == -1)
			{
				printf("\n-> can not open audio device\n");
				goto fail;
			}
			dsp.channels = pCodecCtx->channels;
			dsp.speed = pCodecCtx->sample_rate;
			dsp.format = map_formats(pCodecCtx->sample_fmt);
			if (set_audio(&dsp) < 0)
			{
				printf("\n-> can not set audio device\n");
				goto fail;
			}
			/* video devices */
		}
	}

	nframe = 0;
	while(nframe < opt.frames || opt.frames == -1)
	{
		gettimeofday(&elapsed1, NULL);
		err = av_read_frame(pCtx, &packet);
		if (err < 0)
		{
			printf("\n->(av_read_frame)\tERROR:\t%d\n", err);
			break;
		}
		gettimeofday(&elapsed2, NULL);
		dusecs = (elapsed2.tv_sec - elapsed1.tv_sec)*1000000 + (elapsed2.tv_usec - elapsed1.tv_usec);
		usecs2 += dusecs;        
		timestamp = av_rescale_q(packet.dts, pCtx->streams[packet.stream_index]->time_base, (AVRational){1, AV_TIME_BASE});
		printf("\nFrame No %5d stream#%d\tsize %6dB, timestamp:%6lld, dts:%6lld, pts:%6lld, ", nframe++, packet.stream_index, packet.size, 
				timestamp, packet.dts, packet.pts);

		if (packet.stream_index == opt.streamId)
		{
#if 0 
			for (i = 0; i < 16; /*packet.size;*/ i++)
			{
				if (i%16 == 0) printf("\n pktdata: ");
				printf("%2x  ", packet.data[i]);
			}
			printf("\n");
#endif            
			if (usefo)
			{
				fwrite(packet.data, packet.size, 1, fpo1);
				fflush(fpo1);
			}

			if (pCtx->streams[opt.streamId]->codec->codec_type == CODEC_TYPE_VIDEO && !opt.nodec)
			{
				picheight = pCtx->streams[opt.streamId]->codec->height;
				picwidth = pCtx->streams[opt.streamId]->codec->width;

				gettimeofday(&elapsed1, NULL); 
				avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
				decoded++;
				gettimeofday(&elapsed2, NULL);
				dusecs = (elapsed2.tv_sec - elapsed1.tv_sec)*1000000 + (elapsed2.tv_usec - elapsed1.tv_usec);
				usecs1 += dusecs;

				if (got_picture)
				{
					printf("[Video: type %d, ref %d, pts %lld, pkt_pts %lld, pkt_dts %lld]", 
							pFrame->pict_type, pFrame->reference, pFrame->pts, pFrame->pkt_pts, pFrame->pkt_dts);

					if (pCtx->streams[opt.streamId]->codec->pix_fmt == PIX_FMT_YUV420P)
					{
						if (usefo)
						{
							linesize = pFrame->linesize[0];
							pBuf = pFrame->data[0];
							for (i = 0; i < picheight; i++)
							{
								fwrite(pBuf, picwidth, 1, fpo2);
								pBuf += linesize;
							}

							linesize = pFrame->linesize[1];
							pBuf = pFrame->data[1];
							for (i = 0; i < picheight/2; i++)
							{
								fwrite(pBuf, picwidth/2, 1, fpo2);
								pBuf += linesize;
							}           

							linesize = pFrame->linesize[2];
							pBuf = pFrame->data[2];
							for (i = 0; i < picheight/2; i++)
							{
								fwrite(pBuf, picwidth/2, 1, fpo2);
								pBuf += linesize;
							}  
							fflush(fpo2);
						} 

						if (opt.bplay)
						{
							/* show picture */
						}
					}
				}
				av_free_packet(&packet);
			}
			else if (pCtx->streams[opt.streamId]->codec->codec_type == CODEC_TYPE_AUDIO && !opt.nodec)
			{
				int size = DECODED_AUDIO_BUFFER_SIZE;

				gettimeofday(&elapsed1, NULL);
				avcodec_decode_audio3(pCodecCtx, pSndBuf, &size, &packet);
				decoded++;
				gettimeofday(&elapsed2, NULL);
				dusecs = (elapsed2.tv_sec - elapsed1.tv_sec)*1000000 + (elapsed2.tv_usec - elapsed1.tv_usec);
				usecs1 += dusecs;

				printf("[Audio: %5dB raw data, decoding time: %d]", size, dusecs);
				if (usefo)
				{
					fwrite(pSndBuf, size/2, 1, fpo2);
					fflush(fpo2);
				}
				if (opt.bplay)
				{
					play_pcm(&dsp, pSndBuf, size);
				}
			}
		}
	}  

	if (!opt.nodec && pCodecCtx)
	{
		avcodec_close(pCodecCtx);
	}

	printf("\n%d frames parsed, average %.2f us per frame\n", nframe, usecs2/nframe);
	printf("%d frames decoded, average %.2f us per frame\n", decoded, usecs1/decoded);

	fail:
	if (pSndBuf)
	{
		free(pSndBuf);
	}
	if (pCtx)
	{
		av_close_input_file(pCtx);
	}
	if (fpo1)
	{
		fclose(fpo1);
	}
	if (fpo2)
	{
		fclose(fpo2);
	}
	if (!pFrame)
	{
		av_free(pFrame);
	}
	if (!usefo && (dsp.audio_fd != -1))
	{
		close(dsp.audio_fd);
	}
	return 0;
}
