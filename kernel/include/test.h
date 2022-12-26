#ifndef _TEST_H_
#define _TEST_H_

#include <stdbool.h>
#include <stdint.h>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

#define suitedecl(name) void PPCAT(name, TestSuite)();
#define suite(name) \
  void PPCAT(name, TestSuite)() {      \
    printf("%s:\n", STRINGIZE(PPCAT(name, TestSuite)));
#define suite_end() }

#define test(test_name) \
  do {                  \
    char* name = STRINGIZE(test_name);

#define test_end()  \
    printf("  %s: OK\n", name); \
  } while (0);

int assert_equal_str(char* found, char* expected, int line, char* test, char* file);
int assert_equal_bool(bool found, bool expected, int line, char* test, char* file);
int assert_equal_char(char found, char expected, int line, char* test, char* file);
int assert_equal_uint(uint64_t found, uint64_t expected, int line, char* test, char* file);
int assert_equal_int(int64_t found, int64_t expected, int line, char* test, char* file);

#define _ARGS __LINE__, name, (char*)__FILE__
#define ASSERT_TRUE(found) if (assert_equal_bool(found, true, _ARGS)) { break; }
#define ASSERT_FALSE(found) if (assert_equal_bool(found, false, _ARGS)) { break; }
#define ASSERT_EQUAL_STR(found, expected) if (assert_equal_str(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_CHAR(found, expected) if (assert_equal_char(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_INT(found, expected) if (assert_equal_int(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_UINT(found, expected) if (assert_equal_uint(found, expected, _ARGS)) { break; }

#endif //_TEST_H_
