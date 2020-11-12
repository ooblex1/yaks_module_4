CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:     	testH testQ

%.o:     	%.c %.h
			gcc $(CFLAGS) -c $<

testH: 	    
			gcc $(CFLAGS) testH.o $(LIBS) -o $@ -pthread

testQ: 	    
			gcc $(CFLAGS) testQ.o $(LIBS) -o testQ -pthread

clean:
			rm -f queue.o lhash.o hash.o lqueue.o testH testQ
