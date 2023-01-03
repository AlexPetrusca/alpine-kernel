#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>
#include <sys/cdefs.h>

#define HEXADECIMAL 16
#define DECIMAL 10
#define OCTAL 8
#define BINARY 2

__BEGIN_DECLS

void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

char* itoa(int value, char* str, int base);

__attribute__((__noreturn__)) void abort();
__attribute__((__noreturn__)) void panic(const char* format, ...);

__END_DECLS

#endif
