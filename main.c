#include "candy.c"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const char *const input_file = "input.txt";

  FILE *const file = fopen(input_file, "r");
  if (file == NULL) {
    fprintf(stderr, "Cannot open file %s\n", input_file);
    return 1;
  }

  size_t home_count;
  if (fscanf(file, "%zu\n", &home_count) != 1) {
    fprintf(stderr, "Cannot read home count\n");
    fclose(file);
    return 1;
  }

  uint16_t candies_thresh;
  if (fscanf(file, "%hu\n", &candies_thresh) != 1) {
    fprintf(stderr, "Cannot read max candy count\n");
    fclose(file);
    return 1;
  }

  const size_t candy_counts_size = home_count * sizeof(uint16_t);
  if (candy_counts_size / sizeof(uint16_t) != home_count) {
    fprintf(stderr, "Home count is too large to allocate candy counts\n");
    fclose(file);
    return 1;
  }
  uint16_t *const candy_counts = (uint16_t *)malloc(candy_counts_size);
  if (candy_counts == NULL) {
    fprintf(stderr, "Cannot allocate memory for candy counts\n");
    fclose(file);
    return 1;
  }

  for (size_t home_idx = 0; home_idx < home_count; home_idx++) {
    if (fscanf(file, "%hu\n", &candy_counts[home_idx]) != 1) {
      fprintf(stderr, "Cannot read candy count for home index %zu\n", home_idx);
      free(candy_counts);
      fclose(file);
      return 1;
    }
  }

  size_t home_start;
  size_t home_end;
  findBestHomeRangeParallel(candy_counts, home_count, candies_thresh,
                            &home_start, &home_end);

  if (home_start < home_count && home_end < home_count) {
    uint16_t candies_total = 0;
    for (size_t i = home_start; i <= home_end; i++) {
      candies_total += candy_counts[i];
    }
    printf("Start at home %zu and go to home %zu getting %hu pieces of candy\n",
           home_start + 1, home_end + 1, candies_total);
  } else {
    printf("Don't go here\n");
  }

  free(candy_counts);
  fclose(file);
  return 0;
}
