/***************************************************************/
/** This file is generated, do not edit
 * @file    hw_ssd_def.h
 * @brief   Module related to underlaying HW register access
 * @GeneratorVersion  CodeGen "V1.3"
 *                    Register version:V1.4
 * @GeneratedDate     2008/5/21  13:58
 *
 * @note   Copyright (c) 2007 Sunplus Technology Co., Ltd. All rights reserved.
 *
 *   This software is copyrighted by and is the property of Sunplus
 *   Technology Co., Ltd. All rights are reserved by Sunplus Technology
 *   Co., Ltd. This software may only be used in accordance with the
 *   corresponding license agreement. Any unauthorized use, duplication,
 *   distribution, or disclosure of this software is expressly forbidden.
 *
 *   This Copyright notice MUST not be removed or modified without prior
 *   written consent of Sunplus Technology Co., Ltd.
 *
 *   Sunplus Technology Co., Ltd. reserves the right to modify this
 *   software without notice.
 *
 *   Sunplus Technology Co., Ltd.
 *   19, Innovation First Road, Science-Based Industrial Park,
 *   Hsin-Chu, Taiwan, R.O.C.
 *
 ***************************************************************/
#ifndef _SSD_DEF_H_
#define _SSD_DEF_H_
/*!
 * @brief Sound Modes
 *        0 = Mono/Stereo (default)
 *        6 = VDSII422
 *        7 = VDSII423
 *        8 = SRS TruSurround (DPL)
 *        9 = Noise Sequencing
 *        10 = SRS TruSurround (Passive Matrix)
 *        11 = SRS TruSurround XT (Passive Matrix)
 *        12 = SRS TruSurround XT (DPL)
 *        13 = NVS522
 *        14 = NVS523
 * @range 0..14
 ***/
#define REG_Ssd_sndmod
#define REG_Ssd_sndmod_OFFSET                                            0x0023
#define REG_Ssd_sndmod_MASK                                              0xf
#define REG_Ssd_sndmod_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_sndmod_BIT_MASK                                          0xf
#define REG_Ssd_sndmod_BIT_IDX                                           0
#define REG_Ssd_sndmod_NUM_OF_BITS                                       4
#define REG_Ssd_sndmod_MIN                                               0
#define REG_Ssd_sndmod_MAX                                               5
#define REG_SSD_SNDMOD_VALUE_MONO_STEREO_DEFAULT                         0
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_XT_DPL                      1
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX           2
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_HD_DPL                      3
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_HD_PASSIVE_MATRIX           4
#define REG_SSD_SNDMOD_VALUE_NOISE_SEQUENCING                            5

/*!
 * @brief SRS 3D Enable
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_srs_3Denable
#define REG_Ssd_srs_3Denable_OFFSET											0x0028
#define REG_Ssd_srs_3Denable_MASK											0x1
#define REG_Ssd_srs_3Denable_SIGNED_BIT_MASK								0x0
#define REG_Ssd_srs_3Denable_BIT_MASK										0x1
#define REG_Ssd_srs_3Denable_BIT_IDX										0
#define REG_Ssd_srs_3Denable_NUM_OF_BITS									1
#define REG_Ssd_srs_3Denable_MIN											0
#define REG_Ssd_srs_3Denable_MAX											1
#define REG_SSD_SRS_3DENABLE_VALUE_OFF										0
#define REG_SSD_SRS_3DENABLE_VALUE_ON										1

/*!
 * @brief SRS Mode
 *        0 = SRS 3D Stereo
 *        1 = SRS Extreme
 * @range 0..1
 ***/
#define REG_Ssd_srs_mod
#define REG_Ssd_srs_mod_OFFSET												0x0028
#define REG_Ssd_srs_mod_MASK												0x1
#define REG_Ssd_srs_mod_SIGNED_BIT_MASK										0x0
#define REG_Ssd_srs_mod_BIT_MASK											0x2
#define REG_Ssd_srs_mod_BIT_IDX												1
#define REG_Ssd_srs_mod_NUM_OF_BITS											1
#define REG_Ssd_srs_mod_MIN													0
#define REG_Ssd_srs_mod_MAX													1
#define REG_SSD_SRS_MODE_VALUE_3D_STEREO									0
#define REG_SSD_SRS_MODE_VALUE_EXTREME										1
/*!
 * @brief WOW BitRate
 *        0 = Low bit rate
 *        1 = High bit rate
 * @range 0..1
 ***/
#define REG_Ssd_wowBitRate
#define REG_Ssd_wowBitRate_OFFSET											0x0028
#define REG_Ssd_wowBitRate_MASK												0x1
#define REG_Ssd_wowBitRate_SIGNED_BIT_MASK									0x0
#define REG_Ssd_wowBitRate_BIT_MASK											0x4
#define REG_Ssd_wowBitRate_BIT_IDX											2
#define REG_Ssd_wowBitRate_NUM_OF_BITS										1
#define REG_Ssd_wowBitRate_MIN												0
#define REG_Ssd_wowBitRate_MAX												1
#define REG_SSD_WOWBITRATE_VALUE_LOW										0
#define REG_SSD_WOWBITRATE_VALUE_HIGH										1
/*!
 * @brief SRS Focus Enable
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_srs_FocusEnable
#define REG_Ssd_srs_FocusEnable_OFFSET										0x0028
#define REG_Ssd_srs_FocusEnable_MASK										0x1
#define REG_Ssd_srs_FocusEnable_SIGNED_BIT_MASK								0x0
#define REG_Ssd_srs_FocusEnable_BIT_MASK									0x8
#define REG_Ssd_srs_FocusEnable_BIT_IDX										3
#define REG_Ssd_srs_FocusEnable_NUM_OF_BITS									1
#define REG_Ssd_srs_FocusEnable_MIN											0
#define REG_Ssd_srs_FocusEnable_MAX											1
#define REG_SSD_SRS_FOCUSENABLE_VALUE_OFF									0
#define REG_SSD_SRS_FOCUSENABLE_VALUE_ON									1
/*!
 * @brief SRS Limiter Enable
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_srs_LimiterEnable
#define REG_Ssd_srs_LimiterEnable_OFFSET									0x0028
#define REG_Ssd_srs_LimiterEnable_MASK										0x1
#define REG_Ssd_srs_LimiterEnable_SIGNED_BIT_MASK							0x0
#define REG_Ssd_srs_LimiterEnable_BIT_MASK									0x10
#define REG_Ssd_srs_LimiterEnable_BIT_IDX									4
#define REG_Ssd_srs_LimiterEnable_NUM_OF_BITS								1
#define REG_Ssd_srs_LimiterEnable_MIN										0
#define REG_Ssd_srs_LimiterEnable_MAX										1
#define REG_SSD_SRS_LIMITERENABLE_VALUE_OFF									0
#define REG_SSD_SRS_LIMITERENABLE_VALUE_ON                                  1
/*!
 * @brief SRS Focus Level
 *		0x0 = -4dB 
 *		0x1 = -5dB 
 *		0x2 = -6dB 
 *		0x3 = -7dB 
 *		0x4 = -8dB 
 *		0x5 = -9dB 
 *		0x6 = -10dB
 *		0x7 = -11dB
 *		0x8 = -12dB
 *		0x9 = -13dB
 *		0xA = -14dB
 *		0xB = -15dB
 *		0xC = -16dB
 *		0xD = -17dB
 *		0xE = -18dB
 * @range 0..7
 ***/
#define REG_Ssd_focus_level
#define REG_Ssd_focus_level_OFFSET                                       0x0028
#define REG_Ssd_focus_level_MASK                                         0xf
#define REG_Ssd_focus_level_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_focus_level_BIT_MASK                                     0xf00
#define REG_Ssd_focus_level_BIT_IDX                                      8
#define REG_Ssd_focus_level_NUM_OF_BITS                                  4
#define REG_Ssd_focus_level_MIN                                          0
#define REG_Ssd_focus_level_MAX                                          15
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_4DB                              0
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_5DB                              1
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_6DB                              2
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_7DB                              3
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_8DB                              4
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_9DB                              5
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_10DB                             6
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_11DB                             7
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_12DB                             8
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_13DB                             9
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_14DB                             10
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_15DB                             11
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_16DB                             12
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_17DB                             13
#define REG_SSD_FOCUS_LEVEL_VALUE_MINUS_18DB                             14
/*!
 * @brief SRS Limiter Level
 *		0x0 = -4dB 
 *		0x1 = -5dB 
 *		0x2 = -6dB 
 *		0x3 = -7dB 
 *		0x4 = -8dB 
 *		0x5 = -9dB 
 *		0x6 = -10dB
 *		0x7 = -11dB
 *		0x8 = -12dB
 *		0x9 = -13dB
 *		0xA = -14dB
 *		0xB = -15dB
 *		0xC = -16dB
 *		0xD = -17dB
 *		0xE = -18dB
 * @range 0..7
 ***/
#define REG_Ssd_limiter_level
#define REG_Ssd_limiter_level_OFFSET                                       0x0028
#define REG_Ssd_limiter_level_MASK                                         0xf
#define REG_Ssd_limiter_level_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_limiter_level_BIT_MASK                                     0xf000
#define REG_Ssd_limiter_level_BIT_IDX                                      12
#define REG_Ssd_limiter_level_NUM_OF_BITS                                  4
#define REG_Ssd_limiter_level_MIN                                          0
#define REG_Ssd_limiter_level_MAX                                          15
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_4DB                               0
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_5DB                               1
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_6DB                               2
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_7DB                               3
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_8DB                               4
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_9DB                               5
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_10DB                              6
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_11DB                              7
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_12DB                              8
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_13DB                              9
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_14DB                             10
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_15DB                             11
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_16DB                             12
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_17DB                             13
#define REG_SSD_LIMITER_LEVEL_VALUE_MINUS_18DB                             14
/*!
 * @brief XT/HD output Mode selection
 *        0 = 2_0_0
 *        1 = 2_0_1
 *        2 = headphone
 *        3 = 3_0_0
 *		  4 = 3_0_1
 * @range 0..7
 ***/
#define REG_Ssd_tsxt_o_mode
#define REG_Ssd_tsxt_o_mode_OFFSET                                       0x002a
#define REG_Ssd_tsxt_o_mode_MASK                                         0x7
#define REG_Ssd_tsxt_o_mode_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_tsxt_o_mode_BIT_MASK                                     0x38
#define REG_Ssd_tsxt_o_mode_BIT_IDX                                      3
#define REG_Ssd_tsxt_o_mode_NUM_OF_BITS                                  3
#define REG_Ssd_tsxt_o_mode_MIN                                          0
#define REG_Ssd_tsxt_o_mode_MAX                                          7
#define REG_SSD_TSXT_O_MODE_VALUE_2_0_0                                  0
#define REG_SSD_TSXT_O_MODE_VALUE_2_0_1                                  1
#define REG_SSD_TSXT_O_MODE_VALUE_HEADPHONE                              2
#define REG_SSD_TSXT_O_MODE_VALUE_3_0_0									 3
#define REG_SSD_TSXT_O_MODE_VALUE_3_0_1									 4
/*!
 * @brief Tru Bass Front function control
 *        $0= OFF
 *        $1= ON
 * @range 0..1
 ***/
#define REG_Ssd_tsxt_trubassfrontenable
#define REG_Ssd_tsxt_trubassfrontenable_OFFSET                           0x002a
#define REG_Ssd_tsxt_trubassfrontenable_MASK                             0x1
#define REG_Ssd_tsxt_trubassfrontenable_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_tsxt_trubassfrontenable_BIT_MASK                         0x80
#define REG_Ssd_tsxt_trubassfrontenable_BIT_IDX                          7
#define REG_Ssd_tsxt_trubassfrontenable_NUM_OF_BITS                      1
#define REG_Ssd_tsxt_trubassfrontenable_MIN                              0
#define REG_Ssd_tsxt_trubassfrontenable_MAX                              1
#define REG_SSD_TSXT_TruBassFrontEnable_VALUE_OFF                        0
#define REG_SSD_TSXT_TruBassFrontEnable_VALUE_ON                         1
/*!
 * @brief Tru Bass Sub function control
 *        $0= OFF
 *        $1= ON
 * @range 0..1
 ***/
#define REG_Ssd_tsxt_trubasssubenable
#define REG_Ssd_tsxt_trubasssubenable_OFFSET                             0x002a
#define REG_Ssd_tsxt_trubasssubenable_MASK                               0x1
#define REG_Ssd_tsxt_trubasssubenable_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_tsxt_trubasssubenable_BIT_MASK                           0x100
#define REG_Ssd_tsxt_trubasssubenable_BIT_IDX                            8
#define REG_Ssd_tsxt_trubasssubenable_NUM_OF_BITS                        1
#define REG_Ssd_tsxt_trubasssubenable_MIN                                0
#define REG_Ssd_tsxt_trubasssubenable_MAX                                1
#define REG_SSD_TSXT_TruBassSubEnable_VALUE_OFF                          0
#define REG_SSD_TSXT_TruBassSubEnable_VALUE_ON                           1

/*!
 * @brief front Speaker Size selection
 *        $0= 40Hz
 *        $1= 60Hz
 *        $2= 100Hz
 *        $3= 150Hz
 *        $4= 200Hz
 *        $5= 250Hz
 *        $6= 300Hz
 *        $7= 400Hz
 *        $F= OFF
 * @range 0..15
 ***/
#define REG_Ssd_tsxt_frontspeakersize
#define REG_Ssd_tsxt_frontspeakersize_OFFSET                             0x002a
#define REG_Ssd_tsxt_frontspeakersize_MASK                               0xf
#define REG_Ssd_tsxt_frontspeakersize_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_tsxt_frontspeakersize_BIT_MASK                           0xf000
#define REG_Ssd_tsxt_frontspeakersize_BIT_IDX                            12
#define REG_Ssd_tsxt_frontspeakersize_NUM_OF_BITS                        4
#define REG_Ssd_tsxt_frontspeakersize_MIN                                0
#define REG_Ssd_tsxt_frontspeakersize_MAX                                15
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_40HZ                         0
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_60HZ                         1
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_100HZ                        2
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_150HZ                        3
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_200HZ                        4
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_250HZ                        5
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_300HZ                        6
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_400HZ                        7
#define REG_SSD_TSXT_FrontSpeakerSize_VALUE_OFF                          15
/*!
 * @brief sub Speaker Size selection
 *        $0= 40Hz
 *        $1= 60Hz
 *        $2= 100Hz
 *        $3= 150Hz
 *        $4= 200Hz
 *        $5= 250Hz
 *        $6= 300Hz
 *        $7= 400Hz
 *        $F= OFF
 * @range 0..15
 ***/
#define REG_Ssd_tsxt_subspeakersize
#define REG_Ssd_tsxt_subspeakersize_OFFSET                               0x002b
#define REG_Ssd_tsxt_subspeakersize_MASK                                 0xf
#define REG_Ssd_tsxt_subspeakersize_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_tsxt_subspeakersize_BIT_MASK                             0xf
#define REG_Ssd_tsxt_subspeakersize_BIT_IDX                              0
#define REG_Ssd_tsxt_subspeakersize_NUM_OF_BITS                          4
#define REG_Ssd_tsxt_subspeakersize_MIN                                  0
#define REG_Ssd_tsxt_subspeakersize_MAX                                  15
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_40HZ                           0
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_60HZ                           1
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_100HZ                          2
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_150HZ                          3
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_200HZ                          4
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_250HZ                          5
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_300HZ                          6
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_400HZ                          7
#define REG_SSD_TSXT_SubSpeakerSize_VALUE_OFF                            15

/*!
 * @brief TruBass front gain control
 *        $0 = 0%
 *        $1 = 7%
 *        $2 = 13%
 *        $3 = 20%
 *        $4 = 27%
 *        $5 = 33%
 *        $6 = 40%
 *        $7 = 46%
 *        $8 = 53%
 *        $9 = 60%
 *        $A = 67%
 *        $B = 73%
 *        $C = 79%
 *        $D = 86%
 *        $E = 92%
 *        $F = 100%
 * @range 0..15
 ***/
#define REG_Ssd_tsxt_trubass_frontgain
#define REG_Ssd_tsxt_trubass_frontgain_OFFSET                             0x002b
#define REG_Ssd_tsxt_trubass_frontgain_MASK                               0xf
#define REG_Ssd_tsxt_trubass_frontgain_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_tsxt_trubass_frontgain_BIT_MASK                           0xf00
#define REG_Ssd_tsxt_trubass_frontgain_BIT_IDX                            8
#define REG_Ssd_tsxt_trubass_frontgain_NUM_OF_BITS                        4
#define REG_Ssd_tsxt_trubass_frontgain_MIN                                0
#define REG_Ssd_tsxt_trubass_frontgain_MAX                                15
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_0                            0
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_7                            1
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_13                           2
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_20                           3
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_27                           4
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_33                           5
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_40                           6
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_46                           7
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_53                           8
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_60                           9
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_67                           10
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_73                           11
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_79                           12
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_86                           13
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_92                           14
#define REG_SSD_TSXT_TruBass_FrontGAIN_VALUE_100                          15
/*!
 * @brief TruBass sub gain control
 *        $0 = 0%
 *        $1 = 7%
 *        $2 = 13%
 *        $3 = 20%
 *        $4 = 27%
 *        $5 = 33%
 *        $6 = 40%
 *        $7 = 46%
 *        $8 = 53%
 *        $9 = 60%
 *        $A = 67%
 *        $B = 73%
 *        $C = 79%
 *        $D = 86%
 *        $E = 92%
 *        $F = 100%
 * @range 0..15
 ***/
#define REG_Ssd_tsxt_trubass_subgain
#define REG_Ssd_tsxt_trubass_subgain_OFFSET                                 0x002b
#define REG_Ssd_tsxt_trubass_subgain_MASK                                   0xf
#define REG_Ssd_tsxt_trubass_subgain_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_tsxt_trubass_subgain_BIT_MASK                               0xf000
#define REG_Ssd_tsxt_trubass_subgain_BIT_IDX                                12
#define REG_Ssd_tsxt_trubass_subgain_NUM_OF_BITS                            4
#define REG_Ssd_tsxt_trubass_subgain_MIN                                    0
#define REG_Ssd_tsxt_trubass_subgain_MAX                                    15
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_0                                0
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_7                                1
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_13                               2
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_20                               3
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_27                               4
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_33                               5
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_40                               6
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_46                               7
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_53                               8
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_60                               9
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_67                               10
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_73                               11
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_79                               12
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_86                               13
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_92                               14
#define REG_SSD_TSXT_TruBass_SubGAIN_VALUE_100                              15
#endif // _SSD_DEF_H_

