SHELL = /bin/bash

CC = gcc

CFLAGS = -ansi -Wall -g -pedantic -ggdb -Wextra -o fw

LDFLAGS = -ansi -Wall -g -pedantic -ggdb

fw: fw.c fw.h
	$(CC) $(CFLAGS) fw.c

clean:
	-rm *.o *~
