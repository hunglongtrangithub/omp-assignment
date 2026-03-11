CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror \
         -Wcast-qual -Wnull-dereference \
         -fopenmp \
         -std=c11

all: main_parallel main_sequential

main_parallel: main.c
	$(CC) $(CFLAGS) -DUSE_PARALLEL $^ -o $@

main_sequential: main.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f main_parallel main_sequential

.PHONY: all clean
