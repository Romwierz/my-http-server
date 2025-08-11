CC = gcc
STD := $(shell $(CC) -std=gnu17 -E - < /dev/null > /dev/null 2>&1 && echo gnu17 || echo gnu11)

CFLAGS = -MMD -g -std=$(STD) -fdiagnostics-color=always
# Warnings
CFLAGS += -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs
ifeq ($(CC),gcc)
    CFLAGS += -Wjump-misses-init -Wlogical-op
endif

APP := my-http
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
DEP := $(OBJ:.o=.d)

all: $(APP)

$(APP): $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@ -lws2_32

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

asan:
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address -fno-omit-frame-pointer"

clean:
	rm -f $(APP) $(OBJ) $(DEP)

.PHONY: all clean asan
.DELETE_ON_ERROR:

-include $(DEP)
