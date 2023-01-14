#ifndef ALPINE_KERNEL_HEAP_H
#define ALPINE_KERNEL_HEAP_H

#include <stdint.h>
#include <stddef.h>

void heap_init(uint64_t addr, size_t size);
void heap_print_info();

void* kmalloc(size_t size);
void kfree(void* ptr);
void* kcalloc(size_t nmemb, size_t size);
void* krealloc(void* ptr, size_t size);

uint64_t kgetused();
uint64_t kgetfree();

#endif //ALPINE_KERNEL_HEAP_H
