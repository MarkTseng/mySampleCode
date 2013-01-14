CROSS_COMPILE=mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
CFLAGS = -Wall -O2
#DEBUG_FLAG = -DDEBUG
DEBUG_FLAG = 
RM = rm -f
SRC = iommap_mmap.c
OBJ = iommap_mmap.o
INC = ./
ProgramName = iommap_mmap

all: $(OBJ)
	@echo "Compile $(ProgramName)" 
	$(CC) $(CFLAGS) -o $(ProgramName) $^ 
	$(STRIP) $(ProgramName) 
	
%.o: %.c 
	@echo "Compile $<" 
	$(CC) $(CFLAGS) $(DEBUG_FLAG) -c -o $@ $< -I$(INC) 

clean:
	$(RM) $(ProgramName) *.o
 
