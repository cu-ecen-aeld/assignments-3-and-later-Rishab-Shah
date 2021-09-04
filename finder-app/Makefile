CROSS_COMPILE = gcc
CFFLAGS = -g -Wall

run : writer.o
	$(CROSS_COMPILE) $(CFFLAGS) -o writer writer.o

writer.o  : writer.c
	$(CROSS_COMPILE) $(CFFLAGS) -c writer.c

clean :
	rm -f *.o writer *.elf *.map
