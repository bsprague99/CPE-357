SHELL = /bin/bash

CC = gcc

CFLAGS = -Wall -g -pedantic -ggdb -Wextra -o mush2

LINK =  -L ~pn-cs357/Given/Mush/libmush/lib64 -o mush2 

LINK1 = -I ~pn-cs357/Given/Mush/libmush/include

LDFLAGS = -Wall -g -pedantic

mush2: mush2.o
	$(CC) $(LDLAGS) $(LINK) mush2.o -lmush

mush2.o: mush2.c mush2.h
	$(CC) $(LDLAGS) $(LINK1) -c mush2.c mush2.h

clean:
	-rm *.o *~
