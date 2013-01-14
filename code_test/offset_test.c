#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef unsigned int	UINT32;

typedef struct
{
	// GROUP 0:	Chip Information
	UINT32	stamp;                                  /* 00    0		(0000) $bffe8000*/
	UINT32	emulation;                              /* 01    1		(0004)*/
	UINT32  g0_reserved_2;				/* 02    2              (0008)*/
	UINT32	g0_reserved_3;                          /* 03    3		(000c)*/
	UINT32	clk_sel0;                               /* 04    4		(0010)*/
	UINT32	clk_sel1;                               /* 05    5		(0014)*/
	UINT32	sspll_cfg;                              /* 06    6		(0018)*/
	UINT32	g0_reserved_7;                          /* 07    7		(001c)*/
	UINT32	clken0;                                 /* 08    8		(0020)*/
	UINT32	clken1;                                 /* 09    9		(0024)*/
	UINT32	clken2;                                 /* 0a   10		(0028)*/
	UINT32	clken3;                                 /* 0b   11		(002c)*/
	UINT32	gclken0;                                /* 0c   12		(0030)*/
	UINT32	gclken1;                                /* 0d   13		(0034)*/
	UINT32	gclken2;                                /* 0e   14		(0038)*/
	UINT32	gclken3;                                /* 0f   15		(003c)*/
	UINT32	reset0;                                 /* 10   16		(0040)*/
	UINT32	reset1;                                 /* 11   17		(0044)*/
	UINT32	reset2;                                 /* 12   18		(0048)*/
	UINT32	reset3;                                 /* 13   19		(004c)*/
	UINT32	g0_reserved_20;                         /* 14   20		(0050)*/
	UINT32	g0_reserved_21;                         /* 15   21		(0054)*/
	UINT32	g0_reserved_22;                         /* 16   22		(0058)*/
	UINT32	g0_reserved_23;                         /* 17   23		(005c)*/
	UINT32	hw_bo0;                                 /* 18   24		(0060)*/
	UINT32	hw_bo1;                                 /* 19   25		(0064)*/
	UINT32	hw_bo2;                                 /* 1a   26		(0068)*/
	UINT32	hw_bo3;                                 /* 1b   27		(006c)*/
	UINT32	hw_cfg;                                 /* 1c   28		(0070)*/
	UINT32	hw_cfg_chg;                             /* 1d   29		(0074)*/
	UINT32	g0_reserved_30;                         /* 1e   30		(0078)*/
	UINT32	show_bo_stamp;                          /* 1f   31      	(007c)*/

	// GROUP 1:
	UINT32 sft_cfg0;                        	// 18   56~  63 (00e0)
	UINT32 sft_cfg1;
	UINT32 sft_cfg2;
	UINT32 sft_cfg3;
	UINT32 sft_cfg4;
	UINT32 sft_cfg5;
	UINT32 sft_cfg6;
	UINT32 sft_cfg7;
	UINT32 sft_cfg8;
	UINT32 sft_cfg9;
	UINT32 sft_cfg10;
	UINT32 sft_cfg11;
	UINT32 sft_cfg12;
	UINT32 sft_cfg13;
	UINT32 sft_cfg14;
	UINT32 sft_cfg15;
	UINT32 sft_cfg16;
	UINT32 sft_cfg17;
	UINT32 sft_cfg18;
	UINT32 sft_cfg19;
	UINT32 sft_cfg20;
	UINT32 sft_cfg21;
	UINT32 sft_cfg22;
	UINT32 sft_cfg23;
	UINT32 sft_cfg24;
	UINT32 sft_cfg25;
	UINT32 sft_cfg26;
	UINT32 sft_cfg27;
	UINT32 sft_cfg28;
	UINT32 sft_cfg29;
	UINT32 sft_cfg30;
	UINT32 sft_cfg31;

} RegisterFile;

#define regs0	((RegisterFile *)(0))
#define REG_OFFSET(MEM) (UINT32)(&(MEM))

int main()
{
	printf("clken1 offset: %d\n",REG_OFFSET(regs0->clken1));

	return 0;
}
