#ifndef _TEST_H_
#define _TEST_H_

#include <stdbool.h>
#include <stdint.h>

extern bool failed;

int assert_equal_bool(bool found, bool expected, int line, char* test, char* file);
int assert_equal_char(char found, char expected, int line, char* test, char* file);
int assert_equal_int(int64_t found, int64_t expected, int line, char* test, char* file);
int assert_equal_uint(uint64_t found, uint64_t expected, int line, char* test, char* file);
int assert_equal_str(char* found, char* expected, int line, char* test, char* file);
void runTest(void (* test)(void), const char* name);

#define _ARGS   __LINE__, (char*)__FUNCTION__, (char*)__FILE__

#define ASSERT_TRUE(found) if (assert_equal_bool(found, true, _ARGS)) { failed = true; return; }
#define ASSERT_FALSE(found) if (assert_equal_bool(found, false, _ARGS)) { failed = true; return; }
#define ASSERT_EQUAL_CHAR(found, expected) if (assert_equal_char(found, expected, _ARGS)) { failed = true; return; }
#define ASSERT_EQUAL_INT(found, expected) if (assert_equal_int(found, expected, _ARGS)) { failed = true; return; }
#define ASSERT_EQUAL_UINT(found, expected) if (assert_equal_uint(found, expected, _ARGS)) { failed = true; return; }
#define ASSERT_EQUAL_STR(found, expected) if (assert_equal_str(found, expected, _ARGS)) { failed = true; return; }

#define RUN_TEST(...) RUN_TEST_AT_LINE(__VA_ARGS__, __LINE__, throwaway)
#define RUN_TEST_AT_LINE(func, line, ...) runTest(func, #func)

#endif //_TEST_H_
