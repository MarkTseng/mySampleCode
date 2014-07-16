#include <stdarg.h>
#include <gsl.h>
#include <sys/ioctl.h>
#include <gsl/lambo86_hw.h>
#include <gsl/lambo86_hwop.h>

//#define GSL_MODCTRL_TEST

#ifdef GL_DEBUG_MODCTRL
#define glmod_printf(fmt, arg...)	gsl_printf(fmt, ##arg)
#else
#define glmod_printf(fmt, arg...)
#endif

int GL_moduleResetOn(moduleList m)
{
	if (ioctl(lambo_hw_info.fd, LAMBO86_IOC_MODCTRL_RSTON, &m)) {
		return -1;
	}
	return m;
}

int GL_moduleResetOff(moduleList m)
{
	if (ioctl(lambo_hw_info.fd, LAMBO86_IOC_MODCTRL_RSTOFF, &m)) {
		return -1;
	}
	return m;
}

int GL_moduleClockOn(moduleList m)
{
	if (ioctl(lambo_hw_info.fd, LAMBO86_IOC_MODCTRL_CLKON, &m)) {
		return -1;
	}
	return m;
}

int GL_moduleClockOff(moduleList m)
{
	if (ioctl(lambo_hw_info.fd, LAMBO86_IOC_MODCTRL_CLKOFF, &m)) {
		return -1;
	}
	return m;
}

int GL_moduleGatedClockOn(moduleList m)
{
	if (ioctl(lambo_hw_info.fd, LAMBO86_IOC_MODCTRL_GCLKON, &m)) {
		return -1;
	}
	return m;
}

int GL_moduleGatedClockOff(moduleList m)
{
	if (ioctl(lambo_hw_info.fd, LAMBO86_IOC_MODCTRL_GCLKOFF, &m)) {
		return -1;
	}
	return m;
}

static inline void _gl_module_control_ex(int command, unsigned int *value)
{
	if (ioctl(lambo_hw_info.fd, command, value)) {
		glmod_printf("Error (%d)\n", errno);
	}
}

static inline void gl_module_control_ex(int n, int command, va_list ap)
{
	int i;
	moduleList t;
	unsigned int value[4] = { 0 };

	for (i = 0; i < n; i++) {
		t = va_arg(ap, moduleList);
		value[t / 32] |= 1 << (t & 0x1F);
	}
	_gl_module_control_ex(command, value);
}

/* Extended version */
void GL_moduleResetOnEx(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	gl_module_control_ex(n, LAMBO86_IOC_MODCTRL_RSTON_EX, ap);
	va_end(ap);
}

void GL_moduleResetOffEx(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	gl_module_control_ex(n, LAMBO86_IOC_MODCTRL_RSTOFF_EX, ap);
	va_end(ap);
}

void GL_moduleClockOnEx(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	gl_module_control_ex(n, LAMBO86_IOC_MODCTRL_CLKON_EX, ap);
	va_end(ap);
}

void GL_moduleClockOffEx(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	gl_module_control_ex(n, LAMBO86_IOC_MODCTRL_CLKOFF_EX, ap);
	va_end(ap);
}

void GL_moduleGatedClockOnEx(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	gl_module_control_ex(n, LAMBO86_IOC_MODCTRL_GCLKON_EX, ap);
	va_end(ap);
}

void GL_moduleGatedClockOffEx(int n, ...)
{
	va_list ap;
	va_start(ap, n);
	gl_module_control_ex(n, LAMBO86_IOC_MODCTRL_GCLKOFF_EX, ap);
	va_end(ap);
}

#ifdef GSL_MODCTRL_TEST
#define NUM_TEST_THREAD 4
#define NUM_TEST_COUNT	100000
#define FUNC_CTRL	Reset
#define TEST_FUNC_MOD_ON		GL_moduleGatedClockOn
#define TEST_FUNC_MOD_OFF		GL_moduleGatedClockOff
#define TEST_FUNC_MOD_ON_EX		GL_moduleGatedClockOnEx
#define TEST_FUNC_MOD_OFF_EX	GL_moduleGatedClockOffEx

struct gsl_modctrl_test_struct {
	int count;
	int mids[4];
};
static GL_Task_t test_handle[NUM_TEST_THREAD];
static struct gsl_modctrl_test_struct test_info[NUM_TEST_THREAD] = {
//	{ .count = 1, .mids = { 9 }},
//	{ .count = 1, .mids = { 18 }},
//	{ .count = 1, .mids = { 3 }},
	{ .count = 1, .mids = { 4 }},
	{ .count = 1, .mids = { 16 }},
//	{ .count = 4, .mids = { 20, 21, 22, 23 }},
	{ .count = 4, .mids = { 24, 25, 26, 27 }},
	{ .count = 4, .mids = { 28, 29, 30, 31 }},
};

static void test(void *arg)
{
	int i;
	unsigned int ori;
	struct gsl_modctrl_test_struct *info = &test_info[(int)arg];
	unsigned int t1, t2;
	unsigned long long sum = 0;

	if (4 == info->count) {
		for (i = 0; i < NUM_TEST_COUNT; i++) {
			t1 = GL_GetCpuCnt();
			TEST_FUNC_MOD_ON_EX(info->count, info->mids[0], info->mids[1], info->mids[2], info->mids[3]);
			TEST_FUNC_MOD_OFF_EX(info->count, info->mids[0], info->mids[1], info->mids[2], info->mids[3]);
			t2 = GL_GetCpuCnt();
			sum += (t2 > t1 ? (t2 - t1) : (~0 - t2 + t1));
		}
	} else {
		TEST_FUNC_MOD_ON(info->mids[0]);
		for (i = 0; i < NUM_TEST_COUNT; i++) {
//			diag_printf("A [%d] 0x%08X\n", info->mids[0], HWREG_R(clken[info->mids[0] / 32]));
			t1 = GL_GetCpuCnt();
			ori = TEST_FUNC_MOD_OFF(info->mids[0]);
			if (1 != ori) {
				diag_printf("%s:%d: Error off!!\n", __FUNCTION__, __LINE__);
//				diag_printf("B [%d|%d] 0x%08X\n", info->mids[0], ori, HWREG_R(clken[info->mids[0] / 32]));
			}
//			diag_printf("C [%d] 0x%08X\n", info->mids[0], HWREG_R(clken[info->mids[0] / 32]));
			ori = TEST_FUNC_MOD_ON(info->mids[0]);
			if (0 != ori) {
				diag_printf("%s:%d: Error on!!\n", __FUNCTION__, __LINE__);
//				diag_printf("D [%d|%d] 0x%08X\n", info->mids[0], ori, HWREG_R(clken[info->mids[0] / 32]));
			}
			t2 = GL_GetCpuCnt();
			sum += (t2 > t1 ? (t2 - t1) : (~0 - t2 + t1));
		}
	}
	diag_printf("OK %d %s Average = %.2f\n", (int)arg,
				(4 == info->count) ? "EX" : "  ",
				sum / (double)NUM_TEST_COUNT / 2);
}


void gsl_modctrl_test(void)
{
	int i;

	diag_printf("Module control test begins!\n");
	for (i = 0; i < NUM_TEST_THREAD; ++i) {
		GL_TaskCreate("test", test, (void *)i, 2, 4096, TRUE, &test_handle[i]);
	}
}
#endif

#if defined(IC_SUPPORT_DRAM_A_32)
    #define WORD_PER_T 2
#elif defined(IC_SUPPORT_DRAM_A_16_L)
    #define WORD_PER_T 1
#elif defined(IC_SUPPORT_DRAM_A_16_H)
    #define WORD_PER_T 1
#else
	#error Unkown IC
#endif

/*
 * Use BW_PRINTF (snprintf + diag_printf) to print float numbers because
 * it is forbidden to use printf directly.
 */
#define BW_PRINTF(fmt, arg...)					\
	do {										\
		char buf[128];							\
		snprintf(buf, sizeof(buf), fmt, ##arg);	\
		diag_printf("%s", buf);					\
	} while (0)
#define CENTI_OF(_total, _val) (((double)(_val) * 100) / (_total))

/*
 * A32B32 commands need 128-byte alignment.
 */
#define DUMMY_MASTER_CHECK_MEM_RANGE(_addr, _off)							\
	do {																	\
		if (0 != ((_addr) & (128 - 1))) {									\
			diag_printf("[DummyMaster] Illegal address/offset. 128-byte"	\
						" aligned is required!"								\
						" Addr = 0x%08X, Offset = 0x%08X\n",				\
						(_addr), (_off));									\
			return;															\
		}																	\
	} while (0)

#if (IC_VER == QAE377)
#define BaseIndex(a) ((a) << 25)
#define AddrSpace(a) ((a) << 23)
#define Height(a)    ((a) << 17)
#define Length(a)    ((a) << 11)
#define WBE(a)       ((a) << 3)

int GL_bwASet(int n, const bwValue bwv[])
{
	int i;
	int totalBw;

	for (totalBw = i = 0; i < n; i++) {
		switch (bwv[i].module) {
		case BW_CB2MB:
		case BW_L2C:
		case BW_PERI_BG:
		case BW_CB_DMA:
		case BW_IMG_DEC:
		case BW_ATA_DEV_MAC:
		case BW_MINI_USB:
		case BW_MFD_MR_LUMA:
		case BW_MFD_LF_LUMA:
		case BW_EHCI_USB:
		case BW_G2D:
		case BW_VPP:
		case BW_VPP2:
		case BW_MFD_MR_CHROMA:
		case BW_MFD_LF_CHROMA:
		case BW_PG:
		case BW_OSD:
		case BW_AFRC_W:
		case BW_AFRC_R_TTX:
		case BW_MFD_MPRG:
		case BW_MFD_DMAG:
		case BW_MFD_MC2SD_LUMA:
		case BW_MFD_MC2SD_CHROMA:
		case BW_BD_FE:
		case BW_GPU:
		case BW_FONT:
		case BW_DUMMY:
			// Select page 0
			HWREG_0(mbar0_page_setting, 0xFFFC);
			HWREG_0(mbar0_m_setting[bwv[i].module], 0xFF58);

			if (bwv[i].flags & BW_FLAG_ABOVE_NORMAL) {
				HWREG_1(mbar0_m_setting[bwv[i].module], BW_FLAG_ABOVE_NORMAL);
			}

			if (bwv[i].flags & BW_FLAG_NO_URGENT) {
				HWREG_1(mbar0_m_setting[bwv[i].module], BW_FLAG_NO_URGENT);
			}

			switch (bwv[i].priority) {
			case BW_LOW_LATENCY:
				HWREG_1(mbar0_m_setting[bwv[i].module], 6);
				break;
			case BW_REAL_TIME:
				HWREG_1(mbar0_m_setting[bwv[i].module], 5);
				break;
			case BW_NORMAL:
				HWREG_1(mbar0_m_setting[bwv[i].module], 4);
				break;
			default:
				return -1;
			}

			// Select page 2 for bandwidth setting
			HWREG_1(mbar0_page_setting, 2);
			HWREG_W(mbar0_m_setting[bwv[i].module], bwv[i].bw10x);

			// Select page 1 for round-robin priority setting
			HWREG_S(mbar0_page_setting, 0xFFFC, 1);
			{
				int prip = (((bwv[i].bw10x * 32 * WORD_PER_T / 1000) & 0x3F) << 4);

				prip = (prip == 0) ? (1 << 4) : (prip);
				HWREG_W(mbar0_m_setting[bwv[i].module], prip | 0x0F);
			}
			totalBw += bwv[i].bw10x;
			break;
		default:
			return -1;
		}
	}

	return totalBw;
}

int GL_bwBSet(int n, const bwValue bwv[])
{
	int i;
	int totalBw;

	for (totalBw = i = 0; i < n; i++) {
		switch (bwv[i].module) {
		case BW_CB2MB:
		case BW_L2C:
		case BW_PERI_BG:
		case BW_CB_DMA:
		case BW_IMG_DEC:
		case BW_ATA_DEV_MAC:
		case BW_MINI_USB:
		case BW_MFD_MR_LUMA:
		case BW_MFD_LF_LUMA:
		case BW_EHCI_USB:
		case BW_G2D:
		case BW_VPP:
		case BW_VPP2:
		case BW_MFD_MR_CHROMA:
		case BW_MFD_LF_CHROMA:
		case BW_PG:
		case BW_OSD:
		case BW_AFRC_W:
		case BW_AFRC_R_TTX:
		case BW_MFD_MPRG:
		case BW_MFD_DMAG:
		case BW_MFD_MC2SD_LUMA:
		case BW_MFD_MC2SD_CHROMA:
		case BW_BD_FE:
		case BW_GPU:
		case BW_FONT:
		case BW_DUMMY:
			// Select page 0
			HWREG_0(mbar1_page_setting, 0xFFFC);
			HWREG_0(mbar1_m_setting[bwv[i].module], 0xFF58);

			if (bwv[i].flags & BW_FLAG_ABOVE_NORMAL) {
				HWREG_1(mbar1_m_setting[bwv[i].module], BW_FLAG_ABOVE_NORMAL);
			}

			if (bwv[i].flags & BW_FLAG_NO_URGENT) {
				HWREG_1(mbar1_m_setting[bwv[i].module], BW_FLAG_NO_URGENT);
			}

			switch (bwv[i].priority) {
			case BW_LOW_LATENCY:
				HWREG_1(mbar1_m_setting[bwv[i].module], 6);
				break;
			case BW_REAL_TIME:
				HWREG_1(mbar1_m_setting[bwv[i].module], 5);
				break;
			case BW_NORMAL:
				HWREG_1(mbar1_m_setting[bwv[i].module], 4);
				break;
			default:
				return -1;
			}

			// Select page 2 for bandwidth setting
			HWREG_1(mbar1_page_setting, 2);
			HWREG_W(mbar1_m_setting[bwv[i].module], bwv[i].bw10x);

			// Select page 1 for round-robin priority setting
			HWREG_S(mbar1_page_setting, 0xFFFC, 1);
			{
				int prip = (((bwv[i].bw10x * 32 * 1 / 1000) & 0x3F) << 4);

				prip = (prip == 0) ? (1 << 4) : (prip);
				HWREG_W(mbar1_m_setting[bwv[i].module], prip | 0x0F);
			}

			totalBw += bwv[i].bw10x;
			break;
		default:
			return -1;
		}
	}

	return totalBw;
}

void GL_bwAMonInit(bwMasterList a, bwMasterList b, bwMasterList c)
{
	/* Just set mon targets, not start yet */
	HWREG_W(mbar0_mon_setting, ((a & 0x1F) << 10) | ((b & 0x1F) << 5) | (c & 0x1F));
	/* Clear value */
	HWREG_1(mbar0_page_setting, 1 << 2);
}

void GL_bwBMonInit(bwMasterList a, bwMasterList b, bwMasterList c)
{
	/* Just set mon targets, not start yet */
	HWREG_W(mbar1_mon_setting, ((a & 0x1F) << 10) | ((b & 0x1F) << 5) | (c & 0x1F));
	/* Clear value */
	HWREG_1(mbar1_page_setting, 1 << 2);
}

void GL_bwAMonOn(void)
{
	HWREG_1(mbar0_mon_setting, 1 << 15);
}

void GL_bwBMonOn(void)
{
	HWREG_1(mbar1_mon_setting, 1 << 15);
}

void GL_bwAMonOff(void)
{
	HWREG_0(mbar0_mon_setting, (unsigned short)~(1 << 15));
}

void GL_bwBMonOff(void)
{
	HWREG_0(mbar1_mon_setting, (unsigned short)~(1 << 15));
}

void GL_bwAMonInfo(int *a, int *b, int *c, int *used, int *total)
{
	HWREG_W(mbar0_page_setting, 3);

	if (a != NULL) {
		*a = (HWREG_R(mbar0_m_setting[3]) << 16) + HWREG_R(mbar0_m_setting[4]);
	}

	if (b != NULL) {
		*b = (HWREG_R(mbar0_m_setting[5]) << 16) + HWREG_R(mbar0_m_setting[6]);
	}

	if (c != NULL) {
		*c = (HWREG_R(mbar0_m_setting[7]) << 16) + HWREG_R(mbar0_m_setting[8]);
	}

	if (used != NULL) {
		*used = (HWREG_R(mbar0_m_setting[9]) << 16) + HWREG_R(mbar0_m_setting[10]);
	}

	if (total != NULL) {
		*total = ((HWREG_R(mbar0_m_setting[11]) << 16) + HWREG_R(mbar0_m_setting[12])) / (WORD_PER_T == 2 ? 1 : 2);
	}
}

void GL_bwBMonInfo(int *a, int *b, int *c, int *used, int *total)
{
	HWREG_W(mbar1_page_setting, 3);

	if (a != NULL) {
		*a = (HWREG_R(mbar1_m_setting[3]) << 16) + HWREG_R(mbar1_m_setting[4]);
	}

	if (b != NULL) {
		*b = (HWREG_R(mbar1_m_setting[5]) << 16) + HWREG_R(mbar1_m_setting[6]);
	}

	if (c != NULL) {
		*c = (HWREG_R(mbar1_m_setting[7]) << 16) + HWREG_R(mbar1_m_setting[8]);
	}

	if (used != NULL) {
		*used = (HWREG_R(mbar1_m_setting[9]) << 16) + HWREG_R(mbar1_m_setting[10]);
	}

	if (total != NULL) {
		*total = ((HWREG_R(mbar1_m_setting[11]) << 16) + HWREG_R(mbar1_m_setting[12])) / 2;
	}
}

void GL_bwAMonInfo2(int *a, int *b, int *c, int *total)
{
	HWREG_W(mbar0_page_setting, 3);

	if (a != NULL) {
		*a = HWREG_R(mbar0_m_setting[13]);
	}

	if (b != NULL) {
		*b = HWREG_R(mbar0_m_setting[14]);
	}

	if (c != NULL) {
		*c = HWREG_R(mbar0_m_setting[15]);
	}

	if (total != NULL) {
		*total = HWREG_R(mbar0_m_setting[16]);
	}
}

void GL_bwBMonInfo2(int *a, int *b, int *c, int *total)
{
	HWREG_W(mbar1_page_setting, 3);

	if (a != NULL) {
		*a = HWREG_R(mbar1_m_setting[13]);
	}

	if (b != NULL) {
		*b = HWREG_R(mbar1_m_setting[14]);
	}

	if (c != NULL) {
		*c = HWREG_R(mbar1_m_setting[15]);
	}

	if (total != NULL) {
		*total = HWREG_R(mbar1_m_setting[16]);
	}
}

void GL_bwMonInfoDump(void)
{
	extern const char *mbus_master_name[];
	int setting;
	int a, b, c, used, total;
	int ma0, mb0, mc0, ma1, mb1, mc1;

	setting = HWREG_R(mbar0_mon_setting);

	ma0 = (setting >> 10) & 0x1F;
	mb0 = (setting >> 5) & 0x1F;
	mc0 = setting & 0x1F;

	setting = HWREG_R(mbar1_mon_setting);
	ma1 = (setting >> 10) & 0x1F;
	mb1 = (setting >> 5) & 0x1F;
	mc1 = setting & 0x1F;

	/* Dump results of long-term monitor.  */
	printf("bwA page2: %s = %d, %s = %d, %s = %d\n",
				mbus_master_name[ma0], GL_bwAGetValue(ma0),
				mbus_master_name[mb0], GL_bwAGetValue(mb0),
				mbus_master_name[mc0], GL_bwAGetValue(mc0));
	printf("bwA page2: %s = %d, %s = %d, %s = %d\n",
				mbus_master_name[ma1], GL_bwBGetValue(ma0),
				mbus_master_name[mb1], GL_bwBGetValue(mb0),
				mbus_master_name[mc1], GL_bwBGetValue(mc0));

	printf("BW     %10s(  %%  ) %10s(  %%  ) %10s(  %%  ) %10s %10s(  %%  )\n",
				mbus_master_name[ma0], mbus_master_name[mb0], mbus_master_name[mc0], "Total", "Used");
	GL_bwAMonInfo(&a, &b, &c, &used, &total);
	BW_PRINTF("bwA  : %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13u %10u(%05.2f)\n",
				a, CENTI_OF(total, a),
				b, CENTI_OF(total, b),
				c, CENTI_OF(total, c),
				total,
				used, CENTI_OF(total, used));
	GL_bwAMonInfo2(&a, &b, &c, &total);
	BW_PRINTF("bwA U: %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13u\n",
				a, CENTI_OF(total, a),
				b, CENTI_OF(total, b),
				c, CENTI_OF(total, c),
				total);

	printf("\n====\n");
	printf("BW     %10s(  %%  ) %10s(  %%  ) %10s(  %%  ) %10s %10s(  %%  )\n",
				mbus_master_name[ma1], mbus_master_name[mb1], mbus_master_name[mc1], "Total", "Used");
	GL_bwBMonInfo(&a, &b, &c, &used, &total);
	BW_PRINTF("bwB  : %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13u %10u(%05.2f)\n",
				a, CENTI_OF(total, a),
				b, CENTI_OF(total, b),
				c, CENTI_OF(total, c),
				total,
				used, CENTI_OF(total, used));
	GL_bwBMonInfo2(&a, &b, &c, &total);
	BW_PRINTF("bwB U: %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13u\n",
				a, CENTI_OF(total, a),
				b, CENTI_OF(total, b),
				c, CENTI_OF(total, c),
				total);
	printf("\n");
}

int GL_bwAGetValue(int a)
{
	// Select page 2 for bandwidth setting
	HWREG_S(mbar0_page_setting, 0xFFFC, 2);
	return (HWREG_R(mbar0_m_setting[a]) << 1) / WORD_PER_T;
}

int GL_bwBGetValue(int a)
{
	// Select page 2 for bandwidth setting
	HWREG_S(mbar1_page_setting, 0xFFFC, 2);
	return (HWREG_R(mbar1_m_setting[a]) << 1);
}
#if 0
#define DUMMY_MASTER0_BW_ALLOCATE(shift, bw, num)										\
	do {																				\
		int temp = (((1000 << (shift)) * (num)) >> 4) / (bw);							\
		int natural = temp - 1;															\
		int decimal = ((((100000 << (shift)) * (num)) >> 4) / (bw)) - (100 * temp);		\
		HWREG_W(dummy0_request_period, (natural & 0xFFFF) | ((decimal & 0x7F) << 16));	\
	} while (0)

#define DUMMY_MASTER1_BW_ALLOCATE(shift, bw, num)										\
	do {																				\
		int temp = (((1000 << (shift)) * (num)) >> 4) / (bw);							\
		int natural = temp - 1;															\
		int decimal = ((((100000 << (shift)) * (num)) >> 4) / (bw)) - (100 * temp);		\
		HWREG_W(dummy1_request_period, (natural & 0xFFFF) | ((decimal & 0x7F) << 16));	\
	} while (0)

void GL_bwADummySet(UINT32 dMode, UINT32 dAddr, UINT32 dOffset, UINT8 bUrgent, UINT32 dBw)
{
	const UINT32 length = 32;	/* A32B32 command.  */

	if (((dAddr >= 0x90000000) && (dAddr < 0xA0000000)) ||
		(dAddr >= 0xB0000000)) {
		printf("Dummy master 0 supports only DRAM A address.\n");
		return;
	}
	DUMMY_MASTER_CHECK_MEM_RANGE(dAddr, dOffset);

	GL_moduleClockOn(DUMMY_MASTER);

	dAddr = dAddr & 0x0FFFFFFF;

	/* Set request period.  */
#if defined(IC_SUPPORT_DRAM_A_32)
	DUMMY_MASTER0_BW_ALLOCATE(4, dBw, length);	/* 32-bit bw config.  */
#elif defined(IC_SUPPORT_DRAM_A_16_H) || defined(IC_SUPPORT_DRAM_A_16_L)
	DUMMY_MASTER0_BW_ALLOCATE(5, dBw, length);	/* 16-bit bw config.  */
#else
	#error "Undefined Dram bus-width!"	// Always contain the else part!
#endif

	/* Set urgent mode.  */
	HWREG_W(dummy0_urgent, (bUrgent << 31) | 10);

	/* Set starting address.  */
	HWREG_W(dummy0_addr_base, dAddr);
	HWREG_W(dummy0_addr_offset, dOffset);

	/* Set operation mode.  */
	HWREG_W(dummy0_op_mode, Length(length) | WBE(0xFF) | (dMode & 0x03));
}

void GL_bwADummyOn(void)
{
	HWREG_W(dummy0_control, 1);
}

void GL_bwADummyOff(void)
{
	HWREG_W(dummy0_control, 0);
}

void GL_bwBDummySet(UINT32 dMode, UINT32 dAddr, UINT32 dOffset, UINT8 bUrgent, UINT32 dBw)
{
	const UINT32 length = 32;	/* A32B32 command.  */

	if (((dAddr >= 0x80000000) && (dAddr < 0x90000000))
		|| ((dAddr >= 0xA0000000) && (dAddr < 0xB0000000))
		|| (dAddr >= 0xB8000000)) {
		diag_printf("Dummy master 1 supports only DRAM B address.\n");
		return;
	}
	DUMMY_MASTER_CHECK_MEM_RANGE(dAddr, dOffset);

	GL_moduleClockOn(DUMMY_MASTER);

	dAddr = dAddr & 0x0FFFFFFF;

	/* Set request period.  */
	DUMMY_MASTER1_BW_ALLOCATE(5, dBw, length);

	/* Set urgent mode.  */
	HWREG_W(dummy1_urgent, (bUrgent << 31) | 10);

	/* Set starting address.  */
	HWREG_W(dummy1_addr_base, dAddr);
	HWREG_W(dummy1_addr_offset, dOffset);

	/* Set operation mode.  */
	HWREG_W(dummy1_op_mode, (length << 11) | (0xFF << 3) | (dMode & 0x03));
}

void GL_bwBDummyOn(void)
{
	HWREG_W(dummy1_control, 1);
}

void GL_bwBDummyOff(void)
{
	HWREG_W(dummy1_control, 0);
}

#if 0
/*
 * Total_Bandwidth = 4 or 8 Bytes
 * Occupied bw = (DATA_COUNT * 8) / (RUN_TIME * Total_Bandwidth)
 *
 */
void GL_bwADummyInfo(void)
{
#if defined(IC_USE_DRAM_SIZE_128MB)
	const UINT8 sdram0_bw = 8;
#elif defined(IC_USE_DRAM_SIZE_64MB)
	const UINT8 sdram0_bw = 4;
#else
	#error "Undefined IC version!"	// Always contain the else part!
#endif
	double result;
	char buf[32];
	UINT64 datacount, runtime;

	datacount = HWREG_R(dummy0_datacount_h);
	datacount = (datacount << 32) + HWREG_R(dummy0_datacount_l);
	runtime = HWREG_R(dummy0_runtime_h);
	runtime = (runtime << 32) + HWREG_R(dummy0_runtime_l);

	result = ((double)datacount * 8) / (runtime * sdram0_bw);
	snprintf(buf, sizeof(buf), "%f%%", result * 100);
	diag_printf("Dummy master0 used bw = %s\n", buf);
}

void GL_bwBDummyInfo(void)
{
	const UINT8 sdram1_bw = 4;
	double result;
	char buf[32];
	UINT64 datacount, runtime;

	datacount = HWREG_R(dummy1_datacount_h);
	datacount = (datacount << 32) + HWREG_R(dummy1_datacount_l);
	runtime = HWREG_R(dummy1_runtime_h);
	runtime = (runtime << 32) + HWREG_R(dummy1_runtime_l);

	result = ((double)datacount * 8) / (runtime * sdram1_bw);
	snprintf(buf, sizeof(buf), "%f%%", result * 100);
	diag_printf("Dummy master1 used bw = %s\n", buf);
}

void GL_bwADummyDump(void)
{
	diag_printf("dummy0_op_mode        = 0x%08X\n", HWREG_R(dummy0_op_mode));
	diag_printf("dummy0_addr_base      = 0x%08X\n", HWREG_R(dummy0_addr_base));
	diag_printf("dummy0_addr_offset    = 0x%08X\n", HWREG_R(dummy0_addr_offset));
	diag_printf("dummy0_control        = 0x%08X\n", HWREG_R(dummy0_control));
	diag_printf("dummy0_urgent         = 0x%08X\n", HWREG_R(dummy0_urgent));
	diag_printf("dummy0_request_period = 0x%08X\n", HWREG_R(dummy0_request_period));
	diag_printf("dummy0_error          = 0x%08X\n", HWREG_R(dummy0_error));
	diag_printf("dummy0_runtime_l      = 0x%08X\n", HWREG_R(dummy0_runtime_l));
	diag_printf("dummy0_runtime_h      = 0x%08X\n", HWREG_R(dummy0_runtime_h));
	diag_printf("dummy0_datacount_l    = 0x%08X\n", HWREG_R(dummy0_datacount_l));
	diag_printf("dummy0_datacount_h    = 0x%08X\n", HWREG_R(dummy0_datacount_h));
	diag_printf("dummy0_mdatacount_l   = 0x%08X\n", HWREG_R(dummy0_mdatacount_l));
	diag_printf("dummy0_mdatacount_h   = 0x%08X\n", HWREG_R(dummy0_mdatacount_h));
	diag_printf("dummy0_sdatacount_l   = 0x%08X\n", HWREG_R(dummy0_sdatacount_l));
	diag_printf("dummy0_sdatacount_h   = 0x%08X\n", HWREG_R(dummy0_sdatacount_h));
}
#endif
#endif

#elif (IC_VER == QCE491) || (IC_VER == QCE483)
int GL_bwASet(int n, const bwValue bwv[])
{
	int i;
	int totalBw;

	for (totalBw = i = 0; i < n; i++) {

		switch (bwv[i].module) {
		case BW_CB2MB:
		case BW_L2C:
		case BW_PERI_BG:
		case BW_CB_DMA:
		case BW_IMG_DEC:
		case BW_ATA_DEV_MAC:
		case BW_MINI_USB:
		case BW_MFD_MR_LUMA:
		case BW_MFD_LF_LUMA:
		case BW_EHCI_USB:
		case BW_G2D:
		case BW_VPP:
		case BW_VPP2:
		case BW_MFD_MR_CHROMA:
		case BW_MFD_LF_CHROMA:
		case BW_PG:
		case BW_OSD:
		case BW_AFRC_W:
		case BW_AFRC_R_TTX:
		case BW_MFD_MPRG:
		case BW_MFD_DMAG:
		case BW_MFD_MC2SD_LUMA:
		case BW_MFD_MC2SD_CHROMA:
		case BW_BD_FE:
		case BW_GPU:
		case BW_FONT:
		case BW_DSP_D:
		case BW_DUMMY:
#if (IC_VER == QCE483)
		case BW_V656_IN:
#endif
			// Select page 0
			HWREG_W(mbar0_page_setting, 0);
			if (bwv[i].flags & BW_FLAG_ABOVE_NORMAL) {
				HWREG_1(mbar0_m_setting[bwv[i].module], BW_FLAG_ABOVE_NORMAL);
			}

			if (bwv[i].flags & BW_FLAG_NO_URGENT) {
				HWREG_1(mbar0_m_setting[bwv[i].module], BW_FLAG_NO_URGENT);
			}

			switch (bwv[i].priority) {
			case BW_LOW_LATENCY:
				HWREG_1(mbar0_m_setting[bwv[i].module], 6);
				break;
			case BW_REAL_TIME:
				HWREG_1(mbar0_m_setting[bwv[i].module], 5);
				break;
			case BW_NORMAL:
				HWREG_1(mbar0_m_setting[bwv[i].module], 4);
				break;
			default:
				return -1;
			}
			// Select page 2 for bandwidth setting
			HWREG_W(mbar0_page_setting, 2);
			HWREG_W(mbar0_m_setting[bwv[i].module], bwv[i].bw10x * 500 * WORD_PER_T / 1000);

			// Select page 1 for round-robin priority setting
			HWREG_W(mbar0_page_setting, 1);
			{
				int prip = ((bwv[i].bw10x * 32 * WORD_PER_T / 1000) & 0x3F);

				prip = (prip == 0) ? (1) : (prip);
				HWREG_W(mbar0_m_setting[bwv[i].module], prip);

			}
			totalBw += bwv[i].bw10x;
			break;
		default:
			return -1;
		}
	}

	return totalBw;
}

void GL_bwAMonInit(bwMasterList a, bwMasterList b, bwMasterList c)
{
	/*
	 * Enable monitor clock.
	 *
	 * We don't care about disabling it.
	 */
	HWREG_1(mbus_setting, 1 << 4);

	/* Select page 3.  */
	HWREG_W(mbar0_page_setting, 3);

	/*
	 * GRP(31,15) = MA_ID << 16 | MB_ID << 8 | MC_ID
	 */
	HWREG_W(mbar0_m_setting[15], ((a & 0xFF) << 16) | ((b & 0xFF) << 8) | (c & 0xFF));

	/* Clear value */
	HWREG_W(mbar0_m_setting[30], 0);

/*
This macro enable to monitor total request for specific master ID (a)
if high percentage of urgent/total request for master ID (a), it may be lack of bw.
*/
#define ENABLE_DUMP_TOTAL_URGENT_REQUEST
#ifdef ENABLE_DUMP_TOTAL_URGENT_REQUEST
	//----------------------
	//diag_printf("[bwmon dump 1] gclken[0](0x%08X)=0x%08X\n", (UINT32)HWREG_ADDR(gclken[0]), HWREG_R(gclken[0]) );
	//diag_printf("[bwmon dump 1] mbus_setting(0x%08X)=0x%08X\n", (UINT32)HWREG_ADDR(mbus_setting), HWREG_R(mbus_setting) );
	//diag_printf("[bwmon dump 1] mbus_monitor_setting(0x%08X)=0x%08X\n", (UINT32)HWREG_ADDR(mbus_monitor_setting), HWREG_R(mbus_monitor_setting) );
	//----------------------

	//GRP(29, 0) = ( GRP(29, 0) & ~(3) ) | 2; // set monitor to command mode
	HWREG_S(mbus_setting, ~(3), 2);

	//GRP( 0, 12 ) &= ~( 1 << 12); // set monitor clock to free run clock
	HWREG_0(gclken[0], ~( 1 << 12));

	//GRP( 29, 0) |= 1<<4;  // enable monitor clock
	HWREG_1(mbus_setting, (1<<4) );

	//GRP(29, 1) = Master ID; // set monitor Master ID,
	HWREG_S(mbus_monitor_setting, ~(0x3F), ((5<<12)|(a & 0x1F)) );

	//----------------------
	//diag_printf("[bwmon dump 2] gclken[0](0x%08X)=0x%08X\n", (UINT32)HWREG_ADDR(gclken[0]), HWREG_R(gclken[0]) );
	//diag_printf("[bwmon dump 2] mbus_setting(0x%08X)=0x%08X\n", (UINT32)HWREG_ADDR(mbus_setting), HWREG_R(mbus_setting) );
	//diag_printf("[bwmon dump 2] mbus_monitor_setting(0x%08X)=0x%08X\n", (UINT32)HWREG_ADDR(mbus_monitor_setting), HWREG_R(mbus_monitor_setting) );
	//----------------------
#endif //ENABLE_DUMP_TOTAL_URGENT_REQUEST
}

void GL_bwAMonOn(void)
{
	HWREG_W(mbar0_page_setting, 3);
	HWREG_W(mbar0_m_setting[31], 1);

#ifdef ENABLE_DUMP_TOTAL_URGENT_REQUEST
	//GRP(29, 2) = 1; // enable monitoring
	HWREG_W(mbus_monitor_en, 1);
#endif //ENABLE_DUMP_TOTAL_URGENT_REQUEST

}

void GL_bwAMonOff(void)
{
#ifdef ENABLE_DUMP_TOTAL_URGENT_REQUEST
	//GRP(29, 2) = 0; // disable monitoring
	HWREG_W(mbus_monitor_en, 0);
#endif //ENABLE_DUMP_TOTAL_URGENT_REQUEST

	HWREG_W(mbar0_page_setting, 3);
	HWREG_W(mbar0_m_setting[31], 0);
}

#if (IC_VER == QCE483)
int GL_bwBSet(int n, const bwValue bwv[])
{
	int i;
	int totalBw;

	for (totalBw = i = 0; i < n; i++) {
		switch (bwv[i].module) {
		case BW_CB2MB:
		case BW_L2C:
		case BW_PERI_BG:
		case BW_CB_DMA:
		case BW_IMG_DEC:
		case BW_ATA_DEV_MAC:
		case BW_MINI_USB:
		case BW_MFD_MR_LUMA:
		case BW_MFD_LF_LUMA:
		case BW_EHCI_USB:
		case BW_G2D:
		case BW_VPP:
		case BW_VPP2:
		case BW_MFD_MR_CHROMA:
		case BW_MFD_LF_CHROMA:
		case BW_PG:
		case BW_OSD:
		case BW_AFRC_W:
		case BW_AFRC_R_TTX:
		case BW_MFD_MPRG:
		case BW_MFD_DMAG:
		case BW_MFD_MC2SD_LUMA:
		case BW_MFD_MC2SD_CHROMA:
		case BW_BD_FE:
		case BW_GPU:
		case BW_FONT:
		case BW_DSP_D:
		case BW_DUMMY:
		case BW_V656_IN:
			// Select page 0
			HWREG_W(mbar1_page_setting, 0);
			if (bwv[i].flags & BW_FLAG_ABOVE_NORMAL) {
				HWREG_1(mbar1_m_setting[bwv[i].module], BW_FLAG_ABOVE_NORMAL);
			}

			if (bwv[i].flags & BW_FLAG_NO_URGENT) {
				HWREG_1(mbar1_m_setting[bwv[i].module], BW_FLAG_NO_URGENT);
			}

			switch (bwv[i].priority) {
			case BW_LOW_LATENCY:
				HWREG_1(mbar1_m_setting[bwv[i].module], 6);
				break;
			case BW_REAL_TIME:
				HWREG_1(mbar1_m_setting[bwv[i].module], 5);
				break;
			case BW_NORMAL:
				HWREG_1(mbar1_m_setting[bwv[i].module], 4);
				break;
			default:
				return -1;
			}
			// Select page 2 for bandwidth setting
			HWREG_W(mbar1_page_setting, 2);
			HWREG_W(mbar1_m_setting[bwv[i].module], bwv[i].bw10x);

			// Select page 1 for round-robin priority setting
			HWREG_W(mbar1_page_setting, 1);
			{
				int prip = ((bwv[i].bw10x * 32 * WORD_PER_T / 1000) & 0x3F);

				prip = (prip == 0) ? (1) : (prip);
				HWREG_W(mbar1_m_setting[bwv[i].module], prip);

			}
			totalBw += bwv[i].bw10x;
			break;
		default:
			return -1;
		}
	}

	return totalBw;
}

void GL_bwBMonInit(bwMasterList a, bwMasterList b, bwMasterList c)
{
	/*
	 * Enable monitor clock.
	 *
	 * We don't care about disabling it.
	 */
	HWREG_1(mbus_setting, 1 << 4);

	/* Select page 3.  */
	HWREG_W(mbar1_page_setting, 3);

	/*
	 * GRP(31,15) = MA_ID << 16 | MB_ID << 8 | MC_ID
	 */
	HWREG_W(mbar1_m_setting[15], ((a & 0xFF) << 16) | ((b & 0xFF) << 8) | (c & 0xFF));

	/* Clear value */
	HWREG_W(mbar1_m_setting[30], 0);
}

void GL_bwBMonOn(void)
{
	HWREG_W(mbar1_page_setting, 3);
	HWREG_W(mbar1_m_setting[31], 1);
}

void GL_bwBMonOff(void)
{
	HWREG_W(mbar1_page_setting, 3);
	HWREG_W(mbar1_m_setting[31], 0);
}
#endif

void GL_bwAMonInfo(int *a, int *b, int *c, int *used, int *total)
{
	HWREG_W(mbar0_page_setting, 3);

	if (a != NULL) {
		*a = HWREG_R(mbar0_m_setting[17]);
	}

	if (b != NULL) {
		*b = HWREG_R(mbar0_m_setting[21]);
	}

	if (c != NULL) {
		*c = HWREG_R(mbar0_m_setting[25]);
	}

	if (used != NULL) {
		*used = HWREG_R(mbar0_m_setting[28]);
	}

	if (total != NULL) {
		*total = HWREG_R(mbar0_m_setting[30])/* * WORD_PER_T*/;
	}
}

#if (IC_VER == QCE483)
void GL_bwBMonInfo(int *a, int *b, int *c, int *used, int *total)
{
	HWREG_W(mbar1_page_setting, 3);

	if (a != NULL) {
		*a = HWREG_R(mbar1_m_setting[17]);
	}

	if (b != NULL) {
		*b = HWREG_R(mbar1_m_setting[21]);
	}

	if (c != NULL) {
		*c = HWREG_R(mbar1_m_setting[25]);
	}

	if (used != NULL) {
		*used = HWREG_R(mbar1_m_setting[28]);
	}

	if (total != NULL) {
		*total = HWREG_R(mbar1_m_setting[30])/* * WORD_PER_T*/;
	}
}
#endif

void GL_bwAMonInfo2(int *a, int *b, int *c, int *total)
{
	HWREG_W(mbar0_page_setting, 3);

	if (a != NULL) {
		*a = HWREG_R(mbar0_m_setting[18]);
	}

	if (b != NULL) {
		*b = HWREG_R(mbar0_m_setting[22]);
	}

	if (c != NULL) {
		*c = HWREG_R(mbar0_m_setting[26]);
	}

	if (total != NULL) {
		*total = HWREG_R(mbar0_m_setting[29]);
	}
}

#if (IC_VER == QCE483)
void GL_bwBMonInfo2(int *a, int *b, int *c, int *total)
{
	HWREG_W(mbar1_page_setting, 3);

	if (a != NULL) {
		*a = HWREG_R(mbar1_m_setting[18]);
	}

	if (b != NULL) {
		*b = HWREG_R(mbar1_m_setting[22]);
	}

	if (c != NULL) {
		*c = HWREG_R(mbar1_m_setting[26]);
	}

	if (total != NULL) {
		*total = HWREG_R(mbar1_m_setting[29]);
	}
}
#endif

#ifdef ENABLE_DUMP_TOTAL_URGENT_REQUEST
void GL_bwAMonInfo3(int *a)
{
	if (a != NULL) {
		//Master Request = GRP(29, 5), get total request
		*a = HWREG_R(mbus_command);
	}

	//GRP(29, 4) = 0; // clear
	HWREG_W(mbus_latency, 0);
}
#endif //ENABLE_DUMP_TOTAL_URGENT_REQUEST

void GL_bwMonInfoDump(void)
{
	extern const char *mbus_master_name[];
	int setting;
	int a, b, c, used, total;
	double total_f;
	int ma0, mb0, mc0;

	setting = HWREG_R(mbar0_m_setting[15]);

	ma0 = (setting >> 16) & 0xFF;
	mb0 = (setting >> 8) & 0xFF;
	mc0 = setting & 0xFF;

	/* Dump results of long-term monitor.  */
	diag_printf("bwA page2: %s = %d, %s = %d, %s = %d\n",
				mbus_master_name[ma0], GL_bwAGetValue(ma0),
				mbus_master_name[mb0], GL_bwAGetValue(mb0),
				mbus_master_name[mc0], GL_bwAGetValue(mc0));
#if (IC_VER == QCE483)
	int ma1, mb1, mc1;

	setting = HWREG_R(mbar1_m_setting[15]);
	ma1 = (setting >> 16) & 0xFF;
	mb1 = (setting >> 8) & 0xFF;
	mc1 = setting & 0xFF;
	diag_printf("bwB page2: %s = %d, %s = %d, %s = %d\n",
				mbus_master_name[ma1], GL_bwBGetValue(ma0),
				mbus_master_name[mb1], GL_bwBGetValue(mb0),
				mbus_master_name[mc1], GL_bwBGetValue(mc0));
#endif

	diag_printf("BW     %10s(  %%  ) %10s(  %%  ) %10s(  %%  ) %13s %10s(  %%  )\n",
				mbus_master_name[ma0], mbus_master_name[mb0], mbus_master_name[mc0], "Total", "Used");
	GL_bwAMonInfo(&a, &b, &c, &used, &total);
	total_f = ((double)total * WORD_PER_T * GL_GetMemFreq() / GL_GetSysBusFreq());
	BW_PRINTF("bwA  : %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13.2f %10u(%05.2f)\n",
				a, CENTI_OF(total_f, a),
				b, CENTI_OF(total_f, b),
				c, CENTI_OF(total_f, c),
				total_f,
				used, CENTI_OF(total_f, used));
	GL_bwAMonInfo2(&a, &b, &c, &total);
	BW_PRINTF("bwA U: %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13u\n",
				a, CENTI_OF(total, a),
				b, CENTI_OF(total, b),
				c, CENTI_OF(total, c),
				total);
	if ((0 == ma0) || (0 == mb0) || (0 == mc0)) {
		BW_PRINTF("bwA busy: %.2f%%\n", CENTI_OF(HWREG_R(mbar0_m_setting[29]), HWREG_R(mbar0_m_setting[16])));
	}

#ifdef ENABLE_DUMP_TOTAL_URGENT_REQUEST
	GL_bwAMonInfo3(&b);
	BW_PRINTF("bwA U: total: %10u, %s urgent/request: (%05.2f)\n",
				b, mbus_master_name[ma0], ((((double)a)*100))/b );
#endif // ENABLE_DUMP_TOTAL_URGENT_REQUEST

#if (IC_VER == QCE483)
	diag_printf("\n====\n");
	diag_printf("BW     %10s(  %%  ) %10s(  %%  ) %10s(  %%  ) %10s %10s(  %%  )\n",
				mbus_master_name[ma1], mbus_master_name[mb1], mbus_master_name[mc1], "Total", "Used");
	GL_bwBMonInfo(&a, &b, &c, &used, &total);
	total_f = ((double)total * 1 * GL_GetMemFreq() / GL_GetSysBusFreq());
	BW_PRINTF("bwB  : %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13.2f %10u(%05.2f)\n",
				a, CENTI_OF(total_f, a),
				b, CENTI_OF(total_f, b),
				c, CENTI_OF(total_f, c),
				total_f,
				used, CENTI_OF(total_f, used));
	GL_bwBMonInfo2(&a, &b, &c, &total);
	BW_PRINTF("bwB U: %10u(%05.2f) %10u(%05.2f) %10u(%05.2f) %13u\n",
				a, CENTI_OF(total, a),
				b, CENTI_OF(total, b),
				c, CENTI_OF(total, c),
				total);
	if ((0 == ma1) || (0 == mb1) || (0 == mc1)) {
		BW_PRINTF("bwB busy: %.2f%%\n", CENTI_OF(HWREG_R(mbar1_m_setting[30]), HWREG_R(mbar1_m_setting[17])));
	}
#endif
	diag_printf("\n");
}

int GL_bwAGetValue(int a)
{
	if ((a < BW_CB2MB) || (a > BW_DUMMY)) {
		return -1;
	}

	// Select page 2 for bandwidth setting
	HWREG_W(mbar0_page_setting, 2);
	return (HWREG_R(mbar0_m_setting[a]) << 1) / WORD_PER_T;
}

#if (IC_VER == QCE483)
int GL_bwBGetValue(int a)
{
	if ((a < BW_CB2MB) || (a > BW_DUMMY)) {
		return -1;
	}
	// Select page 2 for bandwidth setting
	HWREG_W(mbar1_page_setting, 2);
	return (HWREG_R(mbar1_m_setting[a]) << 1) / WORD_PER_T;
}
#endif

#define DUMMY_MASTER0_BW_ALLOCATE(shift, bw, num)										\
	do {																				\
		int temp = (((1000 << (shift)) * (num)) >> 4) / (bw);							\
		int natural = temp - 1;															\
		int decimal = ((((100000 << (shift)) * (num)) >> 4) / (bw)) - (100 * temp);		\
		HWREG_W(dummy0_request_period, (natural & 0xFFFF) | ((decimal & 0x7F) << 16));	\
	} while (0)

#define DUMMY_MASTER1_BW_ALLOCATE(shift, bw, num)										\
	do {																				\
		int temp = (((1000 << (shift)) * (num)) >> 4) / (bw);							\
		int natural = temp - 1;															\
		int decimal = ((((100000 << (shift)) * (num)) >> 4) / (bw)) - (100 * temp);		\
		HWREG_W(dummy1_request_period, (natural & 0xFFFF) | ((decimal & 0x7F) << 16));	\
	} while (0)

#define BaseIndex(a) ((a) << 24)
#define AddrSpace(a) ((a) << 4)
#define Height(a)    ((a) << 16)
#define Length(a)    ((a) << 8)
#define WBE(a)       ((a) << 3)
#define BW_ARCH(bw)											\
	({														\
		unsigned long long tmp;								\
		tmp = (unsigned long long)(bw) * GL_GetMemFreq();	\
		(bw) = (typeof(bw))(tmp / GL_GetSysBusFreq());		\
		(bw);\
	})

/*
 * Mantis PD2 0068269
 *   Three hardware bugs:
 *   1. Read-after-write becomes write-after-read if dummy master keeps being
 *      switching on and off.
 *   2. Addresses used dummy master change unexpectedly while turning off
 *      dummy master. It leads to errors of read-after-write commands.
 *   3. Increasing incompleted write transactions overflow.
 *
 */
#define WORKAROUND_DUMMY_ERROR_FLAG

void GL_bwADummySet(UINT32 dMode, UINT32 dAddr, UINT32 dOffset, UINT8 bUrgent, UINT32 dBw)
{
	const UINT32 length = 32;	/* A32B32 command.  */

	if (((dAddr >= 0x90000000) && (dAddr < 0xA0000000)) ||
		(dAddr >= 0xB0000000)) {
		diag_printf("Dummy master 0 supports only DRAM A address.\n");
		return;
	}
	DUMMY_MASTER_CHECK_MEM_RANGE(dAddr, dOffset);

	GL_moduleClockOn(DUMMY_MASTER);

	dAddr = dAddr & 0x0FFFFFFF;
	dBw = BW_ARCH(dBw);

	/* Set request period.  */
#if defined(IC_SUPPORT_DRAM_A_32)
	DUMMY_MASTER0_BW_ALLOCATE(4, dBw, length);	/* 32-bit bw config.  */
#else
	DUMMY_MASTER0_BW_ALLOCATE(5, dBw, length);	/* 16-bit bw config.  */
#endif

	/* Set urgent mode.  */
	HWREG_W(dummy0_urgent, (bUrgent << 31) | 10);

	/* Set starting address.  */
	HWREG_W(dummy0_addr_base, dAddr);
	HWREG_W(dummy0_addr_offset, dAddr + dOffset);

	/* Set operation mode.  */
	HWREG_W(dummy0_op_mode, Length(length) | WBE(0x01) | (dMode & 0x03));
}

void GL_bwADummyOn(void)
{
	HWREG_W(dummy0_control, 1);
}

void GL_bwADummyOff(void)
{
#ifdef WORKAROUND_DUMMY_ERROR_FLAG
	HWREG_W(mbar0_page_setting,0);
	HWREG_W(mbar0_m_setting[29], (HWREG_R(mbar0_m_setting[29]) & 0xFF) | 0x10);
	HWREG_W(dummy0_control, 0);
	HWREG_W(mbar0_m_setting[29], (HWREG_R(mbar0_m_setting[29]) & 0xFF) & ~0x10);

	/* Wait until transactions of dummy master finish.  */
	while (((HWREG_R(mbar0_m_setting[29]) & 0xFF00) >> 8) != 0x0) {
		continue;
	}

	GL_moduleResetOn(DUMMY_MASTER);
	GL_moduleResetOff(DUMMY_MASTER);
#else
	HWREG_W(dummy0_control, 0);
#endif
}

#if (IC_VER == QCE483)
void GL_bwBDummySet(UINT32 dMode, UINT32 dAddr, UINT32 dOffset, UINT8 bUrgent, UINT32 dBw)
{
	const UINT32 length = 32;	/* A32B32 command.  */

	if (((dAddr >= 0x80000000) && (dAddr < 0x90000000))
		|| ((dAddr >= 0xA0000000) && (dAddr < 0xB0000000))
		|| (dAddr >= 0xB8000000)) {
		diag_printf("Dummy master 1 supports only DRAM B address.\n");
		return;
	}
	DUMMY_MASTER_CHECK_MEM_RANGE(dAddr, dOffset);

	GL_moduleClockOn(DUMMY_MASTER);

	dAddr = dAddr & 0x0FFFFFFF;
	dBw = BW_ARCH(dBw);

	/* Set request period.  */
	DUMMY_MASTER1_BW_ALLOCATE(5, dBw, length);

	/* Set urgent mode.  */
	HWREG_W(dummy1_urgent, (bUrgent << 31) | 10);

	/* Set starting address.  */
	HWREG_W(dummy1_addr_base, dAddr);
	HWREG_W(dummy1_addr_offset, dAddr + dOffset);

	/* Set operation mode.  */
	HWREG_W(dummy1_op_mode, Length(length) | WBE(0x01) | (dMode & 0x03));
}

void GL_bwBDummyOn(void)
{
	HWREG_W(dummy1_control, 1);
}

void GL_bwBDummyOff(void)
{
#ifdef WORKAROUND_DUMMY_ERROR_FLAG
	HWREG_W(mbar1_page_setting,0);
	HWREG_W(mbar1_m_setting[29], (HWREG_R(mbar1_m_setting[29]) & 0xFF) | 0x10);
	HWREG_W(dummy1_control, 0);
	HWREG_W(mbar1_m_setting[29], (HWREG_R(mbar1_m_setting[29]) & 0xFF) & ~0x10);

	/* Wait until transactions of dummy master finish.  */
	while (((HWREG_R(mbar1_m_setting[29]) & 0xFF00) >> 8) != 0x0) {
		continue;
	}

	GL_moduleResetOn(DUMMY_MASTER);
	GL_moduleResetOff(DUMMY_MASTER);
#else
	HWREG_W(dummy1_control, 0);
#endif
}
#endif

void GL_bwADummyInfo(void)
{
	/* Function removed since 483/491.  */
}

#if (IC_VER == QCE483)
void GL_bwBDummyInfo(void)
{
	/* Function removed since 483/491.  */
}
#endif

void GL_bwADummyDump(void)
{
	diag_printf("dummy0_op_mode        = 0x%08X\n", HWREG_R(dummy0_op_mode));
	diag_printf("dummy0_addr_base      = 0x%08X\n", HWREG_R(dummy0_addr_base));
	diag_printf("dummy0_addr_offset    = 0x%08X\n", HWREG_R(dummy0_addr_offset));
	diag_printf("dummy0_control        = 0x%08X\n", HWREG_R(dummy0_control));
	diag_printf("dummy0_urgent         = 0x%08X\n", HWREG_R(dummy0_urgent));
	diag_printf("dummy0_request_period = 0x%08X\n", HWREG_R(dummy0_request_period));
	diag_printf("dummy0_request_count  = 0x%08X\n", HWREG_R(dummy0_request_count));
	diag_printf("dummy0_error          = 0x%08X\n", HWREG_R(dummy0_error));
}
#else
#error "Please define correct IC_VER!"
#endif

void GL_moduleDRAMSel(UINT32 modulelists, mDRAMList b)
{
	HWREG_S(sft_cfg_5, ~modulelists, (b == M_DRAM_A) ? 0 : modulelists);
}

