/*! \addtogroup GSL
 * @{
 */
/******************************************************************************/
/*!
*
* @file		gl_modctrl.h
*
* @brief	This file specifies the GSL Module Control APIs
*
* @note		Copyright (c) 2010 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
* @author
*
******************************************************************************/
#ifndef __GSL_MODCTRL_H__
#define __GSL_MODCTRL_H__

/*!
 * @defgroup GL_Modctrl GSL Module Control APIs
 * @brief Specify the attributes of hardware modules such as bandwidth and priority.
 * @{
 */

/*!
 * @enum
 * Module List
 */
typedef enum {
	DPLL			= 31,
	DEMUX2			= 30,
	DEMUX			= 29,
	SDIO			= 28,
	NAND			= 27,
	CARD			= 26,
	CPS				= 25,
	HOST			= 24,
	APP				= 23,
	AUD				= 22,
	PERI			= 21,
	PCMCIA			= 20,
	OTP_I2C			= 19,
	UART1			= 18,
	//UART0			= 17,
	STC				= 16,
	SDCTRL1			= 15,
	MBUS1			= 14,
	SDCTRL0			= 13,
	MBUS0			= 12,
	// reserved		= 11,
	SPI				= 10,
	CB_DMA			=  9,
	RGST			=  8,
	BOOTROM			=  7,
	CBSW			=  6,
	// reserved		=  5,
	IOP				=  4,
	DSP				=  3,
	RISC1			=  2,
	RISC0			=  1,
	SYSTEM			=  0,

	// reserved		= 31 + 32,
	// reserved		= 30 + 32,
	LFRV			= 29 + 32,
	MPRV			= 28 + 32,
	LF264			= 27 + 32,
	MR264			= 26 + 32,
	QT264			= 25 + 32,
	MP264			= 24 + 32,
	LFVC1			= 23 + 32,
	MPVC1			= 22 + 32,
	MR9421			= 21 + 32,
	QT9421			= 20 + 32,
	VLD_AGDC		= 19 + 32,
	PICPARM			= 18 + 32,
	RG				= 17 + 32,
	DMX				= 16 + 32,
	MC_HD2SD		= 15 + 32,
	// reserved		= 14 + 32,
	PNG				= 13 + 32,
	JR				= 12 + 32,
	JPG_IQT			= 11 + 32,
	HUFF			= 10 + 32,
	GPU				=  9 + 32,
	FONT			=  8 + 32,
	// reserved		=  7 + 32,
	// reserved		=  6 + 32,
	M2S				=  5 + 32,
	SRV				=  4 + 32,
	// reserved		=  3 + 32,
	MAC				=  2 + 32,
	USB_SHCI		=  1 + 32,
	USB_EHCI		=  0 + 32,

	// reserved		= 31 + 64,
	// reserved		= 30 + 64,
	// reserved		= 29 + 64,
	// reserved		= 28 + 64,
	// reserved		= 27 + 64,
	// reserved		= 26 + 64,
	// reserved		= 25 + 64,
	// reserved		= 24 + 64,
	// reserved		= 23 + 64,
	// reserved		= 22 + 64,
	// reserved		= 21 + 64,
	// reserved		= 20 + 64,
	// reserved		= 19 + 64,
	// reserved		= 18 + 64,
	// reserved		= 17 + 64,
	DUMMY_MASTER	= 16 + 64,
	CAS				= 15 + 64,
	V656_IN			= 14 + 64,
	HDMI_TX			= 13 + 64,
	TTX_P2S			= 12 + 64,
	TV_DVD2			= 11 + 64,
	AFRC			= 10 + 64,
	HD2SD			=  9 + 64,
	DMIX			=  8 + 64,
	DSCL			=  7 + 64,
	AMIX			=  6 + 64,
	OSD				=  5 + 64,
	G2D				=  4 + 64,
	PG				=  3 + 64,
	VPP2			=  2 + 64,
	VPP				=  1 + 64,
	TGEN			=  0 + 64
} moduleList;

/*!
 * @enum
 * Master ID (MBUS port ID)
 */
typedef enum {
	BW_CB2MB			= 0,
	BW_L2C				= 1,
	BW_PERI_BG			= 2,
	BW_CB_DMA			= 3,
	BW_IMG_DEC			= 4,
	BW_ATA_DEV_MAC		= 5,
	BW_MINI_USB			= 6,
	BW_MFD_MR_LUMA		= 7,
	BW_MFD_LF_LUMA		= 8,
	BW_EHCI_USB			= 9,
	BW_G2D				= 10,
	BW_VPP				= 11,
	BW_VPP2				= 12,
	BW_MFD_MR_CHROMA	= 13,
	BW_MFD_LF_CHROMA	= 14,
	BW_PG				= 15,
	BW_OSD				= 16,
	BW_AFRC_W			= 17,
	BW_AFRC_R_TTX		= 18,
	BW_MFD_MPRG			= 19,
	BW_MFD_DMAG			= 20,
	BW_MFD_MC2SD_LUMA	= 21,
	BW_MFD_MC2SD_CHROMA	= 22,
	// Reserved
	BW_BD_FE			= 24,
	BW_GPU				= 25,
	BW_FONT				= 26,
	BW_DUMMY			= 27
} bwMasterList;

/*!
 * @enum
 * @brief the priority of masters in MBUS
 */
typedef enum {
	BW_LOW_LATENCY,
	BW_REAL_TIME,
	BW_NORMAL
} bwPriority;
//#if (PRODUCT_CAT==DTV)
//#define BW_SCLR					BW_AFRC_W
//#define BW_HIS					BW_AFRC_R_TTX
//#endif
//#define BW_FLAG_ABOVE_NORMAL	(1 << 7)
//#define BW_FLAG_NO_URGENT		(1 << 5)

/*!
 * @typedef
 * @brief The type used to describe bandwidth allocation of a master
 */
typedef struct {
	bwMasterList module;	/**< Master id */
	bwPriority priority;	/**< Priority */
	int bw10x;				/**< Bandwidth percentage * 10 */
	int flags;				/**< Flags */
} bwValue;

/*!
 * @brief Release reset state of a module
 * @param m module index
 * @return original reset status
 */
int GL_moduleResetOff(moduleList m);

/*!
 * @brief Enable clock of a module
 * @param m module index
 * @return original clock status
 */
int GL_moduleClockOn(moduleList m);

#endif
