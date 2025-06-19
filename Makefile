CC = gcc
CFLAGS = -MMD -Wall -Wextra -Wno-unused-parameter -g -std=gnu17 -fdiagnostics-color=always

APP := my-http
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

all: $(APP)

$(APP): $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

clean:
	rm -f $(APP) $(OBJ) $(DEP)

.PHONY: all clean

.DELETE_ON_ERROR:

-include $(DEP)
