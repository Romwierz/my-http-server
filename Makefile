CC = gcc
CFLAGS = -Wall -g -std=gnu17 -fdiagnostics-color=always

all: my-http

my-http: my-http.c
	$(CC) $(CFLAGS) my-http.c -o my-http

clean:
	rm -f my-http

.PHONY: all clean

.DELETE_ON_ERROR:
