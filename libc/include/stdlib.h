#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <stddef.h>
#include <stdint.h>

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

intmax_t strtoimax(const char* nptr, char** endptr, int base);
uintmax_t strtoumax(const char* nptr, char** endptr, int base);
long int strtol(const char* nptr, char** endptr, int base);
long long int strtoll(const char* nptr, char** endptr, int base);
unsigned long int strtoul(const char* nptr, char** endptr, int base);
unsigned long long int strtoull(const char* nptr, char** endptr, int base);

int atoi(const char* nptr);
long int atol(const char* nptr);
long long int atoll(const char* nptr);

char* itoa(int value, char* str, int base);
char* ltoa(long int value, char* str, int base);
char* lltoa(long long int value, char* str, int base);

__attribute__((__noreturn__)) void abort();
__attribute__((__noreturn__)) void panic(const char* format, ...);

__END_DECLS

#endif
