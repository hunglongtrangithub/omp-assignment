#include <stddef.h>
#include <stdint.h>

/**
 * Finds the contiguous subarray of `candy_counts` with the maximum sum that
 * does not exceed `candies_thresh`. Ties are broken by lowest starting index.
 *
 * If no valid range exists (empty array, zero threshold, or all individual
 * homes exceed the threshold), both `home_start` and `home_end` are set to
 * `home_count`.
 *
 * @param candy_counts   array of candy counts per home (0-based)
 * @param home_count     number of homes in the array
 * @param candies_thresh maximum total candy allowed (inclusive)
 * @param home_start     set to the 0-based index of the first home in the best
 * range
 * @param home_end       set to the 0-based index of the last home in the best
 * range
 */
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

  // Use a wider type to avoid potential overflow
  uint32_t curr_total = 0;
  uint32_t best_total = 0;

  size_t start = 0;
  for (size_t end = 0; end < home_count; end++) {
    curr_total += candy_counts[end];
    while (curr_total > candies_thresh) {
      curr_total -= candy_counts[start];
      start++;
    }
    if (curr_total > best_total) {
      best_total = curr_total;
      best_start = start;
      best_end = end;
    }
  }

  // If best_start and best_end are never updated, they will both be home_count,
  // which is the correct output for no valid range.
  *home_start = best_start;
  *home_end = best_end;
}
