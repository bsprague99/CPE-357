SHELL = /bin/bash

CC = gcc

CFLAGS = -Wall -g -pedantic -ggdb -Wextra -o mytalk

LINK =  -L ~pn-cs357/Given/Talk/lib64 -o mytalk 

LINK1 = -I ~pn-cs357/Given/Talk/

LDFLAGS = -Wall -g -pedantic

mytalk: mytalk.o
	$(CC) $(LDLAGS) $(LINK) mytalk.o -ltalk -l ncurses

mytalk.o: mytalk.c
	$(CC) $(LDLAGS) $(LINK1) -c mytalk.c

clean:
	-rm *.o *~
