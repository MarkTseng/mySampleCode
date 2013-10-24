all:
	gcc -c unix_socket_utils.c unix_socket_svr.c unix_socket_cli.c
	gcc -o unix_socket_svr unix_socket_svr.o unix_socket_utils.o
	gcc -o unix_socket_cli unix_socket_cli.o unix_socket_utils.o
	mips-linux-gcc -c unix_socket_utils.c unix_socket_svr.c unix_socket_cli.c
	mips-linux-gcc -o unix_socket_svr_mips unix_socket_svr.o unix_socket_utils.o
	mips-linux-gcc -o unix_socket_cli_mips unix_socket_cli.o unix_socket_utils.o

clean:
	rm -rf unix_socket_svr unix_socket_cli *.o
