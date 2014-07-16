#include <types.h>
#include <platform/regmap.h>

#if (IC_VER == QAE377)
#define SSPL_FREQ_MASK			0xFC231FFF
static int gSysBusFreqFPGA = -1;

static unsigned int gSysBusFreq[][2] = {
#ifdef ENABLE_SPREAD_SPECTRUM //yukai modified for mantis 0114325
	#ifdef SPREAD_SPECTRUM_9_MILLESIMAL
	{ 423000000, 0x006818BC },
	{ 414000000, 0x006818B8 },
	{ 405000000, 0x006818B4 },
	{ 400500000, 0x006818B2 },
	{ 396000000, 0x006818B0 },
	{ 387000000, 0x006818AC },
	{ 378000000, 0x006818A8 },
	{ 369000000, 0x006818A4 },
	{ 360000000, 0x006818A0 },
	{ 351000000, 0x0068189C },
	{ 342000000, 0x00681898 },
	{ 333000000, 0x00681894 },
	{ 324000000, 0x00681890 },
	{ 301500000, 0x00681886 },
	{ 299250000, 0x00681885 },
	{ 297000000, 0x00681884 },
	{ 288000000, 0x00681880 },
	{ 279000000, 0x0068187C },
	{ 270000000, 0x00681878 },
	{ 261000000, 0x00681874 },
	{ 252000000, 0x00681870 },
	{ 198000000, 0x006918B0 },
	{ 124875000, 0x0069186F },
	{ 0, 0 }
	#endif

	#ifdef SPREAD_SPECTRUM_18_MILLESIMAL
	{ 423000000, 0x006838BC },
	{ 414000000, 0x006838B8 },
	{ 405000000, 0x006838B4 },
	{ 400500000, 0x006838B2 },
	{ 396000000, 0x006838B0 },
	{ 387000000, 0x006838AC },
	{ 378000000, 0x006838A8 },
	{ 369000000, 0x006838A4 },
	{ 360000000, 0x006838A0 },
	{ 351000000, 0x0068389C },
	{ 342000000, 0x00683898 },
	{ 333000000, 0x00683894 },
	{ 324000000, 0x00683890 },
	{ 301500000, 0x00683886 },
	{ 299250000, 0x00683885 },
	{ 297000000, 0x00683884 },
	{ 288000000, 0x00683880 },
	{ 279000000, 0x0068387C },
	{ 270000000, 0x00683878 },
	{ 261000000, 0x00683874 },
	{ 252000000, 0x00683870 },
	{ 198000000, 0x006938B0 },
	{ 124875000, 0x0069386F },
	{ 0, 0 }
	#endif

	#ifdef SPREAD_SPECTRUM_36_MILLESIMAL
	{ 423000000, 0x006858BC },
	{ 414000000, 0x006858B8 },
	{ 405000000, 0x006858B4 },
	{ 400500000, 0x006858B2 },
	{ 396000000, 0x006858B0 },
	{ 387000000, 0x006858AC },
	{ 378000000, 0x006858A8 },
	{ 369000000, 0x006858A4 },
	{ 360000000, 0x006858A0 },
	{ 351000000, 0x0068589C },
	{ 342000000, 0x00685898 },
	{ 333000000, 0x00685894 },
	{ 324000000, 0x00685890 },
	{ 301500000, 0x00683886 },
	{ 299250000, 0x00685885 },
	{ 297000000, 0x00685884 },
	{ 288000000, 0x00685880 },
	{ 279000000, 0x0068587C },
	{ 270000000, 0x00685878 },
	{ 261000000, 0x00685874 },
	{ 252000000, 0x00685870 },
	{ 198000000, 0x006958B0 },
	{ 124875000, 0x0069586F },
	{ 0, 0 }
	#endif
#else
	{ 423000000, 0x02E058BC },
	{ 414000000, 0x02E058B8 },
	{ 405000000, 0x02E058B4 },
	{ 400500000, 0x02E058B2 },
	{ 396000000, 0x02E058B0 },
	{ 387000000, 0x02E058AC },
	{ 378000000, 0x02E058A8 },
	{ 369000000, 0x02E058A4 },
	{ 360000000, 0x02E058A0 },
	{ 351000000, 0x02E0589C },
	{ 342000000, 0x02E05898 },
	{ 333000000, 0x02E05894 },
	{ 324000000, 0x02E05890 },
	{ 301500000, 0x02E05886 },
//	{ 301500000, 0x00683886 },	// spread spectrum 1.8%
	{ 299250000, 0x02E05885 },
	{ 297000000, 0x02E05884 },
	{ 288000000, 0x02E05880 },
	{ 279000000, 0x02E0587C },
	{ 270000000, 0x02E05878 },
	{ 261000000, 0x02E05874 },
	{ 252000000, 0x02E05870 },
	{ 198000000, 0x02E158B0 },
	{ 124875000, 0x02E1586F },
	{ 0, 0 }
#endif
};
#elif (IC_VER == QCE491)
#elif (IC_VER == QCE483)
#else
#error "Please define correct IC_VER!"
#endif

#if 0
int GL_SetSysBusFreq(int hz)
{
	int i = 0;

	if (hz < 50000000) {
		/* FPGA case */
		gSysBusFreqFPGA = hz;
		return 0;
	} else {
		/* ASIC case */
		while (gSysBusFreq[i][0] != 0) {
			if (gSysBusFreq[i][0] == (unsigned int)hz) {
#ifdef USE_ROMTER
				HWREG_1(spi_control, 0x07);			// SPI clock = CLK_SPI / 32
#else
				HWREG_S(spi_control, ~0x07, 0x04);	// SPI clock = CLK_SPI / 8
#endif
				HWREG_W(sspll_cfg, gSysBusFreq[i][1]);
				return 0;
			}
			++i;
		}
		return -1;
	}
}
#endif
int GL_GetSysBusFreq(void)
{
#if (IC_VER == QAE377)
	int i = 0;
	unsigned int cfg;

	cfg = HWREG_R(pllsys_cfg);

	/* To see of it is in ASIC case */
	while (gSysBusFreq[i][0] != 0) {
		if (gSysBusFreq[i][1] == (unsigned int)cfg) {
			return gSysBusFreq[i][0];
		}
		++i;
	}

	/* FPGA case */
	return gSysBusFreqFPGA;
#elif (IC_VER == QCE491) || (IC_VER == QCE483)
	unsigned int NS;
	const unsigned int double_freq_step = 27;	// freq_step = 13.5

	NS = HWREG_R(pllsys_cfg) & 0x3F;

	/*
	 * The conventional equation of system clock frequency is:
	 *    SysClk = FreqStep * (NS + 1) / 2
	 *
	 * Due to integer limitation, here we use:
	 *        SysClk = FreqStep * (NS + 1) / 2
	 *               = DoubleFreqStep * (NS + 1) / 4
	 * 1000000: MHz
	 */
	return double_freq_step * (NS + 1) * (1000000 / 4);
#else
#error "No IC_VER definition"
#endif
}

int GL_GetMemFreq(void)
{
#if (IC_VER == QAE377)
	int i = 0;
	unsigned int cfg;

	cfg = HWREG_R(pllsys_cfg);

	/* To see of it is in ASIC case */
	while (gSysBusFreq[i][0] != 0) {
		if (gSysBusFreq[i][1] == (unsigned int)cfg) {
			return gSysBusFreq[i][0];
		}
		++i;
	}

	/* FPGA case */
	return gSysBusFreqFPGA;
#elif (IC_VER == QCE491) || (IC_VER == QCE483)

	unsigned int sspll;
	unsigned int NS;
	unsigned int freq_step;

	sspll = HWREG_R(sft_cfg_26);
	if (sspll & (1 << 19)) {
		// spread spectrum mode
		NS = sspll & 0xFF;
		freq_step = 4;
	} else {
		// normal mode
		NS = sspll & 0x3F;
		freq_step = 18;
	}

	if ((3 <= NS) && (NS < 66)) {
		NS += 2;
	}

	/*
	 * The equation of system clock frequency is:
	 *    DoubleDramClk = FreqStep * (Real_NS / 2) * 2
	 *                  = FreqStep * Real_NS
	 * 1000000: MHz
	 */
	return freq_step * NS * 1000000;
#else
#error "No IC_VER definition"
#endif
}

#if 0
int GL_SetCpuFreqMult(int m)
{
	int prev = HWREG_R(clk_sel[1]) & 0x03;

	switch (m) {
	case 1:
		HWREG_1(clk_sel[1], 0x03);
		break;
	case 2:
		HWREG_0(clk_sel[1], ~(0x03));
		break;
	default:
		return -1;
	}

	return prev;
}
#endif
int GL_GetCpuFreqMult(void)
{
	int m = HWREG_R(clk_sel[1]) & 0x03;

	if (m == 0) {
		return 2;
	} else {
		return 1;
	}
}

/* vim: set ts=4 sw=4 tw=0 noet ft=cpp.doxygen: */
