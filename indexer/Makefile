CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: indexer indexer_step2 indexer_step34

indexer:    	indexer.c    
				gcc $(CFLAGS) $^ $(LIBS) -o $@

indexer_step2:    	indexer_step2.c    
				gcc $(CFLAGS) $^ $(LIBS) -o $@

indexer_step34:    	indexer_step34.c    
				gcc $(CFLAGS) $^ $(LIBS) -o $@

clean:
				rm -f indexer indexer_step2 indexer_step34