
test:test.c mdb 
	gcc -g test.c -o test

mdb:mdb.c
	gcc -Wall -ggdb mdb.c -o mdb -lbfd -lopcodes -lreadline

clean:
	rm -f mdb test 

all:test 
