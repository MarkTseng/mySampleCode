TARGET = hello bin2hexheader hexheader2bin

all: $(TARGET)
	@echo "compile $(TARGET) done"

bin2hexheader:
	gcc -o bin2hexheader bin2hexheader.c 

hexheader2bin:
	gcc -o hexheader2bin hexheader2bin.c

hello:
	gcc -o hello hello.c

clean:
	rm $(TARGET) 
