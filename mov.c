#include <stdio.h>

#define DAMP 0.58f
#define ACCEL 0.1f
#define UPPER 100
#define LOWER -100
#define STEP 5

int main(void)
{
  float input;
  float output;
  int i;
  for (i = LOWER-STEP; i <= UPPER+STEP; i = i + STEP) {
    input = i / (float) UPPER;
    output = (input * ACCEL) / (1.0f - DAMP);
    printf("%f\t%f\n", input, output);
  }
}
