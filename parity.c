#include <stdio.h>

int main(void)
{
  unsigned long val = 0x5AEAA53LU;
  printf("0x%x = %lu\n", val, val);
  val = (val >> 16) ^ (val & 0xFFFF);
  printf("0x%x = %u\n", val, val);
  val = (val >> 8) ^ (val & 0xFF);
  printf("0x%x = %u = '%c'\n", val, val, val);
  val = (val >> 4) ^ (val & 0xF);
  printf("0x%x = %u\n", val, val);
  val = (val >> 2) ^ (val & 0x3);
  printf("0x%x = %u\n", val, val);
  val = (val >> 1) ^ (val & 0x1);
  printf("0x%x = %u\n", val, val);
}
