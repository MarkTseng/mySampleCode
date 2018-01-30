
CROSS_COMPILE=arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
all:
	$(CC) process_on_fixed_CPU.c -pthread -o process_on_fixed_CPU
	$(CC) pthread_on_fixed_CPU.c -pthread -o pthread_on_fixed_CPU
	$(CC) busyloop.c -o busyloop

clean:
	rm -rf process_on_fixed_CPU pthread_on_fixed_CPU busyloop
