TOOL_CHAIN_DIR=/home/mark/mstar_workshop/toolchain/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux/bin
CROSS_COMPILE=$(TOOL_CHAIN_DIR)/arm-linux-gnueabihf-
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
ProgramName = devmem print_segmemts netlink_recv netlink_send tailq_ex netlink_event mmaptest

all: $(ProgramName)
	@echo "Programs is ready $(ProgramName)" 
	
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
clean:
	$(RM) $(ProgramName) *.o
 
