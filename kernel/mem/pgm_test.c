#include <kernel/test/test.h>
#include <kernel/mem/pgm.h>

SUITE(pgm)

  TEST(pgm_first_one)
    ASSERT_EQUAL_INT(pgm_first_one(0), -1);
    ASSERT_EQUAL_INT(pgm_first_one(1), 0);
    ASSERT_EQUAL_INT(pgm_first_one(2), 1);
    ASSERT_EQUAL_INT(pgm_first_one(3), 0);
    ASSERT_EQUAL_INT(pgm_first_one(128), 7);
    ASSERT_EQUAL_INT(pgm_first_one(0xF000000000000000), 60);
    ASSERT_EQUAL_INT(pgm_first_one(0x8000000000000000), 63);
    ASSERT_EQUAL_INT(pgm_first_one(0xFFFFFFFFFFFFFFFF), 0);
    for (int i = 0; i < 64; i++) {
      ASSERT_EQUAL_INT(pgm_first_one(1l << i), i);
    }
    uint64_t v = 0x0F0F0F0F0F0F0F0F;
    for (int i = 0; i < 64; i++) {
      ASSERT_EQUAL_INT(pgm_first_one(v), i);
      v = v << 1;
    }
    END_TEST

  TEST(alloc1)
    void* ptr = malloc(6 * PAGE_SIZE_4K);
    uint64_t base = (((uint64_t) ptr >> 12) + 1) << 12;
    uint64_t handle = pgm_init(base, 4 * PAGE_SIZE_4K);
    ASSERT_EQUAL_INT(pgm_used_pages(handle), 0);
    ASSERT_EQUAL_UINT(pgm_allocate_page(handle), base + 1 * PAGE_SIZE_4K);
    ASSERT_EQUAL_UINT(pgm_allocate_page(handle), base + 2 * PAGE_SIZE_4K);
    ASSERT_EQUAL_UINT(pgm_allocate_page(handle), base + 3 * PAGE_SIZE_4K);
    ASSERT_EQUAL_INT(pgm_used_pages(handle), 3);
    ASSERT_EQUAL_UINT(pgm_allocate_page(handle), 0);  // OOM
    free(ptr);
    END_TEST

  TEST(alloc2)
    void* ptr = malloc(103 * PAGE_SIZE_4K);
    uint64_t base = (((uint64_t) ptr >> 12) + 1) << 12;
    uint64_t handle = pgm_init(base, 101 * PAGE_SIZE_4K);
    ASSERT_EQUAL_INT(pgm_used_pages(handle), 0);
    for (uint64_t i = 0; i < 100; i++) {
      uint64_t page = pgm_allocate_page(handle);
      ASSERT_EQUAL_UINT(page, base + (1 + i) * PAGE_SIZE_4K);
    }
    ASSERT_EQUAL_INT(pgm_used_pages(handle), 100);
    free(ptr);
    END_TEST

  TEST(alloc_free_1)
    void* ptr = malloc(66 * PAGE_SIZE_4K);
    uint64_t base = (((uint64_t) ptr >> 12) + 1) << 12;
    uint64_t handle = pgm_init(base, 64 * PAGE_SIZE_4K);
    uint64_t p1 = pgm_allocate_page(handle);
    uint64_t p2 = pgm_allocate_page(handle);
    uint64_t p3 = pgm_allocate_page(handle);
    ASSERT_EQUAL_UINT(p1, base + 1 * PAGE_SIZE_4K);
    ASSERT_EQUAL_UINT(p2, base + 2 * PAGE_SIZE_4K);
    ASSERT_EQUAL_UINT(p3, base + 3 * PAGE_SIZE_4K);
    pgm_free_page(p2, handle);
    ASSERT_EQUAL_INT(pgm_used_pages(handle), 2);
    ASSERT_EQUAL_UINT(pgm_allocate_page(handle), p2);
    ASSERT_EQUAL_INT(pgm_used_pages(handle), 3);
    free(ptr);
    END_TEST

  TEST(alloc_free_2)
    void* ptr = malloc(140 * PAGE_SIZE_4K);
    uint64_t base = (((uint64_t) ptr >> 12) + 1) << 12;
    uint64_t handle = pgm_init(base, 128 * PAGE_SIZE_4K);
    ASSERT_EQUAL_INT(pgm_free_pages(handle), 127);
    for (int i = 0; i < 100; i++) {
      uint64_t page = pgm_allocate_page(handle);
      ASSERT_EQUAL_UINT(page, base + (1 + i) * PAGE_SIZE_4K);
    }

    for (int i = 0; i < 100; i++) {
      uint64_t page = base + (1 + i) * PAGE_SIZE_4K;
      pgm_free_page(page, handle);
      ASSERT_EQUAL_INT(pgm_used_pages(handle), 99);
      ASSERT_EQUAL_UINT(pgm_allocate_page(handle), page);
      ASSERT_EQUAL_INT(pgm_used_pages(handle), 100);
    }

    free(ptr);
    END_TEST

END_SUITE

