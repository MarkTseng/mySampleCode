all:
	arm-linux-gnueabihf-gcc poll.c -o poll
	arm-linux-gnueabihf-gcc epoll.c -o epoll
	cp poll epoll ~/nfs/ ;sync

clean:
	rm -rf poll epoll
