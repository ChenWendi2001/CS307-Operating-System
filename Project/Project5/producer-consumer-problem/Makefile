# makefile for thread pool
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: pcp.o
	$(CC) $(CFLAGS) -o pcp pcp.o $(PTHREADS)

pcp.o: pcp.c
	$(CC) $(CFLAGS) -c pcp.c $(PTHREADS)


clean:
	rm -rf *.o
	rm -rf pcp

