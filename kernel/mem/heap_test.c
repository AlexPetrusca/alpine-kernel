#include <kernel/test/test.h>

SUITE(heap)

  TEST(basic)
    void* ptr1 = kmalloc(100);
    void* ptr2 = kmalloc(128);
    void* ptr3 = kmalloc(64);
    void* ptr4 = kmalloc(1);
    kfree(ptr3);
    kfree(ptr2);
    kfree(ptr4);
    kfree(ptr1);
  END_TEST

END_SUITE

