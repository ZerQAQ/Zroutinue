CC=gcc
CFLAGS=-I
AR=ar
OUTPUT=libzr.a

lib: obj
	$(AR) -r lib/$(OUTPUT) o/scheduler.o o/channel.o o/list.o

example: lib example0.c example1.c example2.c example3.c example4.c
	$(CC) example0.c -Llib -lzr -o example0
	$(CC) example1.c -Llib -lzr -o example1
	$(CC) example2.c -Llib -lzr -o example2
	$(CC) example3.c -Llib -lzr -o example3
	$(CC) example4.c -Llib -lzr -o example4

obj: src/scheduler.c src/channel.c src/list.c
	$(CC) -c src/scheduler.c -o o/scheduler.o -w
	$(CC) -c src/channel.c -o o/channel.o -w
	$(CC) -c src/list.c -o o/list.o -w

clean:
	rm o/*
	rm lib/*
