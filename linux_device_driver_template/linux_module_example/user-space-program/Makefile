#TOOL_CHAIN_DIR=/home/mark/mstar_workshop/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux/bin
CROSS_COMPILE=arm-linux-gnueabihf-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip

#FFMPEG LIB PATH
#FFMPEG_PATH = /home/mark/SVN/kernelteam/OpenSource/ffmpeg-0.7.9/MIPS_FFMPEG_LIB
#FFMPEG_CFLAGS = -I$(FFMPEG_PATH)/include
#FFMPEG_LDFLAGS = -L$(FFMPEG_PATH)/lib -lavutil -lavcodec -lavformat -lz -lm

# CFLAGS and LDFLAGS
CFLAGS = -Wall -O0 
LDFLAGS = 

#DEBUG_FLAG = -DDEBUG
DEBUG_FLAG = 
FUNC_TRACE_FLAG = -finstrument-functions

RM = rm -f
INC = ./
ProgramName = memtool print_segmemts netlink_recv netlink_send tailq_ex netlink_event mmaptest lab3_poll_test signal_from_kernel

all: $(ProgramName)
	@echo "Programs is ready $(ProgramName)" 
	@echo "copy to nfs"
	cp -a $(ProgramName) ~/nfs;sync
	
print_segmemts: print_segmemts.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

netlink_recv: netlink_recv.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

netlink_event: netlink_event.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

netlink_send: netlink_send.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

tailq_ex: tailq_ex.c queue.h
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

mmaptest: mmaptest.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

devmem: devmem.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

memtool: memtool.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

lab3_poll_test: lab3_poll_test.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

signal_from_kernel: signal_from_kernel.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

clean:
	$(RM) $(ProgramName) *.o
 
