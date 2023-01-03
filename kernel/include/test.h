#ifndef _TEST_H_
#define _TEST_H_

#include <stdbool.h>
#include <stdint.h>
#include <kernel/kerr.h>

#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)
#define PPCAT3_NX(A, B, C) A ## B ## C
#define PPCAT3(A, B, C) PPCAT3_NX(A, B, C)
#define STRINGIZE_NX(A) #A
#define STRINGIZE(A) STRINGIZE_NX(A)

#define SUITEDECL(name) void PPCAT3(test_, name, _suite)()
#define SUITE(name)     \
  SUITEDECL(name) {     \
    printf("%s:\n", STRINGIZE(name));
#define ENDS }

#define TEST(test_name) \
  do {                  \
    char* name = STRINGIZE(test_name); \
    kerr_clear();

#define ENDT \
    ASSERT_KERR(OK, NULL); \
    printf("  %s: OK\n", name); \
  } while (0);

int assert_equal_str(char* found, char* expected, int line, char* test, char* file);
int assert_equal_bool(bool found, bool expected, int line, char* test, char* file);
int assert_equal_char(char found, char expected, int line, char* test, char* file);
int assert_equal_uint(uint64_t found, uint64_t expected, int line, char* test, char* file);
int assert_equal_int(int64_t found, int64_t expected, int line, char* test, char* file);
int assert_equal_ptr(void* found, void* expected, int line, char* test, char* file);
int assert_kerr(kerr_code_t kerr, char* msg, int line, char* test, char* file);

#define _ARGS __LINE__, name, (char*)__FILE__
#define ASSERT_TRUE(found) if (assert_equal_bool(found, true, _ARGS)) { break; }
#define ASSERT_FALSE(found) if (assert_equal_bool(found, false, _ARGS)) { break; }
#define ASSERT_EQUAL_STR(found, expected) if (assert_equal_str(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_CHAR(found, expected) if (assert_equal_char(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_INT(found, expected) if (assert_equal_int(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_UINT(found, expected) if (assert_equal_uint(found, expected, _ARGS)) { break; }
#define ASSERT_EQUAL_PTR(found, expected) if (assert_equal_ptr(found, expected, _ARGS)) { break; }
#define ASSERT_NULL(found) if (assert_equal_ptr(found, NULL, _ARGS)) { break; }
#define ASSERT_KERR(kerr, msg) if (assert_kerr(kerr, msg, _ARGS)) { break; }
#define ASSERT_RAISES_KERR(code, kerr, msg) \
    {                                       \
      ASSERT_KERR(OK, NULL);                  \
      code; \
      ASSERT_KERR(kerr, msg);                 \
      kerr_clear();  \
    }

#endif //_TEST_H_
