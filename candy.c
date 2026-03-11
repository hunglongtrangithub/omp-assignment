#include <assert.h>
#include <omp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// Finds the best home range in parallel. Criteria for "best":
// 1. Only ranges with non-zero sums are allowed to update the best
// range
// 2. If sums are equal, prefer the one with smaller start index
// 3. If sums and start indices are equal, prefer the one with smaller
// end index
// If no valid range exists, sets `home_start` and `home_end` to `home_count`.
void findBestHomeRange(const uint16_t candy_counts[], const size_t home_count,
                       const uint16_t candies_thresh, size_t *const home_start,
                       size_t *const home_end) {
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

// Builds the exclusive prefix sum array `P`. `P` is expected to have length
// `home_count + 1`. Returns 0 on success, non-zero on failure.
uint8_t buildPrefixSum(const uint16_t candy_counts[], const size_t home_count,
                       uint64_t P[]) {
  P[0] = 0;
  // Allocate for max number of threads. Actual number of threads may be less.
  uint64_t *offsets = malloc((size_t)omp_get_max_threads() * sizeof(uint64_t));
  if (offsets == NULL)
    return 1;

  // Every thread computes the prefix sums for its own chunk and stores the
  // total in offsets[tid].value
#pragma omp parallel
  {
    const int nthreads = omp_get_num_threads();
    const int tid = omp_get_thread_num();
    const size_t chunk = (home_count + (size_t)nthreads - 1) / (size_t)nthreads;
    const size_t chunk_start = (size_t)tid * chunk;
    const size_t chunk_end =
        chunk_start + chunk < home_count ? chunk_start + chunk : home_count;

    if (chunk_start < home_count) {
      // Save result for chunk_start
      P[chunk_start + 1] = candy_counts[chunk_start];
      // Compute for the rest of the chunk
      for (size_t i = chunk_start + 1; i < chunk_end; i++)
        P[i + 1] = P[i] + candy_counts[i];
      // Save total for this chunk as offset for the next chunk
      offsets[tid] = P[chunk_end];
    }

#pragma omp barrier

    // Sequentially compute the true offset for each chunk based on the totals
    // computed by each thread
#pragma omp single
    {
      uint64_t curr_offset = 0;
      for (int t = 0; t < nthreads; t++) {
        const uint64_t chunk_total = offsets[t];
        offsets[t] = curr_offset;
        curr_offset += chunk_total;
      }
    }

    // Update the prefix sums for this chunk based on its true offset
    if (chunk_start < home_count) {
      const uint64_t offset = offsets[tid];
      for (size_t i = chunk_start; i < chunk_end; i++)
        P[i + 1] += offset;
    }
  }

  free(offsets);
  return 0;
}

// Does binary search and returns the leftmost index in `P[lo..hi]` where
// `P[index] >= target`. If no such index exists, returns `hi`.
size_t lowerBound(const uint64_t P[], size_t lo, size_t hi,
                  const uint64_t target) {
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (P[mid] >= target)
      hi = mid;
    else
      lo = mid + 1;
  }
  return lo;
}

// Finds the best home range in parallel. Criteria for "best":
// 1. Only ranges with non-zero sums are allowed to update the best
// range
// 2. If sums are equal, prefer the one with smaller start index
// 3. If sums and start indices are equal, prefer the one with smaller
// end index
// If no valid range exists, sets `home_start` and `home_end` to `home_count`.
// Returns 0 on success, non-zero on failure.
uint8_t findBestHomeRangeParallel(const uint16_t candy_counts[],
                                  const size_t home_count,
                                  const uint16_t candies_thresh,
                                  size_t *home_start, size_t *home_end) {
  if (home_count == 0 || candies_thresh == 0) {
    *home_start = home_count;
    *home_end = home_count;
    return 0;
  }

  uint64_t *P = malloc((home_count + 1) * sizeof(uint64_t));
  if (P == NULL)
    return 1;

  uint8_t err = buildPrefixSum(candy_counts, home_count, P);
  if (err != 0) {
    free(P);
    return err;
  }

  size_t best_start = home_count;
  size_t best_end = home_count;
  uint64_t best_total = 0;

#pragma omp parallel
  {
    int tid = omp_get_thread_num();
    int nthreads = omp_get_num_threads();

    const size_t chunk = (home_count + (size_t)nthreads - 1) / (size_t)nthreads;
    const size_t chunk_start = (size_t)tid * chunk;
    const size_t chunk_end =
        chunk_start + chunk < home_count ? chunk_start + chunk : home_count;

    size_t local_start = home_count;
    size_t local_end = home_count;
    uint64_t local_best = 0;

    if (chunk_start < home_count) {
      // Find the leftmost index i such that P[chunk_start + 1] - P[i] <=
      // candies_thresh <=> P[i] >= P[chunk_start + 1] - candies_thresh.
      const uint64_t P_chunk_start = P[chunk_start + 1];
      const uint64_t target =
          P_chunk_start > candies_thresh ? P_chunk_start - candies_thresh : 0;
      size_t i = lowerBound(P, 0, chunk_start + 1, target);

      // Slide through this thread's chunk
      for (size_t j = chunk_start; j < chunk_end; j++) {
        const uint64_t P_j = P[j + 1];
        // Advance i to maintain sum <= thresh
        while (P_j - P[i] > candies_thresh)
          i++;

        const uint64_t sum = P_j - P[i];
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
  return 0;
}
