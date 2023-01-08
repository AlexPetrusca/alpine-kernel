#ifndef _STRING_H
#define _STRING_H 1

#include <stddef.h>
#include <stdbool.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

void* memchr(const void* s, int c, size_t n);
void* memrchr(const void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);

int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
bool strequ(const char* s1, const char* s2);
bool strnequ(const char* s1, const char* s2, size_t n);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strchr(const char* s, int c);
char* strrchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
char* strrstr(const char* haystack, const char* needle);
char* strtok(char* str, const char* delim);
char* strtok_r(char* str, const char* delim, char** saveptr);
size_t strspn(const char* s1, const char* s2);
size_t strcspn(const char* s1, const char* s2);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
char* strrev(char* s);
size_t strlen(const char* s);

__END_DECLS

#endif
