#include <string.h>

#include <kernel/test/test.h>

SUITE(memchr)
  const char* _str = "Lorem ipsum dolor sit amet.";

  TEST(memchr_find)
    char ch = 'm';
    char* ret = memchr(_str, ch, strlen(_str));
    ASSERT_EQUAL_INT(*ret, ch);
  END_TEST

  TEST(memchr_not_found)
    char ch = '$';
    char* ret = memchr(_str, ch, strlen(_str));
    ASSERT_NULL(ret);
  END_TEST

  TEST(memrchr_find)
    char ch = 'm';
    char* ret = memrchr(_str, ch, strlen(_str));
    ASSERT_EQUAL_INT(*ret, ch);
  END_TEST

  TEST(memrchr_not_found)
    char ch = '$';
    char* ret = memchr(_str, ch, strlen(_str));
    ASSERT_NULL(ret);
  END_TEST

  TEST(memrchr_gt_memchr)
    char ch = 'm';
    char* memchr_ret = memchr(_str, ch, strlen(_str));
    char* memrchr_ret = memrchr(_str, ch, strlen(_str));
    ASSERT_TRUE(memrchr_ret > memchr_ret);
  END_TEST

END_SUITE