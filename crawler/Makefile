CFLAGS=-Wall -pedantic -std=c11 -I~/engs50/tse/utils/*.h -L~/engs50/tse/lib -g
LIBS=-lutils -lcurl

crawler:
					gcc $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
				rm -f crawler.c
