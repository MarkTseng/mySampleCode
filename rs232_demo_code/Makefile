CROSS_COMPILE=mips-linux-gnu-
CC = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
CFLAGS = -Wall 
#DEBUG_FLAG = -DDEBUG
DEBUG_FLAG = 
RM = rm -f
SRC = sdebug.c sdebug_api.c 
OBJ = sdebug.o sdebug_api.o
INC = ./
ProgramName = sdebug

all: $(OBJ)
	@echo "all"
	$(CC) $(CFLAGS) -o $(ProgramName) $^ 
	$(STRIP) $(ProgramName)
	$(CC) -o uiomem3 uiomem3.c
	
%.o: %.c 
	@echo "Compile $<"
	$(CC) $(CFLAGS) $(DEBUG_FLAG) -c -o $@ $< -I$(INC)

clean:
	$(RM) $(ProgramName) *.o
 
