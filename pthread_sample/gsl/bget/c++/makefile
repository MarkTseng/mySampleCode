
CC = cc
CPP= g++
COPTS = -O

bget:	bget.cpp bget.h
	$(CPP) $(COPTS) bget.cpp bget-test.c -o bget -lm
	rm -f core

lint:
	lint bget.c -lm

clean:
	rm -f bget core *.o *.bak *.exe *.obj *.swp
