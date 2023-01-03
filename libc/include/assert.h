#ifndef LIBC_INCLUDE_ASSERT_H_
#define LIBC_INCLUDE_ASSERT_H_

#include <stdlib.h>

#define assert(condition, message)    \
    if (!(condition)) panic(message); \

#endif //LIBC_INCLUDE_ASSERT_H_
