#include ${IPCM_TOP}/config.mk

# output compile message
V?=0

# shared library or binary and TARGET name
SHARED_LIS?=0
ifeq ($(SHARED_LIS),0)
    TARGET = hellomake
else
    LDFLAGS += -shared  
    TARGET = libhellomake.so
endif

# Makefile template for shared library
CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
AR = $(CROSS_COMPILE)ar

# cmd
RM = rm -rf  # rm command

CFLAGS += -fPIC -Wall -Wextra -O2 -g -Iinc 
CXXFLAGS += -DMYDEF2
LDFLAGS +=  

SRCS = $(wildcard ./src/*.c) # source files
OBJS = $(subst src,obj,$(SRCS:%.c=%.o))
OUT_DIR=out
LIB_DIR=$(OUT_DIR)/lib
BIN_DIR=$(OUT_DIR)/bin

.PHONY: all clean

all: prepare ${TARGET}

obj/%.o : src/%.c
	@mkdir -p obj
ifeq ($(V),0)
	@echo "  CC  $<"
	@$(CC) $(CFLAGS) -c $< -o $@
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

$(TARGET): $(OBJS) 
	@mkdir -p $(LIB_DIR) $(BIN_DIR)
ifeq ($(SHARED_LIS),0)
	$(CC) ${LDFLAGS} -o $(BIN_DIR)/$@ $^
else
	$(CC) ${LDFLAGS} -o $(LIB_DIR)/$@ $^
endif


clean:
	${RM} ${OBJS} out/

prepare:
	@echo
	@echo "========================================================"
	@echo
	@echo "  PWD = $(shell pwd)"
	@echo "  CC = $(CC)"
	@echo "  STRIP = $(STRIP)"
	@echo "  AR = $(AR)"
	@echo "  IPCM_TOP = $(TOOL_TOP_DIR)"
	@echo "  CHIP_ID = $(CHIP_ID)"
	@echo "  SRCS = $(SRCS)"
	@echo "  OBJS = $(OBJS)"
	@echo "========================================================"
	@echo

