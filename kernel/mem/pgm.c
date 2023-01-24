#include <kernel/mem/pgm.h>
#include <kernel/cpu/asm.h>
#include <assert.h>

#define HANDLE(base)     ROR(base, 17)
#define HEADER(handle)   (pgm_header*) ROL(handle, 17)

typedef struct {
  uint64_t* dir_ptr;
  uint64_t dir_size;
  uint64_t* mem_ptr;
  uint64_t mem_pages;
  uint64_t free_pages;
  uint64_t* ptr;
} pgm_header;

uint64_t pgm_init(uint64_t base, uint64_t size) {
  assert(mem_prev_page_addr(base) == base, "Page Manager: Address should be at a page boundary");
  assert(size % PAGE_SIZE == 0, "Page Manager: Size should be an integer number of pages");
  uint64_t total_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
  uint64_t dir_bytes = (total_pages + 7) / 8;
  uint64_t dir_pages = (dir_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
  pgm_header header;
  header.dir_ptr = (uint64_t*) (base + sizeof(pgm_header));
  header.dir_size = dir_pages * PAGE_SIZE;
  header.mem_ptr = (uint64_t*) (base + header.dir_size);
  header.ptr = (uint64_t*) header.dir_ptr;
  header.mem_pages = total_pages - dir_pages;
  header.free_pages = header.mem_pages;
  try(mem_identity_map_range(base, header.dir_size), 0, "");
  memset((void*) header.dir_ptr, 0xFF, header.dir_size - sizeof(pgm_header));
  *((pgm_header*)base) = header;
  return HANDLE(base);
}

int pgm_first_one(uint64_t x) {
  int i = 0;
  if (!(x & 0x00000000FFFFFFFF)) {
    x = x >> 32;
    i += 32;
  }
  if (!(x & 0x0000FFFF)) {
    x = x >> 16;
    i += 16;
  }
  if (!(x & 0x00FF)) {
    x = x >> 8;
    i += 8;
  }
  if (!(x & 0x0F)) {
    x = x >> 4;
    i += 4;
  }
  if (!(x & 0x03)) {
    x = x >> 2;
    i += 2;
  }
  if (!(x & 0x01)) {
    x = x >> 1;
    i += 1;
  }
  return x ? i : -1;
}

uint64_t pgm_allocate_page(uint64_t handle) {
  pgm_header* _pgm_info = HEADER(handle);
  try(_pgm_info->free_pages, 0, "Page Manager: Out of memory pages");
  while (*_pgm_info->ptr == 0) {
    _pgm_info->ptr++;
  }
  uint64_t bit = pgm_first_one(*_pgm_info->ptr);
  *_pgm_info->ptr ^= (1L << bit); // flip bit
  _pgm_info->free_pages--;
  uint64_t bytes = (uint64_t) _pgm_info->ptr - (uint64_t) _pgm_info->dir_ptr;
  return (uint64_t) _pgm_info->mem_ptr + (bytes * 8L + bit) * PAGE_SIZE;
}

void pgm_free_page(uint64_t addr, uint64_t handle) {
  assert(mem_prev_page_addr(addr) == addr, "Page Manager: Page address is not a page boundary");
  pgm_header* _pgm_info = HEADER(handle);
  assert(addr % PAGE_SIZE == 0, "Page Manager: invalid page address");
  uint64_t page_index = (addr - (uint64_t) _pgm_info->mem_ptr) / PAGE_SIZE;
  uint64_t* ptr = _pgm_info->dir_ptr + page_index / 64L;
  int bit_index = page_index % 64;
  *ptr ^= (1L << bit_index);  // flip bit
  if (ptr < _pgm_info->ptr) {
    _pgm_info->ptr = ptr;
  }
  _pgm_info->free_pages++;
}

uint64_t pgm_total_pages(uint64_t handle) {
  pgm_header* _pgm_info = HEADER(handle);
  return _pgm_info->mem_pages;
}

uint64_t pgm_free_pages(uint64_t handle) {
  pgm_header* _pgm_info = HEADER(handle);
  return _pgm_info->free_pages;
}

uint64_t pgm_used_pages(uint64_t base) {
  return pgm_total_pages(base) - pgm_free_pages(base);
}

