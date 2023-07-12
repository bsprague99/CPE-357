CC      = gcc
CFLAGS  = -g -Wall
LD      = gcc
LDFLAGS = -lm

all: timeit

timeit: timeit.o
	$(LD) $(LDFLAGS) timeit.o -o timeit

timeit.o: timeit.c
	$(CC) $(CFLAGS) -c timeit.c

clean:
	@rm *.o 

