#include <sys/regdef.h>
#include <sys/asm.h>

#define RF_GRP(group, reg)    (4 * (((group) - 256) * 32 + (reg)))

/* FIXME: We should include correct file to define RGST_OFFSET */
#define RGST_OFFSET 0xBFFE8000

/*
 * Used as the ll/sc target memory
 */
//	.section .bss._gsl_sLockStc
//_gsl_sLockStc:
//	.word	0
	.comm _gsl_sLockStc,4,4

	.text

/* unsigned long long GL_GetStc64 (void) */
LEAF(GL_GetStc64)
	 .set	noreorder
	 .extern rgst_offset
#ifdef SHARED_LIB
	 .cpload t9
	 lw		t0, %got(_gsl_sLockStc)(gp)
	 lw		t3, %got(rgst_offset)(gp)
	 lw		t3, 0(t3)
#else
	 la		t0, _gsl_sLockStc
	 lui	t3, %hi(rgst_offset)
	 lw		t3, %lo(rgst_offset)(t3)
#endif
1:
	 ll		zero, 0(t0)
	 sw		zero, RF_GRP(12,26)(t3)
	 sw		zero, RF_GRP(12, 26)(t3)
	 lw		v0, RF_GRP(12, 24)(t3)		# STCL_0
	 lw		t2, RF_GRP(12, 25)(t3)		# STCL_1
	 lw		v1, RF_GRP(12, 26)(t3)		# STCL_2
	 sc		t1, 0(t0)
	 beqz	t1, 1b
	  sll	t2, 16
	 jr		ra
	  or	v0, t2
END (GL_GetStc64)

/* unsigned long long GL_GetStc32 (void) */
LEAF(GL_GetStc32)
	 .set	noreorder
	 .extern rgst_offset
#ifdef SHARED_LIB
	 .cpload t9
	 lw		t0, %got(_gsl_sLockStc)(gp)
	 lw		t3, %got(rgst_offset)(gp)
	 lw		t3, 0(t3)
#else
	 la		t0, _gsl_sLockStc
	 lui	t3, %hi(rgst_offset)
	 lw		t3, %lo(rgst_offset)(t3)
#endif
1:
	 ll		zero, 0(t0)
	 sw		zero, RF_GRP(12, 26)(t3)	# STCL_2
	 lw		t2, RF_GRP(12, 25)(t3)		# STCL_1
	 lw		v0, RF_GRP(12, 24)(t3)		# STCL_0
	 sc		t1, 0(t0)
	 beqz	t1, 1b
 	  sll	t2, 16
	 jr		ra
	  or	v0, t2
END(GL_GetStc32)


/* void GL_SetStc(unsigned long long stc) */
LEAF(GL_SetStc)

/*
 * Use di/ei pair to get exclusive access first
 * Use ll/sc pair to notify others which are accessing STC
*/
	 .set	noreorder
	 .extern rgst_offset
#ifdef SHARED_LIB
	 .cpload t9
	 lw		t0, %got(_gsl_sLockStc)(gp)
	 lw		t3, %got(rgst_offset)(gp)
	 lw		t3, 0(t3)
#else
	 la		t0, _gsl_sLockStc
	 lui	t3, %hi(rgst_offset)
	 lw		t3, %lo(rgst_offset)(t3)
#endif
	 srl	t1, a0, 16
#	 di		v0
#	 andi	v0, v0, 1
1:
	 ll		zero, 0(t0)
	 sw		a1, RF_GRP(12, 2)(t3)	# STC_32
	 sw		t1, RF_GRP(12, 1)(t3)	# STC_31_16
	 sw		a0, RF_GRP(12, 0)(t3)	# STC_15_0
	 sc		t2, 0(t0)
	 beqz	t2, 1b
	  nop
#	 beqz	v0, 1f
#	  nop
#	 ei
#1:
	 jr		ra
	  nop
END(GL_SetStc)

