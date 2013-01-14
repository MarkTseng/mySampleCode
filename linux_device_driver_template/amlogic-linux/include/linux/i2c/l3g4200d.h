/******************** (C) COPYRIGHT 2010 STMicroelectronics ********************
*
* File Name		: l3g4200d.h
* Authors		: MH - C&I BU - Application Team
*			: Carmine Iascone (carmine.iascone@st.com)
*			: Matteo Dameno (matteo.dameno@st.com)
*			: Both authors are willing to be considered the contact
*			: and update points for the driver.
* Version		: V 1.1.3 sysfs
* Date			: 2011/Jun/24
* Description		: L3G4200D digital output gyroscope sensor API
*
********************************************************************************
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* THE PRESENT SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES
* OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, FOR THE SOLE
* PURPOSE TO SUPPORT YOUR APPLICATION DEVELOPMENT.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
********************************************************************************
* REVISON HISTORY
*
* VERSION	| DATE		| AUTHORS		| DESCRIPTION
* 1.0		| 2010/Aug/19	| Carmine Iascone	| First Release
* 1.1.0		| 2011/02/28	| Matteo Dameno		| Self Test Added
* 1.1.1		| 2011/05/25	| Matteo Dameno		| Corrects Polling Bug
* 1.1.2		| 2011/05/30	| Matteo Dameno		| Corrects ODR Bug
* 1.1.3		| 2011/06/24	| Matteo Dameno		| Corrects ODR Bug
*******************************************************************************/

#ifndef __L3G4200D_H__
#define __L3G4200D_H__

#define L3G4200D_MIN_POLL_PERIOD_MS	2

#define SAD0L				0x00
#define SAD0H				0x01
#define L3G4200D_GYR_I2C_SADROOT	0x34
#define L3G4200D_GYR_I2C_SAD_L		((L3G4200D_GYR_I2C_SADROOT<<1)|SAD0L)
#define L3G4200D_GYR_I2C_SAD_H		((L3G4200D_GYR_I2C_SADROOT<<1)|SAD0H)

#define L3G4200D_GYR_DEV_NAME		"l3g4200d_gyr"

#define L3G4200D_GYR_FS_250DPS	0x00
#define L3G4200D_GYR_FS_500DPS	0x10
#define L3G4200D_GYR_FS_2000DPS	0x30

#define L3G4200D_GYR_ENABLED	1
#define L3G4200D_GYR_DISABLED	0

#ifdef __KERNEL__
struct l3g4200d_gyr_platform_data {
	int (*init)(void);
	void (*exit)(void);
	int (*power_on)(void);
	int (*power_off)(void);
	unsigned int poll_interval;
	unsigned int min_interval;

	u8 fs_range;

	u8 axis_map_x;
	u8 axis_map_y;
	u8 axis_map_z;

	u8 negate_x;
	u8 negate_y;
	u8 negate_z;
};
#endif /* __KERNEL__ */

#endif  /* __L3G4200D_H__ */
