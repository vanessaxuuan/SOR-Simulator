CC = gcc
CFLAGS = -Wall -Iinclude -I/usr/local/opt/curl/include -I/usr/local/opt/json-c/include
LDFLAGS = -L/usr/local/opt/curl/lib -L/usr/local/opt/json-c/lib
LIBS = -lcurl -ljson-c -pthread

SRC = src/main.c src/sor.c src/heapsort.c src/exchange-simulator.c
OUT = bin/sor_simulator

all:
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS) $(LIBS)

clean:
	rm -rf bin
