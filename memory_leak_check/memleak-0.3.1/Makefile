CFLAGS+=-ansi -DWITH_DBG_STRDUP
OBJ=memleak.o example.o
BIN=example
CC=arm-linux-gnueabihf-gcc

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	$(RM) $(OBJ) $(BIN)
