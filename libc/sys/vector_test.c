#include <kernel/test/test.h>

SUITE(vector)

  TEST(basic)
    vector* v = vector_new();
    vector_free(v);
  END_TEST

END_SUITE

