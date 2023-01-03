#ifndef ALPINE_KERNEL_VECTOR_H
#define ALPINE_KERNEL_VECTOR_H

#include <stddef.h>

#include <sys/cdefs.h>

#define VECTOR_INIT_CAPACITY 1

typedef struct {
  void** items;
  int capacity;
  int length;
} vector;

__BEGIN_DECLS

vector* vector_new();
void vector_init(vector* vec);
void vector_free(vector* vec);

void* vector_get(vector* vec, int idx);
void vector_set(vector* vec, int idx, void* item);
void vector_add(vector* vec, void* item);
void vector_delete(vector* vec, int idx);
void vector_resize(vector* vec, int capacity);

__END_DECLS

#endif //ALPINE_KERNEL_VECTOR_H
