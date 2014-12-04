all:
	gcc -g -o gdb_test gdb_test.c  
	gcc -g -Wl,-Map,mapfile.map -o mapfile  mapfile.c

clean:
	rm -rf gdb_test mapfile
