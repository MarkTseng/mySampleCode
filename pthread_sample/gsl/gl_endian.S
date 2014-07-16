#include <sys/regdef.h>
#include <sys/asm.h>

	 .text

/* void GL_wle32(BYTE *p, UINT32 x); */
LEAF(GL_wle32)
	 srl	v0, a1, 24	// v0 = a1 >> 24
	 sb	v0, 3(a0)	// a0[3] = a1 >> 24
	 srl	v0, a1, 16	// v0 = a1 >> 16
	 sb	v0, 2(a0)	// a0[2] = a1 >> 16

/*	void GL_wle16(BYTE *p, UINT32 x); */
	 .global	GL_wle16
	 .aent		GL_wle16
GL_wle16:
	 srl	v0, a1, 8	// v0 = a1 >> 8
	 sb	v0, 1(a0)	// a0[1] = a1 >> 8
	 sb	a1, 0(a0)	// a0[0] = a1
	 jr	ra		// ret
	 END(GL_wle32)


/*	void GL_wbe32(BYTE *p, UINT32 x); */

LEAF(GL_wbe32)
	 srl	v0, a1, 24	// v0 = a1 >> 24
	 sb	v0, 0(a0)	// a0[0] = v0;
	 srl	v0, a1, 16	// v0 = a1 >> 16
	 sb	v0, 1(a0)	// a0[1] = v0;
	 addiu	a0, 2		// a0 += 2;

/* void GL_wbe16(BYTE *p, UINT32 x); */
	 .global	GL_wbe16
	 .aent		GL_wbe16
GL_wbe16:
	 srl	v0, a1, 8	// v0 = a1 >> 8
	 sb	v0, 0(a0)	// a0[0] = v0
	 sb	a1, 1(a0)	// a0[1] = a1
	 jr	ra		// ret
	 END(GL_wbe32)


/* UINT32 GL_lle32(BYTE *p); */
LEAF(GL_lle32)
	 lbu	v0, 3(a0)
	 lbu	v1, 2(a0)
	 sll	v0, 8
	 or	v0, v1		// v0 = (a[3]<<8) | a[2]
	 lb	v1, 1(a0)
	 lbu	a0, 0(a0)
	 sll	v0, 8
	 or	v0, v1		// v0 = (v0<<8) | a[1];
	 sll	v0, 8
	 or	v0, a0		// v0 = (v0<<8) | a[0]
	 jr	ra
	 END(GL_lle32)

/* UINT32 GL_lle16(BYTE *p); */
LEAF(GL_lle16)
	 lbu	v0, 1(a0)
	 lbu	v1, 0(a0)
	 sll	v0, 8
	 or	v0, v1
	 jr	ra
	 END(GL_lle16)



/* UINT32 GL_lbe32(BYTE *p); */
LEAF(GL_lbe32)
	 lbu	v0, 0(a0)
	 lbu	v1, 1(a0)
	 sll	v0, 8
	 or	v0, v1
	 lbu	v1, 2(a0)
	 lbu	a0, 3(a0)
	 sll	v0, 8
	 or	v0, v1
	 sll	v0, 8
	 or	v0, a0
	 jr	ra
	 END(GL_lbe32)

/* UINT32 GL_lbe16(BYTE *p); */
LEAF(GL_lbe16)
	 lbu	v0, 0(a0)
	 lbu	v1, 1(a0)
	 sll	v0, 8
	 or	v0, v1
	 jr	ra
	 END(GL_lbe16)

