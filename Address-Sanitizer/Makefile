all:
	arm-linux-gnueabihf-gcc -fsanitize=address -O1 -fno-omit-frame-pointer -g -o asani_arm asan.c
	gcc -fsanitize=address -O1 -fno-omit-frame-pointer -g -o asan asan.c
