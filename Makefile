CC = gcc
CFLAGS = -MMD -g -std=gnu17 -fdiagnostics-color=always
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
	$(CC) $^ $(CFLAGS) -o $@

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

asan:
	$(MAKE) CFLAGS="$(CFLAGS) -fsanitize=address -fno-omit-frame-pointer"
 
clean:
	rm -f $(APP) $(OBJ) $(DEP)

.PHONY: all clean asan

.DELETE_ON_ERROR:

-include $(DEP)
