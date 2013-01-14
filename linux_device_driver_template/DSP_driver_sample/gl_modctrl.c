#include <asm/mach-spv9200/regmap_9200.h>
#include "include/gl_modctrl.h"

/*
 * If a module will be used in many ways, clock of the module is turned off
 * only when no one is using it. Therefore, we add a counter to count using
 * objects.
 */
/*
 * FIXME: argument "cnt" is used temporarily. There is no array to
 * store counters of every module, or we can use "m" to get the position
 * of the counter of specified module.
 */
#define CYG_MACRO_START do {
#define CYG_MACRO_END   } while (0)

#define HAL_RESTORE_INTERRUPTS(old)     \
	    CYG_MACRO_START                     \
    asm volatile (                      \
			        "mfc0   $8, $12;\n"                \
			        "and    %0, %0, 0x1;\n"           \
			        "or     $8, $8, %0;\n"            \
			        "mtc0   $8, $12;\n"               \
			        "ehb;\n"                          \
			        :                               \
			        : "r"(old)                      \
			        : "$8"                          \
			        );                              \
    CYG_MACRO_END

#define HAL_DISABLE_INTERRUPTS(old)     \
	    CYG_MACRO_START                     \
    asm volatile (                      \
			        "mfc0   $8, $12;\n"               \
			        "move   %0, $8;\n"                \
			        "and    $8, $8, 0xFFFFFFFE;\n"    \
			        "mtc0   $8, $12;\n"               \
			        "and    %0, %0, 0x1;\n"           \
			        "ehb;\n"                          \
			        : "=r"(old)                     \
			        :                               \
			        : "$8"                          \
			        );                              \
    CYG_MACRO_END


#define SAFE_CLKEN_ADD_ON(m, ori, cnt)								\
	do {															\
		unsigned int flag;											\
		HAL_DISABLE_INTERRUPTS(flag);								\
		(ori) = HWREG_R(clken[(m) / 32]);							\
		if (0 == (cnt)) {											\
			HWREG_W(clken[(m) / 32], (ori) | (1 << ((m) & 0x1F)));	\
		}															\
		++(cnt);													\
		HAL_RESTORE_INTERRUPTS(flag);								\
	} while (0)

#define SAFE_CLKEN_SUB_OFF(m, ori, cnt)								\
	do {															\
		unsigned int flag;											\
		HAL_DISABLE_INTERRUPTS(flag);								\
		(ori) = HWREG_R(clken[(m) / 32]);							\
		if (1 == (cnt)) {											\
			HWREG_W(clken[(m) / 32], (ori) & ~(1 << ((m) & 0x1F)));	\
		}															\
		--(cnt);													\
		HAL_RESTORE_INTERRUPTS(flag);								\
	} while (0)

static signed char clken_uart1 = 1;	/* clock of uart1 is enabled by default (in system_timing.S).  */


asm (
	"	.macro module_on orig, addr\n"
	"	.set push\n"
	"	.set noreorder\n"
	"	li		$3, 1\n"
	"	and		$4, 0x1F\n"				// It it unnecessary due to "sllv" now,
										// but maybe we need it in the future.
	"	sllv	$3, $3, $4\n"
	"1:\n"
	"	ll		\\orig, 0(\\addr)\n"	// orig = HWREG_R(reset[m / 32]);
	"	or		$8, \\orig, $3\n"		// orig = orig | (1 << (m & 0x1F));
	"	sc		$8, 0(\\addr)\n"		// HWREG_W(reset[m / 32], orig);
	"	beqz	$8, 1b\n"
	"	 and	\\orig, $3\n"
	"	srav	\\orig, \\orig, $4\n"
	"	and		\\orig, 0x01\n"
	"	.set pop\n"
	"	.endm\n"
);

asm (
	"	.macro module_off orig, addr\n"
	"	.set push\n"
	"	.set noreorder\n"
	"	li		$3, 1\n"
	"	and		$4, 0x1F\n"				// It it unnecessary due to "sllv" now,
										// but maybe we need it in the future.
	"	sllv	$3, $3, $4\n"
	"	nor		$9, $3, $0\n"
	"1:\n"
	"	ll		\\orig, 0(\\addr)\n"	// orig = HWREG_R(reset[m / 32]);
	"	and		$8, \\orig, $9\n"		// orig = orig & ~(1 << (m & 0x1F));
	"	sc		$8, 0(\\addr)\n"		// HWREG_W(reset[m / 32], orig);
	"	beqz	$8, 1b\n"
	"	 and	\\orig, $3\n"
	"	srav	\\orig, \\orig, $4\n"
	"	and		\\orig, 0x01\n"
	"	.set pop\n"
	"	.endm\n"
);

void GL_RegisterFileBarrier(void)
{
	register UINT32 t0;

	asm volatile ("lui		%0, 0xBFFE;"
			"lw		%0, 0(%0);"
			"sync"
			:"=r"(t0));
}

int GL_moduleResetOff(moduleList m)
{
	register int orig = 0;
	const unsigned int *addr = (const unsigned int *)HWREG_ADDR(reset[m / 32]);

	asm volatile(
		"module_off %0, %1\n"
		: "+r"(orig)
		: "r"(addr)
		: "$3", "$4", "$8", "$9");

	GL_RegisterFileBarrier();

	return orig;
}

int GL_moduleClockOn(moduleList m)
{
	register int orig = 0;

	switch (m) {
	case UART1:
	{
		SAFE_CLKEN_ADD_ON(m, orig, clken_uart1);
		orig = ((orig & (1 << (m & 0x1F))) != 0);
		break;
	}
	default:
	{
		const unsigned int *addr = (const unsigned int *)HWREG_ADDR(clken[m / 32]);
		asm volatile(
			"module_on %0, %1\n"
			: "+r"(orig)
			: "r"(addr)
			: "$3", "$4", "$8");
		break;
	}
	}

	GL_RegisterFileBarrier();
	return orig;
}

