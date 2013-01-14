# Sunplus DSP driver
# Author: Mark Tseng 
#


# Toolchain setting
TOOL_CHAIN_DIR=/home/mark/MySoftware/bin
CROSS_COMPILE=$(TOOL_CHAIN_DIR)/mips-linux-gnu-
ARCH_PLATFORM=mips

# Kernel path
KERNELDIR=/home/mark/SVN/kernelteam/LinuxKernel/linux-2.6.38

# module install directory
INSTALLDIR=/home/mark/SVN/kernelteam/FileSystem/ramfs/root/lib/modules/2.6.38_SPHE9200_V1

# test cdoe dir
TEST_CDOE_DIR=test_code

# Comment/uncomment the following line to enable/disable debugging
DEBUG = n

ifeq ($(DEBUG),y)
  DEBFLAGS = -O0 -g -DDEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

# Add CFLAGS 
PROJECT_DIR = $(shell pwd)
INCLUDE_DIR += -I$(PROJECT_DIR)/include
INCLUDE_DIR += -I$(PROJECT_DIR)/dsp_codec
EXTRA_CFLAGS += $(DEBFLAGS) $(INCLUDE_DIR)

# target module name 
TARGET = dsp

ifneq ($(KERNELRELEASE),)	# call from kernel build system
obj-m	:= dsp.o
# add other obj in dsp-objs
dsp-objs := dsp_api.o dsp_cmd.o dsp_mem.o dsp_msg.o dsp_util.o dsp_code.o gl_modctrl.o gl_i2c.o status_reg.o auddrv.o aud_hw.o aud_static.o drv_ssd.o drv_ssd_access.o drv_ssd_resource.o tvfe_audio.o dsp_mon.o dsp_main_driver.o 


else

all:  modules testcode 

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) ARCH=$(ARCH_PLATFORM) CROSS_COMPILE=$(CROSS_COMPILE) modules V=0

testcode:
	$(MAKE) -C $(TEST_CDOE_DIR)

install:
	install -c $(TARGET).ko $(INSTALLDIR)

uninstall:
	rm $(INSTALLDIR)/$(TARGET).ko

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers tags
	make clean -C $(TEST_CDOE_DIR)


depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif

endif
