#include <string.h>

#include <kernel/test/test.h>

SUITE(strstr)
  const char* _str = "Lorem ipsum dolor sit amet. Lorem ipsum.";

  TEST(strstr_find)
    char* search = "ipsum";
    char* ret = strstr(_str, search);
    ASSERT_EQUAL_STRN(ret, 5, search);
  END_TEST

  TEST(strstr_not_found)
    char* search = "$$$$";
    char* ret = strstr(_str, search);
    ASSERT_NULL(ret);
  END_TEST

  TEST(strrstr_find)
    char* search = "ipsum";
    char* ret = strrstr(_str, search);
    ASSERT_EQUAL_STRN(ret, 5, search);
  END_TEST

  TEST(strrstr_not_found)
    char* search = "$$$$";
    char* ret = strrstr(_str, search);
    ASSERT_NULL(ret);
  END_TEST

  TEST(strrstr_gt_strstr)
    char* search = "ipsum";
    char* strstr_ret = strstr(_str, search);
    char* strrstr_ret = strrstr(_str, search);
    ASSERT_TRUE(strrstr_ret > strstr_ret);
  END_TEST

END_SUITE