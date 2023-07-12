SHELL = /bin/bash

CC = gcc

CFLAGS = -Wall -g -pedantic -Wextra

LDFLAGS = -Wall -g -pedantic

all: forkit tryit

forkit: forkit.o 
	$(CC) $(CCFLAGS) -lm -o forkit forkit.o

forkit.o: forkit.c
	$(CC) -c $(CCFLAGS) forkit.c

tryit: tryit.o
	$(CC) $(CCFLAGS) -lm -o tryit tryit.o

tryit.o: tryit.c
	$(CC) -c $(CCFLAGS) tryit.c

clean:
	-rm *.o *~