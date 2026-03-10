#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint16_t *home_sequence;
  size_t home_sequence_len;
  uint16_t candies_total;
} HomeSequenceResult;

HomeSequenceResult bestHomeSequence(uint16_t *const candy_counts,
                                    const size_t home_count,
                                    const uint16_t candies_max) {
  const HomeSequenceResult result = {
      .home_sequence = NULL, .home_sequence_len = 0, .candies_total = 0};
  return result;
}

int main() {
  const char *const input_file = "input.txt";

  FILE *const file = fopen(input_file, "r");
  if (file == NULL) {
    fprintf(stderr, "Cannot open file %s\n", input_file);
    return 1;
  }

  size_t home_count;
  if (fscanf(file, "%zu", &home_count) != 1) {
    fprintf(stderr, "Cannot read home count\n");
    fclose(file);
    return 1;
  }

  uint16_t candies_max;
  if (fscanf(file, "%hu", &candies_max) != 1) {
    fprintf(stderr, "Cannot read max candy count\n");
    fclose(file);
    return 1;
  }

  uint16_t *const candy_counts =
      (uint16_t *)malloc(home_count * sizeof(uint16_t));

  const HomeSequenceResult result =
      bestHomeSequence(candy_counts, home_count, candies_max);

  if (result.home_sequence_len > 0) {
    printf("Start at home %hu and go to home %hu getting %hu pieces of candy\n",
           result.home_sequence[0],
           result.home_sequence[result.home_sequence_len - 1],
           result.candies_total);
  } else {
    printf("Don't go here\n");
  }

  free(candy_counts);
  fclose(file);
  return 0;
}
