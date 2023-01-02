#ifndef KERNEL_ARCH_X86_64_KERR_H_
#define KERNEL_ARCH_X86_64_KERR_H_

#include <stddef.h>
#include <kernel/panic.h>

typedef enum {
  OK,
  MISSING_INFO,
  NOT_FOUND,
  NOT_INITED,
  LIMIT_EXCEEDED,
  UNKNOWN
} kerr_code_t;

#define PANIC(code)     \
  {                     \
    (code);               \
    if (kerr_get_code() != OK) { \
      panic("%s (%s)\n", kerr_get_message(), kerr_get_name(kerr_get_code())); \
    }                     \
  }

#define TRY(code)       \
  {                     \
    (code);               \
    if (kerr_get_code() != OK) return; \
  }

#define TRYD(code, def) \
  {                     \
    (code);               \
    if (kerrno_get() != OK) return def; \
  }

#define WARN(code, message)       \
  {                               \
    (code);               \
    if (kerr_get_code() != OK) { \
      printf("Warning: %s: %s (%s)\n", message, kerr_get_message(), kerr_get_name(kerr_get_code())); \
      kerr_clear(); \
    }   \
  }

#define IGNORE(code)    \
  {                     \
    (code);               \
    if (kerr_get_code() != OK) { \
      kerr_clear(); \
    }  \
  }

#define RAISE(code, message) \
  {                          \
    kerr_raise(code, message); \
    return;                  \
  }

#define RAISED(code, def, message) \
  {                                \
    kerr_raise(code, message); \
    return def;\
  }

void kerr_raise(kerr_code_t error, char* message);
kerr_code_t kerr_get_code();
char* kerr_get_message();
char* kerr_get_name(kerr_code_t code);
void kerr_clear();

#endif //KERNEL_ARCH_X86_64_KERR_H_
