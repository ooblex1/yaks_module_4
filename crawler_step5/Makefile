CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:										crawler_5_with_hash

crawler_5_with_hash:		crawler_5_with_hash.c
												gcc $(CFLAGS) crawler_5_with_hash.c $(LIBS) -o $@

clean:
				    						rm -f crawler_5_with_hash
