# Compute Assignment Solution

Solution to the compute assignment. Implements both sequential and parallel (OpenMP) versions.

Time complexities:

- Sequential: O(n)
- Parallel: O(n/p + p + logn) where p is the number of threads

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

## Test

Unit tests in Zig (0.15.2). Run with `zig build test` (macOS).

On Linux, I cannot find a way to link OpenMP libraries yet.
