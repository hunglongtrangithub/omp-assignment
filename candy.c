#include <omp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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

// Builds prefix sum array of length home_count + 1.
// Caller is responsible for allocating P with size home_count + 1.
static void build_prefix_sum(const uint16_t *candy_counts, size_t home_count,
                             uint64_t *P) {
  P[0] = 0;
  for (size_t i = 0; i < home_count; i++)
    P[i + 1] = P[i] + candy_counts[i];
}

// Returns the leftmost index in P[lo..hi] where P[index] >= target.
// If no such index exists, returns hi.
static size_t lower_bound(const uint64_t *P, size_t lo, size_t hi,
                          uint64_t target) {
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (P[mid] >= target)
      hi = mid;
    else
      lo = mid + 1;
  }
  return lo;
}

void findBestHomeRangeParallel(const uint16_t *candy_counts, size_t home_count,
                               uint16_t candies_thresh, size_t *home_start,
                               size_t *home_end) {
  if (home_count == 0 || candies_thresh == 0) {
    *home_start = home_count;
    *home_end = home_count;
    return;
  }

  uint64_t *P = malloc((home_count + 1) * sizeof(uint64_t));
  if (!P) {
    *home_start = home_count;
    *home_end = home_count;
    return;
  }
  build_prefix_sum(candy_counts, home_count, P);

  size_t best_start = home_count;
  size_t best_end = home_count;
  uint64_t best_total = 0;

#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int nthreads = omp_get_num_threads();

    // Compute this thread's chunk boundaries
    size_t chunk = (home_count + (size_t)nthreads - 1) / (size_t)nthreads;
    size_t chunk_start = (size_t)tid * chunk;
    size_t chunk_end =
        chunk_start + chunk < home_count ? chunk_start + chunk : home_count;

    size_t local_start = home_count;
    size_t local_end = home_count;
    uint64_t local_best = 0;

    if (chunk_start < home_count) {
      // Bootstrap: binary search for the correct initial left pointer
      uint64_t target = P[chunk_start + 1] > candies_thresh
                            ? P[chunk_start + 1] - candies_thresh
                            : 0;
      size_t i = lower_bound(P, 0, chunk_start + 1, target);

      // Slide through this thread's chunk
      for (size_t j = chunk_start; j < chunk_end; j++) {
        // Advance i to maintain sum <= thresh
        while (P[j + 1] - P[i] > candies_thresh)
          i++;

        uint64_t sum = P[j + 1] - P[i];
        if (sum > 0 &&
            (sum > local_best || (sum == local_best && i < local_start) ||
             (sum == local_best && i == local_start && j < local_end))) {
          local_best = sum;
          local_start = i;
          local_end = j;
        }
      }
    }

#pragma omp critical
    {
      if (local_best > best_total ||
          (local_best > 0 && local_best == best_total &&
           local_start < best_start) ||
          (local_best > 0 && local_best == best_total &&
           local_start == best_start && local_end < best_end)) {
        best_total = local_best;
        best_start = local_start;
        best_end = local_end;
      }
    }
  }

  free(P);
  *home_start = best_start;
  *home_end = best_end;
}
