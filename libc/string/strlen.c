#include <string.h>

size_t strlen(const char* s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

size_t strnlen(const char* s, size_t n) {
  const char* end = memchr(s, '\0', n);
  return end ? (size_t) (end - s) : n;
}