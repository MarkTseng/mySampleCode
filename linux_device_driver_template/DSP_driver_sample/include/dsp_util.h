/*!                                                                       
************************************************************************* 
* \file    dsp_util.h                                          
*                                                                         
* \brief   audio dsp utilities
* \note    
*                                                                         
* \date    2008/12/16                                                   
*                                                                         
* \author  - jason.su        <jason.su@sunplus.com>                       
*                                                                         
************************************************************************* 
*/                                                                        
 

#ifndef __DSP_UTIL_H__
#define __DSP_UTIL_H__
#include <linux/types.h>
/******************************************************************************
	Defines
 *****************************************************************************/

/******************************************************************************
        Data type defines
 *****************************************************************************/



// ========================================================
// DSP OCEM module
// ========================================================
typedef enum _DSP_OCEM_CMD
{
	OCEM_DVM1		= 0x04, // Data value match register
	OCEM_OFIO7P		= 0x08, // File I/O #7 register protected
	OCEM_OFIO8P		= 0x0C, // File I/O #8 register protected
	OCEM_PADD1		= 0x10, // Program address BP #1 address value
	OCEM_PADD2		= 0x14, // Program address BP #2 address value
	OCEM_OFIO1		= 0x18, // File I/O #1 register
	OCEM_OFIO2		= 0x1C, // File I/O #2 register 
	OCEM_PCOUNT1	= 0x20, // Program address BP #1 count value
	OCEM_PCOUNT2	= 0x24, // Program address BP #2 count value
	OCEM_OFIO3		= 0x28, // File I/O #3 register
	OCEM_OFIO4		= 0x2C, // File I/O #4 register
	OCEM_DADD_LOW	= 0x30, // Data address BP low address value
	OCEM_TKL3_D2_PC	= 0x34, // PC of the D2 Stage
	OCEM_OFIO5		= 0x38, // File I/O #5 register
	OCEM_OFIO6		= 0x3c, // File I/O #6 register
	OCEM_DADD_HIGH	= 0x40, // Data address BP high address value
	OCEM_OFIO7		= 0x48, // File I/O #7 register
	OCEM_OFIO8		= 0x4c, // File I/O #8 register
	OCEM_CONTROL	= 0x50, // OCEM control register
	OCEM_SA_BP_EN	= 0x54, // Standalone BP Enable Register
	OCEM_STATUS 	= 0x60, // General status register
	OCEM_SA_BP_ST	= 0x64, // Standalone BP Status Register
	OCEM_CLOCK	    = 0x70, // Pseudo-register, enables the debugger to apply clock cycles
	OCEM_TL3_VER	= 0x72, // The version register of the TL3 (default value: 0x3210_0110)
	OCORE_DRD		= 0xfc, // The core data read register
	OCEM_INST_SCAN  = 0x80, // Instruction scan chain(used to inject instructions to the core)
}DSP_OCEM_CMD;



/******************************************************************************
        Globals
 *****************************************************************************/  

/******************************************************************************
	Macros
 *****************************************************************************/

/******************************************************************************
        C Funciton Prototype
 *****************************************************************************/

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void write16(UINT16 offset, UINT16 nValue);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void dumpDspIDM(UINT32 pData, UINT32 nByteSize);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void dumpByte(UINT32 pData, UINT32 nByteSize);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void dumpWord16(UINT32 pData, UINT32 nWord16Size);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void dumpWord24(UINT32 pData, UINT32 nWord24Size);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
 
void dumpWord32(UINT32 pData, UINT32 nWord32Size);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
 
void dumpHex(UINT32 pData, UINT32 nHexWordSize);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
UINT32 DSP_GenChecksum(const UINT8 *pCode, UINT32 size);


/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void DSP_Memcpy(void *dest, const void *src, UINT32 n);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void DSP_Memcpy_Cached(void *dest, const void *src, size_t n);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void SetFpgaEnvironment(void);

/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
int ocm_io_read(int OCM_REG);
/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void ocm_io_write(int WRITE_DATA);
/*!
  \brief    describe template_fun1 in brief
  \param    inVar  [IN]1st input variable
  \param    pInBuf [IN]2nd input variable, a pointer to buffer
  \see      template_fun2
  \return   None
  \note     describe the template_fun1 in more detail, if needed
            also anything needs to note
 */
void ocm_ins_scan(int REG0, int REG1, int REG2, int REG3, int REG4, int REG5, int REG6, int REG7);

void dspDumpPCM(void);

void dumpCodecNowID(void);

void DSP_Printf(UINT32 id, const char *format,...);

//void DSP_FileLog(const char *format,...);

#endif //__DSP_UTIL_H__
