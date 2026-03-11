#include <stddef.h>
#include <stdint.h>

void findBestHomeRange(const uint16_t candy_counts[], const size_t home_count,
                       const uint16_t candies_thresh, size_t *const home_start,
                       size_t *const home_end);

uint8_t buildPrefixSum(const uint16_t candy_counts[], const size_t home_count,
                       uint64_t P[]);

uint8_t findBestHomeRangeParallel(const uint16_t candy_counts[],
                                  const size_t home_count,
                                  const uint16_t candies_thresh,
                                  size_t *home_start, size_t *home_end);

size_t lowerBound(const uint64_t P[], size_t lo, size_t hi,
                  const uint64_t target);
