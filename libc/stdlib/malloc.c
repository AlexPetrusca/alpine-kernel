#include <stdlib.h>

#include <kernel/heap.h>

void* malloc(size_t size) {
  return kmalloc(size);
}

void free(void* ptr) {
  kfree(ptr);
}

void* calloc(size_t nmemb, size_t size) {
  return kcalloc(nmemb, size);
}

void* realloc(void* ptr, size_t size) {
  return krealloc(ptr, size);
}