#CROSS_COMPILE=mips-linux-gnu-
#TOOL_CHAIN_DIR=/home/mark/MySoftware/bin
TOOL_CHAIN_DIR=/home/mark/tmp/buildroot-2012.02/output/host/usr/bin
#CROSS_COMPILE=$(TOOL_CHAIN_DIR)/mips-linux-gnu-
CROSS_COMPILE=mips-linux-gnu-
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
GPP = $(CROSS_COMPILE)g++
CPP = $(CC) -E
AR = $(CROSS_COMPILE)ar
NM = $(CROSS_COMPILE)nm
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
RANLIB = $(CROSS_COMPILE)ranlib
SIZE = $(CROSS_COMPILE)size

RM = rm -f

#FFMPEG LIB PATH
#FFMPEG_PATH = /home/mark/SVN/kernelteam/OpenSource/ffmpeg-0.7.9/MIPS_FFMPEG_LIB
#FFMPEG_PATH = /home/mark/tmp/buildroot-2012.02/output/host/usr/mipsel-unknown-linux-uclibc/sysroot/usr
#FFMPEG_CFLAGS = -I$(FFMPEG_PATH)/include
#FFMPEG_LDFLAGS = -L$(FFMPEG_PATH)/lib -lavutil -lavcodec -lavformat -lz -lm -lswscale
FFMPEG_LDFLAGS = -lavutil -lavcodec -lavformat -lz -lm -lswscale

# CFLAGS and LDFLAGS
CFLAGS = -Wall -O0 $(FFMPEG_CFLAGS)
LDFLAGS = 

# DEBUG option
ifeq ($(DEBUG),y)
  CFLAGS += -g -DDEBUG
endif

# Function call trace
FUNC_TRACE_FLAG = -finstrument-functions

ProgramName = dsp_ioctl audio_api mediainfo avplayer dsp_mon

all: $(ProgramName)
	@echo "Programs is ready $(ProgramName)" 
	
dsp_ioctl: dsp_ioctl.c 
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

playback: playback.c cyg-profile.c cyg-profile.h
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) $(FFMPEG_LDFLAGS) $(FUNC_TRACE_FLAG) -o $@ $^
	$(STRIP) $@

mediainfo: mediainfo.c cyg-profile.c cyg-profile.h
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) $(FFMPEG_LDFLAGS) $(FUNC_TRACE_FLAG) -o $@ $^
	$(STRIP) $@

audio_api: audio_api.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

avplayer: avplayer.c cyg-profile.c cyg-profile.h dsp_api.c dsp_api.h
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) $(FFMPEG_LDFLAGS) $(FUNC_TRACE_FLAG) -DDEBUG -o $@ $^ -lpthread
	$(STRIP) $@

dsp_mon: dsp_mon.c
	@echo "Compile $@" 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(STRIP) $@


clean:
	$(RM) $(ProgramName) *.o
 
