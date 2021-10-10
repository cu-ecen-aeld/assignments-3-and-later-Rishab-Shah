CC = $(CROSS_COMPILE)gcc
CROSS_COMPILE =
CFFLAGS = -Wall -Werror -g
LDFLAGS = -pthread -lrt

all : aesdsocket.o
	$(CC) $(CFFLAGS) $(LDFLAGS) -o aesdsocket aesdsocket.o

aesdsocket.o  : aesdsocket.c
	$(CC) $(CFFLAGS) $(LDFLAGS) -c aesdsocket.c

clean :
	rm -f *.o aesdsocket *.elf *.map
