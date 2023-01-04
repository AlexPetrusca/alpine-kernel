#ifndef LIBC_INCLUDE_ASSERT_H_
#define LIBC_INCLUDE_ASSERT_H_

#include <stdlib.h>

#define assert(condition, format, ...)    \
    if (!(condition)) panic(format, ##__VA_ARGS__); \

#endif //LIBC_INCLUDE_ASSERT_H_
