# Sunplus CEVA DSP driver
# Author: Mark Tseng 
#


# Toolchain setting
TOOL_CHAIN_DIR=/home/mark/MySoftware/bin
CROSS_COMPILE=arm-linux-gnueabihf-
ARCH_PLATFORM=arm

# Kernel path
KERNELDIR=/home/mark/mstar_workshop/IPCM-I3/source/linux-3.18

# module install directory
INSTALLDIR=/home/mark/SVN/kernelteam/FileSystem/ramfs/root/lib/modules/2.6.38_SPHE9200_V1

# Comment/uncomment the following line to enable/disable debugging
#DEBUG = y

ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULLC_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

# Add CFLAGS 
PROJECT_DIR = /home/mark/project/spv9200/spv9200_iommap
INCLUDE_DIR += -I$(PROJECT_DIR)/include
INCLUDE_DIR += -I$(PROJECT_DIR)/dsp_codec
EXTRA_CFLAGS += $(DEBFLAGS) $(INCLUDE_DIR)

# target module name 
TARGET = iommap
obj-m	:= iommap.o
# add other obj in iommap-objs
iommap-objs := spv9200_iommap.o 


all: modules

modules:
	#@echo $(EXTRA_CFLAGS)
	#$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules ARCH=$(ARCH_PLATFORM) CROSS_COMPILE=$(CROSS_COMPILE) V=1
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules ARCH=$(ARCH_PLATFORM) CROSS_COMPILE=$(CROSS_COMPILE)



install:
	install -c $(TARGET).ko $(INSTALLDIR)

uninstall:
	rm $(INSTALLDIR)/$(TARGET).ko

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers


depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
