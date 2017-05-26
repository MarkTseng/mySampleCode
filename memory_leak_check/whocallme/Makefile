all:
	gcc -o bfd bfd.c -lbfd
	gcc -o bfd-dumpfun bfd-dumpfun.c -lbfd
	gcc -o bfd-symbol bfd-symbol.c -lbfd
	gcc -o libelf-howto libelf-howto.c -lelf
	arm-linux-gnueabihf-gcc -rdynamic -o symbols symbols.c -ldl 
clean:
	rm -rf *.o bfd bfd-dumpfun bfd-symbol libelf-howto symbols
