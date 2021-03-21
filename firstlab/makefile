CC = gcc
CFLAGS = -g -Wall

default: firstLab

firstLab: main.o proc.o split.o
	$(CC) $(CFLAGS) -o firstLab main.o proc.o split.o

main.o: main.c proc.h split.h
	$(CC) $(CFLAGS) -c main.c

proc.o: proc.c proc.h
	$(CC) $(CFLAGS) -c proc.c

split.o: split.c split.h
	$(CC) $(CFLAGS) -c split.c

clean:
	rm -f firstLab *.o
