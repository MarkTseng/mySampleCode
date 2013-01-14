#ifndef __FILE_DEF_H__
#define __FILE_DEF_H__

// file_type
#define	FILE_TYPE_UNKNOWN		0
#define	FILE_TYPE_AVI			1
#define	FILE_TYPE_ASF			2
#define	FILE_TYPE_QT			3
#define	FILE_TYPE_ND			4
#define FILE_TYPE_MPG           5
#define	FILE_TYPE_WAVE			6
#define	FILE_TYPE_CDXA			7
#define FILE_TYPE_OGG           8
#define FILE_TYPE_MP3 9 //#ifdef SUPPORT_MP3_FILE_PARSER
#define FILE_TYPE_AAC 10 //#ifdef SUPPORT_AAC_FILE_PARSER
#define FILE_TYPE_MKV			11
#define FILE_TYPE_RMF			12
#define FILE_TYPE_TS			13
#define FILE_TYPE_TSPL_PSA		14
#define FILE_TYPE_TSPL_PSV		15
#define FILE_TYPE_TSPL_TSA		16
#define FILE_TYPE_TSPL_TSV		17
#define FILE_TYPE_FLV			18
#define FILE_TYPE_SWF			19
#define FILE_TYPE_AC3			20
#define FILE_TYPE_FLAC			21
#define 	FILE_TYPE_JPEG				22
#define 	FILE_TYPE_BMP				23
#define 	FILE_TYPE_GIF				24
#define 	FILE_TYPE_TIFF				25
#define 	FILE_TYPE_PNG				26
#define 	FILE_TYPE_BD				27
#define 	FILE_TYPE_AVCHD			28
#define 	FILE_TYPE_DVD_VIDEO		29
#define 	FILE_TYPE_DVD_PLUS_VR		30
#define 	FILE_TYPE_DVD_MINUS_VR	31
#define   FILE_TYPE_SVCD			32
#define 	FILE_TYPE_VCD20			33
#define 	FILE_TYPE_VCD11			34
#define 	FILE_TYPE_CDDA			35
#define	FILE_TYPE_DTS			36
#define   FILE_TYPE_MAX                 37      //this is use to judge the number of file types.

// file_media_support
#define	FILE_MODE_ALL			0xFFFF
#define	FILE_SUPPORT_AUDIO		(1<<0)
#define	FILE_SUPPORT_VIDEO		(1<<1)
#define	FILE_SUPPORT_SUPIC		(1<<2)
#define	FILE_SUPPORT_IDX		(1<<3)
#define	FILE_SUPPORT_SUBT		(1<<4)

// file_media_flag
#define FILE_STRM_FLAG_NONE		0
#define FILE_AUDIO				(1<<0)
#define FILE_VIDEO				(1<<1)
#define FILE_SUPIC				(1<<2)

#define FILE_SUPPORTED_AUDIO	(1<<4)
#define FILE_SUPPORTED_VIDEO	(1<<5)
#define FILE_SUPPORTED_SUPIC	(1<<6)

#define FILE_AUDIO_VBR			(1<<8)
#define FILE_AV_INTERLEAVED		(1<<9)

// file_play_state
#define	FILE_NORMAL_STATE		0
#define	FILE_DROPA_STATE		(1<<0)
#define	FILE_DROPV_STATE		(1<<1)
#define	FILE_DROPS_STATE		(1<<2)

// file_error_flag

#endif	// __FILE_DEF_H__
