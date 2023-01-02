#include <stdio.h>
#include <kernel/panic.h>

void panic(const char* format, ...) {
  printf("KERNEL PANIC: ");

  VA_LIST marker;
  VA_START (marker, format);
  vprintf(format, marker);
  VA_END (marker);

  while (1);
}
