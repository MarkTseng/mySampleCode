include ${IPCM_TOP}/config.mk

# output compile message
V?=0

# shared library or binary and TARGET name
SHARED_LIS?=0
ifeq ($(SHARED_LIS),0)
    TARGET = hellomake
    PACKAGENAME = $(addsuffix .tar.xz,$(TARGET))
else
    LDFLAGS += -shared  
    TARGET = libhellomake.so
    PACKAGENAME = $(addsuffix .tar.xz,$(basename $(TARGET)))
endif

# Makefile template for shared library
CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
AR = $(CROSS_COMPILE)ar

# command define
RM = rm -rf  	
MKDIR = mkdir -p
TAR = tar cfJ   
CP = cp -a  	

# CFLAGS and LDFLAGS
CFLAGS += -fPIC -Wall -Wextra -O2 -g -Iinc 
CXXFLAGS += -DMYDEF2
LDFLAGS +=  

SRCS = $(wildcard ./src/*.c) # source files
OBJS = $(subst src,obj,$(SRCS:%.c=%.o))
H_SRCS = $(wildcard ./inc/*.h)
OUT_DIR=out
LIB_DIR=$(OUT_DIR)/lib
BIN_DIR=$(OUT_DIR)/bin

.PHONY: all clean

all: prepare ${TARGET} package

obj/%.o : src/%.c $(H_SRCS)
	@mkdir -p obj
ifeq ($(V),0)
	@echo "  CC  $<"
	@$(CC) $(CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

$(TARGET): $(OBJS)
	$(MKDIR) $(LIB_DIR) $(BIN_DIR)
ifeq ($(SHARED_LIS),0)
	$(CC) ${LDFLAGS} -o $(BIN_DIR)/$@ $^
else
	$(CC) ${LDFLAGS} -o $(LIB_DIR)/$@ $^
endif

clean:
	${RM} ${OBJS} out/ package

package:
	@echo "pack $(PACKAGENAME)"
ifeq ($(SHARED_LIS),0)
	@$(MKDIR) package/$(TARGET) 
	@$(CP) out/* package/$(TARGET)
	@cd package && $(TAR) $(PACKAGENAME) $(TARGET)
else
	@$(MKDIR) package/$(basename $(TARGET))
	@$(CP) out/* package/$(basename $(TARGET))
	@$(CP) inc package/$(basename $(TARGET))
	@cd package && $(TAR) $(PACKAGENAME) $(basename $(TARGET))
endif

prepare:
	@echo
	@echo "========================================================"
	@echo "  PWD = $(shell pwd)"
	@echo "  CC = $(CC)"
	@echo "  STRIP = $(STRIP)"
	@echo "  AR = $(AR)"
	@echo "  IPCM_TOP = $(IPCM_TOP)"
	@echo "  PACKAGE NAME = $(PACKAGENAME)"
	@echo "========================================================"
	@echo

