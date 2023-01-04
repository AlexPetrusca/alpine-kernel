#ifndef ALPINE_KERNEL_HEAP_H
#define ALPINE_KERNEL_HEAP_H

#include <stdint.h>

void heap_init(uint64_t addr, size_t size);

void* kmalloc(size_t size);
void kfree(void* ptr);
void* kcalloc(size_t nmemb, size_t size);
void* krealloc(void* ptr, size_t size);

#endif //ALPINE_KERNEL_HEAP_H
