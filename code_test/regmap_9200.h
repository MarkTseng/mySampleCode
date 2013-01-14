#ifndef __REGMAP_9200_H__
#define __REGMAP_9200_H__

typedef struct
{
	#include "regmap_9200.inc"
} RegisterFile;
register unsigned int *__rf asm("$22");
#define HWREG_W(reg, value)	\
	do {					\
		(*(volatile unsigned int *)(__rf - 256 * 32 + __builtin_offsetof(RegisterFile, reg) / 4)) = (value);	\
	} while (0)
#define HWREG_R(reg) (*(volatile unsigned int *)(__rf - 256 * 32 + __builtin_offsetof(RegisterFile, reg) / 4))

/* Get the address of a HWREG.  */
#define HWREG_ADDR(reg)	(__rf - 256 * 32 + __builtin_offsetof(RegisterFile, reg) / 4)

/* Set HWREG bits.  */
#define HWREG_S(reg, mask, pattern);	\
	do {								\
		unsigned int x;					\
		x = HWREG_R(reg);				\
		x = x & (mask);					\
		x = x | (pattern);				\
		HWREG_W(reg, x);				\
	} while (0)

#define HWREG_1(reg, pattern)	\
	do {						\
		unsigned int x;			\
		x = HWREG_R(reg);		\
		x = x | (pattern);		\
		HWREG_W(reg, x);		\
	} while (0)

#define HWREG_0(reg, pattern)	\
	do {						\
		unsigned int x;			\
		x = HWREG_R(reg);		\
		x = x & (pattern);		\
		HWREG_W(reg, x);		\
	} while (0)


/*
** 	Video
*/
#define RF_CODING_EXT0_PROGRESSIVE_FRAME		(1<<0)
#define RF_CODING_EXT0_CHROMA_420_TYPE			(1<<1)
#define RF_CODING_EXT0_REPEAT_FIRST_FIELD		(1<<2)
#define RF_CODING_EXT0_ALTERNATE_SCAN			(1<<3)
#define RF_CODING_EXT0_INTRA_VLC_FORMAT			(1<<4)
#define RF_CODING_EXT0_Q_SCALE_TYPE				(1<<5)
#define RF_CODING_EXT0_CONCEAL_MOTION_VECTORS	(1<<6)
#define RF_CODING_EXT0_FRAME_PRED_FRAME_DCT		(1<<7)
#define RF_CODING_EXT0_TOP_FIELD_FIRST			(1<<8)
#define RF_CODING_EXT0_PICTURE_STRUCTURE		(0x03<<9)
#define RF_CODING_EXT0_INTRA_DC_PRECISION		(0x03<<11)
//#define RF_CODING_EXT0_LAST_PICTURE			(1<<14)
#define RF_CODING_EXT0_BACK_LAST				(1<<14)
#define RF_CODING_EXT0_SECOND_FIELD				(1<<15)

#define	ext0_pic_struct(x)						(((x)>>9)&0x03)

/*
** 	Display Status
*/
#define RF_Display_OSDRegion	0x00ff

#define	RF_Display_FieldNo		0x1000
#if (PRODUCT_CAT != DTV)
#define DISPLAY_STATUS			(HWREG_R(dtg_status_1))
#define	IsTopField()			((DISPLAY_STATUS & RF_Display_FieldNo)==0)
#define	IsBottomField()			((DISPLAY_STATUS & RF_Display_FieldNo))
#else
#define	IsTopField()			(DISPLAY_STATUS & 0x01)
#define	IsBottomField()			((DISPLAY_STATUS+1) & 0x01)
#endif
#define	RF_Video_VPicEnd		0x0001
#define	RF_Video_VTblErr		0x0002
#define	RF_Video_VRunErr		0x0004
#define	RF_Video_VSliceErr		0x0008
#define	RF_Video_VErr			0x8000


#define IsRPicStart				(HWREG_R(mpeg_pic_start))
#define	VLD_STATUS				(HWREG_R(vld_status))
#define	IsVPicEnd				(VLD_STATUS & RF_Video_VPicEnd)
#define	IsVRunErr				(VLD_STATUS & RF_Video_VRunErr)
#define	IsVTblErr				(VLD_STATUS & RF_Video_VTblErr)
#define	IsVErr					(VLD_STATUS & RF_Video_VErr)

#define	RF_CODING_EXT1_FORWARD_REF0	(0<<1)
#define	RF_CODING_EXT1_FORWARD_REF1	(1<<1)

#define	RF_CODING_EXT1_RECONST_REF0	(0<<2)
#define	RF_CODING_EXT1_RECONST_REF1	(1<<2)
#define	RF_CODING_EXT1_RECONST_B	(2<<2)

#define	RF_CODING_EXT1_FIELDID		(1<<4)

#define RF_Video_MPEG2_flag			0x08
#define RF_Video_MPEG1_flag			0x04


#endif /*__REGMAP_9200_H__*/
