CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o

all:	$(OFILES)
			ar cr ~/engs50/tse/lib/libutils.a $(OFILES)

%.o:	%.c %.h
			gcc $(CFLAGS) -c $<

clean:	
			rm -f *.o
