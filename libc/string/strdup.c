#include <string.h>
#include <stdlib.h>

char* strdup(const char* s) {
  size_t len = strlen(s);
  char* dup = malloc(len + 1);
  if (!dup) {
    return NULL;
  }
  return memcpy(dup, s, len + 1);
}

char* strndup(const char* s, size_t n) {
  size_t len = strnlen(s, n);
  char* dup = malloc(len + 1);
  if (!dup) {
    return NULL;
  }
  memcpy(dup, s, len);
  dup[len] = '\0';
  return dup;
}