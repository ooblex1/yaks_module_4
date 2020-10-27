CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:			piotest1 index_test

piotest1:	piotest1.c
					gcc $(CFLAGS) piotest1.c $(LIBS) -o $@

index_test:	index_test.c
					gcc $(CFLAGS) index_test.c $(LIBS) -o $@

run:
					./index_test.sh

clean:
					rm -f piotest1 newindex index_test
