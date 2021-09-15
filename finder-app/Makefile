CC = $(CROSS_COMPILE)gcc
CROSS_COMPILE =
CFFLAGS = -g -Wall

all : writer.o
	$(CC) $(CFFLAGS) -o writer writer.o

writer.o  : writer.c
	$(CC) $(CFFLAGS) -c writer.c

clean :
	rm -f *.o writer *.elf *.map
