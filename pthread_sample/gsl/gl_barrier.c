#include <types.h>
#include <gsl/gl_types.h>
#include <gsl/lambo86_hwop.h>

/*
 * Barrier
 */
void GL_LumaBarrier(void)
{
	register UINT32 t0;

	t0 = *(volatile int *)lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_A].uncache;
	__asm__ __volatile__ ("sync");
}

void GL_ChromaBarrier(void)
{
	register UINT32 t0;

	t0 = *(volatile int *)lambo_hw_info.meminfo.mem[LAMBO_MEM_ADDR_SDRAM_B].uncache;
	__asm__ __volatile__ ("sync");
}

void GL_RegisterFileBarrier(void)
{
	register UINT32 t0;

	t0 = *(volatile int *)regs0;
	__asm__ __volatile__ ("sync");
}

