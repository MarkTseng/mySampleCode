# Sunplus CEVA DSP driver
# Author: Mark Tseng 
#


# Toolchain setting
#TOOL_CHAIN_DIR=/home/mark/RTK_workshop/merlin/native_linux/kernel/system-rtk/tmp/toolchain/asdk-4.9.2-a53-EL-3.10-g2.19-a32n-141121/bin
#TOOL_CHAIN_DIR=/home/mark/mstar_workshop/toolchain/C4P/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin
TOOL_CHAIN_DIR=/opt/mstar_workshop/toolchain/ARMv8/gcc-linaro-7.2.1-2017.11-i686_aarch64-linux-gnu/bin
#CROSS_COMPILE=$(TOOL_CHAIN_DIR)/arm-linux-gnueabihf-
CROSS_COMPILE=$(TOOL_CHAIN_DIR)/aarch64-linux-gnu-
#ARCH_PLATFORM=arm
ARCH_PLATFORM=arm64

# Kernel path
#KERNELDIR=/home/mark/RTK_workshop/merlin/native_linux/kernel/linux/linux-3.10.19-rtk
#KERNELDIR=/home/mark/mstar_workshop/rd6/C4P/android-4.9
#KERNELDIR=/home/mark/usb/rd6/buildroot_mstar/output/build/linux-C4P_bringup
#KERNELDIR=/home/mark/usb/rd6/C4P/android-4.9_mtk
KERNELDIR=/opt/mstar_workshop/rd6/C4L/android-4.9_mtk

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
#PROJECT_DIR = /home/mark/project/spv9200/spv9200_mm
#INCLUDE_DIR += -I$(PROJECT_DIR)/include
#INCLUDE_DIR += -I$(PROJECT_DIR)/dsp_codec
#EXTRA_CFLAGS += $(DEBFLAGS) $(INCLUDE_DIR)

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
#obj-m += workqueue2.o
#obj-m += tasklet.o
#obj-m += memory_size.o
#obj-m += slab_memory_pool.o
#obj-m += interrupt.o
#obj-m += wait_queue_manual.o
#obj-m += proc.o
#obj-m += proc_quick.o
#obj-m += debugfs.o
#obj-m += system_call.o
#obj-m += cdev_device.o
#obj-m += kthread_example.o
#obj-m += kthread_hw_semaphore.o
#obj-m += file_log.o
#obj-m += cpuset.o
#obj-m += platform_driver_template.o
#obj-m += kprobe_test.o
#obj-m += jprobe_test.o
#obj-m += kretprobe_test.o
#obj-m += mmapdriver.o
#obj-m += file_log_vfs.o
#obj-m += lab3_poll.o
#obj-m += of_platform_device.o
#obj-m += snull.o
#obj-m += firmware.o
#obj-m += dma.o
all: modules

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules ARCH=$(ARCH_PLATFORM) CROSS_COMPILE=$(CROSS_COMPILE) V=1



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
