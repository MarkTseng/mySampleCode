# Makefile template for shared library

UNWIND_DIR=$(shell pwd)/libunwind/build
CC = arm-linux-gnueabihf-g++ # C compiler
CFLAGS = -I${UNWIND_DIR}/include -Iinclude -fPIC -Wall -Wextra -g -fstack-protector -U_FORTIFY_SOURCE# C flags
LDFLAGS = -shared  -fPIC
#LDFLAGS += -L${UNWIND_DIR}/lib
RM = rm -f  # rm command
TARGET_LIB = libmguard.so # target lib

# source files
SRCS = src/mguard.c \
       src/compat.c \
       src/env.c

OBJS = $(SRCS:.c=.o)

UNWIND_LIB_A = ${UNWIND_DIR}/lib/libunwind-arm.a \
               ${UNWIND_DIR}/lib/libunwind.a \

.PHONY: all clean
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	#$(CC) ${LDFLAGS} -o $@ $^ ${UNWIND_LIB_A} -ldl -lpthread
	$(CC) ${LDFLAGS} -o $@ $^ ${UNWIND_LIB_A} -ldl -lpthread

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@

-include $(SRCS:.c=.d)

clean:
	-@${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
