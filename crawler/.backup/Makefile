CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all: 				crawler

crawler:			crawler.c
					gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
				    rm -f crawler
