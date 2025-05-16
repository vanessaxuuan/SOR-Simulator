CC = gcc
CFLAGS = -Wall -Iinclude

SRC = src/main.c src/sor.c src/heapsort.c
OUT = bin/sor_simulator

all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -rf bin
