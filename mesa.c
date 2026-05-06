#include <stdio.h>

#define LOWER 0
#define UPPER 10
#define STEP 1

int main(void)
{
  int n;
  printf("Vejo %d mesas, com:\n", UPPER);
  for (n=LOWER; n<=UPPER; n+=STEP)
    printf("+ %d pessoa%s\n", n, n > 1 ? "s" : "");
  return 0;
}
