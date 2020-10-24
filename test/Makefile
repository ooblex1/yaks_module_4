CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:			piotest1

piotest1:	piotest1.c
					gcc $(CFLAGS) piotest1.c $(LIBS) -o $@

clean:
					rm -f piotest1
