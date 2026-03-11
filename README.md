# Candy Collection

Solution to the compute assignment. Implements both sequential and parallel (OpenMP) versions.

## Build

- Linux: `make`
- macOS: Install `libomp` and `gcc@15` with `brew`, then `make CC=gcc-15`

## Run

Two binaries: `main_sequential` and `main_parallel`

Uses `input.txt` with format:

```
<num_homes>
<threshold>
<candy_count_1>
<candy_count_2>
...
```

Last candy count should end with a newline.

Example: `./main_parallel`
