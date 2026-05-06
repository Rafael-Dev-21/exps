#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
  int value;
  uintptr_t xored;
} Node;

Node *create_node(int value)
{
  Node *node = (Node *)malloc(sizeof(*node));
  assert(node != NULL);
  memset(node, 0, sizeof(*node));
  node->value = value;
  node->xored = 0;
  return node;
}

typedef struct {
  Node *begin;
  Node *end;
} LinkedList;

void ll_append(LinkedList *ll, int value)
{
  if (ll->begin == NULL) {
    assert(ll->end == NULL);
    ll->begin = create_node(value);
    ll->end = ll->begin;
  } else {
    Node *node = create_node(value);
    node->xored     = (uintptr_t)ll->end;
    ll->end->xored ^= (uintptr_t)node;
    ll->end         = node;
  }
}

Node *ll_next(Node *node, uintptr_t *prev)
{
  Node *next = (Node *)(node->xored ^ (*prev));
  *prev = (uintptr_t)node;
  return next;
}

int main(void)
{
  LinkedList ll = {0};

  for (int i = 5; i < 10; i++)
    ll_append(&ll, i);

  uintptr_t prev = 0;
  for (Node *iter = ll.begin; iter; iter = ll_next(iter, &prev))
    printf("%d\n", iter->value);

  printf("\n");
  prev = 0;
  for (Node *iter = ll.end; iter; iter = ll_next(iter, &prev))
    printf("%d\n", iter->value);
}
