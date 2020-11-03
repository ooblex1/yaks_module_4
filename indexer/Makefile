CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: indexer

indexer:    		indexer.c    
					gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
					rm -f indexer
