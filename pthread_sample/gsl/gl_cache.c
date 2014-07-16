#include <gsl.h>
#include <gsl/lambo86_hwop.h>

/*
 * GL_CACHEOP_CHECK_CACHED_ADDR
 *    0: Transform addresses to be cached addresses without warning
 *    1: Transform addresses to be cached addresses with warning
 */
#define GL_CACHEOP_CHECK_CACHED_ADDR 0

#if (GL_CACHEOP_CHECK_CACHED_ADDR != 0)
#define check_cached_addr(pAddr)																	\
	do {																							\
		if (!CHECK_IF_ADDR_IN_DRAM_A_CACHED((unsigned int)(pAddr))) {								\
			diag_printf("%s: Can't do cache ops with uncached addr: 0x%08X, caller addr = 0x%08X\n",\
						__FUNCTION__, (unsigned int)(pAddr), (UINT32)__builtin_return_address(0));	\
			pAddr = GL_Addr2Cache(pAddr);															\
		}																							\
	} while (0)
#else
#define check_cached_addr(pAddr) 			\
	do { 									\
		(pAddr) = GL_Addr2Cache((pAddr));	\
	} while (0);
#endif

GL_Status_t GL_DataCacheFlush(void *pAddr, GL_UINT32 dSize)
{
	check_cached_addr(pAddr);
	lambo86_cache_flush(pAddr, dSize);
	return GL_SUCCESS;
}

GL_Status_t GL_DataCacheInvalidate(void *pAddr, GL_UINT32 dSize)
{
	check_cached_addr(pAddr);
	lambo86_cache_inva(pAddr, dSize);
	return GL_SUCCESS;
}

GL_Status_t GL_DataCacheFlushAndInvalidate(void *pAddr, GL_UINT32 dSize)
{
	check_cached_addr(pAddr);
	lambo86_cache_flush_inva(pAddr, dSize);
	return GL_SUCCESS;
}

#if 0
GL_Status_t GL_DataCacheFlushAndInvalidateAll(void)
{
	lambo86_cache_flush_all();
	return GL_SUCCESS;
}

GL_Status_t GL_InstCacheInvalidate(void *pAddr, GL_UINT32 dSize)
{
	__builtin___clear_cache (pAddr, pAddr + dSize - 1);
	return GL_SUCCESS;
}

GL_Status_t GL_InstCacheInvalidateAll(void)
{
	return GL_SUCCESS;
}

GL_Status_t GL_InstCacheFill(void *pAddr, GL_UINT32 dSize)
{
	__builtin___clear_cache (pAddr, pAddr + dSize - 1);
	return GL_SUCCESS;
}

void *GL_ToDataCacheAddrNoSync(void *pAddr)
{
	return GL_TO_CACHE(pAddr);
}

void *GL_ToDataUncacheAddrNoSync(void *pAddr)
{
	return GL_TO_UNCACHE(pAddr);
}

void *GL_ToDataCacheAddrSync(void *pAddr, GL_UINT32 dSize)
{
	void* cacheaddr;

	if(GL_IS_CACHE(pAddr)){
		return pAddr;
	}

	cacheaddr = GL_TO_CACHE(pAddr);

	GL_DataCacheFlushAndInvalidate(cacheaddr, dSize);

	return cacheaddr;
}

void *GL_ToDataUncacheAddrSync(void *pAddr, GL_UINT32 dSize)
{
	if(GL_IS_CACHE(pAddr)){
		GL_DataCacheFlushAndInvalidate(pAddr, dSize)
	}

	return (void *)GL_TO_UNCACHE(pAddr);
}

GL_Status_t GL_IsDataCacheHit(void *addr)
{
	return GL_FAILURE;
}

GL_Status_t GL_CacheClearAll(void *ptr)
{
	return GL_SUCCESS;
}

void GL_CacheInfo(void)
{
}
#endif

#define TEMP_DELAY()	asm volatile("nop;")

static const unsigned int l2c_size_config[8] = {128, 64, 32, 16, 8, 4, 2, 1};

void GL_L2cSizeA(unsigned int size)
{
	unsigned int setting = 0;
	unsigned int wdog_ctl_l2c;

	if (size == 0 || size > 128) {
		size = 128;
	}
	size = (128 / size) - 1;
	while (size != 0) {
		size >>= 1;
		setting += 1;
	}

	/* Disable L2C.  */
	wdog_ctl_l2c = HWREG_R(cbus_wdog_ctl) & 0x0300;
	HWREG_W(cbus_wdog_ctl, HWREG_R(cbus_wdog_ctl) & ~(0x03 << 8));

	GL_L2cInvalidateA();

	/* Change L2C size.  */
	HWREG_W(l2c0_type_config, (HWREG_R(l2c0_type_config) & ~0x00F0) | ((setting & 0x07) << 4));

	/* Resotre L2C status.  */
	HWREG_W(cbus_wdog_ctl, HWREG_R(cbus_wdog_ctl) | wdog_ctl_l2c);
}

void GL_L2cInvalidateA(void)
{
	HWREG_W(l2c0_rgst_acc_tag[0], HWREG_R(l2c0_rgst_acc_tag[0]) | (1 << 12));
	while (((HWREG_R(l2c0_rgst_acc_tag[0]) & (1 << 13)) >> 13) != 1) {
		continue;
	}
	HWREG_W(l2c0_rgst_acc_tag[0], HWREG_R(l2c0_rgst_acc_tag[0]) & ~(1 << 12));
}

#if defined(DRAM_BUS_DOUBLE)
static int gl_IsL2cDisabled(void)
{
	/* Global L2C switch.  */
	if (((HWREG_R(cbus_wdog_ctl) >> 8) & 0x03) != 0x03) {
		return 1;
	}

	if (((HWREG_R(l2c0_type_config) & 0x01) == 0)
		&& ((HWREG_R(l2c1_type_config) & 0x01) == 0)
		&& ((HWREG_R(l2c0_chk_option) & 0x03) == 0)
		&& ((HWREG_R(l2c1_chk_option) & 0x03) == 0)) {
		return 0;
	}

	return 1;
}

void GL_L2cSizeB(unsigned int size)
{
	unsigned int setting = 0;
	unsigned int wdog_ctl_l2c;

	if (size == 0 || size > 128) {
		size = 128;
	}
	size = (128 / size) - 1;
	while (size != 0) {
		size >>= 1;
		setting += 1;
	}

	/* Disable L2C.  */
	wdog_ctl_l2c = HWREG_R(cbus_wdog_ctl) & 0x0300;
	HWREG_W(cbus_wdog_ctl, HWREG_R(cbus_wdog_ctl) & ~(0x03 << 8));

	GL_L2cInvalidateB();

	/* Change L2C size.  */
	HWREG_W(l2c1_type_config, (HWREG_R(l2c1_type_config) & ~0x00F0) | ((setting & 0x07) << 4));

	/* Resotre L2C status.  */
	HWREG_W(cbus_wdog_ctl, HWREG_R(cbus_wdog_ctl) | wdog_ctl_l2c);
}

void GL_L2cInvalidateB(void)
{
	HWREG_W(l2c1_rgst_acc_tag[0], HWREG_R(l2c1_rgst_acc_tag[0]) | (1 << 12));
	while (((HWREG_R(l2c1_rgst_acc_tag[0]) & (1 << 13)) >> 13) != 1) {
		continue;
	}
	HWREG_W(l2c1_rgst_acc_tag[0], HWREG_R(l2c1_rgst_acc_tag[0]) & ~(1 << 12));
}

static void gl_L2cInfoInitB(void)
{
	/* Reset hit-rate profiling counter */
	HWREG_W(l2c1_hit_prof[0], 0);	// 54.1
	HWREG_W(l2c1_int, 0);			// 53.4

	/* Set counting boundaries */
	HWREG_W(l2c1_hit_prof[1], 0xFFFFFFFF);	// 54.2

	/* Enable specified master profiling for read/write command in MCB */
	HWREG_W(l2c1_hit_prof[0], 5);	// 54.1

}

inline unsigned int GL_L2cInfoBWCmds(void)
{
	return HWREG_R(l2c1_hit_prof_result[0]);
}

inline unsigned int GL_L2cInfoBWHits(void)
{
	return HWREG_R(l2c1_hit_prof_result[1]);
}

inline unsigned int GL_L2cInfoBRCmds(void)
{
	return HWREG_R(l2c1_hit_prof_result[2]);
}

inline unsigned int GL_L2cInfoBRHits(void)
{
	return HWREG_R(l2c1_hit_prof_result[3]);
}

void gl_L2cInfoResultB(void)
{
	unsigned int allB[2], hitB[2];
	char buf[8];

	hitB[0] = GL_L2cInfoBWHits();
	TEMP_DELAY();
	allB[0] = GL_L2cInfoBWCmds() | 0x01;
	TEMP_DELAY();
	hitB[1] = GL_L2cInfoBRHits();
	TEMP_DELAY();
	allB[1] = GL_L2cInfoBRCmds() | 0x01;

	printf("L2C size B = %d KByte\n", l2c_size_config[(HWREG_R(l2c1_type_config) >> 4) & 0x07]);
	snprintf(buf, sizeof(buf), "%.3f", 100 * (hitB[0] / (double)allB[0]));
	printf("========== DRAM B ==========\n");
	printf("        Total write command: %11u\n", allB[0]);
	printf("    Total write hit command: %11u\n", hitB[0]);
	printf("             Write hit rate: [%8s%%]\n", buf);
	snprintf(buf, sizeof(buf), "%.3f", 100  * (hitB[1] / (double)allB[1]));
	printf("         Total read command: %11u\n", allB[1]);
	printf("     Total read hit command: %11u\n", hitB[1]);
	printf("              Read hit rate: [%8s%%]\n", buf);
}
#elif defined(DRAM_BUS_SINGLE)

#define gl_L2cInfoInitB(args...)
#define gl_L2cInfoResultB(args...)
static int gl_IsL2cDisabled(void)
{
	/* Global L2C switch.  */
	if (((HWREG_R(cbus_wdog_ctl) >> 8) & 0x01) != 0x01) {
		return 1;
	}

	if (((HWREG_R(l2c0_type_config) & 0x01) == 0)
		&& ((HWREG_R(l2c0_chk_option) & 0x03) == 0)) {
		return 0;
	}

	return 1;
}

void GL_L2cSizeB(unsigned int size)
{
}

void GL_L2cInvalidateB(void)
{
}

inline unsigned int GL_L2cInfoBWCmds(void)
{
	return 0;
}

inline unsigned int GL_L2cInfoBWHits(void)
{
	return 0;
}

inline unsigned int GL_L2cInfoBRCmds(void)
{
	return 0;
}

inline unsigned int GL_L2cInfoBRHits(void)
{
	return 0;
}

#else
#error "Please define correct DRAM_BUS!"
#endif

int GL_L2cInfoInit(void)
{
	if (1 == gl_IsL2cDisabled()) {
		/* L2CA or L2CB is disabled. */
		return 0;
	}

	/* Reset hit-rate profiling counter */
	HWREG_W(l2c0_hit_prof[0], 0);	// 41.1
	HWREG_W(l2c0_int, 0);			// 40.4

	/* Set counting boundaries */
	HWREG_W(l2c0_hit_prof[1], 0xFFFFFFFF);	// 41.2

	/* Enable specified master profiling for read/write command in MCB */
	HWREG_W(l2c0_hit_prof[0], 5);	// 41.1

	gl_L2cInfoInitB();

	return 1;
}

inline unsigned int GL_L2cInfoAWCmds(void)
{
	return HWREG_R(l2c0_hit_prof_result[0]);
}

inline unsigned int GL_L2cInfoAWHits(void)
{
	return HWREG_R(l2c0_hit_prof_result[1]);
}

inline unsigned int GL_L2cInfoARCmds(void)
{
	return HWREG_R(l2c0_hit_prof_result[2]);
}

inline unsigned int GL_L2cInfoARHits(void)
{
	return HWREG_R(l2c0_hit_prof_result[3]);
}

void GL_L2cInfoResult(void)
{
	unsigned int allA[2], hitA[2];
	char buf[8];

	if (1 == gl_IsL2cDisabled()) {
		printf("L2C is disabled.\n");
		return;
	}

	/* To gather profiling results as possible as simultaneously */
	/*
	 * FIXME: back-to-back accesses to the same hw module causes some errors, so
	 * we add nop to avoid them temporarily.
	 */
	hitA[0] = GL_L2cInfoAWHits();
	TEMP_DELAY();
	allA[0] = GL_L2cInfoAWCmds() | 0x01;	// prevent zero value
	TEMP_DELAY();
	hitA[1] = GL_L2cInfoARHits();
	TEMP_DELAY();
	allA[1] = GL_L2cInfoARCmds() | 0x01;
	TEMP_DELAY();

	printf("L2C size A = %d KByte\n", l2c_size_config[(HWREG_R(l2c0_type_config) >> 4) & 0x07]);
	snprintf(buf, sizeof(buf), "%.3f", 100 * (hitA[0] / (double)allA[0]));
	printf("========== DRAM A ==========\n");
	printf("        Total write command: %11u\n", allA[0]);
	printf("    Total write hit command: %11u\n", hitA[0]);
	printf("             Write hit rate: [%8s%%]\n", buf);
	snprintf(buf, sizeof(buf), "%.3f", 100 * (hitA[1] / (double)allA[1]));
	printf("         Total read command: %11u\n", allA[1]);
	printf("     Total read hit command: %11u\n", hitA[1]);
	printf("              Read hit rate: [%8s%%]\n", buf);

	gl_L2cInfoResultB();
}

