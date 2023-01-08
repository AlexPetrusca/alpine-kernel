#include <string.h>

char* strstr(const char* haystack, const char* needle) {
  size_t n = strlen(needle);
  while (*haystack != '\0') {
    if (!memcmp(haystack++, needle, n)) {
      return (char*) haystack - 1;
    }
  }
  return NULL;
}

char* strrstr(const char* haystack, const char* needle) {
  char* found = NULL;
  size_t n = strlen(needle);
  while (*haystack != '\0') {
    if (!memcmp(haystack++, needle, n)) {
      found = (char*) haystack - 1;
    }
  }
  return found;
}