#include <kernel/mem.h>
#include <stdio.h>
#include <stddef.h>
#include <kernel/multiboot2.h>
#include <kernel/panic.h>

#define PAGE_NUMBER_MASK 0x0000FFFFFFFFF000
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITE      (1 << 1)

#define PML4T_START   0x1000
#define PDPT_START    0x2000
#define PDT_START     0x3000
#define PT_START      0x4000

#define MAX_VIRTUAL_ADDR   0xFFFFFFFFFFFFFFFF

uint64_t _next_page_pointer = PT_START + PAGE_SIZE;

uint32_t _mem_lower = 0;
uint32_t _mem_upper = 0;
mem_range _mem_map[64] = {{0, 0, 0, 0}};
int _mem_map_next = 0;
char* _mem_type[] = {"", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad", "PCI ECAM"};

void mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map) {
  if (mem_map == NULL) {
    panic("Memory map not provided.");
  }
  if (basic_meminfo == NULL) {
    panic("Basic memory info not provided.");
  }
  _mem_lower = basic_meminfo->mem_lower;
  _mem_upper = basic_meminfo->mem_upper;

  for (mb2_mmap_entry_t* mmap = mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) mem_map + mem_map->size;
       mmap = (mb2_mmap_entry_t*) ((uint64_t) mmap + mem_map->entry_size)) {
    mem_range* range = &_mem_map[_mem_map_next++];
    range->phys_addr = mmap->addr;
    range->virt_addr = MAX_VIRTUAL_ADDR;
    range->size = mmap->len;
    range->type = mmap->type;
  }
}

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
void mem_identity_map_range(mem_range* range) {
  for (uint64_t addr = range->phys_addr; addr < range->phys_addr + range->size; addr += PAGE_SIZE) {
    identity_map(addr);
  }
  range->virt_addr = range->phys_addr;
  mem_update_range(range);
}

bool mem_find_range(uint64_t addr, mem_range* range) {
  for (int i = 0; i < _mem_map_next; i++) {
    mem_range* r = &_mem_map[i];
    if (addr >= r->phys_addr && addr < r->phys_addr + r->size) {
      *range = *r;
      return true;
    }
  }
  return false;
}

bool mem_update_range(mem_range* range) {
  for (int i = 0; i < _mem_map_next; i++) {
    mem_range* r = &_mem_map[i];
    if (range->phys_addr == r->phys_addr && range->size == r->size) {
      *r = *range;
      return true;
    }
  }
  return false;
}

void mem_print_map() {
  printf(" Phys Start   Phys End     Virtual Addr     Size\n");
  for (int i = 0; i < _mem_map_next; i++) {
    mem_range* range = &_mem_map[i];
    printf(" %0.12lx %0.12lx %0.16lx %.12ld %s\n",
           range->phys_addr, range->phys_addr + range->size,
           range->virt_addr,
           range->size,
           _mem_type[range->type]);
  }
  printf(" lower %dKB, upper %dKB\n", _mem_lower, _mem_upper);
}