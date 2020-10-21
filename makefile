CC=gcc
CFLAGS=-I
AR=ar
OUTPUT=libzr.a

lib: clean obj
	$(AR) -r lib/$(OUTPUT) o/scheduler.o o/channel.o o/list.o

obj: scheduler.c channel.c list.c
	$(CC) -c scheduler.c -o o/scheduler.o -w
	$(CC) -c channel.c -o o/channel.o -w
	$(CC) -c list.c -o o/list.o -w

clean:
	rm o/*
	rm lib/*
