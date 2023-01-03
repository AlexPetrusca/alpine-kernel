#include <stdio.h>
#include <stdlib.h>

__attribute__((__noreturn__))
void abort(void) {
#if defined(__is_libk)
  panic("");
#else
  // TODO: Abnormally terminate the process as if by SIGABRT.
  printf("abort()\n");
#endif
  while (1) {}
  __builtin_unreachable();
}

__attribute__((__noreturn__))
void panic(const char* format, ...) {
  printf("KERNEL PANIC: ");

  VA_LIST marker;
  VA_START (marker, format);
  vprintf(format, marker);
  VA_END (marker);

  while (1);
}