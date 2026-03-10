CC = gcc
CFLAGS = -fopenmp -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Werror \
         -Wcast-qual -Wnull-dereference \
         -std=c11

all: main
main: main.c
	$(CC) $(CFLAGS) $^ -o $@
clean:
	rm main
