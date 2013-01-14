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
#include "ssd_def.h"
#ifndef _HW_SSD_DEF_H_
#define _HW_SSD_DEF_H_
/*!
 * @brief standard detection result (ASD mode), or selected standard in SSS mode
 *         0 = failed to find any standard
 *         1 = B/G (still searching, SC2 not (yet) found)
 *         2 = D/K (still searching, SC2 not (yet) found)
 *         3 = M (still searching, no ident or pilot found)
 *         4 = B/G A2
 *         5 = B/G NICAM
 *         6 = D/K A2 (1)
 *         7 = D/K A2 (2)
 *         8 = D/K A2 (3)
 *         9 = D/K NICAM
 *        10 = L NICAM
 *        11 = I NICAM
 *        12 = M Korea
 *        13 = M BTSC
 *        14 = M EIAJ
 *        15 = FM Radio, IF = 10.7 MHz, 50 us deemphasis
 *        16 = FM Radio, IF = 10.7 MHz, 75 us deemphasis
 *        17 = FM Radio, selectable IF, 50 us deemphasis
 *        18 = FM Radio, selectable IF, 75 us deemphasis
 *        31 = still searching for a standard (during first few milliseconds after RESTART)
 * @range 0..31
 ***/
#define REG_Ssd_stdres
#define REG_Ssd_stdres_OFFSET                                              0x00004
#define REG_Ssd_stdres_MASK                                              0x1f
#define REG_Ssd_stdres_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_stdres_BIT_MASK                                          0x1f
#define REG_Ssd_stdres_BIT_IDX                                           0
#define REG_Ssd_stdres_NUM_OF_BITS                                       5
#define REG_Ssd_stdres_MIN                                               0
#define REG_Ssd_stdres_MAX                                               31
#define REG_SSD_STDRES_VALUE_FAILED_TO_FIND_ANY_STANDARD                 0
#define REG_SSD_STDRES_VALUE_B_G_STILL_SEARCHING_SC2_NOT_YET_FOUND       1
#define REG_SSD_STDRES_VALUE_D_K_STILL_SEARCHING_SC2_NOT_YET_FOUND       2
#define REG_SSD_STDRES_VALUE_M_STILL_SEARCHING_NO_IDENT_OR_PILOT_FOUND   3
#define REG_SSD_STDRES_VALUE_B_G_A2                                      4
#define REG_SSD_STDRES_VALUE_B_G_NICAM                                   5
#define REG_SSD_STDRES_VALUE_D_K_A2_1                                    6
#define REG_SSD_STDRES_VALUE_D_K_A2_2                                    7
#define REG_SSD_STDRES_VALUE_D_K_A2_3                                    8
#define REG_SSD_STDRES_VALUE_D_K_NICAM                                   9
#define REG_SSD_STDRES_VALUE_L_NICAM                                     10
#define REG_SSD_STDRES_VALUE_I_NICAM                                     11
#define REG_SSD_STDRES_VALUE_M_KOREA                                     12
#define REG_SSD_STDRES_VALUE_M_BTSC                                      13
#define REG_SSD_STDRES_VALUE_M_EIAJ                                      14
#define REG_SSD_STDRES_VALUE_FM_RADIO_IF_10_7_MHZ_50_US_DEEMPHASIS       15
#define REG_SSD_STDRES_VALUE_FM_RADIO_IF_10_7_MHZ_75_US_DEEMPHASIS       16
#define REG_SSD_STDRES_VALUE_FM_RADIO_SELECTABLE_IF_50_US_DEEMPHASIS     17
#define REG_SSD_STDRES_VALUE_FM_RADIO_SELECTABLE_IF_75_US_DEEMPHASIS     18
#define REG_SSD_STDRES_VALUE_STILL_SEARCHING_FOR_A_STANDARD_DURING_FIRST_FEW_MILLISECONDS_AFTER_RESTART 31

/*!
 * @brief general stereo flag (ident source determined by currently detected or selected standard)
 *        0 = No stereo mode
 *        1 = Stereo mode detected
 * @range 0..1
 ***/
#define REG_Ssd_gst
#define REG_Ssd_gst_OFFSET                                                 0x00004
#define REG_Ssd_gst_MASK                                                 0x1
#define REG_Ssd_gst_SIGNED_BIT_MASK                                      0x0
#define REG_Ssd_gst_BIT_MASK                                             0x20
#define REG_Ssd_gst_BIT_IDX                                              5
#define REG_Ssd_gst_NUM_OF_BITS                                          1
#define REG_Ssd_gst_MIN                                                  0
#define REG_Ssd_gst_MAX                                                  1
#define REG_SSD_GST_VALUE_NO_STEREO_MODE                                 0
#define REG_SSD_GST_VALUE_STEREO_MODE_DETECTED                           1

/*!
 * @brief general dual flag
 *        0 = No dual mode
 *        1 = Dual mode detected
 * @range 0..1
 ***/
#define REG_Ssd_gdu
#define REG_Ssd_gdu_OFFSET                                                 0x00004
#define REG_Ssd_gdu_MASK                                                 0x1
#define REG_Ssd_gdu_SIGNED_BIT_MASK                                      0x0
#define REG_Ssd_gdu_BIT_MASK                                             0x40
#define REG_Ssd_gdu_BIT_IDX                                              6
#define REG_Ssd_gdu_NUM_OF_BITS                                          1
#define REG_Ssd_gdu_MIN                                                  0
#define REG_Ssd_gdu_MAX                                                  1
#define REG_SSD_GDU_VALUE_NO_DUAL_MODE                                   0
#define REG_SSD_GDU_VALUE_DUAL_MODE_DETECTED                             1

/*!
 * @brief A2 or EIAJ ident dual flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_adu
#define REG_Ssd_adu_OFFSET                                                 0x00004
#define REG_Ssd_adu_MASK                                                 0x1
#define REG_Ssd_adu_SIGNED_BIT_MASK                                      0x0
#define REG_Ssd_adu_BIT_MASK                                             0x100
#define REG_Ssd_adu_BIT_IDX                                              8
#define REG_Ssd_adu_NUM_OF_BITS                                          1
#define REG_Ssd_adu_MIN                                                  0
#define REG_Ssd_adu_MAX                                                  1
#define REG_SSD_ADU_VALUE_FALSE                                          0
#define REG_SSD_ADU_VALUE_TRUE                                           1

/*!
 * @brief A2 or EIAJ ident stereo flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_ast
#define REG_Ssd_ast_OFFSET                                                 0x00004
#define REG_Ssd_ast_MASK                                                 0x1
#define REG_Ssd_ast_SIGNED_BIT_MASK                                      0x0
#define REG_Ssd_ast_BIT_MASK                                             0x200
#define REG_Ssd_ast_BIT_IDX                                              9
#define REG_Ssd_ast_NUM_OF_BITS                                          1
#define REG_Ssd_ast_MIN                                                  0
#define REG_Ssd_ast_MAX                                                  1
#define REG_SSD_AST_VALUE_FALSE                                          0
#define REG_SSD_AST_VALUE_TRUE                                           1

/*!
 * @brief SC2 (if A2 mode) or EIAJ subchannel muted due to noise
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_aamut
#define REG_Ssd_aamut_OFFSET                                               0x00004
#define REG_Ssd_aamut_MASK                                               0x1
#define REG_Ssd_aamut_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_aamut_BIT_MASK                                           0x400
#define REG_Ssd_aamut_BIT_IDX                                            10
#define REG_Ssd_aamut_NUM_OF_BITS                                        1
#define REG_Ssd_aamut_MIN                                                0
#define REG_Ssd_aamut_MAX                                                1
#define REG_SSD_AAMUT_VALUE_FALSE                                        0
#define REG_SSD_AAMUT_VALUE_TRUE                                         1

/*!
 * @brief BTSC or FM radio pilot tone detected (stereo indicator)
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_bpilot
#define REG_Ssd_bpilot_OFFSET                                              0x00004
#define REG_Ssd_bpilot_MASK                                              0x1
#define REG_Ssd_bpilot_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_bpilot_BIT_MASK                                          0x800
#define REG_Ssd_bpilot_BIT_IDX                                           11
#define REG_Ssd_bpilot_NUM_OF_BITS                                       1
#define REG_Ssd_bpilot_MIN                                               0
#define REG_Ssd_bpilot_MAX                                               1
#define REG_SSD_BPILOT_VALUE_FALSE                                       0
#define REG_SSD_BPILOT_VALUE_TRUE                                        1

/*!
 * @brief SAP carrier detected
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_sapdet
#define REG_Ssd_sapdet_OFFSET                                              0x00004
#define REG_Ssd_sapdet_MASK                                              0x1
#define REG_Ssd_sapdet_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_sapdet_BIT_MASK                                          0x1000
#define REG_Ssd_sapdet_BIT_IDX                                           12
#define REG_Ssd_sapdet_NUM_OF_BITS                                       1
#define REG_Ssd_sapdet_MIN                                               0
#define REG_Ssd_sapdet_MAX                                               1
#define REG_SSD_SAPDET_VALUE_FALSE                                       0
#define REG_SSD_SAPDET_VALUE_TRUE                                        1

/*!
 * @brief BTSC stereo muted due to noise (if noise detector enabled)
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_bamut
#define REG_Ssd_bamut_OFFSET                                               0x00004
#define REG_Ssd_bamut_MASK                                               0x1
#define REG_Ssd_bamut_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_bamut_BIT_MASK                                           0x2000
#define REG_Ssd_bamut_BIT_IDX                                            13
#define REG_Ssd_bamut_NUM_OF_BITS                                        1
#define REG_Ssd_bamut_MIN                                                0
#define REG_Ssd_bamut_MAX                                                1
#define REG_SSD_BAMUT_VALUE_FALSE                                        0
#define REG_SSD_BAMUT_VALUE_TRUE                                         1

/*!
 * @brief SAP muted due to noise (if noise detector enabled)
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_samut
#define REG_Ssd_samut_OFFSET                                               0x00004
#define REG_Ssd_samut_MASK                                               0x1
#define REG_Ssd_samut_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_samut_BIT_MASK                                           0x4000
#define REG_Ssd_samut_BIT_IDX                                            14
#define REG_Ssd_samut_NUM_OF_BITS                                        1
#define REG_Ssd_samut_MIN                                                0
#define REG_Ssd_samut_MAX                                                1
#define REG_SSD_SAMUT_VALUE_FALSE                                        0
#define REG_SSD_SAMUT_VALUE_TRUE                                         1

/*!
 * @brief NICAM decoder VDSP flag
 *        0 = DATA or undefined format
 *        1 = SOUND
 * @range 0..1
 ***/
#define REG_Ssd_vdsp_c
#define REG_Ssd_vdsp_c_OFFSET                                              0x00004
#define REG_Ssd_vdsp_c_MASK                                              0x1
#define REG_Ssd_vdsp_c_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_vdsp_c_BIT_MASK                                          0x8000
#define REG_Ssd_vdsp_c_BIT_IDX                                           15
#define REG_Ssd_vdsp_c_NUM_OF_BITS                                       1
#define REG_Ssd_vdsp_c_MIN                                               0
#define REG_Ssd_vdsp_c_MAX                                               1
#define REG_SSD_VDSP_C_VALUE_DATA_OR_UNDEFINED_FORMAT                    0
#define REG_SSD_VDSP_C_VALUE_SOUND                                       1

/*!
 * @brief NICAM decoder stereo flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_nicst_c
#define REG_Ssd_nicst_c_OFFSET                                             0x00004
#define REG_Ssd_nicst_c_MASK                                             0x1
#define REG_Ssd_nicst_c_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_nicst_c_BIT_MASK                                         0x10000
#define REG_Ssd_nicst_c_BIT_IDX                                          16
#define REG_Ssd_nicst_c_NUM_OF_BITS                                      1
#define REG_Ssd_nicst_c_MIN                                              0
#define REG_Ssd_nicst_c_MAX                                              1
#define REG_SSD_NICST_C_VALUE_FALSE                                      0
#define REG_SSD_NICST_C_VALUE_TRUE                                       1

/*!
 * @brief NICAM decoder dual flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_nicdu_c
#define REG_Ssd_nicdu_c_OFFSET                                             0x00004
#define REG_Ssd_nicdu_c_MASK                                             0x1
#define REG_Ssd_nicdu_c_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_nicdu_c_BIT_MASK                                         0x20000
#define REG_Ssd_nicdu_c_BIT_IDX                                          17
#define REG_Ssd_nicdu_c_NUM_OF_BITS                                      1
#define REG_Ssd_nicdu_c_MIN                                              0
#define REG_Ssd_nicdu_c_MAX                                              1
#define REG_SSD_NICDU_C_VALUE_FALSE                                      0
#define REG_SSD_NICDU_C_VALUE_TRUE                                       1

/*!
 * @brief NICAM automute flag
 *        0 = not muted
 *        1 = muted (fallback to analog sound carrier)
 * @range 0..1
 ***/
#define REG_Ssd_namut
#define REG_Ssd_namut_OFFSET                                               0x00004
#define REG_Ssd_namut_MASK                                               0x1
#define REG_Ssd_namut_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_namut_BIT_MASK                                           0x40000
#define REG_Ssd_namut_BIT_IDX                                            18
#define REG_Ssd_namut_NUM_OF_BITS                                        1
#define REG_Ssd_namut_MIN                                                0
#define REG_Ssd_namut_MAX                                                1
#define REG_SSD_NAMUT_VALUE_NOT_MUTED                                    0
#define REG_SSD_NAMUT_VALUE_MUTED_FALLBACK_TO_ANALOG_SOUND_CARRIER       1

/*!
 * @brief NICAM reserve sound switching flag (=C4), see NICAM specification
 *        0 = ana-sc different contents than NICAM carrier
 *        1 = ana-sc same contents as NICAM carrier (M1 if DUAL)
 * @range 0..1
 ***/
#define REG_Ssd_rssf
#define REG_Ssd_rssf_OFFSET                                                0x00004
#define REG_Ssd_rssf_MASK                                                0x1
#define REG_Ssd_rssf_SIGNED_BIT_MASK                                     0x0
#define REG_Ssd_rssf_BIT_MASK                                            0x80000
#define REG_Ssd_rssf_BIT_IDX                                             19
#define REG_Ssd_rssf_NUM_OF_BITS                                         1
#define REG_Ssd_rssf_MIN                                                 0
#define REG_Ssd_rssf_MAX                                                 1
#define REG_SSD_RSSF_VALUE_ANA_SC_DIFFERENT_CONTENTS_THAN_NICAM_CARRIER  0
#define REG_SSD_RSSF_VALUE_ANA_SC_SAME_CONTENTS_AS_NICAM_CARRIER_M1_IF_DUAL 1

/*!
 * @brief initialization status (set to 0 upon read access)
 *        0 = no reset performed
 *        1 = reset has been applied to DSP and init routine has been executed
 * @range 0..1
 ***/
#define REG_Ssd_initstat
#define REG_Ssd_initstat_OFFSET                                            0x00004
#define REG_Ssd_initstat_MASK                                            0x1
#define REG_Ssd_initstat_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_initstat_BIT_MASK                                        0x100000
#define REG_Ssd_initstat_BIT_IDX                                         20
#define REG_Ssd_initstat_NUM_OF_BITS                                     1
#define REG_Ssd_initstat_MIN                                             0
#define REG_Ssd_initstat_MAX                                             1
#define REG_SSD_INITSTAT_VALUE_NO_RESET_PERFORMED                        0
#define REG_SSD_INITSTAT_VALUE_RESET_HAS_BEEN_APPLIED_TO_DSP_AND_INIT_ROUTINE_HAS_BEEN_EXECUTED 1

/*!
 * @brief NICAM error counter: number of parity errors found in the last 128ms period
 * @range 0..255
 ***/
#define REG_Ssd_err_out
#define REG_Ssd_err_out_OFFSET                                             0x00008
#define REG_Ssd_err_out_MASK                                             0xff
#define REG_Ssd_err_out_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_err_out_BIT_MASK                                         0xff
#define REG_Ssd_err_out_BIT_IDX                                          0
#define REG_Ssd_err_out_NUM_OF_BITS                                      8
#define REG_Ssd_err_out_MIN                                              0
#define REG_Ssd_err_out_MAX                                              255

/*!
 * @brief NICAM ConFiguration Change
 *        0 = No configuration change
 *        1 = Configuration change at the 16 frame (CO) boundary
 * @range 0..1
 ***/
#define REG_Ssd_cfc
#define REG_Ssd_cfc_OFFSET                                                 0x00008
#define REG_Ssd_cfc_MASK                                                 0x1
#define REG_Ssd_cfc_SIGNED_BIT_MASK                                      0x0
#define REG_Ssd_cfc_BIT_MASK                                             0x100
#define REG_Ssd_cfc_BIT_IDX                                              8
#define REG_Ssd_cfc_NUM_OF_BITS                                          1
#define REG_Ssd_cfc_MIN                                                  0
#define REG_Ssd_cfc_MAX                                                  1
#define REG_SSD_CFC_VALUE_NO_CONFIGURATION_CHANGE                        0
#define REG_SSD_CFC_VALUE_CONFIGURATION_CHANGE_AT_THE_16_FRAME_CO_BOUNDARY 1

/*!
 * @brief NICAM frame and CO synchronization
 *        0 = Audio output from NICAM part is digital silence
 *        1 = Device has both frame and CO (16 frames) synchronization
 * @range 0..1
 ***/
#define REG_Ssd_co_locked
#define REG_Ssd_co_locked_OFFSET                                           0x00008
#define REG_Ssd_co_locked_MASK                                           0x1
#define REG_Ssd_co_locked_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_co_locked_BIT_MASK                                       0x200
#define REG_Ssd_co_locked_BIT_IDX                                        9
#define REG_Ssd_co_locked_NUM_OF_BITS                                    1
#define REG_Ssd_co_locked_MIN                                            0
#define REG_Ssd_co_locked_MAX                                            1
#define REG_SSD_CO_LOCKED_VALUE_AUDIO_OUTPUT_FROM_NICAM_PART_IS_DIGITAL_SILENCE 0
#define REG_SSD_CO_LOCKED_VALUE_DEVICE_HAS_BOTH_FRAME_AND_CO_16_FRAMES_SYNCHRONIZATION 1

/*!
 * @brief NICAM application control bits (see C1..C4 in NICAM transmission)
 * @range 0..15
 ***/
#define REG_Ssd_nacb
#define REG_Ssd_nacb_OFFSET                                                0x00008
#define REG_Ssd_nacb_MASK                                                0xf
#define REG_Ssd_nacb_SIGNED_BIT_MASK                                     0x0
#define REG_Ssd_nacb_BIT_MASK                                            0x3c00
#define REG_Ssd_nacb_BIT_IDX                                             10
#define REG_Ssd_nacb_NUM_OF_BITS                                         4
#define REG_Ssd_nacb_MIN                                                 0
#define REG_Ssd_nacb_MAX                                                 15

/*!
 * @brief Identification of NICAM sound
 *        0 = DATA or undefined format
 *        1 = SOUND
 * @range 0..1
 ***/
#define REG_Ssd_vdsp
#define REG_Ssd_vdsp_OFFSET                                                0x00008
#define REG_Ssd_vdsp_MASK                                                0x1
#define REG_Ssd_vdsp_SIGNED_BIT_MASK                                     0x0
#define REG_Ssd_vdsp_BIT_MASK                                            0x4000
#define REG_Ssd_vdsp_BIT_IDX                                             14
#define REG_Ssd_vdsp_NUM_OF_BITS                                         1
#define REG_Ssd_vdsp_MIN                                                 0
#define REG_Ssd_vdsp_MAX                                                 1
#define REG_SSD_VDSP_VALUE_DATA_OR_UNDEFINED_FORMAT                      0
#define REG_SSD_VDSP_VALUE_SOUND                                         1

/*!
 * @brief NICAM stereo flag
 *        0 = No NICAM stereo mode (= Mono mode if NICDU = 0)
 *        1 = NICAM stereo mode
 * @range 0..1
 ***/
#define REG_Ssd_nicst
#define REG_Ssd_nicst_OFFSET                                               0x00008
#define REG_Ssd_nicst_MASK                                               0x1
#define REG_Ssd_nicst_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_nicst_BIT_MASK                                           0x8000
#define REG_Ssd_nicst_BIT_IDX                                            15
#define REG_Ssd_nicst_NUM_OF_BITS                                        1
#define REG_Ssd_nicst_MIN                                                0
#define REG_Ssd_nicst_MAX                                                1
#define REG_SSD_NICST_VALUE_NO_NICAM_STEREO_MODE_MONO_MODE_IF_NICDU      0
#define REG_SSD_NICST_VALUE_NICAM_STEREO_MODE                            1

/*!
 * @brief NICAM dual mono mode
 *        0 = No NICAM dual mono mode (= Mono mode if NICST = 0)
 *        1 = NICAM dual mono mode
 * @range 0..1
 ***/
#define REG_Ssd_nicdu
#define REG_Ssd_nicdu_OFFSET                                               0x00008
#define REG_Ssd_nicdu_MASK                                               0x1
#define REG_Ssd_nicdu_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_nicdu_BIT_MASK                                           0x10000
#define REG_Ssd_nicdu_BIT_IDX                                            16
#define REG_Ssd_nicdu_NUM_OF_BITS                                        1
#define REG_Ssd_nicdu_MIN                                                0
#define REG_Ssd_nicdu_MAX                                                1
#define REG_SSD_NICDU_VALUE_NO_NICAM_DUAL_MONO_MODE_MONO_MODE_IF_NICST   0
#define REG_SSD_NICDU_VALUE_NICAM_DUAL_MONO_MODE                         1

/*!
 * @brief NICAM additional data word (11 bit per frame)
 * @range 0..2047
 ***/
#define REG_Ssd_adw
#define REG_Ssd_adw_OFFSET                                                 0x0000C
#define REG_Ssd_adw_MASK                                                 0x7ff
#define REG_Ssd_adw_SIGNED_BIT_MASK                                      0x0
#define REG_Ssd_adw_BIT_MASK                                             0x7ff
#define REG_Ssd_adw_BIT_IDX                                              0
#define REG_Ssd_adw_NUM_OF_BITS                                          11
#define REG_Ssd_adw_MIN                                                  0
#define REG_Ssd_adw_MAX                                                  2047

/*!
 * @brief CPU load indicator; ratio of busy time to total time averaged over approx. 2^23 clock cycles
 * @range 0..16777215
 ***/
#define REG_Ssd_cpuload
#define REG_Ssd_cpuload_OFFSET                                             0x00010
#define REG_Ssd_cpuload_MASK                                             0xffffff
#define REG_Ssd_cpuload_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_cpuload_BIT_MASK                                         0xffffff
#define REG_Ssd_cpuload_BIT_IDX                                          0
#define REG_Ssd_cpuload_NUM_OF_BITS                                      24
#define REG_Ssd_cpuload_MIN                                              0
#define REG_Ssd_cpuload_MAX                                              16777215

/*!
 * @brief PLL state of output thread
 *        0 = not locked, or disabled
 *        1 = not fully locked, PLL stage 1
 *        3 = locked, PLL stage 2
 * @range 0..3
 ***/
#define REG_Ssd_srcstat_tsout
#define REG_Ssd_srcstat_tsout_OFFSET                                       0x00014
#define REG_Ssd_srcstat_tsout_MASK                                       0x3
#define REG_Ssd_srcstat_tsout_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_srcstat_tsout_BIT_MASK                                   0x3
#define REG_Ssd_srcstat_tsout_BIT_IDX                                    0
#define REG_Ssd_srcstat_tsout_NUM_OF_BITS                                2
#define REG_Ssd_srcstat_tsout_MIN                                        0
#define REG_Ssd_srcstat_tsout_MAX                                        3
#define REG_SSD_SRCSTAT_TSOUT_VALUE_NOT_LOCKED_OR_DISABLED               0
#define REG_SSD_SRCSTAT_TSOUT_VALUE_NOT_FULLY_LOCKED_PLL_STAGE_1         1
#define REG_SSD_SRCSTAT_TSOUT_VALUE_LOCKED_PLL_STAGE_2                   3

/*!
 * @brief PLL state of input #1 thread
 *        0 = not locked, or disabled
 *        1 = not fully locked, PLL stage 1
 *        3 = locked, PLL stage 2
 * @range 0..3
 ***/
#define REG_Ssd_srcstat_tsin1
#define REG_Ssd_srcstat_tsin1_OFFSET                                       0x00014
#define REG_Ssd_srcstat_tsin1_MASK                                       0x3
#define REG_Ssd_srcstat_tsin1_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_srcstat_tsin1_BIT_MASK                                   0xc
#define REG_Ssd_srcstat_tsin1_BIT_IDX                                    2
#define REG_Ssd_srcstat_tsin1_NUM_OF_BITS                                2
#define REG_Ssd_srcstat_tsin1_MIN                                        0
#define REG_Ssd_srcstat_tsin1_MAX                                        3
#define REG_SSD_SRCSTAT_TSIN1_VALUE_NOT_LOCKED_OR_DISABLED               0
#define REG_SSD_SRCSTAT_TSIN1_VALUE_NOT_FULLY_LOCKED_PLL_STAGE_1         1
#define REG_SSD_SRCSTAT_TSIN1_VALUE_LOCKED_PLL_STAGE_2                   3

/*!
 * @brief PLL state of input #2 thread
 *        0 = not locked, or disabled
 *        1 = not fully locked, PLL stage 1
 *        3 = locked, PLL stage 2
 * @range 0..3
 ***/
#define REG_Ssd_srcstat_tsin2
#define REG_Ssd_srcstat_tsin2_OFFSET                                       0x00014
#define REG_Ssd_srcstat_tsin2_MASK                                       0x3
#define REG_Ssd_srcstat_tsin2_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_srcstat_tsin2_BIT_MASK                                   0x30
#define REG_Ssd_srcstat_tsin2_BIT_IDX                                    4
#define REG_Ssd_srcstat_tsin2_NUM_OF_BITS                                2
#define REG_Ssd_srcstat_tsin2_MIN                                        0
#define REG_Ssd_srcstat_tsin2_MAX                                        3
#define REG_SSD_SRCSTAT_TSIN2_VALUE_NOT_LOCKED_OR_DISABLED               0
#define REG_SSD_SRCSTAT_TSIN2_VALUE_NOT_FULLY_LOCKED_PLL_STAGE_1         1
#define REG_SSD_SRCSTAT_TSIN2_VALUE_LOCKED_PLL_STAGE_2                   3

/*!
 * @brief SRC1 mode
 *        0 = downsampling
 *        1 = upsampling
 * @range 0..1
 ***/
#define REG_Ssd_srcstat_up1
#define REG_Ssd_srcstat_up1_OFFSET                                         0x00014
#define REG_Ssd_srcstat_up1_MASK                                         0x1
#define REG_Ssd_srcstat_up1_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_srcstat_up1_BIT_MASK                                     0x100
#define REG_Ssd_srcstat_up1_BIT_IDX                                      8
#define REG_Ssd_srcstat_up1_NUM_OF_BITS                                  1
#define REG_Ssd_srcstat_up1_MIN                                          0
#define REG_Ssd_srcstat_up1_MAX                                          1
#define REG_SSD_SRCSTAT_UP1_VALUE_DOWNSAMPLING                           0
#define REG_SSD_SRCSTAT_UP1_VALUE_UPSAMPLING                             1

/*!
 * @brief SRC2 mode
 *        0 = downsampling
 *        1 = upsampling
 * @range 0..1
 ***/
#define REG_Ssd_srcstat_up2
#define REG_Ssd_srcstat_up2_OFFSET                                         0x00014
#define REG_Ssd_srcstat_up2_MASK                                         0x1
#define REG_Ssd_srcstat_up2_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_srcstat_up2_BIT_MASK                                     0x200
#define REG_Ssd_srcstat_up2_BIT_IDX                                      9
#define REG_Ssd_srcstat_up2_NUM_OF_BITS                                  1
#define REG_Ssd_srcstat_up2_MIN                                          0
#define REG_Ssd_srcstat_up2_MAX                                          1
#define REG_SSD_SRCSTAT_UP2_VALUE_DOWNSAMPLING                           0
#define REG_SSD_SRCSTAT_UP2_VALUE_UPSAMPLING                             1

/*!
 * @brief SRC and preprocessing for
 *        0 = inactive
 *        1 = ACTIVE
 * @range 0..1
 ***/
#define REG_Ssd_srcstat_path1a
#define REG_Ssd_srcstat_path1a_OFFSET                                      0x00014
#define REG_Ssd_srcstat_path1a_MASK                                      0x1
#define REG_Ssd_srcstat_path1a_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_srcstat_path1a_BIT_MASK                                  0x1000
#define REG_Ssd_srcstat_path1a_BIT_IDX                                   12
#define REG_Ssd_srcstat_path1a_NUM_OF_BITS                               1
#define REG_Ssd_srcstat_path1a_MIN                                       0
#define REG_Ssd_srcstat_path1a_MAX                                       1
#define REG_SSD_SRCSTAT_PATH1A_VALUE_INACTIVE                            0
#define REG_SSD_SRCSTAT_PATH1A_VALUE_ACTIVE                              1

/*!
 * @brief SRC and preprocessing for
 *        0 = inactive
 *        1 = ACTIVE
 * @range 0..1
 ***/
#define REG_Ssd_srcstat_path2a
#define REG_Ssd_srcstat_path2a_OFFSET                                      0x00014
#define REG_Ssd_srcstat_path2a_MASK                                      0x1
#define REG_Ssd_srcstat_path2a_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_srcstat_path2a_BIT_MASK                                  0x4000
#define REG_Ssd_srcstat_path2a_BIT_IDX                                   14
#define REG_Ssd_srcstat_path2a_NUM_OF_BITS                               1
#define REG_Ssd_srcstat_path2a_MIN                                       0
#define REG_Ssd_srcstat_path2a_MAX                                       1
#define REG_SSD_SRCSTAT_PATH2A_VALUE_INACTIVE                            0
#define REG_SSD_SRCSTAT_PATH2A_VALUE_ACTIVE                              1

/*!
 * @brief monitor level
 * @range -8388608..8388607
 ***/
#define REG_Ssd_monlevel
#define REG_Ssd_monlevel_OFFSET                                            0x00018
#define REG_Ssd_monlevel_MASK                                            0xffffff
#define REG_Ssd_monlevel_SIGNED_BIT_MASK                                 0x800000
#define REG_Ssd_monlevel_BIT_MASK                                        0xffffff
#define REG_Ssd_monlevel_BIT_IDX                                         0
#define REG_Ssd_monlevel_NUM_OF_BITS                                     24
#define REG_Ssd_monlevel_MIN                                             -8388608
#define REG_Ssd_monlevel_MAX                                             8388607

/*!
 * @brief HW detector stereo
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hw_ast
#define REG_Ssd_hw_ast_OFFSET                                              0x0001C
#define REG_Ssd_hw_ast_MASK                                              0x1
#define REG_Ssd_hw_ast_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_hw_ast_BIT_MASK                                          0x1
#define REG_Ssd_hw_ast_BIT_IDX                                           0
#define REG_Ssd_hw_ast_NUM_OF_BITS                                       1
#define REG_Ssd_hw_ast_MIN                                               0
#define REG_Ssd_hw_ast_MAX                                               1
#define REG_SSD_HW_AST_VALUE_FALSE                                       0
#define REG_SSD_HW_AST_VALUE_TRUE                                        1

/*!
 * @brief HW detector dual
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hw_adu
#define REG_Ssd_hw_adu_OFFSET                                              0x0001C
#define REG_Ssd_hw_adu_MASK                                              0x1
#define REG_Ssd_hw_adu_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_hw_adu_BIT_MASK                                          0x2
#define REG_Ssd_hw_adu_BIT_IDX                                           1
#define REG_Ssd_hw_adu_NUM_OF_BITS                                       1
#define REG_Ssd_hw_adu_MIN                                               0
#define REG_Ssd_hw_adu_MAX                                               1
#define REG_SSD_HW_ADU_VALUE_FALSE                                       0
#define REG_SSD_HW_ADU_VALUE_TRUE                                        1

/*!
 * @brief FLID detector stereo
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_fi_ast
#define REG_Ssd_fi_ast_OFFSET                                              0x0001C
#define REG_Ssd_fi_ast_MASK                                              0x1
#define REG_Ssd_fi_ast_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_fi_ast_BIT_MASK                                          0x4
#define REG_Ssd_fi_ast_BIT_IDX                                           2
#define REG_Ssd_fi_ast_NUM_OF_BITS                                       1
#define REG_Ssd_fi_ast_MIN                                               0
#define REG_Ssd_fi_ast_MAX                                               1
#define REG_SSD_FI_AST_VALUE_FALSE                                       0
#define REG_SSD_FI_AST_VALUE_TRUE                                        1

/*!
 * @brief FLID detector dual
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_fi_adu
#define REG_Ssd_fi_adu_OFFSET                                              0x0001C
#define REG_Ssd_fi_adu_MASK                                              0x1
#define REG_Ssd_fi_adu_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_fi_adu_BIT_MASK                                          0x8
#define REG_Ssd_fi_adu_BIT_IDX                                           3
#define REG_Ssd_fi_adu_NUM_OF_BITS                                       1
#define REG_Ssd_fi_adu_MIN                                               0
#define REG_Ssd_fi_adu_MAX                                               1
#define REG_SSD_FI_ADU_VALUE_FALSE                                       0
#define REG_SSD_FI_ADU_VALUE_TRUE                                        1

/*!
 * @brief MPX pilot level
 * @range -131072..131071
 ***/
#define REG_Ssd_mpxplev
#define REG_Ssd_mpxplev_OFFSET                                             0x0001C
#define REG_Ssd_mpxplev_MASK                                             0x3ffff
#define REG_Ssd_mpxplev_SIGNED_BIT_MASK                                  0x800000
#define REG_Ssd_mpxplev_BIT_MASK                                         0xffffc0
#define REG_Ssd_mpxplev_BIT_IDX                                          6
#define REG_Ssd_mpxplev_NUM_OF_BITS                                      18
#define REG_Ssd_mpxplev_MIN                                              -131072
#define REG_Ssd_mpxplev_MAX                                              131071

/*!
 * @brief DC offset from FM demodulator channel 1
 * @range -8388608..8388607
 ***/
#define REG_Ssd_sc1_dc
#define REG_Ssd_sc1_dc_OFFSET                                              0x00020
#define REG_Ssd_sc1_dc_MASK                                              0xffffff
#define REG_Ssd_sc1_dc_SIGNED_BIT_MASK                                   0x800000
#define REG_Ssd_sc1_dc_BIT_MASK                                          0xffffff
#define REG_Ssd_sc1_dc_BIT_IDX                                           0
#define REG_Ssd_sc1_dc_NUM_OF_BITS                                       24
#define REG_Ssd_sc1_dc_MIN                                               -8388608
#define REG_Ssd_sc1_dc_MAX                                               8388607

/*!
 * @brief magnitude of FM subchannel
 * @range 0..16777215
 ***/
#define REG_Ssd_submagn
#define REG_Ssd_submagn_OFFSET                                             0x00024
#define REG_Ssd_submagn_MASK                                             0xffffff
#define REG_Ssd_submagn_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_submagn_BIT_MASK                                         0xffffff
#define REG_Ssd_submagn_BIT_IDX                                          0
#define REG_Ssd_submagn_NUM_OF_BITS                                      24
#define REG_Ssd_submagn_MIN                                              0
#define REG_Ssd_submagn_MAX                                              16777215

/*!
 * @brief status noise detector channel
 *        0 = channel 1
 *        1 = channel 2
 * @range 0..1
 ***/
#define REG_Ssd_ndetch_stat
#define REG_Ssd_ndetch_stat_OFFSET                                         0x00028
#define REG_Ssd_ndetch_stat_MASK                                         0x1
#define REG_Ssd_ndetch_stat_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_ndetch_stat_BIT_MASK                                     0x1
#define REG_Ssd_ndetch_stat_BIT_IDX                                      0
#define REG_Ssd_ndetch_stat_NUM_OF_BITS                                  1
#define REG_Ssd_ndetch_stat_MIN                                          0
#define REG_Ssd_ndetch_stat_MAX                                          1
#define REG_SSD_NDETCH_STAT_VALUE_CHANNEL_1                              0
#define REG_SSD_NDETCH_STAT_VALUE_CHANNEL_2                              1

/*!
 * @brief status noise detector passband
 *        0 = low | (2.5 fh)
 *        1 = high | (7.5 fh)
 * @range 0..1
 ***/
#define REG_Ssd_ndetpb_stat
#define REG_Ssd_ndetpb_stat_OFFSET                                         0x00028
#define REG_Ssd_ndetpb_stat_MASK                                         0x1
#define REG_Ssd_ndetpb_stat_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_ndetpb_stat_BIT_MASK                                     0x2
#define REG_Ssd_ndetpb_stat_BIT_IDX                                      1
#define REG_Ssd_ndetpb_stat_NUM_OF_BITS                                  1
#define REG_Ssd_ndetpb_stat_MIN                                          0
#define REG_Ssd_ndetpb_stat_MAX                                          1
#define REG_SSD_NDETPB_STAT_VALUE_LOW                                    0
#define REG_SSD_NDETPB_STAT_VALUE_HIGH                                   1

/*!
 * @brief noise detector output
 * @range 0..4194303
 ***/
#define REG_Ssd_noiselevel
#define REG_Ssd_noiselevel_OFFSET                                          0x00028
#define REG_Ssd_noiselevel_MASK                                          0x3fffff
#define REG_Ssd_noiselevel_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_noiselevel_BIT_MASK                                      0xfffffc
#define REG_Ssd_noiselevel_BIT_IDX                                       2
#define REG_Ssd_noiselevel_NUM_OF_BITS                                   22
#define REG_Ssd_noiselevel_MIN                                           0
#define REG_Ssd_noiselevel_MAX                                           4194303

/*!
 * @brief indicates internal FM/AM filter bandwidth (FILTBW)
 *        0 = narrow
 *        1 = extra wide
 *        2 = medium
 *        3 = wide
 * @range 0..3
 ***/
#define REG_Ssd_filtbw_stat
#define REG_Ssd_filtbw_stat_OFFSET                                         0x0002C
#define REG_Ssd_filtbw_stat_MASK                                         0x3
#define REG_Ssd_filtbw_stat_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_filtbw_stat_BIT_MASK                                     0x3
#define REG_Ssd_filtbw_stat_BIT_IDX                                      0
#define REG_Ssd_filtbw_stat_NUM_OF_BITS                                  2
#define REG_Ssd_filtbw_stat_MIN                                          0
#define REG_Ssd_filtbw_stat_MAX                                          3
#define REG_SSD_FILTBW_STAT_VALUE_NARROW                                 0
#define REG_SSD_FILTBW_STAT_VALUE_EXTRA_WIDE                             1
#define REG_SSD_FILTBW_STAT_VALUE_MEDIUM                                 2
#define REG_SSD_FILTBW_STAT_VALUE_WIDE                                   3

/*!
 * @brief current scaling factor for OVMADAPT (off: 1024 = 0 dB)
 * @range 0..4095
 ***/
#define REG_Ssd_ovm_scale_stat
#define REG_Ssd_ovm_scale_stat_OFFSET                                      0x0002C
#define REG_Ssd_ovm_scale_stat_MASK                                      0xfff
#define REG_Ssd_ovm_scale_stat_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_ovm_scale_stat_BIT_MASK                                  0xfff000
#define REG_Ssd_ovm_scale_stat_BIT_IDX                                   12
#define REG_Ssd_ovm_scale_stat_NUM_OF_BITS                               12
#define REG_Ssd_ovm_scale_stat_MIN                                       0
#define REG_Ssd_ovm_scale_stat_MAX                                       4095

/*!
 * @brief major version number.
 * @range 0..15
 ***/
#define REG_Ssd_major_version_nr
#define REG_Ssd_major_version_nr_OFFSET                                    0x00030
#define REG_Ssd_major_version_nr_MASK                                    0xf
#define REG_Ssd_major_version_nr_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_major_version_nr_BIT_MASK                                0xf
#define REG_Ssd_major_version_nr_BIT_IDX                                 0
#define REG_Ssd_major_version_nr_NUM_OF_BITS                             4
#define REG_Ssd_major_version_nr_MIN                                     0
#define REG_Ssd_major_version_nr_MAX                                     15

/*!
 * @brief minor version number.
 *        incremented number means: control interface may have extensions for additional functions or functionality may have changed slightly; driver update recommended.
 * @range 0..15
 ***/
#define REG_Ssd_minor_version_nr
#define REG_Ssd_minor_version_nr_OFFSET                                    0x00030
#define REG_Ssd_minor_version_nr_MASK                                    0xf
#define REG_Ssd_minor_version_nr_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_minor_version_nr_BIT_MASK                                0xf0
#define REG_Ssd_minor_version_nr_BIT_IDX                                 4
#define REG_Ssd_minor_version_nr_NUM_OF_BITS                             4
#define REG_Ssd_minor_version_nr_MIN                                     0
#define REG_Ssd_minor_version_nr_MAX                                     15

/*!
 * @brief patch level number.
 *        incremented number indicates bugfixes of the embedded software without any change of control interface or functionality.
 *        no driver update needed.
 * @range 0..15
 ***/
#define REG_Ssd_patch_level
#define REG_Ssd_patch_level_OFFSET                                         0x00030
#define REG_Ssd_patch_level_MASK                                         0xf
#define REG_Ssd_patch_level_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_patch_level_BIT_MASK                                     0xf00
#define REG_Ssd_patch_level_BIT_IDX                                      8
#define REG_Ssd_patch_level_NUM_OF_BITS                                  4
#define REG_Ssd_patch_level_MIN                                          0
#define REG_Ssd_patch_level_MAX                                          15

/*!
 * @brief device type ID (internal use)
 * @range 0..15
 ***/
#define REG_Ssd_device_type
#define REG_Ssd_device_type_OFFSET                                         0x00030
#define REG_Ssd_device_type_MASK                                         0xf
#define REG_Ssd_device_type_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_device_type_BIT_MASK                                     0xf000
#define REG_Ssd_device_type_BIT_IDX                                      12
#define REG_Ssd_device_type_NUM_OF_BITS                                  4
#define REG_Ssd_device_type_MIN                                          0
#define REG_Ssd_device_type_MAX                                          15

/*!
 * @brief ROM identification code. Unique number for every ROM code ever released.
 * @range 0..255
 ***/
#define REG_Ssd_rom_id
#define REG_Ssd_rom_id_OFFSET                                              0x00030
#define REG_Ssd_rom_id_MASK                                              0xff
#define REG_Ssd_rom_id_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_rom_id_BIT_MASK                                          0xff0000
#define REG_Ssd_rom_id_BIT_IDX                                           16
#define REG_Ssd_rom_id_NUM_OF_BITS                                       8
#define REG_Ssd_rom_id_MIN                                               0
#define REG_Ssd_rom_id_MAX                                               255

/*!
 * @brief measurement of SRC1 input sample rate, 100 Hz steps (for DIGIN: after input decimation)
 * @range 0..4095
 ***/
#define REG_Ssd_fs_src1_in
#define REG_Ssd_fs_src1_in_OFFSET                                          0x00034
#define REG_Ssd_fs_src1_in_MASK                                          0xfff
#define REG_Ssd_fs_src1_in_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_fs_src1_in_BIT_MASK                                      0xfff
#define REG_Ssd_fs_src1_in_BIT_IDX                                       0
#define REG_Ssd_fs_src1_in_NUM_OF_BITS                                   12
#define REG_Ssd_fs_src1_in_MIN                                           0
#define REG_Ssd_fs_src1_in_MAX                                           4095

/*!
 * @brief measurement of  SRC2 input sample rate, 100 Hz steps (for DIGIN: after input decimation)
 * @range 0..4095
 ***/
#define REG_Ssd_fs_src2_in
#define REG_Ssd_fs_src2_in_OFFSET                                          0x00034
#define REG_Ssd_fs_src2_in_MASK                                          0xfff
#define REG_Ssd_fs_src2_in_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_fs_src2_in_BIT_MASK                                      0xfff000
#define REG_Ssd_fs_src2_in_BIT_IDX                                       12
#define REG_Ssd_fs_src2_in_NUM_OF_BITS                                   12
#define REG_Ssd_fs_src2_in_MIN                                           0
#define REG_Ssd_fs_src2_in_MAX                                           4095

/*!
 * @brief number of SPDIF input parity errors (max.4095) since last clear via SPDIF_ERRCLR (only valid if SPDIF input selected as SRC input)
 * @range 0..4095
 ***/
#define REG_Ssd_spdif_errcnt
#define REG_Ssd_spdif_errcnt_OFFSET                                        0x00038
#define REG_Ssd_spdif_errcnt_MASK                                        0xfff
#define REG_Ssd_spdif_errcnt_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_spdif_errcnt_BIT_MASK                                    0xfff
#define REG_Ssd_spdif_errcnt_BIT_IDX                                     0
#define REG_Ssd_spdif_errcnt_NUM_OF_BITS                                 12
#define REG_Ssd_spdif_errcnt_MIN                                         0
#define REG_Ssd_spdif_errcnt_MAX                                         4095

/*!
 * @brief measurement of output (audio processing) sample rate, 100 Hz steps
 * @range 0..4095
 ***/
#define REG_Ssd_fs_out
#define REG_Ssd_fs_out_OFFSET                                              0x00038
#define REG_Ssd_fs_out_MASK                                              0xfff
#define REG_Ssd_fs_out_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_fs_out_BIT_MASK                                          0xfff000
#define REG_Ssd_fs_out_BIT_IDX                                           12
#define REG_Ssd_fs_out_NUM_OF_BITS                                       12
#define REG_Ssd_fs_out_MIN                                               0
#define REG_Ssd_fs_out_MAX                                               4095

/*!
 * @brief Internal Maximum Volume Setting
 *        Gives the volume value in 1/8dB in 'Dynamic Volume Mode' used internal.
 * @range -672..192
 ***/
#define REG_Ssd_int_maxvolume
#define REG_Ssd_int_maxvolume_OFFSET                                       0x0003C
#define REG_Ssd_int_maxvolume_MASK                                       0x7ff
#define REG_Ssd_int_maxvolume_SIGNED_BIT_MASK                            0x400
#define REG_Ssd_int_maxvolume_BIT_MASK                                   0x7ff
#define REG_Ssd_int_maxvolume_BIT_IDX                                    0
#define REG_Ssd_int_maxvolume_NUM_OF_BITS                                11
#define REG_Ssd_int_maxvolume_MIN                                        -672
#define REG_Ssd_int_maxvolume_MAX                                        192

/*!
 * @brief upper threshold for MPX pilot detection (BTSC, FM RADIO) in dB below nominal level
 * @range 0..15
 ***/
#define REG_Ssd_mpx_pilot_thr_up
#define REG_Ssd_mpx_pilot_thr_up_OFFSET                                    0x00040
#define REG_Ssd_mpx_pilot_thr_up_MASK                                    0xf
#define REG_Ssd_mpx_pilot_thr_up_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_mpx_pilot_thr_up_BIT_MASK                                0xf
#define REG_Ssd_mpx_pilot_thr_up_BIT_IDX                                 0
#define REG_Ssd_mpx_pilot_thr_up_NUM_OF_BITS                             4
#define REG_Ssd_mpx_pilot_thr_up_MIN                                     0
#define REG_Ssd_mpx_pilot_thr_up_MAX                                     15

/*!
 * @brief lower threshold for MPX pilot detection (BTSC, FM RADIO) in dB below nominal level
 * @range 0..15
 ***/
#define REG_Ssd_mpx_pilot_thr_lo
#define REG_Ssd_mpx_pilot_thr_lo_OFFSET                                    0x00040
#define REG_Ssd_mpx_pilot_thr_lo_MASK                                    0xf
#define REG_Ssd_mpx_pilot_thr_lo_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_mpx_pilot_thr_lo_BIT_MASK                                0xf0
#define REG_Ssd_mpx_pilot_thr_lo_BIT_IDX                                 4
#define REG_Ssd_mpx_pilot_thr_lo_NUM_OF_BITS                             4
#define REG_Ssd_mpx_pilot_thr_lo_MIN                                     0
#define REG_Ssd_mpx_pilot_thr_lo_MAX                                     15

/*!
 * @brief upper threshold for SAP carrier detection in dB below nominal level
 * @range 0..15
 ***/
#define REG_Ssd_sap_car_thr_up
#define REG_Ssd_sap_car_thr_up_OFFSET                                      0x00040
#define REG_Ssd_sap_car_thr_up_MASK                                      0xf
#define REG_Ssd_sap_car_thr_up_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_sap_car_thr_up_BIT_MASK                                  0xf00
#define REG_Ssd_sap_car_thr_up_BIT_IDX                                   8
#define REG_Ssd_sap_car_thr_up_NUM_OF_BITS                               4
#define REG_Ssd_sap_car_thr_up_MIN                                       0
#define REG_Ssd_sap_car_thr_up_MAX                                       15

/*!
 * @brief lower threshold for SAP carrier detection in dB below nominal level
 * @range 0..15
 ***/
#define REG_Ssd_sap_car_thr_lo
#define REG_Ssd_sap_car_thr_lo_OFFSET                                      0x00040
#define REG_Ssd_sap_car_thr_lo_MASK                                      0xf
#define REG_Ssd_sap_car_thr_lo_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_sap_car_thr_lo_BIT_MASK                                  0xf000
#define REG_Ssd_sap_car_thr_lo_BIT_IDX                                   12
#define REG_Ssd_sap_car_thr_lo_NUM_OF_BITS                               4
#define REG_Ssd_sap_car_thr_lo_MIN                                       0
#define REG_Ssd_sap_car_thr_lo_MAX                                       15

/*!
 * @brief threshold for detection of first sound carrier (SC1) during ASD first step, relative to -30 dBFS. -16 prevents ASD "failure" to produce output regardless of carrier level.
 * @range -16..15
 ***/
#define REG_Ssd_asd_sc1_thr
#define REG_Ssd_asd_sc1_thr_OFFSET                                         0x00040
#define REG_Ssd_asd_sc1_thr_MASK                                         0x1f
#define REG_Ssd_asd_sc1_thr_SIGNED_BIT_MASK                              0x400000
#define REG_Ssd_asd_sc1_thr_BIT_MASK                                     0x7c0000
#define REG_Ssd_asd_sc1_thr_BIT_IDX                                      18
#define REG_Ssd_asd_sc1_thr_NUM_OF_BITS                                  5
#define REG_Ssd_asd_sc1_thr_MIN                                          -16
#define REG_Ssd_asd_sc1_thr_MAX                                          15

/*!
 * @brief noise threshold for automute of SAP (-16 means automute off)
 * @range -16..15
 ***/
#define REG_Ssd_nmute_sap_thr
#define REG_Ssd_nmute_sap_thr_OFFSET                                       0x00044
#define REG_Ssd_nmute_sap_thr_MASK                                       0x1f
#define REG_Ssd_nmute_sap_thr_SIGNED_BIT_MASK                            0x10
#define REG_Ssd_nmute_sap_thr_BIT_MASK                                   0x1f
#define REG_Ssd_nmute_sap_thr_BIT_IDX                                    0
#define REG_Ssd_nmute_sap_thr_NUM_OF_BITS                                5
#define REG_Ssd_nmute_sap_thr_MIN                                        -16
#define REG_Ssd_nmute_sap_thr_MAX                                        15

/*!
 * @brief hysteresis size [dB] for automute of SAP
 * @range 0..15
 ***/
#define REG_Ssd_nmute_sap_hyst
#define REG_Ssd_nmute_sap_hyst_OFFSET                                      0x00044
#define REG_Ssd_nmute_sap_hyst_MASK                                      0xf
#define REG_Ssd_nmute_sap_hyst_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_nmute_sap_hyst_BIT_MASK                                  0x1e0
#define REG_Ssd_nmute_sap_hyst_BIT_IDX                                   5
#define REG_Ssd_nmute_sap_hyst_NUM_OF_BITS                               4
#define REG_Ssd_nmute_sap_hyst_MIN                                       0
#define REG_Ssd_nmute_sap_hyst_MAX                                       15

/*!
 * @brief noise threshold for automute of SC2 in FM A2 standards     (-16 means automute off)
 * @range -16..15
 ***/
#define REG_Ssd_nmute_sc2_thr
#define REG_Ssd_nmute_sc2_thr_OFFSET                                       0x00044
#define REG_Ssd_nmute_sc2_thr_MASK                                       0x1f
#define REG_Ssd_nmute_sc2_thr_SIGNED_BIT_MASK                            0x2000
#define REG_Ssd_nmute_sc2_thr_BIT_MASK                                   0x3e00
#define REG_Ssd_nmute_sc2_thr_BIT_IDX                                    9
#define REG_Ssd_nmute_sc2_thr_NUM_OF_BITS                                5
#define REG_Ssd_nmute_sc2_thr_MIN                                        -16
#define REG_Ssd_nmute_sc2_thr_MAX                                        15

/*!
 * @brief hysteresis size [dB] for automute of SC2 in FM A2 standards
 * @range 0..15
 ***/
#define REG_Ssd_nmute_sc2_hyst
#define REG_Ssd_nmute_sc2_hyst_OFFSET                                      0x00044
#define REG_Ssd_nmute_sc2_hyst_MASK                                      0xf
#define REG_Ssd_nmute_sc2_hyst_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_nmute_sc2_hyst_BIT_MASK                                  0x3c000
#define REG_Ssd_nmute_sc2_hyst_BIT_IDX                                   14
#define REG_Ssd_nmute_sc2_hyst_NUM_OF_BITS                               4
#define REG_Ssd_nmute_sc2_hyst_MIN                                       0
#define REG_Ssd_nmute_sc2_hyst_MAX                                       15

/*!
 * @brief noise threshold for automute of BTSC stereo carrier (-16 means automute off)
 * @range -16..15
 ***/
#define REG_Ssd_nmute_btsc_thr
#define REG_Ssd_nmute_btsc_thr_OFFSET                                      0x00048
#define REG_Ssd_nmute_btsc_thr_MASK                                      0x1f
#define REG_Ssd_nmute_btsc_thr_SIGNED_BIT_MASK                           0x10
#define REG_Ssd_nmute_btsc_thr_BIT_MASK                                  0x1f
#define REG_Ssd_nmute_btsc_thr_BIT_IDX                                   0
#define REG_Ssd_nmute_btsc_thr_NUM_OF_BITS                               5
#define REG_Ssd_nmute_btsc_thr_MIN                                       -16
#define REG_Ssd_nmute_btsc_thr_MAX                                       15

/*!
 * @brief hysteresis size [dB] for automute of BTSC stereo
 * @range 0..15
 ***/
#define REG_Ssd_nmute_btsc_hyst
#define REG_Ssd_nmute_btsc_hyst_OFFSET                                     0x00048
#define REG_Ssd_nmute_btsc_hyst_MASK                                     0xf
#define REG_Ssd_nmute_btsc_hyst_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_nmute_btsc_hyst_BIT_MASK                                 0x1e0
#define REG_Ssd_nmute_btsc_hyst_BIT_IDX                                  5
#define REG_Ssd_nmute_btsc_hyst_NUM_OF_BITS                              4
#define REG_Ssd_nmute_btsc_hyst_MIN                                      0
#define REG_Ssd_nmute_btsc_hyst_MAX                                      15

/*!
 * @brief noise threshold for automute of FM RADIO stereo carrier (-16 means automute off)
 * @range -16..15
 ***/
#define REG_Ssd_nmute_fmra_thr
#define REG_Ssd_nmute_fmra_thr_OFFSET                                      0x00048
#define REG_Ssd_nmute_fmra_thr_MASK                                      0x1f
#define REG_Ssd_nmute_fmra_thr_SIGNED_BIT_MASK                           0x2000
#define REG_Ssd_nmute_fmra_thr_BIT_MASK                                  0x3e00
#define REG_Ssd_nmute_fmra_thr_BIT_IDX                                   9
#define REG_Ssd_nmute_fmra_thr_NUM_OF_BITS                               5
#define REG_Ssd_nmute_fmra_thr_MIN                                       -16
#define REG_Ssd_nmute_fmra_thr_MAX                                       15

/*!
 * @brief hysteresis size [dB] for automute of FM RADIO stereo
 * @range 0..15
 ***/
#define REG_Ssd_nmute_fmra_hyst
#define REG_Ssd_nmute_fmra_hyst_OFFSET                                     0x00048
#define REG_Ssd_nmute_fmra_hyst_MASK                                     0xf
#define REG_Ssd_nmute_fmra_hyst_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_nmute_fmra_hyst_BIT_MASK                                 0x3c000
#define REG_Ssd_nmute_fmra_hyst_BIT_IDX                                  14
#define REG_Ssd_nmute_fmra_hyst_NUM_OF_BITS                              4
#define REG_Ssd_nmute_fmra_hyst_MIN                                      0
#define REG_Ssd_nmute_fmra_hyst_MAX                                      15

/*!
 * @brief noise threshold for automute of EIAJ FM subcarrier (-16 means automute off)
 * @range -16..15
 ***/
#define REG_Ssd_nmute_eiaj_thr
#define REG_Ssd_nmute_eiaj_thr_OFFSET                                      0x0004C
#define REG_Ssd_nmute_eiaj_thr_MASK                                      0x1f
#define REG_Ssd_nmute_eiaj_thr_SIGNED_BIT_MASK                           0x10
#define REG_Ssd_nmute_eiaj_thr_BIT_MASK                                  0x1f
#define REG_Ssd_nmute_eiaj_thr_BIT_IDX                                   0
#define REG_Ssd_nmute_eiaj_thr_NUM_OF_BITS                               5
#define REG_Ssd_nmute_eiaj_thr_MIN                                       -16
#define REG_Ssd_nmute_eiaj_thr_MAX                                       15

/*!
 * @brief hysteresis size [dB] for automute of EIAJ FM subcarrier
 * @range 0..15
 ***/
#define REG_Ssd_nmute_eiaj_hyst
#define REG_Ssd_nmute_eiaj_hyst_OFFSET                                     0x0004C
#define REG_Ssd_nmute_eiaj_hyst_MASK                                     0xf
#define REG_Ssd_nmute_eiaj_hyst_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_nmute_eiaj_hyst_BIT_MASK                                 0x1e0
#define REG_Ssd_nmute_eiaj_hyst_BIT_IDX                                  5
#define REG_Ssd_nmute_eiaj_hyst_NUM_OF_BITS                              4
#define REG_Ssd_nmute_eiaj_hyst_MIN                                      0
#define REG_Ssd_nmute_eiaj_hyst_MAX                                      15

/*!
 * @brief upper threshold for EIAJ SUB carrier detection in dB below nominal level
 * @range 0..15
 ***/
#define REG_Ssd_eiaj_car_thr_up
#define REG_Ssd_eiaj_car_thr_up_OFFSET                                     0x0004C
#define REG_Ssd_eiaj_car_thr_up_MASK                                     0xf
#define REG_Ssd_eiaj_car_thr_up_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_eiaj_car_thr_up_BIT_MASK                                 0x1e00
#define REG_Ssd_eiaj_car_thr_up_BIT_IDX                                  9
#define REG_Ssd_eiaj_car_thr_up_NUM_OF_BITS                              4
#define REG_Ssd_eiaj_car_thr_up_MIN                                      0
#define REG_Ssd_eiaj_car_thr_up_MAX                                      15

/*!
 * @brief lower threshold for EIAJ SUB carrier detection in dB below nominal level
 * @range 0..15
 ***/
#define REG_Ssd_eiaj_car_thr_lo
#define REG_Ssd_eiaj_car_thr_lo_OFFSET                                     0x0004C
#define REG_Ssd_eiaj_car_thr_lo_MASK                                     0xf
#define REG_Ssd_eiaj_car_thr_lo_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_eiaj_car_thr_lo_BIT_MASK                                 0x1e000
#define REG_Ssd_eiaj_car_thr_lo_BIT_IDX                                  13
#define REG_Ssd_eiaj_car_thr_lo_NUM_OF_BITS                              4
#define REG_Ssd_eiaj_car_thr_lo_MIN                                      0
#define REG_Ssd_eiaj_car_thr_lo_MAX                                      15

/*!
 * @brief enable EIAJ SUB carrier detector
 *        0 = disabled | sub carrier detector disabled
 *        1 = enabled | sub carrier detector enabled
 * @range 0..1
 ***/
#define REG_Ssd_eiaj_car_detect
#define REG_Ssd_eiaj_car_detect_OFFSET                                     0x0004C
#define REG_Ssd_eiaj_car_detect_MASK                                     0x1
#define REG_Ssd_eiaj_car_detect_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_eiaj_car_detect_BIT_MASK                                 0x20000
#define REG_Ssd_eiaj_car_detect_BIT_IDX                                  17
#define REG_Ssd_eiaj_car_detect_NUM_OF_BITS                              1
#define REG_Ssd_eiaj_car_detect_MIN                                      0
#define REG_Ssd_eiaj_car_detect_MAX                                      1
#define REG_SSD_EIAJ_CAR_DETECT_VALUE_DISABLED                           0
#define REG_SSD_EIAJ_CAR_DETECT_VALUE_ENABLED                            1

/*!
 * @brief reproduce only related NICAM on DEC output (DDEP only)
 *        0 = false | (NICAM whenever possible)
 *        1 = true | (NICAM suppressed if RSSF=0)
 * @range 0..1
 ***/
#define REG_Ssd_only_related
#define REG_Ssd_only_related_OFFSET                                        0x00050
#define REG_Ssd_only_related_MASK                                        0x1
#define REG_Ssd_only_related_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_only_related_BIT_MASK                                    0x1
#define REG_Ssd_only_related_BIT_IDX                                     0
#define REG_Ssd_only_related_NUM_OF_BITS                                 1
#define REG_Ssd_only_related_MIN                                         0
#define REG_Ssd_only_related_MAX                                         1
#define REG_SSD_ONLY_RELATED_VALUE_FALSE                                 0
#define REG_SSD_ONLY_RELATED_VALUE_TRUE                                  1

/*!
 * @brief NICAM deemphasis (J17) (all modes)
 *        0 = ON
 *        1 = OFF
 * @range 0..1
 ***/
#define REG_Ssd_nicdeem
#define REG_Ssd_nicdeem_OFFSET                                             0x00050
#define REG_Ssd_nicdeem_MASK                                             0x1
#define REG_Ssd_nicdeem_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_nicdeem_BIT_MASK                                         0x8
#define REG_Ssd_nicdeem_BIT_IDX                                          3
#define REG_Ssd_nicdeem_NUM_OF_BITS                                      1
#define REG_Ssd_nicdeem_MIN                                              0
#define REG_Ssd_nicdeem_MAX                                              1
#define REG_SSD_NICDEEM_VALUE_ON                                         0
#define REG_SSD_NICDEEM_VALUE_OFF                                        1

/*!
 * @brief NICAM auto mute function depending on bit error rate (DDEP only)
 *        0 = ON
 *        1 = OFF
 * @range 0..1
 ***/
#define REG_Ssd_nic_amute
#define REG_Ssd_nic_amute_OFFSET                                           0x00050
#define REG_Ssd_nic_amute_MASK                                           0x1
#define REG_Ssd_nic_amute_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_nic_amute_BIT_MASK                                       0x10
#define REG_Ssd_nic_amute_BIT_IDX                                        4
#define REG_Ssd_nic_amute_NUM_OF_BITS                                    1
#define REG_Ssd_nic_amute_MIN                                            0
#define REG_Ssd_nic_amute_MAX                                            1
#define REG_SSD_NIC_AMUTE_VALUE_ON                                       0
#define REG_SSD_NIC_AMUTE_VALUE_OFF                                      1

/*!
 * @brief NICAM lower error limit (DDEP only)
 * @range 0..255
 ***/
#define REG_Ssd_nicloerrlim
#define REG_Ssd_nicloerrlim_OFFSET                                         0x00050
#define REG_Ssd_nicloerrlim_MASK                                         0xff
#define REG_Ssd_nicloerrlim_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_nicloerrlim_BIT_MASK                                     0x1fe0
#define REG_Ssd_nicloerrlim_BIT_IDX                                      5
#define REG_Ssd_nicloerrlim_NUM_OF_BITS                                  8
#define REG_Ssd_nicloerrlim_MIN                                          0
#define REG_Ssd_nicloerrlim_MAX                                          255

/*!
 * @brief NICAM upper error limit (DDEP only)
 * @range 0..255
 ***/
#define REG_Ssd_nicuperrlim
#define REG_Ssd_nicuperrlim_OFFSET                                         0x00050
#define REG_Ssd_nicuperrlim_MASK                                         0xff
#define REG_Ssd_nicuperrlim_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_nicuperrlim_BIT_MASK                                     0x1fe000
#define REG_Ssd_nicuperrlim_BIT_IDX                                      13
#define REG_Ssd_nicuperrlim_NUM_OF_BITS                                  8
#define REG_Ssd_nicuperrlim_MIN                                          0
#define REG_Ssd_nicuperrlim_MAX                                          255

/*!
 * @brief acquisition help for NICAM carrier loop
 *        0 = active
 *        1 = disabled
 * @range 0..1
 ***/
#define REG_Ssd_nicamcpll_acqhelp_off
#define REG_Ssd_nicamcpll_acqhelp_off_OFFSET                               0x00054
#define REG_Ssd_nicamcpll_acqhelp_off_MASK                               0x1
#define REG_Ssd_nicamcpll_acqhelp_off_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_nicamcpll_acqhelp_off_BIT_MASK                           0x1
#define REG_Ssd_nicamcpll_acqhelp_off_BIT_IDX                            0
#define REG_Ssd_nicamcpll_acqhelp_off_NUM_OF_BITS                        1
#define REG_Ssd_nicamcpll_acqhelp_off_MIN                                0
#define REG_Ssd_nicamcpll_acqhelp_off_MAX                                1
#define REG_SSD_NICAMCPLL_ACQHELP_OFF_VALUE_ACTIVE                       0
#define REG_SSD_NICAMCPLL_ACQHELP_OFF_VALUE_DISABLED                     1

/*!
 * @brief SAP filter bandwidth selection
 *        0 = narrow filter
 *        1 = wide filter
 * @range 0..1
 ***/
#define REG_Ssd_sap_bw
#define REG_Ssd_sap_bw_OFFSET                                              0x00054
#define REG_Ssd_sap_bw_MASK                                              0x1
#define REG_Ssd_sap_bw_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_sap_bw_BIT_MASK                                          0x4
#define REG_Ssd_sap_bw_BIT_IDX                                           2
#define REG_Ssd_sap_bw_NUM_OF_BITS                                       1
#define REG_Ssd_sap_bw_MIN                                               0
#define REG_Ssd_sap_bw_MAX                                               1
#define REG_SSD_SAP_BW_VALUE_NARROW_FILTER                               0
#define REG_SSD_SAP_BW_VALUE_WIDE_FILTER                                 1

/*!
 * @brief If OVMADAPT=0, attenuation of analog std. signals in dB
 * @range 0..31
 ***/
#define REG_Ssd_ovmatten
#define REG_Ssd_ovmatten_OFFSET                                            0x00054
#define REG_Ssd_ovmatten_MASK                                            0x1f
#define REG_Ssd_ovmatten_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_ovmatten_BIT_MASK                                        0x1f0
#define REG_Ssd_ovmatten_BIT_IDX                                         4
#define REG_Ssd_ovmatten_NUM_OF_BITS                                     5
#define REG_Ssd_ovmatten_MIN                                             0
#define REG_Ssd_ovmatten_MAX                                             31

/*!
 * @brief DEMDEC Easy Programming (DDEP) mode
 *        0 = ASD | 'AUTOSTANDARD' (ASD). STDSEL[4:0] defines the set of 'allowed' standards.
 *        1 = SSS | 'STATIC STANDARD SELECT' (SSS). STDSEL[4:0] contains standard code.
 *        3 = DEMDEC expert | DEMDEC expert mode (fully manual mode)
 * @range 0..3
 ***/
#define REG_Ssd_epmode
#define REG_Ssd_epmode_OFFSET                                              0x00058
#define REG_Ssd_epmode_MASK                                              0x3
#define REG_Ssd_epmode_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_epmode_BIT_MASK                                          0x3
#define REG_Ssd_epmode_BIT_IDX                                           0
#define REG_Ssd_epmode_NUM_OF_BITS                                       2
#define REG_Ssd_epmode_MIN                                               0
#define REG_Ssd_epmode_MAX                                               3
#define REG_SSD_EPMODE_VALUE_ASD                                         0
#define REG_SSD_EPMODE_VALUE_SSS                                         1
#define REG_SSD_EPMODE_VALUE_DEMDEC_EXPERT                               3

/*!
 * @brief In ASD mode (EPMODE=0): flags for allowed standards B/G | D/K | L/L' | I | M (LSB to MSB).
 *          1 = ASD MODE B/G
 *          2 = ASD MODE D/K
 *          4 = ASD MODE L/L'
 *          8 = ASD MODE I
 *        16 = ASD MODE M
 * @range 0..1
 ***/
#define REG_Ssd_stdsel_asd_mode
#define REG_Ssd_stdsel_asd_mode_OFFSET                                     0x00058
#define REG_Ssd_stdsel_asd_mode_MASK                                     0x1f
#define REG_Ssd_stdsel_asd_mode_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_stdsel_asd_mode_BIT_MASK                                 0x7c
#define REG_Ssd_stdsel_asd_mode_BIT_IDX                                  2
#define REG_Ssd_stdsel_asd_mode_NUM_OF_BITS                              5
#define REG_Ssd_stdsel_asd_mode_MIN                                      0
#define REG_Ssd_stdsel_asd_mode_MAX                                      31
#define REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_B_G                       1
#define REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_D_K                       2
#define REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_L_L                       4
#define REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_I                         8
#define REG_SSD_STDSEL_ASD_MODE_VALUE_ASD_MODE_M                         16

/*!
 * @brief Bits multiplexed for ASD and SSS modes.
 *        In ASD mode (EPMODE=0): flags for allowed standards B/G | D/K | L/L' | I | M (LSB to MSB).
 *        In SSS mode (EPMODE=1): standard code as defined in status register STDRES, e.g. code 4 selects B/G A2.
 *        For details please consult the documentation.
 *          4 = B/G A2
 *          5 = B/G NICAM
 *          6 = D/K A2 (1)
 *          7 = D/K A2 (2)
 *          8 = D/K A2 (3)
 *          9 = D/K NICAM
 *        10 = L NICAM / L'
 *        11 = I NICAM
 *        12 = M Korea
 *        13 = M BTSC
 *        14 = M EIAJ
 *        15 = FM Radio, IF = 10.7 MHz, 50 us deemphasis
 *        16 = FM Radio, IF = 10.7 MHz, 75 us deemphasis
 *        17 = FM Radio, selectable IF, 50 us deemphasis
 *        18 = FM Radio, selectable IF, 75 us deemphasis
 * @range 0..31
 ***/
#define REG_Ssd_stdsel
#define REG_Ssd_stdsel_OFFSET                                              0x00058
#define REG_Ssd_stdsel_MASK                                              0x1f
#define REG_Ssd_stdsel_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_stdsel_BIT_MASK                                          0x7c
#define REG_Ssd_stdsel_BIT_IDX                                           2
#define REG_Ssd_stdsel_NUM_OF_BITS                                       5
#define REG_Ssd_stdsel_MIN                                               0
#define REG_Ssd_stdsel_MAX                                               31
#define REG_SSD_STDSEL_VALUE_B_G_A2                                      4
#define REG_SSD_STDSEL_VALUE_B_G_NICAM                                   5
#define REG_SSD_STDSEL_VALUE_D_K_A2_1                                    6
#define REG_SSD_STDSEL_VALUE_D_K_A2_2                                    7
#define REG_SSD_STDSEL_VALUE_D_K_A2_3                                    8
#define REG_SSD_STDSEL_VALUE_D_K_NICAM                                   9
#define REG_SSD_STDSEL_VALUE_L_NICAM_L                                   10
#define REG_SSD_STDSEL_VALUE_I_NICAM                                     11
#define REG_SSD_STDSEL_VALUE_M_KOREA                                     12
#define REG_SSD_STDSEL_VALUE_M_BTSC                                      13
#define REG_SSD_STDSEL_VALUE_M_EIAJ                                      14
#define REG_SSD_STDSEL_VALUE_FM_RADIO_IF_10_7_MHZ_50_US_DEEMPHASIS       15
#define REG_SSD_STDSEL_VALUE_FM_RADIO_IF_10_7_MHZ_75_US_DEEMPHASIS       16
#define REG_SSD_STDSEL_VALUE_FM_RADIO_SELECTABLE_IF_50_US_DEEMPHASIS     17
#define REG_SSD_STDSEL_VALUE_FM_RADIO_SELECTABLE_IF_75_US_DEEMPHASIS     18

/*!
 * @brief RESTART decoder and initialize DEMDEC after channel switch, if changed from 0 to 1.
 *        0 = running | DEMDEC running
 *        1 = restart | restart DemDec (only in transtition to '1')
 * @range 0..1
 ***/
#define REG_Ssd_rest
#define REG_Ssd_rest_OFFSET                                                0x00058
#define REG_Ssd_rest_MASK                                                0x1
#define REG_Ssd_rest_SIGNED_BIT_MASK                                     0x0
#define REG_Ssd_rest_BIT_MASK                                            0x80
#define REG_Ssd_rest_BIT_IDX                                             7
#define REG_Ssd_rest_NUM_OF_BITS                                         1
#define REG_Ssd_rest_MIN                                                 0
#define REG_Ssd_rest_MAX                                                 1
#define REG_SSD_REST_VALUE_RUNNING                                       0
#define REG_SSD_REST_VALUE_RESTART                                       1

/*!
 * @brief FM overmodulation adaptation  (avoids distortion, filter bandwidth and gain is chosen adaptively)
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_ovmadapt
#define REG_Ssd_ovmadapt_OFFSET                                            0x00058
#define REG_Ssd_ovmadapt_MASK                                            0x1
#define REG_Ssd_ovmadapt_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_ovmadapt_BIT_MASK                                        0x100
#define REG_Ssd_ovmadapt_BIT_IDX                                         8
#define REG_Ssd_ovmadapt_NUM_OF_BITS                                     1
#define REG_Ssd_ovmadapt_MIN                                             0
#define REG_Ssd_ovmadapt_MAX                                             1
#define REG_SSD_OVMADAPT_VALUE_DISABLED                                  0
#define REG_SSD_OVMADAPT_VALUE_ENABLED                                   1

/*!
 * @brief mute DEMDEC output signals (softmute)
 *        0 = no mute
 *        1 = mute
 * @range 0..1
 ***/
#define REG_Ssd_ddmute
#define REG_Ssd_ddmute_OFFSET                                              0x00058
#define REG_Ssd_ddmute_MASK                                              0x1
#define REG_Ssd_ddmute_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_ddmute_BIT_MASK                                          0x200
#define REG_Ssd_ddmute_BIT_IDX                                           9
#define REG_Ssd_ddmute_NUM_OF_BITS                                       1
#define REG_Ssd_ddmute_MIN                                               0
#define REG_Ssd_ddmute_MAX                                               1
#define REG_SSD_DDMUTE_VALUE_NO_MUTE                                     0
#define REG_SSD_DDMUTE_VALUE_MUTE                                        1

/*!
 * @brief FM/AM demodulator filter bandwidth (like FILTBW_M).  NOT effective if BTSC, EIAJ, FMRADIO active, or if OVMADAPT=1
 *        0 = narrow
 *        2 = medium
 *        3 = wide
 *        1 = extra wide | (only ch. 1 active)
 * @range 0..3
 ***/
#define REG_Ssd_filtbw
#define REG_Ssd_filtbw_OFFSET                                              0x00058
#define REG_Ssd_filtbw_MASK                                              0x3
#define REG_Ssd_filtbw_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_filtbw_BIT_MASK                                          0xc00
#define REG_Ssd_filtbw_BIT_IDX                                           10
#define REG_Ssd_filtbw_NUM_OF_BITS                                       2
#define REG_Ssd_filtbw_MIN                                               0
#define REG_Ssd_filtbw_MAX                                               3
#define REG_SSD_FILTBW_VALUE_NARROW                                      0
#define REG_SSD_FILTBW_VALUE_EXTRA_WIDE                                  1
#define REG_SSD_FILTBW_VALUE_MEDIUM                                      2
#define REG_SSD_FILTBW_VALUE_WIDE                                        3

/*!
 * @brief =R[16]C, in ASD mode used after first detection
 *        0 = slow
 *        1 = medium
 *        2 = fast
 * @range 0..3
 ***/
#define REG_Ssd_idmod
#define REG_Ssd_idmod_OFFSET                                               0x00058
#define REG_Ssd_idmod_MASK                                               0x3
#define REG_Ssd_idmod_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_idmod_BIT_MASK                                           0x3000
#define REG_Ssd_idmod_BIT_IDX                                            12
#define REG_Ssd_idmod_NUM_OF_BITS                                        2
#define REG_Ssd_idmod_MIN                                                0
#define REG_Ssd_idmod_MAX                                                3
#define REG_SSD_IDMOD_VALUE_SLOW                                         0
#define REG_SSD_IDMOD_VALUE_MEDIUM                                       1
#define REG_SSD_IDMOD_VALUE_FAST                                         2

/*!
 * @brief SAP Noise Reduction enable
 *        0 = 150 us deemphasis for SAP, Noise Reduction for BTSC stereo
 *        1 = Noise Reduction used for SAP, BTSC stereo forced to mono
 * @range 0..1
 ***/
#define REG_Ssd_sapnr
#define REG_Ssd_sapnr_OFFSET                                               0x00058
#define REG_Ssd_sapnr_MASK                                               0x1
#define REG_Ssd_sapnr_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_sapnr_BIT_MASK                                           0x10000
#define REG_Ssd_sapnr_BIT_IDX                                            16
#define REG_Ssd_sapnr_NUM_OF_BITS                                        1
#define REG_Ssd_sapnr_MIN                                                0
#define REG_Ssd_sapnr_MAX                                                1
#define REG_SSD_SAPNR_VALUE_150_US_DEEMPHASIS_FOR_SAP_NOISE_REDUCTION_FOR_BTSC_STEREO 0
#define REG_SSD_SAPNR_VALUE_NOISE_REDUCTION_USED_FOR_SAP_BTSC_STEREO_FORCED_TO_MONO 1

/*!
 * @brief overmodulation level threshold relative to nominal (applies if OVMADAPT=1)
 *        0 = +3 dB -12 dBFS
 *        1 = +6 dB -9 dBFS
 *        2 = +9 dB -6 dBFS
 *        3 = +12 dB -3 dBFS
 * @range 0..3
 ***/
#define REG_Ssd_ovmthr
#define REG_Ssd_ovmthr_OFFSET                                              0x00058
#define REG_Ssd_ovmthr_MASK                                              0x3
#define REG_Ssd_ovmthr_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_ovmthr_BIT_MASK                                          0xc0000
#define REG_Ssd_ovmthr_BIT_IDX                                           18
#define REG_Ssd_ovmthr_NUM_OF_BITS                                       2
#define REG_Ssd_ovmthr_MIN                                               0
#define REG_Ssd_ovmthr_MAX                                               3
#define REG_SSD_OVMTHR_VALUE_3_DB_MINUS_12_DBFS                          0
#define REG_SSD_OVMTHR_VALUE_6_DB_MINUS_9_DBFS                           1
#define REG_SSD_OVMTHR_VALUE_9_DB_MINUS_6_DBFS                           2
#define REG_SSD_OVMTHR_VALUE_12_DB_MINUS_3_DBFS                          3

/*!
 * @brief fixed deemphasis for analog sound signals (not NICAM, not BTSC)
 *        0 = 50 us (Europe)
 *        1 = 60 us
 *        2 = 75 us (M standard)
 *        3 = J17
 *        4 = OFF (flat)
 * @range 0..7
 ***/
#define REG_Ssd_fmdeem
#define REG_Ssd_fmdeem_OFFSET                                              0x0005C
#define REG_Ssd_fmdeem_MASK                                              0x7
#define REG_Ssd_fmdeem_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_fmdeem_BIT_MASK                                          0x38
#define REG_Ssd_fmdeem_BIT_IDX                                           3
#define REG_Ssd_fmdeem_NUM_OF_BITS                                       3
#define REG_Ssd_fmdeem_MIN                                               0
#define REG_Ssd_fmdeem_MAX                                               7
#define REG_SSD_FMDEEM_VALUE_50_US_EUROPE                                0
#define REG_SSD_FMDEEM_VALUE_60_US                                       1
#define REG_SSD_FMDEEM_VALUE_75_US_M_STANDARD                            2
#define REG_SSD_FMDEEM_VALUE_J17                                         3
#define REG_SSD_FMDEEM_VALUE_OFF_FLAT                                    4

/*!
 * @brief operating mode of demodulator channel 2
 *        0 = FM mode
 *        1 = AM mode
 *        2 = NICAM
 *        3 = not used
 * @range 0..3
 ***/
#define REG_Ssd_ch2mod
#define REG_Ssd_ch2mod_OFFSET                                              0x0005C
#define REG_Ssd_ch2mod_MASK                                              0x3
#define REG_Ssd_ch2mod_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_ch2mod_BIT_MASK                                          0xc0
#define REG_Ssd_ch2mod_BIT_IDX                                           6
#define REG_Ssd_ch2mod_NUM_OF_BITS                                       2
#define REG_Ssd_ch2mod_MIN                                               0
#define REG_Ssd_ch2mod_MAX                                               3
#define REG_SSD_CH2MOD_VALUE_FM_MODE                                     0
#define REG_SSD_CH2MOD_VALUE_AM_MODE                                     1
#define REG_SSD_CH2MOD_VALUE_NICAM                                       2
#define REG_SSD_CH2MOD_VALUE_NOT_USED                                    3

/*!
 * @brief operating mode of demodulator channel 1
 *        0 = FM mode
 *        1 = AM mode
 * @range 0..1
 ***/
#define REG_Ssd_ch1mod
#define REG_Ssd_ch1mod_OFFSET                                              0x0005C
#define REG_Ssd_ch1mod_MASK                                              0x1
#define REG_Ssd_ch1mod_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_ch1mod_BIT_MASK                                          0x100
#define REG_Ssd_ch1mod_BIT_IDX                                           8
#define REG_Ssd_ch1mod_NUM_OF_BITS                                       1
#define REG_Ssd_ch1mod_MIN                                               0
#define REG_Ssd_ch1mod_MAX                                               1
#define REG_SSD_CH1MOD_VALUE_FM_MODE                                     0
#define REG_SSD_CH1MOD_VALUE_AM_MODE                                     1

/*!
 * @brief initialize loop filters in demodulator
 *        0 = normal operation
 *        1 = initialize | (reset states to 0)
 * @range 0..1
 ***/
#define REG_Ssd_initlpf
#define REG_Ssd_initlpf_OFFSET                                             0x0005C
#define REG_Ssd_initlpf_MASK                                             0x1
#define REG_Ssd_initlpf_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_initlpf_BIT_MASK                                         0x200
#define REG_Ssd_initlpf_BIT_IDX                                          9
#define REG_Ssd_initlpf_NUM_OF_BITS                                      1
#define REG_Ssd_initlpf_MIN                                              0
#define REG_Ssd_initlpf_MAX                                              1
#define REG_SSD_INITLPF_VALUE_NORMAL_OPERATION                           0
#define REG_SSD_INITLPF_VALUE_INITIALIZE                                 1

/*!
 * @brief FM/AM demodulator filter bandwidth
 *        0 = narrow
 *        2 = medium
 *        3 = wide
 *        1 = extra wide | (only ch. 1 active)
 * @range 0..3
 ***/
#define REG_Ssd_filtbw_m
#define REG_Ssd_filtbw_m_OFFSET                                            0x0005C
#define REG_Ssd_filtbw_m_MASK                                            0x3
#define REG_Ssd_filtbw_m_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_filtbw_m_BIT_MASK                                        0x1800
#define REG_Ssd_filtbw_m_BIT_IDX                                         11
#define REG_Ssd_filtbw_m_NUM_OF_BITS                                     2
#define REG_Ssd_filtbw_m_MIN                                             0
#define REG_Ssd_filtbw_m_MAX                                             3
#define REG_SSD_FILTBW_M_VALUE_NARROW                                    0
#define REG_SSD_FILTBW_M_VALUE_EXTRA_WIDE                                1
#define REG_SSD_FILTBW_M_VALUE_MEDIUM                                    2
#define REG_SSD_FILTBW_M_VALUE_WIDE                                      3

/*!
 * @brief FM identification (hardware) detection mode
 *        0 = slow
 *        1 = medium
 *        2 = fast
 *        3 = off (reset)
 * @range 0..3
 ***/
#define REG_Ssd_idmod_m
#define REG_Ssd_idmod_m_OFFSET                                             0x0005C
#define REG_Ssd_idmod_m_MASK                                             0x3
#define REG_Ssd_idmod_m_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_idmod_m_BIT_MASK                                         0x6000
#define REG_Ssd_idmod_m_BIT_IDX                                          13
#define REG_Ssd_idmod_m_NUM_OF_BITS                                      2
#define REG_Ssd_idmod_m_MIN                                              0
#define REG_Ssd_idmod_m_MAX                                              3
#define REG_SSD_IDMOD_M_VALUE_SLOW                                       0
#define REG_SSD_IDMOD_M_VALUE_MEDIUM                                     1
#define REG_SSD_IDMOD_M_VALUE_FAST                                       2
#define REG_SSD_IDMOD_M_VALUE_OFF_RESET                                  3

/*!
 * @brief Area/regional code for FM-ident: Europe, Korea, Japan
 *        0 = Europe
 *        1 = Korea
 *        2 = Japan
 * @range 0..3
 ***/
#define REG_Ssd_idarea
#define REG_Ssd_idarea_OFFSET                                              0x0005C
#define REG_Ssd_idarea_MASK                                              0x3
#define REG_Ssd_idarea_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_idarea_BIT_MASK                                          0x18000
#define REG_Ssd_idarea_BIT_IDX                                           15
#define REG_Ssd_idarea_NUM_OF_BITS                                       2
#define REG_Ssd_idarea_MIN                                               0
#define REG_Ssd_idarea_MAX                                               3
#define REG_SSD_IDAREA_VALUE_EUROPE                                      0
#define REG_SSD_IDAREA_VALUE_KOREA                                       1
#define REG_SSD_IDAREA_VALUE_JAPAN                                       2

/*!
 * @brief MPX pilot cancellation
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_bpilcan
#define REG_Ssd_bpilcan_OFFSET                                             0x0005C
#define REG_Ssd_bpilcan_MASK                                             0x1
#define REG_Ssd_bpilcan_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_bpilcan_BIT_MASK                                         0x20000
#define REG_Ssd_bpilcan_BIT_IDX                                          17
#define REG_Ssd_bpilcan_NUM_OF_BITS                                      1
#define REG_Ssd_bpilcan_MIN                                              0
#define REG_Ssd_bpilcan_MAX                                              1
#define REG_SSD_BPILCAN_VALUE_FALSE                                      0
#define REG_SSD_BPILCAN_VALUE_TRUE                                       1

/*!
 * @brief input from demodulator hardware at 4*fs
 *        0 = FM / AM output
 *        1 = MPX demodulator output (for BTSC and FM RADIO)
 * @range 0..1
 ***/
#define REG_Ssd_fm_mpx
#define REG_Ssd_fm_mpx_OFFSET                                              0x0005C
#define REG_Ssd_fm_mpx_MASK                                              0x1
#define REG_Ssd_fm_mpx_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_fm_mpx_BIT_MASK                                          0x40000
#define REG_Ssd_fm_mpx_BIT_IDX                                           18
#define REG_Ssd_fm_mpx_NUM_OF_BITS                                       1
#define REG_Ssd_fm_mpx_MIN                                               0
#define REG_Ssd_fm_mpx_MAX                                               1
#define REG_SSD_FM_MPX_VALUE_FM_AM_OUTPUT                                0
#define REG_SSD_FM_MPX_VALUE_MPX_DEMODULATOR_OUTPUT_FOR_BTSC_AND_FM_RADIO 1

/*!
 * @brief DC level for IDENT pilot detection
 *        0 = Level > 3
 *        1 = Level > 4
 *        2 = Level > 5
 *        3 = Level > 6
 * @range 0..3
 ***/
#define REG_Ssd_id_dc_level
#define REG_Ssd_id_dc_level_OFFSET                                         0x0005C
#define REG_Ssd_id_dc_level_MASK                                         0x3
#define REG_Ssd_id_dc_level_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_id_dc_level_BIT_MASK                                     0x180000
#define REG_Ssd_id_dc_level_BIT_IDX                                      19
#define REG_Ssd_id_dc_level_NUM_OF_BITS                                  2
#define REG_Ssd_id_dc_level_MIN                                          0
#define REG_Ssd_id_dc_level_MAX                                          3
#define REG_SSD_ID_DC_LEVEL_VALUE_LEVEL_3                                0
#define REG_SSD_ID_DC_LEVEL_VALUE_LEVEL_4                                1
#define REG_SSD_ID_DC_LEVEL_VALUE_LEVEL_5                                2
#define REG_SSD_ID_DC_LEVEL_VALUE_LEVEL_6                                3

/*!
 * @brief bypass bandpass filter level detector
 *        0 = off
 *        1 = on (reduced IDENT sensitivity)
 * @range 0..1
 ***/
#define REG_Ssd_id_bybpf
#define REG_Ssd_id_bybpf_OFFSET                                            0x0005C
#define REG_Ssd_id_bybpf_MASK                                            0x1
#define REG_Ssd_id_bybpf_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_id_bybpf_BIT_MASK                                        0x200000
#define REG_Ssd_id_bybpf_BIT_IDX                                         21
#define REG_Ssd_id_bybpf_NUM_OF_BITS                                     1
#define REG_Ssd_id_bybpf_MIN                                             0
#define REG_Ssd_id_bybpf_MAX                                             1
#define REG_SSD_ID_BYBPF_VALUE_OFF                                       0
#define REG_SSD_ID_BYBPF_VALUE_ON_REDUCED_IDENT_SENSITIVITY              1

/*!
 * @brief IDENT pilot bandpass gain
 *        0 = no gain
 *        1 = +6 dB gain for EIAJ
 * @range 0..1
 ***/
#define REG_Ssd_id_pgain
#define REG_Ssd_id_pgain_OFFSET                                            0x0005C
#define REG_Ssd_id_pgain_MASK                                            0x1
#define REG_Ssd_id_pgain_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_id_pgain_BIT_MASK                                        0x400000
#define REG_Ssd_id_pgain_BIT_IDX                                         22
#define REG_Ssd_id_pgain_NUM_OF_BITS                                     1
#define REG_Ssd_id_pgain_MIN                                             0
#define REG_Ssd_id_pgain_MAX                                             1
#define REG_SSD_ID_PGAIN_VALUE_NO_GAIN                                   0
#define REG_SSD_ID_PGAIN_VALUE_6_DB_GAIN_FOR_EIAJ                        1

/*!
 * @brief sound carrier 1 (mixer 1) frequency, also in DDEP mode for FM radio (std. codes 17 and 18)
 *        $555555 = 4.5 MHz
 *        $684BDA = 5.5 MHz
 *        $71C71C = 6.0 MHz
 *        $7B425E = 6.5 MHz
 *        $CAE759 = 10.7 MHz
 * @range 0..16777215
 ***/
#define REG_Ssd_carrier1
#define REG_Ssd_carrier1_OFFSET                                            0x00060
#define REG_Ssd_carrier1_MASK                                            0xffffff
#define REG_Ssd_carrier1_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_carrier1_BIT_MASK                                        0xffffff
#define REG_Ssd_carrier1_BIT_IDX                                         0
#define REG_Ssd_carrier1_NUM_OF_BITS                                     24
#define REG_Ssd_carrier1_MIN                                             0
#define REG_Ssd_carrier1_MAX                                             16777215
#define REG_SSD_CARRIER1_VALUE_4_5_MHZ                                   5592405
#define REG_SSD_CARRIER1_VALUE_5_5_MHZ                                   6835162
#define REG_SSD_CARRIER1_VALUE_6_0_MHZ                                   7456540
#define REG_SSD_CARRIER1_VALUE_6_5_MHZ                                   8077918
#define REG_SSD_CARRIER1_VALUE_10_7_MHZ                                  13297497

/*!
 * @brief sound carrier 2 (mixer 2)  frequency
 *        $5994BF = 4.724 MHz
 *        $6CE2A5 = 5.742 MHz
 *        $6EEEEF = 5.85 MHz
 *        $7C3ECE = 6.552 MHz
 *        $76AB94 = 6.258 MHz
 *        $555555 = 4.5 MHz
 *        $684BDA = 5.5 MHz
 * @range 0..16777215
 ***/
#define REG_Ssd_carrier2
#define REG_Ssd_carrier2_OFFSET                                            0x00064
#define REG_Ssd_carrier2_MASK                                            0xffffff
#define REG_Ssd_carrier2_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_carrier2_BIT_MASK                                        0xffffff
#define REG_Ssd_carrier2_BIT_IDX                                         0
#define REG_Ssd_carrier2_NUM_OF_BITS                                     24
#define REG_Ssd_carrier2_MIN                                             0
#define REG_Ssd_carrier2_MAX                                             16777215
#define REG_SSD_CARRIER2_VALUE_4_5_MHZ                                   5592405
#define REG_SSD_CARRIER2_VALUE_4_724_MHZ                                 5870783
#define REG_SSD_CARRIER2_VALUE_5_5_MHZ                                   6835162
#define REG_SSD_CARRIER2_VALUE_5_742_MHZ                                 7135909
#define REG_SSD_CARRIER2_VALUE_5_85_MHZ                                  7270127
#define REG_SSD_CARRIER2_VALUE_6_258_MHZ                                 7777172
#define REG_SSD_CARRIER2_VALUE_6_552_MHZ                                 8142542

/*!
 * @brief MPX demodulator pilot PLL bandwidth
 *        0 = 5Hz (default)
 *        1 = 10Hz
 * @range 0..1
 ***/
#define REG_Ssd_mpx_pll_bw
#define REG_Ssd_mpx_pll_bw_OFFSET                                          0x00068
#define REG_Ssd_mpx_pll_bw_MASK                                          0x1
#define REG_Ssd_mpx_pll_bw_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_mpx_pll_bw_BIT_MASK                                      0x2
#define REG_Ssd_mpx_pll_bw_BIT_IDX                                       1
#define REG_Ssd_mpx_pll_bw_NUM_OF_BITS                                   1
#define REG_Ssd_mpx_pll_bw_MIN                                           0
#define REG_Ssd_mpx_pll_bw_MAX                                           1
#define REG_SSD_MPX_PLL_BW_VALUE_5HZ_DEFAULT                             0
#define REG_SSD_MPX_PLL_BW_VALUE_10HZ                                    1

/*!
 * @brief MPX pilot frequency
 *        $2630F =15734 Hz (standard NTSC line frequency)
 *        $25ED1 = 15625 Hz (PAL line frequency)
 *        $2E1E3 = 19000 Hz (FM radio)
 * @range 0..4194303
 ***/
#define REG_Ssd_mpx_freq
#define REG_Ssd_mpx_freq_OFFSET                                            0x00068
#define REG_Ssd_mpx_freq_MASK                                            0x3fffff
#define REG_Ssd_mpx_freq_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mpx_freq_BIT_MASK                                        0xfffffc
#define REG_Ssd_mpx_freq_BIT_IDX                                         2
#define REG_Ssd_mpx_freq_NUM_OF_BITS                                     22
#define REG_Ssd_mpx_freq_MIN                                             0
#define REG_Ssd_mpx_freq_MAX                                             4194303
#define REG_SSD_MPX_FREQ_VALUE_15625_HZ_PAL_LINE_FREQUENCY               155345
#define REG_SSD_MPX_FREQ_VALUE_15734_HZ_STANDARD_NTSC_LINE_FREQUENCY     156431
#define REG_SSD_MPX_FREQ_VALUE_19000_HZ_FM_RADIO                         188899

/*!
 * @brief FM subchannel and EIAJ MAIN filter bandwidth
 *        0 = narrow
 *        1 = wide
 * @range 0..1
 ***/
#define REG_Ssd_fmsub_bw
#define REG_Ssd_fmsub_bw_OFFSET                                            0x0006C
#define REG_Ssd_fmsub_bw_MASK                                            0x1
#define REG_Ssd_fmsub_bw_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_fmsub_bw_BIT_MASK                                        0x1
#define REG_Ssd_fmsub_bw_BIT_IDX                                         0
#define REG_Ssd_fmsub_bw_NUM_OF_BITS                                     1
#define REG_Ssd_fmsub_bw_MIN                                             0
#define REG_Ssd_fmsub_bw_MAX                                             1
#define REG_SSD_FMSUB_BW_VALUE_NARROW                                    0
#define REG_SSD_FMSUB_BW_VALUE_WIDE                                      1

/*!
 * @brief delay fine adjustment in MAIN path for EIAJ stereo
 * @range 0..3
 ***/
#define REG_Ssd_eiaj_delay
#define REG_Ssd_eiaj_delay_OFFSET                                          0x0006C
#define REG_Ssd_eiaj_delay_MASK                                          0x3
#define REG_Ssd_eiaj_delay_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_eiaj_delay_BIT_MASK                                      0x6
#define REG_Ssd_eiaj_delay_BIT_IDX                                       1
#define REG_Ssd_eiaj_delay_NUM_OF_BITS                                   2
#define REG_Ssd_eiaj_delay_MIN                                           0
#define REG_Ssd_eiaj_delay_MAX                                           3

/*!
 * @brief noise detector channel
 *        0 = channel 1
 *        1 = channel 2
 * @range 0..1
 ***/
#define REG_Ssd_ndetch
#define REG_Ssd_ndetch_OFFSET                                              0x0006C
#define REG_Ssd_ndetch_MASK                                              0x1
#define REG_Ssd_ndetch_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_ndetch_BIT_MASK                                          0x8
#define REG_Ssd_ndetch_BIT_IDX                                           3
#define REG_Ssd_ndetch_NUM_OF_BITS                                       1
#define REG_Ssd_ndetch_MIN                                               0
#define REG_Ssd_ndetch_MAX                                               1
#define REG_SSD_NDETCH_VALUE_CHANNEL_1                                   0
#define REG_SSD_NDETCH_VALUE_CHANNEL_2                                   1

/*!
 * @brief noise detector passband
 *        0 = low (2.5 fh)
 *        1 = high (7.5 fh)
 * @range 0..1
 ***/
#define REG_Ssd_ndetpb
#define REG_Ssd_ndetpb_OFFSET                                              0x0006C
#define REG_Ssd_ndetpb_MASK                                              0x1
#define REG_Ssd_ndetpb_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_ndetpb_BIT_MASK                                          0x10
#define REG_Ssd_ndetpb_BIT_IDX                                           4
#define REG_Ssd_ndetpb_NUM_OF_BITS                                       1
#define REG_Ssd_ndetpb_MIN                                               0
#define REG_Ssd_ndetpb_MAX                                               1
#define REG_SSD_NDETPB_VALUE_LOW_2_5_FH                                  0
#define REG_SSD_NDETPB_VALUE_HIGH_7_5_FH                                 1

/*!
 * @brief FM subchannel frequency (SAP or Japan)
 *        $2FBD = 5 fh for SAP
 *        $1318 = 2 fh for EIAJ
 * @range 0..65535
 ***/
#define REG_Ssd_fmsub_freq
#define REG_Ssd_fmsub_freq_OFFSET                                          0x0006C
#define REG_Ssd_fmsub_freq_MASK                                          0xffff
#define REG_Ssd_fmsub_freq_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_fmsub_freq_BIT_MASK                                      0xffff00
#define REG_Ssd_fmsub_freq_BIT_IDX                                       8
#define REG_Ssd_fmsub_freq_NUM_OF_BITS                                   16
#define REG_Ssd_fmsub_freq_MIN                                           0
#define REG_Ssd_fmsub_freq_MAX                                           65535
#define REG_SSD_FMSUB_FREQ_VALUE_2_FH_FOR_EIAJ                           4888
#define REG_SSD_FMSUB_FREQ_VALUE_5_FH_FOR_SAP                            12221

/*!
 * @brief DECoder processing mode
 *        0=FM A2
 *        1=BTSC stereo + SAP without noise reduction
 *        2=BTSC mono + SAP with NR
 *        3=EIAJ
 *        4=FM Radio
 *        5=unused
 *        6=BTSC flat (test mode)
 *        7=NICAM + FM MONO
 * @range 0..7
 ***/
#define REG_Ssd_decmode
#define REG_Ssd_decmode_OFFSET                                             0x00070
#define REG_Ssd_decmode_MASK                                             0x7
#define REG_Ssd_decmode_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_decmode_BIT_MASK                                         0x7
#define REG_Ssd_decmode_BIT_IDX                                          0
#define REG_Ssd_decmode_NUM_OF_BITS                                      3
#define REG_Ssd_decmode_MIN                                              0
#define REG_Ssd_decmode_MAX                                              7
#define REG_SSD_DECMODE_VALUE_FM_A2                                      0
#define REG_SSD_DECMODE_VALUE_BTSC_STEREO_SAP_WITHOUT_NOISE_REDUCTION    1
#define REG_SSD_DECMODE_VALUE_BTSC_MONO_SAP_WITH_NR                      2
#define REG_SSD_DECMODE_VALUE_EIAJ                                       3
#define REG_SSD_DECMODE_VALUE_FM_RADIO                                   4
#define REG_SSD_DECMODE_VALUE_UNUSED                                     5
#define REG_SSD_DECMODE_VALUE_BTSC_FLAT_TEST_MODE                        6
#define REG_SSD_DECMODE_VALUE_NICAM_FM_MONO                              7

/*!
 * @brief FM dematrix
 *        0 = mono CH1
 *        1 = mono CH2
 *        2 = dual | (identity matrix)
 *        3 = stereo Europe
 *        4 = stereo M standards | (BTSC, Korea, Japan) and FM Radio
 * @range 0..7
 ***/
#define REG_Ssd_fmdemat
#define REG_Ssd_fmdemat_OFFSET                                             0x00070
#define REG_Ssd_fmdemat_MASK                                             0x7
#define REG_Ssd_fmdemat_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_fmdemat_BIT_MASK                                         0x70
#define REG_Ssd_fmdemat_BIT_IDX                                          4
#define REG_Ssd_fmdemat_NUM_OF_BITS                                      3
#define REG_Ssd_fmdemat_MIN                                              0
#define REG_Ssd_fmdemat_MAX                                              7
#define REG_SSD_FMDEMAT_VALUE_MONO_CH1                                   0
#define REG_SSD_FMDEMAT_VALUE_MONO_CH2                                   1
#define REG_SSD_FMDEMAT_VALUE_DUAL                                       2
#define REG_SSD_FMDEMAT_VALUE_STEREO_EUROPE                              3
#define REG_SSD_FMDEMAT_VALUE_STEREO_M_STANDARDS                         4

/*!
 * @brief mute DEC and MONO outputs (softmute)
 *        0 = no mute
 *        1 = mute
 * @range 0..1
 ***/
#define REG_Ssd_mute_dec_mono
#define REG_Ssd_mute_dec_mono_OFFSET                                       0x00070
#define REG_Ssd_mute_dec_mono_MASK                                       0x1
#define REG_Ssd_mute_dec_mono_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_mute_dec_mono_BIT_MASK                                   0x80
#define REG_Ssd_mute_dec_mono_BIT_IDX                                    7
#define REG_Ssd_mute_dec_mono_NUM_OF_BITS                                1
#define REG_Ssd_mute_dec_mono_MIN                                        0
#define REG_Ssd_mute_dec_mono_MAX                                        1
#define REG_SSD_MUTE_DEC_MONO_VALUE_NO_MUTE                              0
#define REG_SSD_MUTE_DEC_MONO_VALUE_MUTE                                 1

/*!
 * @brief mute SAP output (softmute)
 *        0 = no mute
 *        1 = mute
 * @range 0..1
 ***/
#define REG_Ssd_mute_sap
#define REG_Ssd_mute_sap_OFFSET                                            0x00070
#define REG_Ssd_mute_sap_MASK                                            0x1
#define REG_Ssd_mute_sap_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mute_sap_BIT_MASK                                        0x100
#define REG_Ssd_mute_sap_BIT_IDX                                         8
#define REG_Ssd_mute_sap_NUM_OF_BITS                                     1
#define REG_Ssd_mute_sap_MIN                                             0
#define REG_Ssd_mute_sap_MAX                                             1
#define REG_SSD_MUTE_SAP_VALUE_NO_MUTE                                   0
#define REG_SSD_MUTE_SAP_VALUE_MUTE                                      1

/*!
 * @brief scaling of FM and FM A2 signals
 *        0 = 27 kHz nominal FM deviation (Europe)
 *        1 = 15 kHz nominal FM deviation (M standards)
 *        2 = 0 dB gain (BTSC, EIAJ, FM Radio)
 * @range 0..3
 ***/
#define REG_Ssd_fm_scale
#define REG_Ssd_fm_scale_OFFSET                                            0x00070
#define REG_Ssd_fm_scale_MASK                                            0x3
#define REG_Ssd_fm_scale_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_fm_scale_BIT_MASK                                        0xc00
#define REG_Ssd_fm_scale_BIT_IDX                                         10
#define REG_Ssd_fm_scale_NUM_OF_BITS                                     2
#define REG_Ssd_fm_scale_MIN                                             0
#define REG_Ssd_fm_scale_MAX                                             3
#define REG_SSD_FM_SCALE_VALUE_27_KHZ_NOMINAL_FM_DEVIATION_EUROPE        0
#define REG_SSD_FM_SCALE_VALUE_15_KHZ_NOMINAL_FM_DEVIATION_M_STANDARDS   1
#define REG_SSD_FM_SCALE_VALUE_0_DB_GAIN_BTSC_EIAJ_FM_RADIO              2

/*!
 * @brief expert mode: internal scaling coefficient for all analog demodulator signals. 1024 means 0 dB.
 * @range -2048..2047
 ***/
#define REG_Ssd_anlg_scale
#define REG_Ssd_anlg_scale_OFFSET                                          0x00070
#define REG_Ssd_anlg_scale_MASK                                          0xfff
#define REG_Ssd_anlg_scale_SIGNED_BIT_MASK                               0x800000
#define REG_Ssd_anlg_scale_BIT_MASK                                      0xfff000
#define REG_Ssd_anlg_scale_BIT_IDX                                       12
#define REG_Ssd_anlg_scale_NUM_OF_BITS                                   12
#define REG_Ssd_anlg_scale_MIN                                           -2048
#define REG_Ssd_anlg_scale_MAX                                           2047

/*!
 * @brief source for SRC1
 *        0=SRC1 disabled
 *        1=DEC
 *        2=MONO/SAP
 *        3=Digital audio input
 * @range 0..15
 ***/
#define REG_Ssd_src1_source
#define REG_Ssd_src1_source_OFFSET                                         0x00074
#define REG_Ssd_src1_source_MASK                                         0xf
#define REG_Ssd_src1_source_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_src1_source_BIT_MASK                                     0xf
#define REG_Ssd_src1_source_BIT_IDX                                      0
#define REG_Ssd_src1_source_NUM_OF_BITS                                  4
#define REG_Ssd_src1_source_MIN                                          0
#define REG_Ssd_src1_source_MAX                                          15
#define REG_SSD_SRC1_SOURCE_VALUE_SRC1_DISABLED                          0
#define REG_SSD_SRC1_SOURCE_VALUE_DEC                                    1
#define REG_SSD_SRC1_SOURCE_VALUE_MONO_SAP                               2
#define REG_SSD_SRC1_SOURCE_VALUE_DIGITAL_AUDIO_INPUT                    3

/*!
 * @brief source for SRC2
 *        0=SRC2 disabled
 *        1=DEC
 *        2=MONO/SAP
 *        3=Digital audio input
 * @range 0..15
 ***/
#define REG_Ssd_src2_source
#define REG_Ssd_src2_source_OFFSET                                         0x00074
#define REG_Ssd_src2_source_MASK                                         0xf
#define REG_Ssd_src2_source_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_src2_source_BIT_MASK                                     0xf0
#define REG_Ssd_src2_source_BIT_IDX                                      4
#define REG_Ssd_src2_source_NUM_OF_BITS                                  4
#define REG_Ssd_src2_source_MIN                                          0
#define REG_Ssd_src2_source_MAX                                          15
#define REG_SSD_SRC2_SOURCE_VALUE_SRC2_DISABLED                          0
#define REG_SSD_SRC2_SOURCE_VALUE_DEC                                    1
#define REG_SSD_SRC2_SOURCE_VALUE_MONO_SAP                               2
#define REG_SSD_SRC2_SOURCE_VALUE_DIGITAL_AUDIO_INPUT                    3

/*!
 * @brief SRC scheduling priority (polling order)
 *        0=out,SRC1,SRC2
 *        1=out,SRC2,SRC1
 *        2=SRC1,out,SRC2
 *        3=SRC1,SRC2,out
 *        4=SRC2,out,SRC1
 *        5=SRC2,SRC1,out
 *        6=test mode 1
 *        7=test mode 2
 * @range 0..7
 ***/
#define REG_Ssd_src_sched_prio
#define REG_Ssd_src_sched_prio_OFFSET                                      0x00074
#define REG_Ssd_src_sched_prio_MASK                                      0x7
#define REG_Ssd_src_sched_prio_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_src_sched_prio_BIT_MASK                                  0x700
#define REG_Ssd_src_sched_prio_BIT_IDX                                   8
#define REG_Ssd_src_sched_prio_NUM_OF_BITS                               3
#define REG_Ssd_src_sched_prio_MIN                                       0
#define REG_Ssd_src_sched_prio_MAX                                       7
#define REG_SSD_SRC_SCHED_PRIO_VALUE_OUT_SRC1_SRC2                       0
#define REG_SSD_SRC_SCHED_PRIO_VALUE_OUT_SRC2_SRC1                       1
#define REG_SSD_SRC_SCHED_PRIO_VALUE_SRC1_OUT_SRC2                       2
#define REG_SSD_SRC_SCHED_PRIO_VALUE_SRC1_SRC2_OUT                       3
#define REG_SSD_SRC_SCHED_PRIO_VALUE_SRC2_OUT_SRC1                       4
#define REG_SSD_SRC_SCHED_PRIO_VALUE_SRC2_SRC1_OUT                       5
#define REG_SSD_SRC_SCHED_PRIO_VALUE_TEST_MODE_1                         6
#define REG_SSD_SRC_SCHED_PRIO_VALUE_TEST_MODE_2                         7

/*!
 * @brief level adjust DemDec DEC output ( +15..-15dB, -16 = mute)
 * @range -16..15
 ***/
#define REG_Ssd_declev
#define REG_Ssd_declev_OFFSET                                              0x00078
#define REG_Ssd_declev_MASK                                              0x1f
#define REG_Ssd_declev_SIGNED_BIT_MASK                                   0x10
#define REG_Ssd_declev_BIT_MASK                                          0x1f
#define REG_Ssd_declev_BIT_IDX                                           0
#define REG_Ssd_declev_NUM_OF_BITS                                       5
#define REG_Ssd_declev_MIN                                               -16
#define REG_Ssd_declev_MAX                                               15

/*!
 * @brief level adjust DemDec MONO output ( +15..-15dB, -16 = mute)
 * @range -16..15
 ***/
#define REG_Ssd_monolev
#define REG_Ssd_monolev_OFFSET                                             0x00078
#define REG_Ssd_monolev_MASK                                             0x1f
#define REG_Ssd_monolev_SIGNED_BIT_MASK                                  0x200
#define REG_Ssd_monolev_BIT_MASK                                         0x3e0
#define REG_Ssd_monolev_BIT_IDX                                          5
#define REG_Ssd_monolev_NUM_OF_BITS                                      5
#define REG_Ssd_monolev_MIN                                              -16
#define REG_Ssd_monolev_MAX                                              15

/*!
 * @brief additional level adjust for NICAM  ( +15..-15dB)"
 * @range -15..15
 ***/
#define REG_Ssd_niclev
#define REG_Ssd_niclev_OFFSET                                              0x00078
#define REG_Ssd_niclev_MASK                                              0x1f
#define REG_Ssd_niclev_SIGNED_BIT_MASK                                   0x4000
#define REG_Ssd_niclev_BIT_MASK                                          0x7c00
#define REG_Ssd_niclev_BIT_IDX                                           10
#define REG_Ssd_niclev_NUM_OF_BITS                                       5
#define REG_Ssd_niclev_MIN                                               -15
#define REG_Ssd_niclev_MAX                                               15

/*!
 * @brief level adjust DemDec SAP output ( +15..-15dB, -16 = mute)
 * @range -16..15
 ***/
#define REG_Ssd_saplev
#define REG_Ssd_saplev_OFFSET                                              0x00078
#define REG_Ssd_saplev_MASK                                              0x1f
#define REG_Ssd_saplev_SIGNED_BIT_MASK                                   0x80000
#define REG_Ssd_saplev_BIT_MASK                                          0xf8000
#define REG_Ssd_saplev_BIT_IDX                                           15
#define REG_Ssd_saplev_NUM_OF_BITS                                       5
#define REG_Ssd_saplev_MIN                                               -16
#define REG_Ssd_saplev_MAX                                               15

/*!
 * @brief level adjust ADC input ( +15..-15dB, -16 = mute)
 * @range -16..15
 ***/
#define REG_Ssd_adclev
#define REG_Ssd_adclev_OFFSET                                              0x0007C
#define REG_Ssd_adclev_MASK                                              0x1f
#define REG_Ssd_adclev_SIGNED_BIT_MASK                                   0x10
#define REG_Ssd_adclev_BIT_MASK                                          0x1f
#define REG_Ssd_adclev_BIT_IDX                                           0
#define REG_Ssd_adclev_NUM_OF_BITS                                       5
#define REG_Ssd_adclev_MIN                                               -16
#define REG_Ssd_adclev_MAX                                               15

/*!
 * @brief level adjust Digital Audio input ( +15..-15dB, -16 = mute)
 * @range -16..15
 ***/
#define REG_Ssd_diginlev
#define REG_Ssd_diginlev_OFFSET                                            0x0007C
#define REG_Ssd_diginlev_MASK                                            0x1f
#define REG_Ssd_diginlev_SIGNED_BIT_MASK                                 0x4000
#define REG_Ssd_diginlev_BIT_MASK                                        0x7c00
#define REG_Ssd_diginlev_BIT_IDX                                         10
#define REG_Ssd_diginlev_NUM_OF_BITS                                     5
#define REG_Ssd_diginlev_MIN                                             -16
#define REG_Ssd_diginlev_MAX                                             15

/*!
 * @brief SIGNAL SOURCE MAIN
 *        0=ADC
 *        1=SRC1
 *        2=SRC2
 *        3=Noise/Silence
 *        4=Beeper"
 * @range 0..31
 ***/
#define REG_Ssd_mainss
#define REG_Ssd_mainss_OFFSET                                              0x00080
#define REG_Ssd_mainss_MASK                                              0x1f
#define REG_Ssd_mainss_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_mainss_BIT_MASK                                          0x1f
#define REG_Ssd_mainss_BIT_IDX                                           0
#define REG_Ssd_mainss_NUM_OF_BITS                                       5
#define REG_Ssd_mainss_MIN                                               0
#define REG_Ssd_mainss_MAX                                               31
#define REG_SSD_MAINSS_VALUE_ADC                                         0
#define REG_SSD_MAINSS_VALUE_SRC1                                        1
#define REG_SSD_MAINSS_VALUE_SRC2                                        2
#define REG_SSD_MAINSS_VALUE_NOISE_SILENCE                               3
#define REG_SSD_MAINSS_VALUE_BEEPER                                      4

/*!
 * @brief DIGITAL MATRIX MAIN
 *        0 = AB [Stereo] (automatrix off)
 *        1 = (A+B)/2 [Mono] (automatrix off)
 *        2 = AA [Lang. A] (automatrix off)
 *        3 = BB [Lang. B] (automatrix off)
 *        4 = BA [Swap] (automatrix off)
 *        5 = unused
 *        6 = Language A (automatrix on)
 *        7 = Language B (automatrix on)
 * @range 0..7
 ***/
#define REG_Ssd_maindm
#define REG_Ssd_maindm_OFFSET                                              0x00080
#define REG_Ssd_maindm_MASK                                              0x7
#define REG_Ssd_maindm_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_maindm_BIT_MASK                                          0xe0
#define REG_Ssd_maindm_BIT_IDX                                           5
#define REG_Ssd_maindm_NUM_OF_BITS                                       3
#define REG_Ssd_maindm_MIN                                               0
#define REG_Ssd_maindm_MAX                                               7
#define REG_SSD_MAINDM_VALUE_AB_STEREO_AUTOMATRIX_OFF                    0
#define REG_SSD_MAINDM_VALUE_A_B_2_MONO_AUTOMATRIX_OFF                   1
#define REG_SSD_MAINDM_VALUE_AA_LANG_A_AUTOMATRIX_OFF                    2
#define REG_SSD_MAINDM_VALUE_BB_LANG_B_AUTOMATRIX_OFF                    3
#define REG_SSD_MAINDM_VALUE_BA_SWAP_AUTOMATRIX_OFF                      4
#define REG_SSD_MAINDM_VALUE_UNUSED                                      5
#define REG_SSD_MAINDM_VALUE_LANGUAGE_A_AUTOMATRIX_ON                    6
#define REG_SSD_MAINDM_VALUE_LANGUAGE_B_AUTOMATRIX_ON                    7

/*!
 * @brief SIGNAL SOURCE CENTER
 *        0=ADC Left
 *        1=ADC Right
 *        2=SRC1 Left
 *        3=SRC1 Right
 *        4=SRC2 Left
 *        5=SRC2 Right
 *        6=Noise
 *        7=Silence
 *        8=Beeper"
 * @range 0..31
 ***/
#define REG_Ssd_centerss
#define REG_Ssd_centerss_OFFSET                                            0x00080
#define REG_Ssd_centerss_MASK                                            0x1f
#define REG_Ssd_centerss_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_centerss_BIT_MASK                                        0x1f00
#define REG_Ssd_centerss_BIT_IDX                                         8
#define REG_Ssd_centerss_NUM_OF_BITS                                     5
#define REG_Ssd_centerss_MIN                                             0
#define REG_Ssd_centerss_MAX                                             31
#define REG_SSD_CENTERSS_VALUE_ADC_LEFT                                  0
#define REG_SSD_CENTERSS_VALUE_ADC_RIGHT                                 1
#define REG_SSD_CENTERSS_VALUE_SRC1_LEFT                                 2
#define REG_SSD_CENTERSS_VALUE_SRC1_RIGHT                                3
#define REG_SSD_CENTERSS_VALUE_SRC2_LEFT                                 4
#define REG_SSD_CENTERSS_VALUE_SRC2_RIGHT                                5
#define REG_SSD_CENTERSS_VALUE_NOISE                                     6
#define REG_SSD_CENTERSS_VALUE_SILENCE                                   7
#define REG_SSD_CENTERSS_VALUE_BEEPER                                    8

/*!
 * @brief SIGNAL SOURCE SURROUND
 *        0=ADC Left
 *        1=ADC Right
 *        2=SRC1 Left
 *        3=SRC1 Right
 *        4=SRC2 Left
 *        5=SRC2 Right
 *        6=Noise
 *        7=Silence
 *        8=Beeper"
 * @range 0..31
 ***/
#define REG_Ssd_surroundss
#define REG_Ssd_surroundss_OFFSET                                          0x00080
#define REG_Ssd_surroundss_MASK                                          0x1f
#define REG_Ssd_surroundss_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_surroundss_BIT_MASK                                      0x3e000
#define REG_Ssd_surroundss_BIT_IDX                                       13
#define REG_Ssd_surroundss_NUM_OF_BITS                                   5
#define REG_Ssd_surroundss_MIN                                           0
#define REG_Ssd_surroundss_MAX                                           31
#define REG_SSD_SURROUNDSS_VALUE_ADC_LEFT                                0
#define REG_SSD_SURROUNDSS_VALUE_ADC_RIGHT                               1
#define REG_SSD_SURROUNDSS_VALUE_SRC1_LEFT                               2
#define REG_SSD_SURROUNDSS_VALUE_SRC1_RIGHT                              3
#define REG_SSD_SURROUNDSS_VALUE_SRC2_LEFT                               4
#define REG_SSD_SURROUNDSS_VALUE_SRC2_RIGHT                              5
#define REG_SSD_SURROUNDSS_VALUE_NOISE                                   6
#define REG_SSD_SURROUNDSS_VALUE_SILENCE                                 7
#define REG_SSD_SURROUNDSS_VALUE_BEEPER                                  8

/*!
 * @brief SIGNAL SOURCE AUX1
 *        0=ADC
 *        1=SRC1
 *        2=SRC2
 *        3=Noise/Silence
 *        4=Beeper"
 * @range 0..31
 ***/
#define REG_Ssd_aux1ss
#define REG_Ssd_aux1ss_OFFSET                                              0x00084
#define REG_Ssd_aux1ss_MASK                                              0x1f
#define REG_Ssd_aux1ss_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_aux1ss_BIT_MASK                                          0x1f
#define REG_Ssd_aux1ss_BIT_IDX                                           0
#define REG_Ssd_aux1ss_NUM_OF_BITS                                       5
#define REG_Ssd_aux1ss_MIN                                               0
#define REG_Ssd_aux1ss_MAX                                               31
#define REG_SSD_AUX1SS_VALUE_ADC                                         0
#define REG_SSD_AUX1SS_VALUE_SRC1                                        1
#define REG_SSD_AUX1SS_VALUE_SRC2                                        2
#define REG_SSD_AUX1SS_VALUE_NOISE_SILENCE                               3
#define REG_SSD_AUX1SS_VALUE_BEEPER                                      4

/*!
 * @brief DIGITAL MATRIX AUX1
 *        0 = AB [Stereo] (automatrix off)
 *        1 = (A+B)/2 [Mono] (automatrix off)
 *        2 = AA [Lang. A] (automatrix off)
 *        3 = BB [Lang. B] (automatrix off)
 *        4 = BA [Swap] (automatrix off)
 *        5 = unused
 *        6 = Language A (automatrix on)
 *        7 = Language B (automatrix on)
 * @range 0..7
 ***/
#define REG_Ssd_aux1dm
#define REG_Ssd_aux1dm_OFFSET                                              0x00084
#define REG_Ssd_aux1dm_MASK                                              0x7
#define REG_Ssd_aux1dm_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_aux1dm_BIT_MASK                                          0xe0
#define REG_Ssd_aux1dm_BIT_IDX                                           5
#define REG_Ssd_aux1dm_NUM_OF_BITS                                       3
#define REG_Ssd_aux1dm_MIN                                               0
#define REG_Ssd_aux1dm_MAX                                               7
#define REG_SSD_AUX1DM_VALUE_AB_STEREO_AUTOMATRIX_OFF                    0
#define REG_SSD_AUX1DM_VALUE_A_B_2_MONO_AUTOMATRIX_OFF                   1
#define REG_SSD_AUX1DM_VALUE_AA_LANG_A_AUTOMATRIX_OFF                    2
#define REG_SSD_AUX1DM_VALUE_BB_LANG_B_AUTOMATRIX_OFF                    3
#define REG_SSD_AUX1DM_VALUE_BA_SWAP_AUTOMATRIX_OFF                      4
#define REG_SSD_AUX1DM_VALUE_UNUSED                                      5
#define REG_SSD_AUX1DM_VALUE_LANGUAGE_A_AUTOMATRIX_ON                    6
#define REG_SSD_AUX1DM_VALUE_LANGUAGE_B_AUTOMATRIX_ON                    7

/*!
 * @brief SIGNAL SOURCE AUX2
 *        0=ADC
 *        1=SRC1
 *        2=SRC2
 *        3=Noise/Silence
 *        4=Beeper"
 * @range 0..31
 ***/
#define REG_Ssd_aux2ss
#define REG_Ssd_aux2ss_OFFSET                                              0x00084
#define REG_Ssd_aux2ss_MASK                                              0x1f
#define REG_Ssd_aux2ss_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_aux2ss_BIT_MASK                                          0x1f00
#define REG_Ssd_aux2ss_BIT_IDX                                           8
#define REG_Ssd_aux2ss_NUM_OF_BITS                                       5
#define REG_Ssd_aux2ss_MIN                                               0
#define REG_Ssd_aux2ss_MAX                                               31
#define REG_SSD_AUX2SS_VALUE_ADC                                         0
#define REG_SSD_AUX2SS_VALUE_SRC1                                        1
#define REG_SSD_AUX2SS_VALUE_SRC2                                        2
#define REG_SSD_AUX2SS_VALUE_NOISE_SILENCE                               3
#define REG_SSD_AUX2SS_VALUE_BEEPER                                      4

/*!
 * @brief DIGITAL MATRIX AUX2
 *        0 = AB [Stereo] (automatrix off)
 *        1 = (A+B)/2 [Mono] (automatrix off)
 *        2 = AA [Lang. A] (automatrix off)
 *        3 = BB [Lang. B] (automatrix off)
 *        4 = BA [Swap] (automatrix off)
 *        5 = unused
 *        6 = Language A (automatrix on)
 *        7 = Language B (automatrix on)
 * @range 0..7
 ***/
#define REG_Ssd_aux2dm
#define REG_Ssd_aux2dm_OFFSET                                              0x00084
#define REG_Ssd_aux2dm_MASK                                              0x7
#define REG_Ssd_aux2dm_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_aux2dm_BIT_MASK                                          0xe000
#define REG_Ssd_aux2dm_BIT_IDX                                           13
#define REG_Ssd_aux2dm_NUM_OF_BITS                                       3
#define REG_Ssd_aux2dm_MIN                                               0
#define REG_Ssd_aux2dm_MAX                                               7
#define REG_SSD_AUX2DM_VALUE_AB_STEREO_AUTOMATRIX_OFF                    0
#define REG_SSD_AUX2DM_VALUE_A_B_2_MONO_AUTOMATRIX_OFF                   1
#define REG_SSD_AUX2DM_VALUE_AA_LANG_A_AUTOMATRIX_OFF                    2
#define REG_SSD_AUX2DM_VALUE_BB_LANG_B_AUTOMATRIX_OFF                    3
#define REG_SSD_AUX2DM_VALUE_BA_SWAP_AUTOMATRIX_OFF                      4
#define REG_SSD_AUX2DM_VALUE_UNUSED                                      5
#define REG_SSD_AUX2DM_VALUE_LANGUAGE_A_AUTOMATRIX_ON                    6
#define REG_SSD_AUX2DM_VALUE_LANGUAGE_B_AUTOMATRIX_ON                    7

/*!
 * @brief SIGNAL SOURCE AUX3
 *        0=ADC
 *        1=SRC1
 *        2=SRC2
 *        3=Noise/Silence
 *        4=Beeper"
 * @range 0..31
 ***/
#define REG_Ssd_aux3ss
#define REG_Ssd_aux3ss_OFFSET                                              0x00084
#define REG_Ssd_aux3ss_MASK                                              0x1f
#define REG_Ssd_aux3ss_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_aux3ss_BIT_MASK                                          0x1f0000
#define REG_Ssd_aux3ss_BIT_IDX                                           16
#define REG_Ssd_aux3ss_NUM_OF_BITS                                       5
#define REG_Ssd_aux3ss_MIN                                               0
#define REG_Ssd_aux3ss_MAX                                               31
#define REG_SSD_AUX3SS_VALUE_ADC                                         0
#define REG_SSD_AUX3SS_VALUE_SRC1                                        1
#define REG_SSD_AUX3SS_VALUE_SRC2                                        2
#define REG_SSD_AUX3SS_VALUE_NOISE_SILENCE                               3
#define REG_SSD_AUX3SS_VALUE_BEEPER                                      4

/*!
 * @brief DIGITAL MATRIX AUX3
 *        0 = AB [Stereo] (automatrix off)
 *        1 = (A+B)/2 [Mono] (automatrix off)
 *        2 = AA [Lang. A] (automatrix off)
 *        3 = BB [Lang. B] (automatrix off)
 *        4 = BA [Swap] (automatrix off)
 *        5 = unused
 *        6 = Language A (automatrix on)
 *        7 = Language B (automatrix on)
 * @range 0..7
 ***/
#define REG_Ssd_aux3dm
#define REG_Ssd_aux3dm_OFFSET                                              0x00084
#define REG_Ssd_aux3dm_MASK                                              0x7
#define REG_Ssd_aux3dm_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_aux3dm_BIT_MASK                                          0xe00000
#define REG_Ssd_aux3dm_BIT_IDX                                           21
#define REG_Ssd_aux3dm_NUM_OF_BITS                                       3
#define REG_Ssd_aux3dm_MIN                                               0
#define REG_Ssd_aux3dm_MAX                                               7
#define REG_SSD_AUX3DM_VALUE_AB_STEREO_AUTOMATRIX_OFF                    0
#define REG_SSD_AUX3DM_VALUE_A_B_2_MONO_AUTOMATRIX_OFF                   1
#define REG_SSD_AUX3DM_VALUE_AA_LANG_A_AUTOMATRIX_OFF                    2
#define REG_SSD_AUX3DM_VALUE_BB_LANG_B_AUTOMATRIX_OFF                    3
#define REG_SSD_AUX3DM_VALUE_BA_SWAP_AUTOMATRIX_OFF                      4
#define REG_SSD_AUX3DM_VALUE_UNUSED                                      5
#define REG_SSD_AUX3DM_VALUE_LANGUAGE_A_AUTOMATRIX_ON                    6
#define REG_SSD_AUX3DM_VALUE_LANGUAGE_B_AUTOMATRIX_ON                    7

/*!
 * @brief OUTPUT SELECTION DAC2L
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_asdac1l
#define REG_Ssd_asdac1l_OFFSET                                             0x00088
#define REG_Ssd_asdac1l_MASK                                             0xf
#define REG_Ssd_asdac1l_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_asdac1l_BIT_MASK                                         0xf
#define REG_Ssd_asdac1l_BIT_IDX                                          0
#define REG_Ssd_asdac1l_NUM_OF_BITS                                      4
#define REG_Ssd_asdac1l_MIN                                              0
#define REG_Ssd_asdac1l_MAX                                              15
#define REG_SSD_ASDAC1L_VALUE_MAIN_L                                     0
#define REG_SSD_ASDAC1L_VALUE_MAIN_R                                     1
#define REG_SSD_ASDAC1L_VALUE_SUBWOOFER                                  2
#define REG_SSD_ASDAC1L_VALUE_CENTER                                     3
#define REG_SSD_ASDAC1L_VALUE_SURROUND                                   4
#define REG_SSD_ASDAC1L_VALUE_AUX1_L                                     5
#define REG_SSD_ASDAC1L_VALUE_AUX1_R                                     6
#define REG_SSD_ASDAC1L_VALUE_AUX2_L                                     7
#define REG_SSD_ASDAC1L_VALUE_AUX2_R                                     8
#define REG_SSD_ASDAC1L_VALUE_AUX3_L                                     9
#define REG_SSD_ASDAC1L_VALUE_AUX3_R                                     10
#define REG_SSD_ASDAC1L_VALUE_MAIN_SUM                                   11
#define REG_SSD_ASDAC1L_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION DAC2R
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_asdac1r
#define REG_Ssd_asdac1r_OFFSET                                             0x00088
#define REG_Ssd_asdac1r_MASK                                             0xf
#define REG_Ssd_asdac1r_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_asdac1r_BIT_MASK                                         0xf0
#define REG_Ssd_asdac1r_BIT_IDX                                          4
#define REG_Ssd_asdac1r_NUM_OF_BITS                                      4
#define REG_Ssd_asdac1r_MIN                                              0
#define REG_Ssd_asdac1r_MAX                                              15
#define REG_SSD_ASDAC1R_VALUE_MAIN_L                                     0
#define REG_SSD_ASDAC1R_VALUE_MAIN_R                                     1
#define REG_SSD_ASDAC1R_VALUE_SUBWOOFER                                  2
#define REG_SSD_ASDAC1R_VALUE_CENTER                                     3
#define REG_SSD_ASDAC1R_VALUE_SURROUND                                   4
#define REG_SSD_ASDAC1R_VALUE_AUX1_L                                     5
#define REG_SSD_ASDAC1R_VALUE_AUX1_R                                     6
#define REG_SSD_ASDAC1R_VALUE_AUX2_L                                     7
#define REG_SSD_ASDAC1R_VALUE_AUX2_R                                     8
#define REG_SSD_ASDAC1R_VALUE_AUX3_L                                     9
#define REG_SSD_ASDAC1R_VALUE_AUX3_R                                     10
#define REG_SSD_ASDAC1R_VALUE_MAIN_SUM                                   11
#define REG_SSD_ASDAC1R_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION DAC3L
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_asdac2l
#define REG_Ssd_asdac2l_OFFSET                                             0x00088
#define REG_Ssd_asdac2l_MASK                                             0xf
#define REG_Ssd_asdac2l_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_asdac2l_BIT_MASK                                         0xf00
#define REG_Ssd_asdac2l_BIT_IDX                                          8
#define REG_Ssd_asdac2l_NUM_OF_BITS                                      4
#define REG_Ssd_asdac2l_MIN                                              0
#define REG_Ssd_asdac2l_MAX                                              15
#define REG_SSD_ASDAC2L_VALUE_MAIN_L                                     0
#define REG_SSD_ASDAC2L_VALUE_MAIN_R                                     1
#define REG_SSD_ASDAC2L_VALUE_SUBWOOFER                                  2
#define REG_SSD_ASDAC2L_VALUE_CENTER                                     3
#define REG_SSD_ASDAC2L_VALUE_SURROUND                                   4
#define REG_SSD_ASDAC2L_VALUE_AUX1_L                                     5
#define REG_SSD_ASDAC2L_VALUE_AUX1_R                                     6
#define REG_SSD_ASDAC2L_VALUE_AUX2_L                                     7
#define REG_SSD_ASDAC2L_VALUE_AUX2_R                                     8
#define REG_SSD_ASDAC2L_VALUE_AUX3_L                                     9
#define REG_SSD_ASDAC2L_VALUE_AUX3_R                                     10
#define REG_SSD_ASDAC2L_VALUE_MAIN_SUM                                   11
#define REG_SSD_ASDAC2L_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION DAC3R
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_asdac2r
#define REG_Ssd_asdac2r_OFFSET                                             0x00088
#define REG_Ssd_asdac2r_MASK                                             0xf
#define REG_Ssd_asdac2r_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_asdac2r_BIT_MASK                                         0xf000
#define REG_Ssd_asdac2r_BIT_IDX                                          12
#define REG_Ssd_asdac2r_NUM_OF_BITS                                      4
#define REG_Ssd_asdac2r_MIN                                              0
#define REG_Ssd_asdac2r_MAX                                              15
#define REG_SSD_ASDAC2R_VALUE_MAIN_L                                     0
#define REG_SSD_ASDAC2R_VALUE_MAIN_R                                     1
#define REG_SSD_ASDAC2R_VALUE_SUBWOOFER                                  2
#define REG_SSD_ASDAC2R_VALUE_CENTER                                     3
#define REG_SSD_ASDAC2R_VALUE_SURROUND                                   4
#define REG_SSD_ASDAC2R_VALUE_AUX1_L                                     5
#define REG_SSD_ASDAC2R_VALUE_AUX1_R                                     6
#define REG_SSD_ASDAC2R_VALUE_AUX2_L                                     7
#define REG_SSD_ASDAC2R_VALUE_AUX2_R                                     8
#define REG_SSD_ASDAC2R_VALUE_AUX3_L                                     9
#define REG_SSD_ASDAC2R_VALUE_AUX3_R                                     10
#define REG_SSD_ASDAC2R_VALUE_MAIN_SUM                                   11
#define REG_SSD_ASDAC2R_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION SPDIF LEFT
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_spdif1l
#define REG_Ssd_spdif1l_OFFSET                                             0x0008C
#define REG_Ssd_spdif1l_MASK                                             0xf
#define REG_Ssd_spdif1l_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_spdif1l_BIT_MASK                                         0xf
#define REG_Ssd_spdif1l_BIT_IDX                                          0
#define REG_Ssd_spdif1l_NUM_OF_BITS                                      4
#define REG_Ssd_spdif1l_MIN                                              0
#define REG_Ssd_spdif1l_MAX                                              15
#define REG_SSD_SPDIF1L_VALUE_MAIN_L                                     0
#define REG_SSD_SPDIF1L_VALUE_MAIN_R                                     1
#define REG_SSD_SPDIF1L_VALUE_SUBWOOFER                                  2
#define REG_SSD_SPDIF1L_VALUE_CENTER                                     3
#define REG_SSD_SPDIF1L_VALUE_SURROUND                                   4
#define REG_SSD_SPDIF1L_VALUE_AUX1_L                                     5
#define REG_SSD_SPDIF1L_VALUE_AUX1_R                                     6
#define REG_SSD_SPDIF1L_VALUE_AUX2_L                                     7
#define REG_SSD_SPDIF1L_VALUE_AUX2_R                                     8
#define REG_SSD_SPDIF1L_VALUE_AUX3_L                                     9
#define REG_SSD_SPDIF1L_VALUE_AUX3_R                                     10
#define REG_SSD_SPDIF1L_VALUE_MAIN_SUM                                   11
#define REG_SSD_SPDIF1L_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION SPDIF RIGHT
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_spdif1r
#define REG_Ssd_spdif1r_OFFSET                                             0x0008C
#define REG_Ssd_spdif1r_MASK                                             0xf
#define REG_Ssd_spdif1r_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_spdif1r_BIT_MASK                                         0xf0
#define REG_Ssd_spdif1r_BIT_IDX                                          4
#define REG_Ssd_spdif1r_NUM_OF_BITS                                      4
#define REG_Ssd_spdif1r_MIN                                              0
#define REG_Ssd_spdif1r_MAX                                              15
#define REG_SSD_SPDIF1R_VALUE_MAIN_L                                     0
#define REG_SSD_SPDIF1R_VALUE_MAIN_R                                     1
#define REG_SSD_SPDIF1R_VALUE_SUBWOOFER                                  2
#define REG_SSD_SPDIF1R_VALUE_CENTER                                     3
#define REG_SSD_SPDIF1R_VALUE_SURROUND                                   4
#define REG_SSD_SPDIF1R_VALUE_AUX1_L                                     5
#define REG_SSD_SPDIF1R_VALUE_AUX1_R                                     6
#define REG_SSD_SPDIF1R_VALUE_AUX2_L                                     7
#define REG_SSD_SPDIF1R_VALUE_AUX2_R                                     8
#define REG_SSD_SPDIF1R_VALUE_AUX3_L                                     9
#define REG_SSD_SPDIF1R_VALUE_AUX3_R                                     10
#define REG_SSD_SPDIF1R_VALUE_MAIN_SUM                                   11
#define REG_SSD_SPDIF1R_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION I2S1L
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_asi2s1l
#define REG_Ssd_asi2s1l_OFFSET                                             0x0008C
#define REG_Ssd_asi2s1l_MASK                                             0xf
#define REG_Ssd_asi2s1l_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_asi2s1l_BIT_MASK                                         0xf00
#define REG_Ssd_asi2s1l_BIT_IDX                                          8
#define REG_Ssd_asi2s1l_NUM_OF_BITS                                      4
#define REG_Ssd_asi2s1l_MIN                                              0
#define REG_Ssd_asi2s1l_MAX                                              15
#define REG_SSD_ASI2S1L_VALUE_MAIN_L                                     0
#define REG_SSD_ASI2S1L_VALUE_MAIN_R                                     1
#define REG_SSD_ASI2S1L_VALUE_SUBWOOFER                                  2
#define REG_SSD_ASI2S1L_VALUE_CENTER                                     3
#define REG_SSD_ASI2S1L_VALUE_SURROUND                                   4
#define REG_SSD_ASI2S1L_VALUE_AUX1_L                                     5
#define REG_SSD_ASI2S1L_VALUE_AUX1_R                                     6
#define REG_SSD_ASI2S1L_VALUE_AUX2_L                                     7
#define REG_SSD_ASI2S1L_VALUE_AUX2_R                                     8
#define REG_SSD_ASI2S1L_VALUE_AUX3_L                                     9
#define REG_SSD_ASI2S1L_VALUE_AUX3_R                                     10
#define REG_SSD_ASI2S1L_VALUE_MAIN_SUM                                   11
#define REG_SSD_ASI2S1L_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief OUTPUT SELECTION I2S1R
 *        $0 = MAIN/L
 *        $1 = MAIN/R
 *        $2 = SUBWOOFER
 *        $3 = CENTER
 *        $4 = SURROUND
 *        $5 = AUX1/L
 *        $6 = AUX1/R
 *        $7 = AUX2/L
 *        $8 = AUX2/R
 *        $9 = AUX3/L
 *        $A = AUX3/R
 *        $B = MAIN SUM
 *        $C = digital silence
 * @range 0..15
 ***/
#define REG_Ssd_asi2s1r
#define REG_Ssd_asi2s1r_OFFSET                                             0x0008C
#define REG_Ssd_asi2s1r_MASK                                             0xf
#define REG_Ssd_asi2s1r_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_asi2s1r_BIT_MASK                                         0xf000
#define REG_Ssd_asi2s1r_BIT_IDX                                          12
#define REG_Ssd_asi2s1r_NUM_OF_BITS                                      4
#define REG_Ssd_asi2s1r_MIN                                              0
#define REG_Ssd_asi2s1r_MAX                                              15
#define REG_SSD_ASI2S1R_VALUE_MAIN_L                                     0
#define REG_SSD_ASI2S1R_VALUE_MAIN_R                                     1
#define REG_SSD_ASI2S1R_VALUE_SUBWOOFER                                  2
#define REG_SSD_ASI2S1R_VALUE_CENTER                                     3
#define REG_SSD_ASI2S1R_VALUE_SURROUND                                   4
#define REG_SSD_ASI2S1R_VALUE_AUX1_L                                     5
#define REG_SSD_ASI2S1R_VALUE_AUX1_R                                     6
#define REG_SSD_ASI2S1R_VALUE_AUX2_L                                     7
#define REG_SSD_ASI2S1R_VALUE_AUX2_R                                     8
#define REG_SSD_ASI2S1R_VALUE_AUX3_L                                     9
#define REG_SSD_ASI2S1R_VALUE_AUX3_R                                     10
#define REG_SSD_ASI2S1R_VALUE_MAIN_SUM                                   11
#define REG_SSD_ASI2S1R_VALUE_DIGITAL_SILENCE                            12

/*!
 * @brief Softmute Main Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_mainmut
#define REG_Ssd_mainmut_OFFSET                                             0x00090
#define REG_Ssd_mainmut_MASK                                             0x1
#define REG_Ssd_mainmut_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_mainmut_BIT_MASK                                         0x1
#define REG_Ssd_mainmut_BIT_IDX                                          0
#define REG_Ssd_mainmut_NUM_OF_BITS                                      1
#define REG_Ssd_mainmut_MIN                                              0
#define REG_Ssd_mainmut_MAX                                              1
#define REG_SSD_MAINMUT_VALUE_OFF                                        0
#define REG_SSD_MAINMUT_VALUE_ON                                         1

/*!
 * @brief Softmute Main Left Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_mainlmut
#define REG_Ssd_mainlmut_OFFSET                                            0x00090
#define REG_Ssd_mainlmut_MASK                                            0x1
#define REG_Ssd_mainlmut_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mainlmut_BIT_MASK                                        0x2
#define REG_Ssd_mainlmut_BIT_IDX                                         1
#define REG_Ssd_mainlmut_NUM_OF_BITS                                     1
#define REG_Ssd_mainlmut_MIN                                             0
#define REG_Ssd_mainlmut_MAX                                             1
#define REG_SSD_MAINLMUT_VALUE_OFF                                       0
#define REG_SSD_MAINLMUT_VALUE_ON                                        1

/*!
 * @brief Softmute Main Right Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_mainrmut
#define REG_Ssd_mainrmut_OFFSET                                            0x00090
#define REG_Ssd_mainrmut_MASK                                            0x1
#define REG_Ssd_mainrmut_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mainrmut_BIT_MASK                                        0x4
#define REG_Ssd_mainrmut_BIT_IDX                                         2
#define REG_Ssd_mainrmut_NUM_OF_BITS                                     1
#define REG_Ssd_mainrmut_MIN                                             0
#define REG_Ssd_mainrmut_MAX                                             1
#define REG_SSD_MAINRMUT_VALUE_OFF                                       0
#define REG_SSD_MAINRMUT_VALUE_ON                                        1

/*!
 * @brief Softmute Subwoofer Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_subwmut
#define REG_Ssd_subwmut_OFFSET                                             0x00090
#define REG_Ssd_subwmut_MASK                                             0x1
#define REG_Ssd_subwmut_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_subwmut_BIT_MASK                                         0x8
#define REG_Ssd_subwmut_BIT_IDX                                          3
#define REG_Ssd_subwmut_NUM_OF_BITS                                      1
#define REG_Ssd_subwmut_MIN                                              0
#define REG_Ssd_subwmut_MAX                                              1
#define REG_SSD_SUBWMUT_VALUE_OFF                                        0
#define REG_SSD_SUBWMUT_VALUE_ON                                         1

/*!
 * @brief Softmute Center Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_centermut
#define REG_Ssd_centermut_OFFSET                                           0x00090
#define REG_Ssd_centermut_MASK                                           0x1
#define REG_Ssd_centermut_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_centermut_BIT_MASK                                       0x10
#define REG_Ssd_centermut_BIT_IDX                                        4
#define REG_Ssd_centermut_NUM_OF_BITS                                    1
#define REG_Ssd_centermut_MIN                                            0
#define REG_Ssd_centermut_MAX                                            1
#define REG_SSD_CENTERMUT_VALUE_OFF                                      0
#define REG_SSD_CENTERMUT_VALUE_ON                                       1

/*!
 * @brief Softmute Surround Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_surroundmut
#define REG_Ssd_surroundmut_OFFSET                                         0x00090
#define REG_Ssd_surroundmut_MASK                                         0x1
#define REG_Ssd_surroundmut_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_surroundmut_BIT_MASK                                     0x20
#define REG_Ssd_surroundmut_BIT_IDX                                      5
#define REG_Ssd_surroundmut_NUM_OF_BITS                                  1
#define REG_Ssd_surroundmut_MIN                                          0
#define REG_Ssd_surroundmut_MAX                                          1
#define REG_SSD_SURROUNDMUT_VALUE_OFF                                    0
#define REG_SSD_SURROUNDMUT_VALUE_ON                                     1

/*!
 * @brief Softmute Aux1 Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_aux1mut
#define REG_Ssd_aux1mut_OFFSET                                             0x00090
#define REG_Ssd_aux1mut_MASK                                             0x1
#define REG_Ssd_aux1mut_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_aux1mut_BIT_MASK                                         0x40
#define REG_Ssd_aux1mut_BIT_IDX                                          6
#define REG_Ssd_aux1mut_NUM_OF_BITS                                      1
#define REG_Ssd_aux1mut_MIN                                              0
#define REG_Ssd_aux1mut_MAX                                              1
#define REG_SSD_AUX1MUT_VALUE_OFF                                        0
#define REG_SSD_AUX1MUT_VALUE_ON                                         1

/*!
 * @brief Softmute Aux2 Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_aux2mut
#define REG_Ssd_aux2mut_OFFSET                                             0x00090
#define REG_Ssd_aux2mut_MASK                                             0x1
#define REG_Ssd_aux2mut_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_aux2mut_BIT_MASK                                         0x80
#define REG_Ssd_aux2mut_BIT_IDX                                          7
#define REG_Ssd_aux2mut_NUM_OF_BITS                                      1
#define REG_Ssd_aux2mut_MIN                                              0
#define REG_Ssd_aux2mut_MAX                                              1
#define REG_SSD_AUX2MUT_VALUE_OFF                                        0
#define REG_SSD_AUX2MUT_VALUE_ON                                         1

/*!
 * @brief Softmute Aux3 Output
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_aux3mut
#define REG_Ssd_aux3mut_OFFSET                                             0x00090
#define REG_Ssd_aux3mut_MASK                                             0x1
#define REG_Ssd_aux3mut_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_aux3mut_BIT_MASK                                         0x100
#define REG_Ssd_aux3mut_BIT_IDX                                          8
#define REG_Ssd_aux3mut_NUM_OF_BITS                                      1
#define REG_Ssd_aux3mut_MIN                                              0
#define REG_Ssd_aux3mut_MAX                                              1
#define REG_SSD_AUX3MUT_VALUE_OFF                                        0
#define REG_SSD_AUX3MUT_VALUE_ON                                         1

/*!
 * @brief Softmute Speed Control
 *        0 = 76msec
 *        1 = 38msec
 *        2 = 19msec
 *        3 = 9.5msec
 * @range 0..3
 ***/
#define REG_Ssd_mutespeedctrl
#define REG_Ssd_mutespeedctrl_OFFSET                                       0x00090
#define REG_Ssd_mutespeedctrl_MASK                                       0x3
#define REG_Ssd_mutespeedctrl_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_mutespeedctrl_BIT_MASK                                   0x3000
#define REG_Ssd_mutespeedctrl_BIT_IDX                                    12
#define REG_Ssd_mutespeedctrl_NUM_OF_BITS                                2
#define REG_Ssd_mutespeedctrl_MIN                                        0
#define REG_Ssd_mutespeedctrl_MAX                                        3
#define REG_SSD_MUTESPEEDCTRL_VALUE_76MSEC                               0
#define REG_SSD_MUTESPEEDCTRL_VALUE_38MSEC                               1
#define REG_SSD_MUTESPEEDCTRL_VALUE_19MSEC                               2
#define REG_SSD_MUTESPEEDCTRL_VALUE_9_5MSEC                              3

/*!
 * @brief Execute 'Mode Table'                 After a transition from '0' to '1' the selected 'Mode Table' is executed once. Afterwards it should be cleared again. (If ControlMode='0') ")
 *        0 = clear
 *        1 = execute
 * @range 0..1
 ***/
#define REG_Ssd_exemodtab
#define REG_Ssd_exemodtab_OFFSET                                           0x00094
#define REG_Ssd_exemodtab_MASK                                           0x1
#define REG_Ssd_exemodtab_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_exemodtab_BIT_MASK                                       0x1
#define REG_Ssd_exemodtab_BIT_IDX                                        0
#define REG_Ssd_exemodtab_NUM_OF_BITS                                    1
#define REG_Ssd_exemodtab_MIN                                            0
#define REG_Ssd_exemodtab_MAX                                            1
#define REG_SSD_EXEMODTAB_VALUE_CLEAR                                    0
#define REG_SSD_EXEMODTAB_VALUE_EXECUTE                                  1
#if 0
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
#define REG_Ssd_sndmod_OFFSET                                              0x00094
#define REG_Ssd_sndmod_MASK                                              0x1f
#define REG_Ssd_sndmod_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_sndmod_BIT_MASK                                          0x3e
#define REG_Ssd_sndmod_BIT_IDX                                           1
#define REG_Ssd_sndmod_NUM_OF_BITS                                       5
#define REG_Ssd_sndmod_MIN                                               0
#define REG_Ssd_sndmod_MAX                                               14
#define REG_SSD_SNDMOD_VALUE_MONO_STEREO_DEFAULT                         0
#define REG_SSD_SNDMOD_VALUE_VDSII422                                    6
#define REG_SSD_SNDMOD_VALUE_VDSII423                                    7
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_DPL                         8
#define REG_SSD_SNDMOD_VALUE_NOISE_SEQUENCING                            9
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_PASSIVE_MATRIX              10
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_XT_PASSIVE_MATRIX           11
#define REG_SSD_SNDMOD_VALUE_SRS_TRUSURROUND_XT_DPL                      12
#define REG_SSD_SNDMOD_VALUE_NVS522                                      13
#define REG_SSD_SNDMOD_VALUE_NVS523                                      14
#endif
/*!
 * @brief Clip Management
 *        0 = Clip management OFF (default)
 *        1 = Static Volume Mode
 *        2 = Static Control Mode
 *        3 = Dynamic Control Mode
 *        4 = Dynamic Volume Mode
 * @range 0..7
 ***/
#define REG_Ssd_clipmanage
#define REG_Ssd_clipmanage_OFFSET                                          0x00094
#define REG_Ssd_clipmanage_MASK                                          0x7
#define REG_Ssd_clipmanage_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_clipmanage_BIT_MASK                                      0x1c0
#define REG_Ssd_clipmanage_BIT_IDX                                       6
#define REG_Ssd_clipmanage_NUM_OF_BITS                                   3
#define REG_Ssd_clipmanage_MIN                                           0
#define REG_Ssd_clipmanage_MAX                                           7
#define REG_SSD_CLIPMANAGE_VALUE_CLIP_MANAGEMENT_OFF_DEFAULT             0
#define REG_SSD_CLIPMANAGE_VALUE_STATIC_VOLUME_MODE                      1
#define REG_SSD_CLIPMANAGE_VALUE_STATIC_CONTROL_MODE                     2
#define REG_SSD_CLIPMANAGE_VALUE_DYNAMIC_CONTROL_MODE                    3
#define REG_SSD_CLIPMANAGE_VALUE_DYNAMIC_VOLUME_MODE                     4

/*!
 * @brief Main/Subwoofer signal output control
 *        0 = Normal Subwoofer Mode
 *        1 = Economic Subwoofer Mode
 * @range 0..1
 ***/
#define REG_Ssd_mainsubctrl
#define REG_Ssd_mainsubctrl_OFFSET                                         0x00094
#define REG_Ssd_mainsubctrl_MASK                                         0x1
#define REG_Ssd_mainsubctrl_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_mainsubctrl_BIT_MASK                                     0x200
#define REG_Ssd_mainsubctrl_BIT_IDX                                      9
#define REG_Ssd_mainsubctrl_NUM_OF_BITS                                  1
#define REG_Ssd_mainsubctrl_MIN                                          0
#define REG_Ssd_mainsubctrl_MAX                                          1
#define REG_SSD_MAINSUBCTRL_VALUE_NORMAL_SUBWOOFER_MODE                  0
#define REG_SSD_MAINSUBCTRL_VALUE_ECONOMIC_SUBWOOFER_MODE                1

/*!
 * @brief EQ enable for Main and Center channel
 *        0 = EQ bypass off
 *        1 = EQ bypass on
 * @range 0..1
 ***/
#define REG_Ssd_eqbypass
#define REG_Ssd_eqbypass_OFFSET                                            0x00094
#define REG_Ssd_eqbypass_MASK                                            0x1
#define REG_Ssd_eqbypass_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_eqbypass_BIT_MASK                                        0x400
#define REG_Ssd_eqbypass_BIT_IDX                                         10
#define REG_Ssd_eqbypass_NUM_OF_BITS                                     1
#define REG_Ssd_eqbypass_MIN                                             0
#define REG_Ssd_eqbypass_MAX                                             1
#define REG_SSD_EQBYPASS_VALUE_EQ_BYPASS_OFF                             0
#define REG_SSD_EQBYPASS_VALUE_EQ_BYPASS_ON                              1

/*!
 * @brief Clip Management Peakdetection Selection
 *        0 = Peak detection Main L,R
 *        1 = Peak detection L,R,C,Sub
 * @range 0..1
 ***/
#define REG_Ssd_dvm_peak_sel
#define REG_Ssd_dvm_peak_sel_OFFSET                                        0x00094
#define REG_Ssd_dvm_peak_sel_MASK                                        0x1
#define REG_Ssd_dvm_peak_sel_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_dvm_peak_sel_BIT_MASK                                    0x1000
#define REG_Ssd_dvm_peak_sel_BIT_IDX                                     12
#define REG_Ssd_dvm_peak_sel_NUM_OF_BITS                                 1
#define REG_Ssd_dvm_peak_sel_MIN                                         0
#define REG_Ssd_dvm_peak_sel_MAX                                         1
#define REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_MAIN_L_R               0
#define REG_SSD_DVM_PEAK_SEL_VALUE_PEAK_DETECTION_L_R_C_SUB              1

/*!
 * @brief Mixer Input Channel Selector  for summation to main channel output
 *        0 = Beeper Output
 *        1 = Processed Surround
 *        2 = Un-Processed Surround
 *        3 = Silence
 * @range 0..3
 ***/
#define REG_Ssd_mixersel
#define REG_Ssd_mixersel_OFFSET                                            0x00094
#define REG_Ssd_mixersel_MASK                                            0x3
#define REG_Ssd_mixersel_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mixersel_BIT_MASK                                        0x6000
#define REG_Ssd_mixersel_BIT_IDX                                         13
#define REG_Ssd_mixersel_NUM_OF_BITS                                     2
#define REG_Ssd_mixersel_MIN                                             0
#define REG_Ssd_mixersel_MAX                                             3
#define REG_SSD_MIXERSEL_VALUE_BEEPER_OUTPUT                             0
#define REG_SSD_MIXERSEL_VALUE_PROCESSED_SURROUND                        1
#define REG_SSD_MIXERSEL_VALUE_UN_PROCESSED_SURROUND                     2
#define REG_SSD_MIXERSEL_VALUE_SILENCE                                   3

/*!
 * @brief ISound Fade Time (Table1 / Table2) values
 *        0 = 1.1sec / 136msec
 *        1 = 2.2sec / 272msec
 *        2 = 3.3sec / 410msec
 *        3 = 4.5sec / 546msec
 *        4 = 6.8sec / 820msec
 *        5 = 9.0sec / 1.1sec
 *        6 = 14.5sec / 1.8sec
 *        7 = 19.0sec / 2.4sec
 * @range 0..7
 ***/
#define REG_Ssd_isoundfadetime
#define REG_Ssd_isoundfadetime_OFFSET                                      0x00094
#define REG_Ssd_isoundfadetime_MASK                                      0x7
#define REG_Ssd_isoundfadetime_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_isoundfadetime_BIT_MASK                                  0x38000
#define REG_Ssd_isoundfadetime_BIT_IDX                                   15
#define REG_Ssd_isoundfadetime_NUM_OF_BITS                               3
#define REG_Ssd_isoundfadetime_MIN                                       0
#define REG_Ssd_isoundfadetime_MAX                                       7
#define REG_SSD_ISOUNDFADETIME_VALUE_1_1SEC_136MSEC                      0
#define REG_SSD_ISOUNDFADETIME_VALUE_2_2SEC_272MSEC                      1
#define REG_SSD_ISOUNDFADETIME_VALUE_3_3SEC_410MSEC                      2
#define REG_SSD_ISOUNDFADETIME_VALUE_4_5SEC_546MSEC                      3
#define REG_SSD_ISOUNDFADETIME_VALUE_6_8SEC_820MSEC                      4
#define REG_SSD_ISOUNDFADETIME_VALUE_9_0SEC_1_1SEC                       5
#define REG_SSD_ISOUNDFADETIME_VALUE_14_5SEC_1_8SEC                      6
#define REG_SSD_ISOUNDFADETIME_VALUE_19_0SEC_2_4SEC                      7

/*!
 * @brief ISound Detector Time
 *        0 = 1sec
 *        1 = 5sec
 *        2 = 8sec
 *        3 = 14sec
 * @range 0..3
 ***/
#define REG_Ssd_isounddettime
#define REG_Ssd_isounddettime_OFFSET                                       0x00094
#define REG_Ssd_isounddettime_MASK                                       0x3
#define REG_Ssd_isounddettime_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_isounddettime_BIT_MASK                                   0xc0000
#define REG_Ssd_isounddettime_BIT_IDX                                    18
#define REG_Ssd_isounddettime_NUM_OF_BITS                                2
#define REG_Ssd_isounddettime_MIN                                        0
#define REG_Ssd_isounddettime_MAX                                        3
#define REG_SSD_ISOUNDDETTIME_VALUE_1SEC                                 0
#define REG_SSD_ISOUNDDETTIME_VALUE_5SEC                                 1
#define REG_SSD_ISOUNDDETTIME_VALUE_8SEC                                 2
#define REG_SSD_ISOUNDDETTIME_VALUE_14SEC                                3

/*!
 * @brief SRS Spatializer Selector
 *        0 = 3D Sound
 *        1 = WOW
 * @range 0..1
 ***/
#define REG_Ssd_srs_spatsel
#define REG_Ssd_srs_spatsel_OFFSET                                         0x00094
#define REG_Ssd_srs_spatsel_MASK                                         0x1
#define REG_Ssd_srs_spatsel_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_srs_spatsel_BIT_MASK                                     0x100000
#define REG_Ssd_srs_spatsel_BIT_IDX                                      20
#define REG_Ssd_srs_spatsel_NUM_OF_BITS                                  1
#define REG_Ssd_srs_spatsel_MIN                                          0
#define REG_Ssd_srs_spatsel_MAX                                          1
#define REG_SSD_SRS_SPATSEL_VALUE_3D_SOUND                               0
#define REG_SSD_SRS_SPATSEL_VALUE_WOW                                    1

/*!
 * @brief LipSync Bypas Switch
 *        0 = Bypass
 *        1 = Use External LipSync
 * @range 0..1
 ***/
#define REG_Ssd_lipsync_bypass
#define REG_Ssd_lipsync_bypass_OFFSET                                      0x00094
#define REG_Ssd_lipsync_bypass_MASK                                      0x1
#define REG_Ssd_lipsync_bypass_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_lipsync_bypass_BIT_MASK                                  0x200000
#define REG_Ssd_lipsync_bypass_BIT_IDX                                   21
#define REG_Ssd_lipsync_bypass_NUM_OF_BITS                               1
#define REG_Ssd_lipsync_bypass_MIN                                       0
#define REG_Ssd_lipsync_bypass_MAX                                       1
#define REG_SSD_LipSync_Bypass_VALUE_BYPASS                              0
#define REG_SSD_LipSync_Bypass_VALUE_USE_EXTERNAL_LIPSYNC                1

/*!
 * @brief Isound Fader Table Selector
 *        0 = Table 1 (1.1sec... 19sec)
 *        1 = Table 2 (136msec... 2.4sec)
 * @range 0..1
 ***/
#define REG_Ssd_isoundfadetimesel
#define REG_Ssd_isoundfadetimesel_OFFSET                                   0x00094
#define REG_Ssd_isoundfadetimesel_MASK                                   0x1
#define REG_Ssd_isoundfadetimesel_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_isoundfadetimesel_BIT_MASK                               0x400000
#define REG_Ssd_isoundfadetimesel_BIT_IDX                                22
#define REG_Ssd_isoundfadetimesel_NUM_OF_BITS                            1
#define REG_Ssd_isoundfadetimesel_MIN                                    0
#define REG_Ssd_isoundfadetimesel_MAX                                    1
#define REG_SSD_ISOUNDFADETIMESEL_VALUE_TABLE_1_1_1SEC_19SEC             0
#define REG_SSD_ISOUNDFADETIMESEL_VALUE_TABLE_2_136MSEC_2_4SEC           1

/*!
 * @range 0..15
 ***/
#define REG_Ssd_sou_dvm_ctrl_reg_286
#define REG_Ssd_sou_dvm_ctrl_reg_286_OFFSET                                0x00098
#define REG_Ssd_sou_dvm_ctrl_reg_286_MASK                                0xf
#define REG_Ssd_sou_dvm_ctrl_reg_286_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_sou_dvm_ctrl_reg_286_BIT_MASK                            0xf
#define REG_Ssd_sou_dvm_ctrl_reg_286_BIT_IDX                             0
#define REG_Ssd_sou_dvm_ctrl_reg_286_NUM_OF_BITS                         4
#define REG_Ssd_sou_dvm_ctrl_reg_286_MIN                                 0
#define REG_Ssd_sou_dvm_ctrl_reg_286_MAX                                 15

/*!
 * @brief Dynamic Volume Mode Release Range Upper Limit (Clip Management)
 *        0 = -3,5dBFS
 *        1 = -6dBFS
 *        2 = -9dBFS
 *        3 = -12dBFS
 * @range 0..3
 ***/
#define REG_Ssd_dvm_rr_upper
#define REG_Ssd_dvm_rr_upper_OFFSET                                        0x00098
#define REG_Ssd_dvm_rr_upper_MASK                                        0x3
#define REG_Ssd_dvm_rr_upper_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_dvm_rr_upper_BIT_MASK                                    0x30
#define REG_Ssd_dvm_rr_upper_BIT_IDX                                     4
#define REG_Ssd_dvm_rr_upper_NUM_OF_BITS                                 2
#define REG_Ssd_dvm_rr_upper_MIN                                         0
#define REG_Ssd_dvm_rr_upper_MAX                                         3
#define REG_SSD_DVM_RR_UPPER_VALUE_MINUS_3_5DBFS                         0
#define REG_SSD_DVM_RR_UPPER_VALUE_MINUS_6DBFS                           1
#define REG_SSD_DVM_RR_UPPER_VALUE_MINUS_9DBFS                           2
#define REG_SSD_DVM_RR_UPPER_VALUE_MINUS_12DBFS                          3

/*!
 * @brief Dynamic Volume Mode Release Range Lower Limit (Clip Management)
 *        0 = -40dBFS
 *        1 = -50dBFS
 *        2 = -60dBFS
 *        3 = -inf
 * @range 0..3
 ***/
#define REG_Ssd_dvm_rr_lower
#define REG_Ssd_dvm_rr_lower_OFFSET                                        0x00098
#define REG_Ssd_dvm_rr_lower_MASK                                        0x3
#define REG_Ssd_dvm_rr_lower_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_dvm_rr_lower_BIT_MASK                                    0xc0
#define REG_Ssd_dvm_rr_lower_BIT_IDX                                     6
#define REG_Ssd_dvm_rr_lower_NUM_OF_BITS                                 2
#define REG_Ssd_dvm_rr_lower_MIN                                         0
#define REG_Ssd_dvm_rr_lower_MAX                                         3
#define REG_SSD_DVM_RR_LOWER_VALUE_MINUS_40DBFS                          0
#define REG_SSD_DVM_RR_LOWER_VALUE_MINUS_50DBFS                          1
#define REG_SSD_DVM_RR_LOWER_VALUE_MINUS_60DBFS                          2
#define REG_SSD_DVM_RR_LOWER_VALUE_INF                                   3

/*!
 * @brief Dynamic Volume Mode Attack Speed
 *        0 = 4.8dB/s
 *        1 = 9.6dB/s
 * @range 0..1
 ***/
#define REG_Ssd_dvm_attspeed
#define REG_Ssd_dvm_attspeed_OFFSET                                        0x00098
#define REG_Ssd_dvm_attspeed_MASK                                        0x1
#define REG_Ssd_dvm_attspeed_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_dvm_attspeed_BIT_MASK                                    0x100
#define REG_Ssd_dvm_attspeed_BIT_IDX                                     8
#define REG_Ssd_dvm_attspeed_NUM_OF_BITS                                 1
#define REG_Ssd_dvm_attspeed_MIN                                         0
#define REG_Ssd_dvm_attspeed_MAX                                         1
#define REG_SSD_DVM_ATTSPEED_VALUE_4_8DB_S                               0
#define REG_SSD_DVM_ATTSPEED_VALUE_9_6DB_S                               1

/*!
 * @brief Dynamic Volume Mode Release Speed
 *        0 = 0.06dB/s
 *        1 = 0.12dB/s
 *        2 = 0.24dB/s
 *        3 = 0.48dB/s
 * @range 0..3
 ***/
#define REG_Ssd_dvm_relspeed
#define REG_Ssd_dvm_relspeed_OFFSET                                        0x00098
#define REG_Ssd_dvm_relspeed_MASK                                        0x3
#define REG_Ssd_dvm_relspeed_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_dvm_relspeed_BIT_MASK                                    0x600
#define REG_Ssd_dvm_relspeed_BIT_IDX                                     9
#define REG_Ssd_dvm_relspeed_NUM_OF_BITS                                 2
#define REG_Ssd_dvm_relspeed_MIN                                         0
#define REG_Ssd_dvm_relspeed_MAX                                         3
#define REG_SSD_DVM_RELSPEED_VALUE_0_06DB_S                              0
#define REG_SSD_DVM_RELSPEED_VALUE_0_12DB_S                              1
#define REG_SSD_DVM_RELSPEED_VALUE_0_24DB_S                              2
#define REG_SSD_DVM_RELSPEED_VALUE_0_48DB_S                              3

/*!
 * @brief BBE Contour value
 *        $0 = Min. bass boost
 *        $F = Max. bass boost
 * @range 0..15
 ***/
#define REG_Ssd_bbecontour
#define REG_Ssd_bbecontour_OFFSET                                          0x0009C
#define REG_Ssd_bbecontour_MASK                                          0xf
#define REG_Ssd_bbecontour_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_bbecontour_BIT_MASK                                      0xf
#define REG_Ssd_bbecontour_BIT_IDX                                       0
#define REG_Ssd_bbecontour_NUM_OF_BITS                                   4
#define REG_Ssd_bbecontour_MIN                                           0
#define REG_Ssd_bbecontour_MAX                                           15
#define REG_SSD_BBECONTOUR_VALUE_MIN_BASS_BOOST                          0
#define REG_SSD_BBECONTOUR_VALUE_MAX_BASS_BOOST                          15

/*!
 * @brief BBE Process value
 *        $0 = Min. process
 *        $F = Max. process
 * @range 0..15
 ***/
#define REG_Ssd_bbeprocess
#define REG_Ssd_bbeprocess_OFFSET                                          0x0009C
#define REG_Ssd_bbeprocess_MASK                                          0xf
#define REG_Ssd_bbeprocess_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_bbeprocess_BIT_MASK                                      0xf0
#define REG_Ssd_bbeprocess_BIT_IDX                                       4
#define REG_Ssd_bbeprocess_NUM_OF_BITS                                   4
#define REG_Ssd_bbeprocess_MIN                                           0
#define REG_Ssd_bbeprocess_MAX                                           15
#define REG_SSD_BBEPROCESS_VALUE_MIN_PROCESS                             0
#define REG_SSD_BBEPROCESS_VALUE_MAX_PROCESS                             15

/*!
 * @brief Main Loudness
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_mainloud
#define REG_Ssd_mainloud_OFFSET                                            0x0009C
#define REG_Ssd_mainloud_MASK                                            0x1
#define REG_Ssd_mainloud_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mainloud_BIT_MASK                                        0x100
#define REG_Ssd_mainloud_BIT_IDX                                         8
#define REG_Ssd_mainloud_NUM_OF_BITS                                     1
#define REG_Ssd_mainloud_MIN                                             0
#define REG_Ssd_mainloud_MAX                                             1
#define REG_SSD_MAINLOUD_VALUE_OFF                                       0
#define REG_SSD_MAINLOUD_VALUE_ON                                        1

/*!
 * @brief Main Loudness None Attack Volume Level [Set1 or Set2 selected by "MAINLONAEXT"]
 *        0 = -15dB Volume ; +9dB Volume
 *        1 = -12dB Volume ; +12dB Volume
 *        2 = -9dB Volume  ; +15dB Volume
 *        3 = -6dB Volume  ; +18dB Volume
 *        4 = -3dB Volume  ; +21dB Volume
 *        5 = 0dB Volume   ; +24dB Volume
 *        6 = +3dB Volume ; +27dB Volume
 *        7 = +6dB Volume ; +30dB Volume
 * @range 0..7
 ***/
#define REG_Ssd_mainlona
#define REG_Ssd_mainlona_OFFSET                                            0x0009C
#define REG_Ssd_mainlona_MASK                                            0x7
#define REG_Ssd_mainlona_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mainlona_BIT_MASK                                        0xe00
#define REG_Ssd_mainlona_BIT_IDX                                         9
#define REG_Ssd_mainlona_NUM_OF_BITS                                     3
#define REG_Ssd_mainlona_MIN                                             0
#define REG_Ssd_mainlona_MAX                                             7
#define REG_SSD_MAINLONA_VALUE_MINUS_15DB_VOLUME_9DB_VOLUME              0
#define REG_SSD_MAINLONA_VALUE_MINUS_12DB_VOLUME_12DB_VOLUME             1
#define REG_SSD_MAINLONA_VALUE_MINUS_9DB_VOLUME_15DB_VOLUME              2
#define REG_SSD_MAINLONA_VALUE_MINUS_6DB_VOLUME_18DB_VOLUME              3
#define REG_SSD_MAINLONA_VALUE_MINUS_3DB_VOLUME_21DB_VOLUME              4
#define REG_SSD_MAINLONA_VALUE_0DB_VOLUME_24DB_VOLUME                    5
#define REG_SSD_MAINLONA_VALUE_3DB_VOLUME_27DB_VOLUME                    6
#define REG_SSD_MAINLONA_VALUE_6DB_VOLUME_30DB_VOLUME                    7

/*!
 * @brief Main Loudness Filter Characteristic
 *        0 = standard (500Hz)
 *        1 = extra bass (1000Hz)
 *        bass: 20 Hz -> max. 18.3 dB
 *        treble: 16 kHz -> max. 4.3 dB
 * @range 0..3
 ***/
#define REG_Ssd_mainloch
#define REG_Ssd_mainloch_OFFSET                                            0x0009C
#define REG_Ssd_mainloch_MASK                                            0x3
#define REG_Ssd_mainloch_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_mainloch_BIT_MASK                                        0x3000
#define REG_Ssd_mainloch_BIT_IDX                                         12
#define REG_Ssd_mainloch_NUM_OF_BITS                                     2
#define REG_Ssd_mainloch_MIN                                             0
#define REG_Ssd_mainloch_MAX                                             3
#define REG_SSD_MAINLOCH_VALUE_STANDARD_500HZ                            0
#define REG_SSD_MAINLOCH_VALUE_EXTRA_BASS_1000HZ                         1

/*!
 * @brief DBB, DVB and BBE Control
 *        0 = DBB, DVB and BBE Off
 *        1 = DBB main channel On
 *        2 = DVB main channel On
 *        3 = DBB subwoofer channel On
 *        4 = DVB subwoofer channel On
 *        5 = BBE On
 * @range 0..5
 ***/
#define REG_Ssd_bassfeaturectrl
#define REG_Ssd_bassfeaturectrl_OFFSET                                     0x0009C
#define REG_Ssd_bassfeaturectrl_MASK                                     0x7
#define REG_Ssd_bassfeaturectrl_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_bassfeaturectrl_BIT_MASK                                 0x1c000
#define REG_Ssd_bassfeaturectrl_BIT_IDX                                  14
#define REG_Ssd_bassfeaturectrl_NUM_OF_BITS                              3
#define REG_Ssd_bassfeaturectrl_MIN                                      0
#define REG_Ssd_bassfeaturectrl_MAX                                      5
#define REG_SSD_BASSFEATURECTRL_VALUE_DBB_DVB_AND_BBE_OFF                0
#define REG_SSD_BASSFEATURECTRL_VALUE_DBB_MAIN_CHANNEL_ON                1
#define REG_SSD_BASSFEATURECTRL_VALUE_DVB_MAIN_CHANNEL_ON                2
#define REG_SSD_BASSFEATURECTRL_VALUE_DBB_SUBWOOFER_CHANNEL_ON           3
#define REG_SSD_BASSFEATURECTRL_VALUE_DVB_SUBWOOFER_CHANNEL_ON           4
#define REG_SSD_BASSFEATURECTRL_VALUE_BBE_ON                             5

/*!
 * @brief Main Loudness None Attack Volume Level Extension
 *        0 = Set 1 (-15dB...+6dB)
 *        1 = Set 2 (+9dB...+30dB)
 * @range 0..1
 ***/
#define REG_Ssd_mainlonaext
#define REG_Ssd_mainlonaext_OFFSET                                         0x0009C
#define REG_Ssd_mainlonaext_MASK                                         0x1
#define REG_Ssd_mainlonaext_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_mainlonaext_BIT_MASK                                     0x20000
#define REG_Ssd_mainlonaext_BIT_IDX                                      17
#define REG_Ssd_mainlonaext_NUM_OF_BITS                                  1
#define REG_Ssd_mainlonaext_MIN                                          0
#define REG_Ssd_mainlonaext_MAX                                          1
#define REG_SSD_MAINLONAEXT_VALUE_SET_1_MINUS_15DB_6DB                   0
#define REG_SSD_MAINLONAEXT_VALUE_SET_2_9DB_30DB                         1

/*!
 * @brief Spatializer Sound Effect
 *        0 = OFF
 *        1 = I-Stereo
 *        2 = I-Mono
 *        3 = SRS Spatializer | (3D Sound or WOW selected by "SRS_SPATSEL")
 * @range 0..3
 ***/
#define REG_Ssd_somoctrl
#define REG_Ssd_somoctrl_OFFSET                                            0x000A0
#define REG_Ssd_somoctrl_MASK                                            0x3
#define REG_Ssd_somoctrl_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_somoctrl_BIT_MASK                                        0x3
#define REG_Ssd_somoctrl_BIT_IDX                                         0
#define REG_Ssd_somoctrl_NUM_OF_BITS                                     2
#define REG_Ssd_somoctrl_MIN                                             0
#define REG_Ssd_somoctrl_MAX                                             3
#define REG_SSD_SOMOCTRL_VALUE_OFF                                       0
#define REG_SSD_SOMOCTRL_VALUE_I_STEREO                                  1
#define REG_SSD_SOMOCTRL_VALUE_I_MONO                                    2
#define REG_SSD_SOMOCTRL_VALUE_SRS_SPATIALIZER                           3

/*!
 * @brief I-Mono or I-Stereo Effect
 *        0 = 1 (Min)
 *        1 = 2
 *        2 = 3
 *        3 = 4
 *        4 = 5
 *        5 = 6 (Max)
 * @range 0..7
 ***/
#define REG_Ssd_insoef
#define REG_Ssd_insoef_OFFSET                                              0x000A0
#define REG_Ssd_insoef_MASK                                              0x7
#define REG_Ssd_insoef_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_insoef_BIT_MASK                                          0x1c
#define REG_Ssd_insoef_BIT_IDX                                           2
#define REG_Ssd_insoef_NUM_OF_BITS                                       3
#define REG_Ssd_insoef_MIN                                               0
#define REG_Ssd_insoef_MAX                                               7
#define REG_SSD_INSOEF_VALUE_1_MIN                                       0
#define REG_SSD_INSOEF_VALUE_2                                           1
#define REG_SSD_INSOEF_VALUE_3                                           2
#define REG_SSD_INSOEF_VALUE_4                                           3
#define REG_SSD_INSOEF_VALUE_5                                           4
#define REG_SSD_INSOEF_VALUE_6_MAX                                       5

/*!
 * @brief AVL Mode Control
 *        0 = OFF
 *        1 = very short decay (20 ms)
 *        2 = short decay (2 sec)
 *        3 = medium decay (4 sec)
 *        4 = long decay (8 sec)
 *        5 = very long decay (16 sec)
 * @range 0..7
 ***/
#define REG_Ssd_avlmod
#define REG_Ssd_avlmod_OFFSET                                              0x000A0
#define REG_Ssd_avlmod_MASK                                              0x7
#define REG_Ssd_avlmod_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_avlmod_BIT_MASK                                          0xe0
#define REG_Ssd_avlmod_BIT_IDX                                           5
#define REG_Ssd_avlmod_NUM_OF_BITS                                       3
#define REG_Ssd_avlmod_MIN                                               0
#define REG_Ssd_avlmod_MAX                                               7
#define REG_SSD_AVLMOD_VALUE_OFF                                         0
#define REG_SSD_AVLMOD_VALUE_VERY_SHORT_DECAY_20_MS                      1
#define REG_SSD_AVLMOD_VALUE_SHORT_DECAY_2_SEC                           2
#define REG_SSD_AVLMOD_VALUE_MEDIUM_DECAY_4_SEC                          3
#define REG_SSD_AVLMOD_VALUE_LONG_DECAY_8_SEC                            4
#define REG_SSD_AVLMOD_VALUE_VERY_LONG_DECAY_16_SEC                      5

/*!
 * @brief AVL Weighting Filter
 *        0 = OFF
 *        1 = ON (recommended)
 * @range 0..1
 ***/
#define REG_Ssd_avlweight
#define REG_Ssd_avlweight_OFFSET                                           0x000A0
#define REG_Ssd_avlweight_MASK                                           0x1
#define REG_Ssd_avlweight_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_avlweight_BIT_MASK                                       0x100
#define REG_Ssd_avlweight_BIT_IDX                                        8
#define REG_Ssd_avlweight_NUM_OF_BITS                                    1
#define REG_Ssd_avlweight_MIN                                            0
#define REG_Ssd_avlweight_MAX                                            1
#define REG_SSD_AVLWEIGHT_VALUE_OFF                                      0
#define REG_SSD_AVLWEIGHT_VALUE_ON_RECOMMENDED                           1

/*!
 * @brief AVL Reference Level
 *        2 = high threshold | (-10 dBFS), small reduction ("daytime mode")
 *        7 = medium threshold | (-20 dBFS), medium reduction ("evening mode")
 *        12 = low threshold | (-30 dBFS), strong reduction ("night mode")
 * @range 0..15
 ***/
#define REG_Ssd_avllev
#define REG_Ssd_avllev_OFFSET                                              0x000A0
#define REG_Ssd_avllev_MASK                                              0xf
#define REG_Ssd_avllev_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_avllev_BIT_MASK                                          0x1e00
#define REG_Ssd_avllev_BIT_IDX                                           9
#define REG_Ssd_avllev_NUM_OF_BITS                                       4
#define REG_Ssd_avllev_MIN                                               0
#define REG_Ssd_avllev_MAX                                               15
#define REG_SSD_AVLLEV_VALUE_HIGH_THRESHOLD                              2
#define REG_SSD_AVLLEV_VALUE_MEDIUM_THRESHOLD                            7
#define REG_SSD_AVLLEV_VALUE_LOW_THRESHOLD                               12

/*!
 * @brief SRS 3D Sound Center
 *        $0 = -9dB
 *        $1 = -14dB
 *        $2 = -15dB
 *        $3 = -16dB
 *        $4 = -17dB
 *        $5 = -18dB
 *        $6 = -19dB
 *        $7 = -20dB
 *        $8 = -21dB
 *        $9 = -22dB
 *        $A = -23dB
 *        $B = -24dB
 *        $C = -25dB
 *        $D = -26dB
 *        $E = -27dB
 *        $F = off
 * @range 0..15
 ***/
#define REG_Ssd_srs3dcenter
#define REG_Ssd_srs3dcenter_OFFSET                                         0x000A0
#define REG_Ssd_srs3dcenter_MASK                                         0xf
#define REG_Ssd_srs3dcenter_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_srs3dcenter_BIT_MASK                                     0x1e000
#define REG_Ssd_srs3dcenter_BIT_IDX                                      13
#define REG_Ssd_srs3dcenter_NUM_OF_BITS                                  4
#define REG_Ssd_srs3dcenter_MIN                                          0
#define REG_Ssd_srs3dcenter_MAX                                          15
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_9DB                              0
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_14DB                             1
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_15DB                             2
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_16DB                             3
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_17DB                             4
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_18DB                             5
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_19DB                             6
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_20DB                             7
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_21DB                             8
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_22DB                             9
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_23DB                             10
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_24DB                             11
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_25DB                             12
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_26DB                             13
#define REG_SSD_SRS3DCENTER_VALUE_MINUS_27DB                             14
#define REG_SSD_SRS3DCENTER_VALUE_OFF                                    15

/*!
 * @brief SRS 3D Sound Space
 *        $0 = -4dB
 *        $1 = -5dB
 *        $2 = -6dB
 *        $3 = -7dB
 *        $4 = -8dB
 *        $5 = -9dB
 *        $6 = -10dB
 *        $7 = -11dB
 *        $8 = -12dB
 *        $9 = -13dB
 *        $A = -14dB
 *        $B = -15dB
 *        $C = -16dB
 *        $D = -17dB
 *        $E = -18dB
 *        $F = off
 * @range 0..15
 ***/
#define REG_Ssd_srs3dspace
#define REG_Ssd_srs3dspace_OFFSET                                          0x000A0
#define REG_Ssd_srs3dspace_MASK                                          0xf
#define REG_Ssd_srs3dspace_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_srs3dspace_BIT_MASK                                      0x1e0000
#define REG_Ssd_srs3dspace_BIT_IDX                                       17
#define REG_Ssd_srs3dspace_NUM_OF_BITS                                   4
#define REG_Ssd_srs3dspace_MIN                                           0
#define REG_Ssd_srs3dspace_MAX                                           15
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_4DB                               0
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_5DB                               1
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_6DB                               2
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_7DB                               3
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_8DB                               4
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_9DB                               5
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_10DB                              6
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_11DB                              7
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_12DB                              8
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_13DB                              9
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_14DB                              10
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_15DB                              11
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_16DB                              12
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_17DB                              13
#define REG_SSD_SRS3DSPACE_VALUE_MINUS_18DB                              14
#define REG_SSD_SRS3DSPACE_VALUE_OFF                                     15

/*!
 * @brief SRS 3D Sound Bypass Mode (for test purpose)
 *        0 = 3D Sound active
 *        1 = Bypass active
 * @range 0..1
 ***/
#define REG_Ssd_srs3dbypass
#define REG_Ssd_srs3dbypass_OFFSET                                         0x000A0
#define REG_Ssd_srs3dbypass_MASK                                         0x1
#define REG_Ssd_srs3dbypass_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_srs3dbypass_BIT_MASK                                     0x200000
#define REG_Ssd_srs3dbypass_BIT_IDX                                      21
#define REG_Ssd_srs3dbypass_NUM_OF_BITS                                  1
#define REG_Ssd_srs3dbypass_MIN                                          0
#define REG_Ssd_srs3dbypass_MAX                                          1
#define REG_SSD_SRS3DBYPASS_VALUE_3D_SOUND_ACTIVE                        0
#define REG_SSD_SRS3DBYPASS_VALUE_BYPASS_ACTIVE                          1

/*!
 * @brief AVL Peak Mode Selector
 *        0 = AVL standalone
 *        1 = AVL with 'Clip Management'
 * @range 0..1
 ***/
#define REG_Ssd_avlpeakmode
#define REG_Ssd_avlpeakmode_OFFSET                                         0x000A0
#define REG_Ssd_avlpeakmode_MASK                                         0x1
#define REG_Ssd_avlpeakmode_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_avlpeakmode_BIT_MASK                                     0x400000
#define REG_Ssd_avlpeakmode_BIT_IDX                                      22
#define REG_Ssd_avlpeakmode_NUM_OF_BITS                                  1
#define REG_Ssd_avlpeakmode_MIN                                          0
#define REG_Ssd_avlpeakmode_MAX                                          1
#define REG_SSD_AVLPEAKMODE_VALUE_AVL_STANDALONE                         0
#define REG_SSD_AVLPEAKMODE_VALUE_AVL_WITH_CLIP_MANAGEMENT               1

/*!
 * @brief Incredible Sound Mode Selection
 *        0 = manual mode
 *        1 = automatic mode
 * @range 0..1
 ***/
#define REG_Ssd_isoundmode
#define REG_Ssd_isoundmode_OFFSET                                          0x000A0
#define REG_Ssd_isoundmode_MASK                                          0x1
#define REG_Ssd_isoundmode_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_isoundmode_BIT_MASK                                      0x800000
#define REG_Ssd_isoundmode_BIT_IDX                                       23
#define REG_Ssd_isoundmode_NUM_OF_BITS                                   1
#define REG_Ssd_isoundmode_MIN                                           0
#define REG_Ssd_isoundmode_MAX                                           1
#define REG_SSD_ISOUNDMODE_VALUE_MANUAL_MODE                             0
#define REG_SSD_ISOUNDMODE_VALUE_AUTOMATIC_MODE                          1

/*!
 * @range 0..15
 ***/
#define REG_Ssd_srs_wow_ctrl_reg_316
#define REG_Ssd_srs_wow_ctrl_reg_316_OFFSET                                0x000A4
#define REG_Ssd_srs_wow_ctrl_reg_316_MASK                                0xf
#define REG_Ssd_srs_wow_ctrl_reg_316_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_srs_wow_ctrl_reg_316_BIT_MASK                            0xf
#define REG_Ssd_srs_wow_ctrl_reg_316_BIT_IDX                             0
#define REG_Ssd_srs_wow_ctrl_reg_316_NUM_OF_BITS                         4
#define REG_Ssd_srs_wow_ctrl_reg_316_MIN                                 0
#define REG_Ssd_srs_wow_ctrl_reg_316_MAX                                 15

/*!
 * @brief SRS WOW Brightness Control
 *        0 = Low
 *        1 = Medium
 *        2 = High
 * @range 0..3
 ***/
#define REG_Ssd_wow_brightness
#define REG_Ssd_wow_brightness_OFFSET                                      0x000A4
#define REG_Ssd_wow_brightness_MASK                                      0x3
#define REG_Ssd_wow_brightness_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_wow_brightness_BIT_MASK                                  0x30
#define REG_Ssd_wow_brightness_BIT_IDX                                   4
#define REG_Ssd_wow_brightness_NUM_OF_BITS                               2
#define REG_Ssd_wow_brightness_MIN                                       0
#define REG_Ssd_wow_brightness_MAX                                       3
#define REG_SSD_WOW_BRIGHTNESS_VALUE_LOW                                 0
#define REG_SSD_WOW_BRIGHTNESS_VALUE_MEDIUM                              1
#define REG_SSD_WOW_BRIGHTNESS_VALUE_HIGH                                2

/*!
 * @brief SRS WOW Speaker Size Selection
 *        0 = 40Hz
 *        1 = 60Hz
 *        2 = 100Hz
 *        3 = 150Hz
 *        4 = 200Hz
 *        5 = 250Hz
 *        6 = 300Hz
 *        7 = 400Hz
 * @range 0..7
 ***/
#define REG_Ssd_wow_spksize
#define REG_Ssd_wow_spksize_OFFSET                                         0x000A4
#define REG_Ssd_wow_spksize_MASK                                         0x7
#define REG_Ssd_wow_spksize_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_wow_spksize_BIT_MASK                                     0x1c0
#define REG_Ssd_wow_spksize_BIT_IDX                                      6
#define REG_Ssd_wow_spksize_NUM_OF_BITS                                  3
#define REG_Ssd_wow_spksize_MIN                                          0
#define REG_Ssd_wow_spksize_MAX                                          7
#define REG_SSD_WOW_SPKSIZE_VALUE_40HZ                                   0
#define REG_SSD_WOW_SPKSIZE_VALUE_60HZ                                   1
#define REG_SSD_WOW_SPKSIZE_VALUE_100HZ                                  2
#define REG_SSD_WOW_SPKSIZE_VALUE_150HZ                                  3
#define REG_SSD_WOW_SPKSIZE_VALUE_200HZ                                  4
#define REG_SSD_WOW_SPKSIZE_VALUE_250HZ                                  5
#define REG_SSD_WOW_SPKSIZE_VALUE_300HZ                                  6
#define REG_SSD_WOW_SPKSIZE_VALUE_400HZ                                  7

/*!
 * @brief SRS WOW Bass Level Control
 *        $0 = 0%
 *        $1 = 10%
 *        $2 = 20%
 *        $3 = 30%
 *        $4 = 40%
 *        $5 = 50%
 *        $6 = 60%
 *        $7 = 70%
 *        $8 = 80%
 *        $9 = 90%
 *        $A = 100%
 *        >$B Reserved
 * @range 0..15
 ***/
#define REG_Ssd_wow_basslevel
#define REG_Ssd_wow_basslevel_OFFSET                                       0x000A4
#define REG_Ssd_wow_basslevel_MASK                                       0xf
#define REG_Ssd_wow_basslevel_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_wow_basslevel_BIT_MASK                                   0x1e00
#define REG_Ssd_wow_basslevel_BIT_IDX                                    9
#define REG_Ssd_wow_basslevel_NUM_OF_BITS                                4
#define REG_Ssd_wow_basslevel_MIN                                        0
#define REG_Ssd_wow_basslevel_MAX                                        15
#define REG_SSD_WOW_BASSLEVEL_VALUE_0                                    0
#define REG_SSD_WOW_BASSLEVEL_VALUE_10                                   1
#define REG_SSD_WOW_BASSLEVEL_VALUE_20                                   2
#define REG_SSD_WOW_BASSLEVEL_VALUE_30                                   3
#define REG_SSD_WOW_BASSLEVEL_VALUE_40                                   4
#define REG_SSD_WOW_BASSLEVEL_VALUE_50                                   5
#define REG_SSD_WOW_BASSLEVEL_VALUE_60                                   6
#define REG_SSD_WOW_BASSLEVEL_VALUE_70                                   7
#define REG_SSD_WOW_BASSLEVEL_VALUE_80                                   8
#define REG_SSD_WOW_BASSLEVEL_VALUE_90                                   9
#define REG_SSD_WOW_BASSLEVEL_VALUE_100                                  10

/*!
 * @brief SRS WOW Stereo Width Control
 *        0 = 0%
 *        1 = 10%
 *        2 = 20%
 *        3 = 30%
 *        4 = 40%
 *        5 = 50%
 *        6 = 60%
 *        7 = 70%
 *        8 = 80%
 *        9 = 90%
 *        10 = 100%
 * @range 0..15
 ***/
#define REG_Ssd_wow_stereowidth
#define REG_Ssd_wow_stereowidth_OFFSET                                     0x000A4
#define REG_Ssd_wow_stereowidth_MASK                                     0xf
#define REG_Ssd_wow_stereowidth_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_wow_stereowidth_BIT_MASK                                 0x1e000
#define REG_Ssd_wow_stereowidth_BIT_IDX                                  13
#define REG_Ssd_wow_stereowidth_NUM_OF_BITS                              4
#define REG_Ssd_wow_stereowidth_MIN                                      0
#define REG_Ssd_wow_stereowidth_MAX                                      15
#define REG_SSD_WOW_STEREOWIDTH_VALUE_0                                  0
#define REG_SSD_WOW_STEREOWIDTH_VALUE_10                                 1
#define REG_SSD_WOW_STEREOWIDTH_VALUE_20                                 2
#define REG_SSD_WOW_STEREOWIDTH_VALUE_30                                 3
#define REG_SSD_WOW_STEREOWIDTH_VALUE_40                                 4
#define REG_SSD_WOW_STEREOWIDTH_VALUE_50                                 5
#define REG_SSD_WOW_STEREOWIDTH_VALUE_60                                 6
#define REG_SSD_WOW_STEREOWIDTH_VALUE_70                                 7
#define REG_SSD_WOW_STEREOWIDTH_VALUE_80                                 8
#define REG_SSD_WOW_STEREOWIDTH_VALUE_90                                 9
#define REG_SSD_WOW_STEREOWIDTH_VALUE_100                                10

/*!
 * @brief TSXT K Mode selection
 *        0 = k1_0| (use Sound Mode  is $B)
 *        1 = k2_0| (use Sound Mode  is  $B)
 *        2 = k3_1| (use Sound Mode  is  $C)
 *        3 = kPassiveMatrix| (use Sound Mode  is  $B)
 * @range 0..7
 ***/
#define REG_Ssd_tsxt_k_mode
#define REG_Ssd_tsxt_k_mode_OFFSET                                         0x000A8
#define REG_Ssd_tsxt_k_mode_MASK                                         0x7
#define REG_Ssd_tsxt_k_mode_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_tsxt_k_mode_BIT_MASK                                     0x7
#define REG_Ssd_tsxt_k_mode_BIT_IDX                                      0
#define REG_Ssd_tsxt_k_mode_NUM_OF_BITS                                  3
#define REG_Ssd_tsxt_k_mode_MIN                                          0
#define REG_Ssd_tsxt_k_mode_MAX                                          7
#define REG_SSD_TSXT_K_MODE_VALUE_K1_0                                   0
#define REG_SSD_TSXT_K_MODE_VALUE_K2_0                                   1
#define REG_SSD_TSXT_K_MODE_VALUE_K3_1                                   2
#define REG_SSD_TSXT_K_MODE_VALUE_KPASSIVEMATRIX                         3

/*!
 * @brief Dialog Clarity function control
 *        $0= OFF
 *        $1= ON
 * @range 0..1
 ***/
#define REG_Ssd_tsxt_dialogclarityenable
#define REG_Ssd_tsxt_dialogclarityenable_OFFSET                            0x000A8
#define REG_Ssd_tsxt_dialogclarityenable_MASK                            0x1
#define REG_Ssd_tsxt_dialogclarityenable_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_tsxt_dialogclarityenable_BIT_MASK                        0x8
#define REG_Ssd_tsxt_dialogclarityenable_BIT_IDX                         3
#define REG_Ssd_tsxt_dialogclarityenable_NUM_OF_BITS                     1
#define REG_Ssd_tsxt_dialogclarityenable_MIN                             0
#define REG_Ssd_tsxt_dialogclarityenable_MAX                             1
#define REG_SSD_TSXT_DialogClarityEnable_VALUE_OFF                       0
#define REG_SSD_TSXT_DialogClarityEnable_VALUE_ON                        1

/*!
 * @brief Tru Bass function control
 *        $0= OFF
 *        $1= ON
 * @range 0..1
 ***/
#define REG_Ssd_tsxt_trubassenable
#define REG_Ssd_tsxt_trubassenable_OFFSET                                  0x000A8
#define REG_Ssd_tsxt_trubassenable_MASK                                  0x1
#define REG_Ssd_tsxt_trubassenable_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_tsxt_trubassenable_BIT_MASK                              0x10
#define REG_Ssd_tsxt_trubassenable_BIT_IDX                               4
#define REG_Ssd_tsxt_trubassenable_NUM_OF_BITS                           1
#define REG_Ssd_tsxt_trubassenable_MIN                                   0
#define REG_Ssd_tsxt_trubassenable_MAX                                   1
#define REG_SSD_TSXT_TruBassEnable_VALUE_OFF                             0
#define REG_SSD_TSXT_TruBassEnable_VALUE_ON                              1

/*!
 * @brief Headphone control
 *        $0= OFF
 *        $1= ON
 * @range 0..1
 ***/
#define REG_Ssd_tsxt_headphone_modeenable
#define REG_Ssd_tsxt_headphone_modeenable_OFFSET                           0x000A8
#define REG_Ssd_tsxt_headphone_modeenable_MASK                           0x1
#define REG_Ssd_tsxt_headphone_modeenable_SIGNED_BIT_MASK                0x0
#define REG_Ssd_tsxt_headphone_modeenable_BIT_MASK                       0x20
#define REG_Ssd_tsxt_headphone_modeenable_BIT_IDX                        5
#define REG_Ssd_tsxt_headphone_modeenable_NUM_OF_BITS                    1
#define REG_Ssd_tsxt_headphone_modeenable_MIN                            0
#define REG_Ssd_tsxt_headphone_modeenable_MAX                            1
#define REG_SSD_TSXT_Headphone_ModeEnable_VALUE_OFF                      0
#define REG_SSD_TSXT_Headphone_ModeEnable_VALUE_ON                       1

/*!
 * @brief Center Output Mode control
 *        $0= OFF
 *        $1= ON
 * @range 0..1
 ***/
#define REG_Ssd_tsxt_centerout_523modeenable
#define REG_Ssd_tsxt_centerout_523modeenable_OFFSET                        0x000A8
#define REG_Ssd_tsxt_centerout_523modeenable_MASK                        0x1
#define REG_Ssd_tsxt_centerout_523modeenable_SIGNED_BIT_MASK             0x0
#define REG_Ssd_tsxt_centerout_523modeenable_BIT_MASK                    0x40
#define REG_Ssd_tsxt_centerout_523modeenable_BIT_IDX                     6
#define REG_Ssd_tsxt_centerout_523modeenable_NUM_OF_BITS                 1
#define REG_Ssd_tsxt_centerout_523modeenable_MIN                         0
#define REG_Ssd_tsxt_centerout_523modeenable_MAX                         1
#define REG_SSD_TSXT_CenterOut_523ModeEnable_VALUE_OFF                   0
#define REG_SSD_TSXT_CenterOut_523ModeEnable_VALUE_ON                    1

/*!
 * @brief Speaker Size selection
 *        $0= 40Hz
 *        $1= 60Hz
 *        $2= 100Hz
 *        $3= 150Hz
 *        $4= 200Hz
 *        $5= 250Hz
 *        $6= 300Hz
 *        $7= 400Hz
 * @range 0..7
 ***/
#define REG_Ssd_tsxt_speakersize
#define REG_Ssd_tsxt_speakersize_OFFSET                                    0x000A8
#define REG_Ssd_tsxt_speakersize_MASK                                    0x7
#define REG_Ssd_tsxt_speakersize_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_tsxt_speakersize_BIT_MASK                                0x380
#define REG_Ssd_tsxt_speakersize_BIT_IDX                                 7
#define REG_Ssd_tsxt_speakersize_NUM_OF_BITS                             3
#define REG_Ssd_tsxt_speakersize_MIN                                     0
#define REG_Ssd_tsxt_speakersize_MAX                                     7
#define REG_SSD_TSXT_SpeakerSize_VALUE_40HZ                              0
#define REG_SSD_TSXT_SpeakerSize_VALUE_60HZ                              1
#define REG_SSD_TSXT_SpeakerSize_VALUE_100HZ                             2
#define REG_SSD_TSXT_SpeakerSize_VALUE_150HZ                             3
#define REG_SSD_TSXT_SpeakerSize_VALUE_200HZ                             4
#define REG_SSD_TSXT_SpeakerSize_VALUE_250HZ                             5
#define REG_SSD_TSXT_SpeakerSize_VALUE_300HZ                             6
#define REG_SSD_TSXT_SpeakerSize_VALUE_400HZ                             7

/*!
 * @brief TruBass gain control
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
#define REG_Ssd_tsxt_trubass_gain
#define REG_Ssd_tsxt_trubass_gain_OFFSET                                   0x000A8
#define REG_Ssd_tsxt_trubass_gain_MASK                                   0xf
#define REG_Ssd_tsxt_trubass_gain_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_tsxt_trubass_gain_BIT_MASK                               0x3c00
#define REG_Ssd_tsxt_trubass_gain_BIT_IDX                                10
#define REG_Ssd_tsxt_trubass_gain_NUM_OF_BITS                            4
#define REG_Ssd_tsxt_trubass_gain_MIN                                    0
#define REG_Ssd_tsxt_trubass_gain_MAX                                    15
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_0                                0
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_7                                1
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_13                               2
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_20                               3
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_27                               4
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_33                               5
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_40                               6
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_46                               7
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_53                               8
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_60                               9
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_67                               10
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_73                               11
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_79                               12
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_86                               13
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_92                               14
#define REG_SSD_TSXT_TruBass_GAIN_VALUE_100                              15

/*!
 * @brief Dialog Clarity gain control
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
#define REG_Ssd_tsxt_dialogclarity_gain
#define REG_Ssd_tsxt_dialogclarity_gain_OFFSET                             0x000A8
#define REG_Ssd_tsxt_dialogclarity_gain_MASK                             0xf
#define REG_Ssd_tsxt_dialogclarity_gain_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_tsxt_dialogclarity_gain_BIT_MASK                         0x3c000
#define REG_Ssd_tsxt_dialogclarity_gain_BIT_IDX                          14
#define REG_Ssd_tsxt_dialogclarity_gain_NUM_OF_BITS                      4
#define REG_Ssd_tsxt_dialogclarity_gain_MIN                              0
#define REG_Ssd_tsxt_dialogclarity_gain_MAX                              15
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_0                          0
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_7                          1
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_13                         2
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_20                         3
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_27                         4
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_33                         5
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_40                         6
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_46                         7
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_53                         8
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_60                         9
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_67                         10
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_73                         11
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_79                         12
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_86                         13
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_92                         14
#define REG_SSD_TSXT_DialogClarity_GAIN_VALUE_100                        15

/*!
 * @brief DBB Coefficient Address
 * @range 0..63
 ***/
#define REG_Ssd_dbbadr
#define REG_Ssd_dbbadr_OFFSET                                              0x000AC
#define REG_Ssd_dbbadr_MASK                                              0x3f
#define REG_Ssd_dbbadr_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_dbbadr_BIT_MASK                                          0x3f
#define REG_Ssd_dbbadr_BIT_IDX                                           0
#define REG_Ssd_dbbadr_NUM_OF_BITS                                       6
#define REG_Ssd_dbbadr_MIN                                               0
#define REG_Ssd_dbbadr_MAX                                               63

/*!
 * @brief DBB Coefficients
 * @range -2048..2047
 ***/
#define REG_Ssd_dbbcoef
#define REG_Ssd_dbbcoef_OFFSET                                             0x000AC
#define REG_Ssd_dbbcoef_MASK                                             0xfff
#define REG_Ssd_dbbcoef_SIGNED_BIT_MASK                                  0x800000
#define REG_Ssd_dbbcoef_BIT_MASK                                         0xfff000
#define REG_Ssd_dbbcoef_BIT_IDX                                          12
#define REG_Ssd_dbbcoef_NUM_OF_BITS                                      12
#define REG_Ssd_dbbcoef_MIN                                              -2048
#define REG_Ssd_dbbcoef_MAX                                              2047

/*!
 * @brief DVB Coefficient Address
 * @range 0..255
 ***/
#define REG_Ssd_dvbadr
#define REG_Ssd_dvbadr_OFFSET                                              0x000B0
#define REG_Ssd_dvbadr_MASK                                              0xff
#define REG_Ssd_dvbadr_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_dvbadr_BIT_MASK                                          0xff
#define REG_Ssd_dvbadr_BIT_IDX                                           0
#define REG_Ssd_dvbadr_NUM_OF_BITS                                       8
#define REG_Ssd_dvbadr_MIN                                               0
#define REG_Ssd_dvbadr_MAX                                               255

/*!
 * @brief DVB Coefficients
 * @range -2048..2047
 ***/
#define REG_Ssd_dvbcoef
#define REG_Ssd_dvbcoef_OFFSET                                             0x000B0
#define REG_Ssd_dvbcoef_MASK                                             0xfff
#define REG_Ssd_dvbcoef_SIGNED_BIT_MASK                                  0x800000
#define REG_Ssd_dvbcoef_BIT_MASK                                         0xfff000
#define REG_Ssd_dvbcoef_BIT_IDX                                          12
#define REG_Ssd_dvbcoef_NUM_OF_BITS                                      12
#define REG_Ssd_dvbcoef_MIN                                              -2048
#define REG_Ssd_dvbcoef_MAX                                              2047

/*!
 * @brief VDSII Effect Level: 0..100% (5 steps)
 *        0 = 0%
 *        1 = 20%
 *        2 = 40%
 *        3 = 60%
 *        4 = 80%
 *        5 = 100%
 *        >5 = reserved")
 * @range 0..7
 ***/
#define REG_Ssd_vdsmixlev
#define REG_Ssd_vdsmixlev_OFFSET                                           0x000B4
#define REG_Ssd_vdsmixlev_MASK                                           0x7
#define REG_Ssd_vdsmixlev_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_vdsmixlev_BIT_MASK                                       0x7
#define REG_Ssd_vdsmixlev_BIT_IDX                                        0
#define REG_Ssd_vdsmixlev_NUM_OF_BITS                                    3
#define REG_Ssd_vdsmixlev_MIN                                            0
#define REG_Ssd_vdsmixlev_MAX                                            7
#define REG_SSD_VDSMIXLEV_VALUE_0                                        0
#define REG_SSD_VDSMIXLEV_VALUE_20                                       1
#define REG_SSD_VDSMIXLEV_VALUE_40                                       2
#define REG_SSD_VDSMIXLEV_VALUE_60                                       3
#define REG_SSD_VDSMIXLEV_VALUE_80                                       4
#define REG_SSD_VDSMIXLEV_VALUE_100                                      5

/*!
 * @brief Bass Management Mode
 *        0 = OFF (Wide Centre Mode)
 *        1 = TYP1 configuration (Normal Centre Mode)
 *        2 = TYP2 configuration (Normal Centre Mode)
 * @range 0..2
 ***/
#define REG_Ssd_bamamo
#define REG_Ssd_bamamo_OFFSET                                              0x000B4
#define REG_Ssd_bamamo_MASK                                              0x3
#define REG_Ssd_bamamo_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_bamamo_BIT_MASK                                          0x300
#define REG_Ssd_bamamo_BIT_IDX                                           8
#define REG_Ssd_bamamo_NUM_OF_BITS                                       2
#define REG_Ssd_bamamo_MIN                                               0
#define REG_Ssd_bamamo_MAX                                               2
#define REG_SSD_BAMAMO_VALUE_OFF_WIDE_CENTRE_MODE                        0
#define REG_SSD_BAMAMO_VALUE_TYP1_CONFIGURATION_NORMAL_CENTRE_MODE       1
#define REG_SSD_BAMAMO_VALUE_TYP2_CONFIGURATION_NORMAL_CENTRE_MODE       2

/*!
 * @brief Bass Management Subwoofer Filter Control
 *        0 = Subwoofer filter Off
 *        1 = Subwoofer filter On
 * @range 0..1
 ***/
#define REG_Ssd_bamasub
#define REG_Ssd_bamasub_OFFSET                                             0x000B4
#define REG_Ssd_bamasub_MASK                                             0x1
#define REG_Ssd_bamasub_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_bamasub_BIT_MASK                                         0x400
#define REG_Ssd_bamasub_BIT_IDX                                          10
#define REG_Ssd_bamasub_NUM_OF_BITS                                      1
#define REG_Ssd_bamasub_MIN                                              0
#define REG_Ssd_bamasub_MAX                                              1
#define REG_SSD_BAMASUB_VALUE_SUBWOOFER_FILTER_OFF                       0
#define REG_SSD_BAMASUB_VALUE_SUBWOOFER_FILTER_ON                        1

/*!
 * @brief Bass Management Filtercharacteristics (50 - 400Hz Cornerfrequency)
 *        $0 = 50 Hz
 *        $1 = 60 Hz
 *        $2 = 70 Hz
 *        $3 = 80 Hz
 *        $4 = 90 Hz
 *        $5 = 100 Hz
 *        $6 = 110 Hz
 *        $7 = 120 Hz
 *        $8 = 130 Hz
 *        $9 = 140 Hz
 *        $A = 150 Hz
 *        $B = 200 Hz
 *        $C = 250 Hz
 *        $D = 300 Hz
 *        $E = 350 Hz
 *        $F = 400 Hz
 * @range 0..15
 ***/
#define REG_Ssd_bamafc
#define REG_Ssd_bamafc_OFFSET                                              0x000B4
#define REG_Ssd_bamafc_MASK                                              0xf
#define REG_Ssd_bamafc_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_bamafc_BIT_MASK                                          0x7800
#define REG_Ssd_bamafc_BIT_IDX                                           11
#define REG_Ssd_bamafc_NUM_OF_BITS                                       4
#define REG_Ssd_bamafc_MIN                                               0
#define REG_Ssd_bamafc_MAX                                               15
#define REG_SSD_BAMAFC_VALUE_50_HZ                                       0
#define REG_SSD_BAMAFC_VALUE_60_HZ                                       1
#define REG_SSD_BAMAFC_VALUE_70_HZ                                       2
#define REG_SSD_BAMAFC_VALUE_80_HZ                                       3
#define REG_SSD_BAMAFC_VALUE_90_HZ                                       4
#define REG_SSD_BAMAFC_VALUE_100_HZ                                      5
#define REG_SSD_BAMAFC_VALUE_110_HZ                                      6
#define REG_SSD_BAMAFC_VALUE_120_HZ                                      7
#define REG_SSD_BAMAFC_VALUE_130_HZ                                      8
#define REG_SSD_BAMAFC_VALUE_140_HZ                                      9
#define REG_SSD_BAMAFC_VALUE_150_HZ                                      10
#define REG_SSD_BAMAFC_VALUE_200_HZ                                      11
#define REG_SSD_BAMAFC_VALUE_250_HZ                                      12
#define REG_SSD_BAMAFC_VALUE_300_HZ                                      13
#define REG_SSD_BAMAFC_VALUE_350_HZ                                      14
#define REG_SSD_BAMAFC_VALUE_400_HZ                                      15

/*!
 * @brief Dolby ProLogic Flat 7kHz Filter (for test purpose only)
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_flat_7khz_filter
#define REG_Ssd_flat_7khz_filter_OFFSET                                    0x000B4
#define REG_Ssd_flat_7khz_filter_MASK                                    0x1
#define REG_Ssd_flat_7khz_filter_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_flat_7khz_filter_BIT_MASK                                0x8000
#define REG_Ssd_flat_7khz_filter_BIT_IDX                                 15
#define REG_Ssd_flat_7khz_filter_NUM_OF_BITS                             1
#define REG_Ssd_flat_7khz_filter_MIN                                     0
#define REG_Ssd_flat_7khz_filter_MAX                                     1
#define REG_SSD_FLAT_7KHZ_FILTER_VALUE_OFF                               0
#define REG_SSD_FLAT_7KHZ_FILTER_VALUE_ON                                1

/*!
 * @brief Dolby ProLogic Flat B-Type Filter (for test purpose only)
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_b_type_flat
#define REG_Ssd_b_type_flat_OFFSET                                         0x000B4
#define REG_Ssd_b_type_flat_MASK                                         0x1
#define REG_Ssd_b_type_flat_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_b_type_flat_BIT_MASK                                     0x10000
#define REG_Ssd_b_type_flat_BIT_IDX                                      16
#define REG_Ssd_b_type_flat_NUM_OF_BITS                                  1
#define REG_Ssd_b_type_flat_MIN                                          0
#define REG_Ssd_b_type_flat_MAX                                          1
#define REG_SSD_B_TYPE_FLAT_VALUE_OFF                                    0
#define REG_SSD_B_TYPE_FLAT_VALUE_ON                                     1

/*!
 * @brief Dolby ProLogic Autobalance (for test purpose only)
 *        0 = OFF
 *        1 = ON
 * @range 0..1
 ***/
#define REG_Ssd_abalcfg
#define REG_Ssd_abalcfg_OFFSET                                             0x000B4
#define REG_Ssd_abalcfg_MASK                                             0x1
#define REG_Ssd_abalcfg_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_abalcfg_BIT_MASK                                         0x20000
#define REG_Ssd_abalcfg_BIT_IDX                                          17
#define REG_Ssd_abalcfg_NUM_OF_BITS                                      1
#define REG_Ssd_abalcfg_MIN                                              0
#define REG_Ssd_abalcfg_MAX                                              1
#define REG_SSD_ABALCFG_VALUE_OFF                                        0
#define REG_SSD_ABALCFG_VALUE_ON                                         1

/*!
 * @brief VDSII Balance Control
 *        0 = Left
 *        1 = Center
 *        2 = Rigth
 * @range 0..3
 ***/
#define REG_Ssd_vdsiibalance
#define REG_Ssd_vdsiibalance_OFFSET                                        0x000B4
#define REG_Ssd_vdsiibalance_MASK                                        0x3
#define REG_Ssd_vdsiibalance_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_vdsiibalance_BIT_MASK                                    0xc0000
#define REG_Ssd_vdsiibalance_BIT_IDX                                     18
#define REG_Ssd_vdsiibalance_NUM_OF_BITS                                 2
#define REG_Ssd_vdsiibalance_MIN                                         0
#define REG_Ssd_vdsiibalance_MAX                                         3
#define REG_SSD_VDSIIBALANCE_VALUE_LEFT                                  0
#define REG_SSD_VDSIIBALANCE_VALUE_CENTER                                1
#define REG_SSD_VDSIIBALANCE_VALUE_RIGTH                                 2

/*!
 * @brief Master Volume: (+24..-83.875dB, mute ), controls MAIN, SW, C and S in 1/8dB steps
 *        192 = +24.000 dB
 *        191 = +23.875 dB
 *        ..
 *        184 = +23.000 dB
 *        ..
 *           0 =   0.000 dB
 *          -1 =  -0.125 dB
 *        ..
 *        -671 = -83.875 dB
 *        -672 = mute
 * @range -672..192
 ***/
#define REG_Ssd_mastervol
#define REG_Ssd_mastervol_OFFSET                                           0x000B8
#define REG_Ssd_mastervol_MASK                                           0x7ff
#define REG_Ssd_mastervol_SIGNED_BIT_MASK                                0x400
#define REG_Ssd_mastervol_BIT_MASK                                       0x7ff
#define REG_Ssd_mastervol_BIT_IDX                                        0
#define REG_Ssd_mastervol_NUM_OF_BITS                                    11
#define REG_Ssd_mastervol_MIN                                            -672
#define REG_Ssd_mastervol_MAX                                            192
#define REG_SSD_MASTERVOL_VALUE_MUTE                                     -672
#define REG_SSD_MASTERVOL_VALUE_MINUS_83_875_DB                          -671
#define REG_SSD_MASTERVOL_VALUE_MINUS_0_125_DB                           -1
#define REG_SSD_MASTERVOL_VALUE_0_000_DB                                 0
#define REG_SSD_MASTERVOL_VALUE_23_000_DB                                184
#define REG_SSD_MASTERVOL_VALUE_23_875_DB                                191
#define REG_SSD_MASTERVOL_VALUE_24_000_DB                                192

/*!
 * @brief Beeper Volume: (0..-83dB, mute)
 *        0 = 0 dB
 *        -1 = -1 dB
 *        ..
 *        -84 = mute
 * @range -84..0
 ***/
#define REG_Ssd_beepvol
#define REG_Ssd_beepvol_OFFSET                                             0x000B8
#define REG_Ssd_beepvol_MASK                                             0xff
#define REG_Ssd_beepvol_SIGNED_BIT_MASK                                  0x40000
#define REG_Ssd_beepvol_BIT_MASK                                         0x7f800
#define REG_Ssd_beepvol_BIT_IDX                                          11
#define REG_Ssd_beepvol_NUM_OF_BITS                                      8
#define REG_Ssd_beepvol_MIN                                              -84
#define REG_Ssd_beepvol_MAX                                              0
#define REG_SSD_BEEPVOL_VALUE_MUTE                                       -84
#define REG_SSD_BEEPVOL_VALUE_MINUS_1_DB                                 -1
#define REG_SSD_BEEPVOL_VALUE_0_DB                                       0

/*!
 * @brief Beeper Frequency: 30..12500 Hz [Set1;Set2 selected by BEEPFREEXT]
 *        0 = 200 Hz ; 30Hz
 *        1 = 400 Hz ; 60Hz
 *        2 = 1000 Hz ; 90Hz
 *        3 = 2000 Hz ; 120Hz
 *        4 = 3000 Hz ; 150Hz
 *        5 = 5000 Hz ; 180Hz
 *        6 = 8000 Hz ; 250Hz
 *        7 = 12500 Hz ; 300Hz
 * @range 0..7
 ***/
#define REG_Ssd_beepfreq
#define REG_Ssd_beepfreq_OFFSET                                            0x000B8
#define REG_Ssd_beepfreq_MASK                                            0x7
#define REG_Ssd_beepfreq_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_beepfreq_BIT_MASK                                        0x380000
#define REG_Ssd_beepfreq_BIT_IDX                                         19
#define REG_Ssd_beepfreq_NUM_OF_BITS                                     3
#define REG_Ssd_beepfreq_MIN                                             0
#define REG_Ssd_beepfreq_MAX                                             7
#define REG_SSD_BEEPFREQ_VALUE_200_HZ_30HZ                               0
#define REG_SSD_BEEPFREQ_VALUE_400_HZ_60HZ                               1
#define REG_SSD_BEEPFREQ_VALUE_1000_HZ_90HZ                              2
#define REG_SSD_BEEPFREQ_VALUE_2000_HZ_120HZ                             3
#define REG_SSD_BEEPFREQ_VALUE_3000_HZ_150HZ                             4
#define REG_SSD_BEEPFREQ_VALUE_5000_HZ_180HZ                             5
#define REG_SSD_BEEPFREQ_VALUE_8000_HZ_250HZ                             6
#define REG_SSD_BEEPFREQ_VALUE_12500_HZ_300HZ                            7

/*!
 * @brief BEEPER Frequency Set Selector
 *        0 = Beeper Frequency Set1
 *        1 = Beeper Frequency Set2
 * @range 0..1
 ***/
#define REG_Ssd_beepfreext
#define REG_Ssd_beepfreext_OFFSET                                          0x000B8
#define REG_Ssd_beepfreext_MASK                                          0x1
#define REG_Ssd_beepfreext_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_beepfreext_BIT_MASK                                      0x400000
#define REG_Ssd_beepfreext_BIT_IDX                                       22
#define REG_Ssd_beepfreext_NUM_OF_BITS                                   1
#define REG_Ssd_beepfreext_MIN                                           0
#define REG_Ssd_beepfreext_MAX                                           1
#define REG_SSD_BEEPFREEXT_VALUE_BEEPER_FREQUENCY_SET1                   0
#define REG_SSD_BEEPFREEXT_VALUE_BEEPER_FREQUENCY_SET2                   1

/*!
 * @brief Beeper Mode Selector
 *        0 = Graphic Mode
 *        1 = Parametric Mode
 * @range 0..1
 ***/
#define REG_Ssd_beepmodesel
#define REG_Ssd_beepmodesel_OFFSET                                         0x000B8
#define REG_Ssd_beepmodesel_MASK                                         0x1
#define REG_Ssd_beepmodesel_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_beepmodesel_BIT_MASK                                     0x800000
#define REG_Ssd_beepmodesel_BIT_IDX                                      23
#define REG_Ssd_beepmodesel_NUM_OF_BITS                                  1
#define REG_Ssd_beepmodesel_MIN                                          0
#define REG_Ssd_beepmodesel_MAX                                          1
#define REG_SSD_BEEPMODESEL_VALUE_GRAPHIC_MODE                           0
#define REG_SSD_BEEPMODESEL_VALUE_PARAMETRIC_MODE                        1

/*!
 * @brief MAIN Volume Left: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_mainvoll
#define REG_Ssd_mainvoll_OFFSET                                            0x000BC
#define REG_Ssd_mainvoll_MASK                                            0xff
#define REG_Ssd_mainvoll_SIGNED_BIT_MASK                                 0x80
#define REG_Ssd_mainvoll_BIT_MASK                                        0xff
#define REG_Ssd_mainvoll_BIT_IDX                                         0
#define REG_Ssd_mainvoll_NUM_OF_BITS                                     8
#define REG_Ssd_mainvoll_MIN                                             -84
#define REG_Ssd_mainvoll_MAX                                             24
#define REG_SSD_MAINVOLL_VALUE_MUTE                                      -84
#define REG_SSD_MAINVOLL_VALUE_23_DB                                     23
#define REG_SSD_MAINVOLL_VALUE_24_DB                                     24

/*!
 * @brief MAIN Volume Right: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_mainvolr
#define REG_Ssd_mainvolr_OFFSET                                            0x000BC
#define REG_Ssd_mainvolr_MASK                                            0xff
#define REG_Ssd_mainvolr_SIGNED_BIT_MASK                                 0x8000
#define REG_Ssd_mainvolr_BIT_MASK                                        0xff00
#define REG_Ssd_mainvolr_BIT_IDX                                         8
#define REG_Ssd_mainvolr_NUM_OF_BITS                                     8
#define REG_Ssd_mainvolr_MIN                                             -84
#define REG_Ssd_mainvolr_MAX                                             24
#define REG_SSD_MAINVOLR_VALUE_MUTE                                      -84
#define REG_SSD_MAINVOLR_VALUE_23_DB                                     23
#define REG_SSD_MAINVOLR_VALUE_24_DB                                     24

/*!
 * @brief SUBWOOFER Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_subwvol
#define REG_Ssd_subwvol_OFFSET                                             0x000C0
#define REG_Ssd_subwvol_MASK                                             0xff
#define REG_Ssd_subwvol_SIGNED_BIT_MASK                                  0x80
#define REG_Ssd_subwvol_BIT_MASK                                         0xff
#define REG_Ssd_subwvol_BIT_IDX                                          0
#define REG_Ssd_subwvol_NUM_OF_BITS                                      8
#define REG_Ssd_subwvol_MIN                                              -84
#define REG_Ssd_subwvol_MAX                                              24
#define REG_SSD_SUBWVOL_VALUE_MUTE                                       -84
#define REG_SSD_SUBWVOL_VALUE_23_DB                                      23
#define REG_SSD_SUBWVOL_VALUE_24_DB                                      24

/*!
 * @brief CENTER Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_centervol
#define REG_Ssd_centervol_OFFSET                                           0x000C0
#define REG_Ssd_centervol_MASK                                           0xff
#define REG_Ssd_centervol_SIGNED_BIT_MASK                                0x8000
#define REG_Ssd_centervol_BIT_MASK                                       0xff00
#define REG_Ssd_centervol_BIT_IDX                                        8
#define REG_Ssd_centervol_NUM_OF_BITS                                    8
#define REG_Ssd_centervol_MIN                                            -84
#define REG_Ssd_centervol_MAX                                            24
#define REG_SSD_CENTERVOL_VALUE_MUTE                                     -84
#define REG_SSD_CENTERVOL_VALUE_23_DB                                    23
#define REG_SSD_CENTERVOL_VALUE_24_DB                                    24

/*!
 * @brief SURROUND Volume (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_surroundvol
#define REG_Ssd_surroundvol_OFFSET                                         0x000C0
#define REG_Ssd_surroundvol_MASK                                         0xff
#define REG_Ssd_surroundvol_SIGNED_BIT_MASK                              0x800000
#define REG_Ssd_surroundvol_BIT_MASK                                     0xff0000
#define REG_Ssd_surroundvol_BIT_IDX                                      16
#define REG_Ssd_surroundvol_NUM_OF_BITS                                  8
#define REG_Ssd_surroundvol_MIN                                          -84
#define REG_Ssd_surroundvol_MAX                                          24
#define REG_SSD_SURROUNDVOL_VALUE_MUTE                                   -84
#define REG_SSD_SURROUNDVOL_VALUE_23_DB                                  23
#define REG_SSD_SURROUNDVOL_VALUE_24_DB                                  24

/*!
 * @brief AUX1L Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_aux1voll
#define REG_Ssd_aux1voll_OFFSET                                            0x000C4
#define REG_Ssd_aux1voll_MASK                                            0xff
#define REG_Ssd_aux1voll_SIGNED_BIT_MASK                                 0x80
#define REG_Ssd_aux1voll_BIT_MASK                                        0xff
#define REG_Ssd_aux1voll_BIT_IDX                                         0
#define REG_Ssd_aux1voll_NUM_OF_BITS                                     8
#define REG_Ssd_aux1voll_MIN                                             -84
#define REG_Ssd_aux1voll_MAX                                             24
#define REG_SSD_AUX1VOLL_VALUE_MUTE                                      -84
#define REG_SSD_AUX1VOLL_VALUE_23_DB                                     23
#define REG_SSD_AUX1VOLL_VALUE_24_DB                                     24

/*!
 * @brief AUX1R Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_aux1volr
#define REG_Ssd_aux1volr_OFFSET                                            0x000C4
#define REG_Ssd_aux1volr_MASK                                            0xff
#define REG_Ssd_aux1volr_SIGNED_BIT_MASK                                 0x8000
#define REG_Ssd_aux1volr_BIT_MASK                                        0xff00
#define REG_Ssd_aux1volr_BIT_IDX                                         8
#define REG_Ssd_aux1volr_NUM_OF_BITS                                     8
#define REG_Ssd_aux1volr_MIN                                             -84
#define REG_Ssd_aux1volr_MAX                                             24
#define REG_SSD_AUX1VOLR_VALUE_MUTE                                      -84
#define REG_SSD_AUX1VOLR_VALUE_23_DB                                     23
#define REG_SSD_AUX1VOLR_VALUE_24_DB                                     24

/*!
 * @brief AUX2L Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_aux2voll
#define REG_Ssd_aux2voll_OFFSET                                            0x000C8
#define REG_Ssd_aux2voll_MASK                                            0xff
#define REG_Ssd_aux2voll_SIGNED_BIT_MASK                                 0x80
#define REG_Ssd_aux2voll_BIT_MASK                                        0xff
#define REG_Ssd_aux2voll_BIT_IDX                                         0
#define REG_Ssd_aux2voll_NUM_OF_BITS                                     8
#define REG_Ssd_aux2voll_MIN                                             -84
#define REG_Ssd_aux2voll_MAX                                             24
#define REG_SSD_AUX2VOLL_VALUE_MUTE                                      -84
#define REG_SSD_AUX2VOLL_VALUE_23_DB                                     23
#define REG_SSD_AUX2VOLL_VALUE_24_DB                                     24

/*!
 * @brief AUX2R Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_aux2volr
#define REG_Ssd_aux2volr_OFFSET                                            0x000C8
#define REG_Ssd_aux2volr_MASK                                            0xff
#define REG_Ssd_aux2volr_SIGNED_BIT_MASK                                 0x8000
#define REG_Ssd_aux2volr_BIT_MASK                                        0xff00
#define REG_Ssd_aux2volr_BIT_IDX                                         8
#define REG_Ssd_aux2volr_NUM_OF_BITS                                     8
#define REG_Ssd_aux2volr_MIN                                             -84
#define REG_Ssd_aux2volr_MAX                                             24
#define REG_SSD_AUX2VOLR_VALUE_MUTE                                      -84
#define REG_SSD_AUX2VOLR_VALUE_23_DB                                     23
#define REG_SSD_AUX2VOLR_VALUE_24_DB                                     24

/*!
 * @brief AUX3L Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_aux3voll
#define REG_Ssd_aux3voll_OFFSET                                            0x000CC
#define REG_Ssd_aux3voll_MASK                                            0xff
#define REG_Ssd_aux3voll_SIGNED_BIT_MASK                                 0x80
#define REG_Ssd_aux3voll_BIT_MASK                                        0xff
#define REG_Ssd_aux3voll_BIT_IDX                                         0
#define REG_Ssd_aux3voll_NUM_OF_BITS                                     8
#define REG_Ssd_aux3voll_MIN                                             -84
#define REG_Ssd_aux3voll_MAX                                             24
#define REG_SSD_AUX3VOLL_VALUE_MUTE                                      -84
#define REG_SSD_AUX3VOLL_VALUE_23_DB                                     23
#define REG_SSD_AUX3VOLL_VALUE_24_DB                                     24

/*!
 * @brief AUX3R Volume: (+24..-83dB, mute)
 *        24 = +24 dB
 *        23 = +23 dB
 *        ..
 *        -84 = mute
 * @range -84..24
 ***/
#define REG_Ssd_aux3volr
#define REG_Ssd_aux3volr_OFFSET                                            0x000CC
#define REG_Ssd_aux3volr_MASK                                            0xff
#define REG_Ssd_aux3volr_SIGNED_BIT_MASK                                 0x8000
#define REG_Ssd_aux3volr_BIT_MASK                                        0xff00
#define REG_Ssd_aux3volr_BIT_IDX                                         8
#define REG_Ssd_aux3volr_NUM_OF_BITS                                     8
#define REG_Ssd_aux3volr_MIN                                             -84
#define REG_Ssd_aux3volr_MAX                                             24
#define REG_SSD_AUX3VOLR_VALUE_MUTE                                      -84
#define REG_SSD_AUX3VOLR_VALUE_23_DB                                     23
#define REG_SSD_AUX3VOLR_VALUE_24_DB                                     24

/*!
 * @brief MAIN Bass: (+15..-16dB, 1 dB steps)
 *        15 = +15 dB
 *        ..
 *        -16 = -16 dB
 * @range -16..15
 ***/
#define REG_Ssd_mainbass
#define REG_Ssd_mainbass_OFFSET                                            0x000D0
#define REG_Ssd_mainbass_MASK                                            0x1f
#define REG_Ssd_mainbass_SIGNED_BIT_MASK                                 0x10
#define REG_Ssd_mainbass_BIT_MASK                                        0x1f
#define REG_Ssd_mainbass_BIT_IDX                                         0
#define REG_Ssd_mainbass_NUM_OF_BITS                                     5
#define REG_Ssd_mainbass_MIN                                             -16
#define REG_Ssd_mainbass_MAX                                             15
#define REG_SSD_MAINBASS_VALUE_MINUS_16_DB                               -16
#define REG_SSD_MAINBASS_VALUE_15_DB                                     15

/*!
 * @brief MAIN Treble: (+15..-16dB, 1 dB steps)
 *        15 = +15 dB
 *        ..
 *        -16 = -16 dB
 * @range -16..15
 ***/
#define REG_Ssd_maintreb
#define REG_Ssd_maintreb_OFFSET                                            0x000D0
#define REG_Ssd_maintreb_MASK                                            0x1f
#define REG_Ssd_maintreb_SIGNED_BIT_MASK                                 0x200
#define REG_Ssd_maintreb_BIT_MASK                                        0x3e0
#define REG_Ssd_maintreb_BIT_IDX                                         5
#define REG_Ssd_maintreb_NUM_OF_BITS                                     5
#define REG_Ssd_maintreb_MIN                                             -16
#define REG_Ssd_maintreb_MAX                                             15
#define REG_SSD_MAINTREB_VALUE_MINUS_16_DB                               -16
#define REG_SSD_MAINTREB_VALUE_15_DB                                     15

/*!
 * @brief CENTER Bass: (+15..-16dB, 1 dB steps)
 *        15 = +15 dB
 *        ..
 *        -16 = -16 dB
 * @range -16..15
 ***/
#define REG_Ssd_centerbass
#define REG_Ssd_centerbass_OFFSET                                          0x000D4
#define REG_Ssd_centerbass_MASK                                          0x1f
#define REG_Ssd_centerbass_SIGNED_BIT_MASK                               0x10
#define REG_Ssd_centerbass_BIT_MASK                                      0x1f
#define REG_Ssd_centerbass_BIT_IDX                                       0
#define REG_Ssd_centerbass_NUM_OF_BITS                                   5
#define REG_Ssd_centerbass_MIN                                           -16
#define REG_Ssd_centerbass_MAX                                           15
#define REG_SSD_CENTERBASS_VALUE_MINUS_16_DB                             -16
#define REG_SSD_CENTERBASS_VALUE_15_DB                                   15

/*!
 * @brief CENTER Treble: (+15..-16dB, 1 dB steps)
 *        15 = +15 dB
 *        ..
 *        -16 = -16 dB
 * @range -16..15
 ***/
#define REG_Ssd_centertreb
#define REG_Ssd_centertreb_OFFSET                                          0x000D4
#define REG_Ssd_centertreb_MASK                                          0x1f
#define REG_Ssd_centertreb_SIGNED_BIT_MASK                               0x200
#define REG_Ssd_centertreb_BIT_MASK                                      0x3e0
#define REG_Ssd_centertreb_BIT_IDX                                       5
#define REG_Ssd_centertreb_NUM_OF_BITS                                   5
#define REG_Ssd_centertreb_MIN                                           -16
#define REG_Ssd_centertreb_MAX                                           15
#define REG_SSD_CENTERTREB_VALUE_MINUS_16_DB                             -16
#define REG_SSD_CENTERTREB_VALUE_15_DB                                   15

/*!
 * @brief SURROUND Bass: (+15..-16dB, 1 dB steps)
 *        15 = +15 dB
 *        ..
 *        -16 = -16 dB
 * @range -16..15
 ***/
#define REG_Ssd_surroundbass
#define REG_Ssd_surroundbass_OFFSET                                        0x000D8
#define REG_Ssd_surroundbass_MASK                                        0x1f
#define REG_Ssd_surroundbass_SIGNED_BIT_MASK                             0x10
#define REG_Ssd_surroundbass_BIT_MASK                                    0x1f
#define REG_Ssd_surroundbass_BIT_IDX                                     0
#define REG_Ssd_surroundbass_NUM_OF_BITS                                 5
#define REG_Ssd_surroundbass_MIN                                         -16
#define REG_Ssd_surroundbass_MAX                                         15
#define REG_SSD_SURROUNDBASS_VALUE_MINUS_16_DB                           -16
#define REG_SSD_SURROUNDBASS_VALUE_15_DB                                 15

/*!
 * @brief SURROUND Treble: (+15..-16dB, 1 dB steps)
 *        15 = +15 dB
 *        ..
 *        -16 = -16 dB
 * @range -16..15
 ***/
#define REG_Ssd_surroundtreb
#define REG_Ssd_surroundtreb_OFFSET                                        0x000D8
#define REG_Ssd_surroundtreb_MASK                                        0x1f
#define REG_Ssd_surroundtreb_SIGNED_BIT_MASK                             0x200
#define REG_Ssd_surroundtreb_BIT_MASK                                    0x3e0
#define REG_Ssd_surroundtreb_BIT_IDX                                     5
#define REG_Ssd_surroundtreb_NUM_OF_BITS                                 5
#define REG_Ssd_surroundtreb_MIN                                         -16
#define REG_Ssd_surroundtreb_MAX                                         15
#define REG_SSD_SURROUNDTREB_VALUE_MINUS_16_DB                           -16
#define REG_SSD_SURROUNDTREB_VALUE_15_DB                                 15

/*!
 * @brief MAIN Equalizer Band 1 Control (100 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchm1
#define REG_Ssd_eqchm1_OFFSET                                              0x000DC
#define REG_Ssd_eqchm1_MASK                                              0x1f
#define REG_Ssd_eqchm1_SIGNED_BIT_MASK                                   0x10
#define REG_Ssd_eqchm1_BIT_MASK                                          0x1f
#define REG_Ssd_eqchm1_BIT_IDX                                           0
#define REG_Ssd_eqchm1_NUM_OF_BITS                                       5
#define REG_Ssd_eqchm1_MIN                                               -12
#define REG_Ssd_eqchm1_MAX                                               12
#define REG_SSD_EQCHM1_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHM1_VALUE_12DB                                        12

/*!
 * @brief MAIN Equalizer Band 2 Control (300 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchm2
#define REG_Ssd_eqchm2_OFFSET                                              0x000DC
#define REG_Ssd_eqchm2_MASK                                              0x1f
#define REG_Ssd_eqchm2_SIGNED_BIT_MASK                                   0x200
#define REG_Ssd_eqchm2_BIT_MASK                                          0x3e0
#define REG_Ssd_eqchm2_BIT_IDX                                           5
#define REG_Ssd_eqchm2_NUM_OF_BITS                                       5
#define REG_Ssd_eqchm2_MIN                                               -12
#define REG_Ssd_eqchm2_MAX                                               12
#define REG_SSD_EQCHM2_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHM2_VALUE_12DB                                        12

/*!
 * @brief MAIN Equalizer Band 3 Control (1000 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchm3
#define REG_Ssd_eqchm3_OFFSET                                              0x000DC
#define REG_Ssd_eqchm3_MASK                                              0x1f
#define REG_Ssd_eqchm3_SIGNED_BIT_MASK                                   0x4000
#define REG_Ssd_eqchm3_BIT_MASK                                          0x7c00
#define REG_Ssd_eqchm3_BIT_IDX                                           10
#define REG_Ssd_eqchm3_NUM_OF_BITS                                       5
#define REG_Ssd_eqchm3_MIN                                               -12
#define REG_Ssd_eqchm3_MAX                                               12
#define REG_SSD_EQCHM3_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHM3_VALUE_12DB                                        12

/*!
 * @brief MAIN Equalizer Band 4 Control (3000 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchm4
#define REG_Ssd_eqchm4_OFFSET                                              0x000E0
#define REG_Ssd_eqchm4_MASK                                              0x1f
#define REG_Ssd_eqchm4_SIGNED_BIT_MASK                                   0x10
#define REG_Ssd_eqchm4_BIT_MASK                                          0x1f
#define REG_Ssd_eqchm4_BIT_IDX                                           0
#define REG_Ssd_eqchm4_NUM_OF_BITS                                       5
#define REG_Ssd_eqchm4_MIN                                               -12
#define REG_Ssd_eqchm4_MAX                                               12
#define REG_SSD_EQCHM4_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHM4_VALUE_12DB                                        12

/*!
 * @brief MAIN Equalizer Band 5 Control (8000 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchm5
#define REG_Ssd_eqchm5_OFFSET                                              0x000E0
#define REG_Ssd_eqchm5_MASK                                              0x1f
#define REG_Ssd_eqchm5_SIGNED_BIT_MASK                                   0x200
#define REG_Ssd_eqchm5_BIT_MASK                                          0x3e0
#define REG_Ssd_eqchm5_BIT_IDX                                           5
#define REG_Ssd_eqchm5_NUM_OF_BITS                                       5
#define REG_Ssd_eqchm5_MIN                                               -12
#define REG_Ssd_eqchm5_MAX                                               12
#define REG_SSD_EQCHM5_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHM5_VALUE_12DB                                        12

/*!
 * @brief CENTER Equalizer Band 1 Control (100 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchc1
#define REG_Ssd_eqchc1_OFFSET                                              0x000E4
#define REG_Ssd_eqchc1_MASK                                              0x1f
#define REG_Ssd_eqchc1_SIGNED_BIT_MASK                                   0x10
#define REG_Ssd_eqchc1_BIT_MASK                                          0x1f
#define REG_Ssd_eqchc1_BIT_IDX                                           0
#define REG_Ssd_eqchc1_NUM_OF_BITS                                       5
#define REG_Ssd_eqchc1_MIN                                               -12
#define REG_Ssd_eqchc1_MAX                                               12
#define REG_SSD_EQCHC1_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHC1_VALUE_12DB                                        12

/*!
 * @brief CENTER Equalizer Band 2 Control (300 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchc2
#define REG_Ssd_eqchc2_OFFSET                                              0x000E4
#define REG_Ssd_eqchc2_MASK                                              0x1f
#define REG_Ssd_eqchc2_SIGNED_BIT_MASK                                   0x200
#define REG_Ssd_eqchc2_BIT_MASK                                          0x3e0
#define REG_Ssd_eqchc2_BIT_IDX                                           5
#define REG_Ssd_eqchc2_NUM_OF_BITS                                       5
#define REG_Ssd_eqchc2_MIN                                               -12
#define REG_Ssd_eqchc2_MAX                                               12
#define REG_SSD_EQCHC2_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHC2_VALUE_12DB                                        12

/*!
 * @brief CENTER Equalizer Band 3 Control (1000 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchc3
#define REG_Ssd_eqchc3_OFFSET                                              0x000E4
#define REG_Ssd_eqchc3_MASK                                              0x1f
#define REG_Ssd_eqchc3_SIGNED_BIT_MASK                                   0x4000
#define REG_Ssd_eqchc3_BIT_MASK                                          0x7c00
#define REG_Ssd_eqchc3_BIT_IDX                                           10
#define REG_Ssd_eqchc3_NUM_OF_BITS                                       5
#define REG_Ssd_eqchc3_MIN                                               -12
#define REG_Ssd_eqchc3_MAX                                               12
#define REG_SSD_EQCHC3_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHC3_VALUE_12DB                                        12

/*!
 * @brief CENTER Equalizer Band 3 Control (3000 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchc4
#define REG_Ssd_eqchc4_OFFSET                                              0x000E8
#define REG_Ssd_eqchc4_MASK                                              0x1f
#define REG_Ssd_eqchc4_SIGNED_BIT_MASK                                   0x10
#define REG_Ssd_eqchc4_BIT_MASK                                          0x1f
#define REG_Ssd_eqchc4_BIT_IDX                                           0
#define REG_Ssd_eqchc4_NUM_OF_BITS                                       5
#define REG_Ssd_eqchc4_MIN                                               -12
#define REG_Ssd_eqchc4_MAX                                               12
#define REG_SSD_EQCHC4_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHC4_VALUE_12DB                                        12

/*!
 * @brief CENTER Equalizer Band 5 Control (8000 Hz)
 *        12 = +12dB
 *        ..
 *        -12 = -12dB")
 * @range -12..12
 ***/
#define REG_Ssd_eqchc5
#define REG_Ssd_eqchc5_OFFSET                                              0x000E8
#define REG_Ssd_eqchc5_MASK                                              0x1f
#define REG_Ssd_eqchc5_SIGNED_BIT_MASK                                   0x200
#define REG_Ssd_eqchc5_BIT_MASK                                          0x3e0
#define REG_Ssd_eqchc5_BIT_IDX                                           5
#define REG_Ssd_eqchc5_NUM_OF_BITS                                       5
#define REG_Ssd_eqchc5_MIN                                               -12
#define REG_Ssd_eqchc5_MAX                                               12
#define REG_SSD_EQCHC5_VALUE_MINUS_12DB                                  -12
#define REG_SSD_EQCHC5_VALUE_12DB                                        12

/*!
 * @brief ACC Coefficient Address
 * @range 0..4095
 ***/
#define REG_Ssd_accadr
#define REG_Ssd_accadr_OFFSET                                              0x000EC
#define REG_Ssd_accadr_MASK                                              0xfff
#define REG_Ssd_accadr_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_accadr_BIT_MASK                                          0xfff
#define REG_Ssd_accadr_BIT_IDX                                           0
#define REG_Ssd_accadr_NUM_OF_BITS                                       12
#define REG_Ssd_accadr_MIN                                               0
#define REG_Ssd_accadr_MAX                                               4095

/*!
 * @brief ACC Coefficients
 * @range -2048..2047
 ***/
#define REG_Ssd_acccoef
#define REG_Ssd_acccoef_OFFSET                                             0x000EC
#define REG_Ssd_acccoef_MASK                                             0xfff
#define REG_Ssd_acccoef_SIGNED_BIT_MASK                                  0x800000
#define REG_Ssd_acccoef_BIT_MASK                                         0xfff000
#define REG_Ssd_acccoef_BIT_IDX                                          12
#define REG_Ssd_acccoef_NUM_OF_BITS                                      12
#define REG_Ssd_acccoef_MIN                                              -2048
#define REG_Ssd_acccoef_MAX                                              2047

/*!
 * @brief source for monitor function
 *        00 = FM,AM,MPX (1 fs) input
 *        01 = FM,AM,MPX (4 fs) input
 *        02 = FM/AM/BTSC/EIAJ DC
 *        03 = FM dematrix output
 *        04 = NICAM before softmute
 *        05 = MONO before softmute
 *        06 = SAP before softmute
 *        07 = ADC  (at dig. input crossbar)
 *        08 = Noise / silence generator (at dig. input crossbar)
 *        09 = SRC1 (at dig. input crossbar)
 *        10 = SRC2 (at dig. input crossbar)
 *        11 = MAIN (at dig. output crossbar)
 *        12 = SUBWOOFER (at dig. output crossbar)
 *        13 = CENTER/SURROUND (at dig. output crossbar)
 *        14 = SURROUND/AUX1L (at dig. output crossbar)
 *        15 = AUX1L,R (at dig. output crossbar)
 *        16 = AUX2L,R (at dig. output crossbar)
 *        17 = AUX3L,R (at dig. output crossbar)
 *        18 = MAIN SUM/Silence (at dig. output crossbar)
 *        19 = MAIN L,R (after Bass Management)
 *        20 = SUBWOOFER/CENTER (after Bass Management)
 *        21 = CENTER/SURROUND (after Bass Management)
 * @range 0..23
 ***/
#define REG_Ssd_mon_src
#define REG_Ssd_mon_src_OFFSET                                             0x000F0
#define REG_Ssd_mon_src_MASK                                             0x1f
#define REG_Ssd_mon_src_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_mon_src_BIT_MASK                                         0x1f
#define REG_Ssd_mon_src_BIT_IDX                                          0
#define REG_Ssd_mon_src_NUM_OF_BITS                                      5
#define REG_Ssd_mon_src_MIN                                              0
#define REG_Ssd_mon_src_MAX                                              23
#define REG_SSD_MON_SRC_VALUE_FM_AM_MPX_1_FS_INPUT                       0
#define REG_SSD_MON_SRC_VALUE_FM_AM_MPX_4_FS_INPUT                       1
#define REG_SSD_MON_SRC_VALUE_FM_AM_BTSC_EIAJ_DC                         2
#define REG_SSD_MON_SRC_VALUE_FM_DEMATRIX_OUTPUT                         3
#define REG_SSD_MON_SRC_VALUE_NICAM_BEFORE_SOFTMUTE                      4
#define REG_SSD_MON_SRC_VALUE_MONO_BEFORE_SOFTMUTE                       5
#define REG_SSD_MON_SRC_VALUE_SAP_BEFORE_SOFTMUTE                        6
#define REG_SSD_MON_SRC_VALUE_ADC_AT_DIG_INPUT_CROSSBAR                  7
#define REG_SSD_MON_SRC_VALUE_NOISE_SILENCE_GENERATOR_AT_DIG_INPUT_CROSSBAR 8
#define REG_SSD_MON_SRC_VALUE_SRC1_AT_DIG_INPUT_CROSSBAR                 9
#define REG_SSD_MON_SRC_VALUE_SRC2_AT_DIG_INPUT_CROSSBAR                 10
#define REG_SSD_MON_SRC_VALUE_MAIN_AT_DIG_OUTPUT_CROSSBAR                11
#define REG_SSD_MON_SRC_VALUE_SUBWOOFER_AT_DIG_OUTPUT_CROSSBAR           12
#define REG_SSD_MON_SRC_VALUE_CENTER_SURROUND_AT_DIG_OUTPUT_CROSSBAR     13
#define REG_SSD_MON_SRC_VALUE_SURROUND_AUX1L_AT_DIG_OUTPUT_CROSSBAR      14
#define REG_SSD_MON_SRC_VALUE_AUX1L_R_AT_DIG_OUTPUT_CROSSBAR             15
#define REG_SSD_MON_SRC_VALUE_AUX2L_R_AT_DIG_OUTPUT_CROSSBAR             16
#define REG_SSD_MON_SRC_VALUE_AUX3L_R_AT_DIG_OUTPUT_CROSSBAR             17
#define REG_SSD_MON_SRC_VALUE_MAIN_SUM_SILENCE_AT_DIG_OUTPUT_CROSSBAR    18
#define REG_SSD_MON_SRC_VALUE_MAIN_L_R_AFTER_BASS_MANAGEMENT             19
#define REG_SSD_MON_SRC_VALUE_SUBWOOFER_CENTER_AFTER_BASS_MANAGEMENT     20
#define REG_SSD_MON_SRC_VALUE_CENTER_SURROUND_AFTER_BASS_MANAGEMENT      21

/*!
 * @brief detection type for monitor function
 *        0 = random samples
 *        1 = absolute value peak detection
 *        2 = quasi peak detection
 *        3 = off / reset peak detector
 * @range 0..3
 ***/
#define REG_Ssd_mon_det
#define REG_Ssd_mon_det_OFFSET                                             0x000F0
#define REG_Ssd_mon_det_MASK                                             0x3
#define REG_Ssd_mon_det_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_mon_det_BIT_MASK                                         0x60
#define REG_Ssd_mon_det_BIT_IDX                                          5
#define REG_Ssd_mon_det_NUM_OF_BITS                                      2
#define REG_Ssd_mon_det_MIN                                              0
#define REG_Ssd_mon_det_MAX                                              3
#define REG_SSD_MON_DET_VALUE_RANDOM_SAMPLES                             0
#define REG_SSD_MON_DET_VALUE_ABSOLUTE_VALUE_PEAK_DETECTION              1
#define REG_SSD_MON_DET_VALUE_QUASI_PEAK_DETECTION                       2
#define REG_SSD_MON_DET_VALUE_OFF_RESET_PEAK_DETECTOR                    3

/*!
 * @brief matrix for monitor source
 *        0 = A
 *        1 = (A+B)/2
 *        2 = B
 *        3 = (A-B)/2 (2-ch. sources only)
 * @range 0..3
 ***/
#define REG_Ssd_mon_mat
#define REG_Ssd_mon_mat_OFFSET                                             0x000F0
#define REG_Ssd_mon_mat_MASK                                             0x7
#define REG_Ssd_mon_mat_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_mon_mat_BIT_MASK                                         0x380
#define REG_Ssd_mon_mat_BIT_IDX                                          7
#define REG_Ssd_mon_mat_NUM_OF_BITS                                      3
#define REG_Ssd_mon_mat_MIN                                              0
#define REG_Ssd_mon_mat_MAX                                              3
#define REG_SSD_MON_MAT_VALUE_A                                          0
#define REG_SSD_MON_MAT_VALUE_A_B_2                                      1
#define REG_SSD_MON_MAT_VALUE_B                                          2
#define REG_SSD_MON_MAT_VALUE_A_B_2_2_CH_SOURCES_ONLY                    3

/*!
 * @brief deemphasis for digital audio input (I2S or SPDIF)
 *        0=no deemphasis
 *        1=50/15 us deemphasis for 32 kHz sample rate
 *        2=50/15 us deemphasis for 44.1 kHz sample rate
 *        3=50/15 us deemphasis for 48 kHz sample rate
 *        4=J17 deemphasis for 32 kHz sample rate
 *        5=J17 deemphasis for 44.1 kHz sample rate
 *        6=J17 deemphasis for 48 kHz sample rate
 * @range 0..7
 ***/
#define REG_Ssd_digin_deem
#define REG_Ssd_digin_deem_OFFSET                                          0x000F4
#define REG_Ssd_digin_deem_MASK                                          0x7
#define REG_Ssd_digin_deem_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_digin_deem_BIT_MASK                                      0x7
#define REG_Ssd_digin_deem_BIT_IDX                                       0
#define REG_Ssd_digin_deem_NUM_OF_BITS                                   3
#define REG_Ssd_digin_deem_MIN                                           0
#define REG_Ssd_digin_deem_MAX                                           7
#define REG_SSD_DIGIN_DEEM_VALUE_NO_DEEMPHASIS                           0
#define REG_SSD_DIGIN_DEEM_VALUE_50_15_US_DEEMPHASIS_FOR_32_KHZ_SAMPLE_RATE 1
#define REG_SSD_DIGIN_DEEM_VALUE_50_15_US_DEEMPHASIS_FOR_44_1_KHZ_SAMPLE_RATE 2
#define REG_SSD_DIGIN_DEEM_VALUE_50_15_US_DEEMPHASIS_FOR_48_KHZ_SAMPLE_RATE 3
#define REG_SSD_DIGIN_DEEM_VALUE_J17_DEEMPHASIS_FOR_32_KHZ_SAMPLE_RATE   4
#define REG_SSD_DIGIN_DEEM_VALUE_J17_DEEMPHASIS_FOR_44_1_KHZ_SAMPLE_RATE 5
#define REG_SSD_DIGIN_DEEM_VALUE_J17_DEEMPHASIS_FOR_48_KHZ_SAMPLE_RATE   6

/*!
 * @brief clear SPDIF parity error counter SPDIF_ERRCNT
 *        0=counter not cleared
 *        1=counter cleared
 * @range 0..1
 ***/
#define REG_Ssd_spdif_errclr
#define REG_Ssd_spdif_errclr_OFFSET                                        0x000F4
#define REG_Ssd_spdif_errclr_MASK                                        0x1
#define REG_Ssd_spdif_errclr_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_spdif_errclr_BIT_MASK                                    0x10
#define REG_Ssd_spdif_errclr_BIT_IDX                                     4
#define REG_Ssd_spdif_errclr_NUM_OF_BITS                                 1
#define REG_Ssd_spdif_errclr_MIN                                         0
#define REG_Ssd_spdif_errclr_MAX                                         1
#define REG_SSD_SPDIF_ERRCLR_VALUE_COUNTER_NOT_CLEARED                   0
#define REG_SSD_SPDIF_ERRCLR_VALUE_COUNTER_CLEARED                       1

/*!
 * @brief enforce SPDIF audio processing if non-audio signalled by SPDIF
 *        0=auto-mute on
 *        1=auto-mute off
 * @range 0..1
 ***/
#define REG_Ssd_spdif_forceaudio
#define REG_Ssd_spdif_forceaudio_OFFSET                                    0x000F4
#define REG_Ssd_spdif_forceaudio_MASK                                    0x1
#define REG_Ssd_spdif_forceaudio_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_spdif_forceaudio_BIT_MASK                                0x20
#define REG_Ssd_spdif_forceaudio_BIT_IDX                                 5
#define REG_Ssd_spdif_forceaudio_NUM_OF_BITS                             1
#define REG_Ssd_spdif_forceaudio_MIN                                     0
#define REG_Ssd_spdif_forceaudio_MAX                                     1
#define REG_SSD_SPDIF_FORCEAUDIO_VALUE_AUTO_MUTE_ON                      0
#define REG_SSD_SPDIF_FORCEAUDIO_VALUE_AUTO_MUTE_OFF                     1

/*!
 * @brief Processing mode of FM Identification (B/G A2, D/K A2, M Korea, M EIAJ)
 *        0=Hardware only (HW-IDENT)
 *        1=DSP-firmware only (FlexIdent)
 *        2=OR combination of HW-IDENT and FlexIdent
 *        3=AND combination of HW-IDENT and FlexIdent
 * @range 0..3
 ***/
#define REG_Ssd_idjoinmode
#define REG_Ssd_idjoinmode_OFFSET                                          0x000F8
#define REG_Ssd_idjoinmode_MASK                                          0x3
#define REG_Ssd_idjoinmode_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_idjoinmode_BIT_MASK                                      0x3
#define REG_Ssd_idjoinmode_BIT_IDX                                       0
#define REG_Ssd_idjoinmode_NUM_OF_BITS                                   2
#define REG_Ssd_idjoinmode_MIN                                           0
#define REG_Ssd_idjoinmode_MAX                                           3
#define REG_SSD_IDJOINMODE_VALUE_HARDWARE_ONLY_HW_IDENT                  0
#define REG_SSD_IDJOINMODE_VALUE_DSP_FIRMWARE_ONLY_FLEXIDENT             1
#define REG_SSD_IDJOINMODE_VALUE_OR_COMBINATION_OF_HW_IDENT_AND_FLEXIDENT 2
#define REG_SSD_IDJOINMODE_VALUE_AND_COMBINATION_OF_HW_IDENT_AND_FLEXIDENT 3

/*!
 * @brief FlexIdent coefficient index (0=no change; details see app note)
 * @range 0..63
 ***/
#define REG_Ssd_flidcoefidx
#define REG_Ssd_flidcoefidx_OFFSET                                         0x000F8
#define REG_Ssd_flidcoefidx_MASK                                         0x3f
#define REG_Ssd_flidcoefidx_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_flidcoefidx_BIT_MASK                                     0x3f0
#define REG_Ssd_flidcoefidx_BIT_IDX                                      4
#define REG_Ssd_flidcoefidx_NUM_OF_BITS                                  6
#define REG_Ssd_flidcoefidx_MIN                                          0
#define REG_Ssd_flidcoefidx_MAX                                          63

/*!
 * @brief Standard for FlexIdent (DemDec expert mode only)
 *        0=off
 *        1=Europe
 *        2=Korea
 *        3=Japan
 *        4=BTSC stereo pilot
 *        5=FM Radio stereo pilot
 * @range 0..7
 ***/
#define REG_Ssd_flidstd
#define REG_Ssd_flidstd_OFFSET                                             0x000F8
#define REG_Ssd_flidstd_MASK                                             0x7
#define REG_Ssd_flidstd_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_flidstd_BIT_MASK                                         0x7000
#define REG_Ssd_flidstd_BIT_IDX                                          12
#define REG_Ssd_flidstd_NUM_OF_BITS                                      3
#define REG_Ssd_flidstd_MIN                                              0
#define REG_Ssd_flidstd_MAX                                              7
#define REG_SSD_FLIDSTD_VALUE_OFF                                        0
#define REG_SSD_FLIDSTD_VALUE_EUROPE                                     1
#define REG_SSD_FLIDSTD_VALUE_KOREA                                      2
#define REG_SSD_FLIDSTD_VALUE_JAPAN                                      3
#define REG_SSD_FLIDSTD_VALUE_BTSC_STEREO_PILOT                          4
#define REG_SSD_FLIDSTD_VALUE_FM_RADIO_STEREO_PILOT                      5

/*!
 * @brief FlexIdent coefficient data (24 or 12 bit), copied if FLIDCOEFIDX is nonzero
 * @range 0..16777215
 ***/
#define REG_Ssd_flidcoefdata
#define REG_Ssd_flidcoefdata_OFFSET                                        0x000FC
#define REG_Ssd_flidcoefdata_MASK                                        0xffffff
#define REG_Ssd_flidcoefdata_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_flidcoefdata_BIT_MASK                                    0xffffff
#define REG_Ssd_flidcoefdata_BIT_IDX                                     0
#define REG_Ssd_flidcoefdata_NUM_OF_BITS                                 24
#define REG_Ssd_flidcoefdata_MIN                                         0
#define REG_Ssd_flidcoefdata_MAX                                         16777215

/*!
 * @brief pilot frequency offset to nominal
 * @range -2048..2047
 ***/
#define REG_Ssd_flid_pilot_offset
#define REG_Ssd_flid_pilot_offset_OFFSET                                   0x00100
#define REG_Ssd_flid_pilot_offset_MASK                                   0xfff
#define REG_Ssd_flid_pilot_offset_SIGNED_BIT_MASK                        0x800
#define REG_Ssd_flid_pilot_offset_BIT_MASK                               0xfff
#define REG_Ssd_flid_pilot_offset_BIT_IDX                                0
#define REG_Ssd_flid_pilot_offset_NUM_OF_BITS                            12
#define REG_Ssd_flid_pilot_offset_MIN                                    -2048
#define REG_Ssd_flid_pilot_offset_MAX                                    2047

/*!
 * @brief level detection by pilot PLL
 * @range -2048..2047
 ***/
#define REG_Ssd_flid_pilot_level
#define REG_Ssd_flid_pilot_level_OFFSET                                    0x00100
#define REG_Ssd_flid_pilot_level_MASK                                    0xfff
#define REG_Ssd_flid_pilot_level_SIGNED_BIT_MASK                         0x800000
#define REG_Ssd_flid_pilot_level_BIT_MASK                                0xfff000
#define REG_Ssd_flid_pilot_level_BIT_IDX                                 12
#define REG_Ssd_flid_pilot_level_NUM_OF_BITS                             12
#define REG_Ssd_flid_pilot_level_MIN                                     -2048
#define REG_Ssd_flid_pilot_level_MAX                                     2047

/*!
 * @brief enable silence detection for MAIN left/right channels
 *        0=disabled
 *        1=enabled
 * @range 0..1
 ***/
#define REG_Ssd_sild_en_main
#define REG_Ssd_sild_en_main_OFFSET                                        0x00104
#define REG_Ssd_sild_en_main_MASK                                        0x1
#define REG_Ssd_sild_en_main_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_sild_en_main_BIT_MASK                                    0x1
#define REG_Ssd_sild_en_main_BIT_IDX                                     0
#define REG_Ssd_sild_en_main_NUM_OF_BITS                                 1
#define REG_Ssd_sild_en_main_MIN                                         0
#define REG_Ssd_sild_en_main_MAX                                         1
#define REG_SSD_SILD_EN_MAIN_VALUE_DISABLED                              0
#define REG_SSD_SILD_EN_MAIN_VALUE_ENABLED                               1

/*!
 * @brief silence detection threshold in 3 dB steps (MAIN)
 *        0=-30 dBFS
 *        1=-33 dBFS
 *        2=-36 dBFS
 *        5=-45 dBFS
 *        10=-60 dBFS
 *        15=-75 dBFS
 *        20=-90 dBFS
 *        30=-120 dBFS
 * @range 0..31
 ***/
#define REG_Ssd_sild_thr_main
#define REG_Ssd_sild_thr_main_OFFSET                                       0x00104
#define REG_Ssd_sild_thr_main_MASK                                       0x1f
#define REG_Ssd_sild_thr_main_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sild_thr_main_BIT_MASK                                   0x3e
#define REG_Ssd_sild_thr_main_BIT_IDX                                    1
#define REG_Ssd_sild_thr_main_NUM_OF_BITS                                5
#define REG_Ssd_sild_thr_main_MIN                                        0
#define REG_Ssd_sild_thr_main_MAX                                        31
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_30_DBFS                        0
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_33_DBFS                        1
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_36_DBFS                        2
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_45_DBFS                        5
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_60_DBFS                        10
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_75_DBFS                        15
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_90_DBFS                        20
#define REG_SSD_SILD_THR_MAIN_VALUE_MINUS_120_DBFS                       30

/*!
 * @brief delay (in samples) of soft-mute after silence detected (MAIN)
 *        0=32 samples
 *        1=64 samples
 *        2=128 samples
 *        3=256 samples
 *        5=1024 samples
 *        8=8192 samples
 *        10=32768 samples
 * @range 0..15
 ***/
#define REG_Ssd_sild_delay_main
#define REG_Ssd_sild_delay_main_OFFSET                                     0x00104
#define REG_Ssd_sild_delay_main_MASK                                     0xf
#define REG_Ssd_sild_delay_main_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_sild_delay_main_BIT_MASK                                 0x3c0
#define REG_Ssd_sild_delay_main_BIT_IDX                                  6
#define REG_Ssd_sild_delay_main_NUM_OF_BITS                              4
#define REG_Ssd_sild_delay_main_MIN                                      0
#define REG_Ssd_sild_delay_main_MAX                                      15
#define REG_SSD_SILD_DELAY_MAIN_VALUE_32_SAMPLES                         0
#define REG_SSD_SILD_DELAY_MAIN_VALUE_64_SAMPLES                         1
#define REG_SSD_SILD_DELAY_MAIN_VALUE_128_SAMPLES                        2
#define REG_SSD_SILD_DELAY_MAIN_VALUE_256_SAMPLES                        3
#define REG_SSD_SILD_DELAY_MAIN_VALUE_1024_SAMPLES                       5
#define REG_SSD_SILD_DELAY_MAIN_VALUE_8192_SAMPLES                       8
#define REG_SSD_SILD_DELAY_MAIN_VALUE_32768_SAMPLES                      10

/*!
 * @brief enable silence detection for AUX1 left/right channels
 *        0=disabled
 *        1=enabled
 * @range 0..1
 ***/
#define REG_Ssd_sild_en_aux1
#define REG_Ssd_sild_en_aux1_OFFSET                                        0x00104
#define REG_Ssd_sild_en_aux1_MASK                                        0x1
#define REG_Ssd_sild_en_aux1_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_sild_en_aux1_BIT_MASK                                    0x1000
#define REG_Ssd_sild_en_aux1_BIT_IDX                                     12
#define REG_Ssd_sild_en_aux1_NUM_OF_BITS                                 1
#define REG_Ssd_sild_en_aux1_MIN                                         0
#define REG_Ssd_sild_en_aux1_MAX                                         1
#define REG_SSD_SILD_EN_AUX1_VALUE_DISABLED                              0
#define REG_SSD_SILD_EN_AUX1_VALUE_ENABLED                               1

/*!
 * @brief silence detection threshold in 3 dB steps (AUX1)
 *        0=-30 dBFS
 *        1=-33 dBFS
 *        2=-36 dBFS
 *        5=-45 dBFS
 *        10=-60 dBFS
 *        15=-75 dBFS
 *        20=-90 dBFS
 *        30=-120 dBFS
 * @range 0..31
 ***/
#define REG_Ssd_sild_thr_aux1
#define REG_Ssd_sild_thr_aux1_OFFSET                                       0x00104
#define REG_Ssd_sild_thr_aux1_MASK                                       0x1f
#define REG_Ssd_sild_thr_aux1_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sild_thr_aux1_BIT_MASK                                   0x3e000
#define REG_Ssd_sild_thr_aux1_BIT_IDX                                    13
#define REG_Ssd_sild_thr_aux1_NUM_OF_BITS                                5
#define REG_Ssd_sild_thr_aux1_MIN                                        0
#define REG_Ssd_sild_thr_aux1_MAX                                        31
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_30_DBFS                        0
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_33_DBFS                        1
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_36_DBFS                        2
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_45_DBFS                        5
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_60_DBFS                        10
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_75_DBFS                        15
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_90_DBFS                        20
#define REG_SSD_SILD_THR_AUX1_VALUE_MINUS_120_DBFS                       30

/*!
 * @brief delay (in samples) of soft-mute after silence detected (AUX1)
 *        0=32 samples
 *        1=64 samples
 *        2=128 samples
 *        3=256 samples
 *        5=1024 samples
 *        8=8192 samples
 *        10=32768 samples
 * @range 0..15
 ***/
#define REG_Ssd_sild_delay_aux1
#define REG_Ssd_sild_delay_aux1_OFFSET                                     0x00104
#define REG_Ssd_sild_delay_aux1_MASK                                     0xf
#define REG_Ssd_sild_delay_aux1_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_sild_delay_aux1_BIT_MASK                                 0x3c0000
#define REG_Ssd_sild_delay_aux1_BIT_IDX                                  18
#define REG_Ssd_sild_delay_aux1_NUM_OF_BITS                              4
#define REG_Ssd_sild_delay_aux1_MIN                                      0
#define REG_Ssd_sild_delay_aux1_MAX                                      15
#define REG_SSD_SILD_DELAY_AUX1_VALUE_32_SAMPLES                         0
#define REG_SSD_SILD_DELAY_AUX1_VALUE_64_SAMPLES                         1
#define REG_SSD_SILD_DELAY_AUX1_VALUE_128_SAMPLES                        2
#define REG_SSD_SILD_DELAY_AUX1_VALUE_256_SAMPLES                        3
#define REG_SSD_SILD_DELAY_AUX1_VALUE_1024_SAMPLES                       5
#define REG_SSD_SILD_DELAY_AUX1_VALUE_8192_SAMPLES                       8
#define REG_SSD_SILD_DELAY_AUX1_VALUE_32768_SAMPLES                      10

/*!
 * @brief contains address of last access via IIC interface
 * @range 0..65535
 ***/
#define REG_Ssd_xr_jtaddr
#define REG_Ssd_xr_jtaddr_OFFSET                                           0x03FC0
#define REG_Ssd_xr_jtaddr_MASK                                           0xffff
#define REG_Ssd_xr_jtaddr_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_jtaddr_BIT_MASK                                       0xffff
#define REG_Ssd_xr_jtaddr_BIT_IDX                                        0
#define REG_Ssd_xr_jtaddr_NUM_OF_BITS                                    16
#define REG_Ssd_xr_jtaddr_MIN                                            0
#define REG_Ssd_xr_jtaddr_MAX                                            65535

/*!
 * @brief Reserved
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_jta_6
#define REG_Ssd_rsd_xreg_jta_6_OFFSET                                      0x03FC0
#define REG_Ssd_rsd_xreg_jta_6_MASK                                      0xff
#define REG_Ssd_rsd_xreg_jta_6_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_rsd_xreg_jta_6_BIT_MASK                                  0xff0000
#define REG_Ssd_rsd_xreg_jta_6_BIT_IDX                                   16
#define REG_Ssd_rsd_xreg_jta_6_NUM_OF_BITS                               8
#define REG_Ssd_rsd_xreg_jta_6_MIN                                       0
#define REG_Ssd_rsd_xreg_jta_6_MAX                                       255

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_jta_7
#define REG_Ssd_rsd_xreg_jta_7_OFFSET                                      0x03FC0
#define REG_Ssd_rsd_xreg_jta_7_MASK                                      0xff
#define REG_Ssd_rsd_xreg_jta_7_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_rsd_xreg_jta_7_BIT_MASK                                  0xff000000
#define REG_Ssd_rsd_xreg_jta_7_BIT_IDX                                   24
#define REG_Ssd_rsd_xreg_jta_7_NUM_OF_BITS                               8
#define REG_Ssd_rsd_xreg_jta_7_MIN                                       0
#define REG_Ssd_rsd_xreg_jta_7_MAX                                       255

/*!
 * @brief A2 or EIAJ pilot tone detected
 *        $0 = False
 *        $1 = True
 * @range 0..1
 ***/
#define REG_Ssd_xr_apilot
#define REG_Ssd_xr_apilot_OFFSET                                           0x03FC4
#define REG_Ssd_xr_apilot_MASK                                           0x1
#define REG_Ssd_xr_apilot_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_apilot_BIT_MASK                                       0x1
#define REG_Ssd_xr_apilot_BIT_IDX                                        0
#define REG_Ssd_xr_apilot_NUM_OF_BITS                                    1
#define REG_Ssd_xr_apilot_MIN                                            0
#define REG_Ssd_xr_apilot_MAX                                            1
#define REG_SSD_XR_APILOT_VALUE_FALSE                                    0
#define REG_SSD_XR_APILOT_VALUE_TRUE                                     1

/*!
 * @brief A2 or EIAJ ident dual flag set
 *        $0 = False
 *        $1 = True
 * @range 0..1
 ***/
#define REG_Ssd_xr_adu
#define REG_Ssd_xr_adu_OFFSET                                              0x03FC4
#define REG_Ssd_xr_adu_MASK                                              0x1
#define REG_Ssd_xr_adu_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_adu_BIT_MASK                                          0x2
#define REG_Ssd_xr_adu_BIT_IDX                                           1
#define REG_Ssd_xr_adu_NUM_OF_BITS                                       1
#define REG_Ssd_xr_adu_MIN                                               0
#define REG_Ssd_xr_adu_MAX                                               1
#define REG_SSD_XR_ADU_VALUE_FALSE                                       0
#define REG_SSD_XR_ADU_VALUE_TRUE                                        1

/*!
 * @brief A2 or EIAJ ident Stereo flag set
 *        $0 = False
 *        $1 = True
 * @range 0..1
 ***/
#define REG_Ssd_xr_ast
#define REG_Ssd_xr_ast_OFFSET                                              0x03FC4
#define REG_Ssd_xr_ast_MASK                                              0x1
#define REG_Ssd_xr_ast_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_ast_BIT_MASK                                          0x4
#define REG_Ssd_xr_ast_BIT_IDX                                           2
#define REG_Ssd_xr_ast_NUM_OF_BITS                                       1
#define REG_Ssd_xr_ast_MIN                                               0
#define REG_Ssd_xr_ast_MAX                                               1
#define REG_SSD_XR_AST_VALUE_FALSE                                       0
#define REG_SSD_XR_AST_VALUE_TRUE                                        1

/*!
 * @brief Reserved
 * @range 0..3
 ***/
#define REG_Ssd_rsd_xreg_hwstatus_12
#define REG_Ssd_rsd_xreg_hwstatus_12_OFFSET                                0x03FC4
#define REG_Ssd_rsd_xreg_hwstatus_12_MASK                                0x3
#define REG_Ssd_rsd_xreg_hwstatus_12_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_rsd_xreg_hwstatus_12_BIT_MASK                            0x18
#define REG_Ssd_rsd_xreg_hwstatus_12_BIT_IDX                             3
#define REG_Ssd_rsd_xreg_hwstatus_12_NUM_OF_BITS                         2
#define REG_Ssd_rsd_xreg_hwstatus_12_MIN                                 0
#define REG_Ssd_rsd_xreg_hwstatus_12_MAX                                 3

/*!
 * @brief NICAM error counter: number of parity errors found in the past 128ms period
 * @range 0..255
 ***/
#define REG_Ssd_xr_err_out
#define REG_Ssd_xr_err_out_OFFSET                                          0x03FC4
#define REG_Ssd_xr_err_out_MASK                                          0xff
#define REG_Ssd_xr_err_out_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_xr_err_out_BIT_MASK                                      0x1fe0
#define REG_Ssd_xr_err_out_BIT_IDX                                       5
#define REG_Ssd_xr_err_out_NUM_OF_BITS                                   8
#define REG_Ssd_xr_err_out_MIN                                           0
#define REG_Ssd_xr_err_out_MAX                                           255

/*!
 * @brief NICAM ConFiguration Change
 *        $0 = no configuration change
 *        $1 = Configuration change at the 16 frame (CO) boundary
 * @range 0..1
 ***/
#define REG_Ssd_xr_cfc
#define REG_Ssd_xr_cfc_OFFSET                                              0x03FC4
#define REG_Ssd_xr_cfc_MASK                                              0x1
#define REG_Ssd_xr_cfc_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_cfc_BIT_MASK                                          0x2000
#define REG_Ssd_xr_cfc_BIT_IDX                                           13
#define REG_Ssd_xr_cfc_NUM_OF_BITS                                       1
#define REG_Ssd_xr_cfc_MIN                                               0
#define REG_Ssd_xr_cfc_MAX                                               1
#define REG_SSD_XR_CFC_VALUE_NO_CONFIGURATION_CHANGE                     0
#define REG_SSD_XR_CFC_VALUE_CONFIGURATION_CHANGE_AT_THE_16_FRAME_CO_BOUNDARY 1

/*!
 * @brief NICAM frame and CO synchronization
 *        $0 = Audio output from NICAM part is digital silence
 *        $1 = Device has both frame and CO (16 frames) synchronization
 * @range 0..1
 ***/
#define REG_Ssd_xr_co_locked
#define REG_Ssd_xr_co_locked_OFFSET                                        0x03FC4
#define REG_Ssd_xr_co_locked_MASK                                        0x1
#define REG_Ssd_xr_co_locked_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_xr_co_locked_BIT_MASK                                    0x4000
#define REG_Ssd_xr_co_locked_BIT_IDX                                     14
#define REG_Ssd_xr_co_locked_NUM_OF_BITS                                 1
#define REG_Ssd_xr_co_locked_MIN                                         0
#define REG_Ssd_xr_co_locked_MAX                                         1
#define REG_SSD_XR_CO_LOCKED_VALUE_AUDIO_OUTPUT_FROM_NICAM_PART_IS_DIGITAL_SILENCE 0
#define REG_SSD_XR_CO_LOCKED_VALUE_DEVICE_HAS_BOTH_FRAME_AND_CO_16_FRAMES_SYNCHRONIZATION 1

/*!
 * @brief NICAM application control bits C1..C4
 * @range 0..15
 ***/
#define REG_Ssd_xr_nacb
#define REG_Ssd_xr_nacb_OFFSET                                             0x03FC4
#define REG_Ssd_xr_nacb_MASK                                             0xf
#define REG_Ssd_xr_nacb_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_xr_nacb_BIT_MASK                                         0x78000
#define REG_Ssd_xr_nacb_BIT_IDX                                          15
#define REG_Ssd_xr_nacb_NUM_OF_BITS                                      4
#define REG_Ssd_xr_nacb_MIN                                              0
#define REG_Ssd_xr_nacb_MAX                                              15

/*!
 * @brief Identification of NICAM sound
 *        $0 = DATA or undefined format
 *        $1 = SOUND
 * @range 0..1
 ***/
#define REG_Ssd_xr_vdsp
#define REG_Ssd_xr_vdsp_OFFSET                                             0x03FC4
#define REG_Ssd_xr_vdsp_MASK                                             0x1
#define REG_Ssd_xr_vdsp_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_xr_vdsp_BIT_MASK                                         0x80000
#define REG_Ssd_xr_vdsp_BIT_IDX                                          19
#define REG_Ssd_xr_vdsp_NUM_OF_BITS                                      1
#define REG_Ssd_xr_vdsp_MIN                                              0
#define REG_Ssd_xr_vdsp_MAX                                              1
#define REG_SSD_XR_VDSP_VALUE_DATA_OR_UNDEFINED_FORMAT                   0
#define REG_SSD_XR_VDSP_VALUE_SOUND                                      1

/*!
 * @brief NICAM stereo flag
 *        $0 = No NICAM stereo mode
 *        $1 = NICAM stereo mode
 * @range 0..1
 ***/
#define REG_Ssd_xr_nicst
#define REG_Ssd_xr_nicst_OFFSET                                            0x03FC4
#define REG_Ssd_xr_nicst_MASK                                            0x1
#define REG_Ssd_xr_nicst_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_xr_nicst_BIT_MASK                                        0x100000
#define REG_Ssd_xr_nicst_BIT_IDX                                         20
#define REG_Ssd_xr_nicst_NUM_OF_BITS                                     1
#define REG_Ssd_xr_nicst_MIN                                             0
#define REG_Ssd_xr_nicst_MAX                                             1
#define REG_SSD_XR_NICST_VALUE_NO_NICAM_STEREO_MODE                      0
#define REG_SSD_XR_NICST_VALUE_NICAM_STEREO_MODE                         1

/*!
 * @brief NICAM dual mode
 *        $0 = no NICAM dual mode
 *        $1 = NICAM dual mode
 * @range 0..1
 ***/
#define REG_Ssd_xr_nicdu
#define REG_Ssd_xr_nicdu_OFFSET                                            0x03FC4
#define REG_Ssd_xr_nicdu_MASK                                            0x1
#define REG_Ssd_xr_nicdu_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_xr_nicdu_BIT_MASK                                        0x200000
#define REG_Ssd_xr_nicdu_BIT_IDX                                         21
#define REG_Ssd_xr_nicdu_NUM_OF_BITS                                     1
#define REG_Ssd_xr_nicdu_MIN                                             0
#define REG_Ssd_xr_nicdu_MAX                                             1
#define REG_SSD_XR_NICDU_VALUE_NO_NICAM_DUAL_MODE                        0
#define REG_SSD_XR_NICDU_VALUE_NICAM_DUAL_MODE                           1

/*!
 * @brief Reserved
 * @range 0..3
 ***/
#define REG_Ssd_rsd_xreg_hwstatus_20
#define REG_Ssd_rsd_xreg_hwstatus_20_OFFSET                                0x03FC4
#define REG_Ssd_rsd_xreg_hwstatus_20_MASK                                0x3
#define REG_Ssd_rsd_xreg_hwstatus_20_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_rsd_xreg_hwstatus_20_BIT_MASK                            0xc00000
#define REG_Ssd_rsd_xreg_hwstatus_20_BIT_IDX                             22
#define REG_Ssd_rsd_xreg_hwstatus_20_NUM_OF_BITS                         2
#define REG_Ssd_rsd_xreg_hwstatus_20_MIN                                 0
#define REG_Ssd_rsd_xreg_hwstatus_20_MAX                                 3

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_hwstatus_21
#define REG_Ssd_rsd_xreg_hwstatus_21_OFFSET                                0x03FC4
#define REG_Ssd_rsd_xreg_hwstatus_21_MASK                                0xff
#define REG_Ssd_rsd_xreg_hwstatus_21_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_rsd_xreg_hwstatus_21_BIT_MASK                            0xff000000
#define REG_Ssd_rsd_xreg_hwstatus_21_BIT_IDX                             24
#define REG_Ssd_rsd_xreg_hwstatus_21_NUM_OF_BITS                         8
#define REG_Ssd_rsd_xreg_hwstatus_21_MIN                                 0
#define REG_Ssd_rsd_xreg_hwstatus_21_MAX                                 255

/*!
 * @brief Additional data word (11 bits)
 * @range 0..2047
 ***/
#define REG_Ssd_xr_adw
#define REG_Ssd_xr_adw_OFFSET                                              0x03FC8
#define REG_Ssd_xr_adw_MASK                                              0x7ff
#define REG_Ssd_xr_adw_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_adw_BIT_MASK                                          0x7ff
#define REG_Ssd_xr_adw_BIT_IDX                                           0
#define REG_Ssd_xr_adw_NUM_OF_BITS                                       11
#define REG_Ssd_xr_adw_MIN                                               0
#define REG_Ssd_xr_adw_MAX                                               2047

/*!
 * @brief CI bits, decoded by majority logic from the parity checks of the last ten samples in a frame.
 * @range 0..3
 ***/
#define REG_Ssd_xr_parchk
#define REG_Ssd_xr_parchk_OFFSET                                           0x03FC8
#define REG_Ssd_xr_parchk_MASK                                           0x3
#define REG_Ssd_xr_parchk_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_parchk_BIT_MASK                                       0x1800
#define REG_Ssd_xr_parchk_BIT_IDX                                        11
#define REG_Ssd_xr_parchk_NUM_OF_BITS                                    2
#define REG_Ssd_xr_parchk_MIN                                            0
#define REG_Ssd_xr_parchk_MAX                                            3

/*!
 * @brief New additional data bits are written into IC, reset when data are read.
 *        $0 = False
 *        $1 = True
 * @range 0..1
 ***/
#define REG_Ssd_xr_sad
#define REG_Ssd_xr_sad_OFFSET                                              0x03FC8
#define REG_Ssd_xr_sad_MASK                                              0x1
#define REG_Ssd_xr_sad_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_sad_BIT_MASK                                          0x2000
#define REG_Ssd_xr_sad_BIT_IDX                                           13
#define REG_Ssd_xr_sad_NUM_OF_BITS                                       1
#define REG_Ssd_xr_sad_MIN                                               0
#define REG_Ssd_xr_sad_MAX                                               1
#define REG_SSD_XR_SAD_VALUE_FALSE                                       0
#define REG_SSD_XR_SAD_VALUE_TRUE                                        1

/*!
 * @brief New additional data bits are written into IC, without the previous bits being read.
 *        $0 = False
 *        $1 = True
 * @range 0..1
 ***/
#define REG_Ssd_xr_ovw
#define REG_Ssd_xr_ovw_OFFSET                                              0x03FC8
#define REG_Ssd_xr_ovw_MASK                                              0x1
#define REG_Ssd_xr_ovw_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_ovw_BIT_MASK                                          0x4000
#define REG_Ssd_xr_ovw_BIT_IDX                                           14
#define REG_Ssd_xr_ovw_NUM_OF_BITS                                       1
#define REG_Ssd_xr_ovw_MIN                                               0
#define REG_Ssd_xr_ovw_MAX                                               1
#define REG_SSD_XR_OVW_VALUE_FALSE                                       0
#define REG_SSD_XR_OVW_VALUE_TRUE                                        1

/*!
 * @brief Reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_xreg_nicamst_27
#define REG_Ssd_rsd_xreg_nicamst_27_OFFSET                                 0x03FC8
#define REG_Ssd_rsd_xreg_nicamst_27_MASK                                 0x1
#define REG_Ssd_rsd_xreg_nicamst_27_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_rsd_xreg_nicamst_27_BIT_MASK                             0x8000
#define REG_Ssd_rsd_xreg_nicamst_27_BIT_IDX                              15
#define REG_Ssd_rsd_xreg_nicamst_27_NUM_OF_BITS                          1
#define REG_Ssd_rsd_xreg_nicamst_27_MIN                                  0
#define REG_Ssd_rsd_xreg_nicamst_27_MAX                                  1

/*!
 * @brief DQP debug output
 * @range 0..255
 ***/
#define REG_Ssd_xr_carloopint
#define REG_Ssd_xr_carloopint_OFFSET                                       0x03FC8
#define REG_Ssd_xr_carloopint_MASK                                       0xff
#define REG_Ssd_xr_carloopint_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_carloopint_BIT_MASK                                   0xff0000
#define REG_Ssd_xr_carloopint_BIT_IDX                                    16
#define REG_Ssd_xr_carloopint_NUM_OF_BITS                                8
#define REG_Ssd_xr_carloopint_MIN                                        0
#define REG_Ssd_xr_carloopint_MAX                                        255

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_nicamst_30
#define REG_Ssd_rsd_xreg_nicamst_30_OFFSET                                 0x03FC8
#define REG_Ssd_rsd_xreg_nicamst_30_MASK                                 0xff
#define REG_Ssd_rsd_xreg_nicamst_30_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_rsd_xreg_nicamst_30_BIT_MASK                             0xff000000
#define REG_Ssd_rsd_xreg_nicamst_30_BIT_IDX                              24
#define REG_Ssd_rsd_xreg_nicamst_30_NUM_OF_BITS                          8
#define REG_Ssd_rsd_xreg_nicamst_30_MIN                                  0
#define REG_Ssd_rsd_xreg_nicamst_30_MAX                                  255

/*!
 * @brief SPDIF input status bit CS1
 *        0=audio sample represents linear PCM
 *        1=audio sample word used for other purposes (e.g. Dolby Digital)
 * @range 0..1
 ***/
#define REG_Ssd_xr_spdifi_cs1
#define REG_Ssd_xr_spdifi_cs1_OFFSET                                       0x03FCC
#define REG_Ssd_xr_spdifi_cs1_MASK                                       0x1
#define REG_Ssd_xr_spdifi_cs1_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_spdifi_cs1_BIT_MASK                                   0x1
#define REG_Ssd_xr_spdifi_cs1_BIT_IDX                                    0
#define REG_Ssd_xr_spdifi_cs1_NUM_OF_BITS                                1
#define REG_Ssd_xr_spdifi_cs1_MIN                                        0
#define REG_Ssd_xr_spdifi_cs1_MAX                                        1
#define REG_SSD_XR_SPDIFI_CS1_VALUE_AUDIO_SAMPLE_REPRESENTS_LINEAR_PCM   0
#define REG_SSD_XR_SPDIFI_CS1_VALUE_AUDIO_SAMPLE_WORD_USED_FOR_OTHER_PURPOSES_E_G_DOLBY_DIGITAL 1

/*!
 * @brief SPDIF input parity bit right
 *        0=parity error
 *        1=no parity error
 * @range 0..1
 ***/
#define REG_Ssd_xr_spdifiparr
#define REG_Ssd_xr_spdifiparr_OFFSET                                       0x03FCC
#define REG_Ssd_xr_spdifiparr_MASK                                       0x1
#define REG_Ssd_xr_spdifiparr_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_spdifiparr_BIT_MASK                                   0x2
#define REG_Ssd_xr_spdifiparr_BIT_IDX                                    1
#define REG_Ssd_xr_spdifiparr_NUM_OF_BITS                                1
#define REG_Ssd_xr_spdifiparr_MIN                                        0
#define REG_Ssd_xr_spdifiparr_MAX                                        1
#define REG_SSD_XR_SPDIFIPARR_VALUE_PARITY_ERROR                         0
#define REG_SSD_XR_SPDIFIPARR_VALUE_NO_PARITY_ERROR                      1

/*!
 * @brief SPDIF input parity bit left
 *        0=parity error
 *        1=no parity error
 * @range 0..1
 ***/
#define REG_Ssd_xr_spdifiparl
#define REG_Ssd_xr_spdifiparl_OFFSET                                       0x03FCC
#define REG_Ssd_xr_spdifiparl_MASK                                       0x1
#define REG_Ssd_xr_spdifiparl_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_spdifiparl_BIT_MASK                                   0x4
#define REG_Ssd_xr_spdifiparl_BIT_IDX                                    2
#define REG_Ssd_xr_spdifiparl_NUM_OF_BITS                                1
#define REG_Ssd_xr_spdifiparl_MIN                                        0
#define REG_Ssd_xr_spdifiparl_MAX                                        1
#define REG_SSD_XR_SPDIFIPARL_VALUE_PARITY_ERROR                         0
#define REG_SSD_XR_SPDIFIPARL_VALUE_NO_PARITY_ERROR                      1

/*!
 * @brief Digital input select
 *        0 = I2S
 *        1 = SPDIF
 * @range 0..1
 ***/
#define REG_Ssd_xr_diginsel
#define REG_Ssd_xr_diginsel_OFFSET                                         0x03FCC
#define REG_Ssd_xr_diginsel_MASK                                         0x1
#define REG_Ssd_xr_diginsel_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_diginsel_BIT_MASK                                     0x8
#define REG_Ssd_xr_diginsel_BIT_IDX                                      3
#define REG_Ssd_xr_diginsel_NUM_OF_BITS                                  1
#define REG_Ssd_xr_diginsel_MIN                                          0
#define REG_Ssd_xr_diginsel_MAX                                          1
#define REG_SSD_XR_DIGINSEL_VALUE_I2S                                    0
#define REG_SSD_XR_DIGINSEL_VALUE_SPDIF                                  1

/*!
 * @brief Divider setting for digital audio input
 *        0 = 1 fs mode (divider = 1)
 *        1 = 2 fs mode (divider = 2)
 *        2 = 4 fs mode (divider = 4)
 * @range 0..3
 ***/
#define REG_Ssd_xr_digindiv
#define REG_Ssd_xr_digindiv_OFFSET                                         0x03FCC
#define REG_Ssd_xr_digindiv_MASK                                         0x3
#define REG_Ssd_xr_digindiv_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_digindiv_BIT_MASK                                     0x30
#define REG_Ssd_xr_digindiv_BIT_IDX                                      4
#define REG_Ssd_xr_digindiv_NUM_OF_BITS                                  2
#define REG_Ssd_xr_digindiv_MIN                                          0
#define REG_Ssd_xr_digindiv_MAX                                          3
#define REG_SSD_XR_DIGINDIV_VALUE_1_FS_MODE_DIVIDER_1                    0
#define REG_SSD_XR_DIGINDIV_VALUE_2_FS_MODE_DIVIDER_2                    1
#define REG_SSD_XR_DIGINDIV_VALUE_4_FS_MODE_DIVIDER_4                    2

/*!
 * @brief MPX pilot level
 * @range -131072..131071
 ***/
#define REG_Ssd_xr_mpx_plev
#define REG_Ssd_xr_mpx_plev_OFFSET                                         0x03FCC
#define REG_Ssd_xr_mpx_plev_MASK                                         0x3ffff
#define REG_Ssd_xr_mpx_plev_SIGNED_BIT_MASK                              0x800000
#define REG_Ssd_xr_mpx_plev_BIT_MASK                                     0xffffc0
#define REG_Ssd_xr_mpx_plev_BIT_IDX                                      6
#define REG_Ssd_xr_mpx_plev_NUM_OF_BITS                                  18
#define REG_Ssd_xr_mpx_plev_MIN                                          -131072
#define REG_Ssd_xr_mpx_plev_MAX                                          131071

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_mpxplev_34
#define REG_Ssd_rsd_xreg_mpxplev_34_OFFSET                                 0x03FCC
#define REG_Ssd_rsd_xreg_mpxplev_34_MASK                                 0xff
#define REG_Ssd_rsd_xreg_mpxplev_34_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_rsd_xreg_mpxplev_34_BIT_MASK                             0xff000000
#define REG_Ssd_rsd_xreg_mpxplev_34_BIT_IDX                              24
#define REG_Ssd_rsd_xreg_mpxplev_34_NUM_OF_BITS                          8
#define REG_Ssd_rsd_xreg_mpxplev_34_MIN                                  0
#define REG_Ssd_rsd_xreg_mpxplev_34_MAX                                  255

/*!
 * @brief noise detector output
 * @range 0..16777215
 ***/
#define REG_Ssd_xr_noise_level
#define REG_Ssd_xr_noise_level_OFFSET                                      0x03FD0
#define REG_Ssd_xr_noise_level_MASK                                      0xffffff
#define REG_Ssd_xr_noise_level_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_xr_noise_level_BIT_MASK                                  0xffffff
#define REG_Ssd_xr_noise_level_BIT_IDX                                   0
#define REG_Ssd_xr_noise_level_NUM_OF_BITS                               24
#define REG_Ssd_xr_noise_level_MIN                                       0
#define REG_Ssd_xr_noise_level_MAX                                       16777215

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_noiselev_37
#define REG_Ssd_rsd_xreg_noiselev_37_OFFSET                                0x03FD0
#define REG_Ssd_rsd_xreg_noiselev_37_MASK                                0xff
#define REG_Ssd_rsd_xreg_noiselev_37_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_rsd_xreg_noiselev_37_BIT_MASK                            0xff000000
#define REG_Ssd_rsd_xreg_noiselev_37_BIT_IDX                             24
#define REG_Ssd_rsd_xreg_noiselev_37_NUM_OF_BITS                         8
#define REG_Ssd_rsd_xreg_noiselev_37_MIN                                 0
#define REG_Ssd_rsd_xreg_noiselev_37_MAX                                 255

/*!
 * @brief operating mode of demodulator channel 2
 *        $0 = FM
 *        $1 = AM
 *        $2 = NICAM
 *        $3 = not used
 * @range 0..3
 ***/
#define REG_Ssd_xr_ch2mod
#define REG_Ssd_xr_ch2mod_OFFSET                                           0x03FD4
#define REG_Ssd_xr_ch2mod_MASK                                           0x3
#define REG_Ssd_xr_ch2mod_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_ch2mod_BIT_MASK                                       0x3
#define REG_Ssd_xr_ch2mod_BIT_IDX                                        0
#define REG_Ssd_xr_ch2mod_NUM_OF_BITS                                    2
#define REG_Ssd_xr_ch2mod_MIN                                            0
#define REG_Ssd_xr_ch2mod_MAX                                            3
#define REG_SSD_XR_CH2MOD_VALUE_FM                                       0
#define REG_SSD_XR_CH2MOD_VALUE_AM                                       1
#define REG_SSD_XR_CH2MOD_VALUE_NICAM                                    2
#define REG_SSD_XR_CH2MOD_VALUE_NOT_USED                                 3

/*!
 * @brief operating mode of demodulator channel 1
 *        $0 = FM
 *        $1 = AM
 * @range 0..1
 ***/
#define REG_Ssd_xr_ch1mod
#define REG_Ssd_xr_ch1mod_OFFSET                                           0x03FD4
#define REG_Ssd_xr_ch1mod_MASK                                           0x1
#define REG_Ssd_xr_ch1mod_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_ch1mod_BIT_MASK                                       0x4
#define REG_Ssd_xr_ch1mod_BIT_IDX                                        2
#define REG_Ssd_xr_ch1mod_NUM_OF_BITS                                    1
#define REG_Ssd_xr_ch1mod_MIN                                            0
#define REG_Ssd_xr_ch1mod_MAX                                            1
#define REG_SSD_XR_CH1MOD_VALUE_FM                                       0
#define REG_SSD_XR_CH1MOD_VALUE_AM                                       1

/*!
 * @brief initialization of loop filters in demodulator
 *        $0 = normal operation
 *        $1 = initialization (reset to zero)
 * @range 0..1
 ***/
#define REG_Ssd_xr_initlpf
#define REG_Ssd_xr_initlpf_OFFSET                                          0x03FD4
#define REG_Ssd_xr_initlpf_MASK                                          0x1
#define REG_Ssd_xr_initlpf_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_xr_initlpf_BIT_MASK                                      0x8
#define REG_Ssd_xr_initlpf_BIT_IDX                                       3
#define REG_Ssd_xr_initlpf_NUM_OF_BITS                                   1
#define REG_Ssd_xr_initlpf_MIN                                           0
#define REG_Ssd_xr_initlpf_MAX                                           1
#define REG_SSD_XR_INITLPF_VALUE_NORMAL_OPERATION                        0
#define REG_SSD_XR_INITLPF_VALUE_INITIALIZATION_RESET_TO_ZERO            1

/*!
 * @brief Reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_xreg_hwcfg_42
#define REG_Ssd_rsd_xreg_hwcfg_42_OFFSET                                   0x03FD4
#define REG_Ssd_rsd_xreg_hwcfg_42_MASK                                   0x1
#define REG_Ssd_rsd_xreg_hwcfg_42_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_hwcfg_42_BIT_MASK                               0x10
#define REG_Ssd_rsd_xreg_hwcfg_42_BIT_IDX                                4
#define REG_Ssd_rsd_xreg_hwcfg_42_NUM_OF_BITS                            1
#define REG_Ssd_rsd_xreg_hwcfg_42_MIN                                    0
#define REG_Ssd_rsd_xreg_hwcfg_42_MAX                                    1

/*!
 * @brief sound carrier filter bandwidth
 *        $0 = narrow
 *        $1 = extra wide (high deviation mode)
 *        $2 = medium
 *        $3 = wide
 * @range 0..3
 ***/
#define REG_Ssd_xr_filtbw
#define REG_Ssd_xr_filtbw_OFFSET                                           0x03FD4
#define REG_Ssd_xr_filtbw_MASK                                           0x3
#define REG_Ssd_xr_filtbw_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_filtbw_BIT_MASK                                       0x60
#define REG_Ssd_xr_filtbw_BIT_IDX                                        5
#define REG_Ssd_xr_filtbw_NUM_OF_BITS                                    2
#define REG_Ssd_xr_filtbw_MIN                                            0
#define REG_Ssd_xr_filtbw_MAX                                            3
#define REG_SSD_XR_FILTBW_VALUE_NARROW                                   0
#define REG_SSD_XR_FILTBW_VALUE_EXTRA_WIDE_HIGH_DEVIATION_MODE           1
#define REG_SSD_XR_FILTBW_VALUE_MEDIUM                                   2
#define REG_SSD_XR_FILTBW_VALUE_WIDE                                     3

/*!
 * @brief FM ident speed
 *        $0 = slow
 *        $1 = medium
 *        $2 = fast
 *        $3 = off (reset)
 * @range 0..3
 ***/
#define REG_Ssd_xr_idmod
#define REG_Ssd_xr_idmod_OFFSET                                            0x03FD4
#define REG_Ssd_xr_idmod_MASK                                            0x3
#define REG_Ssd_xr_idmod_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_xr_idmod_BIT_MASK                                        0x180
#define REG_Ssd_xr_idmod_BIT_IDX                                         7
#define REG_Ssd_xr_idmod_NUM_OF_BITS                                     2
#define REG_Ssd_xr_idmod_MIN                                             0
#define REG_Ssd_xr_idmod_MAX                                             3
#define REG_SSD_XR_IDMOD_VALUE_SLOW                                      0
#define REG_SSD_XR_IDMOD_VALUE_MEDIUM                                    1
#define REG_SSD_XR_IDMOD_VALUE_FAST                                      2
#define REG_SSD_XR_IDMOD_VALUE_OFF_RESET                                 3

/*!
 * @brief Area/regional code for FM-ident: Europe, Korea, Japan
 *        $0 = Europe
 *        $1 = Korea
 *        $2 = Japan
 * @range 0..3
 ***/
#define REG_Ssd_xr_idarea
#define REG_Ssd_xr_idarea_OFFSET                                           0x03FD4
#define REG_Ssd_xr_idarea_MASK                                           0x3
#define REG_Ssd_xr_idarea_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_idarea_BIT_MASK                                       0x600
#define REG_Ssd_xr_idarea_BIT_IDX                                        9
#define REG_Ssd_xr_idarea_NUM_OF_BITS                                    2
#define REG_Ssd_xr_idarea_MIN                                            0
#define REG_Ssd_xr_idarea_MAX                                            3
#define REG_SSD_XR_IDAREA_VALUE_EUROPE                                   0
#define REG_SSD_XR_IDAREA_VALUE_KOREA                                    1
#define REG_SSD_XR_IDAREA_VALUE_JAPAN                                    2

/*!
 * @brief MPX pilot cancellation
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_bpilcan
#define REG_Ssd_xr_bpilcan_OFFSET                                          0x03FD4
#define REG_Ssd_xr_bpilcan_MASK                                          0x1
#define REG_Ssd_xr_bpilcan_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_xr_bpilcan_BIT_MASK                                      0x800
#define REG_Ssd_xr_bpilcan_BIT_IDX                                       11
#define REG_Ssd_xr_bpilcan_NUM_OF_BITS                                   1
#define REG_Ssd_xr_bpilcan_MIN                                           0
#define REG_Ssd_xr_bpilcan_MAX                                           1
#define REG_SSD_XR_BPILCAN_VALUE_OFF                                     0
#define REG_SSD_XR_BPILCAN_VALUE_ON                                      1

/*!
 * @brief input from demodulator hardware at 4*fs
 *        $0 = FM / AM output
 *        $1 = MPX demodulator output
 * @range 0..1
 ***/
#define REG_Ssd_xr_fm_mpx
#define REG_Ssd_xr_fm_mpx_OFFSET                                           0x03FD4
#define REG_Ssd_xr_fm_mpx_MASK                                           0x1
#define REG_Ssd_xr_fm_mpx_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_fm_mpx_BIT_MASK                                       0x1000
#define REG_Ssd_xr_fm_mpx_BIT_IDX                                        12
#define REG_Ssd_xr_fm_mpx_NUM_OF_BITS                                    1
#define REG_Ssd_xr_fm_mpx_MIN                                            0
#define REG_Ssd_xr_fm_mpx_MAX                                            1
#define REG_SSD_XR_FM_MPX_VALUE_FM_AM_OUTPUT                             0
#define REG_SSD_XR_FM_MPX_VALUE_MPX_DEMODULATOR_OUTPUT                   1

/*!
 * @brief Switch DC Level for Ident pilot tone
 *        $0 = Level > 3
 *        $1 = Level > 4
 *        $2 = Level > 5
 *        $3 = Level > 6
 * @range 0..3
 ***/
#define REG_Ssd_xr_id_dc_level
#define REG_Ssd_xr_id_dc_level_OFFSET                                      0x03FD4
#define REG_Ssd_xr_id_dc_level_MASK                                      0x3
#define REG_Ssd_xr_id_dc_level_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_xr_id_dc_level_BIT_MASK                                  0x6000
#define REG_Ssd_xr_id_dc_level_BIT_IDX                                   13
#define REG_Ssd_xr_id_dc_level_NUM_OF_BITS                               2
#define REG_Ssd_xr_id_dc_level_MIN                                       0
#define REG_Ssd_xr_id_dc_level_MAX                                       3
#define REG_SSD_XR_ID_DC_LEVEL_VALUE_LEVEL_3                             0
#define REG_SSD_XR_ID_DC_LEVEL_VALUE_LEVEL_4                             1
#define REG_SSD_XR_ID_DC_LEVEL_VALUE_LEVEL_5                             2
#define REG_SSD_XR_ID_DC_LEVEL_VALUE_LEVEL_6                             3

/*!
 * @brief Bypass Bandpass Filter Level detector
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_id_bybpf
#define REG_Ssd_xr_id_bybpf_OFFSET                                         0x03FD4
#define REG_Ssd_xr_id_bybpf_MASK                                         0x1
#define REG_Ssd_xr_id_bybpf_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_id_bybpf_BIT_MASK                                     0x8000
#define REG_Ssd_xr_id_bybpf_BIT_IDX                                      15
#define REG_Ssd_xr_id_bybpf_NUM_OF_BITS                                  1
#define REG_Ssd_xr_id_bybpf_MIN                                          0
#define REG_Ssd_xr_id_bybpf_MAX                                          1
#define REG_SSD_XR_ID_BYBPF_VALUE_OFF                                    0
#define REG_SSD_XR_ID_BYBPF_VALUE_ON                                     1

/*!
 * @brief Enable for output register noise level BSJ
 *        $0 = disable
 *        $1 = enable
 * @range 0..1
 ***/
#define REG_Ssd_xr_noiselev_en
#define REG_Ssd_xr_noiselev_en_OFFSET                                      0x03FD4
#define REG_Ssd_xr_noiselev_en_MASK                                      0x1
#define REG_Ssd_xr_noiselev_en_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_xr_noiselev_en_BIT_MASK                                  0x10000
#define REG_Ssd_xr_noiselev_en_BIT_IDX                                   16
#define REG_Ssd_xr_noiselev_en_NUM_OF_BITS                               1
#define REG_Ssd_xr_noiselev_en_MIN                                       0
#define REG_Ssd_xr_noiselev_en_MAX                                       1
#define REG_SSD_XR_NOISELEV_EN_VALUE_DISABLE                             0
#define REG_SSD_XR_NOISELEV_EN_VALUE_ENABLE                              1

/*!
 * @brief Enable for output register pilot level BSJ
 *        $0 = disable
 *        $1 = enable
 * @range 0..1
 ***/
#define REG_Ssd_xr_pilotlev_en
#define REG_Ssd_xr_pilotlev_en_OFFSET                                      0x03FD4
#define REG_Ssd_xr_pilotlev_en_MASK                                      0x1
#define REG_Ssd_xr_pilotlev_en_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_xr_pilotlev_en_BIT_MASK                                  0x20000
#define REG_Ssd_xr_pilotlev_en_BIT_IDX                                   17
#define REG_Ssd_xr_pilotlev_en_NUM_OF_BITS                               1
#define REG_Ssd_xr_pilotlev_en_MIN                                       0
#define REG_Ssd_xr_pilotlev_en_MAX                                       1
#define REG_SSD_XR_PILOTLEV_EN_VALUE_DISABLE                             0
#define REG_SSD_XR_PILOTLEV_EN_VALUE_ENABLE                              1

/*!
 * @brief IDENT pilot bandpass gain
 *        $0 = no gain
 *        $1 = +6 dB gain for EIAJ
 * @range 0..1
 ***/
#define REG_Ssd_xr_id_pgain
#define REG_Ssd_xr_id_pgain_OFFSET                                         0x03FD4
#define REG_Ssd_xr_id_pgain_MASK                                         0x1
#define REG_Ssd_xr_id_pgain_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_id_pgain_BIT_MASK                                     0x40000
#define REG_Ssd_xr_id_pgain_BIT_IDX                                      18
#define REG_Ssd_xr_id_pgain_NUM_OF_BITS                                  1
#define REG_Ssd_xr_id_pgain_MIN                                          0
#define REG_Ssd_xr_id_pgain_MAX                                          1
#define REG_SSD_XR_ID_PGAIN_VALUE_NO_GAIN                                0
#define REG_SSD_XR_ID_PGAIN_VALUE_6_DB_GAIN_FOR_EIAJ                     1

/*!
 * @brief Reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_xreg_hwcfg_53
#define REG_Ssd_rsd_xreg_hwcfg_53_OFFSET                                   0x03FD4
#define REG_Ssd_rsd_xreg_hwcfg_53_MASK                                   0x1
#define REG_Ssd_rsd_xreg_hwcfg_53_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_hwcfg_53_BIT_MASK                               0x80000
#define REG_Ssd_rsd_xreg_hwcfg_53_BIT_IDX                                19
#define REG_Ssd_rsd_xreg_hwcfg_53_NUM_OF_BITS                            1
#define REG_Ssd_rsd_xreg_hwcfg_53_MIN                                    0
#define REG_Ssd_rsd_xreg_hwcfg_53_MAX                                    1

/*!
 * @brief Reserved
 * @range 0..7
 ***/
#define REG_Ssd_rsd_xreg_hwcfg_54
#define REG_Ssd_rsd_xreg_hwcfg_54_OFFSET                                   0x03FD4
#define REG_Ssd_rsd_xreg_hwcfg_54_MASK                                   0x7
#define REG_Ssd_rsd_xreg_hwcfg_54_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_hwcfg_54_BIT_MASK                               0x700000
#define REG_Ssd_rsd_xreg_hwcfg_54_BIT_IDX                                20
#define REG_Ssd_rsd_xreg_hwcfg_54_NUM_OF_BITS                            3
#define REG_Ssd_rsd_xreg_hwcfg_54_MIN                                    0
#define REG_Ssd_rsd_xreg_hwcfg_54_MAX                                    7

/*!
 * @brief Reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_xreg_hwcfg_55
#define REG_Ssd_rsd_xreg_hwcfg_55_OFFSET                                   0x03FD4
#define REG_Ssd_rsd_xreg_hwcfg_55_MASK                                   0x1
#define REG_Ssd_rsd_xreg_hwcfg_55_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_hwcfg_55_BIT_MASK                               0x800000
#define REG_Ssd_rsd_xreg_hwcfg_55_BIT_IDX                                23
#define REG_Ssd_rsd_xreg_hwcfg_55_NUM_OF_BITS                            1
#define REG_Ssd_rsd_xreg_hwcfg_55_MIN                                    0
#define REG_Ssd_rsd_xreg_hwcfg_55_MAX                                    1

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_hwcfg_56
#define REG_Ssd_rsd_xreg_hwcfg_56_OFFSET                                   0x03FD4
#define REG_Ssd_rsd_xreg_hwcfg_56_MASK                                   0xff
#define REG_Ssd_rsd_xreg_hwcfg_56_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_hwcfg_56_BIT_MASK                               0xff000000
#define REG_Ssd_rsd_xreg_hwcfg_56_BIT_IDX                                24
#define REG_Ssd_rsd_xreg_hwcfg_56_NUM_OF_BITS                            8
#define REG_Ssd_rsd_xreg_hwcfg_56_MIN                                    0
#define REG_Ssd_rsd_xreg_hwcfg_56_MAX                                    255

/*!
 * @brief sound carrier 1 (mixer 1) frequency
 *        $0 = reserved
 *        $555555 = 4.5 MHz
 *        $684BDA = 5.5 MHz
 *        $71C71C = 6.0 MHz
 *        $7B425E = 6.5 MHz
 *        $CAE759 = 10.7 MHz
 * @range 0..16777215
 ***/
#define REG_Ssd_xr_mixer1
#define REG_Ssd_xr_mixer1_OFFSET                                           0x03FD8
#define REG_Ssd_xr_mixer1_MASK                                           0xffffff
#define REG_Ssd_xr_mixer1_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_mixer1_BIT_MASK                                       0xffffff
#define REG_Ssd_xr_mixer1_BIT_IDX                                        0
#define REG_Ssd_xr_mixer1_NUM_OF_BITS                                    24
#define REG_Ssd_xr_mixer1_MIN                                            0
#define REG_Ssd_xr_mixer1_MAX                                            16777215
#define REG_SSD_XR_MIXER1_VALUE_RESERVED                                 0
#define REG_SSD_XR_MIXER1_VALUE_4_5_MHZ                                  5592405
#define REG_SSD_XR_MIXER1_VALUE_5_5_MHZ                                  6835162
#define REG_SSD_XR_MIXER1_VALUE_6_0_MHZ                                  7456540
#define REG_SSD_XR_MIXER1_VALUE_6_5_MHZ                                  8077918
#define REG_SSD_XR_MIXER1_VALUE_10_7_MHZ                                 13297497

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_car1_59
#define REG_Ssd_rsd_xreg_car1_59_OFFSET                                    0x03FD8
#define REG_Ssd_rsd_xreg_car1_59_MASK                                    0xff
#define REG_Ssd_rsd_xreg_car1_59_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_rsd_xreg_car1_59_BIT_MASK                                0xff000000
#define REG_Ssd_rsd_xreg_car1_59_BIT_IDX                                 24
#define REG_Ssd_rsd_xreg_car1_59_NUM_OF_BITS                             8
#define REG_Ssd_rsd_xreg_car1_59_MIN                                     0
#define REG_Ssd_rsd_xreg_car1_59_MAX                                     255

/*!
 * @brief sound carrier 2 (mixer 2)  frequency
 *        $0 = reserved
 *        $5994BF = 4.724 MHz
 *        $6CE2A5 = 5.742 MHz
 *        $6EEEEF = 5.85 MHz
 *        $7C3ECE = 6.552 MHz
 *        $76AB94 = 6.258 MHz
 *        $555555 = 4.5 MHz
 * @range 0..16777215
 ***/
#define REG_Ssd_xr_mixer2
#define REG_Ssd_xr_mixer2_OFFSET                                           0x03FDC
#define REG_Ssd_xr_mixer2_MASK                                           0xffffff
#define REG_Ssd_xr_mixer2_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_mixer2_BIT_MASK                                       0xffffff
#define REG_Ssd_xr_mixer2_BIT_IDX                                        0
#define REG_Ssd_xr_mixer2_NUM_OF_BITS                                    24
#define REG_Ssd_xr_mixer2_MIN                                            0
#define REG_Ssd_xr_mixer2_MAX                                            16777215
#define REG_SSD_XR_MIXER2_VALUE_RESERVED                                 0
#define REG_SSD_XR_MIXER2_VALUE_4_5_MHZ                                  5592405
#define REG_SSD_XR_MIXER2_VALUE_4_724_MHZ                                5870783
#define REG_SSD_XR_MIXER2_VALUE_5_742_MHZ                                7135909
#define REG_SSD_XR_MIXER2_VALUE_5_85_MHZ                                 7270127
#define REG_SSD_XR_MIXER2_VALUE_6_258_MHZ                                7777172
#define REG_SSD_XR_MIXER2_VALUE_6_552_MHZ                                8142542

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_car2_62
#define REG_Ssd_rsd_xreg_car2_62_OFFSET                                    0x03FDC
#define REG_Ssd_rsd_xreg_car2_62_MASK                                    0xff
#define REG_Ssd_rsd_xreg_car2_62_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_rsd_xreg_car2_62_BIT_MASK                                0xff000000
#define REG_Ssd_rsd_xreg_car2_62_BIT_IDX                                 24
#define REG_Ssd_rsd_xreg_car2_62_NUM_OF_BITS                             8
#define REG_Ssd_rsd_xreg_car2_62_MIN                                     0
#define REG_Ssd_rsd_xreg_car2_62_MAX                                     255

/*!
 * @brief Reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_xreg_mpxcfg_64
#define REG_Ssd_rsd_xreg_mpxcfg_64_OFFSET                                  0x03FE0
#define REG_Ssd_rsd_xreg_mpxcfg_64_MASK                                  0x1
#define REG_Ssd_rsd_xreg_mpxcfg_64_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_xreg_mpxcfg_64_BIT_MASK                              0x1
#define REG_Ssd_rsd_xreg_mpxcfg_64_BIT_IDX                               0
#define REG_Ssd_rsd_xreg_mpxcfg_64_NUM_OF_BITS                           1
#define REG_Ssd_rsd_xreg_mpxcfg_64_MIN                                   0
#define REG_Ssd_rsd_xreg_mpxcfg_64_MAX                                   1

/*!
 * @brief MPX demod. pilot PLL bandwidth
 *        $0 = 5Hz (slow)
 *        $1 = 10Hz (fast)
 * @range 0..1
 ***/
#define REG_Ssd_xr_mpx_pll_bw
#define REG_Ssd_xr_mpx_pll_bw_OFFSET                                       0x03FE0
#define REG_Ssd_xr_mpx_pll_bw_MASK                                       0x1
#define REG_Ssd_xr_mpx_pll_bw_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_mpx_pll_bw_BIT_MASK                                   0x2
#define REG_Ssd_xr_mpx_pll_bw_BIT_IDX                                    1
#define REG_Ssd_xr_mpx_pll_bw_NUM_OF_BITS                                1
#define REG_Ssd_xr_mpx_pll_bw_MIN                                        0
#define REG_Ssd_xr_mpx_pll_bw_MAX                                        1
#define REG_SSD_XR_MPX_PLL_BW_VALUE_5HZ_SLOW                             0
#define REG_SSD_XR_MPX_PLL_BW_VALUE_10HZ_FAST                            1

/*!
 * @brief MPX pilot frequency
 *        $0 = reserved
 *        $29F54 =15734 Hz (standard NTSC line frequency)
 *        $29AAA = 15625 Hz (PAL line frequency)
 *        $32AAA = 19000 Hz (FM radio)
 * @range 0..4194303
 ***/
#define REG_Ssd_xr_mpx_freq
#define REG_Ssd_xr_mpx_freq_OFFSET                                         0x03FE0
#define REG_Ssd_xr_mpx_freq_MASK                                         0x3fffff
#define REG_Ssd_xr_mpx_freq_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_mpx_freq_BIT_MASK                                     0xfffffc
#define REG_Ssd_xr_mpx_freq_BIT_IDX                                      2
#define REG_Ssd_xr_mpx_freq_NUM_OF_BITS                                  22
#define REG_Ssd_xr_mpx_freq_MIN                                          0
#define REG_Ssd_xr_mpx_freq_MAX                                          4194303
#define REG_SSD_XR_MPX_FREQ_VALUE_RESERVED                               0
#define REG_SSD_XR_MPX_FREQ_VALUE_15625_HZ_PAL_LINE_FREQUENCY            170666
#define REG_SSD_XR_MPX_FREQ_VALUE_15734_HZ_STANDARD_NTSC_LINE_FREQUENCY  171860
#define REG_SSD_XR_MPX_FREQ_VALUE_19000_HZ_FM_RADIO                      207530

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_mpxcfg_67
#define REG_Ssd_rsd_xreg_mpxcfg_67_OFFSET                                  0x03FE0
#define REG_Ssd_rsd_xreg_mpxcfg_67_MASK                                  0xff
#define REG_Ssd_rsd_xreg_mpxcfg_67_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_xreg_mpxcfg_67_BIT_MASK                              0xff000000
#define REG_Ssd_rsd_xreg_mpxcfg_67_BIT_IDX                               24
#define REG_Ssd_rsd_xreg_mpxcfg_67_NUM_OF_BITS                           8
#define REG_Ssd_rsd_xreg_mpxcfg_67_MIN                                   0
#define REG_Ssd_rsd_xreg_mpxcfg_67_MAX                                   255

/*!
 * @brief FM subchannel filter bandwidth
 *        $0 = narrow
 *        $1 = wide
 * @range 0..1
 ***/
#define REG_Ssd_xr_fmsub_bw
#define REG_Ssd_xr_fmsub_bw_OFFSET                                         0x03FE4
#define REG_Ssd_xr_fmsub_bw_MASK                                         0x1
#define REG_Ssd_xr_fmsub_bw_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_fmsub_bw_BIT_MASK                                     0x1
#define REG_Ssd_xr_fmsub_bw_BIT_IDX                                      0
#define REG_Ssd_xr_fmsub_bw_NUM_OF_BITS                                  1
#define REG_Ssd_xr_fmsub_bw_MIN                                          0
#define REG_Ssd_xr_fmsub_bw_MAX                                          1
#define REG_SSD_XR_FMSUB_BW_VALUE_NARROW                                 0
#define REG_SSD_XR_FMSUB_BW_VALUE_WIDE                                   1

/*!
 * @brief delay fine adjustment in MAIN path for EIAJ stereo
 * @range 0..3
 ***/
#define REG_Ssd_xr_eiaj_delay
#define REG_Ssd_xr_eiaj_delay_OFFSET                                       0x03FE4
#define REG_Ssd_xr_eiaj_delay_MASK                                       0x3
#define REG_Ssd_xr_eiaj_delay_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_eiaj_delay_BIT_MASK                                   0x6
#define REG_Ssd_xr_eiaj_delay_BIT_IDX                                    1
#define REG_Ssd_xr_eiaj_delay_NUM_OF_BITS                                2
#define REG_Ssd_xr_eiaj_delay_MIN                                        0
#define REG_Ssd_xr_eiaj_delay_MAX                                        3

/*!
 * @brief Noise detector channel
 *        $0 = channel 1
 *        $1 = channel 2
 * @range 0..1
 ***/
#define REG_Ssd_xr_ndetch
#define REG_Ssd_xr_ndetch_OFFSET                                           0x03FE4
#define REG_Ssd_xr_ndetch_MASK                                           0x1
#define REG_Ssd_xr_ndetch_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_ndetch_BIT_MASK                                       0x8
#define REG_Ssd_xr_ndetch_BIT_IDX                                        3
#define REG_Ssd_xr_ndetch_NUM_OF_BITS                                    1
#define REG_Ssd_xr_ndetch_MIN                                            0
#define REG_Ssd_xr_ndetch_MAX                                            1
#define REG_SSD_XR_NDETCH_VALUE_CHANNEL_1                                0
#define REG_SSD_XR_NDETCH_VALUE_CHANNEL_2                                1

/*!
 * @brief Noise detector passband
 *        $0 = low (2.5 fh)
 *        $1 = high (7.5 fh)
 * @range 0..1
 ***/
#define REG_Ssd_xr_ndetpb
#define REG_Ssd_xr_ndetpb_OFFSET                                           0x03FE4
#define REG_Ssd_xr_ndetpb_MASK                                           0x1
#define REG_Ssd_xr_ndetpb_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_ndetpb_BIT_MASK                                       0x10
#define REG_Ssd_xr_ndetpb_BIT_IDX                                        4
#define REG_Ssd_xr_ndetpb_NUM_OF_BITS                                    1
#define REG_Ssd_xr_ndetpb_MIN                                            0
#define REG_Ssd_xr_ndetpb_MAX                                            1
#define REG_SSD_XR_NDETPB_VALUE_LOW_2_5_FH                               0
#define REG_SSD_XR_NDETPB_VALUE_HIGH_7_5_FH                              1

/*!
 * @brief Reserved
 * @range 0..7
 ***/
#define REG_Ssd_rsd_xreg_fmsubcfg_73
#define REG_Ssd_rsd_xreg_fmsubcfg_73_OFFSET                                0x03FE4
#define REG_Ssd_rsd_xreg_fmsubcfg_73_MASK                                0x7
#define REG_Ssd_rsd_xreg_fmsubcfg_73_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_rsd_xreg_fmsubcfg_73_BIT_MASK                            0xe0
#define REG_Ssd_rsd_xreg_fmsubcfg_73_BIT_IDX                             5
#define REG_Ssd_rsd_xreg_fmsubcfg_73_NUM_OF_BITS                         3
#define REG_Ssd_rsd_xreg_fmsubcfg_73_MIN                                 0
#define REG_Ssd_rsd_xreg_fmsubcfg_73_MAX                                 7

/*!
 * @brief FM subchannel frequency (SAP or Japan)
 *        $0 = reserved
 *        $3473 = 5 fh for SAP
 *        $14FB = 2 fh for EIAJ
 * @range 0..65535
 ***/
#define REG_Ssd_xr_fmsub_freq
#define REG_Ssd_xr_fmsub_freq_OFFSET                                       0x03FE4
#define REG_Ssd_xr_fmsub_freq_MASK                                       0xffff
#define REG_Ssd_xr_fmsub_freq_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_fmsub_freq_BIT_MASK                                   0xffff00
#define REG_Ssd_xr_fmsub_freq_BIT_IDX                                    8
#define REG_Ssd_xr_fmsub_freq_NUM_OF_BITS                                16
#define REG_Ssd_xr_fmsub_freq_MIN                                        0
#define REG_Ssd_xr_fmsub_freq_MAX                                        65535
#define REG_SSD_XR_FMSUB_FREQ_VALUE_RESERVED                             0
#define REG_SSD_XR_FMSUB_FREQ_VALUE_2_FH_FOR_EIAJ                        5371
#define REG_SSD_XR_FMSUB_FREQ_VALUE_5_FH_FOR_SAP                         13427

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_fmsubcfg_75
#define REG_Ssd_rsd_xreg_fmsubcfg_75_OFFSET                                0x03FE4
#define REG_Ssd_rsd_xreg_fmsubcfg_75_MASK                                0xff
#define REG_Ssd_rsd_xreg_fmsubcfg_75_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_rsd_xreg_fmsubcfg_75_BIT_MASK                            0xff000000
#define REG_Ssd_rsd_xreg_fmsubcfg_75_BIT_IDX                             24
#define REG_Ssd_rsd_xreg_fmsubcfg_75_NUM_OF_BITS                         8
#define REG_Ssd_rsd_xreg_fmsubcfg_75_MIN                                 0
#define REG_Ssd_rsd_xreg_fmsubcfg_75_MAX                                 255

/*!
 * @brief DemDec status register
 * @range 0..16777215
 ***/
#define REG_Ssd_xr_ddstatus
#define REG_Ssd_xr_ddstatus_OFFSET                                         0x03FE8
#define REG_Ssd_xr_ddstatus_MASK                                         0xffffff
#define REG_Ssd_xr_ddstatus_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_xr_ddstatus_BIT_MASK                                     0xffffff
#define REG_Ssd_xr_ddstatus_BIT_IDX                                      0
#define REG_Ssd_xr_ddstatus_NUM_OF_BITS                                  24
#define REG_Ssd_xr_ddstatus_MIN                                          0
#define REG_Ssd_xr_ddstatus_MAX                                          16777215

/*!
 * @brief Reserved
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_iis_control_78
#define REG_Ssd_rsd_xreg_iis_control_78_OFFSET                             0x03FE8
#define REG_Ssd_rsd_xreg_iis_control_78_MASK                             0xff
#define REG_Ssd_rsd_xreg_iis_control_78_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_rsd_xreg_iis_control_78_BIT_MASK                         0xff000000
#define REG_Ssd_rsd_xreg_iis_control_78_BIT_IDX                          24
#define REG_Ssd_rsd_xreg_iis_control_78_NUM_OF_BITS                      8
#define REG_Ssd_rsd_xreg_iis_control_78_MIN                              0
#define REG_Ssd_rsd_xreg_iis_control_78_MAX                              255

/*!
 * @brief ssd software versioning enable bits:
 *        [0] VDS Virtual Dolby Surround
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_vds
#define REG_Ssd_xr_vds_OFFSET                                              0x03FEC
#define REG_Ssd_xr_vds_MASK                                              0x1
#define REG_Ssd_xr_vds_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_vds_BIT_MASK                                          0x1
#define REG_Ssd_xr_vds_BIT_IDX                                           0
#define REG_Ssd_xr_vds_NUM_OF_BITS                                       1
#define REG_Ssd_xr_vds_MIN                                               0
#define REG_Ssd_xr_vds_MAX                                               1
#define REG_SSD_XR_VDS_VALUE_OFF                                         0
#define REG_SSD_XR_VDS_VALUE_ON                                          1

/*!
 * @brief ssd software versioning enable bits:
 *        [1] SRS/TruSurround passiv
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_srs_ts_pa
#define REG_Ssd_xr_srs_ts_pa_OFFSET                                        0x03FEC
#define REG_Ssd_xr_srs_ts_pa_MASK                                        0x1
#define REG_Ssd_xr_srs_ts_pa_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_xr_srs_ts_pa_BIT_MASK                                    0x2
#define REG_Ssd_xr_srs_ts_pa_BIT_IDX                                     1
#define REG_Ssd_xr_srs_ts_pa_NUM_OF_BITS                                 1
#define REG_Ssd_xr_srs_ts_pa_MIN                                         0
#define REG_Ssd_xr_srs_ts_pa_MAX                                         1
#define REG_SSD_XR_SRS_TS_PA_VALUE_OFF                                   0
#define REG_SSD_XR_SRS_TS_PA_VALUE_ON                                    1

/*!
 * @brief ssd software versioning enable bits:
 *        [2] SRS 3D Sound
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_srs_3ds
#define REG_Ssd_xr_srs_3ds_OFFSET                                          0x03FEC
#define REG_Ssd_xr_srs_3ds_MASK                                          0x1
#define REG_Ssd_xr_srs_3ds_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_xr_srs_3ds_BIT_MASK                                      0x4
#define REG_Ssd_xr_srs_3ds_BIT_IDX                                       2
#define REG_Ssd_xr_srs_3ds_NUM_OF_BITS                                   1
#define REG_Ssd_xr_srs_3ds_MIN                                           0
#define REG_Ssd_xr_srs_3ds_MAX                                           1
#define REG_SSD_XR_SRS_3DS_VALUE_OFF                                     0
#define REG_SSD_XR_SRS_3DS_VALUE_ON                                      1

/*!
 * @brief Reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_xreg_iis_control_79
#define REG_Ssd_rsd_xreg_iis_control_79_OFFSET                             0x03FEC
#define REG_Ssd_rsd_xreg_iis_control_79_MASK                             0x1
#define REG_Ssd_rsd_xreg_iis_control_79_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_rsd_xreg_iis_control_79_BIT_MASK                         0x8
#define REG_Ssd_rsd_xreg_iis_control_79_BIT_IDX                          3
#define REG_Ssd_rsd_xreg_iis_control_79_NUM_OF_BITS                      1
#define REG_Ssd_rsd_xreg_iis_control_79_MIN                              0
#define REG_Ssd_rsd_xreg_iis_control_79_MAX                              1

/*!
 * @brief ssd software versioning enable bits:
 *        [4] BBE
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_bbe
#define REG_Ssd_xr_bbe_OFFSET                                              0x03FEC
#define REG_Ssd_xr_bbe_MASK                                              0x1
#define REG_Ssd_xr_bbe_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_bbe_BIT_MASK                                          0x10
#define REG_Ssd_xr_bbe_BIT_IDX                                           4
#define REG_Ssd_xr_bbe_NUM_OF_BITS                                       1
#define REG_Ssd_xr_bbe_MIN                                               0
#define REG_Ssd_xr_bbe_MAX                                               1
#define REG_SSD_XR_BBE_VALUE_OFF                                         0
#define REG_SSD_XR_BBE_VALUE_ON                                          1

/*!
 * @brief ssd software versioning enable bits:
 *        [5] DBX
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_dbx
#define REG_Ssd_xr_dbx_OFFSET                                              0x03FEC
#define REG_Ssd_xr_dbx_MASK                                              0x1
#define REG_Ssd_xr_dbx_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_dbx_BIT_MASK                                          0x20
#define REG_Ssd_xr_dbx_BIT_IDX                                           5
#define REG_Ssd_xr_dbx_NUM_OF_BITS                                       1
#define REG_Ssd_xr_dbx_MIN                                               0
#define REG_Ssd_xr_dbx_MAX                                               1
#define REG_SSD_XR_DBX_VALUE_OFF                                         0
#define REG_SSD_XR_DBX_VALUE_ON                                          1

/*!
 * @brief ssd software versioning enable bits:
 *        [6] FM Radio
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_fmr
#define REG_Ssd_xr_fmr_OFFSET                                              0x03FEC
#define REG_Ssd_xr_fmr_MASK                                              0x1
#define REG_Ssd_xr_fmr_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_xr_fmr_BIT_MASK                                          0x40
#define REG_Ssd_xr_fmr_BIT_IDX                                           6
#define REG_Ssd_xr_fmr_NUM_OF_BITS                                       1
#define REG_Ssd_xr_fmr_MIN                                               0
#define REG_Ssd_xr_fmr_MAX                                               1
#define REG_SSD_XR_FMR_VALUE_OFF                                         0
#define REG_SSD_XR_FMR_VALUE_ON                                          1

/*!
 * @brief ssd software versioning enable bits:
 *        [7] SRS/TruSurround active (was sound8)
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_srs_ts_ac
#define REG_Ssd_xr_srs_ts_ac_OFFSET                                        0x03FEC
#define REG_Ssd_xr_srs_ts_ac_MASK                                        0x1
#define REG_Ssd_xr_srs_ts_ac_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_xr_srs_ts_ac_BIT_MASK                                    0x80
#define REG_Ssd_xr_srs_ts_ac_BIT_IDX                                     7
#define REG_Ssd_xr_srs_ts_ac_NUM_OF_BITS                                 1
#define REG_Ssd_xr_srs_ts_ac_MIN                                         0
#define REG_Ssd_xr_srs_ts_ac_MAX                                         1
#define REG_SSD_XR_SRS_TS_AC_VALUE_OFF                                   0
#define REG_SSD_XR_SRS_TS_AC_VALUE_ON                                    1

/*!
 * @brief ssd software versioning enable bits:
 *        [8] DEMDEC on (FM stereo decoding, also known as 'ssd_on')
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_demdec
#define REG_Ssd_xr_demdec_OFFSET                                           0x03FEC
#define REG_Ssd_xr_demdec_MASK                                           0x1
#define REG_Ssd_xr_demdec_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_xr_demdec_BIT_MASK                                       0x100
#define REG_Ssd_xr_demdec_BIT_IDX                                        8
#define REG_Ssd_xr_demdec_NUM_OF_BITS                                    1
#define REG_Ssd_xr_demdec_MIN                                            0
#define REG_Ssd_xr_demdec_MAX                                            1
#define REG_SSD_XR_DEMDEC_VALUE_OFF                                      0
#define REG_SSD_XR_DEMDEC_VALUE_ON                                       1

/*!
 * @brief ssd software versioning enable bits:
 *        [9] standard
 *        $0 = BTSC only
 *        $1 = Multi
 * @range 0..1
 ***/
#define REG_Ssd_xr_multi_btsc
#define REG_Ssd_xr_multi_btsc_OFFSET                                       0x03FEC
#define REG_Ssd_xr_multi_btsc_MASK                                       0x1
#define REG_Ssd_xr_multi_btsc_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_xr_multi_btsc_BIT_MASK                                   0x200
#define REG_Ssd_xr_multi_btsc_BIT_IDX                                    9
#define REG_Ssd_xr_multi_btsc_NUM_OF_BITS                                1
#define REG_Ssd_xr_multi_btsc_MIN                                        0
#define REG_Ssd_xr_multi_btsc_MAX                                        1
#define REG_SSD_XR_MULTI_BTSC_VALUE_BTSC_ONLY                            0
#define REG_SSD_XR_MULTI_BTSC_VALUE_MULTI                                1

/*!
 * @brief ssd software versioning enable bits:
 *        [10] SRS/WOW active
 *        $0 = off
 *        $1 = on
 * @range 0..1
 ***/
#define REG_Ssd_xr_srs_wow
#define REG_Ssd_xr_srs_wow_OFFSET                                          0x03FEC
#define REG_Ssd_xr_srs_wow_MASK                                          0x1
#define REG_Ssd_xr_srs_wow_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_xr_srs_wow_BIT_MASK                                      0x400
#define REG_Ssd_xr_srs_wow_BIT_IDX                                       10
#define REG_Ssd_xr_srs_wow_NUM_OF_BITS                                   1
#define REG_Ssd_xr_srs_wow_MIN                                           0
#define REG_Ssd_xr_srs_wow_MAX                                           1
#define REG_SSD_XR_SRS_WOW_VALUE_OFF                                     0
#define REG_SSD_XR_SRS_WOW_VALUE_ON                                      1

/*!
 * @brief Reserved
 * @range 0..8191
 ***/
#define REG_Ssd_rsd_xreg_swv_96
#define REG_Ssd_rsd_xreg_swv_96_OFFSET                                     0x03FEC
#define REG_Ssd_rsd_xreg_swv_96_MASK                                     0x1fff
#define REG_Ssd_rsd_xreg_swv_96_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_xreg_swv_96_BIT_MASK                                 0xfff800
#define REG_Ssd_rsd_xreg_swv_96_BIT_IDX                                  11
#define REG_Ssd_rsd_xreg_swv_96_NUM_OF_BITS                              13
#define REG_Ssd_rsd_xreg_swv_96_MIN                                      0
#define REG_Ssd_rsd_xreg_swv_96_MAX                                      8191

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_swv_98
#define REG_Ssd_rsd_xreg_swv_98_OFFSET                                     0x03FEC
#define REG_Ssd_rsd_xreg_swv_98_MASK                                     0xff
#define REG_Ssd_rsd_xreg_swv_98_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_xreg_swv_98_BIT_MASK                                 0xff000000
#define REG_Ssd_rsd_xreg_swv_98_BIT_IDX                                  24
#define REG_Ssd_rsd_xreg_swv_98_NUM_OF_BITS                              8
#define REG_Ssd_rsd_xreg_swv_98_MIN                                      0
#define REG_Ssd_rsd_xreg_swv_98_MAX                                      255

/*!
 * @brief Identification mixer frequency
 *        $0 = reserved
 *        $212F= Ident pilot frequency Europe
 *        $216B = Ident pilot frequency Korea
 * @range 0..65535
 ***/
#define REG_Ssd_idmix_freq
#define REG_Ssd_idmix_freq_OFFSET                                          0x03FF0
#define REG_Ssd_idmix_freq_MASK                                          0xffff
#define REG_Ssd_idmix_freq_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_idmix_freq_BIT_MASK                                      0xffff
#define REG_Ssd_idmix_freq_BIT_IDX                                       0
#define REG_Ssd_idmix_freq_NUM_OF_BITS                                   16
#define REG_Ssd_idmix_freq_MIN                                           0
#define REG_Ssd_idmix_freq_MAX                                           65535
#define REG_SSD_IDMIX_FREQ_VALUE_RESERVED                                0
#define REG_SSD_IDMIX_FREQ_VALUE_IDENT_PILOT_FREQUENCY_EUROPE            8495
#define REG_SSD_IDMIX_FREQ_VALUE_IDENT_PILOT_FREQUENCY_KOREA             8555

/*!
 * @brief Reserved
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_idmix_106
#define REG_Ssd_rsd_xreg_idmix_106_OFFSET                                  0x03FF0
#define REG_Ssd_rsd_xreg_idmix_106_MASK                                  0xff
#define REG_Ssd_rsd_xreg_idmix_106_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_xreg_idmix_106_BIT_MASK                              0xff0000
#define REG_Ssd_rsd_xreg_idmix_106_BIT_IDX                               16
#define REG_Ssd_rsd_xreg_idmix_106_NUM_OF_BITS                           8
#define REG_Ssd_rsd_xreg_idmix_106_MIN                                   0
#define REG_Ssd_rsd_xreg_idmix_106_MAX                                   255

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_idmix_107
#define REG_Ssd_rsd_xreg_idmix_107_OFFSET                                  0x03FF0
#define REG_Ssd_rsd_xreg_idmix_107_MASK                                  0xff
#define REG_Ssd_rsd_xreg_idmix_107_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_xreg_idmix_107_BIT_MASK                              0xff000000
#define REG_Ssd_rsd_xreg_idmix_107_BIT_IDX                               24
#define REG_Ssd_rsd_xreg_idmix_107_NUM_OF_BITS                           8
#define REG_Ssd_rsd_xreg_idmix_107_MIN                                   0
#define REG_Ssd_rsd_xreg_idmix_107_MAX                                   255

/*!
 * @brief Silence Detection Status Aux1
 *        $0 = Un-Muted
 *        $1= Muted
 * @range 0..1
 ***/
#define REG_Ssd_sild_aux1_sta
#define REG_Ssd_sild_aux1_sta_OFFSET                                       0x03FF4
#define REG_Ssd_sild_aux1_sta_MASK                                       0x1
#define REG_Ssd_sild_aux1_sta_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sild_aux1_sta_BIT_MASK                                   0x1
#define REG_Ssd_sild_aux1_sta_BIT_IDX                                    0
#define REG_Ssd_sild_aux1_sta_NUM_OF_BITS                                1
#define REG_Ssd_sild_aux1_sta_MIN                                        0
#define REG_Ssd_sild_aux1_sta_MAX                                        1
#define REG_SSD_SILD_AUX1_STA_VALUE_UN_MUTED                             0
#define REG_SSD_SILD_AUX1_STA_VALUE_MUTED                                1

/*!
 * @brief Silence Detection Status Main
 *        $0 = Un-Muted
 *        $1= Muted
 * @range 0..1
 ***/
#define REG_Ssd_sild_main_sta
#define REG_Ssd_sild_main_sta_OFFSET                                       0x03FF4
#define REG_Ssd_sild_main_sta_MASK                                       0x1
#define REG_Ssd_sild_main_sta_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sild_main_sta_BIT_MASK                                   0x2
#define REG_Ssd_sild_main_sta_BIT_IDX                                    1
#define REG_Ssd_sild_main_sta_NUM_OF_BITS                                1
#define REG_Ssd_sild_main_sta_MIN                                        0
#define REG_Ssd_sild_main_sta_MAX                                        1
#define REG_SSD_SILD_MAIN_STA_VALUE_UN_MUTED                             0
#define REG_SSD_SILD_MAIN_STA_VALUE_MUTED                                1

/*!
 * @brief Reserved
 * @range 0..4194303
 ***/
#define REG_Ssd_rsd_xreg_misc_103
#define REG_Ssd_rsd_xreg_misc_103_OFFSET                                   0x03FF4
#define REG_Ssd_rsd_xreg_misc_103_MASK                                   0x3fffff
#define REG_Ssd_rsd_xreg_misc_103_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_misc_103_BIT_MASK                               0xfffffc
#define REG_Ssd_rsd_xreg_misc_103_BIT_IDX                                2
#define REG_Ssd_rsd_xreg_misc_103_NUM_OF_BITS                            22
#define REG_Ssd_rsd_xreg_misc_103_MIN                                    0
#define REG_Ssd_rsd_xreg_misc_103_MAX                                    4194303

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_xreg_misc_104
#define REG_Ssd_rsd_xreg_misc_104_OFFSET                                   0x03FF4
#define REG_Ssd_rsd_xreg_misc_104_MASK                                   0xff
#define REG_Ssd_rsd_xreg_misc_104_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_rsd_xreg_misc_104_BIT_MASK                               0xff000000
#define REG_Ssd_rsd_xreg_misc_104_BIT_IDX                                24
#define REG_Ssd_rsd_xreg_misc_104_NUM_OF_BITS                            8
#define REG_Ssd_rsd_xreg_misc_104_MIN                                    0
#define REG_Ssd_rsd_xreg_misc_104_MAX                                    255

/*!
 * @brief EIR_BUS[31..24]
 * @range 0..255
 ***/
#define REG_Ssd_eir_bus_msb
#define REG_Ssd_eir_bus_msb_OFFSET                                         0x180A8
#define REG_Ssd_eir_bus_msb_MASK                                         0xff
#define REG_Ssd_eir_bus_msb_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_eir_bus_msb_BIT_MASK                                     0xff
#define REG_Ssd_eir_bus_msb_BIT_IDX                                      0
#define REG_Ssd_eir_bus_msb_NUM_OF_BITS                                  8
#define REG_Ssd_eir_bus_msb_MIN                                          0
#define REG_Ssd_eir_bus_msb_MAX                                          255

/*!
 * @brief TCB_GOTO_MODE
 * @range 0..1
 ***/
#define REG_Ssd_tcb_goto_mode
#define REG_Ssd_tcb_goto_mode_OFFSET                                       0x180A8
#define REG_Ssd_tcb_goto_mode_MASK                                       0x1
#define REG_Ssd_tcb_goto_mode_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_tcb_goto_mode_BIT_MASK                                   0x100
#define REG_Ssd_tcb_goto_mode_BIT_IDX                                    8
#define REG_Ssd_tcb_goto_mode_NUM_OF_BITS                                1
#define REG_Ssd_tcb_goto_mode_MIN                                        0
#define REG_Ssd_tcb_goto_mode_MAX                                        1

/*!
 * @brief reserved
 * @range 0..32767
 ***/
#define REG_Ssd_rsd_testreg4_7
#define REG_Ssd_rsd_testreg4_7_OFFSET                                      0x180A8
#define REG_Ssd_rsd_testreg4_7_MASK                                      0x7fff
#define REG_Ssd_rsd_testreg4_7_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_rsd_testreg4_7_BIT_MASK                                  0xfffe00
#define REG_Ssd_rsd_testreg4_7_BIT_IDX                                   9
#define REG_Ssd_rsd_testreg4_7_NUM_OF_BITS                               15
#define REG_Ssd_rsd_testreg4_7_MIN                                       0
#define REG_Ssd_rsd_testreg4_7_MAX                                       32767

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_testreg4_8
#define REG_Ssd_rsd_testreg4_8_OFFSET                                      0x180A8
#define REG_Ssd_rsd_testreg4_8_MASK                                      0xff
#define REG_Ssd_rsd_testreg4_8_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_rsd_testreg4_8_BIT_MASK                                  0xff000000
#define REG_Ssd_rsd_testreg4_8_BIT_IDX                                   24
#define REG_Ssd_rsd_testreg4_8_NUM_OF_BITS                               8
#define REG_Ssd_rsd_testreg4_8_MIN                                       0
#define REG_Ssd_rsd_testreg4_8_MAX                                       255

/*!
 * @brief EIR_BUS[23..0]
 * @range 0..16777215
 ***/
#define REG_Ssd_eir_bus_lsb
#define REG_Ssd_eir_bus_lsb_OFFSET                                         0x180AC
#define REG_Ssd_eir_bus_lsb_MASK                                         0xffffff
#define REG_Ssd_eir_bus_lsb_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_eir_bus_lsb_BIT_MASK                                     0xffffff
#define REG_Ssd_eir_bus_lsb_BIT_IDX                                      0
#define REG_Ssd_eir_bus_lsb_NUM_OF_BITS                                  24
#define REG_Ssd_eir_bus_lsb_MIN                                          0
#define REG_Ssd_eir_bus_lsb_MAX                                          16777215

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_testreg3_11
#define REG_Ssd_rsd_testreg3_11_OFFSET                                     0x180AC
#define REG_Ssd_rsd_testreg3_11_MASK                                     0xff
#define REG_Ssd_rsd_testreg3_11_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg3_11_BIT_MASK                                 0xff000000
#define REG_Ssd_rsd_testreg3_11_BIT_IDX                                  24
#define REG_Ssd_rsd_testreg3_11_NUM_OF_BITS                              8
#define REG_Ssd_rsd_testreg3_11_MIN                                      0
#define REG_Ssd_rsd_testreg3_11_MAX                                      255

/*!
 * @brief sine wave generator in BTSC
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_sinegen
#define REG_Ssd_sinegen_OFFSET                                             0x180B0
#define REG_Ssd_sinegen_MASK                                             0x1
#define REG_Ssd_sinegen_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_sinegen_BIT_MASK                                         0x1
#define REG_Ssd_sinegen_BIT_IDX                                          0
#define REG_Ssd_sinegen_NUM_OF_BITS                                      1
#define REG_Ssd_sinegen_MIN                                              0
#define REG_Ssd_sinegen_MAX                                              1
#define REG_SSD_SINEGEN_VALUE_OFF                                        0
#define REG_SSD_SINEGEN_VALUE_ON                                         1

/*!
 * @brief MUX III
 *        0 = normal operation
 *        1 = f_mix1 + OutB
 * @range 0..1
 ***/
#define REG_Ssd_intl_fm_freq1
#define REG_Ssd_intl_fm_freq1_OFFSET                                       0x180B0
#define REG_Ssd_intl_fm_freq1_MASK                                       0x1
#define REG_Ssd_intl_fm_freq1_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_intl_fm_freq1_BIT_MASK                                   0x2
#define REG_Ssd_intl_fm_freq1_BIT_IDX                                    1
#define REG_Ssd_intl_fm_freq1_NUM_OF_BITS                                1
#define REG_Ssd_intl_fm_freq1_MIN                                        0
#define REG_Ssd_intl_fm_freq1_MAX                                        1
#define REG_SSD_INTL_FM_FREQ1_VALUE_NORMAL_OPERATION                     0
#define REG_SSD_INTL_FM_FREQ1_VALUE_F_MIX1_OUTB                          1

/*!
 * @brief MUX VI
 *        0 = normal operation
 *        1 = f_mix2 + OutA
 * @range 0..1
 ***/
#define REG_Ssd_intl_fm_freq2
#define REG_Ssd_intl_fm_freq2_OFFSET                                       0x180B0
#define REG_Ssd_intl_fm_freq2_MASK                                       0x1
#define REG_Ssd_intl_fm_freq2_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_intl_fm_freq2_BIT_MASK                                   0x4
#define REG_Ssd_intl_fm_freq2_BIT_IDX                                    2
#define REG_Ssd_intl_fm_freq2_NUM_OF_BITS                                1
#define REG_Ssd_intl_fm_freq2_MIN                                        0
#define REG_Ssd_intl_fm_freq2_MAX                                        1
#define REG_SSD_INTL_FM_FREQ2_VALUE_NORMAL_OPERATION                     0
#define REG_SSD_INTL_FM_FREQ2_VALUE_F_MIX2_OUTA                          1

/*!
 * @brief MUX II (ch. 1)
 *        0 = Hilbert filter
 *        1 = MAGN1
 * @range 0..1
 ***/
#define REG_Ssd_intl_fm_magn1
#define REG_Ssd_intl_fm_magn1_OFFSET                                       0x180B0
#define REG_Ssd_intl_fm_magn1_MASK                                       0x1
#define REG_Ssd_intl_fm_magn1_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_intl_fm_magn1_BIT_MASK                                   0x8
#define REG_Ssd_intl_fm_magn1_BIT_IDX                                    3
#define REG_Ssd_intl_fm_magn1_NUM_OF_BITS                                1
#define REG_Ssd_intl_fm_magn1_MIN                                        0
#define REG_Ssd_intl_fm_magn1_MAX                                        1
#define REG_SSD_INTL_FM_MAGN1_VALUE_HILBERT_FILTER                       0
#define REG_SSD_INTL_FM_MAGN1_VALUE_MAGN1                                1

/*!
 * @brief MUX V (ch. 2)
 *        0 = Hilbert filter
 *        1 = MAGN2
 * @range 0..1
 ***/
#define REG_Ssd_intl_fm_magn2
#define REG_Ssd_intl_fm_magn2_OFFSET                                       0x180B0
#define REG_Ssd_intl_fm_magn2_MASK                                       0x1
#define REG_Ssd_intl_fm_magn2_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_intl_fm_magn2_BIT_MASK                                   0x10
#define REG_Ssd_intl_fm_magn2_BIT_IDX                                    4
#define REG_Ssd_intl_fm_magn2_NUM_OF_BITS                                1
#define REG_Ssd_intl_fm_magn2_MIN                                        0
#define REG_Ssd_intl_fm_magn2_MAX                                        1
#define REG_SSD_INTL_FM_MAGN2_VALUE_HILBERT_FILTER                       0
#define REG_SSD_INTL_FM_MAGN2_VALUE_MAGN2                                1

/*!
 * @brief reserved
 * @range 0..7
 ***/
#define REG_Ssd_rsd_testreg2_18
#define REG_Ssd_rsd_testreg2_18_OFFSET                                     0x180B0
#define REG_Ssd_rsd_testreg2_18_MASK                                     0x7
#define REG_Ssd_rsd_testreg2_18_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg2_18_BIT_MASK                                 0xe0
#define REG_Ssd_rsd_testreg2_18_BIT_IDX                                  5
#define REG_Ssd_rsd_testreg2_18_NUM_OF_BITS                              3
#define REG_Ssd_rsd_testreg2_18_MIN                                      0
#define REG_Ssd_rsd_testreg2_18_MAX                                      7

/*!
 * @brief channel 1 CORDIC 2nd magnitude input
 * @range 0..255
 ***/
#define REG_Ssd_magn1
#define REG_Ssd_magn1_OFFSET                                               0x180B0
#define REG_Ssd_magn1_MASK                                               0xff
#define REG_Ssd_magn1_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_magn1_BIT_MASK                                           0xff00
#define REG_Ssd_magn1_BIT_IDX                                            8
#define REG_Ssd_magn1_NUM_OF_BITS                                        8
#define REG_Ssd_magn1_MIN                                                0
#define REG_Ssd_magn1_MAX                                                255

/*!
 * @brief channel 2 CORDIC 2nd magnitude input
 * @range 0..255
 ***/
#define REG_Ssd_magn2
#define REG_Ssd_magn2_OFFSET                                               0x180B0
#define REG_Ssd_magn2_MASK                                               0xff
#define REG_Ssd_magn2_SIGNED_BIT_MASK                                    0x0
#define REG_Ssd_magn2_BIT_MASK                                           0xff0000
#define REG_Ssd_magn2_BIT_IDX                                            16
#define REG_Ssd_magn2_NUM_OF_BITS                                        8
#define REG_Ssd_magn2_MIN                                                0
#define REG_Ssd_magn2_MAX                                                255

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_testreg2_21
#define REG_Ssd_rsd_testreg2_21_OFFSET                                     0x180B0
#define REG_Ssd_rsd_testreg2_21_MASK                                     0xff
#define REG_Ssd_rsd_testreg2_21_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg2_21_BIT_MASK                                 0xff000000
#define REG_Ssd_rsd_testreg2_21_BIT_IDX                                  24
#define REG_Ssd_rsd_testreg2_21_NUM_OF_BITS                              8
#define REG_Ssd_rsd_testreg2_21_MIN                                      0
#define REG_Ssd_rsd_testreg2_21_MAX                                      255

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [0] VDS Virtual Dolby Surround
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_vds_sta
#define REG_Ssd_vds_sta_OFFSET                                             0x180B4
#define REG_Ssd_vds_sta_MASK                                             0x1
#define REG_Ssd_vds_sta_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_vds_sta_BIT_MASK                                         0x1
#define REG_Ssd_vds_sta_BIT_IDX                                          0
#define REG_Ssd_vds_sta_NUM_OF_BITS                                      1
#define REG_Ssd_vds_sta_MIN                                              0
#define REG_Ssd_vds_sta_MAX                                              1
#define REG_SSD_VDS_STA_VALUE_OFF                                        0
#define REG_SSD_VDS_STA_VALUE_ON                                         1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [1] SRS/TruSurround passiv
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_srs_ts_pa_sta
#define REG_Ssd_srs_ts_pa_sta_OFFSET                                       0x180B4
#define REG_Ssd_srs_ts_pa_sta_MASK                                       0x1
#define REG_Ssd_srs_ts_pa_sta_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_srs_ts_pa_sta_BIT_MASK                                   0x2
#define REG_Ssd_srs_ts_pa_sta_BIT_IDX                                    1
#define REG_Ssd_srs_ts_pa_sta_NUM_OF_BITS                                1
#define REG_Ssd_srs_ts_pa_sta_MIN                                        0
#define REG_Ssd_srs_ts_pa_sta_MAX                                        1
#define REG_SSD_SRS_TS_PA_STA_VALUE_OFF                                  0
#define REG_SSD_SRS_TS_PA_STA_VALUE_ON                                   1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [2] SRS 3D Sound
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_srs_3ds_sta
#define REG_Ssd_srs_3ds_sta_OFFSET                                         0x180B4
#define REG_Ssd_srs_3ds_sta_MASK                                         0x1
#define REG_Ssd_srs_3ds_sta_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_srs_3ds_sta_BIT_MASK                                     0x4
#define REG_Ssd_srs_3ds_sta_BIT_IDX                                      2
#define REG_Ssd_srs_3ds_sta_NUM_OF_BITS                                  1
#define REG_Ssd_srs_3ds_sta_MIN                                          0
#define REG_Ssd_srs_3ds_sta_MAX                                          1
#define REG_SSD_SRS_3DS_STA_VALUE_OFF                                    0
#define REG_SSD_SRS_3DS_STA_VALUE_ON                                     1

/*!
 * @brief reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_testreg2_22
#define REG_Ssd_rsd_testreg2_22_OFFSET                                     0x180B4
#define REG_Ssd_rsd_testreg2_22_MASK                                     0x1
#define REG_Ssd_rsd_testreg2_22_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg2_22_BIT_MASK                                 0x8
#define REG_Ssd_rsd_testreg2_22_BIT_IDX                                  3
#define REG_Ssd_rsd_testreg2_22_NUM_OF_BITS                              1
#define REG_Ssd_rsd_testreg2_22_MIN                                      0
#define REG_Ssd_rsd_testreg2_22_MAX                                      1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [4] BBE
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_bbe_sta
#define REG_Ssd_bbe_sta_OFFSET                                             0x180B4
#define REG_Ssd_bbe_sta_MASK                                             0x1
#define REG_Ssd_bbe_sta_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_bbe_sta_BIT_MASK                                         0x10
#define REG_Ssd_bbe_sta_BIT_IDX                                          4
#define REG_Ssd_bbe_sta_NUM_OF_BITS                                      1
#define REG_Ssd_bbe_sta_MIN                                              0
#define REG_Ssd_bbe_sta_MAX                                              1
#define REG_SSD_BBE_STA_VALUE_OFF                                        0
#define REG_SSD_BBE_STA_VALUE_ON                                         1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [5] DBX
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_dbx_sta
#define REG_Ssd_dbx_sta_OFFSET                                             0x180B4
#define REG_Ssd_dbx_sta_MASK                                             0x1
#define REG_Ssd_dbx_sta_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_dbx_sta_BIT_MASK                                         0x20
#define REG_Ssd_dbx_sta_BIT_IDX                                          5
#define REG_Ssd_dbx_sta_NUM_OF_BITS                                      1
#define REG_Ssd_dbx_sta_MIN                                              0
#define REG_Ssd_dbx_sta_MAX                                              1
#define REG_SSD_DBX_STA_VALUE_OFF                                        0
#define REG_SSD_DBX_STA_VALUE_ON                                         1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [6] FM Radio
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_fmr_sta
#define REG_Ssd_fmr_sta_OFFSET                                             0x180B4
#define REG_Ssd_fmr_sta_MASK                                             0x1
#define REG_Ssd_fmr_sta_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_fmr_sta_BIT_MASK                                         0x40
#define REG_Ssd_fmr_sta_BIT_IDX                                          6
#define REG_Ssd_fmr_sta_NUM_OF_BITS                                      1
#define REG_Ssd_fmr_sta_MIN                                              0
#define REG_Ssd_fmr_sta_MAX                                              1
#define REG_SSD_FMR_STA_VALUE_OFF                                        0
#define REG_SSD_FMR_STA_VALUE_ON                                         1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [7] SRS/TruSurround active (was sound8)
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_srs_ts_ac_sta
#define REG_Ssd_srs_ts_ac_sta_OFFSET                                       0x180B4
#define REG_Ssd_srs_ts_ac_sta_MASK                                       0x1
#define REG_Ssd_srs_ts_ac_sta_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_srs_ts_ac_sta_BIT_MASK                                   0x80
#define REG_Ssd_srs_ts_ac_sta_BIT_IDX                                    7
#define REG_Ssd_srs_ts_ac_sta_NUM_OF_BITS                                1
#define REG_Ssd_srs_ts_ac_sta_MIN                                        0
#define REG_Ssd_srs_ts_ac_sta_MAX                                        1
#define REG_SSD_SRS_TS_AC_STA_VALUE_OFF                                  0
#define REG_SSD_SRS_TS_AC_STA_VALUE_ON                                   1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [8] DEMDEC on (FM stereo decoding, also known as 'ssd_on')
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_demdec_sta
#define REG_Ssd_demdec_sta_OFFSET                                          0x180B4
#define REG_Ssd_demdec_sta_MASK                                          0x1
#define REG_Ssd_demdec_sta_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_demdec_sta_BIT_MASK                                      0x100
#define REG_Ssd_demdec_sta_BIT_IDX                                       8
#define REG_Ssd_demdec_sta_NUM_OF_BITS                                   1
#define REG_Ssd_demdec_sta_MIN                                           0
#define REG_Ssd_demdec_sta_MAX                                           1
#define REG_SSD_DEMDEC_STA_VALUE_OFF                                     0
#define REG_SSD_DEMDEC_STA_VALUE_ON                                      1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [9] standard
 *        0 = BTSC only
 *        1 = Multi
 * @range 0..1
 ***/
#define REG_Ssd_multi_btsc_sta
#define REG_Ssd_multi_btsc_sta_OFFSET                                      0x180B4
#define REG_Ssd_multi_btsc_sta_MASK                                      0x1
#define REG_Ssd_multi_btsc_sta_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_multi_btsc_sta_BIT_MASK                                  0x200
#define REG_Ssd_multi_btsc_sta_BIT_IDX                                   9
#define REG_Ssd_multi_btsc_sta_NUM_OF_BITS                               1
#define REG_Ssd_multi_btsc_sta_MIN                                       0
#define REG_Ssd_multi_btsc_sta_MAX                                       1
#define REG_SSD_MULTI_BTSC_STA_VALUE_BTSC_ONLY                           0
#define REG_SSD_MULTI_BTSC_STA_VALUE_MULTI                               1

/*!
 * @brief ssd software standalone versioning enable bits :
 *        [10] SRS/WOW
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_srs_wow
#define REG_Ssd_srs_wow_OFFSET                                             0x180B4
#define REG_Ssd_srs_wow_MASK                                             0x1
#define REG_Ssd_srs_wow_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_srs_wow_BIT_MASK                                         0x400
#define REG_Ssd_srs_wow_BIT_IDX                                          10
#define REG_Ssd_srs_wow_NUM_OF_BITS                                      1
#define REG_Ssd_srs_wow_MIN                                              0
#define REG_Ssd_srs_wow_MAX                                              1
#define REG_SSD_SRS_WOW_VALUE_OFF                                        0
#define REG_SSD_SRS_WOW_VALUE_ON                                         1

/*!
 * @brief audio ADC phaseshift of 1fs between left/right channel:
 *        [11] ADC_1FS_PHASESHIFT
 *        0 = off
 *        1 = on
 * @range 0..1
 ***/
#define REG_Ssd_adc_1fs_phaseshift
#define REG_Ssd_adc_1fs_phaseshift_OFFSET                                  0x180B4
#define REG_Ssd_adc_1fs_phaseshift_MASK                                  0x1
#define REG_Ssd_adc_1fs_phaseshift_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_adc_1fs_phaseshift_BIT_MASK                              0x800
#define REG_Ssd_adc_1fs_phaseshift_BIT_IDX                               11
#define REG_Ssd_adc_1fs_phaseshift_NUM_OF_BITS                           1
#define REG_Ssd_adc_1fs_phaseshift_MIN                                   0
#define REG_Ssd_adc_1fs_phaseshift_MAX                                   1
#define REG_SSD_ADC_1FS_PHASESHIFT_VALUE_OFF                             0
#define REG_SSD_ADC_1FS_PHASESHIFT_VALUE_ON                              1

/*!
 * @brief reserved
 * @range 0..8191
 ***/
#define REG_Ssd_rsd_testreg1_33
#define REG_Ssd_rsd_testreg1_33_OFFSET                                     0x180B4
#define REG_Ssd_rsd_testreg1_33_MASK                                     0x1fff
#define REG_Ssd_rsd_testreg1_33_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg1_33_BIT_MASK                                 0x1fff000
#define REG_Ssd_rsd_testreg1_33_BIT_IDX                                  12
#define REG_Ssd_rsd_testreg1_33_NUM_OF_BITS                              13
#define REG_Ssd_rsd_testreg1_33_MIN                                      0
#define REG_Ssd_rsd_testreg1_33_MAX                                      8191

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_testreg1_34
#define REG_Ssd_rsd_testreg1_34_OFFSET                                     0x180B4
#define REG_Ssd_rsd_testreg1_34_MASK                                     0xff
#define REG_Ssd_rsd_testreg1_34_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg1_34_BIT_MASK                                 0xfe000000
#define REG_Ssd_rsd_testreg1_34_BIT_IDX                                  25
#define REG_Ssd_rsd_testreg1_34_NUM_OF_BITS                              8
#define REG_Ssd_rsd_testreg1_34_MIN                                      0
#define REG_Ssd_rsd_testreg1_34_MAX                                      255

/*!
 * @brief SPDIF right output valid
 *        $0 = valid data
 *        $1 = don't use data
 * @range 0..1
 ***/
#define REG_Ssd_spdifout_val_r
#define REG_Ssd_spdifout_val_r_OFFSET                                      0x180B8
#define REG_Ssd_spdifout_val_r_MASK                                      0x1
#define REG_Ssd_spdifout_val_r_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_spdifout_val_r_BIT_MASK                                  0x1
#define REG_Ssd_spdifout_val_r_BIT_IDX                                   0
#define REG_Ssd_spdifout_val_r_NUM_OF_BITS                               1
#define REG_Ssd_spdifout_val_r_MIN                                       0
#define REG_Ssd_spdifout_val_r_MAX                                       1
#define REG_SSD_SPDIFOUT_VAL_R_VALUE_VALID_DATA                          0
#define REG_SSD_SPDIFOUT_VAL_R_VALUE_DON_T_USE_DATA                      1

/*!
 * @brief SPDIF left output valid
 *        $0 = valid data
 *        $1 = don't use data
 * @range 0..1
 ***/
#define REG_Ssd_spdifout_val_l
#define REG_Ssd_spdifout_val_l_OFFSET                                      0x180B8
#define REG_Ssd_spdifout_val_l_MASK                                      0x1
#define REG_Ssd_spdifout_val_l_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_spdifout_val_l_BIT_MASK                                  0x2
#define REG_Ssd_spdifout_val_l_BIT_IDX                                   1
#define REG_Ssd_spdifout_val_l_NUM_OF_BITS                               1
#define REG_Ssd_spdifout_val_l_MIN                                       0
#define REG_Ssd_spdifout_val_l_MAX                                       1
#define REG_SSD_SPDIFOUT_VAL_L_VALUE_VALID_DATA                          0
#define REG_SSD_SPDIFOUT_VAL_L_VALUE_DON_T_USE_DATA                      1

/*!
 * @brief SPDIF output copy bit
 *        $0 = copy protection is asserted
 *        $1 = no copy protection
 * @range 0..1
 ***/
#define REG_Ssd_spdifout_copy
#define REG_Ssd_spdifout_copy_OFFSET                                       0x180B8
#define REG_Ssd_spdifout_copy_MASK                                       0x1
#define REG_Ssd_spdifout_copy_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_spdifout_copy_BIT_MASK                                   0x4
#define REG_Ssd_spdifout_copy_BIT_IDX                                    2
#define REG_Ssd_spdifout_copy_NUM_OF_BITS                                1
#define REG_Ssd_spdifout_copy_MIN                                        0
#define REG_Ssd_spdifout_copy_MAX                                        1
#define REG_SSD_SPDIFOUT_COPY_VALUE_COPY_PROTECTION_IS_ASSERTED          0
#define REG_SSD_SPDIFOUT_COPY_VALUE_NO_COPY_PROTECTION                   1

/*!
 * @brief SPDIF output category (IEC 958-3 consumer mode)
 * @range 0..7
 ***/
#define REG_Ssd_spdifout_cat
#define REG_Ssd_spdifout_cat_OFFSET                                        0x180B8
#define REG_Ssd_spdifout_cat_MASK                                        0x7
#define REG_Ssd_spdifout_cat_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_spdifout_cat_BIT_MASK                                    0x38
#define REG_Ssd_spdifout_cat_BIT_IDX                                     3
#define REG_Ssd_spdifout_cat_NUM_OF_BITS                                 3
#define REG_Ssd_spdifout_cat_MIN                                         0
#define REG_Ssd_spdifout_cat_MAX                                         7

/*!
 * @brief SPDIFIN bypass mode
 *        $0 = disable
 *        $1 = enable
 * @range 0..1
 ***/
#define REG_Ssd_spdifin_bypass
#define REG_Ssd_spdifin_bypass_OFFSET                                      0x180B8
#define REG_Ssd_spdifin_bypass_MASK                                      0x1
#define REG_Ssd_spdifin_bypass_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_spdifin_bypass_BIT_MASK                                  0x40
#define REG_Ssd_spdifin_bypass_BIT_IDX                                   6
#define REG_Ssd_spdifin_bypass_NUM_OF_BITS                               1
#define REG_Ssd_spdifin_bypass_MIN                                       0
#define REG_Ssd_spdifin_bypass_MAX                                       1
#define REG_SSD_SPDIFIN_BYPASS_VALUE_DISABLE                             0
#define REG_SSD_SPDIFIN_BYPASS_VALUE_ENABLE                              1

/*!
 * @brief SPIDIFIN source select
 *        $0 = HDMI
 *        $1 = external
 * @range 0..1
 ***/
#define REG_Ssd_spdifin_sel
#define REG_Ssd_spdifin_sel_OFFSET                                         0x180B8
#define REG_Ssd_spdifin_sel_MASK                                         0x1
#define REG_Ssd_spdifin_sel_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_spdifin_sel_BIT_MASK                                     0x80
#define REG_Ssd_spdifin_sel_BIT_IDX                                      7
#define REG_Ssd_spdifin_sel_NUM_OF_BITS                                  1
#define REG_Ssd_spdifin_sel_MIN                                          0
#define REG_Ssd_spdifin_sel_MAX                                          1
#define REG_SSD_SPDIFIN_SEL_VALUE_HDMI                                   0
#define REG_SSD_SPDIFIN_SEL_VALUE_EXTERNAL                               1

/*!
 * @brief I2SIN bypass mode
 *        $0 = disable
 *        $1 = enable
 * @range 0..1
 ***/
#define REG_Ssd_i2sin_bypass
#define REG_Ssd_i2sin_bypass_OFFSET                                        0x180B8
#define REG_Ssd_i2sin_bypass_MASK                                        0x1
#define REG_Ssd_i2sin_bypass_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_i2sin_bypass_BIT_MASK                                    0x100
#define REG_Ssd_i2sin_bypass_BIT_IDX                                     8
#define REG_Ssd_i2sin_bypass_NUM_OF_BITS                                 1
#define REG_Ssd_i2sin_bypass_MIN                                         0
#define REG_Ssd_i2sin_bypass_MAX                                         1
#define REG_SSD_I2SIN_BYPASS_VALUE_DISABLE                               0
#define REG_SSD_I2SIN_BYPASS_VALUE_ENABLE                                1

/*!
 * @brief I2SIN source select
 *        $0 = HDMI
 *        $1 = external
 * @range 0..1
 ***/
#define REG_Ssd_i2sin_sel
#define REG_Ssd_i2sin_sel_OFFSET                                           0x180B8
#define REG_Ssd_i2sin_sel_MASK                                           0x1
#define REG_Ssd_i2sin_sel_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_i2sin_sel_BIT_MASK                                       0x200
#define REG_Ssd_i2sin_sel_BIT_IDX                                        9
#define REG_Ssd_i2sin_sel_NUM_OF_BITS                                    1
#define REG_Ssd_i2sin_sel_MIN                                            0
#define REG_Ssd_i2sin_sel_MAX                                            1
#define REG_SSD_I2SIN_SEL_VALUE_HDMI                                     0
#define REG_SSD_I2SIN_SEL_VALUE_EXTERNAL                                 1

/*!
 * @brief I2S format control from external in
 *        $0 = Philips
 *        $1 = Sony
 *        $2 = Japanese 24 bit
 * @range 0..3
 ***/
#define REG_Ssd_i2sout_format_dac
#define REG_Ssd_i2sout_format_dac_OFFSET                                   0x180B8
#define REG_Ssd_i2sout_format_dac_MASK                                   0x3
#define REG_Ssd_i2sout_format_dac_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_i2sout_format_dac_BIT_MASK                               0xc00
#define REG_Ssd_i2sout_format_dac_BIT_IDX                                10
#define REG_Ssd_i2sout_format_dac_NUM_OF_BITS                            2
#define REG_Ssd_i2sout_format_dac_MIN                                    0
#define REG_Ssd_i2sout_format_dac_MAX                                    3
#define REG_SSD_I2SOUT_FORMAT_DAC_VALUE_PHILIPS                          0
#define REG_SSD_I2SOUT_FORMAT_DAC_VALUE_SONY                             1
#define REG_SSD_I2SOUT_FORMAT_DAC_VALUE_JAPANESE_24_BIT                  2

/*!
 * @brief I2S format control from external in
 *        $0 = Philips
 *        $1 = Sony
 *        $2 = Japanese 24 bit
 * @range 0..3
 ***/
#define REG_Ssd_i2sout_format_ext
#define REG_Ssd_i2sout_format_ext_OFFSET                                   0x180B8
#define REG_Ssd_i2sout_format_ext_MASK                                   0x3
#define REG_Ssd_i2sout_format_ext_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_i2sout_format_ext_BIT_MASK                               0x3000
#define REG_Ssd_i2sout_format_ext_BIT_IDX                                12
#define REG_Ssd_i2sout_format_ext_NUM_OF_BITS                            2
#define REG_Ssd_i2sout_format_ext_MIN                                    0
#define REG_Ssd_i2sout_format_ext_MAX                                    3
#define REG_SSD_I2SOUT_FORMAT_EXT_VALUE_PHILIPS                          0
#define REG_SSD_I2SOUT_FORMAT_EXT_VALUE_SONY                             1
#define REG_SSD_I2SOUT_FORMAT_EXT_VALUE_JAPANESE_24_BIT                  2

/*!
 * @brief I2S format control from external in
 *        $0 = Philips
 *        $1 = Sony
 *        $2 = Japanese 24 bit
 * @range 0..3
 ***/
#define REG_Ssd_i2sin_format_ext
#define REG_Ssd_i2sin_format_ext_OFFSET                                    0x180B8
#define REG_Ssd_i2sin_format_ext_MASK                                    0x3
#define REG_Ssd_i2sin_format_ext_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_i2sin_format_ext_BIT_MASK                                0xc000
#define REG_Ssd_i2sin_format_ext_BIT_IDX                                 14
#define REG_Ssd_i2sin_format_ext_NUM_OF_BITS                             2
#define REG_Ssd_i2sin_format_ext_MIN                                     0
#define REG_Ssd_i2sin_format_ext_MAX                                     3
#define REG_SSD_I2SIN_FORMAT_EXT_VALUE_PHILIPS                           0
#define REG_SSD_I2SIN_FORMAT_EXT_VALUE_SONY                              1
#define REG_SSD_I2SIN_FORMAT_EXT_VALUE_JAPANESE_24_BIT                   2

/*!
 * @brief Digital audio input selection
 *        0 = I2S
 *        1 = SPDIF
 * @range 0..1
 ***/
#define REG_Ssd_diginsel
#define REG_Ssd_diginsel_OFFSET                                            0x180B8
#define REG_Ssd_diginsel_MASK                                            0x1
#define REG_Ssd_diginsel_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_diginsel_BIT_MASK                                        0x10000
#define REG_Ssd_diginsel_BIT_IDX                                         16
#define REG_Ssd_diginsel_NUM_OF_BITS                                     1
#define REG_Ssd_diginsel_MIN                                             0
#define REG_Ssd_diginsel_MAX                                             1
#define REG_SSD_DIGINSEL_VALUE_I2S                                       0
#define REG_SSD_DIGINSEL_VALUE_SPDIF                                     1

/*!
 * @brief Divider setting for digital audio input
 *        0 = 1 fs mode (divider = 1)
 *        1 = 2 fs mode (divider = 2)
 *        2 = 4 fs mode (divider = 4)
 * @range 0..3
 ***/
#define REG_Ssd_digindiv
#define REG_Ssd_digindiv_OFFSET                                            0x180B8
#define REG_Ssd_digindiv_MASK                                            0x3
#define REG_Ssd_digindiv_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_digindiv_BIT_MASK                                        0x60000
#define REG_Ssd_digindiv_BIT_IDX                                         17
#define REG_Ssd_digindiv_NUM_OF_BITS                                     2
#define REG_Ssd_digindiv_MIN                                             0
#define REG_Ssd_digindiv_MAX                                             3
#define REG_SSD_DIGINDIV_VALUE_1_FS_MODE_DIVIDER_1                       0
#define REG_SSD_DIGINDIV_VALUE_2_FS_MODE_DIVIDER_2                       1
#define REG_SSD_DIGINDIV_VALUE_4_FS_MODE_DIVIDER_4                       2

/*!
 * @brief IISIN master mode select
 *        0 = slave (using external bclk / ws)
 *        1 = master (using internal clk64fs_n / clk_1fs)
 * @range 0..1
 ***/
#define REG_Ssd_sel_iisin_master
#define REG_Ssd_sel_iisin_master_OFFSET                                    0x180B8
#define REG_Ssd_sel_iisin_master_MASK                                    0x1
#define REG_Ssd_sel_iisin_master_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_sel_iisin_master_BIT_MASK                                0x80000
#define REG_Ssd_sel_iisin_master_BIT_IDX                                 19
#define REG_Ssd_sel_iisin_master_NUM_OF_BITS                             1
#define REG_Ssd_sel_iisin_master_MIN                                     0
#define REG_Ssd_sel_iisin_master_MAX                                     1
#define REG_SSD_SEL_IISIN_MASTER_VALUE_SLAVE_USING_EXTERNAL_BCLK_WS      0
#define REG_SSD_SEL_IISIN_MASTER_VALUE_MASTER_USING_INTERNAL_CLK64FS_N_CLK_1FS 1

/*!
 * @brief reserved
 * @range 0..15
 ***/
#define REG_Ssd_rsd_testreg0_36
#define REG_Ssd_rsd_testreg0_36_OFFSET                                     0x180B8
#define REG_Ssd_rsd_testreg0_36_MASK                                     0xf
#define REG_Ssd_rsd_testreg0_36_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg0_36_BIT_MASK                                 0xf00000
#define REG_Ssd_rsd_testreg0_36_BIT_IDX                                  20
#define REG_Ssd_rsd_testreg0_36_NUM_OF_BITS                              4
#define REG_Ssd_rsd_testreg0_36_MIN                                      0
#define REG_Ssd_rsd_testreg0_36_MAX                                      15

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_testreg0_37
#define REG_Ssd_rsd_testreg0_37_OFFSET                                     0x180B8
#define REG_Ssd_rsd_testreg0_37_MASK                                     0xff
#define REG_Ssd_rsd_testreg0_37_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_rsd_testreg0_37_BIT_MASK                                 0xff000000
#define REG_Ssd_rsd_testreg0_37_BIT_IDX                                  24
#define REG_Ssd_rsd_testreg0_37_NUM_OF_BITS                              8
#define REG_Ssd_rsd_testreg0_37_MIN                                      0
#define REG_Ssd_rsd_testreg0_37_MAX                                      255

/*!
 * @brief Switch DTL - IIC control (only via IIC interface)
 *        0 =DTL control
 *        1 = IIC debug mode
 * @range 0..1
 ***/
#define REG_Ssd_switch_dtl_iic
#define REG_Ssd_switch_dtl_iic_OFFSET                                      0x180BC
#define REG_Ssd_switch_dtl_iic_MASK                                      0x1
#define REG_Ssd_switch_dtl_iic_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_switch_dtl_iic_BIT_MASK                                  0x1
#define REG_Ssd_switch_dtl_iic_BIT_IDX                                   0
#define REG_Ssd_switch_dtl_iic_NUM_OF_BITS                               1
#define REG_Ssd_switch_dtl_iic_MIN                                       0
#define REG_Ssd_switch_dtl_iic_MAX                                       1
#define REG_SSD_SWITCH_DTL_IIC_VALUE_DTL_CONTROL                         0
#define REG_SSD_SWITCH_DTL_IIC_VALUE_IIC_DEBUG_MODE                      1

/*!
 * @brief bus allocation minimizer enable
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_bam_en
#define REG_Ssd_bam_en_OFFSET                                              0x180BC
#define REG_Ssd_bam_en_MASK                                              0x1
#define REG_Ssd_bam_en_SIGNED_BIT_MASK                                   0x0
#define REG_Ssd_bam_en_BIT_MASK                                          0x2
#define REG_Ssd_bam_en_BIT_IDX                                           1
#define REG_Ssd_bam_en_NUM_OF_BITS                                       1
#define REG_Ssd_bam_en_MIN                                               0
#define REG_Ssd_bam_en_MAX                                               1
#define REG_SSD_BAM_EN_VALUE_DISABLED                                    0
#define REG_SSD_BAM_EN_VALUE_ENABLED                                     1

/*!
 * @brief unused
 * @range 0..1
 ***/
#define REG_Ssd_rsd_modecontrol_41
#define REG_Ssd_rsd_modecontrol_41_OFFSET                                  0x180BC
#define REG_Ssd_rsd_modecontrol_41_MASK                                  0x1
#define REG_Ssd_rsd_modecontrol_41_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_modecontrol_41_BIT_MASK                              0x4
#define REG_Ssd_rsd_modecontrol_41_BIT_IDX                               2
#define REG_Ssd_rsd_modecontrol_41_NUM_OF_BITS                           1
#define REG_Ssd_rsd_modecontrol_41_MIN                                   0
#define REG_Ssd_rsd_modecontrol_41_MAX                                   1

/*!
 * @brief unused
 * @range 0..1
 ***/
#define REG_Ssd_rsd_modecontrol_42
#define REG_Ssd_rsd_modecontrol_42_OFFSET                                  0x180BC
#define REG_Ssd_rsd_modecontrol_42_MASK                                  0x1
#define REG_Ssd_rsd_modecontrol_42_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_modecontrol_42_BIT_MASK                              0x8
#define REG_Ssd_rsd_modecontrol_42_BIT_IDX                               3
#define REG_Ssd_rsd_modecontrol_42_NUM_OF_BITS                           1
#define REG_Ssd_rsd_modecontrol_42_MIN                                   0
#define REG_Ssd_rsd_modecontrol_42_MAX                                   1

/*!
 * @brief reserved
 * @range 0..16383
 ***/
#define REG_Ssd_rsd_modecontrol_43
#define REG_Ssd_rsd_modecontrol_43_OFFSET                                  0x180BC
#define REG_Ssd_rsd_modecontrol_43_MASK                                  0x3fff
#define REG_Ssd_rsd_modecontrol_43_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_modecontrol_43_BIT_MASK                              0x3fff0
#define REG_Ssd_rsd_modecontrol_43_BIT_IDX                               4
#define REG_Ssd_rsd_modecontrol_43_NUM_OF_BITS                           14
#define REG_Ssd_rsd_modecontrol_43_MIN                                   0
#define REG_Ssd_rsd_modecontrol_43_MAX                                   16383

/*!
 * @brief SSD Hardware reset via DTL control interface
 *        0 = No reset
 *        1 = Reset
 * @range 0..1
 ***/
#define REG_Ssd_reset_hw
#define REG_Ssd_reset_hw_OFFSET                                            0x180BC
#define REG_Ssd_reset_hw_MASK                                            0x1
#define REG_Ssd_reset_hw_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_reset_hw_BIT_MASK                                        0x40000
#define REG_Ssd_reset_hw_BIT_IDX                                         18
#define REG_Ssd_reset_hw_NUM_OF_BITS                                     1
#define REG_Ssd_reset_hw_MIN                                             0
#define REG_Ssd_reset_hw_MAX                                             1
#define REG_SSD_RESET_HW_VALUE_NO_RESET                                  0
#define REG_SSD_RESET_HW_VALUE_RESET                                     1

/*!
 * @brief Selection bit for program ROM
 *        0 = PROM selected
 *        1 = PMEM selected
 * @range 0..1
 ***/
#define REG_Ssd_praprosel
#define REG_Ssd_praprosel_OFFSET                                           0x180BC
#define REG_Ssd_praprosel_MASK                                           0x1
#define REG_Ssd_praprosel_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_praprosel_BIT_MASK                                       0x80000
#define REG_Ssd_praprosel_BIT_IDX                                        19
#define REG_Ssd_praprosel_NUM_OF_BITS                                    1
#define REG_Ssd_praprosel_MIN                                            0
#define REG_Ssd_praprosel_MAX                                            1
#define REG_SSD_PRAPROSEL_VALUE_PROM_SELECTED                            0
#define REG_SSD_PRAPROSEL_VALUE_PMEM_SELECTED                            1

/*!
 * @brief Selection bit for PMEM
 *        0 = PRAM selected
 *        1 = BIOS ROM selected
 * @range 0..1
 ***/
#define REG_Ssd_bioprasel
#define REG_Ssd_bioprasel_OFFSET                                           0x180BC
#define REG_Ssd_bioprasel_MASK                                           0x1
#define REG_Ssd_bioprasel_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_bioprasel_BIT_MASK                                       0x100000
#define REG_Ssd_bioprasel_BIT_IDX                                        20
#define REG_Ssd_bioprasel_NUM_OF_BITS                                    1
#define REG_Ssd_bioprasel_MIN                                            0
#define REG_Ssd_bioprasel_MAX                                            1
#define REG_SSD_BIOPRASEL_VALUE_PRAM_SELECTED                            0
#define REG_SSD_BIOPRASEL_VALUE_BIOS_ROM_SELECTED                        1

/*!
 * @brief EPICS program counter reset bit
 *        0 = No reset
 *        1 = Reset
 * @range 0..1
 ***/
#define REG_Ssd_pc_reset
#define REG_Ssd_pc_reset_OFFSET                                            0x180BC
#define REG_Ssd_pc_reset_MASK                                            0x1
#define REG_Ssd_pc_reset_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_pc_reset_BIT_MASK                                        0x200000
#define REG_Ssd_pc_reset_BIT_IDX                                         21
#define REG_Ssd_pc_reset_NUM_OF_BITS                                     1
#define REG_Ssd_pc_reset_MIN                                             0
#define REG_Ssd_pc_reset_MAX                                             1
#define REG_SSD_PC_RESET_VALUE_NO_RESET                                  0
#define REG_SSD_PC_RESET_VALUE_RESET                                     1

/*!
 * @brief Address space selection (only valid via IIC interface)
 *        0 = Customer address space selected
 *        1 = Customer + expert address space selected
 *        2 = Full address space selected
 * @range 0..3
 ***/
#define REG_Ssd_addspasel
#define REG_Ssd_addspasel_OFFSET                                           0x180BC
#define REG_Ssd_addspasel_MASK                                           0x3
#define REG_Ssd_addspasel_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_addspasel_BIT_MASK                                       0xc00000
#define REG_Ssd_addspasel_BIT_IDX                                        22
#define REG_Ssd_addspasel_NUM_OF_BITS                                    2
#define REG_Ssd_addspasel_MIN                                            0
#define REG_Ssd_addspasel_MAX                                            3
#define REG_SSD_ADDSPASEL_VALUE_CUSTOMER_OFFSETESS_SPACE_SELECTED          0
#define REG_SSD_ADDSPASEL_VALUE_CUSTOMER_EXPERT_OFFSETESS_SPACE_SELECTED   1
#define REG_SSD_ADDSPASEL_VALUE_FULL_OFFSETESS_SPACE_SELECTED              2

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_modecontrol_49
#define REG_Ssd_rsd_modecontrol_49_OFFSET                                  0x180BC
#define REG_Ssd_rsd_modecontrol_49_MASK                                  0xff
#define REG_Ssd_rsd_modecontrol_49_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_rsd_modecontrol_49_BIT_MASK                              0xff000000
#define REG_Ssd_rsd_modecontrol_49_BIT_IDX                               24
#define REG_Ssd_rsd_modecontrol_49_NUM_OF_BITS                           8
#define REG_Ssd_rsd_modecontrol_49_MIN                                   0
#define REG_Ssd_rsd_modecontrol_49_MAX                                   255

/*!
 * @brief standard detection result (ASD mode), or selected standard in SSS mode
 *         0 = failed to find any standard
 *         1 = B/G (still searching, SC2 not (yet) found)
 *         2 = D/K (still searching, SC2 not (yet) found)
 *         3 = M (still searching, no ident or pilot found)
 *         4 = B/G A2
 *         5 = B/G NICAM
 *         6 = D/K A2 (1)
 *         7 = D/K A2 (2)
 *         8 = D/K A2 (3)
 *         9 = D/K NICAM
 *        10 = L NICAM
 *        11 = I NICAM
 *        12 = M Korea
 *        13 = M BTSC
 *        14 = M EIAJ
 *        15 = FM Radio, IF = 10.7 MHz, 50 us deemphasis
 *        16 = FM Radio, IF = 10.7 MHz, 75 us deemphasis
 *        17 = FM Radio, selectable IF, 50 us deemphasis
 *        18 = FM Radio, selectable IF, 75 us deemphasis
 *        31 = still searching for a standard (can occur only during a few milliseconds)
 * @range 0..31
 ***/
#define REG_Ssd_hwst_stdres
#define REG_Ssd_hwst_stdres_OFFSET                                         0x180C0
#define REG_Ssd_hwst_stdres_MASK                                         0x1f
#define REG_Ssd_hwst_stdres_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_hwst_stdres_BIT_MASK                                     0x1f
#define REG_Ssd_hwst_stdres_BIT_IDX                                      0
#define REG_Ssd_hwst_stdres_NUM_OF_BITS                                  5
#define REG_Ssd_hwst_stdres_MIN                                          0
#define REG_Ssd_hwst_stdres_MAX                                          31
#define REG_SSD_HWST_STDRES_VALUE_FAILED_TO_FIND_ANY_STANDARD            0
#define REG_SSD_HWST_STDRES_VALUE_B_G_STILL_SEARCHING_SC2_NOT_YET_FOUND  1
#define REG_SSD_HWST_STDRES_VALUE_D_K_STILL_SEARCHING_SC2_NOT_YET_FOUND  2
#define REG_SSD_HWST_STDRES_VALUE_M_STILL_SEARCHING_NO_IDENT_OR_PILOT_FOUND 3
#define REG_SSD_HWST_STDRES_VALUE_B_G_A2                                 4
#define REG_SSD_HWST_STDRES_VALUE_B_G_NICAM                              5
#define REG_SSD_HWST_STDRES_VALUE_D_K_A2_1                               6
#define REG_SSD_HWST_STDRES_VALUE_D_K_A2_2                               7
#define REG_SSD_HWST_STDRES_VALUE_D_K_A2_3                               8
#define REG_SSD_HWST_STDRES_VALUE_D_K_NICAM                              9
#define REG_SSD_HWST_STDRES_VALUE_L_NICAM                                10
#define REG_SSD_HWST_STDRES_VALUE_I_NICAM                                11
#define REG_SSD_HWST_STDRES_VALUE_M_KOREA                                12
#define REG_SSD_HWST_STDRES_VALUE_M_BTSC                                 13
#define REG_SSD_HWST_STDRES_VALUE_M_EIAJ                                 14
#define REG_SSD_HWST_STDRES_VALUE_FM_RADIO_IF_10_7_MHZ_50_US_DEEMPHASIS  15
#define REG_SSD_HWST_STDRES_VALUE_FM_RADIO_IF_10_7_MHZ_75_US_DEEMPHASIS  16
#define REG_SSD_HWST_STDRES_VALUE_FM_RADIO_SELECTABLE_IF_50_US_DEEMPHASIS 17
#define REG_SSD_HWST_STDRES_VALUE_FM_RADIO_SELECTABLE_IF_75_US_DEEMPHASIS 18
#define REG_SSD_HWST_STDRES_VALUE_STILL_SEARCHING_FOR_A_STANDARD_CAN_OCCUR_ONLY_DURING_A_FEW_MILLISECONDS 31

/*!
 * @brief general stereo flag (ident source determined by currently detected or selected standard)
 *        0 = No stereo mode
 *        1 = Stereo mode detected
 * @range 0..1
 ***/
#define REG_Ssd_hwst_gst
#define REG_Ssd_hwst_gst_OFFSET                                            0x180C0
#define REG_Ssd_hwst_gst_MASK                                            0x1
#define REG_Ssd_hwst_gst_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_hwst_gst_BIT_MASK                                        0x20
#define REG_Ssd_hwst_gst_BIT_IDX                                         5
#define REG_Ssd_hwst_gst_NUM_OF_BITS                                     1
#define REG_Ssd_hwst_gst_MIN                                             0
#define REG_Ssd_hwst_gst_MAX                                             1
#define REG_SSD_HWST_GST_VALUE_NO_STEREO_MODE                            0
#define REG_SSD_HWST_GST_VALUE_STEREO_MODE_DETECTED                      1

/*!
 * @brief general dual flag
 *        0 = No dual mode
 *        1 = Dual mode detected
 * @range 0..1
 ***/
#define REG_Ssd_hwst_gdu
#define REG_Ssd_hwst_gdu_OFFSET                                            0x180C0
#define REG_Ssd_hwst_gdu_MASK                                            0x1
#define REG_Ssd_hwst_gdu_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_hwst_gdu_BIT_MASK                                        0x40
#define REG_Ssd_hwst_gdu_BIT_IDX                                         6
#define REG_Ssd_hwst_gdu_NUM_OF_BITS                                     1
#define REG_Ssd_hwst_gdu_MIN                                             0
#define REG_Ssd_hwst_gdu_MAX                                             1
#define REG_SSD_HWST_GDU_VALUE_NO_DUAL_MODE                              0
#define REG_SSD_HWST_GDU_VALUE_DUAL_MODE_DETECTED                        1

/*!
 * @brief A2 or EIAJ ident dual flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_adu
#define REG_Ssd_hwst_adu_OFFSET                                            0x180C0
#define REG_Ssd_hwst_adu_MASK                                            0x1
#define REG_Ssd_hwst_adu_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_hwst_adu_BIT_MASK                                        0x100
#define REG_Ssd_hwst_adu_BIT_IDX                                         8
#define REG_Ssd_hwst_adu_NUM_OF_BITS                                     1
#define REG_Ssd_hwst_adu_MIN                                             0
#define REG_Ssd_hwst_adu_MAX                                             1
#define REG_SSD_HWST_ADU_VALUE_FALSE                                     0
#define REG_SSD_HWST_ADU_VALUE_TRUE                                      1

/*!
 * @brief A2 or EIAJ ident stereo flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_ast
#define REG_Ssd_hwst_ast_OFFSET                                            0x180C0
#define REG_Ssd_hwst_ast_MASK                                            0x1
#define REG_Ssd_hwst_ast_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_hwst_ast_BIT_MASK                                        0x200
#define REG_Ssd_hwst_ast_BIT_IDX                                         9
#define REG_Ssd_hwst_ast_NUM_OF_BITS                                     1
#define REG_Ssd_hwst_ast_MIN                                             0
#define REG_Ssd_hwst_ast_MAX                                             1
#define REG_SSD_HWST_AST_VALUE_FALSE                                     0
#define REG_SSD_HWST_AST_VALUE_TRUE                                      1

/*!
 * @brief SC2 (if A2 mode) or EIAJ subchannel muted due to noise
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_aamut
#define REG_Ssd_hwst_aamut_OFFSET                                          0x180C0
#define REG_Ssd_hwst_aamut_MASK                                          0x1
#define REG_Ssd_hwst_aamut_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_hwst_aamut_BIT_MASK                                      0x400
#define REG_Ssd_hwst_aamut_BIT_IDX                                       10
#define REG_Ssd_hwst_aamut_NUM_OF_BITS                                   1
#define REG_Ssd_hwst_aamut_MIN                                           0
#define REG_Ssd_hwst_aamut_MAX                                           1
#define REG_SSD_HWST_AAMUT_VALUE_FALSE                                   0
#define REG_SSD_HWST_AAMUT_VALUE_TRUE                                    1

/*!
 * @brief BTSC or FM radio pilot tone detected (stereo indicator)
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_bpilot
#define REG_Ssd_hwst_bpilot_OFFSET                                         0x180C0
#define REG_Ssd_hwst_bpilot_MASK                                         0x1
#define REG_Ssd_hwst_bpilot_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_hwst_bpilot_BIT_MASK                                     0x800
#define REG_Ssd_hwst_bpilot_BIT_IDX                                      11
#define REG_Ssd_hwst_bpilot_NUM_OF_BITS                                  1
#define REG_Ssd_hwst_bpilot_MIN                                          0
#define REG_Ssd_hwst_bpilot_MAX                                          1
#define REG_SSD_HWST_BPILOT_VALUE_FALSE                                  0
#define REG_SSD_HWST_BPILOT_VALUE_TRUE                                   1

/*!
 * @brief SAP carrier detected
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_sapdet
#define REG_Ssd_hwst_sapdet_OFFSET                                         0x180C0
#define REG_Ssd_hwst_sapdet_MASK                                         0x1
#define REG_Ssd_hwst_sapdet_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_hwst_sapdet_BIT_MASK                                     0x1000
#define REG_Ssd_hwst_sapdet_BIT_IDX                                      12
#define REG_Ssd_hwst_sapdet_NUM_OF_BITS                                  1
#define REG_Ssd_hwst_sapdet_MIN                                          0
#define REG_Ssd_hwst_sapdet_MAX                                          1
#define REG_SSD_HWST_SAPDET_VALUE_FALSE                                  0
#define REG_SSD_HWST_SAPDET_VALUE_TRUE                                   1

/*!
 * @brief BTSC stereo muted due to noise (if noise detector enabled)
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_bamut
#define REG_Ssd_hwst_bamut_OFFSET                                          0x180C0
#define REG_Ssd_hwst_bamut_MASK                                          0x1
#define REG_Ssd_hwst_bamut_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_hwst_bamut_BIT_MASK                                      0x2000
#define REG_Ssd_hwst_bamut_BIT_IDX                                       13
#define REG_Ssd_hwst_bamut_NUM_OF_BITS                                   1
#define REG_Ssd_hwst_bamut_MIN                                           0
#define REG_Ssd_hwst_bamut_MAX                                           1
#define REG_SSD_HWST_BAMUT_VALUE_FALSE                                   0
#define REG_SSD_HWST_BAMUT_VALUE_TRUE                                    1

/*!
 * @brief SAP muted due to noise (if noise detector enabled)
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_samut
#define REG_Ssd_hwst_samut_OFFSET                                          0x180C0
#define REG_Ssd_hwst_samut_MASK                                          0x1
#define REG_Ssd_hwst_samut_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_hwst_samut_BIT_MASK                                      0x4000
#define REG_Ssd_hwst_samut_BIT_IDX                                       14
#define REG_Ssd_hwst_samut_NUM_OF_BITS                                   1
#define REG_Ssd_hwst_samut_MIN                                           0
#define REG_Ssd_hwst_samut_MAX                                           1
#define REG_SSD_HWST_SAMUT_VALUE_FALSE                                   0
#define REG_SSD_HWST_SAMUT_VALUE_TRUE                                    1

/*!
 * @brief NICAM decoder VDSP flag
 *        0 = DATA or undefined format
 *        1 = SOUND
 * @range 0..1
 ***/
#define REG_Ssd_hwst_vdsp_c
#define REG_Ssd_hwst_vdsp_c_OFFSET                                         0x180C0
#define REG_Ssd_hwst_vdsp_c_MASK                                         0x1
#define REG_Ssd_hwst_vdsp_c_SIGNED_BIT_MASK                              0x0
#define REG_Ssd_hwst_vdsp_c_BIT_MASK                                     0x8000
#define REG_Ssd_hwst_vdsp_c_BIT_IDX                                      15
#define REG_Ssd_hwst_vdsp_c_NUM_OF_BITS                                  1
#define REG_Ssd_hwst_vdsp_c_MIN                                          0
#define REG_Ssd_hwst_vdsp_c_MAX                                          1
#define REG_SSD_HWST_VDSP_C_VALUE_DATA_OR_UNDEFINED_FORMAT               0
#define REG_SSD_HWST_VDSP_C_VALUE_SOUND                                  1

/*!
 * @brief NICAM decoder stereo flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_nicst_c
#define REG_Ssd_hwst_nicst_c_OFFSET                                        0x180C0
#define REG_Ssd_hwst_nicst_c_MASK                                        0x1
#define REG_Ssd_hwst_nicst_c_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_hwst_nicst_c_BIT_MASK                                    0x10000
#define REG_Ssd_hwst_nicst_c_BIT_IDX                                     16
#define REG_Ssd_hwst_nicst_c_NUM_OF_BITS                                 1
#define REG_Ssd_hwst_nicst_c_MIN                                         0
#define REG_Ssd_hwst_nicst_c_MAX                                         1
#define REG_SSD_HWST_NICST_C_VALUE_FALSE                                 0
#define REG_SSD_HWST_NICST_C_VALUE_TRUE                                  1

/*!
 * @brief NICAM decoder dual flag
 *        0 = False
 *        1 = True
 * @range 0..1
 ***/
#define REG_Ssd_hwst_nicdu_c
#define REG_Ssd_hwst_nicdu_c_OFFSET                                        0x180C0
#define REG_Ssd_hwst_nicdu_c_MASK                                        0x1
#define REG_Ssd_hwst_nicdu_c_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_hwst_nicdu_c_BIT_MASK                                    0x20000
#define REG_Ssd_hwst_nicdu_c_BIT_IDX                                     17
#define REG_Ssd_hwst_nicdu_c_NUM_OF_BITS                                 1
#define REG_Ssd_hwst_nicdu_c_MIN                                         0
#define REG_Ssd_hwst_nicdu_c_MAX                                         1
#define REG_SSD_HWST_NICDU_C_VALUE_FALSE                                 0
#define REG_SSD_HWST_NICDU_C_VALUE_TRUE                                  1

/*!
 * @brief NICAM automute flag
 *        0 = not muted
 *        1 = muted (fallback to analog sound carrier)
 * @range 0..1
 ***/
#define REG_Ssd_hwst_namut
#define REG_Ssd_hwst_namut_OFFSET                                          0x180C0
#define REG_Ssd_hwst_namut_MASK                                          0x1
#define REG_Ssd_hwst_namut_SIGNED_BIT_MASK                               0x0
#define REG_Ssd_hwst_namut_BIT_MASK                                      0x40000
#define REG_Ssd_hwst_namut_BIT_IDX                                       18
#define REG_Ssd_hwst_namut_NUM_OF_BITS                                   1
#define REG_Ssd_hwst_namut_MIN                                           0
#define REG_Ssd_hwst_namut_MAX                                           1
#define REG_SSD_HWST_NAMUT_VALUE_NOT_MUTED                               0
#define REG_SSD_HWST_NAMUT_VALUE_MUTED_FALLBACK_TO_ANALOG_SOUND_CARRIER  1

/*!
 * @brief NICAM reserve sound switching flag (=C4), see NICAM specification
 *        0 = ana-sc different contents than NICAM carrier
 *        1 = ana-sc same contents as NICAM carrier (M1 if DUAL)
 * @range 0..1
 ***/
#define REG_Ssd_hwst_rssf
#define REG_Ssd_hwst_rssf_OFFSET                                           0x180C0
#define REG_Ssd_hwst_rssf_MASK                                           0x1
#define REG_Ssd_hwst_rssf_SIGNED_BIT_MASK                                0x0
#define REG_Ssd_hwst_rssf_BIT_MASK                                       0x80000
#define REG_Ssd_hwst_rssf_BIT_IDX                                        19
#define REG_Ssd_hwst_rssf_NUM_OF_BITS                                    1
#define REG_Ssd_hwst_rssf_MIN                                            0
#define REG_Ssd_hwst_rssf_MAX                                            1
#define REG_SSD_HWST_RSSF_VALUE_ANA_SC_DIFFERENT_CONTENTS_THAN_NICAM_CARRIER 0
#define REG_SSD_HWST_RSSF_VALUE_ANA_SC_SAME_CONTENTS_AS_NICAM_CARRIER_M1_IF_DUAL 1

/*!
 * @brief initialization status (set to 0 upon read access)
 *        0 = no reset performed
 *        1 = reset has been applied to DSP and init routine has been executed
 * @range 0..1
 ***/
#define REG_Ssd_hwst_initstat
#define REG_Ssd_hwst_initstat_OFFSET                                       0x180C0
#define REG_Ssd_hwst_initstat_MASK                                       0x1
#define REG_Ssd_hwst_initstat_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_hwst_initstat_BIT_MASK                                   0x100000
#define REG_Ssd_hwst_initstat_BIT_IDX                                    20
#define REG_Ssd_hwst_initstat_NUM_OF_BITS                                1
#define REG_Ssd_hwst_initstat_MIN                                        0
#define REG_Ssd_hwst_initstat_MAX                                        1
#define REG_SSD_HWST_INITSTAT_VALUE_NO_RESET_PERFORMED                   0
#define REG_SSD_HWST_INITSTAT_VALUE_RESET_HAS_BEEN_APPLIED_TO_DSP_AND_INIT_ROUTINE_HAS_BEEN_EXECUTED 1

/*!
 * @brief reserved
 * @range 0..7
 ***/
#define REG_Ssd_rsd_hw_copy_snd_status0_68
#define REG_Ssd_rsd_hw_copy_snd_status0_68_OFFSET                          0x180C0
#define REG_Ssd_rsd_hw_copy_snd_status0_68_MASK                          0x7
#define REG_Ssd_rsd_hw_copy_snd_status0_68_SIGNED_BIT_MASK               0x0
#define REG_Ssd_rsd_hw_copy_snd_status0_68_BIT_MASK                      0xe00000
#define REG_Ssd_rsd_hw_copy_snd_status0_68_BIT_IDX                       21
#define REG_Ssd_rsd_hw_copy_snd_status0_68_NUM_OF_BITS                   3
#define REG_Ssd_rsd_hw_copy_snd_status0_68_MIN                           0
#define REG_Ssd_rsd_hw_copy_snd_status0_68_MAX                           7

/*!
 * @brief unused
 * @range 0..255
 ***/
#define REG_Ssd_rsd_hw_copy_snd_status0_69
#define REG_Ssd_rsd_hw_copy_snd_status0_69_OFFSET                          0x180C0
#define REG_Ssd_rsd_hw_copy_snd_status0_69_MASK                          0xff
#define REG_Ssd_rsd_hw_copy_snd_status0_69_SIGNED_BIT_MASK               0x0
#define REG_Ssd_rsd_hw_copy_snd_status0_69_BIT_MASK                      0xff000000
#define REG_Ssd_rsd_hw_copy_snd_status0_69_BIT_IDX                       24
#define REG_Ssd_rsd_hw_copy_snd_status0_69_NUM_OF_BITS                   8
#define REG_Ssd_rsd_hw_copy_snd_status0_69_MIN                           0
#define REG_Ssd_rsd_hw_copy_snd_status0_69_MAX                           255

/*!
 * @brief S/PDIF input signal use (channel status bit 0)
 *        0=consumer mode (IEC 958-3)
 *        1=professional mode (IEC 958-4)
 * @range 0..1
 ***/
#define REG_Ssd_sics_consumermode
#define REG_Ssd_sics_consumermode_OFFSET                                   0x180C4
#define REG_Ssd_sics_consumermode_MASK                                   0x1
#define REG_Ssd_sics_consumermode_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_sics_consumermode_BIT_MASK                               0x1
#define REG_Ssd_sics_consumermode_BIT_IDX                                0
#define REG_Ssd_sics_consumermode_NUM_OF_BITS                            1
#define REG_Ssd_sics_consumermode_MIN                                    0
#define REG_Ssd_sics_consumermode_MAX                                    1
#define REG_SSD_SICS_CONSUMERMODE_VALUE_CONSUMER_MODE_IEC_958MINUS_3     0
#define REG_SSD_SICS_CONSUMERMODE_VALUE_PROFESSIONAL_MODE_IEC_958MINUS_4 1

/*!
 * @brief signal type of SPDIF input (bit 1)
 *        0=linear PCM audio
 *        1=other (e.g. compressed audio data)
 * @range 0..1
 ***/
#define REG_Ssd_sics_pcm
#define REG_Ssd_sics_pcm_OFFSET                                            0x180C4
#define REG_Ssd_sics_pcm_MASK                                            0x1
#define REG_Ssd_sics_pcm_SIGNED_BIT_MASK                                 0x0
#define REG_Ssd_sics_pcm_BIT_MASK                                        0x2
#define REG_Ssd_sics_pcm_BIT_IDX                                         1
#define REG_Ssd_sics_pcm_NUM_OF_BITS                                     1
#define REG_Ssd_sics_pcm_MIN                                             0
#define REG_Ssd_sics_pcm_MAX                                             1
#define REG_SSD_SICS_PCM_VALUE_LINEAR_PCM_AUDIO                          0
#define REG_SSD_SICS_PCM_VALUE_OTHER_E_G_COMPRESSED_AUDIO_DATA           1

/*!
 * @brief copyright info (IEC consumer meaning) (bit 2)
 *        0=copyright asserted
 *        1=no copyright asserted
 * @range 0..1
 ***/
#define REG_Ssd_sics_copyright
#define REG_Ssd_sics_copyright_OFFSET                                      0x180C4
#define REG_Ssd_sics_copyright_MASK                                      0x1
#define REG_Ssd_sics_copyright_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_sics_copyright_BIT_MASK                                  0x4
#define REG_Ssd_sics_copyright_BIT_IDX                                   2
#define REG_Ssd_sics_copyright_NUM_OF_BITS                               1
#define REG_Ssd_sics_copyright_MIN                                       0
#define REG_Ssd_sics_copyright_MAX                                       1
#define REG_SSD_SICS_COPYRIGHT_VALUE_COPYRIGHT_ASSERTED                  0
#define REG_SSD_SICS_COPYRIGHT_VALUE_NO_COPYRIGHT_ASSERTED               1

/*!
 * @brief additional format information (IEC consumer meaning) (bits 5-3)
 *        0=(if lin.PCM) 2 audio channels without pre-emphasis
 *        1=(if lin.PCM) 2 audio channels with 50/15us pre-emphasis
 *        4=(if lin.PCM) 4 audio channels without pre-emphasis
 *        5=(if lin.PCM) 4 audio channels with 50/15us pre-emphasis
 * @range 0..7
 ***/
#define REG_Ssd_sics_addformat
#define REG_Ssd_sics_addformat_OFFSET                                      0x180C4
#define REG_Ssd_sics_addformat_MASK                                      0x7
#define REG_Ssd_sics_addformat_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_sics_addformat_BIT_MASK                                  0x38
#define REG_Ssd_sics_addformat_BIT_IDX                                   3
#define REG_Ssd_sics_addformat_NUM_OF_BITS                               3
#define REG_Ssd_sics_addformat_MIN                                       0
#define REG_Ssd_sics_addformat_MAX                                       7
#define REG_SSD_SICS_ADDFORMAT_VALUE_IF_LIN_PCM_2_AUDIO_CHANNELS_WITHOUT_PRE_EMPHASIS 0
#define REG_SSD_SICS_ADDFORMAT_VALUE_IF_LIN_PCM_2_AUDIO_CHANNELS_WITH_50_15US_PRE_EMPHASIS 1
#define REG_SSD_SICS_ADDFORMAT_VALUE_IF_LIN_PCM_4_AUDIO_CHANNELS_WITHOUT_PRE_EMPHASIS 4
#define REG_SSD_SICS_ADDFORMAT_VALUE_IF_LIN_PCM_4_AUDIO_CHANNELS_WITH_50_15US_PRE_EMPHASIS 5

/*!
 * @brief channel status mode (IEC consumer meaning) (bits 7-6)
 * @range 0..3
 ***/
#define REG_Ssd_sics_chstmode
#define REG_Ssd_sics_chstmode_OFFSET                                       0x180C4
#define REG_Ssd_sics_chstmode_MASK                                       0x3
#define REG_Ssd_sics_chstmode_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sics_chstmode_BIT_MASK                                   0xc0
#define REG_Ssd_sics_chstmode_BIT_IDX                                    6
#define REG_Ssd_sics_chstmode_NUM_OF_BITS                                2
#define REG_Ssd_sics_chstmode_MIN                                        0
#define REG_Ssd_sics_chstmode_MAX                                        3

/*!
 * @brief category code (IEC consumer meaning) (bits 15-8)
 * @range 0..255
 ***/
#define REG_Ssd_sics_category
#define REG_Ssd_sics_category_OFFSET                                       0x180C4
#define REG_Ssd_sics_category_MASK                                       0xff
#define REG_Ssd_sics_category_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sics_category_BIT_MASK                                   0xff00
#define REG_Ssd_sics_category_BIT_IDX                                    8
#define REG_Ssd_sics_category_NUM_OF_BITS                                8
#define REG_Ssd_sics_category_MIN                                        0
#define REG_Ssd_sics_category_MAX                                        255

/*!
 * @brief source number (IEC consumer meaning) (bits 19-16)
 * @range 0..15
 ***/
#define REG_Ssd_sics_sourceno
#define REG_Ssd_sics_sourceno_OFFSET                                       0x180C4
#define REG_Ssd_sics_sourceno_MASK                                       0xf
#define REG_Ssd_sics_sourceno_SIGNED_BIT_MASK                            0x0
#define REG_Ssd_sics_sourceno_BIT_MASK                                   0xf0000
#define REG_Ssd_sics_sourceno_BIT_IDX                                    16
#define REG_Ssd_sics_sourceno_NUM_OF_BITS                                4
#define REG_Ssd_sics_sourceno_MIN                                        0
#define REG_Ssd_sics_sourceno_MAX                                        15

/*!
 * @brief channel number (IEC consumer meaning) (bits 23-20)
 * @range 0..15
 ***/
#define REG_Ssd_sics_channelno
#define REG_Ssd_sics_channelno_OFFSET                                      0x180C4
#define REG_Ssd_sics_channelno_MASK                                      0xf
#define REG_Ssd_sics_channelno_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_sics_channelno_BIT_MASK                                  0xf00000
#define REG_Ssd_sics_channelno_BIT_IDX                                   20
#define REG_Ssd_sics_channelno_NUM_OF_BITS                               4
#define REG_Ssd_sics_channelno_MIN                                       0
#define REG_Ssd_sics_channelno_MAX                                       15

/*!
 * @brief reserved
 * @range 0..255
 ***/
#if 0
#define REG_Ssd_rsd_hw_spdifin_status_69
#define REG_Ssd_rsd_hw_spdifin_status_69_OFFSET                            0x180C4
#define REG_Ssd_rsd_hw_spdifin_status_69_MASK                            0xff
#define REG_Ssd_rsd_hw_spdifin_status_69_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_rsd_hw_spdifin_status_69_BIT_MASK                        0xff000000
#define REG_Ssd_rsd_hw_spdifin_status_69_BIT_IDX                         24
#define REG_Ssd_rsd_hw_spdifin_status_69_NUM_OF_BITS                     8
#define REG_Ssd_rsd_hw_spdifin_status_69_MIN                             0
#define REG_Ssd_rsd_hw_spdifin_status_69_MAX                             255
#endif

/*!
 * @brief sampling frequency (IEC consumer meaning) (bits 25-24)
 *        2=48 kHz
 *        0=44.1 kHz
 *        3=32 kHz
 *        6=24 kHz
 *        4=22.05 kHz
 *        10=96 kHz
 *        14=192 kHz
 * @range 0..15
 ***/
#define REG_Ssd_sics_fs
#define REG_Ssd_sics_fs_OFFSET                                             0x180C8
#define REG_Ssd_sics_fs_MASK                                             0xf
#define REG_Ssd_sics_fs_SIGNED_BIT_MASK                                  0x0
#define REG_Ssd_sics_fs_BIT_MASK                                         0xf
#define REG_Ssd_sics_fs_BIT_IDX                                          0
#define REG_Ssd_sics_fs_NUM_OF_BITS                                      4
#define REG_Ssd_sics_fs_MIN                                              0
#define REG_Ssd_sics_fs_MAX                                              15
#define REG_SSD_SICS_FS_VALUE_44_1_KHZ                                   0
#define REG_SSD_SICS_FS_VALUE_48_KHZ                                     2
#define REG_SSD_SICS_FS_VALUE_32_KHZ                                     3
#define REG_SSD_SICS_FS_VALUE_22_05_KHZ                                  4
#define REG_SSD_SICS_FS_VALUE_24_KHZ                                     6
#define REG_SSD_SICS_FS_VALUE_96_KHZ                                     10
#define REG_SSD_SICS_FS_VALUE_192_KHZ                                    14

/*!
 * @brief clock accuracy (IEC consumer meaning) (bits 29-28)
 *        1 = level I (high accuracy, +/- 50ppm)
 *        0 = level II (normal accuracy, +/- 0.1%)
 *        2 = level III (variable pitch, +/- 12.5%)
 *        3 = reserved
 * @range 0..3
 ***/
#define REG_Ssd_sics_clkaccuracy
#define REG_Ssd_sics_clkaccuracy_OFFSET                                    0x180C8
#define REG_Ssd_sics_clkaccuracy_MASK                                    0x3
#define REG_Ssd_sics_clkaccuracy_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_sics_clkaccuracy_BIT_MASK                                0x30
#define REG_Ssd_sics_clkaccuracy_BIT_IDX                                 4
#define REG_Ssd_sics_clkaccuracy_NUM_OF_BITS                             2
#define REG_Ssd_sics_clkaccuracy_MIN                                     0
#define REG_Ssd_sics_clkaccuracy_MAX                                     3
#define REG_SSD_SICS_CLKACCURACY_VALUE_LEVEL_II_NORMAL_ACCURACY_0_1      0
#define REG_SSD_SICS_CLKACCURACY_VALUE_LEVEL_I_HIGH_ACCURACY_50PPM       1
#define REG_SSD_SICS_CLKACCURACY_VALUE_LEVEL_III_VARIABLE_PITCH_12_5     2
#define REG_SSD_SICS_CLKACCURACY_VALUE_RESERVED                          3

/*!
 * @brief reserved
 * @range 0..3
 ***/
#if 0
#define REG_Ssd_rsd_hw_spdifin_status_69
#define REG_Ssd_rsd_hw_spdifin_status_69_OFFSET                            0x180C8
#define REG_Ssd_rsd_hw_spdifin_status_69_MASK                            0x3
#define REG_Ssd_rsd_hw_spdifin_status_69_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_rsd_hw_spdifin_status_69_BIT_MASK                        0xc0
#define REG_Ssd_rsd_hw_spdifin_status_69_BIT_IDX                         6
#define REG_Ssd_rsd_hw_spdifin_status_69_NUM_OF_BITS                     2
#define REG_Ssd_rsd_hw_spdifin_status_69_MIN                             0
#define REG_Ssd_rsd_hw_spdifin_status_69_MAX                             3
#endif

/*!
 * @brief SPDIF input max. word length (bit 32)
 *        0=maximum audio sample length is 20 bits
 *        1=maximum audio sample length is 24 bits
 * @range 0..1
 ***/
#define REG_Ssd_sics_maxwlength
#define REG_Ssd_sics_maxwlength_OFFSET                                     0x180C8
#define REG_Ssd_sics_maxwlength_MASK                                     0x1
#define REG_Ssd_sics_maxwlength_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_sics_maxwlength_BIT_MASK                                 0x100
#define REG_Ssd_sics_maxwlength_BIT_IDX                                  8
#define REG_Ssd_sics_maxwlength_NUM_OF_BITS                              1
#define REG_Ssd_sics_maxwlength_MIN                                      0
#define REG_Ssd_sics_maxwlength_MAX                                      1
#define REG_SSD_SICS_MAXWLENGTH_VALUE_MAXIMUM_AUDIO_SAMPLE_LENGTH_IS_20_BITS 0
#define REG_SSD_SICS_MAXWLENGTH_VALUE_MAXIMUM_AUDIO_SAMPLE_LENGTH_IS_24_BITS 1

/*!
 * @brief SPDIF input word length (bits 35-33)
 *        0 = word length not indicated (default)
 *        1 = 20bit / 16bit
 *        2 = 22bit / 18bit
 *        4 = 23bit / 19bit
 *        5 = 24bit / 20bit
 *        6 = 21bit / 17bit
 * @range 0..7
 ***/
#define REG_Ssd_sics_wlength
#define REG_Ssd_sics_wlength_OFFSET                                        0x180C8
#define REG_Ssd_sics_wlength_MASK                                        0x7
#define REG_Ssd_sics_wlength_SIGNED_BIT_MASK                             0x0
#define REG_Ssd_sics_wlength_BIT_MASK                                    0xe00
#define REG_Ssd_sics_wlength_BIT_IDX                                     9
#define REG_Ssd_sics_wlength_NUM_OF_BITS                                 3
#define REG_Ssd_sics_wlength_MIN                                         0
#define REG_Ssd_sics_wlength_MAX                                         7
#define REG_SSD_SICS_WLENGTH_VALUE_WORD_LENGTH_NOT_INDICATED_DEFAULT     0
#define REG_SSD_SICS_WLENGTH_VALUE_20BIT_16BIT                           1
#define REG_SSD_SICS_WLENGTH_VALUE_22BIT_18BIT                           2
#define REG_SSD_SICS_WLENGTH_VALUE_23BIT_19BIT                           4
#define REG_SSD_SICS_WLENGTH_VALUE_24BIT_20BIT                           5
#define REG_SSD_SICS_WLENGTH_VALUE_21BIT_17BIT                           6

/*!
 * @brief SPDIF input validity, left
 * @range 0..1
 ***/
#define REG_Ssd_sivl
#define REG_Ssd_sivl_OFFSET                                                0x180C8
#define REG_Ssd_sivl_MASK                                                0x1
#define REG_Ssd_sivl_SIGNED_BIT_MASK                                     0x0
#define REG_Ssd_sivl_BIT_MASK                                            0x1000
#define REG_Ssd_sivl_BIT_IDX                                             12
#define REG_Ssd_sivl_NUM_OF_BITS                                         1
#define REG_Ssd_sivl_MIN                                                 0
#define REG_Ssd_sivl_MAX                                                 1

/*!
 * @brief SPDIF input validity, right
 * @range 0..1
 ***/
#define REG_Ssd_sivr
#define REG_Ssd_sivr_OFFSET                                                0x180C8
#define REG_Ssd_sivr_MASK                                                0x1
#define REG_Ssd_sivr_SIGNED_BIT_MASK                                     0x0
#define REG_Ssd_sivr_BIT_MASK                                            0x2000
#define REG_Ssd_sivr_BIT_IDX                                             13
#define REG_Ssd_sivr_NUM_OF_BITS                                         1
#define REG_Ssd_sivr_MIN                                                 0
#define REG_Ssd_sivr_MAX                                                 1

/*!
 * @brief reserved
 * @range 0..262143
 ***/
#if 0
#define REG_Ssd_rsd_hw_spdifin_status_69
#define REG_Ssd_rsd_hw_spdifin_status_69_OFFSET                            0x180C8
#define REG_Ssd_rsd_hw_spdifin_status_69_MASK                            0x3ffff
#define REG_Ssd_rsd_hw_spdifin_status_69_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_rsd_hw_spdifin_status_69_BIT_MASK                        0xffffc000
#define REG_Ssd_rsd_hw_spdifin_status_69_BIT_IDX                         14
#define REG_Ssd_rsd_hw_spdifin_status_69_NUM_OF_BITS                     18
#define REG_Ssd_rsd_hw_spdifin_status_69_MIN                             0
#define REG_Ssd_rsd_hw_spdifin_status_69_MAX                             262143
#endif

/*!
 * @brief indicates a pending interrupt caused by a change of STDRES in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_stdres
#define REG_Ssd_int_status_stdres_OFFSET                                   0x3FFE0
#define REG_Ssd_int_status_stdres_MASK                                   0x1
#define REG_Ssd_int_status_stdres_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_int_status_stdres_BIT_MASK                               0x1
#define REG_Ssd_int_status_stdres_BIT_IDX                                0
#define REG_Ssd_int_status_stdres_NUM_OF_BITS                            1
#define REG_Ssd_int_status_stdres_MIN                                    0
#define REG_Ssd_int_status_stdres_MAX                                    1
#define REG_SSD_INT_STATUS_STDRES_VALUE_NOT_PENDING                      0
#define REG_SSD_INT_STATUS_STDRES_VALUE_PENDING                          1

/*!
 * @brief reserved
 * @range 0..15
 ***/
#define REG_Ssd_rsd_snd_int_status_72
#define REG_Ssd_rsd_snd_int_status_72_OFFSET                               0x3FFE0
#define REG_Ssd_rsd_snd_int_status_72_MASK                               0xf
#define REG_Ssd_rsd_snd_int_status_72_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_status_72_BIT_MASK                           0x1e
#define REG_Ssd_rsd_snd_int_status_72_BIT_IDX                            1
#define REG_Ssd_rsd_snd_int_status_72_NUM_OF_BITS                        4
#define REG_Ssd_rsd_snd_int_status_72_MIN                                0
#define REG_Ssd_rsd_snd_int_status_72_MAX                                15

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_GST in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_gst
#define REG_Ssd_int_status_gst_OFFSET                                      0x3FFE0
#define REG_Ssd_int_status_gst_MASK                                      0x1
#define REG_Ssd_int_status_gst_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_int_status_gst_BIT_MASK                                  0x20
#define REG_Ssd_int_status_gst_BIT_IDX                                   5
#define REG_Ssd_int_status_gst_NUM_OF_BITS                               1
#define REG_Ssd_int_status_gst_MIN                                       0
#define REG_Ssd_int_status_gst_MAX                                       1
#define REG_SSD_INT_STATUS_GST_VALUE_NOT_PENDING                         0
#define REG_SSD_INT_STATUS_GST_VALUE_PENDING                             1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_GDU in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_gdu
#define REG_Ssd_int_status_gdu_OFFSET                                      0x3FFE0
#define REG_Ssd_int_status_gdu_MASK                                      0x1
#define REG_Ssd_int_status_gdu_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_int_status_gdu_BIT_MASK                                  0x40
#define REG_Ssd_int_status_gdu_BIT_IDX                                   6
#define REG_Ssd_int_status_gdu_NUM_OF_BITS                               1
#define REG_Ssd_int_status_gdu_MIN                                       0
#define REG_Ssd_int_status_gdu_MAX                                       1
#define REG_SSD_INT_STATUS_GDU_VALUE_NOT_PENDING                         0
#define REG_SSD_INT_STATUS_GDU_VALUE_PENDING                             1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_ADU in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_adu
#define REG_Ssd_int_status_adu_OFFSET                                      0x3FFE0
#define REG_Ssd_int_status_adu_MASK                                      0x1
#define REG_Ssd_int_status_adu_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_int_status_adu_BIT_MASK                                  0x100
#define REG_Ssd_int_status_adu_BIT_IDX                                   8
#define REG_Ssd_int_status_adu_NUM_OF_BITS                               1
#define REG_Ssd_int_status_adu_MIN                                       0
#define REG_Ssd_int_status_adu_MAX                                       1
#define REG_SSD_INT_STATUS_ADU_VALUE_NOT_PENDING                         0
#define REG_SSD_INT_STATUS_ADU_VALUE_PENDING                             1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_AST in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_ast
#define REG_Ssd_int_status_ast_OFFSET                                      0x3FFE0
#define REG_Ssd_int_status_ast_MASK                                      0x1
#define REG_Ssd_int_status_ast_SIGNED_BIT_MASK                           0x0
#define REG_Ssd_int_status_ast_BIT_MASK                                  0x200
#define REG_Ssd_int_status_ast_BIT_IDX                                   9
#define REG_Ssd_int_status_ast_NUM_OF_BITS                               1
#define REG_Ssd_int_status_ast_MIN                                       0
#define REG_Ssd_int_status_ast_MAX                                       1
#define REG_SSD_INT_STATUS_AST_VALUE_NOT_PENDING                         0
#define REG_SSD_INT_STATUS_AST_VALUE_PENDING                             1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_AAMUT in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_aamut
#define REG_Ssd_int_status_aamut_OFFSET                                    0x3FFE0
#define REG_Ssd_int_status_aamut_MASK                                    0x1
#define REG_Ssd_int_status_aamut_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_int_status_aamut_BIT_MASK                                0x400
#define REG_Ssd_int_status_aamut_BIT_IDX                                 10
#define REG_Ssd_int_status_aamut_NUM_OF_BITS                             1
#define REG_Ssd_int_status_aamut_MIN                                     0
#define REG_Ssd_int_status_aamut_MAX                                     1
#define REG_SSD_INT_STATUS_AAMUT_VALUE_NOT_PENDING                       0
#define REG_SSD_INT_STATUS_AAMUT_VALUE_PENDING                           1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_BPILOT in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_bpilot
#define REG_Ssd_int_status_bpilot_OFFSET                                   0x3FFE0
#define REG_Ssd_int_status_bpilot_MASK                                   0x1
#define REG_Ssd_int_status_bpilot_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_int_status_bpilot_BIT_MASK                               0x800
#define REG_Ssd_int_status_bpilot_BIT_IDX                                11
#define REG_Ssd_int_status_bpilot_NUM_OF_BITS                            1
#define REG_Ssd_int_status_bpilot_MIN                                    0
#define REG_Ssd_int_status_bpilot_MAX                                    1
#define REG_SSD_INT_STATUS_BPILOT_VALUE_NOT_PENDING                      0
#define REG_SSD_INT_STATUS_BPILOT_VALUE_PENDING                          1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_SAPDET in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_sapdet
#define REG_Ssd_int_status_sapdet_OFFSET                                   0x3FFE0
#define REG_Ssd_int_status_sapdet_MASK                                   0x1
#define REG_Ssd_int_status_sapdet_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_int_status_sapdet_BIT_MASK                               0x1000
#define REG_Ssd_int_status_sapdet_BIT_IDX                                12
#define REG_Ssd_int_status_sapdet_NUM_OF_BITS                            1
#define REG_Ssd_int_status_sapdet_MIN                                    0
#define REG_Ssd_int_status_sapdet_MAX                                    1
#define REG_SSD_INT_STATUS_SAPDET_VALUE_NOT_PENDING                      0
#define REG_SSD_INT_STATUS_SAPDET_VALUE_PENDING                          1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_BAMUT in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_bamut
#define REG_Ssd_int_status_bamut_OFFSET                                    0x3FFE0
#define REG_Ssd_int_status_bamut_MASK                                    0x1
#define REG_Ssd_int_status_bamut_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_int_status_bamut_BIT_MASK                                0x2000
#define REG_Ssd_int_status_bamut_BIT_IDX                                 13
#define REG_Ssd_int_status_bamut_NUM_OF_BITS                             1
#define REG_Ssd_int_status_bamut_MIN                                     0
#define REG_Ssd_int_status_bamut_MAX                                     1
#define REG_SSD_INT_STATUS_BAMUT_VALUE_NOT_PENDING                       0
#define REG_SSD_INT_STATUS_BAMUT_VALUE_PENDING                           1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_SAPMUT in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_sapmut
#define REG_Ssd_int_status_sapmut_OFFSET                                   0x3FFE0
#define REG_Ssd_int_status_sapmut_MASK                                   0x1
#define REG_Ssd_int_status_sapmut_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_int_status_sapmut_BIT_MASK                               0x4000
#define REG_Ssd_int_status_sapmut_BIT_IDX                                14
#define REG_Ssd_int_status_sapmut_NUM_OF_BITS                            1
#define REG_Ssd_int_status_sapmut_MIN                                    0
#define REG_Ssd_int_status_sapmut_MAX                                    1
#define REG_SSD_INT_STATUS_SAPMUT_VALUE_NOT_PENDING                      0
#define REG_SSD_INT_STATUS_SAPMUT_VALUE_PENDING                          1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_VDSP_C in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_vdsp_c
#define REG_Ssd_int_status_vdsp_c_OFFSET                                   0x3FFE0
#define REG_Ssd_int_status_vdsp_c_MASK                                   0x1
#define REG_Ssd_int_status_vdsp_c_SIGNED_BIT_MASK                        0x0
#define REG_Ssd_int_status_vdsp_c_BIT_MASK                               0x8000
#define REG_Ssd_int_status_vdsp_c_BIT_IDX                                15
#define REG_Ssd_int_status_vdsp_c_NUM_OF_BITS                            1
#define REG_Ssd_int_status_vdsp_c_MIN                                    0
#define REG_Ssd_int_status_vdsp_c_MAX                                    1
#define REG_SSD_INT_STATUS_VDSP_C_VALUE_NOT_PENDING                      0
#define REG_SSD_INT_STATUS_VDSP_C_VALUE_PENDING                          1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_NICST_C in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_nicst_c
#define REG_Ssd_int_status_nicst_c_OFFSET                                  0x3FFE0
#define REG_Ssd_int_status_nicst_c_MASK                                  0x1
#define REG_Ssd_int_status_nicst_c_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_int_status_nicst_c_BIT_MASK                              0x10000
#define REG_Ssd_int_status_nicst_c_BIT_IDX                               16
#define REG_Ssd_int_status_nicst_c_NUM_OF_BITS                           1
#define REG_Ssd_int_status_nicst_c_MIN                                   0
#define REG_Ssd_int_status_nicst_c_MAX                                   1
#define REG_SSD_INT_STATUS_NICST_C_VALUE_NOT_PENDING                     0
#define REG_SSD_INT_STATUS_NICST_C_VALUE_PENDING                         1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_NICDU_C in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_nicdu_c
#define REG_Ssd_int_status_nicdu_c_OFFSET                                  0x3FFE0
#define REG_Ssd_int_status_nicdu_c_MASK                                  0x1
#define REG_Ssd_int_status_nicdu_c_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_int_status_nicdu_c_BIT_MASK                              0x20000
#define REG_Ssd_int_status_nicdu_c_BIT_IDX                               17
#define REG_Ssd_int_status_nicdu_c_NUM_OF_BITS                           1
#define REG_Ssd_int_status_nicdu_c_MIN                                   0
#define REG_Ssd_int_status_nicdu_c_MAX                                   1
#define REG_SSD_INT_STATUS_NICDU_C_VALUE_NOT_PENDING                     0
#define REG_SSD_INT_STATUS_NICDU_C_VALUE_PENDING                         1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_NAMUT in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_namut
#define REG_Ssd_int_status_namut_OFFSET                                    0x3FFE0
#define REG_Ssd_int_status_namut_MASK                                    0x1
#define REG_Ssd_int_status_namut_SIGNED_BIT_MASK                         0x0
#define REG_Ssd_int_status_namut_BIT_MASK                                0x40000
#define REG_Ssd_int_status_namut_BIT_IDX                                 18
#define REG_Ssd_int_status_namut_NUM_OF_BITS                             1
#define REG_Ssd_int_status_namut_MIN                                     0
#define REG_Ssd_int_status_namut_MAX                                     1
#define REG_SSD_INT_STATUS_NAMUT_VALUE_NOT_PENDING                       0
#define REG_SSD_INT_STATUS_NAMUT_VALUE_PENDING                           1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_RSSF in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_rssf
#define REG_Ssd_int_status_rssf_OFFSET                                     0x3FFE0
#define REG_Ssd_int_status_rssf_MASK                                     0x1
#define REG_Ssd_int_status_rssf_SIGNED_BIT_MASK                          0x0
#define REG_Ssd_int_status_rssf_BIT_MASK                                 0x80000
#define REG_Ssd_int_status_rssf_BIT_IDX                                  19
#define REG_Ssd_int_status_rssf_NUM_OF_BITS                              1
#define REG_Ssd_int_status_rssf_MIN                                      0
#define REG_Ssd_int_status_rssf_MAX                                      1
#define REG_SSD_INT_STATUS_RSSF_VALUE_NOT_PENDING                        0
#define REG_SSD_INT_STATUS_RSSF_VALUE_PENDING                            1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_INITSTAT in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_initstat
#define REG_Ssd_int_status_initstat_OFFSET                                 0x3FFE0
#define REG_Ssd_int_status_initstat_MASK                                 0x1
#define REG_Ssd_int_status_initstat_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_int_status_initstat_BIT_MASK                             0x100000
#define REG_Ssd_int_status_initstat_BIT_IDX                              20
#define REG_Ssd_int_status_initstat_NUM_OF_BITS                          1
#define REG_Ssd_int_status_initstat_MIN                                  0
#define REG_Ssd_int_status_initstat_MAX                                  1
#define REG_SSD_INT_STATUS_INITSTAT_VALUE_NOT_PENDING                    0
#define REG_SSD_INT_STATUS_INITSTAT_VALUE_PENDING                        1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_SRC_UNLOCK in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_src_unlock
#define REG_Ssd_int_status_src_unlock_OFFSET                               0x3FFE0
#define REG_Ssd_int_status_src_unlock_MASK                               0x1
#define REG_Ssd_int_status_src_unlock_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_status_src_unlock_BIT_MASK                           0x200000
#define REG_Ssd_int_status_src_unlock_BIT_IDX                            21
#define REG_Ssd_int_status_src_unlock_NUM_OF_BITS                        1
#define REG_Ssd_int_status_src_unlock_MIN                                0
#define REG_Ssd_int_status_src_unlock_MAX                                1
#define REG_SSD_INT_STATUS_SRC_UNLOCK_VALUE_NOT_PENDING                  0
#define REG_SSD_INT_STATUS_SRC_UNLOCK_VALUE_PENDING                      1

/*!
 * @brief indicates a pending interrupt caused by a change of INT_STATUS_SRD_STATUS in the DEMDEC main status register
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_srd_status
#define REG_Ssd_int_status_srd_status_OFFSET                               0x3FFE0
#define REG_Ssd_int_status_srd_status_MASK                               0x1
#define REG_Ssd_int_status_srd_status_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_status_srd_status_BIT_MASK                           0x400000
#define REG_Ssd_int_status_srd_status_BIT_IDX                            22
#define REG_Ssd_int_status_srd_status_NUM_OF_BITS                        1
#define REG_Ssd_int_status_srd_status_MIN                                0
#define REG_Ssd_int_status_srd_status_MAX                                1
#define REG_SSD_INT_STATUS_SRD_STATUS_VALUE_NOT_PENDING                  0
#define REG_SSD_INT_STATUS_SRD_STATUS_VALUE_PENDING                      1

/*!
 * @brief reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_snd_int_status_91
#define REG_Ssd_rsd_snd_int_status_91_OFFSET                               0x3FFE0
#define REG_Ssd_rsd_snd_int_status_91_MASK                               0x1
#define REG_Ssd_rsd_snd_int_status_91_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_status_91_BIT_MASK                           0x800000
#define REG_Ssd_rsd_snd_int_status_91_BIT_IDX                            23
#define REG_Ssd_rsd_snd_int_status_91_NUM_OF_BITS                        1
#define REG_Ssd_rsd_snd_int_status_91_MIN                                0
#define REG_Ssd_rsd_snd_int_status_91_MAX                                1

/*!
 * @brief indicates a pending interrupt caused to inform the DTL initiator that a new transactions on a high latency software control register will be granted (alias: RTF = Ready for Transfer Flag)
 *        0 = not pending
 *        1 = pending
 * @range 0..1
 ***/
#define REG_Ssd_int_status_tnsact_grant
#define REG_Ssd_int_status_tnsact_grant_OFFSET                             0x3FFE0
#define REG_Ssd_int_status_tnsact_grant_MASK                             0x1
#define REG_Ssd_int_status_tnsact_grant_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_status_tnsact_grant_BIT_MASK                         0x1000000
#define REG_Ssd_int_status_tnsact_grant_BIT_IDX                          24
#define REG_Ssd_int_status_tnsact_grant_NUM_OF_BITS                      1
#define REG_Ssd_int_status_tnsact_grant_MIN                              0
#define REG_Ssd_int_status_tnsact_grant_MAX                              1
#define REG_SSD_INT_STATUS_TNSACT_GRANT_VALUE_NOT_PENDING                0
#define REG_SSD_INT_STATUS_TNSACT_GRANT_VALUE_PENDING                    1

/*!
 * @brief unused
 * @range 0..127
 ***/
#define REG_Ssd_rsd_snd_int_status_93
#define REG_Ssd_rsd_snd_int_status_93_OFFSET                               0x3FFE0
#define REG_Ssd_rsd_snd_int_status_93_MASK                               0x7f
#define REG_Ssd_rsd_snd_int_status_93_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_status_93_BIT_MASK                           0xfe000000
#define REG_Ssd_rsd_snd_int_status_93_BIT_IDX                            25
#define REG_Ssd_rsd_snd_int_status_93_NUM_OF_BITS                        7
#define REG_Ssd_rsd_snd_int_status_93_MIN                                0
#define REG_Ssd_rsd_snd_int_status_93_MAX                                127

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_stdres
#define REG_Ssd_int_enable_status_stdres_OFFSET                            0x3FFE4
#define REG_Ssd_int_enable_status_stdres_MASK                            0x1
#define REG_Ssd_int_enable_status_stdres_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_enable_status_stdres_BIT_MASK                        0x1
#define REG_Ssd_int_enable_status_stdres_BIT_IDX                         0
#define REG_Ssd_int_enable_status_stdres_NUM_OF_BITS                     1
#define REG_Ssd_int_enable_status_stdres_MIN                             0
#define REG_Ssd_int_enable_status_stdres_MAX                             1
#define REG_SSD_INT_ENABLE_STATUS_STDRES_VALUE_DISABLED                  0
#define REG_SSD_INT_ENABLE_STATUS_STDRES_VALUE_ENABLED                   1

/*!
 * @brief reserved
 * @range 0..15
 ***/
#define REG_Ssd_rsd_snd_int_enable_96
#define REG_Ssd_rsd_snd_int_enable_96_OFFSET                               0x3FFE4
#define REG_Ssd_rsd_snd_int_enable_96_MASK                               0xf
#define REG_Ssd_rsd_snd_int_enable_96_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_enable_96_BIT_MASK                           0x1e
#define REG_Ssd_rsd_snd_int_enable_96_BIT_IDX                            1
#define REG_Ssd_rsd_snd_int_enable_96_NUM_OF_BITS                        4
#define REG_Ssd_rsd_snd_int_enable_96_MIN                                0
#define REG_Ssd_rsd_snd_int_enable_96_MAX                                15

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_gst
#define REG_Ssd_int_enable_status_gst_OFFSET                               0x3FFE4
#define REG_Ssd_int_enable_status_gst_MASK                               0x1
#define REG_Ssd_int_enable_status_gst_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_enable_status_gst_BIT_MASK                           0x20
#define REG_Ssd_int_enable_status_gst_BIT_IDX                            5
#define REG_Ssd_int_enable_status_gst_NUM_OF_BITS                        1
#define REG_Ssd_int_enable_status_gst_MIN                                0
#define REG_Ssd_int_enable_status_gst_MAX                                1
#define REG_SSD_INT_ENABLE_STATUS_GST_VALUE_DISABLED                     0
#define REG_SSD_INT_ENABLE_STATUS_GST_VALUE_ENABLED                      1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_gdu
#define REG_Ssd_int_enable_status_gdu_OFFSET                               0x3FFE4
#define REG_Ssd_int_enable_status_gdu_MASK                               0x1
#define REG_Ssd_int_enable_status_gdu_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_enable_status_gdu_BIT_MASK                           0x40
#define REG_Ssd_int_enable_status_gdu_BIT_IDX                            6
#define REG_Ssd_int_enable_status_gdu_NUM_OF_BITS                        1
#define REG_Ssd_int_enable_status_gdu_MIN                                0
#define REG_Ssd_int_enable_status_gdu_MAX                                1
#define REG_SSD_INT_ENABLE_STATUS_GDU_VALUE_DISABLED                     0
#define REG_SSD_INT_ENABLE_STATUS_GDU_VALUE_ENABLED                      1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_adu
#define REG_Ssd_int_enable_status_adu_OFFSET                               0x3FFE4
#define REG_Ssd_int_enable_status_adu_MASK                               0x1
#define REG_Ssd_int_enable_status_adu_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_enable_status_adu_BIT_MASK                           0x100
#define REG_Ssd_int_enable_status_adu_BIT_IDX                            8
#define REG_Ssd_int_enable_status_adu_NUM_OF_BITS                        1
#define REG_Ssd_int_enable_status_adu_MIN                                0
#define REG_Ssd_int_enable_status_adu_MAX                                1
#define REG_SSD_INT_ENABLE_STATUS_ADU_VALUE_DISABLED                     0
#define REG_SSD_INT_ENABLE_STATUS_ADU_VALUE_ENABLED                      1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_ast
#define REG_Ssd_int_enable_status_ast_OFFSET                               0x3FFE4
#define REG_Ssd_int_enable_status_ast_MASK                               0x1
#define REG_Ssd_int_enable_status_ast_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_enable_status_ast_BIT_MASK                           0x200
#define REG_Ssd_int_enable_status_ast_BIT_IDX                            9
#define REG_Ssd_int_enable_status_ast_NUM_OF_BITS                        1
#define REG_Ssd_int_enable_status_ast_MIN                                0
#define REG_Ssd_int_enable_status_ast_MAX                                1
#define REG_SSD_INT_ENABLE_STATUS_AST_VALUE_DISABLED                     0
#define REG_SSD_INT_ENABLE_STATUS_AST_VALUE_ENABLED                      1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_aamut
#define REG_Ssd_int_enable_status_aamut_OFFSET                             0x3FFE4
#define REG_Ssd_int_enable_status_aamut_MASK                             0x1
#define REG_Ssd_int_enable_status_aamut_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_enable_status_aamut_BIT_MASK                         0x400
#define REG_Ssd_int_enable_status_aamut_BIT_IDX                          10
#define REG_Ssd_int_enable_status_aamut_NUM_OF_BITS                      1
#define REG_Ssd_int_enable_status_aamut_MIN                              0
#define REG_Ssd_int_enable_status_aamut_MAX                              1
#define REG_SSD_INT_ENABLE_STATUS_AAMUT_VALUE_DISABLED                   0
#define REG_SSD_INT_ENABLE_STATUS_AAMUT_VALUE_ENABLED                    1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_bpilot
#define REG_Ssd_int_enable_status_bpilot_OFFSET                            0x3FFE4
#define REG_Ssd_int_enable_status_bpilot_MASK                            0x1
#define REG_Ssd_int_enable_status_bpilot_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_enable_status_bpilot_BIT_MASK                        0x800
#define REG_Ssd_int_enable_status_bpilot_BIT_IDX                         11
#define REG_Ssd_int_enable_status_bpilot_NUM_OF_BITS                     1
#define REG_Ssd_int_enable_status_bpilot_MIN                             0
#define REG_Ssd_int_enable_status_bpilot_MAX                             1
#define REG_SSD_INT_ENABLE_STATUS_BPILOT_VALUE_DISABLED                  0
#define REG_SSD_INT_ENABLE_STATUS_BPILOT_VALUE_ENABLED                   1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_sapdet
#define REG_Ssd_int_enable_status_sapdet_OFFSET                            0x3FFE4
#define REG_Ssd_int_enable_status_sapdet_MASK                            0x1
#define REG_Ssd_int_enable_status_sapdet_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_enable_status_sapdet_BIT_MASK                        0x1000
#define REG_Ssd_int_enable_status_sapdet_BIT_IDX                         12
#define REG_Ssd_int_enable_status_sapdet_NUM_OF_BITS                     1
#define REG_Ssd_int_enable_status_sapdet_MIN                             0
#define REG_Ssd_int_enable_status_sapdet_MAX                             1
#define REG_SSD_INT_ENABLE_STATUS_SAPDET_VALUE_DISABLED                  0
#define REG_SSD_INT_ENABLE_STATUS_SAPDET_VALUE_ENABLED                   1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_bamut
#define REG_Ssd_int_enable_status_bamut_OFFSET                             0x3FFE4
#define REG_Ssd_int_enable_status_bamut_MASK                             0x1
#define REG_Ssd_int_enable_status_bamut_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_enable_status_bamut_BIT_MASK                         0x2000
#define REG_Ssd_int_enable_status_bamut_BIT_IDX                          13
#define REG_Ssd_int_enable_status_bamut_NUM_OF_BITS                      1
#define REG_Ssd_int_enable_status_bamut_MIN                              0
#define REG_Ssd_int_enable_status_bamut_MAX                              1
#define REG_SSD_INT_ENABLE_STATUS_BAMUT_VALUE_DISABLED                   0
#define REG_SSD_INT_ENABLE_STATUS_BAMUT_VALUE_ENABLED                    1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_sapmut
#define REG_Ssd_int_enable_status_sapmut_OFFSET                            0x3FFE4
#define REG_Ssd_int_enable_status_sapmut_MASK                            0x1
#define REG_Ssd_int_enable_status_sapmut_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_enable_status_sapmut_BIT_MASK                        0x4000
#define REG_Ssd_int_enable_status_sapmut_BIT_IDX                         14
#define REG_Ssd_int_enable_status_sapmut_NUM_OF_BITS                     1
#define REG_Ssd_int_enable_status_sapmut_MIN                             0
#define REG_Ssd_int_enable_status_sapmut_MAX                             1
#define REG_SSD_INT_ENABLE_STATUS_SAPMUT_VALUE_DISABLED                  0
#define REG_SSD_INT_ENABLE_STATUS_SAPMUT_VALUE_ENABLED                   1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_vdsp_c
#define REG_Ssd_int_enable_status_vdsp_c_OFFSET                            0x3FFE4
#define REG_Ssd_int_enable_status_vdsp_c_MASK                            0x1
#define REG_Ssd_int_enable_status_vdsp_c_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_enable_status_vdsp_c_BIT_MASK                        0x8000
#define REG_Ssd_int_enable_status_vdsp_c_BIT_IDX                         15
#define REG_Ssd_int_enable_status_vdsp_c_NUM_OF_BITS                     1
#define REG_Ssd_int_enable_status_vdsp_c_MIN                             0
#define REG_Ssd_int_enable_status_vdsp_c_MAX                             1
#define REG_SSD_INT_ENABLE_STATUS_VDSP_C_VALUE_DISABLED                  0
#define REG_SSD_INT_ENABLE_STATUS_VDSP_C_VALUE_ENABLED                   1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_nicst_c
#define REG_Ssd_int_enable_status_nicst_c_OFFSET                           0x3FFE4
#define REG_Ssd_int_enable_status_nicst_c_MASK                           0x1
#define REG_Ssd_int_enable_status_nicst_c_SIGNED_BIT_MASK                0x0
#define REG_Ssd_int_enable_status_nicst_c_BIT_MASK                       0x10000
#define REG_Ssd_int_enable_status_nicst_c_BIT_IDX                        16
#define REG_Ssd_int_enable_status_nicst_c_NUM_OF_BITS                    1
#define REG_Ssd_int_enable_status_nicst_c_MIN                            0
#define REG_Ssd_int_enable_status_nicst_c_MAX                            1
#define REG_SSD_INT_ENABLE_STATUS_NICST_C_VALUE_DISABLED                 0
#define REG_SSD_INT_ENABLE_STATUS_NICST_C_VALUE_ENABLED                  1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_nicdu_c
#define REG_Ssd_int_enable_status_nicdu_c_OFFSET                           0x3FFE4
#define REG_Ssd_int_enable_status_nicdu_c_MASK                           0x1
#define REG_Ssd_int_enable_status_nicdu_c_SIGNED_BIT_MASK                0x0
#define REG_Ssd_int_enable_status_nicdu_c_BIT_MASK                       0x20000
#define REG_Ssd_int_enable_status_nicdu_c_BIT_IDX                        17
#define REG_Ssd_int_enable_status_nicdu_c_NUM_OF_BITS                    1
#define REG_Ssd_int_enable_status_nicdu_c_MIN                            0
#define REG_Ssd_int_enable_status_nicdu_c_MAX                            1
#define REG_SSD_INT_ENABLE_STATUS_NICDU_C_VALUE_DISABLED                 0
#define REG_SSD_INT_ENABLE_STATUS_NICDU_C_VALUE_ENABLED                  1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_namut
#define REG_Ssd_int_enable_status_namut_OFFSET                             0x3FFE4
#define REG_Ssd_int_enable_status_namut_MASK                             0x1
#define REG_Ssd_int_enable_status_namut_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_enable_status_namut_BIT_MASK                         0x40000
#define REG_Ssd_int_enable_status_namut_BIT_IDX                          18
#define REG_Ssd_int_enable_status_namut_NUM_OF_BITS                      1
#define REG_Ssd_int_enable_status_namut_MIN                              0
#define REG_Ssd_int_enable_status_namut_MAX                              1
#define REG_SSD_INT_ENABLE_STATUS_NAMUT_VALUE_DISABLED                   0
#define REG_SSD_INT_ENABLE_STATUS_NAMUT_VALUE_ENABLED                    1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_rssf
#define REG_Ssd_int_enable_status_rssf_OFFSET                              0x3FFE4
#define REG_Ssd_int_enable_status_rssf_MASK                              0x1
#define REG_Ssd_int_enable_status_rssf_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_int_enable_status_rssf_BIT_MASK                          0x80000
#define REG_Ssd_int_enable_status_rssf_BIT_IDX                           19
#define REG_Ssd_int_enable_status_rssf_NUM_OF_BITS                       1
#define REG_Ssd_int_enable_status_rssf_MIN                               0
#define REG_Ssd_int_enable_status_rssf_MAX                               1
#define REG_SSD_INT_ENABLE_STATUS_RSSF_VALUE_DISABLED                    0
#define REG_SSD_INT_ENABLE_STATUS_RSSF_VALUE_ENABLED                     1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_initstat
#define REG_Ssd_int_enable_status_initstat_OFFSET                          0x3FFE4
#define REG_Ssd_int_enable_status_initstat_MASK                          0x1
#define REG_Ssd_int_enable_status_initstat_SIGNED_BIT_MASK               0x0
#define REG_Ssd_int_enable_status_initstat_BIT_MASK                      0x100000
#define REG_Ssd_int_enable_status_initstat_BIT_IDX                       20
#define REG_Ssd_int_enable_status_initstat_NUM_OF_BITS                   1
#define REG_Ssd_int_enable_status_initstat_MIN                           0
#define REG_Ssd_int_enable_status_initstat_MAX                           1
#define REG_SSD_INT_ENABLE_STATUS_INITSTAT_VALUE_DISABLED                0
#define REG_SSD_INT_ENABLE_STATUS_INITSTAT_VALUE_ENABLED                 1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_src_unlock
#define REG_Ssd_int_enable_status_src_unlock_OFFSET                        0x3FFE4
#define REG_Ssd_int_enable_status_src_unlock_MASK                        0x1
#define REG_Ssd_int_enable_status_src_unlock_SIGNED_BIT_MASK             0x0
#define REG_Ssd_int_enable_status_src_unlock_BIT_MASK                    0x200000
#define REG_Ssd_int_enable_status_src_unlock_BIT_IDX                     21
#define REG_Ssd_int_enable_status_src_unlock_NUM_OF_BITS                 1
#define REG_Ssd_int_enable_status_src_unlock_MIN                         0
#define REG_Ssd_int_enable_status_src_unlock_MAX                         1
#define REG_SSD_INT_ENABLE_STATUS_SRC_UNLOCK_VALUE_DISABLED              0
#define REG_SSD_INT_ENABLE_STATUS_SRC_UNLOCK_VALUE_ENABLED               1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_srd_status
#define REG_Ssd_int_enable_status_srd_status_OFFSET                        0x3FFE4
#define REG_Ssd_int_enable_status_srd_status_MASK                        0x1
#define REG_Ssd_int_enable_status_srd_status_SIGNED_BIT_MASK             0x0
#define REG_Ssd_int_enable_status_srd_status_BIT_MASK                    0x400000
#define REG_Ssd_int_enable_status_srd_status_BIT_IDX                     22
#define REG_Ssd_int_enable_status_srd_status_NUM_OF_BITS                 1
#define REG_Ssd_int_enable_status_srd_status_MIN                         0
#define REG_Ssd_int_enable_status_srd_status_MAX                         1
#define REG_SSD_INT_ENABLE_STATUS_SRD_STATUS_VALUE_DISABLED              0
#define REG_SSD_INT_ENABLE_STATUS_SRD_STATUS_VALUE_ENABLED               1

/*!
 * @brief reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_snd_int_enable_115
#define REG_Ssd_rsd_snd_int_enable_115_OFFSET                              0x3FFE4
#define REG_Ssd_rsd_snd_int_enable_115_MASK                              0x1
#define REG_Ssd_rsd_snd_int_enable_115_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_rsd_snd_int_enable_115_BIT_MASK                          0x800000
#define REG_Ssd_rsd_snd_int_enable_115_BIT_IDX                           23
#define REG_Ssd_rsd_snd_int_enable_115_NUM_OF_BITS                       1
#define REG_Ssd_rsd_snd_int_enable_115_MIN                               0
#define REG_Ssd_rsd_snd_int_enable_115_MAX                               1

/*!
 * @brief enable interrupt request condition SND_INT_STATUS[i]
 *        0 = disabled
 *        1 = enabled
 * @range 0..1
 ***/
#define REG_Ssd_int_enable_status_tnsact_grant
#define REG_Ssd_int_enable_status_tnsact_grant_OFFSET                      0x3FFE4
#define REG_Ssd_int_enable_status_tnsact_grant_MASK                      0x1
#define REG_Ssd_int_enable_status_tnsact_grant_SIGNED_BIT_MASK           0x0
#define REG_Ssd_int_enable_status_tnsact_grant_BIT_MASK                  0x1000000
#define REG_Ssd_int_enable_status_tnsact_grant_BIT_IDX                   24
#define REG_Ssd_int_enable_status_tnsact_grant_NUM_OF_BITS               1
#define REG_Ssd_int_enable_status_tnsact_grant_MIN                       0
#define REG_Ssd_int_enable_status_tnsact_grant_MAX                       1
#define REG_SSD_INT_ENABLE_STATUS_TNSACT_GRANT_VALUE_DISABLED            0
#define REG_SSD_INT_ENABLE_STATUS_TNSACT_GRANT_VALUE_ENABLED             1

/*!
 * @brief unused
 * @range 0..127
 ***/
#define REG_Ssd_rsd_snd_int_enable_117
#define REG_Ssd_rsd_snd_int_enable_117_OFFSET                              0x3FFE4
#define REG_Ssd_rsd_snd_int_enable_117_MASK                              0x7f
#define REG_Ssd_rsd_snd_int_enable_117_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_rsd_snd_int_enable_117_BIT_MASK                          0xfe000000
#define REG_Ssd_rsd_snd_int_enable_117_BIT_IDX                           25
#define REG_Ssd_rsd_snd_int_enable_117_NUM_OF_BITS                       7
#define REG_Ssd_rsd_snd_int_enable_117_MIN                               0
#define REG_Ssd_rsd_snd_int_enable_117_MAX                               127

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_stdres
#define REG_Ssd_int_clear_status_stdres_OFFSET                             0x3FFE8
#define REG_Ssd_int_clear_status_stdres_MASK                             0x1
#define REG_Ssd_int_clear_status_stdres_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_clear_status_stdres_BIT_MASK                         0x1
#define REG_Ssd_int_clear_status_stdres_BIT_IDX                          0
#define REG_Ssd_int_clear_status_stdres_NUM_OF_BITS                      1
#define REG_Ssd_int_clear_status_stdres_MIN                              0
#define REG_Ssd_int_clear_status_stdres_MAX                              1
#define REG_SSD_INT_CLEAR_STATUS_STDRES_VALUE_PRESERVE_OLD_STATE         0
#define REG_SSD_INT_CLEAR_STATUS_STDRES_VALUE_CLEAR                      1

/*!
 * @brief reserved
 * @range 0..15
 ***/
#define REG_Ssd_rsd_snd_int_clear_120
#define REG_Ssd_rsd_snd_int_clear_120_OFFSET                               0x3FFE8
#define REG_Ssd_rsd_snd_int_clear_120_MASK                               0xf
#define REG_Ssd_rsd_snd_int_clear_120_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_clear_120_BIT_MASK                           0x1e
#define REG_Ssd_rsd_snd_int_clear_120_BIT_IDX                            1
#define REG_Ssd_rsd_snd_int_clear_120_NUM_OF_BITS                        4
#define REG_Ssd_rsd_snd_int_clear_120_MIN                                0
#define REG_Ssd_rsd_snd_int_clear_120_MAX                                15

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_gst
#define REG_Ssd_int_clear_status_gst_OFFSET                                0x3FFE8
#define REG_Ssd_int_clear_status_gst_MASK                                0x1
#define REG_Ssd_int_clear_status_gst_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_clear_status_gst_BIT_MASK                            0x20
#define REG_Ssd_int_clear_status_gst_BIT_IDX                             5
#define REG_Ssd_int_clear_status_gst_NUM_OF_BITS                         1
#define REG_Ssd_int_clear_status_gst_MIN                                 0
#define REG_Ssd_int_clear_status_gst_MAX                                 1
#define REG_SSD_INT_CLEAR_STATUS_GST_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_CLEAR_STATUS_GST_VALUE_CLEAR                         1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_gdu
#define REG_Ssd_int_clear_status_gdu_OFFSET                                0x3FFE8
#define REG_Ssd_int_clear_status_gdu_MASK                                0x1
#define REG_Ssd_int_clear_status_gdu_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_clear_status_gdu_BIT_MASK                            0x40
#define REG_Ssd_int_clear_status_gdu_BIT_IDX                             6
#define REG_Ssd_int_clear_status_gdu_NUM_OF_BITS                         1
#define REG_Ssd_int_clear_status_gdu_MIN                                 0
#define REG_Ssd_int_clear_status_gdu_MAX                                 1
#define REG_SSD_INT_CLEAR_STATUS_GDU_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_CLEAR_STATUS_GDU_VALUE_CLEAR                         1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_adu
#define REG_Ssd_int_clear_status_adu_OFFSET                                0x3FFE8
#define REG_Ssd_int_clear_status_adu_MASK                                0x1
#define REG_Ssd_int_clear_status_adu_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_clear_status_adu_BIT_MASK                            0x100
#define REG_Ssd_int_clear_status_adu_BIT_IDX                             8
#define REG_Ssd_int_clear_status_adu_NUM_OF_BITS                         1
#define REG_Ssd_int_clear_status_adu_MIN                                 0
#define REG_Ssd_int_clear_status_adu_MAX                                 1
#define REG_SSD_INT_CLEAR_STATUS_ADU_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_CLEAR_STATUS_ADU_VALUE_CLEAR                         1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_ast
#define REG_Ssd_int_clear_status_ast_OFFSET                                0x3FFE8
#define REG_Ssd_int_clear_status_ast_MASK                                0x1
#define REG_Ssd_int_clear_status_ast_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_clear_status_ast_BIT_MASK                            0x200
#define REG_Ssd_int_clear_status_ast_BIT_IDX                             9
#define REG_Ssd_int_clear_status_ast_NUM_OF_BITS                         1
#define REG_Ssd_int_clear_status_ast_MIN                                 0
#define REG_Ssd_int_clear_status_ast_MAX                                 1
#define REG_SSD_INT_CLEAR_STATUS_AST_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_CLEAR_STATUS_AST_VALUE_CLEAR                         1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_aamut
#define REG_Ssd_int_clear_status_aamut_OFFSET                              0x3FFE8
#define REG_Ssd_int_clear_status_aamut_MASK                              0x1
#define REG_Ssd_int_clear_status_aamut_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_int_clear_status_aamut_BIT_MASK                          0x400
#define REG_Ssd_int_clear_status_aamut_BIT_IDX                           10
#define REG_Ssd_int_clear_status_aamut_NUM_OF_BITS                       1
#define REG_Ssd_int_clear_status_aamut_MIN                               0
#define REG_Ssd_int_clear_status_aamut_MAX                               1
#define REG_SSD_INT_CLEAR_STATUS_AAMUT_VALUE_PRESERVE_OLD_STATE          0
#define REG_SSD_INT_CLEAR_STATUS_AAMUT_VALUE_CLEAR                       1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_bpilot
#define REG_Ssd_int_clear_status_bpilot_OFFSET                             0x3FFE8
#define REG_Ssd_int_clear_status_bpilot_MASK                             0x1
#define REG_Ssd_int_clear_status_bpilot_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_clear_status_bpilot_BIT_MASK                         0x800
#define REG_Ssd_int_clear_status_bpilot_BIT_IDX                          11
#define REG_Ssd_int_clear_status_bpilot_NUM_OF_BITS                      1
#define REG_Ssd_int_clear_status_bpilot_MIN                              0
#define REG_Ssd_int_clear_status_bpilot_MAX                              1
#define REG_SSD_INT_CLEAR_STATUS_BPILOT_VALUE_PRESERVE_OLD_STATE         0
#define REG_SSD_INT_CLEAR_STATUS_BPILOT_VALUE_CLEAR                      1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_sapdet
#define REG_Ssd_int_clear_status_sapdet_OFFSET                             0x3FFE8
#define REG_Ssd_int_clear_status_sapdet_MASK                             0x1
#define REG_Ssd_int_clear_status_sapdet_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_clear_status_sapdet_BIT_MASK                         0x1000
#define REG_Ssd_int_clear_status_sapdet_BIT_IDX                          12
#define REG_Ssd_int_clear_status_sapdet_NUM_OF_BITS                      1
#define REG_Ssd_int_clear_status_sapdet_MIN                              0
#define REG_Ssd_int_clear_status_sapdet_MAX                              1
#define REG_SSD_INT_CLEAR_STATUS_SAPDET_VALUE_PRESERVE_OLD_STATE         0
#define REG_SSD_INT_CLEAR_STATUS_SAPDET_VALUE_CLEAR                      1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_bamut
#define REG_Ssd_int_clear_status_bamut_OFFSET                              0x3FFE8
#define REG_Ssd_int_clear_status_bamut_MASK                              0x1
#define REG_Ssd_int_clear_status_bamut_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_int_clear_status_bamut_BIT_MASK                          0x2000
#define REG_Ssd_int_clear_status_bamut_BIT_IDX                           13
#define REG_Ssd_int_clear_status_bamut_NUM_OF_BITS                       1
#define REG_Ssd_int_clear_status_bamut_MIN                               0
#define REG_Ssd_int_clear_status_bamut_MAX                               1
#define REG_SSD_INT_CLEAR_STATUS_BAMUT_VALUE_PRESERVE_OLD_STATE          0
#define REG_SSD_INT_CLEAR_STATUS_BAMUT_VALUE_CLEAR                       1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_sapmut
#define REG_Ssd_int_clear_status_sapmut_OFFSET                             0x3FFE8
#define REG_Ssd_int_clear_status_sapmut_MASK                             0x1
#define REG_Ssd_int_clear_status_sapmut_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_clear_status_sapmut_BIT_MASK                         0x4000
#define REG_Ssd_int_clear_status_sapmut_BIT_IDX                          14
#define REG_Ssd_int_clear_status_sapmut_NUM_OF_BITS                      1
#define REG_Ssd_int_clear_status_sapmut_MIN                              0
#define REG_Ssd_int_clear_status_sapmut_MAX                              1
#define REG_SSD_INT_CLEAR_STATUS_SAPMUT_VALUE_PRESERVE_OLD_STATE         0
#define REG_SSD_INT_CLEAR_STATUS_SAPMUT_VALUE_CLEAR                      1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_vdsp_c
#define REG_Ssd_int_clear_status_vdsp_c_OFFSET                             0x3FFE8
#define REG_Ssd_int_clear_status_vdsp_c_MASK                             0x1
#define REG_Ssd_int_clear_status_vdsp_c_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_clear_status_vdsp_c_BIT_MASK                         0x8000
#define REG_Ssd_int_clear_status_vdsp_c_BIT_IDX                          15
#define REG_Ssd_int_clear_status_vdsp_c_NUM_OF_BITS                      1
#define REG_Ssd_int_clear_status_vdsp_c_MIN                              0
#define REG_Ssd_int_clear_status_vdsp_c_MAX                              1
#define REG_SSD_INT_CLEAR_STATUS_VDSP_C_VALUE_PRESERVE_OLD_STATE         0
#define REG_SSD_INT_CLEAR_STATUS_VDSP_C_VALUE_CLEAR                      1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_nicst_c
#define REG_Ssd_int_clear_status_nicst_c_OFFSET                            0x3FFE8
#define REG_Ssd_int_clear_status_nicst_c_MASK                            0x1
#define REG_Ssd_int_clear_status_nicst_c_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_clear_status_nicst_c_BIT_MASK                        0x10000
#define REG_Ssd_int_clear_status_nicst_c_BIT_IDX                         16
#define REG_Ssd_int_clear_status_nicst_c_NUM_OF_BITS                     1
#define REG_Ssd_int_clear_status_nicst_c_MIN                             0
#define REG_Ssd_int_clear_status_nicst_c_MAX                             1
#define REG_SSD_INT_CLEAR_STATUS_NICST_C_VALUE_PRESERVE_OLD_STATE        0
#define REG_SSD_INT_CLEAR_STATUS_NICST_C_VALUE_CLEAR                     1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_nicdu_c
#define REG_Ssd_int_clear_status_nicdu_c_OFFSET                            0x3FFE8
#define REG_Ssd_int_clear_status_nicdu_c_MASK                            0x1
#define REG_Ssd_int_clear_status_nicdu_c_SIGNED_BIT_MASK                 0x0
#define REG_Ssd_int_clear_status_nicdu_c_BIT_MASK                        0x20000
#define REG_Ssd_int_clear_status_nicdu_c_BIT_IDX                         17
#define REG_Ssd_int_clear_status_nicdu_c_NUM_OF_BITS                     1
#define REG_Ssd_int_clear_status_nicdu_c_MIN                             0
#define REG_Ssd_int_clear_status_nicdu_c_MAX                             1
#define REG_SSD_INT_CLEAR_STATUS_NICDU_C_VALUE_PRESERVE_OLD_STATE        0
#define REG_SSD_INT_CLEAR_STATUS_NICDU_C_VALUE_CLEAR                     1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_namut
#define REG_Ssd_int_clear_status_namut_OFFSET                              0x3FFE8
#define REG_Ssd_int_clear_status_namut_MASK                              0x1
#define REG_Ssd_int_clear_status_namut_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_int_clear_status_namut_BIT_MASK                          0x40000
#define REG_Ssd_int_clear_status_namut_BIT_IDX                           18
#define REG_Ssd_int_clear_status_namut_NUM_OF_BITS                       1
#define REG_Ssd_int_clear_status_namut_MIN                               0
#define REG_Ssd_int_clear_status_namut_MAX                               1
#define REG_SSD_INT_CLEAR_STATUS_NAMUT_VALUE_PRESERVE_OLD_STATE          0
#define REG_SSD_INT_CLEAR_STATUS_NAMUT_VALUE_CLEAR                       1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_rssf
#define REG_Ssd_int_clear_status_rssf_OFFSET                               0x3FFE8
#define REG_Ssd_int_clear_status_rssf_MASK                               0x1
#define REG_Ssd_int_clear_status_rssf_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_clear_status_rssf_BIT_MASK                           0x80000
#define REG_Ssd_int_clear_status_rssf_BIT_IDX                            19
#define REG_Ssd_int_clear_status_rssf_NUM_OF_BITS                        1
#define REG_Ssd_int_clear_status_rssf_MIN                                0
#define REG_Ssd_int_clear_status_rssf_MAX                                1
#define REG_SSD_INT_CLEAR_STATUS_RSSF_VALUE_PRESERVE_OLD_STATE           0
#define REG_SSD_INT_CLEAR_STATUS_RSSF_VALUE_CLEAR                        1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_initstat
#define REG_Ssd_int_clear_status_initstat_OFFSET                           0x3FFE8
#define REG_Ssd_int_clear_status_initstat_MASK                           0x1
#define REG_Ssd_int_clear_status_initstat_SIGNED_BIT_MASK                0x0
#define REG_Ssd_int_clear_status_initstat_BIT_MASK                       0x100000
#define REG_Ssd_int_clear_status_initstat_BIT_IDX                        20
#define REG_Ssd_int_clear_status_initstat_NUM_OF_BITS                    1
#define REG_Ssd_int_clear_status_initstat_MIN                            0
#define REG_Ssd_int_clear_status_initstat_MAX                            1
#define REG_SSD_INT_CLEAR_STATUS_INITSTAT_VALUE_PRESERVE_OLD_STATE       0
#define REG_SSD_INT_CLEAR_STATUS_INITSTAT_VALUE_CLEAR                    1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_src_unlock
#define REG_Ssd_int_clear_status_src_unlock_OFFSET                         0x3FFE8
#define REG_Ssd_int_clear_status_src_unlock_MASK                         0x1
#define REG_Ssd_int_clear_status_src_unlock_SIGNED_BIT_MASK              0x0
#define REG_Ssd_int_clear_status_src_unlock_BIT_MASK                     0x200000
#define REG_Ssd_int_clear_status_src_unlock_BIT_IDX                      21
#define REG_Ssd_int_clear_status_src_unlock_NUM_OF_BITS                  1
#define REG_Ssd_int_clear_status_src_unlock_MIN                          0
#define REG_Ssd_int_clear_status_src_unlock_MAX                          1
#define REG_SSD_INT_CLEAR_STATUS_SRC_UNLOCK_VALUE_PRESERVE_OLD_STATE     0
#define REG_SSD_INT_CLEAR_STATUS_SRC_UNLOCK_VALUE_CLEAR                  1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_srd_status
#define REG_Ssd_int_clear_status_srd_status_OFFSET                         0x3FFE8
#define REG_Ssd_int_clear_status_srd_status_MASK                         0x1
#define REG_Ssd_int_clear_status_srd_status_SIGNED_BIT_MASK              0x0
#define REG_Ssd_int_clear_status_srd_status_BIT_MASK                     0x400000
#define REG_Ssd_int_clear_status_srd_status_BIT_IDX                      22
#define REG_Ssd_int_clear_status_srd_status_NUM_OF_BITS                  1
#define REG_Ssd_int_clear_status_srd_status_MIN                          0
#define REG_Ssd_int_clear_status_srd_status_MAX                          1
#define REG_SSD_INT_CLEAR_STATUS_SRD_STATUS_VALUE_PRESERVE_OLD_STATE     0
#define REG_SSD_INT_CLEAR_STATUS_SRD_STATUS_VALUE_CLEAR                  1

/*!
 * @brief reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_snd_int_clear_139
#define REG_Ssd_rsd_snd_int_clear_139_OFFSET                               0x3FFE8
#define REG_Ssd_rsd_snd_int_clear_139_MASK                               0x1
#define REG_Ssd_rsd_snd_int_clear_139_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_clear_139_BIT_MASK                           0x800000
#define REG_Ssd_rsd_snd_int_clear_139_BIT_IDX                            23
#define REG_Ssd_rsd_snd_int_clear_139_NUM_OF_BITS                        1
#define REG_Ssd_rsd_snd_int_clear_139_MIN                                0
#define REG_Ssd_rsd_snd_int_clear_139_MAX                                1

/*!
 * @brief clear interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_clear_status_tnsact_grant
#define REG_Ssd_int_clear_status_tnsact_grant_OFFSET                       0x3FFE8
#define REG_Ssd_int_clear_status_tnsact_grant_MASK                       0x1
#define REG_Ssd_int_clear_status_tnsact_grant_SIGNED_BIT_MASK            0x0
#define REG_Ssd_int_clear_status_tnsact_grant_BIT_MASK                   0x1000000
#define REG_Ssd_int_clear_status_tnsact_grant_BIT_IDX                    24
#define REG_Ssd_int_clear_status_tnsact_grant_NUM_OF_BITS                1
#define REG_Ssd_int_clear_status_tnsact_grant_MIN                        0
#define REG_Ssd_int_clear_status_tnsact_grant_MAX                        1
#define REG_SSD_INT_CLEAR_STATUS_TNSACT_GRANT_VALUE_PRESERVE_OLD_STATE   0
#define REG_SSD_INT_CLEAR_STATUS_TNSACT_GRANT_VALUE_CLEAR                1

/*!
 * @brief unused
 * @range 0..127
 ***/
#define REG_Ssd_rsd_snd_int_clear_141
#define REG_Ssd_rsd_snd_int_clear_141_OFFSET                               0x3FFE8
#define REG_Ssd_rsd_snd_int_clear_141_MASK                               0x7f
#define REG_Ssd_rsd_snd_int_clear_141_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_rsd_snd_int_clear_141_BIT_MASK                           0xfe000000
#define REG_Ssd_rsd_snd_int_clear_141_BIT_IDX                            25
#define REG_Ssd_rsd_snd_int_clear_141_NUM_OF_BITS                        7
#define REG_Ssd_rsd_snd_int_clear_141_MIN                                0
#define REG_Ssd_rsd_snd_int_clear_141_MAX                                127

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_stdres
#define REG_Ssd_int_set_status_stdres_OFFSET                               0x3FFEC
#define REG_Ssd_int_set_status_stdres_MASK                               0x1
#define REG_Ssd_int_set_status_stdres_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_set_status_stdres_BIT_MASK                           0x1
#define REG_Ssd_int_set_status_stdres_BIT_IDX                            0
#define REG_Ssd_int_set_status_stdres_NUM_OF_BITS                        1
#define REG_Ssd_int_set_status_stdres_MIN                                0
#define REG_Ssd_int_set_status_stdres_MAX                                1
#define REG_SSD_INT_SET_STATUS_STDRES_VALUE_PRESERVE_OLD_STATE           0
#define REG_SSD_INT_SET_STATUS_STDRES_VALUE_SET                          1

/*!
 * @brief reserved
 * @range 0..15
 ***/
#define REG_Ssd_rsd_snd_int_set_144
#define REG_Ssd_rsd_snd_int_set_144_OFFSET                                 0x3FFEC
#define REG_Ssd_rsd_snd_int_set_144_MASK                                 0xf
#define REG_Ssd_rsd_snd_int_set_144_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_rsd_snd_int_set_144_BIT_MASK                             0x1e
#define REG_Ssd_rsd_snd_int_set_144_BIT_IDX                              1
#define REG_Ssd_rsd_snd_int_set_144_NUM_OF_BITS                          4
#define REG_Ssd_rsd_snd_int_set_144_MIN                                  0
#define REG_Ssd_rsd_snd_int_set_144_MAX                                  15

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_gst
#define REG_Ssd_int_set_status_gst_OFFSET                                  0x3FFEC
#define REG_Ssd_int_set_status_gst_MASK                                  0x1
#define REG_Ssd_int_set_status_gst_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_int_set_status_gst_BIT_MASK                              0x20
#define REG_Ssd_int_set_status_gst_BIT_IDX                               5
#define REG_Ssd_int_set_status_gst_NUM_OF_BITS                           1
#define REG_Ssd_int_set_status_gst_MIN                                   0
#define REG_Ssd_int_set_status_gst_MAX                                   1
#define REG_SSD_INT_SET_STATUS_GST_VALUE_PRESERVE_OLD_STATE              0
#define REG_SSD_INT_SET_STATUS_GST_VALUE_SET                             1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_gdu
#define REG_Ssd_int_set_status_gdu_OFFSET                                  0x3FFEC
#define REG_Ssd_int_set_status_gdu_MASK                                  0x1
#define REG_Ssd_int_set_status_gdu_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_int_set_status_gdu_BIT_MASK                              0x40
#define REG_Ssd_int_set_status_gdu_BIT_IDX                               6
#define REG_Ssd_int_set_status_gdu_NUM_OF_BITS                           1
#define REG_Ssd_int_set_status_gdu_MIN                                   0
#define REG_Ssd_int_set_status_gdu_MAX                                   1
#define REG_SSD_INT_SET_STATUS_GDU_VALUE_PRESERVE_OLD_STATE              0
#define REG_SSD_INT_SET_STATUS_GDU_VALUE_SET                             1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_adu
#define REG_Ssd_int_set_status_adu_OFFSET                                  0x3FFEC
#define REG_Ssd_int_set_status_adu_MASK                                  0x1
#define REG_Ssd_int_set_status_adu_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_int_set_status_adu_BIT_MASK                              0x100
#define REG_Ssd_int_set_status_adu_BIT_IDX                               8
#define REG_Ssd_int_set_status_adu_NUM_OF_BITS                           1
#define REG_Ssd_int_set_status_adu_MIN                                   0
#define REG_Ssd_int_set_status_adu_MAX                                   1
#define REG_SSD_INT_SET_STATUS_ADU_VALUE_PRESERVE_OLD_STATE              0
#define REG_SSD_INT_SET_STATUS_ADU_VALUE_SET                             1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_ast
#define REG_Ssd_int_set_status_ast_OFFSET                                  0x3FFEC
#define REG_Ssd_int_set_status_ast_MASK                                  0x1
#define REG_Ssd_int_set_status_ast_SIGNED_BIT_MASK                       0x0
#define REG_Ssd_int_set_status_ast_BIT_MASK                              0x200
#define REG_Ssd_int_set_status_ast_BIT_IDX                               9
#define REG_Ssd_int_set_status_ast_NUM_OF_BITS                           1
#define REG_Ssd_int_set_status_ast_MIN                                   0
#define REG_Ssd_int_set_status_ast_MAX                                   1
#define REG_SSD_INT_SET_STATUS_AST_VALUE_PRESERVE_OLD_STATE              0
#define REG_SSD_INT_SET_STATUS_AST_VALUE_SET                             1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_aamut
#define REG_Ssd_int_set_status_aamut_OFFSET                                0x3FFEC
#define REG_Ssd_int_set_status_aamut_MASK                                0x1
#define REG_Ssd_int_set_status_aamut_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_set_status_aamut_BIT_MASK                            0x400
#define REG_Ssd_int_set_status_aamut_BIT_IDX                             10
#define REG_Ssd_int_set_status_aamut_NUM_OF_BITS                         1
#define REG_Ssd_int_set_status_aamut_MIN                                 0
#define REG_Ssd_int_set_status_aamut_MAX                                 1
#define REG_SSD_INT_SET_STATUS_AAMUT_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_SET_STATUS_AAMUT_VALUE_SET                           1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_bpilot
#define REG_Ssd_int_set_status_bpilot_OFFSET                               0x3FFEC
#define REG_Ssd_int_set_status_bpilot_MASK                               0x1
#define REG_Ssd_int_set_status_bpilot_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_set_status_bpilot_BIT_MASK                           0x800
#define REG_Ssd_int_set_status_bpilot_BIT_IDX                            11
#define REG_Ssd_int_set_status_bpilot_NUM_OF_BITS                        1
#define REG_Ssd_int_set_status_bpilot_MIN                                0
#define REG_Ssd_int_set_status_bpilot_MAX                                1
#define REG_SSD_INT_SET_STATUS_BPILOT_VALUE_PRESERVE_OLD_STATE           0
#define REG_SSD_INT_SET_STATUS_BPILOT_VALUE_SET                          1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_sapdet
#define REG_Ssd_int_set_status_sapdet_OFFSET                               0x3FFEC
#define REG_Ssd_int_set_status_sapdet_MASK                               0x1
#define REG_Ssd_int_set_status_sapdet_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_set_status_sapdet_BIT_MASK                           0x1000
#define REG_Ssd_int_set_status_sapdet_BIT_IDX                            12
#define REG_Ssd_int_set_status_sapdet_NUM_OF_BITS                        1
#define REG_Ssd_int_set_status_sapdet_MIN                                0
#define REG_Ssd_int_set_status_sapdet_MAX                                1
#define REG_SSD_INT_SET_STATUS_SAPDET_VALUE_PRESERVE_OLD_STATE           0
#define REG_SSD_INT_SET_STATUS_SAPDET_VALUE_SET                          1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_bamut
#define REG_Ssd_int_set_status_bamut_OFFSET                                0x3FFEC
#define REG_Ssd_int_set_status_bamut_MASK                                0x1
#define REG_Ssd_int_set_status_bamut_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_set_status_bamut_BIT_MASK                            0x2000
#define REG_Ssd_int_set_status_bamut_BIT_IDX                             13
#define REG_Ssd_int_set_status_bamut_NUM_OF_BITS                         1
#define REG_Ssd_int_set_status_bamut_MIN                                 0
#define REG_Ssd_int_set_status_bamut_MAX                                 1
#define REG_SSD_INT_SET_STATUS_BAMUT_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_SET_STATUS_BAMUT_VALUE_SET                           1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_sapmut
#define REG_Ssd_int_set_status_sapmut_OFFSET                               0x3FFEC
#define REG_Ssd_int_set_status_sapmut_MASK                               0x1
#define REG_Ssd_int_set_status_sapmut_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_set_status_sapmut_BIT_MASK                           0x4000
#define REG_Ssd_int_set_status_sapmut_BIT_IDX                            14
#define REG_Ssd_int_set_status_sapmut_NUM_OF_BITS                        1
#define REG_Ssd_int_set_status_sapmut_MIN                                0
#define REG_Ssd_int_set_status_sapmut_MAX                                1
#define REG_SSD_INT_SET_STATUS_SAPMUT_VALUE_PRESERVE_OLD_STATE           0
#define REG_SSD_INT_SET_STATUS_SAPMUT_VALUE_SET                          1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_vdsp_c
#define REG_Ssd_int_set_status_vdsp_c_OFFSET                               0x3FFEC
#define REG_Ssd_int_set_status_vdsp_c_MASK                               0x1
#define REG_Ssd_int_set_status_vdsp_c_SIGNED_BIT_MASK                    0x0
#define REG_Ssd_int_set_status_vdsp_c_BIT_MASK                           0x8000
#define REG_Ssd_int_set_status_vdsp_c_BIT_IDX                            15
#define REG_Ssd_int_set_status_vdsp_c_NUM_OF_BITS                        1
#define REG_Ssd_int_set_status_vdsp_c_MIN                                0
#define REG_Ssd_int_set_status_vdsp_c_MAX                                1
#define REG_SSD_INT_SET_STATUS_VDSP_C_VALUE_PRESERVE_OLD_STATE           0
#define REG_SSD_INT_SET_STATUS_VDSP_C_VALUE_SET                          1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_nicst_c
#define REG_Ssd_int_set_status_nicst_c_OFFSET                              0x3FFEC
#define REG_Ssd_int_set_status_nicst_c_MASK                              0x1
#define REG_Ssd_int_set_status_nicst_c_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_int_set_status_nicst_c_BIT_MASK                          0x10000
#define REG_Ssd_int_set_status_nicst_c_BIT_IDX                           16
#define REG_Ssd_int_set_status_nicst_c_NUM_OF_BITS                       1
#define REG_Ssd_int_set_status_nicst_c_MIN                               0
#define REG_Ssd_int_set_status_nicst_c_MAX                               1
#define REG_SSD_INT_SET_STATUS_NICST_C_VALUE_PRESERVE_OLD_STATE          0
#define REG_SSD_INT_SET_STATUS_NICST_C_VALUE_SET                         1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_nicdu_c
#define REG_Ssd_int_set_status_nicdu_c_OFFSET                              0x3FFEC
#define REG_Ssd_int_set_status_nicdu_c_MASK                              0x1
#define REG_Ssd_int_set_status_nicdu_c_SIGNED_BIT_MASK                   0x0
#define REG_Ssd_int_set_status_nicdu_c_BIT_MASK                          0x20000
#define REG_Ssd_int_set_status_nicdu_c_BIT_IDX                           17
#define REG_Ssd_int_set_status_nicdu_c_NUM_OF_BITS                       1
#define REG_Ssd_int_set_status_nicdu_c_MIN                               0
#define REG_Ssd_int_set_status_nicdu_c_MAX                               1
#define REG_SSD_INT_SET_STATUS_NICDU_C_VALUE_PRESERVE_OLD_STATE          0
#define REG_SSD_INT_SET_STATUS_NICDU_C_VALUE_SET                         1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_namut
#define REG_Ssd_int_set_status_namut_OFFSET                                0x3FFEC
#define REG_Ssd_int_set_status_namut_MASK                                0x1
#define REG_Ssd_int_set_status_namut_SIGNED_BIT_MASK                     0x0
#define REG_Ssd_int_set_status_namut_BIT_MASK                            0x40000
#define REG_Ssd_int_set_status_namut_BIT_IDX                             18
#define REG_Ssd_int_set_status_namut_NUM_OF_BITS                         1
#define REG_Ssd_int_set_status_namut_MIN                                 0
#define REG_Ssd_int_set_status_namut_MAX                                 1
#define REG_SSD_INT_SET_STATUS_NAMUT_VALUE_PRESERVE_OLD_STATE            0
#define REG_SSD_INT_SET_STATUS_NAMUT_VALUE_SET                           1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_rssf
#define REG_Ssd_int_set_status_rssf_OFFSET                                 0x3FFEC
#define REG_Ssd_int_set_status_rssf_MASK                                 0x1
#define REG_Ssd_int_set_status_rssf_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_int_set_status_rssf_BIT_MASK                             0x80000
#define REG_Ssd_int_set_status_rssf_BIT_IDX                              19
#define REG_Ssd_int_set_status_rssf_NUM_OF_BITS                          1
#define REG_Ssd_int_set_status_rssf_MIN                                  0
#define REG_Ssd_int_set_status_rssf_MAX                                  1
#define REG_SSD_INT_SET_STATUS_RSSF_VALUE_PRESERVE_OLD_STATE             0
#define REG_SSD_INT_SET_STATUS_RSSF_VALUE_SET                            1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_initstat
#define REG_Ssd_int_set_status_initstat_OFFSET                             0x3FFEC
#define REG_Ssd_int_set_status_initstat_MASK                             0x1
#define REG_Ssd_int_set_status_initstat_SIGNED_BIT_MASK                  0x0
#define REG_Ssd_int_set_status_initstat_BIT_MASK                         0x100000
#define REG_Ssd_int_set_status_initstat_BIT_IDX                          20
#define REG_Ssd_int_set_status_initstat_NUM_OF_BITS                      1
#define REG_Ssd_int_set_status_initstat_MIN                              0
#define REG_Ssd_int_set_status_initstat_MAX                              1
#define REG_SSD_INT_SET_STATUS_INITSTAT_VALUE_PRESERVE_OLD_STATE         0
#define REG_SSD_INT_SET_STATUS_INITSTAT_VALUE_SET                        1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_src_unlock
#define REG_Ssd_int_set_status_src_unlock_OFFSET                           0x3FFEC
#define REG_Ssd_int_set_status_src_unlock_MASK                           0x1
#define REG_Ssd_int_set_status_src_unlock_SIGNED_BIT_MASK                0x0
#define REG_Ssd_int_set_status_src_unlock_BIT_MASK                       0x200000
#define REG_Ssd_int_set_status_src_unlock_BIT_IDX                        21
#define REG_Ssd_int_set_status_src_unlock_NUM_OF_BITS                    1
#define REG_Ssd_int_set_status_src_unlock_MIN                            0
#define REG_Ssd_int_set_status_src_unlock_MAX                            1
#define REG_SSD_INT_SET_STATUS_SRC_UNLOCK_VALUE_PRESERVE_OLD_STATE       0
#define REG_SSD_INT_SET_STATUS_SRC_UNLOCK_VALUE_SET                      1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = set
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_srd_status
#define REG_Ssd_int_set_status_srd_status_OFFSET                           0x3FFEC
#define REG_Ssd_int_set_status_srd_status_MASK                           0x1
#define REG_Ssd_int_set_status_srd_status_SIGNED_BIT_MASK                0x0
#define REG_Ssd_int_set_status_srd_status_BIT_MASK                       0x400000
#define REG_Ssd_int_set_status_srd_status_BIT_IDX                        22
#define REG_Ssd_int_set_status_srd_status_NUM_OF_BITS                    1
#define REG_Ssd_int_set_status_srd_status_MIN                            0
#define REG_Ssd_int_set_status_srd_status_MAX                            1
#define REG_SSD_INT_SET_STATUS_SRD_STATUS_VALUE_PRESERVE_OLD_STATE       0
#define REG_SSD_INT_SET_STATUS_SRD_STATUS_VALUE_SET                      1

/*!
 * @brief reserved
 * @range 0..1
 ***/
#define REG_Ssd_rsd_snd_int_set_163
#define REG_Ssd_rsd_snd_int_set_163_OFFSET                                 0x3FFEC
#define REG_Ssd_rsd_snd_int_set_163_MASK                                 0x1
#define REG_Ssd_rsd_snd_int_set_163_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_rsd_snd_int_set_163_BIT_MASK                             0x800000
#define REG_Ssd_rsd_snd_int_set_163_BIT_IDX                              23
#define REG_Ssd_rsd_snd_int_set_163_NUM_OF_BITS                          1
#define REG_Ssd_rsd_snd_int_set_163_MIN                                  0
#define REG_Ssd_rsd_snd_int_set_163_MAX                                  1

/*!
 * @brief set interrupt request condition SND_INT_STATUS[i]
 *        0 = preserve old state
 *        1 = clear
 * @range 0..1
 ***/
#define REG_Ssd_int_set_status_tnsact_grant
#define REG_Ssd_int_set_status_tnsact_grant_OFFSET                         0x3FFEC
#define REG_Ssd_int_set_status_tnsact_grant_MASK                         0x1
#define REG_Ssd_int_set_status_tnsact_grant_SIGNED_BIT_MASK              0x0
#define REG_Ssd_int_set_status_tnsact_grant_BIT_MASK                     0x1000000
#define REG_Ssd_int_set_status_tnsact_grant_BIT_IDX                      24
#define REG_Ssd_int_set_status_tnsact_grant_NUM_OF_BITS                  1
#define REG_Ssd_int_set_status_tnsact_grant_MIN                          0
#define REG_Ssd_int_set_status_tnsact_grant_MAX                          1
#define REG_SSD_INT_SET_STATUS_TNSACT_GRANT_VALUE_PRESERVE_OLD_STATE     0
#define REG_SSD_INT_SET_STATUS_TNSACT_GRANT_VALUE_CLEAR                  1

/*!
 * @brief unused
 * @range 0..127
 ***/
#define REG_Ssd_rsd_snd_int_set_165
#define REG_Ssd_rsd_snd_int_set_165_OFFSET                                 0x3FFEC
#define REG_Ssd_rsd_snd_int_set_165_MASK                                 0x7f
#define REG_Ssd_rsd_snd_int_set_165_SIGNED_BIT_MASK                      0x0
#define REG_Ssd_rsd_snd_int_set_165_BIT_MASK                             0xfe000000
#define REG_Ssd_rsd_snd_int_set_165_BIT_IDX                              25
#define REG_Ssd_rsd_snd_int_set_165_NUM_OF_BITS                          7
#define REG_Ssd_rsd_snd_int_set_165_MIN                                  0
#define REG_Ssd_rsd_snd_int_set_165_MAX                                  127

#endif
