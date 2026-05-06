#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  size_t count;
  size_t capacity;
} Header;

#define ARR_INITIAL_CAPACITY 8
#define ARR_GROWTH_RATE 1.5

#define arr_push(arr, x) \
  do { \
    Header *header; \
    if ((arr) == NULL) { \
      header = malloc(sizeof(*arr) * ARR_INITIAL_CAPACITY + sizeof(Header)); \
      header->capacity = ARR_INITIAL_CAPACITY; \
      header->count = 0; \
      arr = (void *)(header + 1); \
    } else { \
      header = (Header*)(arr)-1; \
    } \
    if (header->count >= header->capacity) { \
      header->capacity = (size_t)(header->capacity * ARR_GROWTH_RATE); \
      header = realloc(header, sizeof(*arr) * header->capacity + sizeof(Header)); \
      arr = (void *)(header + 1); \
    } \
    arr[header->count++] = (x); \
  } while (0)
#define arr_len(arr) ((Header*)(arr) - 1)->count
#define arr_free(arr) free((Header*)(arr) - 1)

int main(void)
{
  int *is = NULL;
  arr_push(is, 5);
  arr_push(is, 6);
  arr_push(is, 7);
  arr_push(is, 8);
  for (size_t i = 0; i < arr_len(is); i++) {
    printf("%d\n", is[i]);
  }
  arr_free(is);
  float *fs = NULL;
  arr_push(fs, 5);
  arr_push(fs, 6);
  arr_push(fs, 7);
  arr_push(fs, 8);
  for (size_t i = 0; i < arr_len(fs); i++) {
    printf("%f\n", fs[i]);
  }
  arr_free(fs);
  return 0;
}
