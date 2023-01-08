#include <string.h>

#include <kernel/test/test.h>

SUITE(strcat)
  char _str[128];

  TEST(strcat_concat_two)
    strcpy(_str, "foo");
    char* concat = strcat(_str, "bar");
    ASSERT_EQUAL_STR(concat, "foobar");
  END_TEST

  TEST(strcat_concat_multiple)
    strcpy(_str, "foo");
    strcat(_str, "bar");
    strcat(_str, "buz");
    strcat(_str, "bam");
    ASSERT_EQUAL_STR(_str, "foobarbuzbam");
  END_TEST

  TEST(strncat_concat_two)
    strcpy(_str, "foo");
    char* concat = strncat(_str, "bar_foo", 3);
    ASSERT_EQUAL_STR(concat, "foobar");
  END_TEST

  TEST(strncat_concat_multiple)
    strcpy(_str, "foo");
    strncat(_str, "barbuz", 3);
    strncat(_str, "buzbam", 3);
    strncat(_str, "bambiz", 3);
    ASSERT_EQUAL_STR(_str, "foobarbuzbam");
  END_TEST

END_SUITE