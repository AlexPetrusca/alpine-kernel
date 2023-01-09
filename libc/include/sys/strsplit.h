#ifndef ALPINE_KERNEL_STRSPLIT_H
#define ALPINE_KERNEL_STRSPLIT_H

#include <sys/vector.h>

vector* strsplit(char* str, const char* delim);

#endif //ALPINE_KERNEL_STRSPLIT_H
