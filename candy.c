#include <stddef.h>
#include <stdint.h>

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
  uint64_t curr_total = 0;
  uint64_t best_total = 0;

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

void findBestHomeRangeParallel(const uint16_t *const candy_counts,
                               const size_t home_count,
                               const uint16_t candies_thresh,
                               size_t *const home_start,
                               size_t *const home_end) {
  if (home_count == 0 || candies_thresh == 0) {
    *home_start = home_count;
    *home_end = home_count;
    return;
  }

  size_t best_start = home_count;
  size_t best_end = home_count;
  uint64_t best_total = 0;

  // We parallelize the search by having each thread compute the best range
  // starting from a different home index. Time complexity is O(n^2/p) where p
  // is the number of threads, compared to O(n) for the optimal sequential
  // solution.
#pragma omp parallel
  {
    size_t local_best_start = home_count;
    size_t local_best_end = home_count;
    uint64_t local_best_total = 0;

    // We use schedule(dynamic) since the amount of work for each starting index
    // can vary.
#pragma omp for schedule(dynamic)
    for (size_t start = 0; start < home_count; start++) {
      uint64_t curr_total = 0;
      for (size_t end = start; end < home_count; end++) {
        curr_total += candy_counts[end];
        if (curr_total > candies_thresh) {
          break;
        }
        if (curr_total > local_best_total) {
          local_best_total = curr_total;
          local_best_start = start;
          local_best_end = end;
        }
      }
    }

    // Update the global best range with the local best range found by this
    // thread in a critical section.
#pragma omp critical
    {
      if (local_best_total > best_total ||
          (local_best_total == best_total && local_best_start < best_start)) {
        best_total = local_best_total;
        best_start = local_best_start;
        best_end = local_best_end;
      }
    }
  }

  *home_start = best_start;
  *home_end = best_end;
}
