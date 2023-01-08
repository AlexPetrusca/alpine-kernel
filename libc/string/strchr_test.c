#include <string.h>

#include <kernel/test/test.h>

SUITE(strchr)
  const char* _str = "Lorem ipsum dolor sit amet.";

  TEST(strchr_find)
    char ch = 'm';
    char* ret = strchr(_str, ch);
    ASSERT_EQUAL_INT(*ret, ch);
  END_TEST

  TEST(strchr_not_found)
    char ch = '$';
    char* ret = strchr(_str, ch);
    ASSERT_NULL(ret);
  END_TEST

  TEST(strrchr_find)
    char ch = 'm';
    char* ret = strrchr(_str, ch);
    ASSERT_EQUAL_INT(*ret, ch);
  END_TEST

  TEST(strrchr_not_found)
    char ch = '$';
    char* ret = strchr(_str, ch);
    ASSERT_NULL(ret);
  END_TEST

  TEST(strrchr_gt_strchr)
    char ch = 'm';
    char* strchr_ret = strchr(_str, ch);
    char* strrchr_ret = strrchr(_str, ch);
    ASSERT_TRUE(strrchr_ret > strchr_ret);
  END_TEST

END_SUITE