#include <stdint.h>
#include <stdio.h>

void findBestHomeRange(const uint16_t *const candy_counts,
                       const size_t home_count, const uint16_t candies_thresh,
                       size_t *const home_start, size_t *const home_end) {
  if (home_count == 0 || candies_thresh == 0) {
    *home_start = home_count;
    *home_end = home_count;
    return;
  }

  size_t best_start = home_count;
  size_t best_end = home_count;

  size_t start = 0;
  uint32_t curr_total = 0;
  uint32_t best_total = 0;
  for (size_t end = 0; end < home_count; end++) {
    curr_total += candy_counts[end];
    while (curr_total > candies_thresh) {
      curr_total -= candy_counts[start];
      start++;
    }
    if (curr_total > best_total) {
      while (candy_counts[start] == 0) {
        start++;
      }
      best_total = curr_total;
      best_start = start;
      best_end = end;
    }
  }

  *home_start = best_start;
  *home_end = best_end;
}
