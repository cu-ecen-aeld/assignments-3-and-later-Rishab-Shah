CC = $(CROSS_COMPILE)gcc
CROSS_COMPILE =
CFFLAGS = -g -Wall -Werror

all : aesdsocket.o
	$(CC) $(CFFLAGS) -o aesdsocket aesdsocket.o

aesdsocket.o  : aesdsocket.c
	$(CC) $(CFFLAGS) -c aesdsocket.c

clean :
	rm -f *.o aesdsocket *.elf *.map
