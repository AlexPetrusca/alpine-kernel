#ifndef _STDIO_H
#define _STDIO_H 1

#include <stdarg.h>

#include <sys/cdefs.h>

#define EOF -1

__BEGIN_DECLS

int printf(const char * format, ...);
int sprintf(char * str, const char * format, ...);
int vprintf(const char * format, va_list ap);
int vsprintf(char * str, const char * format, va_list ap);

int getchar();
int putchar(int c);
int puts(const char * s);

__END_DECLS

#endif
