CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:     	lhtest1 lhtest2 lhtest3 lhtest4

%.o:     	%.c %.h
			gcc $(CFLAGS) -c $<

lhtest1: 	lhtest1.o lhash.o queue.o hash.o
			gcc $(CFLAGS) $^ -o $@ -lpthread

lhtest2: 	lhtest2.o lhash.o queue.o hash.o
			gcc $(CFLAGS) $^ -o $@ -lpthread

lhtest3: 	lhtest3.o lhash.o queue.o hash.o
			gcc $(CFLAGS) $^ -o $@ -lpthread

lhtest4: 	lhtest4.o lhash.o queue.o hash.o
			gcc $(CFLAGS) $^ -o $@ -lpthread

clean:
			rm -f *.o lhtest1 lhtest2 lhtest3 lhtest4
