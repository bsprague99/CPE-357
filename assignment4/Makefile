CC      = gcc
CFLAGS  = -g -Wall 
LD      = gcc
LDFLAGS = -lm

all: mytar

mytar: mytar.o
	$(LD) $(LDFLAGS) mytar.o -o mytar

mytar.o: mytar.c mytar.h
	$(CC) $(CFLAGS) -c mytar.c

clean:
	@rm *.o 
