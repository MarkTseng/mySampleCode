/*
 * Sunplus  Multimedia player program
 * Author : Mark Tseng
 * DOCUMENT: multimedia player 
 *   Note: restruct program follow, refer to ffplay.c      
 *   ffmpeg reference: http://dranger.com/ffmpeg/
 */
static char const rcsid[] = "$Id: avplayer, v0.1.0.0 " __DATE__ " " __TIME__ " Mark Exp $";

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

#ifdef MEMDEBUG
	#include <mcheck.h>  // check memory leak
#endif

// ffmpeg header
#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/samplefmt.h>
#include <libavutil/avstring.h>
#include <libswscale/swscale.h>

#include "cyg-profile.h"
#include "dsp_api.h"
#include "colortext.h"
int DSPfd;

// BSFeed Bitrate
#define BSFeed32K	(32 * 1024)
#define BSFeed40K	(40 * 1024)
#define BSFeed48K	(48 * 1024)
#define BSFeed64K	(64 * 1024)
#define BSFeed128K	(128 * 1024)
#define BSFeed224K	(224 * 1024)
#define BSFeed256K	(256 * 1024)
#define BSFeed384K	(384 * 1024)
#define BSFeed512K	(512 * 1024)
#define BSFeed640K	(640 * 1024)
#define BSFeed700K	(700 * 1024)

//audio queue buffer size
#define SDL_AUDIO_BUFFER_SIZE 1024
#define MAX_AUDIOQ_SIZE (5 * 16 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 256 * 1024)

#define MIN_FRAMES 5

/* no AV sync correction is done if below the AV sync threshold */
#define AV_SYNC_THRESHOLD 0.01
/* no AV correction is done if too big error */
#define AV_NOSYNC_THRESHOLD 10.0

#define FRAME_SKIP_FACTOR 0.05

/* maximum audio speed change to get correct sync */
#define SAMPLE_CORRECTION_PERCENT_MAX 10

/* we use about AUDIO_DIFF_AVG_NB A-V differences to make the average */
#define AUDIO_DIFF_AVG_NB   20

typedef struct PacketQueue {
    AVPacketList *first_pkt, *last_pkt;
    int nb_packets;
    int size;
    int abort_request;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} __attribute__((packed)) PacketQueue;

#define VIDEO_PICTURE_QUEUE_SIZE 2
#define SUBPICTURE_QUEUE_SIZE 4

enum {
    AV_SYNC_AUDIO_MASTER, /* default choice */
    AV_SYNC_VIDEO_MASTER,
    AV_SYNC_EXTERNAL_CLOCK, /* synchronize to an external clock */
};

#define DEFAULT_AV_SYNC_TYPE AV_SYNC_AUDIO_MASTER

typedef struct VideoPicture {
    double pts;             ///<presentation time stamp for this picture
    double target_clock;    ///<av_gettime() time at which this should be displayed ideally
    int64_t pos;            ///<byte position in file
    //SDL_Overlay *bmp;
    int width, height; /* source height & width */
    int allocated;
    enum PixelFormat pix_fmt;

} VideoPicture;

typedef struct SubPicture {
    double pts; /* presentation time stamp for this picture */
    AVSubtitle sub;
} SubPicture;

typedef enum {
	ProgStop,
	ProgRun,
	ProgExit,
}eProgStatus;

// global option variable
static int g_opt_saveframe = 0;
static int g_opt_dropframe = 1;

typedef struct VideoState {

	AVFormatContext *pFormatCtx;
	int             videoStream, audioStream;

	int             av_sync_type;
	double          external_clock; /* external clock base */
	int64_t         external_clock_time;
	int             seek_req;
	int             seek_flags;
	int64_t         seek_pos;
	double          audio_clock;
	AVStream        *audio_st;
	PacketQueue     audioq;
	DECLARE_ALIGNED(16, uint8_t, audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2]);
	unsigned int    audio_buf_size;
	unsigned int    audio_buf_index;
	AVPacket        audio_pkt;
//	uint8_t         *audio_pkt_data;
//	int             audio_pkt_size;
	int             audio_hw_buf_size;  
	double          audio_diff_cum; /* used for AV difference average computation */
	double          audio_diff_avg_coef;
	double          audio_diff_threshold;
	int             audio_diff_avg_count;
	double          frame_timer;
	double          frame_last_pts;
	double          frame_last_delay;
	double          video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame
	double          video_current_pts; ///<current displayed pts (different from video_clock if frame fifos are used)
	int64_t         video_current_pts_time;  ///<time (av_gettime) at which we updated video_current_pts - used to have running video pts
	AVStream        *video_st;
	PacketQueue     videoq;

	// picture queue
	VideoPicture    pictq[VIDEO_PICTURE_QUEUE_SIZE];
	int             pictq_size, pictq_rindex, pictq_windex;
	pthread_mutex_t *pictq_mutex;
	pthread_cond_t  *pictq_cond;
	pthread_t      	parse_tid;
	pthread_t      	video_tid;
	pthread_t      	audio_tid;
	pthread_attr_t 	thread_attr;
	char            filename[1024];
	
	// program control 
	int             quit;
	int             parser_done;
	double			av_diff_time;

	// SaveFrame
	int             numBytes;
	uint8_t         *buffer;
	AVCodecContext  *pVideoCodecCtx;
	AVFrame         *pFrameRGB;

} VideoState;

/* Since we only have one decoding thread, the Big Struct
   can be global in case we need it. */
VideoState *global_video_state;
static AVPacket flush_pkt;

static void packet_queue_flush(PacketQueue *q)
{
    AVPacketList *pkt, *pkt1;

    pthread_mutex_lock(&q->mutex);
    for(pkt = q->first_pkt; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    q->last_pkt = NULL;
    q->first_pkt = NULL;
    q->nb_packets = 0;
    q->size = 0;
    pthread_mutex_unlock(&q->mutex);
}

static void packet_queue_end(PacketQueue *q)
{
    packet_queue_flush(q);
    pthread_mutex_destroy(&q->mutex);
	pthread_cond_destroy(&q->cond);
}

static void packet_queue_abort(PacketQueue *q)
{
    pthread_mutex_lock(&q->mutex);

    q->abort_request = 1;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
    AVPacketList *pkt1;

    /* duplicate the packet */
    if (pkt!=&flush_pkt && av_dup_packet(pkt) < 0)
        return -1;

    pkt1 = av_malloc(sizeof(AVPacketList));
    if (!pkt1)
        return -1;
    pkt1->pkt = *pkt;
    pkt1->next = NULL;


    pthread_mutex_lock(&q->mutex);

    if (!q->last_pkt)

        q->first_pkt = pkt1;
    else
        q->last_pkt->next = pkt1;
    q->last_pkt = pkt1;
    q->nb_packets++;
	q->size += pkt1->pkt.size ;
#if 0
	av_log(NULL,AV_LOG_WARNING,"queue(%p): size=%d, packets=%d\n",q,q->size,q->nb_packets);
	av_log(NULL,AV_LOG_WARNING,"queue(%p): pts=%lld, dts=%lld\n",q,pkt1->pkt.pts,pkt1->pkt.dts);
#endif
    /* XXX: should duplicate packet data in DV case */
    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
    AVPacketList *pkt1;
    int ret;

    pthread_mutex_lock(&q->mutex);

    for(;;) {
        if (q->abort_request) {
            ret = -1;
            break;
        }

        pkt1 = q->first_pkt;
        if (pkt1) {
            q->first_pkt = pkt1->next;
            if (!q->first_pkt)
                q->last_pkt = NULL;
            q->nb_packets--;
            q->size -= pkt1->pkt.size ;
            *pkt = pkt1->pkt;
            av_free(pkt1);
            ret = 1;
            break;
        } else if (!block) {
            ret = 0;
            break;
        } else {
            pthread_cond_wait(&q->cond, &q->mutex);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return ret;
}

/* packet queue handling */
static void packet_queue_init(PacketQueue *q)
{
    memset(q, 0, sizeof(PacketQueue));
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond,NULL);
}

static double get_audio_clock(VideoState *is) {
	double pts;
	int hw_buf_size, bytes_per_sec, n;

	pts = is->audio_clock; /* maintained in the audio thread */
	hw_buf_size = is->audio_buf_size - is->audio_buf_index;

	// dsp hw_buf_size 
	if(hw_buf_size == 0)
		return pts;

	bytes_per_sec = 0;
	n = is->audio_st->codec->channels * 2;
	if(is->audio_st) {
		bytes_per_sec = is->audio_st->codec->sample_rate * n;
	}
	if(bytes_per_sec) {
		pts -= (double)hw_buf_size / bytes_per_sec;
	}
	return pts;
}

static double get_video_clock(VideoState *is) {
	double delta;

	delta = (av_gettime() - is->video_current_pts_time) / 1000000.0;
	return is->video_current_pts + delta;
}

static double get_external_clock(VideoState *is) {
	return av_gettime() / 1000000.0;
}

static double get_master_clock(VideoState *is) {
	if(is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
		return get_video_clock(is);
	} else if(is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
		return get_audio_clock(is);
	} else {
		return get_external_clock(is);
	}
}

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%06d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

void SaveToPPM(VideoState *is, AVFrame *pFrame, int iFrameNum)
{
	static struct SwsContext *img_convert_ctx;
	img_convert_ctx = sws_getCachedContext(img_convert_ctx,is->pVideoCodecCtx->width, 
										   is->pVideoCodecCtx->height, is->pVideoCodecCtx->pix_fmt, 
										   is->pVideoCodecCtx->width, is->pVideoCodecCtx->height, 
							    		   PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

	//Convert the image from its native format to RGB
	sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, 
			is->pVideoCodecCtx->height, is->pFrameRGB->data, is->pFrameRGB->linesize);

	SaveFrame(is->pFrameRGB, is->pVideoCodecCtx->width, is->pVideoCodecCtx->height, iFrameNum);
}

static void ShowDecoderTime(int64_t timestamp)
{
	
	int hours, mins, secs, us;
	secs = timestamp / AV_TIME_BASE;
	us = timestamp % AV_TIME_BASE;
	mins = secs / 60;
	secs %= 60;
	hours = mins / 60;
	mins %= 60;
	av_log(NULL, AV_LOG_INFO, "time: %02d:%02d:%02d.%02d\r", hours, mins, secs,(100 * us) / AV_TIME_BASE);
}

static void *AVPLAYER_Audio_Thread(void *arg)
{
	VideoState *is = (VideoState *)arg;
	int BSFreeSize; 
	AVPacket *pkt = &is->audio_pkt;

	while(!is->quit)
	{	
	
		// the end of audio decode thread
		if((is->parser_done == 1) && (is->audioq.size == 0))
		{
			is->quit = 1;
		}
	
		// Set DSP buf size which is the same as AudioBitrate
		BSFreeSize = DSPDEV_GetBSFreeSize(DSPfd); 	
		if(BSFreeSize > BSFeed640K )
		{	
			// block thread here to read pakcet
			if(packet_queue_get(&is->audioq, pkt, 1))
			{
				write(DSPfd, pkt->data, pkt->size);

				/* if update, update the audio clock w/pts */
				if(pkt->pts != AV_NOPTS_VALUE) 
				{
					is->audio_clock = av_q2d(is->audio_st->time_base)*pkt->pts;
				}
	
				// free audio packet
				av_free_packet(pkt);
			}
		}

		usleep(10000);
		//AVPLAYER_DBGMSG("Audio decode lopp \n");
	}
	
	pthread_exit(NULL);
}

// signal callback function
void StopMainProgram(int signo)
{
	global_video_state->quit = 1;
	DSPDEV_CleanAudioBuf(DSPfd);
	av_log(NULL, AV_LOG_WARNING,"capture SIGINT signal\n");
}

uint64_t global_video_pkt_pts = AV_NOPTS_VALUE;

/* These are called whenever we allocate a frame
 * buffer. We use this to store the global_pts in
 * a frame at the time it is allocated.
 */
int our_get_buffer(struct AVCodecContext *c, AVFrame *pic) 
{
	int ret = avcodec_default_get_buffer(c, pic);
	uint64_t *pts = av_malloc(sizeof(uint64_t));
	*pts = global_video_pkt_pts;
	pic->opaque = pts;
	return ret;
}
void our_release_buffer(struct AVCodecContext *c, AVFrame *pic) 
{
	if(pic) av_freep(&pic->opaque);
	avcodec_default_release_buffer(c, pic);
}

double synchronize_video(VideoState *is, AVFrame *src_frame, double pts) 
{

	double frame_delay;

	if(pts != 0) {
		/* if we have pts, set video clock to it */
		is->video_clock = pts;
	} else {
		/* if we aren't given a pts, set it to the clock */
		pts = is->video_clock;
	}
	/* update the video clock */
	frame_delay = av_q2d(is->video_st->codec->time_base);
	/* if we are repeating a frame, adjust clock accordingly */
	frame_delay += src_frame->repeat_pict * (frame_delay * 0.5);
	is->video_clock += frame_delay;
	return pts;
}


void *AVPLAYER_Video_Thread(void *arg) 
{
	VideoState *is = (VideoState *)arg;
	AVPacket pkt1, *packet = &pkt1;
	int len1, frameFinished;
	AVFrame *pFrame;
	double pts;
	int iFrameNum=0;	

	// alloce an avcodec frame
	pFrame = avcodec_alloc_frame();

	while(!is->quit)
	{
		if(packet_queue_get(&is->videoq, packet, 1) < 0) {
			// means we quit getting packets
			break;
		}
		if(packet->data == flush_pkt.data) {
			avcodec_flush_buffers(is->video_st->codec);
			continue;
		}

		if(g_opt_dropframe == 1)
		{
			if(is->av_diff_time > 1)
			{
				// check packet contain with key frame, 1: keyframe, 0: non-keyframe
				if((packet->flags & AV_PKT_FLAG_KEY) == 0)
				{
#if 0
					AVPLAYER_DBGMSG("keyframe=%d, size=%d, ", ((packet->flags & AV_PKT_FLAG_KEY) != 0), packet->size);
					if(packet->dts != AV_NOPTS_VALUE)
						AVPLAYER_DBGMSG("dts=%d, ", packet->pts);
					if(packet->pts != AV_NOPTS_VALUE)
						AVPLAYER_DBGMSG("pts=%d\n", packet->pts);
#endif			
					// drop frame
					AVPLAYER_DBGMSG("Drop video frame\n");
					av_free_packet(packet);
					continue;
				}
			}
		}
		pts = 0;

		// Save global pts to be stored in pFrame
		global_video_pkt_pts = packet->pts;
		// Decode video frame
		len1 = avcodec_decode_video2(is->video_st->codec, pFrame, &frameFinished, packet);

		//AVPLAYER_DBGMSG("video decode len: %d\n",len1);
		
		if(packet->dts == AV_NOPTS_VALUE 
				&& pFrame->opaque && *(uint64_t*)pFrame->opaque != AV_NOPTS_VALUE) {
			pts = *(uint64_t *)pFrame->opaque;
		} else if(packet->dts != AV_NOPTS_VALUE) {
			pts = packet->dts;
		} else {
			pts = 0;
		}
		pts *= av_q2d(is->video_st->time_base);


		// Did we get a video frame?
		if(frameFinished) {
			pts = synchronize_video(is, pFrame, pts);

			if(g_opt_saveframe == 1)
			{
				// save pFrame to PPM format
				SaveToPPM(is, pFrame, iFrameNum);
				iFrameNum++;
			}
		}
		av_free_packet(packet);
		//AVPLAYER_DBGMSG("video decode lopp \n");
	}
	av_free(pFrame);
	pthread_exit(NULL);
}

int AVPLAYER_Stream_Component_Open(VideoState *is, int stream_index) {

	AVFormatContext *pFormatCtx = is->pFormatCtx;
	AVCodecContext *codecCtx;
	AVCodec *codec;

	if(stream_index < 0 || stream_index >= pFormatCtx->nb_streams) {
		return -1;
	}

	// Get a pointer to the codec context for the video stream
	codecCtx = pFormatCtx->streams[stream_index]->codec;

	codec = avcodec_find_decoder(codecCtx->codec_id);

	switch(codecCtx->codec_type) {
		case AVMEDIA_TYPE_AUDIO:
			is->audioStream = stream_index;
			is->audio_st = pFormatCtx->streams[stream_index];
			is->audio_buf_size = 0;
			is->audio_buf_index = 0;

			/* averaging filter for audio sync */
			is->audio_diff_avg_coef = exp(log(0.01 / AUDIO_DIFF_AVG_NB));
			is->audio_diff_avg_count = 0;
			/* Correct audio only if larger error than this */
			is->audio_diff_threshold = 2.0 * SDL_AUDIO_BUFFER_SIZE / codecCtx->sample_rate;

			memset(&is->audio_pkt, 0, sizeof(is->audio_pkt));
			packet_queue_init(&is->audioq);
			pthread_create(&is->audio_tid, &is->thread_attr, AVPLAYER_Audio_Thread, is);
			
			// setting DSP codec type 
			if(!strcmp(codec->name,"mp3"))
			{
				if(DSPDEV_SetAudioWavefmt(DSPfd,WAVE_FORMAT_MPEGLAYER3))
					perror("DSPDEV_SetAudioWavefmt fail");
			}

			if(!strcmp(codec->name,"ac3"))
			{	
				if(DSPDEV_SetAudioWavefmt(DSPfd,WAVE_FORMAT_DOLBY_AC3))
					perror("DSPDEV_SetAudioWavefmt fail");
			}

			break;

		case AVMEDIA_TYPE_VIDEO:
			is->videoStream = stream_index;
			is->video_st = pFormatCtx->streams[stream_index];

			is->frame_timer = (double)av_gettime() / 1000000.0;
			is->frame_last_delay = 40e-3;
			is->video_current_pts_time = av_gettime();

			packet_queue_init(&is->videoq);
			pthread_create(&is->video_tid, &is->thread_attr, AVPLAYER_Video_Thread, is);
			codecCtx->get_buffer = our_get_buffer;
			codecCtx->release_buffer = our_release_buffer;
			
			// open software codec
			if(!codec || (avcodec_open(codecCtx, codec) < 0)) {
				fprintf(stderr, "Unsupported codec!\n");
				return -1;
			}

			// save codecCtx to VideoState
			is->pVideoCodecCtx = codecCtx;

			// Allocate an AVFrame structure
			is->pFrameRGB=avcodec_alloc_frame();
			
			// Determine required buffer size and allocate buffer
			is->numBytes=avpicture_get_size(PIX_FMT_RGB24, codecCtx->width, codecCtx->height);
			is->buffer=(uint8_t *)av_malloc(is->numBytes*sizeof(uint8_t));

			// Assign appropriate parts of buffer to image planes in pFrameRGB
			// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
			// of AVPicture
			avpicture_fill((AVPicture *)is->pFrameRGB, is->buffer, PIX_FMT_RGB24,
							codecCtx->width, codecCtx->height);


			break;

		default:
			break;
	}

	return 0;
}

int decode_interrupt_cb(void) {
	return (global_video_state && global_video_state->quit);
}

void *AVPLAYER_Parser_Thread(void *arg) {

	VideoState *is = (VideoState *)arg;
	AVFormatContext *pFormatCtx;
	AVPacket pkt1, *packet = &pkt1;
	int64_t timestamp;

	int video_index = -1;
	int audio_index = -1;
	int i;

	is->videoStream=-1;
	is->audioStream=-1;

	global_video_state = is;
	// will interrupt blocking functions if we quit!
    avio_set_interrupt_cb(decode_interrupt_cb);

	// Open video file
	if(avformat_open_input(&pFormatCtx, is->filename, NULL, NULL)!=0)
	{
		is->quit = 1;
		av_log(NULL, AV_LOG_WARNING,"Couldn't open file\n");
	}

	pFormatCtx->probesize = 64;
	pFormatCtx->flags |= AVFMT_FLAG_GENPTS;
	is->pFormatCtx = pFormatCtx;

	// Retrieve stream information
	if(av_find_stream_info(pFormatCtx)<0)
	{
		av_log(NULL, AV_LOG_WARNING,"Couldn't find stream information\n");
	}

	// Dump information about file onto standard error
	av_dump_format(pFormatCtx, 0, is->filename, 0);

	// Find the first video stream
	for(i=0; i<pFormatCtx->nb_streams; i++) {
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO &&
				video_index < 0) {
			video_index=i;
		}
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO &&
				audio_index < 0) {
			audio_index=i;
		}
	}

	// find audio data
	if(audio_index >= 0) {
		AVPLAYER_Stream_Component_Open(is, audio_index);
	}

	// find video data
	if(video_index >= 0) {
		AVPLAYER_Stream_Component_Open(is, video_index);
	}   

	if( is->videoStream < 0 && is->audioStream < 0) {
		fprintf(stderr, "%s: could not open codecs\n", is->filename);
		goto fail;
	}

	// main parser loop
	for(;;) {
		if(is->quit) {
			break;
		}
		// seek stuff goes here
		if(is->seek_req) {
			int stream_index= -1;
			int64_t seek_target = is->seek_pos;

			if     (is->videoStream >= 0) stream_index = is->videoStream;
			else if(is->audioStream >= 0) stream_index = is->audioStream;

			if(stream_index>=0){
				seek_target= av_rescale_q(seek_target, AV_TIME_BASE_Q, pFormatCtx->streams[stream_index]->time_base);
			}
            //avformat_seek_file(is->ic, -1, seek_min, seek_target, seek_max, is->seek_flags);
			//if(av_seek_frame(is->pFormatCtx, stream_index, seek_target, is->seek_flags) < 0) {
			if(avformat_seek_file(is->pFormatCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0) < 0) {
				//if (is->pFormatCtx->iformat->read_seek) {
				//	printf("format specific\n");
				//} else if(is->pFormatCtx->iformat->read_timestamp) {
				//	printf("frame_binary\n");
				//} else {
				//	printf("generic\n");
				//}
				fprintf(stderr, "%s: could not seek to position %0.3f\n", is->pFormatCtx->filename, (double)timestamp / AV_TIME_BASE);
				//fprintf(stderr, "%s: error while seeking. target: %d, stream_index: %d\n", is->pFormatCtx->filename, seek_target, stream_index);
			} else {
				if(is->audioStream >= 0) {
					packet_queue_flush(&is->audioq);
					packet_queue_put(&is->audioq, &flush_pkt);
				}
				if(is->videoStream >= 0) {
					packet_queue_flush(&is->videoq);
					packet_queue_put(&is->videoq, &flush_pkt);
				}
			}
			is->seek_req = 0;
		}
		if(is->audioq.size > MAX_AUDIOQ_SIZE ||
				is->videoq.size > MAX_VIDEOQ_SIZE) {
			usleep(10000);
			continue;
		}
		if(av_read_frame(is->pFormatCtx, packet) < 0) {
			if(pFormatCtx->pb->error) {
				usleep(100000); /* no error; wait for user input */
				AVPLAYER_DBGMSG("parser error \n");
				//is->quit = 1;
				continue;
			} else {
				AVPLAYER_DBGMSG("parser done\n");
				is->parser_done = 1;
				break;
			}
		}
		// Is this a packet from the video stream?
		if(packet->stream_index == is->videoStream) {
			packet_queue_put(&is->videoq, packet);
		} else if(packet->stream_index == is->audioStream) {
			packet_queue_put(&is->audioq, packet);
		} else {
			av_free_packet(packet);
		}
	}
	/* all done - wait for it */
	while(!is->quit) {
		usleep(100000);
	}

fail:
	{
#if 0		
		SDL_Event event;
		event.type = FF_QUIT_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);
#endif
		is->quit = 1;
	}
	return 0;
}

void AVPLAYER_Thread_Attr_Initial(VideoState *is)
{
	int ret, audio_max_priority, audio_min_priority;
	struct sched_param audio_scheduling_value;

	ret = pthread_attr_init(&is->thread_attr);
	if (ret != 0) {
		perror("Attribute creation failed");
		exit(EXIT_FAILURE);
	}

	ret = pthread_attr_setdetachstate(&is->thread_attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0) {
		perror("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}

	ret = pthread_attr_setschedpolicy(&is->thread_attr, SCHED_OTHER);
	if (ret != 0) {
		perror("Setting schedpolicy failed");
		exit(EXIT_FAILURE);
	}

	audio_max_priority = sched_get_priority_max(SCHED_OTHER);
	audio_min_priority = sched_get_priority_min(SCHED_OTHER);
	audio_scheduling_value.sched_priority = audio_min_priority;
	ret = pthread_attr_setschedparam(&is->thread_attr, &audio_scheduling_value);
	if (ret != 0) {
		perror("Setting schedpolicy failed");
		exit(EXIT_FAILURE);
	}
	AVPLAYER_DBGMSG("Scheduling priority set to %d, max allowed was %d\n", audio_min_priority, audio_max_priority);
	
}

int AVPLAYER_Thread_Attr_Destroy(VideoState *is)
{
	return pthread_attr_destroy(&is->thread_attr);
}

int AVPLAYER_DSP_Initial()
{
	DSPfd = DSPDEV_OPEN();
	if(DSPfd < 0)
		return(-EBADF);

	// clean DSP decoder buf data
	return DSPDEV_CleanAudioBuf(DSPfd);
}

int AVPLAYER_DSP_Destroy()
{
	// close DSP fd
	return DSPDEV_CLOSE(DSPfd);;
}

void stream_seek(VideoState *is, int64_t pos, int rel) {

	if(!is->seek_req) {
		is->seek_pos = pos;
		is->seek_flags = rel < 0 ? AVSEEK_FLAG_BACKWARD : 0;
		is->seek_req = 1;
	}
}

int main (int argc, char **argv)
{
	int ret = 0;
	VideoState      *is;

	if(argc < 2) {
		printf("Please provide a movie file\n");
		exit(EXIT_FAILURE);
	}

	is = av_mallocz(sizeof(VideoState));
	memset(is,0,sizeof(VideoState));

    /* register all codecs, demux and protocols */
    avcodec_register_all();
	AVPLAYER_DBGMSG("avcodec_register_all done\n");

	// Register all formats and codecs
	av_register_all();
	AVPLAYER_DBGMSG("av_register_all done\n");

	// set pthread attr
	AVPLAYER_Thread_Attr_Initial(is);

	// open DSP device
	ret = AVPLAYER_DSP_Initial();
	if(ret < 0)
	{
		perror("DSP open fail");
		exit(ret);
	}

#ifdef FUNCDEBUG
	//function trace log
	AVPLAYER_DBGMSG("Logfile: %s\n", cygprofile_getfilename ());
    cygprofile_enable ();
#endif
#ifdef MEMDEBUG
	AVPLAYER_DBGMSG("set MALLOC_TRACE environment variable\n");
	setenv("MALLOC_TRACE", "mtrace_check", 1);
	mtrace();
#endif
#ifdef DEBUG
	// ffmpeg log level to DEBUG
	av_log_set_level(AV_LOG_DEBUG);
#endif

	av_strlcpy(is->filename, argv[1], sizeof(is->filename));

	//is->pictq_mutex = SDL_CreateMutex();
	//is->pictq_cond = SDL_CreateCond();
	//schedule_refresh(is, 40);

	is->av_sync_type = DEFAULT_AV_SYNC_TYPE;
	pthread_create(&is->parse_tid, &is->thread_attr, AVPLAYER_Parser_Thread, is);
	AVPLAYER_DBGMSG("AVPLAYER_Parser_Thread done\n");

	if(!is->parse_tid) {
		av_free(is);
		return -1;
	}

	// initial flush_pkt
	av_init_packet(&flush_pkt);
	flush_pkt.data= "FLUSH";

	// register signal 
	signal(SIGINT,StopMainProgram);

	while(!is->quit)
	{
		usleep(100000);
		//AVPLAYER_DBGMSG("main lopp \n");

		//av_log(NULL,AV_LOG_WARNING,"video queue: size=%d, packets=%d\n",is->videoq.size,is->videoq.nb_packets);
		//av_log(NULL,AV_LOG_WARNING,"audio queue: size=%d, packets=%d\n",is->audioq.size,is->audioq.nb_packets);
		is->av_diff_time = is->audio_clock - is->video_clock;
		av_log(NULL,AV_LOG_WARNING,"A:%f , V:%f , A-V: %f, master: %f\r",is->audio_clock, is->video_clock,is->av_diff_time,get_master_clock(is));
		
	}

	// Free the RGB image
	av_free(is->buffer);
	av_free(is->pFrameRGB);

	// free queue buffer
	packet_queue_abort(&is->audioq);
	packet_queue_abort(&is->videoq);
	packet_queue_end(&is->audioq);
	packet_queue_end(&is->videoq);

	AVPLAYER_Thread_Attr_Destroy(is);
	AVPLAYER_DSP_Destroy();

	return 0;
}


