#include "types.h"

#include "gsl.h"


#if (IC_VER == QAE377)
#if defined(IC_SUPPORT_DRAM_A_32)
    #define WORD_PER_T 2
#else
	#define WORD_PER_T 1
#endif
static int idx_array[] = {
	0, 	// BW_CB2MB
	1, 	// BW_L2C
	2, 	// BW_PERI_BG
	3, 	// BW_CB_DMA
	4, 	// BW_IMG_DEC
	5, 	// BW_ATA_DEV_MAC
	6, 	// BW_MINI_USB
	7, 	// BW_MFD_MR_LUMA
	8, 	// BW_MFD_LF_LUMA
	9, 	// BW_EHCI_USB
	10, // BW_G2D
	11, // BW_VPP
	12, // BW_VPP2
	13, // BW_MFD_MR_CHROMA
	14, // BW_MFD_LF_CHROMA
	15, // BW_PG
	16, // BW_OSD
	17, // BW_AFRC_W
	18, // BW_AFRC_R_TTX
	19, // BW_MFD_MPRG
	20, // BW_MFD_DMAG
	21, // BW_MFD_MC2SD_LUMA
	22, // BW_MFD_MC2SD_CHROMA
	-1, /* Reserved */
	23, // BW_BD_FE
	24, // BW_GPU
	25, // BW_FONT
	26, // BW_DUMMY
};

static bwValue bwLuma[] = {
	{BW_CB2MB,				BW_NORMAL,		  0,	0},
	{BW_L2C,				BW_NORMAL,		  0,	0},
	{BW_PERI_BG,			BW_NORMAL,		  0,	0},
	{BW_CB_DMA,				BW_NORMAL,		  0,	0},
	{BW_IMG_DEC,			BW_NORMAL,		  0,	0},
	{BW_ATA_DEV_MAC,		BW_NORMAL,		  0,	0},
	{BW_MINI_USB,			BW_NORMAL,		  0,	0},
	{BW_MFD_MR_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_LUMA,		BW_NORMAL,		  0,	0},
	{BW_EHCI_USB,			BW_NORMAL,		  0,	0},
	{BW_G2D,				BW_NORMAL,		  0,	0},
	{BW_VPP,				BW_NORMAL,		  0,	0},
	{BW_VPP2,				BW_NORMAL,		  0,	0},
	{BW_MFD_MR_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_PG,					BW_NORMAL,		  0,	0},
	{BW_OSD,				BW_NORMAL,		  0,	0},
	{BW_AFRC_W,				BW_NORMAL,		  0,	0},
	{BW_AFRC_R_TTX,			BW_NORMAL,		  0,	0},
	{BW_MFD_MPRG,			BW_NORMAL,		  0,	0},
	{BW_MFD_DMAG,			BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_CHROMA,	BW_NORMAL,		  0,	0},
	{BW_BD_FE,				BW_NORMAL,		  0,	0},
	{BW_GPU,				BW_NORMAL,		  0,	0},
	{BW_FONT,				BW_NORMAL,		  0,	0},
	{BW_DUMMY,				BW_NORMAL,		  0,	0}
};

static bwValue bwChroma[] = {
	{BW_CB2MB,				BW_NORMAL,		  0,	0},
	{BW_L2C,				BW_NORMAL,		  0,	0},
	{BW_PERI_BG,			BW_NORMAL,		  0,	0},
	{BW_CB_DMA,				BW_NORMAL,		  0,	0},
	{BW_IMG_DEC,			BW_NORMAL,		  0,	0},
	{BW_ATA_DEV_MAC,		BW_NORMAL,		  0,	0},
	{BW_MINI_USB,			BW_NORMAL,		  0,	0},
	{BW_MFD_MR_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_LUMA,		BW_NORMAL,		  0,	0},
	{BW_EHCI_USB,			BW_NORMAL,		  0,	0},
	{BW_G2D,				BW_NORMAL,		  0,	0},
	{BW_VPP,				BW_NORMAL,		  0,	0},
	{BW_VPP2,				BW_NORMAL,		  0,	0},
	{BW_MFD_MR_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_PG,					BW_NORMAL,		  0,	0},
	{BW_OSD,				BW_NORMAL,		  0,	0},
	{BW_AFRC_W,				BW_NORMAL,		  0,	0},
	{BW_AFRC_R_TTX,			BW_NORMAL,		  0,	0},
	{BW_MFD_MPRG,			BW_NORMAL,		  0,	0},
	{BW_MFD_DMAG,			BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_CHROMA,	BW_NORMAL,		  0,	0},
	{BW_BD_FE,				BW_NORMAL,		  0,	0},
	{BW_GPU,				BW_NORMAL,		  0,	0},
	{BW_FONT,				BW_NORMAL,		  0,	0},
	{BW_DUMMY,				BW_NORMAL,		  0,	0}
};

int GL_BwASetBw(bwMasterList module, int bw, bwPriority prio, int flag)
{
	int MaxBw;
	const int MinBw10x = 1000 / 500 / WORD_PER_T;

	if ((module < BW_CB2MB) ||
		(module > BW_DUMMY)) {
		return -1;
	}

	if ((prio != BW_LOW_LATENCY) &&
		(prio != BW_REAL_TIME) &&
		(prio != BW_NORMAL)) {
		return -1;
	}

	if ((flag != 0) &&
		(flag != BW_FLAG_ABOVE_NORMAL) &&
		(flag != BW_FLAG_NO_URGENT)) {
		return -1;
	}

	// 2: DDR2
	// 1000: millesimal for bw10x usage
	// WORD_PER_T * 16: n bits for each T
	// 8: unit is byte
	MaxBw = ((GL_GetMemFreq() * 2) / 1000) * ((WORD_PER_T * 16) / 8);

	bwLuma[idx_array[module]].priority = prio;
	bwLuma[idx_array[module]].flags = flag;
	bwLuma[idx_array[module]].bw10x = (bw * 1000) / MaxBw;
	if ((bw > 0) && (bwLuma[idx_array[module]].bw10x < MinBw10x)) {
		diag_printf("%s: bw of master %d is too small!\n", __FUNCTION__, idx_array[module]);
		bwLuma[idx_array[module]].bw10x = (MinBw10x == 0) ? (1) : (MinBw10x);
	}

	return 0;
}

int GL_BwAGetBw(bwMasterList module, int *bw, bwPriority *prio, int *flag)
{
	*prio = bwLuma[idx_array[module]].priority;
	*bw = bwLuma[idx_array[module]].bw10x;
	*flag = bwLuma[idx_array[module]].flags;

	return 0;
}

int GL_BwBSetBw(bwMasterList module, int bw, bwPriority prio, int flag)
{
	int MaxBw;
	const int MinBw10x = 1000 / 500 / 1;

	if ((module < BW_CB2MB) ||
		(module > BW_DUMMY)) {
		return -1;
	}

	if ((prio != BW_LOW_LATENCY) &&
		(prio != BW_REAL_TIME) &&
		(prio != BW_NORMAL)) {
		return -1;
	}

	if ((flag != 0) &&
		(flag != BW_FLAG_ABOVE_NORMAL) &&
		(flag != BW_FLAG_NO_URGENT)) {
		return -1;
	}

	// Reserved on DRAM B
	if ((module == BW_MFD_MR_LUMA) ||
		(module == BW_MFD_LF_LUMA) ||
		(module == BW_MFD_MC2SD_LUMA)) {
		return -1;
	}

	// 2: DDR2
	// 1000: millesimal for bw10x usage
	// 1 * 16: 16 bits for each T
	// 8: unit is byte
	MaxBw = ((GL_GetSysBusFreq() * 2) / 1000) * ((1 * 16) / 8);

	bwChroma[idx_array[module]].priority = prio;
	bwChroma[idx_array[module]].flags = flag;
	bwChroma[idx_array[module]].bw10x = (bw * 1000) / MaxBw;
	if ((bw > 0) && (bwChroma[idx_array[module]].bw10x < MinBw10x)) {
		diag_printf("%s: bw of master %d is too small!\n", __FUNCTION__, idx_array[module]);
		bwChroma[idx_array[module]].bw10x = (MinBw10x == 0) ? (1) : (MinBw10x);
	}

	return 0;
}

int GL_BwBGetBw(bwMasterList module, int *bw, bwPriority *prio, int *flag)
{
	*prio = bwChroma[idx_array[module]].priority;
	*bw = bwChroma[idx_array[module]].bw10x;
	*flag = bwChroma[idx_array[module]].flags;

	return 0;
}

int GL_BwAClearSetting(void)
{
	int i;

	for (i = 0; i < (sizeof(bwLuma) / sizeof(bwLuma[0])); i++) {
		bwLuma[i].priority = BW_NORMAL;
		bwLuma[i].bw10x = 0;
		bwLuma[i].flags = 0;
	}

	return 0;
}

int GL_BwBClearSetting(void)
{
	int i;

	for (i = 0; i < (sizeof(bwChroma) / sizeof(bwChroma[0])); i++) {
		bwChroma[i].priority = BW_NORMAL;
		bwChroma[i].bw10x = 0;
		bwChroma[i].flags = 0;
	}

	return 0;
}

int GL_BwAGetSetting(int *low_latency, int *real_time, int *normal)
{
	int i;

	for (i = 0; i < (sizeof(bwLuma) / sizeof(bwLuma[0])); i++) {
		switch (bwLuma[i].priority) {
			case BW_LOW_LATENCY:
				*low_latency += bwLuma[i].bw10x;
				break;

			case BW_REAL_TIME:
				*real_time += bwLuma[i].bw10x;
				break;

			case BW_NORMAL:
				*normal += bwLuma[i].bw10x;
				break;

			default:
				return -1;
				break;
		}
	}

	return 0;
}

int GL_BwBGetSetting(int *low_latency, int *real_time, int *normal)
{
	int i;

	for (i = 0; i < (sizeof(bwChroma) / sizeof(bwChroma[0])); i++) {
		switch (bwChroma[i].priority) {
			case BW_LOW_LATENCY:
				*low_latency += bwChroma[i].bw10x;
				break;

			case BW_REAL_TIME:
				*real_time += bwChroma[i].bw10x;
				break;

			case BW_NORMAL:
				*normal += bwChroma[i].bw10x;
				break;

			default:
				return -1;
				break;
		}
	}

	return 0;
}

int GL_BwActivate(void)
{
	int totalBwLowLatency, totalBwRealTime, totalBwNormal;

	// error check for ram A
	totalBwLowLatency = 0;
	totalBwRealTime = 0;
	totalBwNormal = 0;
	GL_BwAGetSetting(&totalBwLowLatency, &totalBwRealTime, &totalBwNormal);
	if ((totalBwLowLatency + totalBwRealTime + totalBwNormal) > 1000) {
		diag_printf("Total BW value exceeds max. bw value for ram A\n");
	}

	// error check for ram B
	totalBwLowLatency = 0;
	totalBwRealTime = 0;
	totalBwNormal = 0;
	GL_BwBGetSetting(&totalBwLowLatency, &totalBwRealTime, &totalBwNormal);
	if ((totalBwLowLatency + totalBwRealTime + totalBwNormal) > 1000) {
		diag_printf("Total BW value exceeds max. bw value for ram B\n");
	}

	if (GL_bwASet(sizeof(bwLuma) / sizeof(bwLuma[0]), bwLuma) == -1) {
		return -1;
	}
#ifndef	DRAM_BUS_SINGLE
	if (GL_bwBSet(sizeof(bwChroma) / sizeof(bwChroma[0]), bwChroma) == -1) {
		return -1;
	}
#endif

	return 0;
}

void GL_BwDump(void)
{
	int i;

	// Select page 0
	HWREG_0(mbar0_page_setting, 0xFFFC);

	diag_printf("Page0:\n");
	diag_printf("mbar0_page_setting = 0x%08X\n", HWREG_R(mbar0_page_setting));
	for (i = 0; i < 28; ++i) {
		diag_printf("mbar0_m_setting[%2d] = 0x%08X\n", i, HWREG_R(mbar0_m_setting[i]));
	}
	diag_printf("\n");

	// Select page 2
	HWREG_1(mbar0_page_setting, 2);
	diag_printf("Page2:\n");
	diag_printf("mbar0_page_setting = 0x%08X\n", HWREG_R(mbar0_page_setting));
	for (i = 0; i < 28; ++i) {
		diag_printf("mbar0_m_setting[%2d] = 0x%08X\n", i, HWREG_R(mbar0_m_setting[i]));
	}
	diag_printf("\n");

	// Select page 1
	HWREG_S(mbar0_page_setting, 0xFFFC, 1);
	diag_printf("Page1:\n");
	diag_printf("mbar0_page_setting = 0x%08X\n", HWREG_R(mbar0_page_setting));
	for (i = 0; i < 28; ++i) {
		diag_printf("mbar0_m_setting[%2d] = 0x%08X\n", i, HWREG_R(mbar0_m_setting[i]));
	}
	diag_printf("\n");
}
#elif (IC_VER == QCE491) || (IC_VER == QCE483) || (IC_VER == QAF536)

#if (IC_VER == QCE491)||(IC_VER == QAF536)
#if defined(IC_SUPPORT_DRAM_A_32)
    #define WORD_PER_T 2
#else
	#define WORD_PER_T 1
#endif
#endif

#if (IC_VER == QCE483)
	#define WORD_PER_T 1
#endif
static bwValue bwLuma[] = {
	{BW_CB2MB,				BW_NORMAL,		  0,	0},
	{BW_L2C,				BW_NORMAL,		  0,	0},
	{BW_PERI_BG,			BW_NORMAL,		  0,	0},
	{BW_CB_DMA,				BW_NORMAL,		  0,	0},
	{BW_IMG_DEC,			BW_NORMAL,		  0,	0},
	{BW_ATA_DEV_MAC,		BW_NORMAL,		  0,	0},
	{BW_MINI_USB,			BW_NORMAL,		  0,	0},
	{BW_MFD_MR_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_LUMA,		BW_NORMAL,		  0,	0},
	{BW_EHCI_USB,			BW_NORMAL,		  0,	0},
	{BW_G2D,				BW_NORMAL,		  0,	0},
	{BW_VPP,				BW_NORMAL,		  0,	0},
	{BW_VPP2,				BW_NORMAL,		  0,	0},
	{BW_MFD_MR_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_PG,					BW_NORMAL,		  0,	0},
	{BW_OSD,				BW_NORMAL,		  0,	0},
	{BW_AFRC_W,				BW_NORMAL,		  0,	0},
	{BW_AFRC_R_TTX,			BW_NORMAL,		  0,	0},
	{BW_MFD_MPRG,			BW_NORMAL,		  0,	0},
	{BW_MFD_DMAG,			BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_CHROMA,	BW_NORMAL,		  0,	0},
	{BW_BD_FE,				BW_NORMAL,		  0,	0},
	{BW_GPU,				BW_NORMAL,		  0,	0},
	{BW_FONT,				BW_NORMAL,		  0,	0},
	{BW_DSP_D,				BW_NORMAL,		  0,	0},
	{BW_DUMMY,				BW_NORMAL,		  0,	0}
};

static bwValue bwChroma[] = {
	{BW_CB2MB,				BW_NORMAL,		  0,	0},
	{BW_L2C,				BW_NORMAL,		  0,	0},
	{BW_PERI_BG,			BW_NORMAL,		  0,	0},
	{BW_CB_DMA,				BW_NORMAL,		  0,	0},
	{BW_IMG_DEC,			BW_NORMAL,		  0,	0},
	{BW_ATA_DEV_MAC,		BW_NORMAL,		  0,	0},
	{BW_MINI_USB,			BW_NORMAL,		  0,	0},
	{BW_MFD_MR_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_LUMA,		BW_NORMAL,		  0,	0},
	{BW_EHCI_USB,			BW_NORMAL,		  0,	0},
	{BW_G2D,				BW_NORMAL,		  0,	0},
	{BW_VPP,				BW_NORMAL,		  0,	0},
	{BW_VPP2,				BW_NORMAL,		  0,	0},
	{BW_MFD_MR_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_LF_CHROMA,		BW_NORMAL,		  0,	0},
	{BW_PG,					BW_NORMAL,		  0,	0},
	{BW_OSD,				BW_NORMAL,		  0,	0},
	{BW_AFRC_W,				BW_NORMAL,		  0,	0},
	{BW_AFRC_R_TTX,			BW_NORMAL,		  0,	0},
	{BW_MFD_MPRG,			BW_NORMAL,		  0,	0},
	{BW_MFD_DMAG,			BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_LUMA,		BW_NORMAL,		  0,	0},
	{BW_MFD_MC2SD_CHROMA,	BW_NORMAL,		  0,	0},
	{BW_BD_FE,				BW_NORMAL,		  0,	0},
	{BW_GPU,				BW_NORMAL,		  0,	0},
	{BW_FONT,				BW_NORMAL,		  0,	0},
	{BW_DSP_D,				BW_NORMAL,		  0,	0},
	{BW_DUMMY,				BW_NORMAL,		  0,	0}
};

static int find_index(const bwValue bwv[], int size, int module)
{
	int i;

	for (i = 0; i < size; ++i) {
		if (bwv[i].module == module) {
			return i;
		}
	}
	return -1;
}

int GL_BwASetBw(bwMasterList module, int bw, bwPriority prio, int flag)
{
	int MaxBw;
	int index;
	const int MinBw10x = 1000 / 500 / WORD_PER_T;

	if ((module < BW_CB2MB) ||
		(module > BW_DUMMY)) {
		return -1;
	}

	if ((prio != BW_LOW_LATENCY) &&
		(prio != BW_REAL_TIME) &&
		(prio != BW_NORMAL)) {
		return -1;
	}

	if ((flag != 0) &&
		(flag != BW_FLAG_ABOVE_NORMAL) &&
		(flag != BW_FLAG_NO_URGENT)) {
		return -1;
	}

	/*
	 * ClockFactor is ignored because the result is the same as that generated
	 * by the original equation.
	 *
	 * ArbitratorPeriod = AP
	 * ClockFactor = CF = SDRAM_CLK / SYSBUS_CLK
	 * WORD_PER_T = WPT
	 * X: a factor to compute bw10x
	 *
	 * final_bw = bw10x                  * AP * WPT * CF                       / 1000
	 *          = (bw / MaxBw)           * AP * WPT * CF                       / 1000
	 *          = (bw / (SDRAM_CLK * X)) * AP * WPT * (SDRAM_CLK / SYSBUS_CLK) / 1000
	 *          <Reduce SDRAM_CLK and move SYSBUS_CLK forward>
	 *          = (bw / (SYSBUS_CLK * X) * AP * WPT / 1000)
	 */

	/*
	 * Note that the real equation of MaxBw is:
	 *   Real_MaxBw = ((GL_GetMemFreq() * 2) / 1000) * ((WORD_PER_T * 16) / 8);
	 *
	 * 2: DDR2
	 * 1000: millesimal for bw10x usage
	 * WORD_PER_T * 16: n bits for each T
	 * 8: unit is byte
	 */
	MaxBw = ((GL_GetMemFreq() * 2) / 1000) * ((WORD_PER_T * 16) / 8);

	index = find_index(bwLuma, sizeof(bwLuma) / sizeof(bwLuma[0]), module);
	bwLuma[index].priority = prio;
	bwLuma[index].flags = flag;
	bwLuma[index].bw10x = (bw * 1000) / MaxBw;
	if ((bw > 0) && (bwLuma[index].bw10x < MinBw10x)) {
		diag_printf("%s: bw of master %d is too small (%d)!\n", __FUNCTION__, module, bw);
		bwLuma[index].bw10x = (MinBw10x == 0) ? (1) : (MinBw10x);
	}

	return 0;
}

int GL_BwAGetBw(bwMasterList module, int *bw, bwPriority *prio, int *flag)
{
	int index;

	index = find_index(bwLuma, sizeof(bwLuma) / sizeof(bwLuma[0]), module);
	*prio = bwLuma[index].priority;
	*bw = bwLuma[index].bw10x;
	*flag = bwLuma[index].flags;

	return 0;
}

int GL_BwBSetBw(bwMasterList module, int bw, bwPriority prio, int flag)
{
	int MaxBw;
	int index;
	const int MinBw10x = 1000 / 500 / 1;

	if ((module < BW_CB2MB) ||
		(module > BW_DUMMY)) {
		return -1;
	}

	if ((prio != BW_LOW_LATENCY) &&
		(prio != BW_REAL_TIME) &&
		(prio != BW_NORMAL)) {
		return -1;
	}

	if ((flag != 0) &&
		(flag != BW_FLAG_ABOVE_NORMAL) &&
		(flag != BW_FLAG_NO_URGENT)) {
		return -1;
	}

	// Reserved on DRAM B
	if ((module == BW_MFD_MR_LUMA) ||
		(module == BW_MFD_LF_LUMA) ||
		(module == BW_MFD_MC2SD_LUMA)) {
		return -1;
	}

	// 2: DDR2
	// 1000: millesimal for bw10x usage
	// 1 * 16: 16 bits for each T
	// 8: unit is byte
	MaxBw = ((GL_GetSysBusFreq() * 2) / 1000) * ((1 * 16) / 8);

	index = find_index(bwChroma, sizeof(bwChroma) / sizeof(bwChroma[0]), module);
	bwChroma[index].priority = prio;
	bwChroma[index].flags = flag;
	bwChroma[index].bw10x = (bw * 1000) / MaxBw;
	if ((bw > 0) && (bwChroma[index].bw10x < MinBw10x)) {
		diag_printf("%s: bw of master %d is too small (%d)!\n", __FUNCTION__, module, bw);
		bwChroma[index].bw10x = (MinBw10x == 0) ? (1) : (MinBw10x);
	}

	return 0;
}

int GL_BwBGetBw(bwMasterList module, int *bw, bwPriority *prio, int *flag)
{
	int index;

	index = find_index(bwChroma, sizeof(bwChroma) / sizeof(bwChroma[0]), module);
	*prio = bwChroma[index].priority;
	*bw = bwChroma[index].bw10x;
	*flag = bwChroma[index].flags;

	return 0;
}

int GL_BwAClearSetting(void)
{
	int i;

	for (i = 0; i < (sizeof(bwLuma) / sizeof(bwLuma[0])); i++) {
		bwLuma[i].priority = BW_NORMAL;
		bwLuma[i].bw10x = 0;
		bwLuma[i].flags = 0;
	}

	return 0;
}

int GL_BwBClearSetting(void)
{
	int i;

	for (i = 0; i < (sizeof(bwChroma) / sizeof(bwChroma[0])); i++) {
		bwChroma[i].priority = BW_NORMAL;
		bwChroma[i].bw10x = 0;
		bwChroma[i].flags = 0;
	}

	return 0;
}

int GL_BwAGetSetting(int *low_latency, int *real_time, int *normal)
{
	int i;

	for (i = 0; i < (sizeof(bwLuma) / sizeof(bwLuma[0])); i++) {
		switch (bwLuma[i].priority) {
			case BW_LOW_LATENCY:
				*low_latency += bwLuma[i].bw10x;
				break;

			case BW_REAL_TIME:
				*real_time += bwLuma[i].bw10x;
				break;

			case BW_NORMAL:
				*normal += bwLuma[i].bw10x;
				break;

			default:
				return -1;
				break;
		}
	}

	return 0;
}

int GL_BwBGetSetting(int *low_latency, int *real_time, int *normal)
{
	int i;

	for (i = 0; i < (sizeof(bwChroma) / sizeof(bwChroma[0])); i++) {
		switch (bwChroma[i].priority) {
			case BW_LOW_LATENCY:
				*low_latency += bwChroma[i].bw10x;
				break;

			case BW_REAL_TIME:
				*real_time += bwChroma[i].bw10x;
				break;

			case BW_NORMAL:
				*normal += bwChroma[i].bw10x;
				break;

			default:
				return -1;
				break;
		}
	}

	return 0;
}

int GL_BwActivate(void)
{
	int totalBwLowLatency, totalBwRealTime, totalBwNormal;

	// error check for ram A
	totalBwLowLatency = 0;
	totalBwRealTime = 0;
	totalBwNormal = 0;
	GL_BwAGetSetting(&totalBwLowLatency, &totalBwRealTime, &totalBwNormal);
	if ((totalBwLowLatency + totalBwRealTime + totalBwNormal) > 1000) {
		diag_printf("Total BW value exceeds max. bw value for ram A\n");
	}
	HWREG_W(mbar0_page_setting, 3);
	HWREG_W(mbar0_m_setting[2], 500);
#ifndef	DRAM_BUS_SINGLE
	// error check for ram B
	totalBwLowLatency = 0;
	totalBwRealTime = 0;
	totalBwNormal = 0;
	GL_BwBGetSetting(&totalBwLowLatency, &totalBwRealTime, &totalBwNormal);
	if ((totalBwLowLatency + totalBwRealTime + totalBwNormal) > 1000) {
		diag_printf("Total BW value exceeds max. bw value for ram B\n");
	}
	HWREG_W(mbar1_page_setting, 3);
	HWREG_W(mbar1_m_setting[2], 500);
#endif

	if (GL_bwASet(sizeof(bwLuma) / sizeof(bwLuma[0]), bwLuma) == -1) {
		return -1;
	}
#ifndef	DRAM_BUS_SINGLE
	if (GL_bwBSet(sizeof(bwChroma) / sizeof(bwChroma[0]), bwChroma) == -1) {
		return -1;
	}
#endif

	return 0;
}

void GL_BwDumpMbar0(void)
{
	int i;

	// Select page 0
	HWREG_0(mbar0_page_setting, 0xFFFC);

	diag_printf("Page0:\n");
	diag_printf("mbar0_page_setting(0x%08x) = 0x%08X\n", (UINT32)HWREG_ADDR(mbar0_page_setting), HWREG_R(mbar0_page_setting));
	for (i = 1; i < 29; ++i) {
		diag_printf("mbar0_m_setting[%2d](0x%08x) = 0x%08X\n", i, (UINT32)HWREG_ADDR(mbar0_m_setting[i]), HWREG_R(mbar0_m_setting[i]));
	}
	diag_printf("\n");

	// Select page 2
	HWREG_1(mbar0_page_setting, 2);
	diag_printf("Page2:\n");
	diag_printf("mbar0_page_setting(0x%08x) = 0x%08X\n", (UINT32)HWREG_ADDR(mbar0_page_setting), HWREG_R(mbar0_page_setting));
	for (i = 1; i < 29; ++i) {
		diag_printf("mbar0_m_setting[%2d](0x%08x) = 0x%08X\n", i, (UINT32)HWREG_ADDR(mbar0_m_setting[i]), HWREG_R(mbar0_m_setting[i]));
	}
	diag_printf("\n");

	// Select page 1
	HWREG_S(mbar0_page_setting, 0xFFFC, 1);
	diag_printf("Page1:\n");
	diag_printf("mbar0_page_setting(0x%08x) = 0x%08X\n", (UINT32)HWREG_ADDR(mbar0_page_setting), HWREG_R(mbar0_page_setting));
	for (i = 1; i < 29; ++i) {
		diag_printf("mbar0_m_setting[%2d](0x%08x) = 0x%08X\n", i, (UINT32)HWREG_ADDR(mbar0_m_setting[i]), HWREG_R(mbar0_m_setting[i]));
	}
	diag_printf("\n");
}
#ifndef	DRAM_BUS_SINGLE
void GL_BwDumpMbar1(void)
{
	int i;

	// Select page 0
	HWREG_0(mbar1_page_setting, 0xFFFC);

	diag_printf("Page0:\n");
	diag_printf("mbar1_page_setting(0x%08x) = 0x%08X\n", (UINT32)HWREG_ADDR(mbar1_page_setting), HWREG_R(mbar1_page_setting));
	for (i = 1; i < 29; ++i) {
		diag_printf("mbar1_m_setting[%2d](0x%08x) = 0x%08X\n", i, (UINT32)HWREG_ADDR(mbar1_m_setting[i]), HWREG_R(mbar1_m_setting[i]));
	}
	diag_printf("\n");

	// Select page 2
	HWREG_1(mbar1_page_setting, 2);
	diag_printf("Page2:\n");
	diag_printf("mbar1_page_setting(0x%08x) = 0x%08X\n", (UINT32)HWREG_ADDR(mbar1_page_setting), HWREG_R(mbar1_page_setting));
	for (i = 1; i < 29; ++i) {
		diag_printf("mbar1_m_setting[%2d](0x%08x) = 0x%08X\n", i, (UINT32)HWREG_ADDR(mbar1_m_setting[i]), HWREG_R(mbar1_m_setting[i]));
	}
	diag_printf("\n");

	// Select page 1
	HWREG_S(mbar1_page_setting, 0xFFFC, 1);
	diag_printf("Page1:\n");
	diag_printf("mbar1_page_setting(0x%08x) = 0x%08X\n", (UINT32)HWREG_ADDR(mbar1_page_setting), HWREG_R(mbar1_page_setting));
	for (i = 0; i < 28; ++i) {
		diag_printf("mbar1_m_setting[%2d](0x%08x) = 0x%08X\n", i, (UINT32)HWREG_ADDR(mbar1_m_setting[i]), HWREG_R(mbar1_m_setting[i]));
	}
	diag_printf("\n");
}
#endif

#else
#error "Please define correct IC_VER!"
#endif
