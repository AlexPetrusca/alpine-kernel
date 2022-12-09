#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

__BEGIN_DECLS

int memcmp(const void * s1, const void * s2, size_t n);
void * memcpy(void * __restrict dest, const void * __restrict src, size_t n);
void * memmove(void * dest, const void * src, size_t n);
void * memset(void * s, int c, size_t n);

char * strcpy(char * dest, const char * src);
char * strncpy(char *dest, const char *src, size_t n);
char * strrev(char * s);
size_t strlen(const char * s);

__END_DECLS

#endif
