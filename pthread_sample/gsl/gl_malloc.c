#include <gsl.h>
#include "gl_common.h"

#ifdef MEMORY_WATCH
#undef GL_MemAlloc
#undef GL_MemRealloc
#undef GL_MemFree
#endif

#define GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR	0
#define GL_MEMALLOC_ALIGNED_EXTRA_SIZE	(2 * HAL_DCACHE_LINE_SIZE + 8)

#define	RETURN_IF_MALLOC_ZERO()											\
	do {																\
		if (0 == dSize) {												\
			return (void *)GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR;	\
		}																\
    } while (0)

#define	RETURN_IF_FREE_SPECIAL()												\
	do {																		\
		if (GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR == (unsigned int)pAddr) {	\
			return GL_SUCCESS;													\
		}																		\
	} while (0)

#if GL_USE_BGET_MALLOC
#define MALLOC(args...)	bget_malloc(args)
#define REALLOC(args...)	bget_realloc(args)
#define MALLOCA(args...)	bget_mallocA(args)
#define REALLOCA(args...)	bget_reallocA(args)
#define MALLOCB(args...)	bget_mallocB(args)
#define REALLOCB(args...)	bget_reallocB(args)
#define FREE(args...)		bget_free(args)
#else
#define MALLOC(args...)	malloc(args)
#define REALLOC(args...)	realloc(args)
#define MALLOCA(args...)	mallocA(args)
#define REALLOCA(args...)	reallocA(args)
#define MALLOCB(args...)	mallocB(args)
#define REALLOCB(args...)	reallocB(args)
#define FREE(args...)		free(args)
#endif

void *GL_MemAlloc(UINT32 dSize)
{
#if 0
	RETURN_IF_MALLOC_ZERO();
	return MALLOC(dSize);
#else
	return malloc(dSize);
#endif
}

void *GL_MemRealloc(void *ptr, UINT32 dSize)
{
#if 0
	if (dSize == 0) {
		if (ptr) {
			FREE(ptr);
		}
		return (void *)GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR;
	}
	return REALLOC(ptr, dSize);
#else
	return realloc(ptr, dSize);
#endif
}

static inline void *gl_MemAllocAligned_inner(void *ptr, UINT32 dSize)
{
	UINT32 *memaddr;

	if (ptr != NULL) {
		memaddr = (UINT32 *)((((UINT32)ptr + HAL_DCACHE_LINE_SIZE + 8) & (~(HAL_DCACHE_LINE_SIZE - 1))) - 8);

		/* Store original pointer and size.  */
		*memaddr++ = dSize;	// only use for ReallocAligned
		*memaddr++ = (UINT32)ptr;

		return memaddr;
	}
	return NULL;
}

void *GL_MemAllocAligned(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	// head and tail padding
	return gl_MemAllocAligned_inner(MALLOC(dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE), dSize);
}

void *GL_MemReallocAligned(void *ptr, UINT32 dSize)
{
	UINT32 *tmp;
	UINT32 *ori, *new;
	UINT32 ori_size;
	UINT8 off1, off2;
	const unsigned int cacheline_mask = HAL_DCACHE_LINE_SIZE - 1;

	if (dSize == 0) {
		if (ptr) {
			GL_MemFreeAligned(ptr);
		}
		return (void *)GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR;
	}

	/* read the real memory pointer getting from malloc(). */
	tmp = (UINT32 *)ptr;
	ori = (UINT32 *)*(tmp - 1);
	ori_size = *(tmp - 2);

	new = REALLOC((void *)ori, dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE);
	if (!new) {
		return NULL;
	}
	/*
	 *   If the offset between malloc result and cacheline-aligned pointer is
	 *   the same, the contents will be correct.
	 *
	 *   If not, we should move them to the right position.
	 */
	off1 = (UINT8)((UINT32)ori & cacheline_mask);
	off2 = (UINT8)((UINT32)new & cacheline_mask);
	if (off1 == off2) {
		return gl_MemAllocAligned_inner(new, dSize);
	} else {
		void *src = (void *)((UINT32)new + (HAL_DCACHE_LINE_SIZE - off1));
		tmp = new;

		new = (UINT32 *)(((UINT32)new + HAL_DCACHE_LINE_SIZE + 8) & (~(HAL_DCACHE_LINE_SIZE - 1)));

		memmove(new, src, ori_size);
		/* Store original pointer and size.  */
		*(new - 2) = dSize;	// only use for ReallocAligned
		*(new - 1) = (UINT32)tmp;

		return new;
	}
}

static inline void *gl_MemAllocUncached_inner(void *ptr, UINT32 dSize)
{
	UINT32 *memaddr = NULL;

	if (ptr != NULL) {
		memaddr = (UINT32 *)((((UINT32)ptr + HAL_DCACHE_LINE_SIZE + 8) & (~(HAL_DCACHE_LINE_SIZE - 1))) - 8);
		*memaddr++ = dSize;
		*memaddr++ = (UINT32)ptr;
		GL_DataCacheFlushAndInvalidate(memaddr, (dSize + 32) & ~31);

#if GL_USE_BGET_MALLOC
		memaddr = GL_CACHE_TO_UNCACHE(memaddr);
#else
		memaddr = (void *)(((UINT32)memaddr) | 0x20000000);
#endif
	}
	return memaddr;
}

void *GL_MemAllocUncached(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return gl_MemAllocUncached_inner(MALLOC(dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE), dSize);
}

inline void *GL_MemAllocA(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return MALLOCA(dSize);
}

inline void *GL_MemAllocB(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return MALLOCB(dSize);
}

void *GL_MemAllocAlignedA(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return gl_MemAllocAligned_inner(MALLOCA(dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE), dSize);
}

void *GL_MemAllocAlignedB(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return gl_MemAllocAligned_inner(MALLOCB(dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE), dSize);
}

void *GL_MemAllocUncachedA(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return gl_MemAllocUncached_inner(MALLOCA(dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE), dSize);
}

void *GL_MemAllocUncachedB(UINT32 dSize)
{
	RETURN_IF_MALLOC_ZERO();
	return gl_MemAllocUncached_inner(MALLOCB(dSize + GL_MEMALLOC_ALIGNED_EXTRA_SIZE), dSize);
}

void *GL_MemReallocA(void *ptr, UINT32 dSize)
{
	if (dSize == 0) {
		if (ptr) {
			free(ptr);
		}
		return (void *)GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR;
	}
	return REALLOCA(ptr, dSize);
}

void *GL_MemReallocB(void *ptr, UINT32 dSize)
{
	if (dSize == 0) {
		if (ptr) {
			free(ptr);
		}
		return (void *)GL_MEMALLOC_MALLOC_ZERO_SPECIAL_ADDR;
	}
	return REALLOCB(ptr, dSize);
}

/* inline for GL_MemFreeAligned and GL_MemFreeUncached */
inline GL_Status_t GL_MemFree(void *pAddr)
{
#if 0
	RETURN_IF_FREE_SPECIAL();
	FREE(pAddr);
#else
	free(pAddr);
#endif
	return GL_SUCCESS;
}

inline GL_Status_t GL_MemFreeA(void *pAddr)  //welson
{
#if 1
	RETURN_IF_FREE_SPECIAL();
	FREE(pAddr);
#else
	free(pAddr);
#endif
	return GL_SUCCESS;
}

GL_Status_t GL_MemFreeAligned(void *pAddr)
{
	RETURN_IF_FREE_SPECIAL();
	FREE((void *)(*(UINT32 *)((UINT32)pAddr - 4)));
	return GL_SUCCESS;
}

GL_Status_t GL_MemFreeUncached(void *pAddr)
{
	UINT32 *memaddr;

	RETURN_IF_FREE_SPECIAL();

#if GL_USE_BGET_MALLOC
	memaddr = (UINT32 *)((UINT32)GL_UNCACHE_TO_CACHE(pAddr) - 4);
#else
	memaddr = (UINT32 *)((((UINT32)pAddr) & (~0x20000000)) - 4);
#endif

	FREE((void *)(*memaddr));
	return GL_SUCCESS;
}

UINT8 GL_MemValidateAddress(void *pAddr)
{
	if (pAddr != NULL) {
		return TRUE;
	}
	return FALSE;
}

#if 0//#ifdef CYGDBG_MEMALLOC_FREE_RESET
#if (GL_FREE_AND_SET_DEADBEEF == 0)
INT32 GL_MemFreeClearOn(void)
{
	diag_printf("Please set definition \"GL_FREE_AND_SET_DEADBEEF\" to be \"1\"\n");
}

INT32 GL_MemFreeClearOff(void)
{
	diag_printf("Please set definition \"GL_FREE_AND_SET_DEADBEEF\" to be \"1\"\n");
}

void cyg_memalloc_free_callback(cyg_GL_UINT32 *ptr, cyg_GL_UINT32 sz)
{
	/* do nothing */
}
#else
INT8 free_clear_switch = 0;

INT32 GL_MemFreeClearOn(void)
{
	INT32 prev = free_clear_switch;
	free_clear_switch = 1;
	diag_printf("Free and clear function is: On\n");
	return prev;
}

INT32 GL_MemFreeClearOff(void)
{
	INT32 prev = free_clear_switch;
	free_clear_switch = 0;
	diag_printf("Free and clear function is: Off\n");
	return prev;
}

void cyg_memalloc_free_callback(cyg_GL_UINT32 *ptr, cyg_GL_UINT32 sz)
{
	if (free_clear_switch) {
		cyg_GL_UINT32 *ptr_top = (cyg_GL_UINT32 *)((cyg_uint8 *)ptr + sz);
		while (ptr + 8 <= ptr_top) {
			*(ptr + 0) = 0xDEADBEEF;
			*(ptr + 1) = 0xDEADBEEF;
			*(ptr + 2) = 0xDEADBEEF;
			*(ptr + 3) = 0xDEADBEEF;
			*(ptr + 4) = 0xDEADBEEF;
			*(ptr + 5) = 0xDEADBEEF;
			*(ptr + 6) = 0xDEADBEEF;
			*(ptr + 7) = 0xDEADBEEF;
			ptr += 8;
		}
		/* We don't care about the condition that size of remainder is smaller than 4 bytes,
		 * because malloc guarantees at least 4-byte alignement.
		 */
		while (ptr < ptr_top) {
			*ptr++ = 0xDEADBEEF;
		}
	}
}
#endif
#endif

UINT32 GL_ProbeDramSize(void)
{
	return GL_ProbeDramASize();
}

UINT32 GL_ProbeDramASize(void)
{
#if GL_USE_BGET_MALLOC
	return GL_POOLA_SIZE;
#else
	return 64;
#endif
}

UINT32 GL_ProbeDramBSize(void)
{
#if GL_USE_BGET_MALLOC
	return GL_POOLB_SIZE;
#else
	return 64;
#endif
}

