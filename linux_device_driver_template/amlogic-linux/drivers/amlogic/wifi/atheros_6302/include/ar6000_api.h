//------------------------------------------------------------------------------
// <copyright file="ar6000_api.h" company="Atheros">
//    Copyright (c) 2004-2008 Atheros Corporation.  All rights reserved.
// 
// The software source and binaries included in this development package are
// licensed, not sold. You, or your company, received the package under one
// or more license agreements. The rights granted to you are specifically
// listed in these license agreement(s). All other rights remain with Atheros
// Communications, Inc., its subsidiaries, or the respective owner including
// those listed on the included copyright notices.  Distribution of any
// portion of this package must be in strict compliance with the license
// agreement(s) terms.
// </copyright>
// 
// <summary>
// 	Wifi driver for AR6002
// </summary>
//
//------------------------------------------------------------------------------
//==============================================================================
// This file contains the API to access the OS dependent atheros host driver
// by the WMI or WLAN generic modules.
//
// Author(s): ="Atheros"
//==============================================================================
#ifndef _AR6000_API_H_
#define _AR6000_API_H_

#if defined(__linux__) && !defined(LINUX_EMULATION)
#include "../os/linux/include/ar6xapi_linux.h"
#endif

#ifdef UNDER_NWIFI
#include "../os/windows/include/ar6xapi.h"
#endif

#ifdef ATHR_CE_LEGACY
#include "../os/windows/include/ar6xapi.h"
#endif

#ifdef REXOS
#include "../os/rexos/include/common/ar6xapi_rexos.h"
#endif

#if defined ART_WIN
#include "../os/win_art/include/ar6xapi_win.h"
#endif

#ifdef WIN_NWF
#include "../os/windows/include/ar6xapi.h"
#endif

#endif /* _AR6000_API_H */

