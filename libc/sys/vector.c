#include <stdlib.h>

#include <sys/vector.h>

// todo: add error checking

vector* vector_new() {
  vector* vec = malloc(sizeof(vector));
  vector_init(vec);
  return vec;
}

void vector_init(vector* vec) {
  vec->capacity = VECTOR_INIT_CAPACITY;
  vec->length = 0;
  vec->items = malloc(sizeof(void*) * vec->capacity);
}

void vector_free(vector* vec) {
  free(vec->items);
  free(vec);
}

void vector_resize(vector* vec, int capacity) {
  void** items = realloc(vec->items, sizeof(void*) * capacity);
  if (items) {
    vec->items = items;
    vec->capacity = capacity;
  }
}

void vector_add(vector* vec, void* item) {
  if (vec->capacity == vec->length) {
    vector_resize(vec, vec->capacity * 2);
  }
  vec->items[vec->length++] = item;
}

void vector_set(vector* vec, int idx, void* item) {
  if (idx >= 0 && idx < vec->length) {
    vec->items[idx] = item;
  }
}

void* vector_get(vector* vec, int idx) {
  if (idx >= 0 && idx < vec->length) {
    return vec->items[idx];
  }
  return NULL;
}

void vector_delete(vector* vec, int idx) {
  if (idx < 0 || idx >= vec->length) {
    return;
  }

  vec->items[idx] = NULL;
  for (int i = idx; i < vec->length - 1; i++) {
    vec->items[i] = vec->items[i + 1];
    vec->items[i + 1] = NULL;
  }

  vec->length--;
  if (vec->length > 0 && vec->length == vec->capacity / 4) {
    vector_resize(vec, vec->capacity / 2);
  }
}
