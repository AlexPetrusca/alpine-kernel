#include <string.h>
#include <stdlib.h>

#include <kernel/test/test.h>

SUITE(strdup)
  char* _str = "foobar";

  TEST(strdup)
    char* dup = strdup(_str);
    ASSERT_EQUAL_STR(dup, "foobar");
    free(dup);
  END_TEST

  TEST(strndup_lt_len)
    char* dup = strndup(_str, 3);
    ASSERT_EQUAL_STR(dup, "foo");
    free(dup);
  END_TEST

  TEST(strndup_eq_len)
    char* dup = strndup(_str, 6);
    ASSERT_EQUAL_STR(dup, "foobar");
    free(dup);
  END_TEST

  TEST(strndup_gt_len)
    char* dup = strndup(_str, 9);
    ASSERT_EQUAL_STR(dup, "foobar");
    free(dup);
  END_TEST

END_SUITE