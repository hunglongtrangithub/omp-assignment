CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror \
         -Wcast-qual -Wnull-dereference \
         -std=c11

all: main_parallel main_sequential

main_parallel: main.c
	$(CC) -fopenmp $(CFLAGS) -DUSE_PARALLEL $^ -o $@

main_sequential: main.c
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -f main_parallel main_sequential
