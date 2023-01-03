#ifndef KERNEL_ARCH_X86_64_ERROR_H_
#define KERNEL_ARCH_X86_64_ERROR_H_

#include <stddef.h>
#include <stdbool.h>

#define try(code, def, message)       \
    if ((code) <= 0) { \
      printf("Warning: %s:", message);\
      return def; \
    }

#endif //KERNEL_ARCH_X86_64_ERROR_H_
