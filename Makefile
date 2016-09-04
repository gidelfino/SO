CC = gcc
CFLAGS = -Wall -ansi -O2 -g

all: ep1sh ep1

ep1sh: ep1sh.c
	$(CC) $(CFLAGS) ep1sh.c -o ep1sh -lreadline  
ep1: ep1.c utility.c utility.h maxheap.c maxheap.h minheap.c minheap.h scheduler.c scheduler.h
	$(CC) $(CFLAGS) ep1.c utility.c maxheap.c minheap.c scheduler.c -o ep1 -lpthread

clean:
	rm -f ep1sh ep1