#include <stddef.h>
#include <stdint.h>

void findBestHomeRange(const uint16_t *const candy_counts,
                       const size_t home_count, const uint16_t candies_thresh,
                       size_t *const home_start, size_t *const home_end);

void findBestHomeRangeParallel(const uint16_t *const candy_counts,
                               const size_t home_count,
                               const uint16_t candies_thresh,
                               size_t *const home_start,
                               size_t *const home_end);
