#include <string.h>

#include <sys/strsplit.h>
#include <kernel/test/test.h>

SUITE(strsplit)
  char _str[128];

  TEST(strsplit_empty_split)
    strcpy(_str, "  \t \n  ");
    vector* split = strsplit(_str, " \t\n");
    ASSERT_EQUAL_UINT(split->length, 0);
    ASSERT_NULL(vector_get(split, 0));
    vector_free(split);
  END_TEST

  TEST(strsplit_single_token)
    strcpy(_str, "foobar");
    vector* split = strsplit(_str, "$_?");
    ASSERT_EQUAL_UINT(split->length, 1);
    ASSERT_EQUAL_STR(vector_get(split, 0), "foobar");
    vector_free(split);
  END_TEST

  TEST(strsplit_multiple_tokens)
    strcpy(_str, "Lorem ipsum dolor sit amet.");
    vector* split = strsplit(_str, " ");
    ASSERT_EQUAL_UINT(split->length, 5);
    ASSERT_EQUAL_STR(vector_get(split, 0), "Lorem");
    ASSERT_EQUAL_STR(vector_get(split, 1), "ipsum");
    ASSERT_EQUAL_STR(vector_get(split, 2), "dolor");
    ASSERT_EQUAL_STR(vector_get(split, 3), "sit");
    ASSERT_EQUAL_STR(vector_get(split, 4), "amet.");
    vector_free(split);
  END_TEST

END_SUITE