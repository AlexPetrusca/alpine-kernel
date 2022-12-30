#include <kernel/mem.h>
#include <stdio.h>
#include <stdbool.h>
#include <kernel/multiboot2.h>

#define PAGE_NUMBER_MASK 0x0000FFFFFFFFF000
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITE      (1 << 1)

#define PML4T_START   0x1000
#define PDPT_START    0x2000
#define PDT_START     0x3000
#define PT_START      0x4000

uint64_t _next_page_pointer = PT_START + PAGE_SIZE;

uint32_t _mem_lower;
uint32_t _mem_upper;
struct multiboot_tag_mmap* _mem_map;
char* mem_type[] = {"", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad"};

uint64_t allocate_page() {
  uint64_t page_ptr = _next_page_pointer;
  _next_page_pointer += PAGE_SIZE;
  for (uint64_t* p = (uint64_t*) page_ptr; p < (uint64_t*) _next_page_pointer; p++) {
    *p = 0;
  }
  return page_ptr;
}

void identity_map(uint64_t addr) {
  VirtualAddress vaddr = {addr};
  uint64_t* pml4_entry = ((uint64_t*) PML4T_START) + vaddr.pml4_index;
  if (*pml4_entry == 0) {
    *pml4_entry = allocate_page() | PAGE_PRESENT | PAGE_WRITE;
  }
  uint64_t* pdpt_entry = ((uint64_t*) (*pml4_entry & PAGE_NUMBER_MASK)) + vaddr.pdpt_index;
  if (*pdpt_entry == 0) {
    *pdpt_entry = allocate_page() | PAGE_PRESENT | PAGE_WRITE;
  }
  uint64_t* pd_entry = ((uint64_t*) (*pdpt_entry & PAGE_NUMBER_MASK)) + vaddr.pd_index;
  if (*pd_entry == 0) {
    *pd_entry = allocate_page() | PAGE_PRESENT | PAGE_WRITE;
  }
  uint64_t* pt_entry = ((uint64_t*) (*pd_entry & PAGE_NUMBER_MASK)) + vaddr.pt_index;
  *pt_entry = (vaddr.value & PAGE_NUMBER_MASK) | PAGE_PRESENT | PAGE_WRITE;
}

// Good explanation of the concepts can be found here: https://back.engineering/23/08/2020/
void mem_identity_map_range(uint64_t start_addr, uint64_t end_addr) {
  for (uint64_t addr = start_addr; addr < end_addr; addr += PAGE_SIZE) {
    identity_map(addr);
  }
}

bool mem_find_range(uint64_t addr, MemRange* range) {
  for (multiboot_mmap_entry_t* mmap = _mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) _mem_map + _mem_map->size;
       mmap = (multiboot_mmap_entry_t*) ((uint64_t) mmap + _mem_map->entry_size)) {
    if (addr >= mmap->addr && addr < mmap->addr + mmap->len) {
      range->address = mmap->addr;
      range->size = mmap->len;
      range->type = mmap->type;
      return true;
    }
  }
  return false;
}

void mem_print_map() {
  printf("mem_lower = %dKB, mem_upper = %dKB\n", _mem_lower, _mem_upper);
  for (multiboot_mmap_entry_t* mmap = _mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) _mem_map + _mem_map->size;
       mmap = (multiboot_mmap_entry_t * )((uint64_t) mmap + _mem_map->entry_size)) {
    printf(" %0.12lx - %0.12lx (%,ld): %s\n",
           mmap->addr, mmap->addr + mmap->len, mmap->len,
           mem_type[mmap->type]);
  }
}