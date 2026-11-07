CC ?= gcc
CFLAGS ?= -std=c99 -Wall -Wextra -O2 -Iinclude
LDFLAGS ?= -lcurl

all: libnoxaeapi.a

libnoxaeapi.a: src/noxaeapi.o
	ar rcs $@ $^

src/noxaeapi.o: src/noxaeapi.c include/noxaeapi.h
	$(CC) $(CFLAGS) -c $< -o $@

example: examples/main.c libnoxaeapi.a
	$(CC) $(CFLAGS) $< -L. -lnoxaeapi $(LDFLAGS) -o example_app

clean:
	rm -f src/*.o *.a example_app

.PHONY: all clean example
