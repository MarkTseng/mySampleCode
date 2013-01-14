#ifndef	__AUDCLK_H
#define	__AUDCLK_H

/***********************************************************************
 *  2011.9.6 dylan.hong
 *  This file is all about audio PLLA clock  macro define
 ***********************************************************************/
#define	AUDCLK_XCK_ENABLE		0x4000
#define	AUDCLK_XCKPAD_ENABLE		0x2000

#define	AUDCLK_128XCK_04096     		0x0012      //128*32 = 4096
#define	AUDCLK_128XCK_05644	     		0x000C      //128*44.1 = 5644
#define	AUDCLK_128XCK_06144     		0x000C      //128*48 = 6144
#define	AUDCLK_128XCK_08192     		0x0009      //128*64 = 8192
#define	AUDCLK_128XCK_11290     		0x0006      //128*88.2 = 11290
#define	AUDCLK_128XCK_12288     		0x0006      //128*96 = 12288
#define	AUDCLK_128XCK_22579     		0x0003      //128*176.4 = 22579
#define	AUDCLK_128XCK_24576     		0x0003      //128*192 = 24576


#define	AUDCLK_256XCK_08192     		0x0009      //256*32 = 8192
#define	AUDCLK_256XCK_11290     		0x0006      //256*44.1 = 11290
#define	AUDCLK_256XCK_12288     		0x0006      //256*48 = 12288
#define	AUDCLK_256XCK_16384     		0x0000      //256*64 = 16384
#define	AUDCLK_256XCK_22579     		0x0003      //256*88.2 = 22579
#define	AUDCLK_256XCK_24576     		0x0003      //256*96 = 24576
#define	AUDCLK_256XCK_45158     		0x0000      //256*176.4 = 45158
#define	AUDCLK_256XCK_49152     		0x0000      //256*192 = 49152
#endif/*__AUDCLK_H*/

