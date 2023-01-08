#include <string.h>

#include <kernel/test/test.h>

SUITE(strtok)
  char _str[128];

  TEST(strtok_tokenize_single)
    strcpy(_str, "Lorem ipsum dolor sit amet.");
    char* token = strtok(_str, " ");
    ASSERT_EQUAL_STR(token, "Lorem");
  END_TEST

  TEST(strtok_tokenize_all)
    strcpy(_str, "Lorem ipsum dolor sit amet.");
    int counter = 0;
    char* token = strtok(_str, " ");
    while (token != NULL) {
      token = strtok(NULL, " ");
      counter++;
    }
    ASSERT_EQUAL_INT(counter, 5);
  END_TEST

  TEST(strtok_r_tokenize_single)
    strcpy(_str, "Lorem ipsum dolor sit amet.");
    char* saveptr;
    char* token = strtok_r(_str, " ", &saveptr);
    ASSERT_EQUAL_STR(token, "Lorem");
  END_TEST

  TEST(strtok_r_tokenize_all)
    strcpy(_str, "Lorem ipsum dolor sit amet.");
    int counter = 0;
    char* saveptr;
    char* token = strtok_r(_str, " ", &saveptr);
    while (token != NULL) {
      token = strtok_r(NULL, " ", &saveptr);
      counter++;
    }
    ASSERT_EQUAL_INT(counter, 5);
  END_TEST

END_SUITE