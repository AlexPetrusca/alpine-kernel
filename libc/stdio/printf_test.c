#include <stdio.h>
#include "test.h"

suite(Printf)
  char _str[128];

  test(no_format)
    sprintf(_str, "World");
    ASSERT_EQUAL_STR(_str, "World");
    test_end()

  test(char_format)
    sprintf(_str, "%c", 'a');
    ASSERT_EQUAL_STR(_str, "a");
    test_end()

  test(string_format)
    sprintf(_str, "%s", "World");
    ASSERT_EQUAL_STR(_str, "World");
    sprintf(_str, "%.3s", "World");
    ASSERT_EQUAL_STR(_str, "Wor");

    const char* s = "monkeys";
    sprintf(_str, "'%s' is not justified.", s);
    ASSERT_EQUAL_STR(_str, "'monkeys' is not justified.");
    sprintf(_str, "'%10s' is right-justified.", s);
    ASSERT_EQUAL_STR(_str, "'   monkeys' is right-justified.");
    sprintf(_str, "'%-10s' is left-justified using a minus sign.", s);
    ASSERT_EQUAL_STR(_str, "'monkeys   ' is left-justified using a minus sign.");
    test_end()

  test(decimal_format)
    sprintf(_str, "The size is %d bytes", 124);
    ASSERT_EQUAL_STR(_str, "The size is 124 bytes");
    sprintf(_str, "The size is %i bytes", 124);
    ASSERT_EQUAL_STR(_str, "The size is 124 bytes");
    sprintf(_str, "The number is space padded '% 6d'", 124);
    ASSERT_EQUAL_STR(_str, "The number is space padded '   124'");
    sprintf(_str, "The number is zero padded '%05d'", 124);
    ASSERT_EQUAL_STR(_str, "The number is zero padded '00124'");

    int x = 345;
    sprintf(_str, "<%d> is not justified.", x);
    ASSERT_EQUAL_STR(_str, "<345> is not justified.");
    sprintf(_str, "<%5d> is right-justified.", x);
    ASSERT_EQUAL_STR(_str, "<  345> is right-justified.");
    sprintf(_str, "<%-5d> The minus sign makes it left-justified.", x);
    ASSERT_EQUAL_STR(_str, "<345  > The minus sign makes it left-justified.");
    test_end()

  test(hex_format)
    sprintf(_str, "The size is %x bytes", 255);
    ASSERT_EQUAL_STR(_str, "The size is FF bytes");
    test_end()

  test(binary_format)
    sprintf(_str, "The bits are %b.", 255);
    ASSERT_EQUAL_STR(_str, "The bits are 11111111.");
    sprintf(_str, "The bits are %b.", 128);
    ASSERT_EQUAL_STR(_str, "The bits are 10000000.");
    test_end()

  test(octal_format)
    sprintf(_str, "The octs are %o.", 255);
    ASSERT_EQUAL_STR(_str, "The octs are 377.");
    sprintf(_str, "The octs are %o.", 255);
    ASSERT_EQUAL_STR(_str, "The octs are 377.");
    test_end()

suite_end()