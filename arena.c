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
      (arr) = (void *)(header + 1); \
    } \
    (arr)[header->count++] = (x); \
  } while (0)
#define arr_len(arr) ((arr) ? ((Header*)(arr) - 1)->count : 0)
#define arr_free(arr) do { free((arr) ? (Header*)(arr) - 1 : 0); (arr) = NULL; } while (0)

typedef struct arena {
  char *data;
} Arena;

void *arena_alloc(Arena *ar, size_t size)
{
  int ptr = arr_len(ar->data);
  for (int i = 0; i < size; i++) {
    arr_push(ar->data, size);
  }
  return ar->data + ptr;
}

size_t arena_len(Arena *ar)
{
  return arr_len(ar->data);
}

void arena_clear(Arena *ar)
{
  if (!arena_len(ar))
    return;
  (((Header*)ar->data)-1)->count = 0;
}

void arena_free(Arena *ar)
{
  arr_free(ar->data);
}

int main(void)
{
  Arena arena = {0};

  char *buf = arena_alloc(&arena, sizeof(char) * 1024);
  uint32_t *fs = arena_alloc(&arena, sizeof(uint32_t) * 3);
  
  for (int i = 0; i < 1023; ++i) {
    buf[i] = (rand()>>10) & 255;
    for (int j = 0; j < 3; ++j)
      fs[j] = fs[j] * 31 + buf[i];
  }

  printf("<%d,%d,%d>\n", fs[0], fs[1], fs[2]);

  arena_clear(&arena);

  int * pI = arena_alloc(&arena, sizeof(int));

  *pI = 5;

  arena_free(&arena);

  return 0;
}
