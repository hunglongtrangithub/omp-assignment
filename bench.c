#include "candy.c"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CANDY_COUNT 1000
#define N_RUNS 8

static double now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static uint8_t verify(const uint16_t *candy_counts, size_t home_count,
                      size_t par_start, size_t par_end, size_t seq_start,
                      size_t seq_end) {
  uint64_t par_sum = 0;
  if (par_start <= par_end && par_end < home_count)
    for (size_t i = par_start; i <= par_end; i++)
      par_sum += candy_counts[i];

  uint64_t seq_sum = 0;
  if (seq_start <= seq_end && seq_end < home_count)
    for (size_t i = seq_start; i <= seq_end; i++)
      seq_sum += candy_counts[i];

  if (par_sum != seq_sum) {
    fprintf(stderr,
            "  [FAIL] sum mismatch: parallel=%lu sequential=%lu\n"
            "         parallel   range [%zu, %zu]\n"
            "         sequential range [%zu, %zu]\n",
            par_sum, seq_sum, par_start, par_end, seq_start, seq_end);
    return 0;
  }
  return 1;
}

static void bench(const uint16_t *candy_counts, size_t home_count) {
  printf("  %-8s %-7s  %-14s %-14s %-10s %-8s\n", "homes", "thresh",
         "sequential", "parallel", "speedup", "result");
  printf(
      "  %s\n",
      "----------------------------------------------------------------------");

  for (int r = 0; r < N_RUNS; r++) {
    uint16_t thresh = (uint16_t)(rand() % (MAX_CANDY_COUNT + 1));

    size_t seq_start, seq_end;
    double t;

    t = now_ns();
    findBestHomeRange(candy_counts, home_count, thresh, &seq_start, &seq_end);
    double seq_ns = now_ns() - t;

    size_t par_start, par_end;
    t = now_ns();
    findBestHomeRangeParallel(candy_counts, home_count, thresh, &par_start,
                              &par_end);
    double par_ns = now_ns() - t;

    uint64_t seq_sum = 0;
    if (seq_start <= seq_end && seq_end < home_count)
      for (size_t i = seq_start; i <= seq_end; i++)
        seq_sum += candy_counts[i];

    uint8_t ok = verify(candy_counts, home_count, par_start, par_end, seq_start,
                        seq_end);

    printf("  %-8zu %-7u  %8.3f ms     %8.3f ms     %6.2fx     sum=%-6lu  %s\n",
           home_count, thresh, seq_ns / 1e6, par_ns / 1e6,
           seq_ns / (par_ns > 0.0 ? par_ns : 1e-9), seq_sum,
           ok ? "[OK]" : "[FAIL]");
  }
}

int main(int argc, char *argv[]) {

  for (size_t home_count = 1000; home_count <= 100000000; home_count *= 10) {
    uint16_t *candy_counts = malloc(home_count * sizeof(uint16_t));
    if (candy_counts == NULL) {
      fprintf(stderr, "error: allocation failed for %zu homes\n", home_count);
      return 1;
    }

    srand(42);
    for (size_t i = 0; i < home_count; i++)
      candy_counts[i] = (uint16_t)(rand() % (MAX_CANDY_COUNT + 1));

    printf("Benchmark: %zu homes, candy counts in [0, 1000], %d random "
           "thresholds\n\n",
           home_count, N_RUNS);

    bench(candy_counts, home_count);

    free(candy_counts);
  }

  return 0;
}
