#/**************************************************************************
# *                                                                        *
# *         Copyright (c) 2005 by Sunplus Technology Co., Ltd.             *
# *                                                                        *
# *  This software is copyrighted by and is the property of Sunplus        *
# *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
# *  Co., Ltd. This software may only be used in accordance with the       *
# *  corresponding license agreement. Any unauthorized use, duplication,   *
# *  distribution, or disclosure of this software is expressly forbidden.  *
# *                                                                        *
# *  This Copyright notice MUST not be removed or modified without prior   *
# *  written consent of Sunplus Technology Co., Ltd.                       *
# *                                                                        *
# *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
# *  software without notice.                                              *
# *                                                                        *
# *  Sunplus Technology Co., Ltd.                                          *
# *  19, Innovation First Road, Science-Based Industrial Park,             *
# *  Hsin-Chu, Taiwan, R.O.C.                                              *
# *                                                                        *
# *  Author: Jason                                                         *
# *                                                                        *
# *  Reviewer:                                                             *
# *                                                                        *
# **************************************************************************/

#---------------------------------------------------------------------------
# Target definition (User)
GOAL  = test_ipc

C_SRC = test_cross_flag.c test_cross_mutex.c test_cross_sema.c test_list.c  test_queue.c \
		test_cross.c test_cross_list.c test_cross_queue.c test_flag.c test_mutex.c test_sema.c

#---------------------------------------------------------------------------
# Source section (User)
#---------------------------------------------------------------------------
ROOT_DIR = ../..

SOURCE_DIR	+=$(ROOT_DIR)/src
TEST_DIR	+=$(ROOT_DIR)/test/ipc

include $(ROOT_DIR)/make.include
include $(ROOT_DIR)/Makefile.in

