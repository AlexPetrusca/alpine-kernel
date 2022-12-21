#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#define HEXADECIMAL 16
#define DECIMAL 10
#define OCTAL 8
#define BINARY 2

__BEGIN_DECLS

__attribute__((__noreturn__))
void abort();

char* itoa(int value, char* str, int base);

__END_DECLS

#endif
