#include "test.h"
#include "stdio.h"
#include "string.h"

int assert_equal_bool(bool found, bool expected, int line, char* test, char* file) {
  if (found != expected) {
    char* found_str = found ? "true" : "false";
    char* expected_str = expected ? "true" : "false";
    printf("  %s: FAILURE (%s:%d): Expected %s but found %s\n", test, file, line, expected_str, found_str);
    return 1;
  }
  return 0;
}

int assert_equal_str(char* found, char* expected, int line, char* test, char* file) {
  if (strcmp(found, expected) != 0) {
    printf("  %s: FAILURE (%s:%d): Expected '%s' but found '%s'\n", test, file, line, expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_char(char found, char expected, int line, char* test, char* file) {
  if (found != expected) {
    printf("  %s: FAILURE (%s:%d): Expected '%c' but found '%c'\n", test, file, line, expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_uint(uint64_t found, uint64_t expected, int line, char* test, char* file) {
  if (found != expected) {
    printf("%s: FAILURE (%s:%d): Expected %lu but found %lu\n", test, file, line, expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_int(int64_t found, int64_t expected, int line, char* test, char* file) {
  if (found != expected) {
    printf("  %s: FAILURE (%s:%d): Expected %ld but found %ld\n", test, file, line, expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_ptr(void* found, void* expected, int line, char* test, char* file) {
  if (found != expected) {
    printf("  %s: FAILURE (%s:%d): Expected 0x%lx but found 0x%lx\n", test, file, line,
           (uint64_t) expected, (uint64_t) found);
    return 1;
  }
  return 0;
}

