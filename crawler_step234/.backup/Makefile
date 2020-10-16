CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: 				crawler_step2 crawler_with_queue crawler_with_hash

crawler_step2:		crawler_step2.c
					gcc $(CFLAGS) crawler_step2.c $(LIBS) -o $@

crawler_with_queue:	crawler_with_queue.c
					gcc $(CFLAGS) crawler_with_queue.c $(LIBS) -o $@

crawler_with_hash:	crawler_with_hash.c
					gcc $(CFLAGS) crawler_with_hash.c $(LIBS) -o $@

clean:
				    rm -f crawler_step2 crawler_with_queue crawler with hash
