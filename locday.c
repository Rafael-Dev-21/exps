#include <stdio.h>
#include <math.h>

#define DAY1  140.0
#define RATE  2.26
#define LOWER 1
#define UPPER 14
#define STEP  1

int main(void)
{
  int i;
  double value, total;

  total = 0;
  printf("DAY   LOCS   ACC.\n");
  printf("-----------------\n");
  for (i = LOWER; i <= UPPER; i += STEP) {
    value = DAY1 * RATE / powf(RATE, powf(i, logf(i) / logf(RATE)));
    total += value;
    printf("%3d %6.2f %6.2f\n", i, value, total);
  }
  printf("-----------------\n");
  printf("TOTAL: %6.2f\n", total);
}
