#include <stdio.h>

int main(void)
{
  unsigned h = 963;
  const char *data = "Do you even call that a hotel I didnt even get a mint under my pillow or anything 1/15*.";
  while (*data) {
    h = h * 31 + *data++;
  }
  printf("lu h = %x\n", h);
}
