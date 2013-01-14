/*! \addtogroup GSL
 * @{
 */
/*!
*
* @file		gl_i2c.h
*
* @brief	This file specifies the i2c control APIs
*
* @note		Copyright (c) 2009 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
* @author
*
******************************************************************************/
#ifndef __GL_I2C_H__
#define __GL_I2C_H__

/*!
 * @defgroup GL_I2C I2C Control APIs
 * @brief I2C Control APIs
 * @{
*/
/******************************************************************************/
/*!
 * @brief	Turn on I2C hand-shake.
 *
 * @note	Can only do once time before setting AD/DA or MAC.
 *******************************************************************************/
void GL_TurnONI2cOtp( void );

/******************************************************************************/
/*!
 * @brief	Read i2c register value.
 * @param	offset offset value of i2c register
 * @return	@a value of specific i2c register
 * @note	There is a while loop inside to check i2c status if ready or not.
 *******************************************************************************/
int GL_ReadI2cReg(int offset);

/******************************************************************************/
/*!
 * @brief	Write i2c register value.
 * @param	offset offset value of i2c register
 * @param	data the value to be written into i2c register
 * @note	There is a while loop inside to check i2c status if ready or not.
 *******************************************************************************/
void GL_WriteI2cReg(int offset, int data);

/*! @} end of defgroup GL_I2C */

#endif // __GL_I2C_H__

/*! @} end of addtogroup GSL */
