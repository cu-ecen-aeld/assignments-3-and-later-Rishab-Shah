CROSS_COMPILE =
CFFLAGS = -g -Wall

all : writer.o
	$(CROSS_COMPILE)gcc $(CFFLAGS) -o writer writer.o

writer.o  : writer.c
	$(CROSS_COMPILE)gcc $(CFFLAGS) -c writer.c

clean :
	rm -f *.o writer *.elf *.map
