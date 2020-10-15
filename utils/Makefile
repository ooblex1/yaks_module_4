CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o

all:	$(OFILES)
			ar cr ~/engs50/tse/lib/libutils.a $(OFILES)

%.o:	%.c %.h
			gcc $(CFLAGS) -c $<

htest1:	htest1.o hash.o queue.o
				gcc $(CFLAGS) $^ -o $@

htest2:	htest2.o hash.o queue.o
				gcc $(CFLAGS) $^ -o $@

qtest1:	qtest1.o queue.o
				gcc $(CFLAGS) $^ -o $@

qtest2:	qtest2.o queue.o
				gcc $(CFLAGS) $^ -o $@

qtest3:	qtest3.o queue.o
				gcc $(CFLAGS) $^ -o $@

clean:	
				rm -f *.o htest1 htest2 qtest1 qtest2 qtest3
