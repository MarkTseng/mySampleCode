LIB=/home/mark/tmp/ncurses-6.0/LIBNCURSES/lib
INC=/home/mark/tmp/ncurses-6.0/LIBNCURSES/include
all:
	arm-linux-gnueabihf-gcc -o ftw_test ftw_test.c -L$(LIB) -lmenu -lpanel -lncurses -I$(INC)
	arm-linux-gnueabihf-gcc -o test test.c -L$(LIB) -lmenu -lpanel -lncurses -I$(INC)
	cp ftw_test test ~/nfs/;sync
clean:
	rm -rf ftw_test test
