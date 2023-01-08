#include <string.h>

#include <kernel/test/test.h>

SUITE(strtrim)
  char _str[128];

  TEST(strtrim_leading)
    strcpy(_str, " \t\n\v\f\r foobar \t\n\v\f\r ");
    char* trimmed = strtrim_leading(_str);
    ASSERT_TRUE(_str != trimmed);
    ASSERT_EQUAL_STR(trimmed, "foobar \t\n\v\f\r ");
  END_TEST

  TEST(strtrim_trailing)
    strcpy(_str, " \t\n\v\f\r foobar \t\n\v\f\r ");
    char* trimmed = strtrim_trailing(_str);
    ASSERT_TRUE(_str == trimmed);
    ASSERT_EQUAL_STR(trimmed, " \t\n\v\f\r foobar");
  END_TEST

  TEST(strtrim_leading_trailing)
    strcpy(_str, " \t\n\v\f\r foobar \t\n\v\f\r ");
    char* trimmed = strtrim(_str);
    ASSERT_TRUE(_str != trimmed);
    ASSERT_EQUAL_STR(trimmed, "foobar");
  END_TEST

END_SUITE