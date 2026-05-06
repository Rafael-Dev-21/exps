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
#define arr_len(arr) ((Header*)(arr) - 1)->count
#define arr_free(arr) free((Header*)(arr) - 1)

int readin(char **buf)
{
  int c;
  while ((c = getchar()) != EOF) {
    arr_push(*buf, (char)c);
    if (c == '\n') {
      arr_push(*buf, '\0');
      break;
    }
  }
  return c != EOF;
}

int main(void)
{
  char *line = NULL;
  printf("> ");
  while (readin(&line)) {
    if (arr_len(line)) {
      printf("%s", line);
    }
    arr_free(line);
    line = NULL;
    printf("> ");
  }
  printf("\n");
  if (line) {
    arr_free(line);
  }
  line = NULL;
  return 0;
}
