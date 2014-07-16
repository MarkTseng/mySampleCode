#include <gsl.h>
#include "gl_common.h"
#include <gsl/bget_malloc.h>
#include <gsl/lambo86_hwop.h>

#define CFG_SRC_START		(0 << 14)
#define CFG_SRC_END			(1 << 14)
#define CFG_DST				(2 << 14)
#define CFG_LITTLE_ENDIAN	(1 << 4)
#define FREE_MODE_SPEEDUP	(1 << 7)

/*
	Endian:
		In the dma free read/write mode, 1: little endian, 0: big endian.
		In the other mode, this option is not work.
*/
#define GSL_DMA_MODE_CLEAR			0x0
#define GSL_DMA_MODE_GLOBAL_READ	((0 << 5) | CFG_LITTLE_ENDIAN | 0x07)
#define GSL_DMA_MODE_GLOBAL_WRITE	((1 << 5) | CFG_LITTLE_ENDIAN | 0x07)
#define GSL_DMA_MODE_MOVE			(CFG_LITTLE_ENDIAN | 0x0C)
#define GSL_DMA_MODE_FREE_READ		(FREE_MODE_SPEEDUP | CFG_LITTLE_ENDIAN | 0x0E)
#define GSL_DMA_MODE_FREE_WRITE		(FREE_MODE_SPEEDUP | CFG_LITTLE_ENDIAN | 0x0F)

//#define GL_CBDMA_DEBUG
#ifdef GL_CBDMA_DEBUG
#define dma_printf(fmt, arg...)		printf("%s:%d "fmt, __FILE__, __LINE__, ##arg)
#else
#define dma_printf(...)
#endif
#define dma_printf_err(fmt, arg...)	printf("%s:%d "fmt, __FILE__, __LINE__, ##arg)

#define GL_SOFT_DMA

static GL_Mutex_t sDmaMutex;
#ifndef GL_SOFT_DMA
static void CB_DMA_Open(void);
static void CB_DMA_Close(void);
#endif

GL_Status_t GL_DmaInit(void)
{
	if (GL_MutexCreate("sDmaMutex", &sDmaMutex) == GL_FAILURE) {
		dma_printf("Cannot create dma mutex!!\n");
		return GL_FAILURE;
	}
#ifndef GL_SOFT_DMA
	CB_DMA_Open();
#endif
	return GL_SUCCESS;
}

void GL_DmaUninit(void)
{
	if (GL_MutexDelete(sDmaMutex) == GL_FAILURE) {
		dma_printf("Cannot create dma mutex!!\n");
	}
#ifndef GL_SOFT_DMA
	CB_DMA_Close();
#endif
}

void GL_DmaLock(void)
{
	GL_MutexLock(sDmaMutex);
}

void GL_DmaUnlock(void)
{
	GL_MutexUnlock(sDmaMutex);
}


// Software implementation of DMA
#ifdef GL_SOFT_DMA
GL_Status_t GL_DmaCopy(UINT32 dst, UINT32 src, UINT32 len, UINT32 option)
{
	GL_MutexLock(sDmaMutex);
	memmove((void *)dst, (void *)src, len);
	GL_MutexUnlock(sDmaMutex);
	return GL_SUCCESS;
}

GL_Status_t GL_DmaSet(UINT32 dst, UINT32 value, UINT32 len, UINT32 option)
{
	UINT32 tmp;
	UINT32 w_len;
	UINT32 *w_p;

	GL_MutexLock(sDmaMutex);

	w_len = 0;

	if (0 == (tmp = dst % 4)) {
		w_p = (UINT32 *)dst;
	} else {
		UINT8 *p;

		p = (UINT8 *)dst;
		while (tmp < 4) {
			//*p = ((value << (tmp * 8)) & 0xFF000000) >> 24;
			*p = ((value >> (tmp * 8)) & 0xFF);
			tmp++;
			p++;
			w_len++;
		}
		w_p = (UINT32 *)p;
	}

	while ((w_len + 4) <= len) {
		*w_p = value;
		w_p++;
		w_len += 4;
	}

	if (0 != (tmp = (dst + len) % 4)) {
		UINT8 *p;
		UINT32 i;

		p = (UINT8 *)w_p;
		for (i = 0; i < tmp; i++) {
			//*p = ((value << (i * 8)) & 0xFF000000) >> 24;
			*p = ((value >> (i * 8)) & 0xFF);
			p++;
			w_len++;
		}
	}

	if (w_len != len) {
		printf("%s assertion failed! %d %d\n", __FUNCTION__, w_len, len);
		while (1) {
			asm volatile ("break\n");
		}
	}

	GL_MutexUnlock(sDmaMutex);

	return GL_SUCCESS;
}
#else
/******************************************************************************

Copyright(c) 2005~2009 SunMedia technology (Chengdu) co. Ltd.All right reserved!

 ******************************************************************************
  File Name     : Mod_CbDma.c
  Version       : Initial Draft
  Author        : sjhuang
  Created       : 2009/8/24
  Last Modified :
  Description   :

1) normal test
    1> READ
    2> WRITE
    3> READ_WRITE
    4> MEMCPY
    5> MEMSET
    And include SINGLE mbus and TWO_MBUS mbus test ,
    each bus test include uncache<==>uncache, uncache<==>cache,  cache<==>uncache, cache<==>cache,
    Open test by TEST_NORMAL_SINGLE_MBUS_CROSS and TEST_NORMAL_TWO_MBUS_CROSS

2) ABNORMAL
    You could add some code in "Trouble_func_xx",which is called by polling mode when waitting DMA DONE.
    Now include PIO R/W, memcpy
    Defined by TEST_ABNORMAL.

3) OVERLAP
    For the  memcpy  which is overlap, now it's processed by  SW.
    Defined by TEST_OVERLAP

4) time cost vs soft memcpy
    Use CPU time counter CP9, 0,  to record SW and HW time cost
    Defined by TEST_TIME

  Function List :
  main function is
                   CB_DMA_Test(void)

 History Information Description
 Date                         Author                         Modification
 2009/8/24                    sjhuang                        Created function

******************************************************************************/

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/

#if (IC_VER == QAE377)
/*
 * FIXME: (Mantis_PD2 0004533)
 *   Due to the design of CB_DMA, we set CB_DMA port to SBUS protocol first.
 *   Second, we need to wait at least 2XX T for SDCTRL to receive MSI's data
 *   after MSI done. And then, reset CB_DMA to set CB_DMA's MDataValid be zero.
 *
 *   The following steps are the workaround method to use CB_DMA:
 *     0. set CB_DMA port to SBUS protocol (once)
 *     1. wait till CB_DMA status shows done
 *     2. init new CB_DMA transaction
 *     3. wait till CB_DMA status shows done
 *     4. wait 300T
 *     5. reset CB_DMA
 */
#define WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
#endif

#ifdef WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
/*
 * FIXME: We don't care about whether CPU clock is single or double
 * of system clock. Worst case is to wait for 300 * 2 ticks.
 */
#define WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY_WAIT_MSI()	\
	do {												\
		unsigned int t1, t2;							\
		asm volatile ("mfc0	%0, $9;" : "=r"(t1));		\
		while (1) {										\
			asm volatile ("mfc0	%0, $9;" : "=r"(t2));	\
			if ((t1 + 300) < t2) {						\
				break;									\
			}											\
		}												\
	} while (0)
#endif


/* Open this define to run test program.  */
//#define HAVE_CB_DMA_TEST

#define CB_DMA_USE_INTERRUPT
//#define CB_DMA_USE_INTERRUPT_DEFAULT //3 If you only need interrupt test,open it
//#define CB_DMA_BIG_ENDIAN

//Following is spec
#define CB_DMA_SIZE_MAIN_SHIFT				13	// 8192
#define CB_DMA_MAIN_RAM_SIZE				(1 << CB_DMA_SIZE_MAIN_SHIFT)	// 0x2000
#define CB_DMA_SIZE_CP_SET_MAX_SHIFT		24 //16mb
#define CB_DMA_MAX_LEN						(1 << CB_DMA_SIZE_CP_SET_MAX_SHIFT)	 // 16MB
//#define CN_DMA_OVERLAP_MOVE_MINI			32
#define CB_DMA_BASE_ADR						0xBFFC0000
#define CB_DMA_END_ADR						(CB_DMA_BASE_ADR + CB_DMA_MAIN_RAM_SIZE)
#define CB_DMA_ADDR_ERROR(src, dst)														\
	do {																				\
		dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	\
		dma_printf_err("~                 DMA Address Error !!!               ~\n");	\
		dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	\
		dma_printf_err("~src addr = 0x%08X, dst addr = 0x%08X ~\n",						\
					   (UINT32)(src), (UINT32)(dst));									\
		dma_printf_err("~ caller = 0x%08X  ~\n", (UINT32)__builtin_return_address(0));	\
		asm volatile("break;");															\
	} while (0)

#define CB_DMA_SIZE_ERROR(len)															\
	do {																				\
		dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	\
		dma_printf_err("~                 DMA Length Error !!!                ~\n");	\
		dma_printf_err("~          The length should be word-align!!          ~\n");	\
		dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	\
		dma_printf_err("~      length = %d       ~\n", (UINT32)(len));					\
		dma_printf_err("~ caller = 0x%08X  ~\n", (UINT32)__builtin_return_address(0));	\
		asm volatile("break;");															\
	} while (0)

#define CB_DMA_ADDR_SIZE_CHECK_CACHELINE(src, dst, len)										\
	do {																					\
		if ((((dst) & (HAL_DCACHE_LINE_SIZE - 1)) != 0) ||									\
			(((src) & (HAL_DCACHE_LINE_SIZE - 1)) != 0) ||									\
			(((len) & (HAL_DCACHE_LINE_SIZE - 1)) != 0)) {									\
			dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	\
			dma_printf_err("~  DMA Address/Length should be cacheline-aligned!!!  ~\n");	\
			dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");	\
			dma_printf_err("~ src = 0x%08X, dst = 0x%08X, length = %d ~\n",					\
						(UINT32)(src), (UINT32)(dst), (UINT32)(len));						\
			dma_printf_err("~ caller = 0x%08X  ~\n", (UINT32)__builtin_return_address(0));	\
			asm volatile("break;");															\
		}																					\
	} while (0)

#define CB_DMA_LOCK()						GL_MutexLock(sDmaMutex)
#define CB_DMA_UNLOCK()						GL_MutexUnlock(sDmaMutex)
#define CB_DMA_START()						HWREG_1(dma_config, (1 << 8))
#define CB_DMA_IS_DOING()					(HWREG_R(dma_config) & (1 << 8))
#define CB_DMA_WAIT_DONE()					\
	do {									\
		while (CB_DMA_IS_DOING()) {			\
			dma_printf("Wait dma done\n");	\
		}									\
	} while (0)
#define CB_SET_MODE(mode)					HWREG_W(dma_config, (HWREG_R(dma_config) & (~3)) | mode)
#define CB_SET_DMA_LENGTH(length)			HWREG_W(dma_length, length)
#define CB_SET_DMA_SRC_ADDR(ADDR)			HWREG_W(dma_src, ADDR)
#define CB_SET_DMA_DST_ADDR(ADDR)			HWREG_W(dma_dst, ADDR)
#define CB_GET_DMA_LENGTH()					HWREG_R(dma_length)
#define CB_GET_MODE()						(HWREG_R(dma_config) & 3)
#define CB_FILL_MEMSET_VALUE_8(bValue)		\
	HWREG_W(dma_memset_value, ((bValue << 24) | (bValue << 16) | (bValue << 8) | bValue))
#define CB_FILL_MEMSET_VALUE_32B(IValue)	HWREG_W(dma_memset_value, IValue)
#define CB_FILL_MEMSET_VALUE_32L(IValue)			\
	do {											\
		HWREG_W(dma_memset_value,					\
				(((IValue & 0xFF) << 24) |			\
				((IValue & 0xFF00) << 8) |			\
				((IValue & 0xFF0000) >> 8) |		\
				((IValue & 0xFF000000) >> 24)));	\
	} while (0)

#define CB_GET_INT_FLAG()					HWREG_R(dma_int_flag)
#define CB_CLEAR_INT_FLAG(x)				HWREG_1(dma_int_flag, 1 << (x))
#define CB_DMA_ENABLE_INT(x)				HWREG_1(dma_int_en, 1 << (x))
#define CB_DMA_DISABLE_INT(x)				HWREG_0(dma_int_en, ~(1 << (x)))
#define CB_DMA_INTERRUPT_NUM				CYGNUM_HAL_INTERRUPT_CBDMA
#define FLAG_BIT_DMA_DONE					0x00000001

enum CB_DMA_Mode {
	DMA_MODE_MEMSET = 0,
	DMA_MODE_SRAM_TO_DRAM,
	DMA_MODE_DRAM_TO_SRAM,
	DMA_MODE_DRAM_TO_DRAM,
};

enum CB_DMA_Int {
	DMA_DONE = 0,
	ADDR_CONFLICT,
	CBSRAM_OVFLOW,
	DMA_SRAM_OVFLOW,
#if (IC_VER == QAE377)
	DMA_SDRAM_SPAN,
	DMA_SDRAM_OVFLOW,
#elif ((IC_VER == QCE491) || (IC_VER == QCE483))
	DMA_SDRAMA_OVFLOW,
	DMA_SDRAMB_OVFLOW,
#else
#error "Please define correct IC_VER!"
#endif
	COPY_ADDR_OVLAP,
};

const char *dma_error_msg[] = {
	"DMA DONE",
	"ADDR CONFLICT",
	"CBSRAM OVERFLOW",
	"SRAM OVERFLOW",
#if (IC_VER == QAE377)
	"SDRAM SPAN",
	"SDRAM OVERFLOW",
#elif ((IC_VER == QCE491) || (IC_VER == QCE483))
	"SDRAMA OVERFLOW",
	"SDRAMB OVERFLOW",
#else
#error "Please define correct IC_VER!"
#endif
	"ADDR OVERLAP"
};

enum {
	CBDMA_DRAM_SIZE_8MB   = 1 << 0,
	CBDMA_DRAM_SIZE_16MB  = 1 << 1,
	CBDMA_DRAM_SIZE_32MB  = 1 << 2,
	CBDMA_DRAM_SIZE_64MB  = 1 << 3,
	CBDMA_DRAM_SIZE_128MB = 1 << 4,
	CBDMA_DRAM_SIZE_256MB = 1 << 5,
	CBDMA_DRAM_SIZE_512MB = 1 << 6,
	CBDMA_DRAM_SIZE_1GB   = 1 << 7,
};

static GL_Semaphore_t dma_semaphore;

static int CB_DMA_MemToDma(char *pDst, char *pSrc, size_t uiLen);
static int CB_DMA_DmaToMem(char *pDst, char *pSrc, size_t uiLen);
static int CB_DMA_MemToMem(char *pDst, char *pSrc, size_t uiLen);
static int CB_DMA_MemSetU32(char *pDst, UINT32 uiValue, UINT8 bStartValueOffset, size_t uiLen);

#ifdef HAVE_CB_DMA_TEST
/*
 * We do not use GSL API here, because it may fail when doing cacheline-algined
 * check. However, CB DMA can support for byte-align operations.
 */
static inline void src_cache_hit_wb_inv(unsigned int addr, size_t n)
{
	unsigned int eaddr = addr + n;

	for (addr &= ~(HAL_DCACHE_LINE_SIZE - 1); addr < eaddr; addr += HAL_DCACHE_LINE_SIZE) {
		GL_DataCacheFlushAndInvalidate(addr, HAL_DCACHE_LINE_SIZE);
	}
}

static inline void dst_cache_hit_inv(unsigned int addr, size_t n)
{
	unsigned int eaddr = addr + n;

	for (addr &= ~(HAL_DCACHE_LINE_SIZE - 1); addr < eaddr; addr += HAL_DCACHE_LINE_SIZE) {
		GL_DataCacheInvalidate(addr, HAL_DCACHE_LINE_SIZE);
	}
}
#endif

inline unsigned int mem_src_cpu2bus(unsigned int addr, size_t n)
{
	if (CHECK_IF_ADDR_IN_DRAM_A_CACHED(addr)) { /* cached Luma */
#ifdef HAVE_CB_DMA_TEST
		src_cache_hit_wb_inv(addr, n);
#else
		GL_DataCacheFlushAndInvalidate((void *)addr, n);
#endif
		addr = (unsigned int)GL_Phys2MemA(GL_CACHE_TO_PHYSICAL((void *)addr));
		GL_LumaBarrier();
	} else if (CHECK_IF_ADDR_IN_DRAM_B_CACHED(addr)) { /* cached Chroma */
#ifdef HAVE_CB_DMA_TEST
		src_cache_hit_wb_inv(addr, n);
#else
		GL_DataCacheFlushAndInvalidate((void *)addr, n);
#endif
		addr = (unsigned int)GL_Phys2MemB(GL_CACHE_TO_PHYSICAL((void *)addr)) + 0x40000000;
		GL_ChromaBarrier();
	} else if (CHECK_IF_ADDR_IN_DRAM_A_UNCACHED(addr)) { /* Luma */
		addr = (unsigned int)GL_Phys2MemA(GL_UNCACHE_TO_PHYSICAL((void *)addr));
	} else if (CHECK_IF_ADDR_IN_DRAM_B_UNCACHED(addr)) { /* Chroma */
		addr = (unsigned int)GL_Phys2MemB(GL_UNCACHE_TO_PHYSICAL((void *)addr)) + 0x40000000;
	} else {
		dma_printf_err("Illegal address!\n");
	}

	return addr;
}

inline unsigned int mem_dst_cpu2bus(unsigned int addr, size_t n)
{
	if (CHECK_IF_ADDR_IN_DRAM_A_CACHED(addr)) { /*cached Luma */
#ifdef HAVE_CB_DMA_TEST
		dst_cache_hit_inv(addr, n);
#else
		GL_DataCacheInvalidate((void *)addr, n);
#endif
		addr = (unsigned int)GL_Phys2MemA(GL_CACHE_TO_PHYSICAL((void *)addr));
	} else if (CHECK_IF_ADDR_IN_DRAM_B_CACHED(addr)) { /*cached Chroma */
#ifdef HAVE_CB_DMA_TEST
		dst_cache_hit_inv(addr, n);
#else
		GL_DataCacheInvalidate((void *)addr, n);
#endif
		addr = (unsigned int)GL_Phys2MemB(GL_CACHE_TO_PHYSICAL((void *)addr)) + 0x40000000;
	} else if (CHECK_IF_ADDR_IN_DRAM_A_UNCACHED(addr)) { /* Luma */
		addr = (unsigned int)GL_Phys2MemA(GL_UNCACHE_TO_PHYSICAL((void *)addr));
	} else if (CHECK_IF_ADDR_IN_DRAM_B_UNCACHED(addr)) { /* Chroma */
		addr = (unsigned int)GL_Phys2MemB(GL_UNCACHE_TO_PHYSICAL((void *)addr)) + 0x40000000;
	} else {
		dma_printf_err("Illegal address!\n");
	}

	return addr;
}

int GL_DmaPeek(void)
{
	return CB_DMA_IS_DOING();
}

static int CB_DMA_wait_done(void)
{
#ifdef CB_DMA_USE_INTERRUPT
	if (GL_SemaphoreTake(dma_semaphore, 2000) != GL_SUCCESS) {
		printf("%s:%d: dma operation may not be done!!!\n", __FUNCTION__, __LINE__);
	}
#ifdef WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
	WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY_WAIT_MSI();
	/* Rest CB_DMA.  */
	HWREG_W(reset[0], (HWREG_R(reset[0]) | (1 << 9)));
	HWREG_W(reset[0], (HWREG_R(reset[0]) & (~(1 << 9))));
#endif

#else
	while (0 != CB_DMA_IS_DOING()) {
		usleep(1000);
	}
#endif
#ifdef WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
	WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY_WAIT_MSI();
#endif

	return 0;
}

static int do_dma_copy(char *pDst, char *pSrc, size_t uiLen, UINT32 mode)
{
	UINT32 src_addr, dst_addr;

	src_addr = mem_src_cpu2bus((unsigned int)pSrc, uiLen);
	dst_addr = mem_dst_cpu2bus((unsigned int)pDst, uiLen);
	CB_SET_MODE(mode);
	CB_SET_DMA_LENGTH(uiLen);
	CB_SET_DMA_DST_ADDR(dst_addr);
	CB_SET_DMA_SRC_ADDR(src_addr);
#if 0
    dma_printf("dst_addr=0x%08X src_addr=0x%08X\n",dst_addr, src_addr);
	dma_printf("HWREG_R(dma_config)      = 0x%08X\n", HWREG_R(dma_config));
	dma_printf("HWREG_R(dma_length)      = 0x%08X\n", CB_GET_DMA_LENGTH());
	dma_printf("HWREG_R(dma_src)         = 0x%08X\n", HWREG_R(dma_src));
	dma_printf("HWREG_R(dma_dst)         = 0x%08X\n", HWREG_R(dma_dst));
	dma_printf("HWREG_R(dma_memset_value)= 0x%08X\n", HWREG_R(dma_memset_value));
#endif
	CB_DMA_START();

	return 0;
}

static inline void do_dma_memset(UINT32 dst, UINT32 val, UINT32 off, UINT32 len)
{
	dst = mem_dst_cpu2bus(dst, len);
	CB_SET_MODE(DMA_MODE_MEMSET);
#ifdef CB_DMA_BIG_ENDIAN
	CB_FILL_MEMSET_VALUE_32B(val);
#else
	CB_FILL_MEMSET_VALUE_32L(val);
#endif
	CB_SET_DMA_LENGTH(len);
	CB_SET_DMA_DST_ADDR(dst);
	CB_SET_DMA_SRC_ADDR(off);

	CB_DMA_START();
}

GL_Status_t GL_DmaCopy(UINT32 dst, UINT32 src, UINT32 len, UINT32 option)
{
	GL_Status_t ret;
	enum {					/* Not support sram to sram mode.  */
		SRAM_SRC = 1,		/* sram to dram mode.  */
		SRAM_DST = 2,		/* dram to sram mode.  */
		NONBLOCKING = 4,	/* use non-blocking method.  */
	};
	int flag = 0;
	char *s;
	char *d;

	GL_CHECK_BLOCK_IN_INTR();

	if ((option & GL_DMA_UNCACHED) > 0) {
		/* Check if they are uncached address.  */
		if (!CHECK_IF_ADDR_IN_DRAM_UNCACHED(src)
			|| !CHECK_IF_ADDR_IN_DRAM_UNCACHED(dst)) {
			CB_DMA_ADDR_ERROR(src, dst);
			return GL_ERR_INVALID_PARAM;
		}
	} else {
		/* Check if they are cached address.  */
		if (!CHECK_IF_ADDR_IN_DRAM_CACHED(src)
			|| !CHECK_IF_ADDR_IN_DRAM_CACHED(dst)) {
			/* Check if cache-aligned address and size.  */
			CB_DMA_ADDR_SIZE_CHECK_CACHELINE(src, dst, len);
		}
	}

	if (((unsigned int)src >= CB_DMA_BASE_ADR) && ((unsigned int)src < CB_DMA_END_ADR)) {
		flag |= SRAM_SRC;
	}
	if (((unsigned int)dst >= CB_DMA_BASE_ADR) && ((unsigned int)dst < CB_DMA_END_ADR)) {
		flag |= SRAM_DST;
	}

	if ((option & GL_DMA_NONBLOCK) > 0) {
		if (len > CB_DMA_MAX_LEN) {
			dma_printf_err("Non-blocking gsl dma method does NOT support the length (%u) larger than %u bytes!\n",
						   len, CB_DMA_MAX_LEN);
			return GL_ERR_INVALID_PARAM;
		}
		flag |= NONBLOCKING;
		CB_DMA_LOCK();
		CB_DMA_WAIT_DONE();
		CB_DMA_DISABLE_INT(DMA_DONE);
	}

	s = (char *)src;
	d = (char *)dst;
	switch (flag) {
	case 0:
		ret = CB_DMA_MemToMem(d, s, len);
		break;
	case SRAM_SRC:
		ret = CB_DMA_DmaToMem(d, s, len);
		break;
	case SRAM_DST:
		ret = CB_DMA_MemToDma(d, s, len);
		break;
	/* Non-blocking methods.  */
	case NONBLOCKING:
		ret = do_dma_copy(d, s, len, DMA_MODE_DRAM_TO_DRAM);
		break;
	case (NONBLOCKING | SRAM_SRC):
		ret = do_dma_copy(d, s, len, DMA_MODE_SRAM_TO_DRAM);
		break;
	case (NONBLOCKING | SRAM_DST):
		ret = do_dma_copy(d, s, len, DMA_MODE_DRAM_TO_SRAM);
		break;
	default:
		dma_printf("Not support for SRAM to SRAM copy.\n");
		return GL_ERR_INVALID_PARAM;
	}

	if (1 == ret) {
		return GL_FAILURE;
	}

	return GL_SUCCESS;
}

GL_Status_t GL_DmaSet(UINT32 dst, UINT32 value, UINT32 len, UINT32 option)
{
	int ret;

	GL_CHECK_BLOCK_IN_INTR();

	if ((option & GL_DMA_UNCACHED) > 0) {
		if (!CHECK_IF_ADDR_IN_DRAM_UNCACHED(dst)) {
			CB_DMA_ADDR_ERROR(0, dst);
			return GL_ERR_INVALID_PARAM;
		}
		if ((len & 0x03) != 0) {	/* 4-byte alignement check.  */
			CB_DMA_SIZE_ERROR(len);
		}
	} else {
		CB_DMA_ADDR_SIZE_CHECK_CACHELINE(0, dst, len);
	}

	if ((option & GL_DMA_NONBLOCK) > 0) {
		CB_DMA_LOCK();
		CB_DMA_WAIT_DONE();
		CB_DMA_DISABLE_INT(DMA_DONE);
		do_dma_memset(dst, value, 0, len);
		ret = 0;
	} else {
		ret = CB_DMA_MemSetU32((char *)dst, value, 0, len);
	}
	if (1 == ret) {
		return GL_FAILURE;
	}

	return GL_SUCCESS;
}

GL_Status_t GL_DmaClear(void)
{
	if (CB_DMA_IS_DOING()) {
		return GL_FAILURE;
	}
#ifdef WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
	WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY_WAIT_MSI();

	/* Rest CB_DMA.  */
	HWREG_W(reset[0], (HWREG_R(reset[0]) | (1 << 9)));
	HWREG_W(reset[0], (HWREG_R(reset[0]) & (~(1 << 9))));
	GL_RegisterFileBarrier();
#endif
	CB_DMA_UNLOCK();

	return GL_SUCCESS;
}

void GL_DmaAbort(void)
{
#ifdef WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
	WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY_WAIT_MSI();
#endif
	HWREG_W(reset[0], (HWREG_R(reset[0]) | (1 << 9)));
	HWREG_W(reset[0], (HWREG_R(reset[0]) & (~(1 << 9))));
	GL_RegisterFileBarrier();
}

#if 0
char qqBuffer[1024];

void GL_DmaSetTest(void)
{
	dma_printf("Start testing ...\n");
	memset(qqBuffer, 0, 1024);
	GL_DmaSet((UINT32)(qqBuffer + 3), 0xF1F2F3F4, 8);
	UINT8 *p = (UINT8 *)qqBuffer;
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("0x%02X\n", *p++);
	dma_printf("End testing ...\n");
	asm volatile ("wait");
}
#endif

static void check_dma_error(UINT32 flags)
{
	UINT32 error;

	if (1 == flags) {
		/* No need to check if it is DMA_DONE singal.  */
		return;
	}

	asm volatile("clz %0, %1;" : "=r"(error) : "r"(flags));
	error = 32 - error - 1;

	dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	dma_printf_err("~                     DMA Error !!!                   ~\n");
	dma_printf_err("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	if (error < (sizeof(dma_error_msg) / sizeof(dma_error_msg[1]))) {
		dma_printf_err("cause: %s. dma_int_flag = 0x%08X\n", dma_error_msg[error], flags);
	} else {
		dma_printf_err("Unknown dma error occurs! dma_int_flag = 0x%08X\n", flags);
	}
	dma_printf_err("dma_config = 0x%08X\n", HWREG_R(dma_config));
	dma_printf_err("dma_length = 0x%08X\n", HWREG_R(dma_length));
	dma_printf_err("dma_src    = 0x%08X\n", HWREG_R(dma_src));
	dma_printf_err("dma_dst    = 0x%08X\n", HWREG_R(dma_dst));
	dma_printf_err("dma_int_en = 0x%08X\n", HWREG_R(dma_int_en));
	dma_printf_err("memset_val = 0x%08X\n", HWREG_R(dma_memset_value));

	asm volatile("break;");
}

#ifndef HAVE_CB_DMA_TEST
GL_IsrReturn_t cb_dma_done_isr(void *pParam)
{
	check_dma_error(CB_GET_INT_FLAG());
	return (GL_ISR_HANDLED | GL_ISR_CALL_DSR);
}

GL_DsrReturn_t cb_dma_done_dsr(void *pParam)
{
	CB_CLEAR_INT_FLAG(DMA_DONE);
	GL_SemaphoreGive(dma_semaphore);

	return GL_DSR_HANDLED;
}

#else
cyg_uint32 cb_dma_done_isr(cyg_vector_t vector, cyg_addrword_t data)
{
	check_dma_error(CB_GET_INT_FLAG());
	cyg_interrupt_mask(vector);
	cyg_interrupt_acknowledge(vector);
	return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);
}

void cb_dma_done_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	GL_SemaphoreGive(dma_semaphore);
	CB_CLEAR_INT_FLAG(DMA_DONE);
#ifdef HAVE_CB_DMA_TEST
	cyg_interrupt_unmask(vector);
#endif
}
#endif

static void CB_DMA_Open(void)
{
	dma_printf("Current cbdma dma_version is %x \n", HWREG_R(dma_version));

#ifdef WORKAROUND_CB_DMA_ALWAYS_SHOWS_BUSY
	/* Set CB_DMA port to SBUS protocol.  */
	HWREG_W(g30_reserved[17], 0xfffffff7);
	HWREG_W(g43_reserved[17], 0xfffffff7);
#endif

#ifdef CB_DMA_USE_INTERRUPT
#ifdef HAVE_CB_DMA_TEST
	static cyg_handle_t   Cbdma_handle;
	static cyg_interrupt   Cbdma_intr;

	cyg_interrupt_create(CYGNUM_HAL_INTERRUPT_CBDMA,
						 0,
						 0,
						 &cb_dma_done_isr,
						 &cb_dma_done_dsr,
						 &Cbdma_handle,
						 &Cbdma_intr);
	cyg_interrupt_attach(Cbdma_handle);
	cyg_interrupt_unmask(CYGNUM_HAL_INTERRUPT_CBDMA);
#else
	GL_IntrAttach(cbdma_handle);
	GL_IntrEnable(cbdma_handle);
#endif
	CB_DMA_ENABLE_INT(DMA_DONE);
	CB_DMA_ENABLE_INT(ADDR_CONFLICT);
	CB_DMA_ENABLE_INT(CBSRAM_OVFLOW);
	CB_DMA_ENABLE_INT(DMA_SRAM_OVFLOW);
#if (IC_VER == QAE377)
	CB_DMA_ENABLE_INT(DMA_SDRAM_SPAN);
	CB_DMA_ENABLE_INT(DMA_SDRAM_OVFLOW);
#elif ((IC_VER == QCE491) || (IC_VER == QCE483))
#if defined(IC_USE_DRAM_SIZE_256MB) && defined(IC_SUPPORT_DRAM_A_16_H)
	HWREG_W(dma_sdram_size_config, CBDMA_DRAM_SIZE_256MB);
#elif defined(IC_USE_DRAM_SIZE_128MB) && defined(IC_SUPPORT_DRAM_A_16_H)
	HWREG_W(dma_sdram_size_config, CBDMA_DRAM_SIZE_128MB);
#elif defined(IC_USE_DRAM_SIZE_128MB) && defined(IC_SUPPORT_DRAM_A_32)
	HWREG_W(dma_sdram_size_config, CBDMA_DRAM_SIZE_256MB);
#elif defined(IC_USE_DRAM_SIZE_64MB)
	HWREG_W(dma_sdram_size_config, CBDMA_DRAM_SIZE_64MB);
#else
#error "Please define the corresponding setting!"
#endif // DRAM_SIZE and DRAM_BUS

#if (IC_VER == QCE483)
	HWREG_1(dma_sdram_size_config, CBDMA_DRAM_SIZE_128MB << 8);
#else
	HWREG_1(dma_sdram_size_config, 0 << 8);
#endif // (IC_VER == QCE483)
	CB_DMA_ENABLE_INT(DMA_SDRAMA_OVFLOW);
	CB_DMA_ENABLE_INT(DMA_SDRAMB_OVFLOW);
#else
#error "Please define correct IC_VER!"
#endif
	CB_DMA_ENABLE_INT(COPY_ADDR_OVLAP);

#endif	// CB_DMA_USE_INTERRUPT

	if (GL_SUCCESS != GL_SemaphoreCreate("GL_DMA_SEM", 1, 0, &dma_semaphore)) {
		dma_printf_err("Critical error: cannot create DMA semaphore!\n");
	}
}

static void CB_DMA_Close(void)
{
	dma_printf("cbdma uninit ...\n");

	if (GL_SemaphoreDelete(dma_semaphore)) {
		dma_printf_err("Error: cannot delete DMA semaphore!\n");
	}
#ifdef CB_DMA_USE_INTERRUPT
#ifdef HAVE_CB_DMA_TEST
	cyg_interrupt_mask(CYGNUM_HAL_INTERRUPT_CBDMA);
	cyg_interrupt_detach(Cbdma_handle);
#else
	GL_IntrDisable(cbdma_handle);
	GL_IntrDetach(cbdma_handle);
#endif
#endif
}

static int CB_DMA_MemToDma(char *pDst, char *pSrc, size_t uiLen)
{
	if ((((UINT32)pDst - CB_DMA_BASE_ADR) + uiLen) > CB_DMA_MAIN_RAM_SIZE) {
		dma_printf("Dma len fail: len=%d\n", uiLen);
		return 1;
	}

	CB_DMA_LOCK();
	CB_DMA_WAIT_DONE();
	CB_DMA_ENABLE_INT(DMA_DONE);
	do_dma_copy(pDst, pSrc, uiLen, DMA_MODE_DRAM_TO_SRAM);
	if (CB_DMA_wait_done()) {
		CB_DMA_UNLOCK();
		return 1;
	}
	CB_DMA_UNLOCK();

	return 0;
}

static int CB_DMA_DmaToMem(char *pDst, char *pSrc, size_t uiLen)
{
	if ((((UINT32)pSrc - CB_DMA_BASE_ADR) + uiLen) > CB_DMA_MAIN_RAM_SIZE) {
		dma_printf("Dma len fail: len=%d\n", uiLen);
		return 1;
	}

	CB_DMA_LOCK();
	CB_DMA_WAIT_DONE();
	CB_DMA_ENABLE_INT(DMA_DONE);
	do_dma_copy(pDst, pSrc, uiLen, DMA_MODE_SRAM_TO_DRAM);
	if (CB_DMA_wait_done()) {
		CB_DMA_UNLOCK();
		return 1;
	}
	CB_DMA_UNLOCK();

	return 0;
}

static int CB_DMA_MemToMem(char *pDst, char *pSrc, size_t uiLen)
{
	UINT32 uiTimes;
	UINT32 uiCpLen;
	UINT32 uiLeft;
	UINT32 uiIter;
	char *pSrc1 = (char *)pSrc;
	char *pDst1 = (char *)pDst;

	if (pDst == pSrc) {
		return 0;
	}

	uiCpLen = CB_DMA_MAX_LEN;
	uiTimes = uiLen / uiCpLen;
	uiLeft = uiLen % uiCpLen;
	if (((UINT32)pDst1 <= (UINT32)pSrc1) || ((UINT32)pDst1 >= (UINT32)pSrc1 + uiLen)) {
		/* Non-overlap.  */
		CB_DMA_LOCK();
		CB_DMA_WAIT_DONE();
		CB_DMA_ENABLE_INT(DMA_DONE);

		for (uiIter = 0; uiIter < uiTimes; uiIter++) {
			do_dma_copy(pDst1, pSrc1, uiCpLen, DMA_MODE_DRAM_TO_DRAM);
			if (CB_DMA_wait_done()) {
				CB_DMA_UNLOCK();
				return 1;
			}
			pDst1 += uiCpLen;
			pSrc1 += uiCpLen;
		}
		/* Copy remaining bytes.  */
		if (0 != uiLeft) {
			do_dma_copy(pDst1, pSrc1, uiLeft, DMA_MODE_DRAM_TO_DRAM);
			if (CB_DMA_wait_done()) {
				CB_DMA_UNLOCK();
				return 1;
			}
		}
		CB_DMA_UNLOCK();
	} else {
		/* Overlap and need to copy from end to front.  */
		uiCpLen = (UINT32)pDst1 - (UINT32)pSrc1;
#ifdef CN_DMA_OVERLAP_MOVE_MINI
		//direct copy,cause it would be error,so current not use U32 to up speed
		if ((uiCpLen < CN_DMA_OVERLAP_MOVE_MINI) || (uiLen < CN_DMA_OVERLAP_MOVE_MINI)) {
			dma_printf("len [%d] Use PIO--\n", uiLen);
			for ( ; uiLen > 0; uiLen--) {
				pDst1[uiLen - 1] = pSrc1[uiLen - 1];
			}
		}
#else
		dma_printf("Have overlap src=0x%x dst=0x%x len=%d\n", (UINT32)pSrc1, (UINT32)pDst1, uiLen);
		uiTimes = uiLen / uiCpLen;
		uiLeft = uiLen % uiCpLen;
		pDst1 += uiLen;
		pSrc1 += uiLen;

		CB_DMA_LOCK();
		CB_DMA_WAIT_DONE();
		CB_DMA_ENABLE_INT(DMA_DONE);
		for (uiIter= 0; uiIter < uiTimes; uiIter++){
			pDst1 -= uiCpLen;
			pSrc1 -= uiCpLen;
			do_dma_copy(pDst1, pSrc1, uiCpLen, DMA_MODE_DRAM_TO_DRAM);
			if (CB_DMA_wait_done()) {
				CB_DMA_UNLOCK();
				return 1;
			}
		}
		pDst1 -= uiLeft;
		pSrc1 -= uiLeft;
		if (0 != uiLeft) {
			do_dma_copy(pDst1, pSrc1, uiLeft, DMA_MODE_DRAM_TO_DRAM);
			if (CB_DMA_wait_done()) {
				CB_DMA_UNLOCK();
				return 1;
			}
		}
		CB_DMA_UNLOCK();
#endif //		#ifdef CN_DMA_OVERLAP_MOVE_MINI
	}
	return 0;
}

#ifdef HAVE_CB_DMA_TEST
static int CB_DMA_MemSetByte(char *pDst, UINT8 bValue, size_t uiLen)
{
	UINT32 uiTimes;
	UINT32 uiCpLen;
	UINT32 uiLeft;
	UINT32 uiIter;
	UINT32 uiValue;

	uiCpLen = CB_DMA_MAX_LEN;
	uiTimes = uiLen / uiCpLen;
	uiLeft = uiLen % uiCpLen;
	uiValue = (bValue << 24) | (bValue << 16) | (bValue << 8) | bValue;

	CB_DMA_LOCK();
	CB_DMA_WAIT_DONE();
	CB_DMA_ENABLE_INT(DMA_DONE);
	for (uiIter = 0; uiIter < uiTimes; uiIter++) {
		do_dma_memset((UINT32)pDst, uiValue, 0, uiCpLen);
		if (CB_DMA_wait_done()) {
			CB_DMA_UNLOCK();
			return 1;
		}
		pDst += uiCpLen;
	}

	do_dma_memset((UINT32)pDst, uiValue, 0, uiLeft);
	if (CB_DMA_wait_done()) {
		CB_DMA_UNLOCK();
		return 1;
	}
	CB_DMA_UNLOCK();

	return 0;
}
#endif

static int CB_DMA_MemSetU32(char *pDst, UINT32 uiValue, UINT8 bStartValueOffset, size_t uiLen)
{
	dma_printf("pDst = 0x%08X, uiValue = 0x%08X\n", (UINT32)pDst, uiValue);

	UINT32 uiTimes;
	UINT32 uiCpLen;
	UINT32 uiLeft;
	UINT32 uiIter;

	uiCpLen = CB_DMA_MAX_LEN;
	uiTimes = uiLen / uiCpLen;
	uiLeft = uiLen % uiCpLen;

	CB_DMA_LOCK();
	CB_DMA_WAIT_DONE();
	CB_DMA_ENABLE_INT(DMA_DONE);
	for (uiIter = 0; uiIter < uiTimes; uiIter++) {
		do_dma_memset((UINT32)pDst, uiValue, bStartValueOffset, uiCpLen);
		if (CB_DMA_wait_done()) {
			CB_DMA_UNLOCK();
			return 1;
		}
		pDst += uiCpLen;
	}
	do_dma_memset((UINT32)pDst, uiValue, bStartValueOffset, uiLeft);
	if (CB_DMA_wait_done()) {
		CB_DMA_UNLOCK();
		return 1;
	}
	CB_DMA_UNLOCK();

	return 0;
}

#ifdef HAVE_CB_DMA_TEST
//1 TEST PANTTEN
#define TEST_NORMAL_SINGLE_MBUS_CROSS
#define TEST_NORMAL_TWO_MBUS_CROSS
#define TEST_ABNORMAL
#define TEST_OVERLAP
#define TEST_TIME
#define TEST_MAX_MEM_ALIGN				128	// test mem align[1, TEST_MAX_MEM_ALIGN)
#ifdef TEST_MAX_MEM_ALIGN
#define TEST_DMA_ADDR_OFFSET_B4			128	// when test len > TEST_DMA_ADDR_OFFSET_B4, will not test align again
#endif
#define TIME_TEST_TIMES					2	// [1,n] use to average result

#define TEST_NORMAL
#ifdef TEST_NORMAL
#define TEST_DMA_LENGTH_ONE_BY_ONE
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
#define TEST_DMA_LENGTH_ONE_BY_ONE_R_W_B4		(CB_DMA_MAIN_RAM_SIZE)
#define TEST_DMA_LENGTH_ONE_BY_ONE_CP_SET_B4	(CB_DMA_MAIN_RAM_SIZE + 128)
#endif // TEST_DMA_LENGTH_ONE_BY_ONE

#define TEST_NORMAL_READ
#define TEST_NORMAL_WRITE
#define TEST_NORMAL_READ_WRITE
#define TEST_NORMAL_MEMCPY
#define TEST_NORMAL_MEMSET
#endif // TEST_NORMAL

#ifdef  TEST_TIME
#define TEST_TIME_OUT_FOR_EXCEL //3Easy to copy to the excel
#endif

#define TEST_OPEN_FILL_RAND_DATA //3Use rand data to fill buffer

#define DO_FAIL()  																	\
	do {																			\
		dma_printf_err("fail ,Reg dump:\n");										\
		dma_printf_err("HWREG_R(dma_config)=%x\n",HWREG_R(dma_config));				\
		dma_printf_err("HWREG_R(dma_length)=%x\n",CB_GET_DMA_LENGTH());				\
		dma_printf_err("HWREG_R(dma_src)=%x\n",HWREG_R(dma_src));					\
		dma_printf_err("HWREG_R(dma_dst)=%x\n",HWREG_R(dma_dst));					\
		dma_printf_err("HWREG_R(dma_memset_value)=%x\n",HWREG_R(dma_memset_value));	\
		dma_printf_err("HWREG_R(dma_int_en)=%x\n",HWREG_R(dma_int_en));				\
		dma_printf_err("HWREG_R(dma_int_flag)=%x\n",HWREG_R(dma_int_flag));			\
		while (1) {																	\
			continue;																\
		}																			\
	} while(0)

typedef int (* pFunWaitDmaDone)(void);
static pFunWaitDmaDone g_FunWaitDmaDone = NULL;

static UINT32 __Memcmp8(UINT8* pBuffStar1, UINT8 bValue, UINT32 uiBuffLen)
{
	UINT32 uiRet = 0;
	UINT32 uiIter;

	for (uiIter = 0; uiIter < uiBuffLen; uiIter++) {
		if (*(pBuffStar1+ uiIter) != bValue) {
			dma_printf("diff when %d, %x!=%x\n", uiIter, *(pBuffStar1+ uiIter), bValue);
			uiRet = uiIter + 1;
			break;
		}
	}
	return uiRet;
}

static UINT32 __Memcmp32(UINT8* pBuff, UINT32 uiValue, UINT32 uiLen)
{
	UINT32 uiIter;
	UINT32 uiCount = uiLen / 4;
	UINT32 uiLeft = uiLen & 3;
	UINT8 *pbBuff;
	UINT8 a, b, c, d;

#ifdef CB_DMA_BIG_ENDIAN
	d =  uiValue & 0xff;
	c =  (uiValue >> 8) & 0xff;
	b =  (uiValue >> 16) & 0xff;
	a =  (uiValue >> 24) & 0xff;
#else
	a =  uiValue & 0xff;
	b =  (uiValue >> 8) & 0xff;
	c =  (uiValue >> 16) & 0xff;
	d =  (uiValue >> 24) & 0xff;
#endif
	for (uiIter = 0; uiIter < uiCount; uiIter += 4) {
		if ((*(pBuff + uiIter) != a )
			|| (*(pBuff + uiIter + 1) != b)
			|| (*(pBuff + uiIter + 2) != c)
			|| (*(pBuff + uiIter + 3) != d)) {
			dma_printf("1Diff when %d (0x%x)\n",uiIter * 4, *(pBuff + uiIter));
			return (uiIter + 1) * 4;
		}
	}
	if (uiLeft) {
		pbBuff = pBuff + uiCount * 4;
		for (uiIter = 0; uiIter < uiLeft; uiIter++) {
			a = *(pbBuff + uiIter);

#ifdef CB_DMA_BIG_ENDIAN
				b = (uiValue >> (8 * (3 - uiIter))) & 0xff;
#else
				b = (uiValue >> (8 * uiIter)) & 0xff;
#endif
			if (a != b) {
				dma_printf("2Diff when %d (0x%x) %x != %x \n",uiCount*4+uiIter, *(pbBuff + uiIter),a,b);
				return uiCount * 4 + uiIter + 1;
			}
		}
	}
	return 0;
}

static void PrintBlock(UINT8* pBuffStar, UINT32 uiBuffLen)
{
	pBuffStar = (UINT8 *)((UINT32)pBuffStar | 0x20000000);

	UINT32 uiIter;
	printf(" %02x", pBuffStar[0]);
	for (uiIter = 1; uiIter < uiBuffLen; uiIter++) {
		if (uiIter % 32 == 0) {
			printf("\n");
		}
		if (uiIter % 512 == 0) {
			printf("\n");
		}
		printf(" %02x", pBuffStar[uiIter]);
	}
	printf("\n");
}

static void __attribute__((unused)) Dump_CB_DMA(cyg_ucount32 uiLen)
{
	printf("Dump_CB_DMA==>\n");
	PrintBlock((UINT8 *)CB_DMA_BASE_ADR, uiLen);
}

#if 1
static int mipsrand(void)
{
	unsigned int c0_rand;
	asm volatile ("mfc0 %0, $9" : "=r"(c0_rand));
	return c0_rand;
}
#define RAND()		mipsrand()
#else
#define RAND()		rand()
#endif
#define rand_ul()	((unsigned int)RAND() | ( (unsigned int)RAND() << 16))

static void __attribute__((unused)) __Fill_rand_value(char *pBuff, UINT32 uiLen)
{
	UINT32 uiIter;
	UINT32 *puiBuff = (UINT32 *)(((UINT32)pBuff + 3)& (~3));
	UINT32 uiLen32 = (uiLen / 4);

	if (uiLen < 4) {
		memset(pBuff, 0xa5, uiLen);
	} else {
		for (uiIter = 0; uiIter < uiLen32; uiIter++) {
			*(puiBuff+uiIter) = rand_ul();
		}
	}
}

/*****************************************************************************
 Function Name: CB_DMA_NormalTest
 Purposes     : Test normal read write and cpy set
 Description  :
 Arguments    :
       [In]  pLuma < memory address 1, may be MBUS0 >
       [In]  pChroma < memory address 1, may be MBUS1 >
       [In]  uiMaxOffset < normal set 128, mean will test [0,128], USE to test address align >
       [Out] None
 Return Value :
          None
 See also :
          None

 History Information Description
*******************************************************************************/
UINT32 CB_DMA_NormalTest(UINT8* pLuma, UINT8* pChroma,UINT32 uiMaxOffset)
{
	UINT32 uiIter;
	UINT8 *luma_addr = pLuma;
	UINT8 *chroma_base = pChroma;
	UINT32 testLenShift;
	UINT32 testLen;
	UINT32 uiTemp;
	INT32 iRet;

	dma_printf("chroma_base=%x\n", (int)chroma_base);
#ifdef TEST_NORMAL_READ
	for (testLenShift = 0; testLenShift <= CB_DMA_SIZE_MAIN_SHIFT; testLenShift++) {
		testLen = (1 << testLenShift);
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		UINT32 uiIter2;

		for (uiIter2 = 0; uiIter2 < (1 << testLenShift); uiIter2++) {
			if ((testLen >= TEST_DMA_LENGTH_ONE_BY_ONE_R_W_B4) ||
				((testLenShift == CB_DMA_SIZE_MAIN_SHIFT))) {
				uiIter2 = (1 << testLenShift);//out loop
			} else if (uiIter2 > 0) {
				testLen++;
			}
#endif
			luma_addr = pLuma;
			for (uiIter = 0; uiIter < uiMaxOffset; uiIter++, luma_addr++) {
#ifdef CB_DMA_USE_INTERRUPT
				dma_printf("INT " );
#endif
				dma_printf("Read Src=%x ", (UINT32)luma_addr);
				dma_printf("len(%10d) offset(%3d) ", testLen, uiIter);
				#ifdef TEST_OPEN_FILL_RAND_DATA
				__Fill_rand_value((char *)luma_addr, testLen);
				__Fill_rand_value((char *)CB_DMA_BASE_ADR, testLen);
				#else
				memset(luma_addr, uiIter, testLen);
				memset(CB_DMA_BASE_ADR, ~uiIter, testLen);//invalid
				#endif

				CB_DMA_MemToDma((char *)CB_DMA_BASE_ADR, (char *)luma_addr, testLen);
				if (0 != memcmp((char *)CB_DMA_BASE_ADR, luma_addr, testLen)) {
					dma_printf("=MemToDma =Fail ==uiIter=%d datalen=%d\n",uiIter,testLen);
					PrintBlock(luma_addr, testLen);
					Dump_CB_DMA(testLen);
					DO_FAIL();
				} else {
					dma_printf("\tOK\n");
				}

				if (testLen > TEST_DMA_ADDR_OFFSET_B4) {
					break;
				}
			}
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		}
#endif
	}
	dma_printf("===================================\n");
	dma_printf("=MemToDma(Read) =OK \n\n");
	dma_printf("===================================\n");
#endif

#ifdef TEST_NORMAL_WRITE
	for (testLenShift = 0; testLenShift <= CB_DMA_SIZE_MAIN_SHIFT; testLenShift++) {
		testLen = 1 << testLenShift;
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		for (uiIter2 = 0; uiIter2 < (1 << testLenShift); uiIter2++){
			if ((testLen >= TEST_DMA_LENGTH_ONE_BY_ONE_R_W_B4) ||
				((testLenShift == CB_DMA_SIZE_MAIN_SHIFT)))	{
				uiIter2 = (1 << testLenShift);//out loop
			} else if (uiIter2 > 0) {
				testLen ++;
			}
#endif
			luma_addr = pLuma;
			for (uiIter = 0; uiIter < uiMaxOffset;uiIter++, luma_addr++) {
#ifdef CB_DMA_USE_INTERRUPT
				dma_printf("INT ");
#endif
				dma_printf("Write Src=%x ", (UINT32)luma_addr);
				dma_printf("len(%10d) offset(%3d) ",testLen,uiIter);
				#ifdef TEST_OPEN_FILL_RAND_DATA
				__Fill_rand_value((char *)CB_DMA_BASE_ADR, testLen);
				__Fill_rand_value((char *)luma_addr, testLen);
				#else
				memset(CB_DMA_BASE_ADR, uiIter, testLen);
				memset(luma_addr, 0xff, testLen);//invalid
				#endif

				CB_DMA_DmaToMem((char *)luma_addr, (char *)CB_DMA_BASE_ADR, testLen);

				if (0 != memcmp((char *)CB_DMA_BASE_ADR, (char *)luma_addr, testLen)) {
					dma_printf("Dump chroma_base\n");
					PrintBlock(luma_addr, testLen);
					Dump_CB_DMA(testLen);
					DO_FAIL();
				} else {
					dma_printf("\tOK\n");
				}

				if (testLen > TEST_DMA_ADDR_OFFSET_B4)
					break;
			}
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		}
#endif
	}
	dma_printf("===================================\n");
	dma_printf("\n=CB_DMA_DmaToMem (write)=OK \n\n");
	dma_printf("===================================\n");
#endif

#ifdef TEST_NORMAL_READ_WRITE
	for (testLenShift = 0; testLenShift <= CB_DMA_SIZE_MAIN_SHIFT; testLenShift++) {
		testLen = 1<< testLenShift;
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		for (uiIter2 = 0; uiIter2 < (1<< testLenShift); uiIter2++){
			if ((testLen >= TEST_DMA_LENGTH_ONE_BY_ONE_R_W_B4) ||
				((testLenShift == CB_DMA_SIZE_MAIN_SHIFT))) {
				uiIter2 = (1<< testLenShift);//out loop
			} else if (uiIter2 > 0) {
				testLen++;
			}
#endif
			chroma_base = pChroma;
			luma_addr = pLuma;
			//3 Offset ,test align [0,uiMaxOffset]
			for(uiIter = 0; uiIter < uiMaxOffset; uiIter++, chroma_base++, luma_addr++) {
#ifdef CB_DMA_USE_INTERRUPT
				dma_printf("INT " );
#endif
				dma_printf("Read & Write Src=%x Dst=%x ", (UINT32)chroma_base, (UINT32)luma_addr);
				dma_printf("len(%10d) offset(%3d) ",testLen,uiIter);
#ifdef TEST_OPEN_FILL_RAND_DATA
				__Fill_rand_value((char *)chroma_base, testLen);
				__Fill_rand_value((char *)luma_addr, testLen);
#else
				memset(chroma_base, uiIter, testLen);
				memset(luma_addr, uiIter + 1, testLen);
#endif
				CB_DMA_MemToDma((char *)CB_DMA_BASE_ADR, (char *)chroma_base, testLen);
				CB_DMA_DmaToMem((char *)luma_addr, (char *)CB_DMA_BASE_ADR, testLen);

				if (0 != memcmp(luma_addr, chroma_base,testLen)) {
					dma_printf("===Fail ==uiIter=0x%x datalen=%d\n",uiIter,testLen);
					dma_printf("addr1 is==>\n");
					PrintBlock(luma_addr, testLen);
					Dump_CB_DMA(testLen);
					dma_printf("addr2  is==>\n");
					PrintBlock(chroma_base, testLen);
					DO_FAIL();
				} else {
					dma_printf("\tOK\n");
				}

				if (testLen > TEST_DMA_ADDR_OFFSET_B4)
					break;
			}
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		}
#endif
	}
	dma_printf("===================================\n");
	dma_printf("MemToDma , DmaToMem=OK(read write) \n");
	dma_printf("===================================\n\n");
#endif

#ifdef TEST_NORMAL_MEMCPY
	//3MemCopy
	memset(luma_addr, 0xa5, CB_DMA_MAIN_RAM_SIZE);//invalid
	dma_printf("Test Copy\n");
	for (testLenShift = 0; testLenShift <= CB_DMA_SIZE_CP_SET_MAX_SHIFT; testLenShift++) {
		testLen = 1 << testLenShift;
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		for (uiIter2 = 0; uiIter2 < (1<< testLenShift); uiIter2++) {
			if ((testLen >= TEST_DMA_LENGTH_ONE_BY_ONE_CP_SET_B4)
				|| (testLenShift == CB_DMA_SIZE_CP_SET_MAX_SHIFT)) {
				uiIter2 = (1 << testLenShift);//out loop
			} else if (uiIter2 > 0) {
				testLen++;
			}
#endif
			chroma_base = pChroma;
			luma_addr= pLuma;
			for(uiIter = 0; uiIter < uiMaxOffset; uiIter++, chroma_base++, luma_addr++) {
#ifdef CB_DMA_USE_INTERRUPT
				dma_printf("INT " );
#endif
				dma_printf("MemCopy Src=%x Dst=%x ", (UINT32)chroma_base, (UINT32)luma_addr);
				dma_printf("len(%10d) offset(%3d) ",testLen,uiIter);

#ifdef TEST_OPEN_FILL_RAND_DATA
				__Fill_rand_value((char *)chroma_base, testLen);
				__Fill_rand_value((char *)luma_addr, testLen);
#else
				memset(chroma_base, uiIter, testLen);
				memset(luma_addr, uiIter+1, testLen); //invalid
#endif

				CB_DMA_MemToMem((char *)luma_addr, (char *)chroma_base, testLen);
				if (0 != memcmp((char *)luma_addr, (char *)chroma_base, testLen)) {
					dma_printf("===Fail ==uiIter=%d datalen=%d\n",uiIter,testLen);
					dma_printf("src=%x\n", (UINT32)chroma_base);
					PrintBlock(chroma_base, testLen);
					dma_printf("dst=%x\n", (UINT32)luma_addr);
					PrintBlock(luma_addr, testLen);
					DO_FAIL();
				} else {
					dma_printf("\tOK \n");
				}

				if (testLen > TEST_DMA_ADDR_OFFSET_B4)
					break;
			}
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		}
#endif
	}
	dma_printf("===================================\n");
	dma_printf("Dram2Dram MemCopy OK \n");
	dma_printf("===================================\n\n");
#endif

#ifdef TEST_NORMAL_MEMSET
	//3MemSet
	dma_printf("\n===================================\n");
	dma_printf("Test memset\n");
	for (testLenShift = 0; testLenShift <= CB_DMA_SIZE_CP_SET_MAX_SHIFT; testLenShift++) {
		testLen = 1 << testLenShift;
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		for (uiIter2 = 0; uiIter2 < (1<< testLenShift); uiIter2++){
			if ((testLen >= TEST_DMA_LENGTH_ONE_BY_ONE_CP_SET_B4) ||
				((testLenShift == CB_DMA_SIZE_CP_SET_MAX_SHIFT)))	{
				uiIter2 = (1 << testLenShift);//out loop
			} else if (uiIter2 > 0) {
				testLen ++;
			}
#endif
			chroma_base = pChroma;
			luma_addr= pLuma;
			for (uiIter = 0; uiIter <  uiMaxOffset; uiIter++,chroma_base++,luma_addr++) {
#ifdef CB_DMA_USE_INTERRUPT
				dma_printf("INT " );
#endif
				dma_printf("MemSet Src=%x  ", (UINT32)luma_addr );
				dma_printf("len(%10d) offset(%3d) ", testLen, uiIter);

				uiTemp = rand_ul();
				CB_DMA_MemSetByte((char *)luma_addr, uiTemp, testLen);
				iRet = __Memcmp8(luma_addr, uiTemp, testLen);
				if (0 != iRet) {
					dma_printf("1===Fail set luma_addr%x ==uiIter=%d datalen=%d iRet=%d fill %x\n",
						(UINT32)luma_addr,uiIter,testLen,iRet,uiTemp);
					PrintBlock(luma_addr + iRet - 1, 512);
					DO_FAIL();
				} else {
					dma_printf("\tOK 1 ");
				}
				uiTemp = rand_ul();
				CB_DMA_MemSetU32((char *)luma_addr, uiTemp, 0, testLen);
				iRet = __Memcmp32((UINT8 *)luma_addr, uiTemp, testLen);
				if (0 != iRet) {
					dma_printf("2===Fail set chroma_base%x ==uiIter=%d datalen=%d  iRet=%d  fill %x\n",
						(UINT32)chroma_base,uiIter,testLen,iRet, uiTemp);

					PrintBlock(luma_addr+iRet-1, 512);
					DO_FAIL();
				} else{
					dma_printf("\tOK 2\n ");
				}

				if (testLen > TEST_DMA_ADDR_OFFSET_B4) {
					break;
				}
			}
#ifdef TEST_DMA_LENGTH_ONE_BY_ONE
		}
#endif
	}
	dma_printf("===================================\n");
	dma_printf("Memset OK \n\n");
	dma_printf("\n===================================\n");
#endif
	return 0;
}


//0 for success , else for fail
/*
uiMaxShiftbit: if = 20,mean will test 1<< 0, 1<<1,1<<2,1<<3,1<<4,..... 1<<20 , memcpy time cost
uiOffset: normal [1,128]
*/
void CB_DMA_TimeTest(UINT8* pLuma, UINT8* pChroma, UINT32 uiMaxShiftbit, UINT32 uiOffset, UINT32 uiTestTimes)
{
	UINT32 uiIter ;
	UINT32 uiIter2;
	UINT32 count1;
	UINT32 count2;
	UINT8* luma_addr= pLuma;
	UINT8* chroma_base = pChroma;
	UINT32 testLenShift;
	UINT32 testLen ;
	UINT32 uiHwTicks;
	UINT32 uiSwTicks;

	//3 Test memcpy time
	//dma_printf("\n\nTime test .....................\n");
#ifdef CB_DMA_USE_INTERRUPT
	dma_printf("USE INTERRUPT to wait dma done..............\n");
#else
	dma_printf("USE POLLING  to wait dma done..............\n");
#endif
	dma_printf("\nmemcpy  test .....................\n");
	dma_printf("addr1\taddr2\tuiTestTimes\tlen\tuiSwTicks\tuiHwTicks\tPercent[S*100/H]\n");
	for (testLenShift = 0; testLenShift <= uiMaxShiftbit; testLenShift++) {
		testLen = 1 << testLenShift;
		luma_addr= pLuma;
		chroma_base = pChroma;

		for (uiIter2 = 0; uiIter2 <= uiOffset; uiIter2++,luma_addr++,chroma_base++) {
			//3HWware
#ifdef TEST_TIME_OUT_FOR_EXCEL
			dma_printf("%8x\t%8x\t%3d", (UINT32)luma_addr, (UINT32)chroma_base, uiTestTimes);
#else
			dma_printf("\nTest time memcp   addr1=%8x. ....addr2=%8x... ", (UINT32)luma_addr, (UINT32)chroma_base);
			dma_printf(" len =%10d [%10d KB]\n",testLen, testLen >> 10);
#endif
			do {
				asm volatile ("mfc0	%0, $9" : "=r"(count1));
				for (uiIter = 0; uiIter < uiTestTimes; uiIter++) {
					CB_DMA_MemToMem((char *)luma_addr, (char *)chroma_base, testLen);
				}
				asm volatile ("mfc0	%0, $9" : "=r"(count2));
			} while (count2 < count1);
			uiHwTicks = count2 - count1;

			//3 Software
			do {
				asm volatile ("mfc0	%0, $9" : "=r"(count1));
				for (uiIter = 0; uiIter < uiTestTimes; uiIter++) {
					memcpy(luma_addr,chroma_base,testLen);
				}
				asm volatile ("mfc0	%0, $9" : "=r"(count2));
			} while (count2 < count1);
			uiSwTicks = count2 - count1;

#ifdef TEST_TIME_OUT_FOR_EXCEL
			dma_printf("\t%10d\t%10d\t%10d\t", testLen, uiSwTicks, uiHwTicks);
			if ((uiSwTicks > uiHwTicks) && ((uiHwTicks >> 16) > 0)) {
				uiSwTicks >>= 8;
				uiHwTicks >>= 8;
			}
			//dma_printf("%10d\n",((int)uiSwTicks - (int)uiHwTicks)*100/(int)uiHwTicks );
			dma_printf("%10d\n", (uiSwTicks * 100 / uiHwTicks));
#else
			dma_printf("memcpy SW\t %d times need (\t %10d ticks)\n", uiTestTimes, uiSwTicks);
			dma_printf("memcpy HW\t %d times need (\t %10d ticks)\n", uiTestTimes, uiHwTicks);
#endif
		}
	}

	//3 Test memset time
	dma_printf("\nmemset test .....................\n");
	dma_printf("addr1\taddr2\tuiTestTimes\tlen\tuiSwTicks\tuiHwTicks\tPercent[S*100/H]\n");
	for (testLenShift = 0; testLenShift <= uiMaxShiftbit; testLenShift++)	{
		testLen = 1 << testLenShift;
		luma_addr= pLuma;
		chroma_base = pChroma;

		for (uiIter2 = 0; uiIter2 <= uiOffset; uiIter2++,luma_addr++,chroma_base++){
			//3HWware
#ifdef TEST_TIME_OUT_FOR_EXCEL
			dma_printf("%8x\t%8x\t%3d", (UINT32)luma_addr, (UINT32)chroma_base, uiTestTimes);
#else
			dma_printf("\nTest time memset   addr1=%8x. ....addr2=%8x... ", (UINT32)luma_addr, (UINT32)chroma_base);
			dma_printf(" len =%10d [%10d KB]\n",testLen, testLen >> 10);
#endif
			do {
				asm volatile ("mfc0	%0, $9" : "=r"(count1));
				for (uiIter = 0; uiIter < uiTestTimes; uiIter++) {
					CB_DMA_MemSetByte((char *)luma_addr, 0x5a, testLen);
				}
				asm volatile ("mfc0	%0, $9" : "=r"(count2));
			} while(count2 < count1);
			uiHwTicks = count2 - count1;

			//3 Software
			do {
				asm volatile ("mfc0	%0, $9" : "=r"(count1));
				for (uiIter = 0; uiIter < uiTestTimes; uiIter++) {
					memset(luma_addr, 0x5a, testLen);
				}
				asm volatile ("mfc0	%0, $9" : "=r"(count2));
			} while(count2 < count1);
			uiSwTicks = count2 - count1;

#ifdef TEST_TIME_OUT_FOR_EXCEL
			dma_printf("\t%10d\t%10d\t%10d\t",testLen, uiSwTicks, uiHwTicks);
			if ((uiSwTicks > uiHwTicks) &&  ((uiHwTicks >>16) > 0) ){
				uiSwTicks >>= 8;
				uiHwTicks >>= 8;
			}
			dma_printf("%10d\n",(uiSwTicks * 100 / uiHwTicks ));
#else
			dma_printf("memset SW\t %d times need (\t %10d ticks)\n", uiTestTimes, uiSwTicks);
			dma_printf("memset HW\t %d times need (\t %10d ticks)\n", uiTestTimes, uiHwTicks);
#endif
		}
	}
}

static void __ReadData(UINT8* pBuff, UINT32 uiLen)
{
	UINT32 uiIter;
	UINT8 bTemp;
	for (uiIter = 0; uiIter < uiLen; uiIter++){//will be ??
		bTemp = *(pBuff + uiIter);
	}
}

static int Trouble_func_HW_Cfg(void)
{
	UINT8 *pSrcAddr= (UINT8 *)0x84000000;

	dma_printf("%s\n",__FUNCTION__);
	CB_SET_DMA_LENGTH(0xa5);
	CB_SET_DMA_SRC_ADDR((UINT32)pSrcAddr);
	CB_SET_DMA_DST_ADDR((UINT32)(pSrcAddr + (1 << 20)));
	CB_FILL_MEMSET_VALUE_8(0x5a);

	return 1;
}

static int Trouble_func_PIO_read(void)
{
	UINT32 uiDMAlen = CB_GET_DMA_LENGTH();
	UINT32 uiLen;

	uiLen = CB_DMA_MAIN_RAM_SIZE - uiDMAlen - 1;
	if (uiLen > 32) {
		uiLen = 32;
	}
	__ReadData((UINT8 *)CB_DMA_BASE_ADR, CB_DMA_MAIN_RAM_SIZE);
	PrintBlock((UINT8 *)CB_DMA_BASE_ADR, uiLen);
	PrintBlock((UINT8 *)CB_DMA_BASE_ADR + CB_DMA_MAIN_RAM_SIZE - 256, 32);

	return 1;
}

static int Trouble_func_PIO_Write(void)
{
	UINT32 uiDMAlen = CB_GET_DMA_LENGTH();
	UINT32 uiLen;

	if (uiDMAlen < CB_DMA_MAIN_RAM_SIZE){
		uiLen = CB_DMA_MAIN_RAM_SIZE-uiDMAlen - 1;
		if (uiLen > 64) {
			uiLen = 64;
		}
		memset((void *)(CB_DMA_BASE_ADR + uiDMAlen), 0xa5, uiLen);
		PrintBlock((UINT8 *)(CB_DMA_BASE_ADR + uiDMAlen), uiLen);
	}
	return 1;
}

static int Trouble_func_memcp(void)
{
	dma_printf("%s  \n",__FUNCTION__);
	//3copy to test last 256
	if (CB_GET_MODE() == DMA_MODE_DRAM_TO_DRAM) {
		memset((void *)(CB_DMA_BASE_ADR + CB_DMA_MAIN_RAM_SIZE - 256), 0xa5, 255);
		if (CB_GET_INT_FLAG() & (1 << ADDR_CONFLICT))
			dma_printf("INT_FLAG=%x you are acess last 256B when MEMCPY\n", CB_GET_INT_FLAG());
	}
	return 1;
}

int CB_DMA_AbnormalTest(UINT8* pLuma, UINT8* pChroma)
{
	int iRet;

	g_FunWaitDmaDone = Trouble_func_HW_Cfg;
	iRet = CB_DMA_NormalTest(pLuma, pChroma, 1);

	g_FunWaitDmaDone = Trouble_func_PIO_read;
	iRet = CB_DMA_NormalTest(pLuma, pChroma, 1);
	CB_CLEAR_INT_FLAG(ADDR_CONFLICT);

	g_FunWaitDmaDone = Trouble_func_PIO_Write;
	iRet = CB_DMA_NormalTest(pLuma, pChroma, 1);
	CB_CLEAR_INT_FLAG(ADDR_CONFLICT);

	g_FunWaitDmaDone = Trouble_func_memcp;
	iRet = CB_DMA_NormalTest(pLuma, pChroma, 1);

	g_FunWaitDmaDone = NULL;
	return iRet;
}

int CB_DMA_OverLapTest(void)
{
	UINT32 uiIter;
	UINT32 uiLen;
	UINT32 uiStartlen = 16;
	UINT32 uiItervalMax = 128;
#ifdef DRAM_BUS_SINGLE
	UINT8 *pSrc = (UINT8*)0x84000000;
	UINT8 *pBackData = (UINT8 *)pSrc + CB_DMA_MAIN_RAM_SIZE * 2;
	if ((unsigned int)pBackData > (RAM_A_START + (RAM_A_SIZE_MB << 20))) {
		diag_printf("%s:%d: Memory shortage!\n", __FUNCTION__, __LINE__);
		return -1;
	}
#else
	UINT8 *pSrc = (UINT8*)0x82000000;
	UINT8 *pBackData = (UINT8 *)0x90000000;
#endif

	for (uiLen  = uiStartlen; uiLen  < CB_DMA_MAIN_RAM_SIZE; uiLen += uiItervalMax) {
		for(uiIter = 0; uiIter < uiLen; uiIter += 2) {
			((UINT32 *)pSrc)[uiIter] = uiIter;
		}

		memcpy(pBackData, pSrc, uiLen);

		for (uiIter = 1; uiIter < uiItervalMax ; uiIter++){
			memcpy(pSrc, pBackData, uiLen);
			//CB_DMA_MemToMem(pSrc, (char*)((UINT32)pSrc+ uiIter), uiLen);
			CB_DMA_MemToMem((char *)(pSrc + uiIter), (char *)pSrc, uiLen);
			if (memcmp(pBackData, pSrc + uiIter, uiLen)){
				dma_printf("=Fail src=0x%x dst=0x%x len=%d", (UINT32)pSrc, (UINT32)pSrc + uiIter,uiLen);
				DO_FAIL();
			}
		}
	}
	dma_printf("%s out \n",__FUNCTION__);

	return 1;
}
void CB_DMA_Test(void)
{
#ifdef DRAM_BUS_SINGLE
	UINT8 *lumaUncacheAddr = (UINT8*)0xA4000000;
	UINT8 *lumaCacheAddr = (UINT8*)0x84000000;
	UINT8 *ChromaUncacheAddr = (UINT8*)0xA6000000;
	UINT8 *ChromaCacheAddr = (UINT8*)0x86000000;
#else
	UINT8 *lumaUncacheAddr = (UINT8*)0xA2000000;
	UINT8 *lumaCacheAddr = (UINT8*)0x82000000;
	UINT8 *ChromaUncacheAddr = (UINT8*)0xb0020000;
	UINT8 *ChromaCacheAddr = (UINT8*)0x90000000;
#endif
	UINT32 uiIter;

	CB_DMA_Open();

	dma_printf("lumaCacheAddr=%x \n",(int)lumaCacheAddr);
	dma_printf("lumaUncacheAddr=%x \n",(int)lumaUncacheAddr);
	dma_printf("ChromaCacheAddr=%x \n",(int)ChromaCacheAddr);
	dma_printf("ChromaUncacheAddr=%x \n",(int)ChromaUncacheAddr);

	for (uiIter = 0; uiIter < 2; uiIter++) {
	//2 First is polling DMA DONE, Then use interrupt
#ifdef CB_DMA_USE_INTERRUPT_DEFAULT
		if (uiIter == 0) {
			;
		} else if (uiIter == 1) {
			break;//3Only onece
		}
#endif
		if (uiIter == 1) {
			;
		}

#ifdef TEST_NORMAL_SINGLE_MBUS_CROSS
		dma_printf("\n\n====== Normal test=====\n");
		dma_printf("=======================\n");
		dma_printf("\n\n=======================\n");
		dma_printf("====test single mbus test  \n");
		dma_printf("=======================\n");

		dma_printf("\n====mbus0 luma Uncache test \t");
		if (CB_DMA_NormalTest(lumaUncacheAddr, lumaUncacheAddr + CB_DMA_MAX_LEN, TEST_MAX_MEM_ALIGN)) {
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}

		dma_printf("\n======mbus0 luma cache uncache test\t");

		if (CB_DMA_NormalTest(lumaCacheAddr, lumaCacheAddr + CB_DMA_MAX_LEN, TEST_MAX_MEM_ALIGN)){
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}

#ifndef DRAM_BUS_SINGLE
		dma_printf("\n======mbus1 Chroma uncache test\t");

		if (CB_DMA_NormalTest(ChromaUncacheAddr, ChromaUncacheAddr + CB_DMA_MAX_LEN, TEST_MAX_MEM_ALIGN)){
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}

		dma_printf("\n=======mbus1 Chroma cache test\t");
		if (CB_DMA_NormalTest(ChromaCacheAddr, ChromaCacheAddr + CB_DMA_MAX_LEN, TEST_MAX_MEM_ALIGN)){
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}
#endif
#endif//#ifdef TEST_NORMAL_SINGLE_MBUS_CROSS

#ifndef DRAM_BUS_SINGLE
#ifdef TEST_NORMAL_TWO_MBUS_CROSS
		dma_printf("\n\n=======================\n");
		dma_printf("====two mbus cross test \n");
		dma_printf("=======================\n\n\n");

		dma_printf("\n====mbus0 mbus1 Uncache test \t");
		if (CB_DMA_NormalTest(lumaUncacheAddr, ChromaUncacheAddr, TEST_MAX_MEM_ALIGN)) {
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}
		dma_printf("\n==mbus0 mbus1=cache uncache test\t");
		if (CB_DMA_NormalTest(lumaCacheAddr, ChromaUncacheAddr, TEST_MAX_MEM_ALIGN)) {
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}
		dma_printf("\n===mbus0 mbus1=======cache uncache test\t");
		if (CB_DMA_NormalTest(lumaUncacheAddr, ChromaCacheAddr, TEST_MAX_MEM_ALIGN)){
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}
		dma_printf("\n===mbus0 mbus1 ==========cache test\t");
		if (CB_DMA_NormalTest(lumaCacheAddr, ChromaCacheAddr, TEST_MAX_MEM_ALIGN)){
			dma_printf("Fail\n");
		} else {
			dma_printf("OK\n");
		}
#endif//TEST_NORMAL_TWO_MBUS_CROSS

#ifdef TEST_ABNORMAL
	 //3 abnormal test
	    dma_printf("\n\n=======================\n");
	    dma_printf("====== abnormal test=====\n");
	    dma_printf("=======================\n\n\n");

		dma_printf("\n===========cache uncache test\n");
		if (CB_DMA_AbnormalTest(lumaCacheAddr, ChromaUncacheAddr) < 0) {
			dma_printf("Fail\n");
		} else {
			dma_printf("ok\n");
		}

		if (CB_DMA_AbnormalTest(lumaUncacheAddr, ChromaUncacheAddr) < 0) {
			dma_printf("Fail\n");
		} else {
			dma_printf("ok\n");
		}
		dma_printf("\n===========cache uncache test\n");
		if (CB_DMA_AbnormalTest(lumaUncacheAddr, ChromaCacheAddr) < 0) {
			dma_printf("Fail\n");
		} else {
			dma_printf("ok\n");
		}
		dma_printf("\n===========cache  test\n");
		if (CB_DMA_AbnormalTest(lumaCacheAddr, ChromaCacheAddr) < 0) {
			dma_printf("Fail\n");
		} else {
			dma_printf("ok\n");
		}
#endif//#ifdef TEST_ABNORMAL
#endif

#ifdef TEST_OVERLAP
		CB_DMA_OverLapTest();
#endif

#ifdef TEST_TIME
	    //3 Time test
	    dma_printf("\n\n=======================\n");
	    dma_printf("====== time test=====\n");
	    dma_printf("=======================\n\n\n");

	    dma_printf("\n=========Uncache test \n");
		CB_DMA_TimeTest(lumaUncacheAddr, ChromaUncacheAddr,CB_DMA_SIZE_CP_SET_MAX_SHIFT, 0, TIME_TEST_TIMES);
	    dma_printf("\n===========cache uncache test\n");
	    CB_DMA_TimeTest(lumaCacheAddr, ChromaUncacheAddr,CB_DMA_SIZE_CP_SET_MAX_SHIFT, 0, TIME_TEST_TIMES);
	    dma_printf("\n=========== uncache cache test\n");
	    CB_DMA_TimeTest(lumaUncacheAddr, ChromaCacheAddr,CB_DMA_SIZE_CP_SET_MAX_SHIFT, 0, TIME_TEST_TIMES);
	    dma_printf("\n===========cache  test\n");
	    CB_DMA_TimeTest(lumaCacheAddr, ChromaCacheAddr,CB_DMA_SIZE_CP_SET_MAX_SHIFT, 0, TIME_TEST_TIMES);
#endif//#ifdef TEST_TIME
	}//3	for (uiIter = 0; uiIter < 2; uiIter ++)

	dma_printf("Test end \n");
}

static cyg_handle_t thread_handle;
static cyg_thread thread_data;
static char stack[CYGNUM_HAL_STACK_SIZE_TYPICAL * 2];
static void example_main(void)
{
	dma_printf("# HELLO 8600 FPGA, Build: %s, %s\n",__DATE__,__TIME__);
	CB_DMA_Test();
	dma_printf("# EXAMPLE MODULE DONE !\n");
}

void cbdma_example_main(void)
{
	cyg_thread_create(
				3,                                  // Priority - just a number
				(cyg_thread_entry_t *)example_main, // entry
				0,                                  // index
				0,                                  // no name
				&stack[0],                          // Stack
				sizeof(stack),                      // Size
				&thread_handle,                     // Handle
				&thread_data                        // Thread data structure
				);
	cyg_thread_resume(thread_handle);
}
#endif // HAVE_CB_DMA_TEST
#endif
