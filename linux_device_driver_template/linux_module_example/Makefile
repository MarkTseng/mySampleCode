# Sunplus CEVA DSP driver
# Author: Mark Tseng 
#


# Toolchain setting
TOOL_CHAIN_DIR=/home/mark/project/Buildroot/output/host/usr/bin
CROSS_COMPILE=$(TOOL_CHAIN_DIR)/mipsel-linux-
ARCH_PLATFORM=mips

# Kernel path
KERNELDIR=/home/mark/STB_Project/STB1502_Linux/linux-kernel/linux-2.6.33.7_SVN

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
PROJECT_DIR = /home/mark/project/spv9200/spv9200_mm
INCLUDE_DIR += -I$(PROJECT_DIR)/include
INCLUDE_DIR += -I$(PROJECT_DIR)/dsp_codec
EXTRA_CFLAGS += $(DEBFLAGS) $(INCLUDE_DIR)

# target module name 
#obj-m += ProcessList.o
#obj-m += traceinit.o
#obj-m += print_kern_ds.o
#obj-m += netlink_unicast.o
#obj-m += netlink_bcast.o
#obj-m += bytestream-example.o
#obj-m += link_list.o
#obj-m += timer.o
#obj-m += workqueue.o
#obj-m += tasklet.o
#obj-m += memory_size.o
#obj-m += slab_memory_pool.o
obj-m += interrupt.o
#obj-m += wait_queue_manual.o
#obj-m += proc.o
#obj-m += proc_quick.o
#obj-m += debugfs.o
#obj-m += system_call.o
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
