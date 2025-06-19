CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-parameter -g -std=gnu17 -fdiagnostics-color=always

APP := my-http

all: $(APP)

$(APP): main.o utils.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -f $(APP) *.o

.PHONY: all clean

.DELETE_ON_ERROR:
