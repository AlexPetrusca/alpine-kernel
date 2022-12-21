#include <string.h>

void* memchr(const void* s, int c, size_t n) {
  unsigned char* p = (unsigned char*) s;
  while (n--) {
    if (*p != (unsigned char) c) {
      p++;
    } else {
      return p;
    }
  }
  return NULL;
}

void* memrchr(const void* s, int c, size_t n) {
  unsigned char* p = (unsigned char*) s;
  while (n--) {
    if (p[n] == (unsigned char) c) {
      return p + n;
    }
  }
  return NULL;
}