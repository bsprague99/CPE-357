SHELL = /bin/bash

CC = gcc

CFLAGS = -Wall -g -pedantic -Wextra

LDFLAGS = -Wall -g -pedantic

all: hencode hdecode

hencode: hencode.o 
	$(CC) $(CCFLAGS) -lm -o hencode hencode.o

hencode.o: hencode.c hencode.h
	$(CC) -c $(CCFLAGS) hencode.c

hdecode: hdecode.o
	$(CC) $(CCFLAGS) -lm -o hdecode hdecode.o

hdecode.o: hdecode.c hdecode.h
	$(CC) -c $(CCFLAGS) hdecode.c

clean:
	-rm *.o *~







# SHELL = /bin/bash

# CC = gcc

# CFLAGS = -ansi -Wall -g -pedantic -Wextra

# LDFLAGS = -ansi -Wall -g -pedantic

# all: hencode hdecode

# hencode: hencode.o 
# 	$(CC) $(CCFLAGS) -o hencode hencode.o

# hencode.o: hencode.c htable.h
# 	$(CC) -c $(CCFLAGS) hencode.c

# hdecode: hdecode.o
# 	$(CC) $(CCFLAGS) -o hdecode hdecode.o

# hdecode.o: hdecode.c htable.h
# 	$(CC) -c $(CCFLAGS) hdecode.c

# clean:
# 	-rm *.o *~
