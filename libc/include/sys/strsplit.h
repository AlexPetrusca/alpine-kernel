#ifndef ALPINE_KERNEL_STRSPLIT_H
#define ALPINE_KERNEL_STRSPLIT_H

#include <sys/vector.h>

typedef vector strsplit_t;

strsplit_t* strsplit(char* str, const char* delim);
void strsplit_free(strsplit_t* split);

#endif //ALPINE_KERNEL_STRSPLIT_H
