CC ?= gcc
CFLAGS ?= -std=c89 -pedantic -Wall -Wextra -g3 -O0
LDFLAGS ?=

all: unPACK

unPACK: unPACK.c
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

clean:
	rm -f unPACK unPACK.exe *.o


.PHONY: clean
