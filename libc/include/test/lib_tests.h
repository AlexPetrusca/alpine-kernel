#ifndef LIBC_STDIO_STDIO_TESTS_H_
#define LIBC_STDIO_STDIO_TESTS_H_

#include <kernel/test/test.h>

SUITEDECL(memchr);
SUITEDECL(sprintf);
SUITEDECL(strchr);
SUITEDECL(strtok);
SUITEDECL(strstr);
SUITEDECL(strcat);
SUITEDECL(strdup);

SUITEDECL(dll);
SUITEDECL(strsplit);

#endif //LIBC_STDIO_STDIO_TESTS_H_
