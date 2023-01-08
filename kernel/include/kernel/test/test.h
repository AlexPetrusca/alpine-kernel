#ifndef _TEST_H_
#define _TEST_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <kernel/error/error.h>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define PPCAT3_NX(A, B, C) A ## B ## C
#define PPCAT3(A, B, C) PPCAT3_NX(A, B, C)
#define STRINGIFY_NX(A) #A
#define STRINGIFY(A) STRINGIFY_NX(A)

#define SUITEDECL(name)                  \
  void PPCAT3(test_, name, _suite)()
#define SUITE(name)                      \
  SUITEDECL(name) {                     \
    printf("%s:\n", STRINGIFY(name));
#define END_SUITE }

#define TEST(test_name)                  \
  do {                                  \
    char* _name = STRINGIFY(test_name);

#define END_TEST                         \
    printf("  %s: OK\n", _name);        \
  } while (0);

int assert_equal_str(char* found, char* expected, int line, char* test, char* file);
int assert_equal_strn(char* found, size_t length, char* expected, int line, char* test, char* file);
int assert_equal_substr(char* found, int start, int end, char* expected, int line, char* test, char* file);
int assert_equal_bool(bool found, bool expected, int line, char* test, char* file);
int assert_equal_char(char found, char expected, int line, char* test, char* file);
int assert_equal_uint(uint64_t found, uint64_t expected, int line, char* test, char* file);
int assert_equal_int(int64_t found, int64_t expected, int line, char* test, char* file);
int assert_equal_ptr(void* found, void* expected, int line, char* test, char* file);

#define _ASSERT_ARGS __LINE__, _name, (char*) __FILE__
#define ASSERT_TRUE(found) if (assert_equal_bool(found, true, _ASSERT_ARGS)) { break; }
#define ASSERT_FALSE(found) if (assert_equal_bool(found, false, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_STR(found, expected) if (assert_equal_str(found, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_STRN(found, length, expected) if (assert_equal_strn(found, length, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_SUBSTR(found, start, end, expected) if (assert_equal_substr(found, start, end, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_CHAR(found, expected) if (assert_equal_char(found, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_INT(found, expected) if (assert_equal_int(found, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_UINT(found, expected) if (assert_equal_uint(found, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_EQUAL_PTR(found, expected) if (assert_equal_ptr(found, expected, _ASSERT_ARGS)) { break; }
#define ASSERT_NULL(found) if (assert_equal_ptr(found, NULL, _ASSERT_ARGS)) { break; }

#endif //_TEST_H_
