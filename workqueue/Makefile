CC=arm-linux-gnueabihf-gcc
CFLAGS=-c -Wall
LDFLAGS= -pthread
SOURCES=jobqueue.c thpool.c workqueue.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=workqueue

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
