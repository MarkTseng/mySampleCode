	.file	1 "float.c"
	.section .mdebug.abi32
	.previous
	.gnu_attribute 4, 3
	.abicalls
	.globl	sin_table
	.data
	.align	2
	.type	sin_table, @object
	.size	sin_table, 32
sin_table:
	.word	0
	.word	52701887
	.word	105372028
	.word	157978697
	.word	210490206
	.word	262874923
	.word	315101295
	.word	367137861
	.globl	__divdf3
	.rdata
	.align	2
$LC2:
	.ascii	"r = %f\012\000"
	.globl	__subdf3
	.text
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function
main:
	.frame	$fp,56,$31		# vars= 24, regs= 2/0, args= 16, gp= 8
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-56
	sw	$31,52($sp)
	sw	$fp,48($sp)
	move	$fp,$sp
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	.cprestore	16
	lui	$2,%hi($LC0)
	lw	$3,%lo($LC0+4)($2)
	lw	$2,%lo($LC0)($2)
	sw	$2,24($fp)
	sw	$3,28($fp)
	lui	$2,%hi($LC1)
	lw	$3,%lo($LC1+4)($2)
	lw	$2,%lo($LC1)($2)
	sw	$2,32($fp)
	sw	$3,36($fp)
	lw	$2,%call16(__divdf3)($28)
	lw	$4,24($fp)
	lw	$5,28($fp)
	lw	$6,32($fp)
	lw	$7,36($fp)
	move	$25,$2
	.reloc	1f,R_MIPS_JALR,__divdf3
1:	jalr	$25
	nop

	lw	$28,16($fp)
	sw	$2,40($fp)
	sw	$3,44($fp)
	lui	$2,%hi($LC2)
	addiu	$2,$2,%lo($LC2)
	move	$4,$2
	lw	$6,40($fp)
	lw	$7,44($fp)
	lw	$2,%call16(printf)($28)
	move	$25,$2
	.reloc	1f,R_MIPS_JALR,printf
1:	jalr	$25
	nop

	lw	$28,16($fp)
	lw	$2,%call16(__subdf3)($28)
	lw	$4,24($fp)
	lw	$5,28($fp)
	lw	$6,32($fp)
	lw	$7,36($fp)
	move	$25,$2
	.reloc	1f,R_MIPS_JALR,__subdf3
1:	jalr	$25
	nop

	lw	$28,16($fp)
	sw	$2,40($fp)
	sw	$3,44($fp)
	lui	$2,%hi($LC2)
	addiu	$2,$2,%lo($LC2)
	move	$4,$2
	lw	$6,40($fp)
	lw	$7,44($fp)
	lw	$2,%call16(printf)($28)
	move	$25,$2
	.reloc	1f,R_MIPS_JALR,printf
1:	jalr	$25
	nop

	lw	$28,16($fp)
	move	$2,$0
	move	$sp,$fp
	lw	$31,52($sp)
	lw	$fp,48($sp)
	addiu	$sp,$sp,56
	j	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.rdata
	.align	3
$LC0:
	.word	0
	.word	1080623104
	.align	3
$LC1:
	.word	858993459
	.word	1081807667
	.ident	"GCC: (crosstool-NG hg_unknown@20110630.110740 - 20110726) 4.5.3"
