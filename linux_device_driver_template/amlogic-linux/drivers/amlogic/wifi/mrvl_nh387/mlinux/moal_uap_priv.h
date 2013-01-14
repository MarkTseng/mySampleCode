/** @file  moal_uap_priv.h
  *
  * @brief This file contains definition for extended private IOCTL call.
  *
  * Copyright (C) 2010, Marvell International Ltd.
  *
  * This software file (the "File") is distributed by Marvell International
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991
  * (the "License").  You may use, redistribute and/or modify this File in
  * accordance with the terms and conditions of the License, a copy of which
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
  * this warranty disclaimer.
  *
  */

/************************************************************************
Change log:
    08/06/2010: initial version
************************************************************************/

#ifndef _MOAL_UAP_PRIV_H_
#define _MOAL_UAP_PRIV_H_

/** Maximum buffer length for WOAL_UAP_SET_GET_256_CHAR */
#define MAX_BUF_LEN                 256

/** Private command ID */
#define WOAL_UAP_IOCTL              0x8BE0

/** Private command to get/set 256 chars */
#define WOAL_UAP_SET_GET_256_CHAR   (WOAL_UAP_IOCTL + 1)
/** Private command ID to FW reload */
#define WOAL_WL_FW_RELOAD           1
/** Private command ID to set AP configuration */
#define WOAL_AP_SET_CFG             2

/** Private command ID to set/get none */
#define WOAL_UAP_SETNONE_GETNONE    (WOAL_UAP_IOCTL + 2)
/** Private command ID to start UAP */
#define WOAL_UAP_START              1
/** Private command ID to stop UAP */
#define WOAL_UAP_STOP               2
/** Private command ID to start AP BSS */
#define WOAL_AP_BSS_START           3
/** Private command ID to stop AP BSS */
#define WOAL_AP_BSS_STOP            4

/** Private command ID for hostcmd */
#define WOAL_UAP_HOST_CMD           (WOAL_UAP_IOCTL + 17)

/** The following command IDs are for Froyo app */
/** Private command ID to start AP BSS */
#define WOAL_UAP_FROYO_AP_BSS_START (WOAL_UAP_IOCTL + 24)
/** Private command ID to stop AP BSS */
#define WOAL_UAP_FROYO_AP_BSS_STOP  (WOAL_UAP_IOCTL + 26)
/** Private command ID to set AP config */
#define WOAL_UAP_FROYO_AP_SET_CFG   (WOAL_UAP_IOCTL + 27)
/** Private command ID to start driver */
#define WOAL_UAP_FROYO_START        (WOAL_UAP_IOCTL + 28)
/** Private command ID to reload FW */
#define WOAL_UAP_FROYO_WL_FW_RELOAD (WOAL_UAP_IOCTL + 29)
/** Private command ID to stop driver */
#define WOAL_UAP_FROYO_STOP         (WOAL_UAP_IOCTL + 30)

/**
 * iwpriv ioctl handlers
 */
static const struct iw_priv_args woal_uap_priv_args[] = {
    {
     WOAL_UAP_SETNONE_GETNONE,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     ""},
    {
     WOAL_UAP_START,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "start"},
    {
     WOAL_UAP_STOP,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "stop"},
    {
     WOAL_AP_BSS_START,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "bssstart"},
    {
     WOAL_AP_BSS_STOP,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "bssstop"},
    {
     WOAL_UAP_SET_GET_256_CHAR,
     IW_PRIV_TYPE_CHAR | 256,
     IW_PRIV_TYPE_CHAR | 256,
     ""},
    {
     WOAL_WL_FW_RELOAD,
     IW_PRIV_TYPE_CHAR | 256,
     IW_PRIV_TYPE_CHAR | 256,
     "fwreload"},
    {
     WOAL_AP_SET_CFG,
     IW_PRIV_TYPE_CHAR | 256,
     IW_PRIV_TYPE_CHAR | 256,
     "apcfg"},
    {
     WOAL_UAP_HOST_CMD,
     IW_PRIV_TYPE_BYTE | 2047,
     IW_PRIV_TYPE_BYTE | 2047,
     "hostcmd"},
    {
     WOAL_UAP_FROYO_START,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "START"},
    {
     WOAL_UAP_FROYO_STOP,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "STOP"},
    {
     WOAL_UAP_FROYO_AP_BSS_START,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "AP_BSS_START"},
    {
     WOAL_UAP_FROYO_AP_BSS_STOP,
     IW_PRIV_TYPE_NONE,
     IW_PRIV_TYPE_NONE,
     "AP_BSS_STOP"},
    {
     WOAL_UAP_FROYO_WL_FW_RELOAD,
     IW_PRIV_TYPE_CHAR | 256,
     IW_PRIV_TYPE_CHAR | 256,
     "WL_FW_RELOAD"},
    {
     WOAL_UAP_FROYO_AP_SET_CFG,
     IW_PRIV_TYPE_CHAR | 256,
     IW_PRIV_TYPE_CHAR | 256,
     "AP_SET_CFG"},
};

#endif /* _MOAL_UAP_PRIV_H_ */
