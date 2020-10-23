CC=gcc
CFLAGS=-Isrc -g
AR=ar
OUTPUT=libzr.a
EXAM_DIR=example
BIN_DAR=bin

lib: obj
	$(AR) -r lib/$(OUTPUT) o/scheduler.o o/channel.o o/list.o

example: lib $(EXAM_DIR)/example0.c $(EXAM_DIR)/example1.c $(EXAM_DIR)/example2.c $(EXAM_DIR)/example3.c $(EXAM_DIR)/example4.c
	$(CC) $(EXAM_DIR)/example0.c -Llib -lzr -o $(BIN_DIR)/example0 $(CFLAGS) 
	$(CC) $(EXAM_DIR)/example1.c -Llib -lzr -o $(BIN_DIR)/example1 $(CFLAGS)
	$(CC) $(EXAM_DIR)/example2.c -Llib -lzr -o $(BIN_DIR)/example2 $(CFLAGS)
	$(CC) $(EXAM_DIR)/example3.c -Llib -lzr -o $(BIN_DIR)/example3 $(CFLAGS)
	$(CC) $(EXAM_DIR)/example4.c -Llib -lzr -o $(BIN_DIR)/example4 $(CFLAGS)

obj: src/scheduler.c src/channel.c src/list.c
	$(CC) -c src/scheduler.c -o o/scheduler.o -w
	$(CC) -c src/channel.c -o o/channel.o -w
	$(CC) -c src/list.c -o o/list.o -w

clean:
	rm o/*
	rm lib/*
