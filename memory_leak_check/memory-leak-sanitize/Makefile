
all:
	g++ -ggdb -fsanitize=address -fno-omit-frame-pointer -static-libstdc++ -static-libasan -lrt memory_leak.cpp -o memory_leak

clean:
	rm -rf memory_leak
