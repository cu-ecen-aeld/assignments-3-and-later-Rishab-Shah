CC = $(CROSS_COMPILE)gcc
CROSS_COMPILE =
CFFLAGS = -g -Wall
LDFLAGS = -lrt -pthread

all : aesdsocket.o
	$(CC) $(CFFLAGS) $(LDFLAGS) -o aesdsocket aesdsocket.o

aesdsocket.o  : aesdsocket.c
	$(CC) $(CFFLAGS) $(LDFLAGS) -c aesdsocket.c

clean :
	rm -f *.o aesdsocket *.elf *.map
