
#ifndef __WAVE_FMT_H
#define __WAVE_FMT_H

// wFormatTag definitions in mmreg.h
#define	WAVE_FORMAT_UNKNOWN			0x0000
#define	WAVE_FORMAT_PCM				0x0001
#define	WAVE_FORMAT_ADPCM			0x0002
#define	WAVE_FORMAT_IEEE_FLOAT		0x0003
#define	WAVE_FORMAT_ADPCM_U_LAW		0x0007 
#define	WAVE_FORMAT_G723_ADPCM		0x0014
#define	WAVE_FORMAT_G721_ADPCM		0x0040
#define	WAVE_FORMAT_G728_CELP		0x0041
#define WAVE_FORMAT_G726_ADPCM		0x0064
#define	WAVE_FORMAT_DOLBY_AC2		0x0030
#define	WAVE_FORMAT_DOLBY_AC3		0x2000
#define	WAVE_FORMAT_DOLBY_DTS		0x2001
#define	WAVE_FORMAT_MPEG			0x0050
#define	WAVE_FORMAT_MPEGLAYER3		0x0055
#define	WAVE_FORMAT_IMA_ADPCM		0x0011

#define WAVE_FORMAT_WMA_OLD			0x0160		// WMA version type 1, not supported any more ->ycchou
#define WAVE_FORMAT_WMA_STEREO		0x0161		// could be supported, version 7, 8, 9
#define WAVE_FORMAT_WMA_PRO			0x0162
#define WAVE_FORMAT_WMA_LOSSLESS	0x0163
#define WAVE_FORMAT_WMA_VOICE		0x000A

#define	WAVE_FORMAT_NEC_AAC			0x00B0		// NEC AAC
#define	WAVE_FORMAT_MPEG2AAC		0x0180		// MP2 AAC
#define	WAVE_FORMAT_DIVIO_AAC		0x4143		// DIVIO AAC
#define	WAVE_FORMAT_AAC				0xa106		// MP4 AAC

//hcsu:added, not list on mmreg.h so far
#define WAVE_FORMAT_DTSHD				0x3000
#define WAVE_FORMAT_DTSHD_MASTER_AUDIO		0x3001
#define WAVE_FORMAT_DTSLBR              0x3008
#define WAVE_FORMAT_DOLBY_DIGITAL_PLUS  0x3010
#define WAVE_FORMAT_TRUEHD				0x3020
#define WAVE_FORMAT_OGG_VORBIS			0x3030
#define WAVE_FORMAT_REAL_AUDIO			0x3040
#define WAVE_FORMAT_NOISE_GEN           0x3050
#define WAVE_FORMAT_FLAC                0x3060
#define WAVE_FORMAT_APE                 0x3070
#define WAVE_FORMAT_AVS                 0x3080

// ccyang add
#define WAVE_FORMAT_CD_LPCM		0x5001
#define WAVE_FORMAT_DVD_LPCM	0x5002
#define WAVE_FORMAT_BD_LPCM		0x5003
#define WAVE_FORMAT_COOK		0x5004
#define WAVE_FORMAT_ULaw_LPCM	0x5005
#define WAVE_FORMAT_BigEndian_LPCM	0x5006

// ccyang add
#define	WAVE_FORMAT_Encode_MPEG 0x6001
#define	WAVE_FORMAT_Encode_AC3  0x6002
#define	WAVE_FORMAT_Encode_DTS  0x6003

// proprietary wma header
typedef	struct _wmaheader
{
	
	UINT16	wFormatTag;
	UINT32	dwSampRate;
	UINT32	dwBytePerSec;
	UINT16	wBlockAlign;
	UINT16	wReserve1;		// nChannels+nBitsPerSample
	UINT32	dwSampPerBlk;
	UINT16	wEncOption;
	UINT16	wTag;

} __attribute__ ((packed)) WMAHEADER;

typedef struct tWAVEFORMATEX
{
    UINT16	wFormatTag       ;//__attribute__ ((packed));   /* format type */
    UINT16	nChannels        ;//__attribute__ ((packed));   /* number of channels (i.e. mono, stereo...) */
    UINT32	nSamplesPerSec   ;//__attribute__ ((packed));   /* sample rate */
    UINT32	nAvgBytesPerSec  ;//__attribute__ ((packed));   /* for buffer estimation */
    UINT16	nBlockAlign     ; //__attribute__ ((packed));   /* block size of data */
    UINT16	wBitsPerSample   ;//__attribute__ ((packed));   /* Number of bits per sample of mono data */
    UINT16	cbSize           ;//__attribute__ ((packed));   /* The count in bytes of the size of extra information (after cbSize) */
} __attribute__ ((packed)) WAVEFORMATEX;

typedef struct mpeg1waveformat_tag {
    WAVEFORMATEX    wfx				;//__attribute__ ((packed));
    UINT16	fwHeadLayer		;//__attribute__ ((packed));
    UINT32	dwHeadBitrate	;//__attribute__ ((packed));
    UINT16	fwHeadMode		;//__attribute__ ((packed));
    UINT16	fwHeadModeExt	;//__attribute__ ((packed));
    UINT16	wHeadEmphasis	;//__attribute__ ((packed));
    UINT16	fwHeadFlags		;//__attribute__ ((packed));
    UINT32	dwPTSLow		;//__attribute__ ((packed));
    UINT32	dwPTSHigh		;//__attribute__ ((packed));
} __attribute__ ((packed)) MPEG1WAVEFORMAT;

typedef struct mpeglayer3waveformat_tag {
    WAVEFORMATEX  wfx			;//__attribute__ ((packed));
    UINT16	wID					;//__attribute__ ((packed));
    UINT32	fdwFlags			;//__attribute__ ((packed));
    UINT16	nBlockSize			;//__attribute__ ((packed));
    UINT16	nFramesPerBlock		;//__attribute__ ((packed));
    UINT16	nCodecDelay			;//__attribute__ ((packed));
} __attribute__ ((packed)) MPEGLAYER3WAVEFORMAT;

//hcsu:: for wma codec parameters
typedef struct tWAVEFORMATEX2
{
    UINT16	wFormatTag    ;   
    UINT16	nChannels       ; 
    UINT32	nSamplesPerSec   ;
    UINT32	nAvgBytesPerSec  ;
    UINT16	nBlockAlign      ;
    UINT16	wBitsPerSample   ;
    UINT16	cbSize           ;
    UINT16	dwSamplesPerBlockL; // alignment not match, can't get UINT32
    UINT16	dwSamplesPerBlockH;
    UINT16  wEncodeOptions;
    UINT16  wReserved1;
    UINT16  wReserved2;
} __attribute__ ((packed)) WAVEFORMATEX2;


/*
typedef struct tWAVEFORMATEXPRO
{
    UINT16	wFormatTag    ;   
    UINT16	nChannels       ; 
    UINT32	nSamplesPerSec   ;
    UINT32	nAvgBytesPerSec  ;
    UINT16	nBlockAlign      ;
    UINT16	wBitsPerSample   ;
    UINT16	cbSize ;
	UINT16	wReserved1;
    UINT32  dwChannelMask;
	UINT32	wReserved2[2];
    UINT16  wEncodeOptions;
    UINT16  wReserved3[8];
}__attribute__ ((packed)) WAVEFORMATEXPRO;
*/

/***********************************
*			WMA
************************************/
typedef struct tWMAUDIO2WAVEFORMAT
{
    WAVEFORMATEX wfx;
    UINT32      dwSamplesPerBlock;
    UINT16      wEncodeOptions;
    UINT32      dwSuperBlockAlign;
}   __attribute__ ((packed)) WMAUDIO2WAVEFORMAT;

/***********************************
*			WMA Pro
************************************/
typedef struct tWMAPROWAVEFORMAT
{
    WAVEFORMATEX wfx;
    UINT16      cbsize;
    UINT16      nChannelMask;
    UINT8       reserve0[10];
    UINT16      nEncodeOpt;
}   __attribute__ ((packed)) WMAPROWAVEFORMAT;

/***********************************
*			AAC
************************************/
typedef struct tAACWAVEFORMAT
{
    WAVEFORMATEX wfx;
    UINT16      BSType;
}   __attribute__ ((packed)) AACWAVEFORMAT;

/***********************************
*			RA
************************************/
typedef struct tRAwaveformat_tag 
{
	WAVEFORMATEX  wfx;
	UINT16   nFrameBits;
	UINT16   nSamples;	
	UINT16   nRegions;
	UINT16   cplStart;		
	UINT16   cplQbits;		
	UINT16   MaxByteRate;
	UINT16   AvgByteRate;
} __attribute__ ((packed)) RAWAVEFORMAT;

/***********************************
*			ADPCM
************************************/
typedef struct tPCMwaveformat_tag 
{
	WAVEFORMATEX  wfx;
	UINT16	nSamplesPerBlock;
} __attribute__ ((packed)) PCMWAVEFORMAT;

/***********************************
*	BD LPCM
************************************/
typedef struct tBDLPCMWAVEFORMAT 
{
	WAVEFORMATEX  wfx;
	UINT16	channel_cfg;
} __attribute__ ((packed)) BDLPCMWAVEFORMAT;

/***********************************
*			MPEG Encode
************************************/
typedef struct tEncodeMPEGWAVEFORMAT
{
    WAVEFORMATEX wfx;
	UINT16       input_mode;
    UINT16       BitRate;
	UINT16       encode_chn_num;
	UINT16       MPEG_layer;
	UINT16       recoding_mode;
}   __attribute__ ((packed)) EncodeMPEGWAVEFORMAT;

typedef struct tWAVFORMATMACRO
{
    WAVEFORMATEX    wfx;
    UINT8           reserve[30];
}   __attribute__ ((packed)) WAVFORMATMACRO;

#endif      // __WAVE_FMT_H

