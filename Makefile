all: main

main: main.c
	gcc -fopenmp $^ -o $@

clean:
	rm main
