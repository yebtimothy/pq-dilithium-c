CC ?= gcc
CFLAGS += -O3 -Wall -Wextra -march=native -Iinclude
SOURCES = src/fips202.c src/reduce.c src/ntt.c src/poly.c src/polyvec.c src/randombytes.c src/packing.c src/commit.c src/sign.c
OBJECTS = $(SOURCES:.c=.o)
LIBRARY = libpqdilithium.a
TEST = test_dilithium

all: $(LIBRARY) $(TEST)

$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

$(TEST): tests/test_dilithium.c $(LIBRARY)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(LIBRARY) $(TEST)

.PHONY: all clean
