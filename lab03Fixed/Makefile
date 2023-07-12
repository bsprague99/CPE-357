SHELL = /bin/bash

CC = gcc

OPTI = -O3

MEM = -valgrin

CFLAGS =  -Wall -g -pedantic -ggdb -Wextra -o htable

LDFLAGS =  -Wall -g -pedantic -ggdb

htable: htable.c htable.h
	$(CC) $(CFLAGS) $(OPTI) htable.c

clean:
	-rm *.o *~
