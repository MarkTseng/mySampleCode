/***************************************************************************
**+----------------------------------------------------------------------+**
**|                                ****                                  |**
**|                                ****                                  |**
**|                                ******o***                            |**
**|                          ********_///_****                           |**
**|                           ***** /_//_/ ****                          |**
**|                            ** ** (__/ ****                           |**
**|                                *********                             |**
**|                                 ****                                 |**
**|                                  ***                                 |**
**|                                                                      |**
**|     Copyright (c) 1998 - 2009 Texas Instruments Incorporated         |**
**|                        ALL RIGHTS RESERVED                           |**
**|                                                                      |**
**| Permission is hereby granted to licensees of Texas Instruments       |**
**| Incorporated (TI) products to use this computer program for the sole |**
**| purpose of implementing a licensee product based on TI products.     |**
**| No other rights to reproduce, use, or disseminate this computer      |**
**| program, whether in part or in whole, are granted.                   |**
**|                                                                      |**
**| TI makes no representation or warranties with respect to the         |**
**| performance of this computer program, and specifically disclaims     |**
**| any responsibility for any damages, special or consequential,        |**
**| connected with the use of this program.                              |**
**|                                                                      |**
**+----------------------------------------------------------------------+**
***************************************************************************/

#ifndef __OMAP2430_SDIODRV_API_H
#define __OMAP2430_SDIODRV_API_H

#include <asm/types.h>
#include <linux/mmc/mmc.h>

#define TIWLAN_MMC_MAX_DMA                      (8192)
#define SDIO_DRIVER_NAME                        "TIWLAN_SDIO"
#define SDIO_TOTAL_FUNCS                        (2)
#define SDIO_WLAN_FUNC                          (2)
#define SDIO_BT_FUNC                            (1)
#define SDIO_CTRL_FUNC                          (0)


/* Card Common Control Registers (CCCR) */

#define CCCR_SDIO_REVISION                  0x00
#define CCCR_SD_SPECIFICATION_REVISION      0x01
#define CCCR_IO_ENABLE                      0x02
#define CCCR_IO_READY                       0x03
#define CCCR_INT_ENABLE                     0x04
#define CCCR_INT_PENDING                    0x05
#define CCCR_IO_ABORT                       0x06
#define CCCR_BUS_INTERFACE_CONTOROL         0x07
#define CCCR_CARD_CAPABILITY	            0x08
#define CCCR_COMMON_CIS_POINTER             0x09 /*0x09-0x0B*/
#define CCCR_FNO_BLOCK_SIZE	                0x10 /*0x10-0x11*/
#define FN0_CCCR_REG_32	                    0x64

/* Pprotocol defined constants */  
         
#define SD_IO_GO_IDLE_STATE		  		    0  
#define SD_IO_SEND_RELATIVE_ADDR	  	    3 
#define SDIO_CMD5			  			    5
#define SD_IO_SELECT_CARD		  		    7 
#define SDIO_CMD52		 	 			    52		
#define SDIO_CMD53		 	 			    53
//#define SD_IO_SEND_OP_COND		            SDIO_CMD5  
//#define SD_IO_RW_DIRECT			            SDIO_CMD52 
//#define SD_IO_RW_EXTENDED		            SDIO_CMD53 
#define SDIO_SHIFT(v,n)                     (v<<n)
#define SDIO_RWFLAG(v)                      (SDIO_SHIFT(v,31))
#define SDIO_FUNCN(v)                       (SDIO_SHIFT(v,28))
#define SDIO_RAWFLAG(v)                     (SDIO_SHIFT(v,27))
#define SDIO_BLKM(v)                        (SDIO_SHIFT(v,27))
#define SDIO_OPCODE(v)                      (SDIO_SHIFT(v,26))
#define SDIO_ADDRREG(v)                     (SDIO_SHIFT(v,9))


#define VDD_VOLTAGE_WINDOW                  0xffffc0
#define FN2_OBI_INV                         0x0002

//#define MMC_RSP_NONE	                    (0 << 0)
#define MMC_RSP_SHORT	                    (1 << 0)
#define MMC_RSP_LONG	                    (2 << 0)
#define MMC_RSP_MASK	                    (3 << 0)
//#define MMC_RSP_CRC	                        (1 << 3)
//#define MMC_RSP_BUSY	                    (1 << 4)

//#define MMC_RSP_R1	                        (MMC_RSP_SHORT|MMC_RSP_CRC)
//#define MMC_RSP_R1B	                        (MMC_RSP_SHORT|MMC_RSP_CRC|MMC_RSP_BUSY)
//#define MMC_RSP_R2	                        (MMC_RSP_LONG|MMC_RSP_CRC)
//#define MMC_RSP_R3	                        (MMC_RSP_SHORT)

/* HSMMC controller bit definitions
 * */
#define OMAP_HSMMC_CMD_NO_RESPONSE          0 << 0
#define OMAP_HSMMC_CMD_LONG_RESPONSE        1 << 0
#define OMAP_HSMMC_CMD_SHORT_RESPONSE       2 << 0

#define MMC_ERR_NONE	                    0
#define MMC_ERR_TIMEOUT	                    1
#define MMC_ERR_BADCRC	                    2
#define MMC_ERR_FIFO	                    3
#define MMC_ERR_FAILED	                    4
#define MMC_ERR_INVALID	                    5

#undef  MMC_RSP_R4
#define MMC_RSP_R4 OMAP_HSMMC_CMD_SHORT_RESPONSE
#undef  MMC_RSP_R5
#define MMC_RSP_R5 OMAP_HSMMC_CMD_SHORT_RESPONSE
#undef  MMC_RSP_R6
#define MMC_RSP_R6 OMAP_HSMMC_CMD_SHORT_RESPONSE

/********************************************************************/
/*	SDIO driver functions prototypes                                */
/********************************************************************/
int sdioDrv_ConnectBus     (void *       fCbFunc, 
                            void *       hCbArg, 
                            unsigned int uBlkSizeShift, 
                            unsigned int uSdioThreadPriority);

int sdioDrv_DisconnectBus  (void);

#if 0
int sdioDrv_ExecuteCmd     (unsigned int uCmd, 
                            unsigned int uArg, 
                            unsigned int uRespType, 
                            void *       pResponse, 
                            unsigned int uLen);
#endif
                           
int sdioDrv_ReadSync       (unsigned int uFunc, 
                            unsigned int uHwAddr, 
                            void *       pData, 
                            unsigned int uLen, 
                            unsigned int bIncAddr,
                            unsigned int bMore);

int sdioDrv_ReadAsync      (unsigned int uFunc, 
                            unsigned int uHwAddr, 
                            void *       pData, 
                            unsigned int uLen, 
                            unsigned int bBlkMode,
                            unsigned int bIncAddr,
                            unsigned int bMore);

int sdioDrv_WriteSync      (unsigned int uFunc, 
                            unsigned int uHwAddr, 
                            void *       pData, 
                            unsigned int uLen,
                            unsigned int bIncAddr,
                            unsigned int bMore);

int sdioDrv_WriteAsync     (unsigned int uFunc, 
                            unsigned int uHwAddr, 
                            void *       pData, 
                            unsigned int uLen, 
                            unsigned int bBlkMode,
                            unsigned int bIncAddr,
                            unsigned int bMore);

int sdioDrv_ReadSyncBytes  (unsigned int  uFunc, 
                            unsigned int  uHwAddr, 
                            unsigned char *pData, 
                            unsigned int  uLen, 
                            unsigned int  bMore);
                           
int sdioDrv_WriteSyncBytes (unsigned int  uFunc, 
                            unsigned int  uHwAddr, 
                            unsigned char *pData, 
                            unsigned int  uLen, 
                            unsigned int  bMore);
int sdioDrv_EnableFunction(unsigned int uFunc);
int sdioDrv_EnableInterrupt(unsigned int uFunc);
int sdioDrv_DisableFunction(unsigned int uFunc);
int sdioDrv_DisableInterrupt(unsigned int uFunc);
int sdioDrv_SetBlockSize(unsigned int uFunc, unsigned int blksz);
void sdioDrv_Register_Notification(void (*notify_sdio_ready)(void));
void sdioDrv_ReleaseHost(unsigned int uFunc);
void sdioDrv_ClaimHost(unsigned int uFunc);

int sdioDrv_init(void);
void sdioDrv_exit(void);

#endif/* _OMAP2430_SDIODRV_H */
