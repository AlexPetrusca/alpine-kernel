#include <stdio.h>
#include <string.h>
#include <kernel/test/test.h>

void test_print_failure(int line, char* test, char* suite, char* file) {
  printf("%s:%s: FAILURE (%s:%d): ", suite, test, file, line);
}

void fail(int line, char* test, char* suite, char* file, char* msg, ...) {
  test_print_failure(line, test, suite, file);
  VA_LIST marker;
  VA_START (marker, msg);
  vprintf(msg, marker);
  VA_END (marker);
}

int assert_equal_bool(bool found, bool expected, int line, char* test, char* suite, char* file) {
  if (found != expected) {
    char* found_str = found ? "true" : "false";
    char* expected_str = expected ? "true" : "false";
    test_print_failure(line, test, suite, file);
    printf("Expected %s but found %s\n", expected_str, found_str);
    return 1;
  }
  return 0;
}

int assert_equal_str(char* found, char* expected, int line, char* test, char* suite, char* file) {
  if (strcmp(found, expected) != 0) {
    test_print_failure(line, test, suite, file);
    printf("Expected '%s' but found '%s'\n", expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_strn(char* found, size_t length, char* expected, int line, char* test, char* suite, char* file) {
  if (strncmp(found, expected, length) != 0) {
    test_print_failure(line, test, suite, file);
    printf("Expected '%s' but found '%s'\n", expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_substr(char* found, int start, int end, char* expected,
                        int line, char* test, char* suite, char* file) {
  if (end < start) {
    test_print_failure(line, test, suite, file);
    printf("assert_equal_substr: end must be greater than or equal to start (start=%d, end=%d)\n",
           start, end);
    return 1;
  } else if (strncmp(found + start, expected, end - start) != 0) {
    test_print_failure(line, test, suite, file);
    printf("Expected '%s' but found '%s'\n", expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_char(char found, char expected, int line, char* test, char* suite, char* file) {
  if (found != expected) {
    test_print_failure(line, test, suite, file);
    printf("Expected '%c' but found '%c'\n", expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_uint(uint64_t found, uint64_t expected, int line, char* test, char* suite, char* file) {
  if (found != expected) {
    test_print_failure(line, test, suite, file);
    printf("Expected %lu but found %lu\n", expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_int(int64_t found, int64_t expected, int line, char* test, char* suite, char* file) {
  if (found != expected) {
    test_print_failure(line, test, suite, file);
    printf("Expected %ld but found %ld\n", expected, found);
    return 1;
  }
  return 0;
}

int assert_equal_ptr(void* found, void* expected, int line, char* test, char* suite, char* file) {
  if (found != expected) {
    test_print_failure(line, test, suite, file);
    printf("Expected 0x%lx but found 0x%lx\n", (uint64_t) expected, (uint64_t) found);
    return 1;
  }
  return 0;
}

