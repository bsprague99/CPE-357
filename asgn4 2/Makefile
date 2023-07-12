CC      = gcc
CFLAGS  = -g -std=c99
LD      = gcc
LDFLAGS = -lm

all: mytar

mytar: mytar.o
	$(LD) $(LDFLAGS) mytar.o -o mytar

mytar.o: mytar.c mytar.h
	$(CC) $(CFLAGS) -c mytar.c

clean:
	@rm *.o 
