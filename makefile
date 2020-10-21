CC=gcc
CFLAGS=-I
AR=ar
OUTPUT=libzr.a

lib: obj
	$(AR) -r lib/$(OUTPUT) o/scheduler.o o/channel.o o/list.o

obj: src/scheduler.c src/channel.c src/list.c
	$(CC) -c src/scheduler.c -o o/scheduler.o -w
	$(CC) -c src/channel.c -o o/channel.o -w
	$(CC) -c src/list.c -o o/list.o -w

clean:
	rm o/*
	rm lib/*
