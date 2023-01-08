#include <kernel/test/test.h>
#include <sys/strsplit.h>

SUITE(strsplit)
  char* _simple_str = "foobar";
  char* _complex_str = "Lorem ipsum dolor sit amet.";

  TEST(strsplit_single)
    strsplit_t* split = strsplit(_simple_str, "$_?");
    ASSERT_EQUAL_UINT(split->length, 1);
    ASSERT_EQUAL_STR(vector_get(split, 0), "foobar");
    strsplit_free(split);
  END_TEST

  TEST(strsplit_multiple)
    strsplit_t* split = strsplit(_complex_str, " ");
    ASSERT_EQUAL_UINT(split->length, 5);
    ASSERT_EQUAL_STR(vector_get(split, 0), "Lorem");
    ASSERT_EQUAL_STR(vector_get(split, 1), "ipsum");
    ASSERT_EQUAL_STR(vector_get(split, 2), "dolor");
    ASSERT_EQUAL_STR(vector_get(split, 3), "sit");
    ASSERT_EQUAL_STR(vector_get(split, 4), "amet.");
    strsplit_free(split);
  END_TEST

END_SUITE