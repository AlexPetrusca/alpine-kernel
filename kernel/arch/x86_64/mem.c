#include <kernel/mem.h>
#include <stdio.h>
#include <assert.h>
#include "kernel/heap.h"

#define PAGE_NUMBER_MASK 0x0000FFFFFFFFF000
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITE      (1 << 1)

#define PML4T_START   0x1000
#define PDPT_START    0x2000
#define PDT_START     0x3000
#define PT_START      0x4000

#define MAX_VIRTUAL_ADDR    0xFFFFFFFFFFFFFFFF
#define MAIN_MEM_START      0x100000 // todo: define known ranges in mem.h
#define HEAP_SIZE           512 * PAGE_SIZE

uint64_t _next_page_pointer = PT_START + PAGE_SIZE;

uint32_t _mem_lower = 0;
uint32_t _mem_upper = 0;
dequeue _mem_map;
char* _mem_type[] = {"", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad", "PCI ECAM", "Heap"};

void identity_map(uint64_t addr);

mb2_mmap_entry* mem_find_main_mem(mb2_tag_mmap* mem_map) {
  for (mb2_mmap_entry* mmap = mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) mem_map + mem_map->size;
       mmap = (mb2_mmap_entry*) ((uint64_t) mmap + mem_map->entry_size)) {
    if (mmap->addr == MAIN_MEM_START) {
      return mmap;
    }
  }
  return NULL;
}

bool mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map) {
  assert(mem_map != NULL, "Memory map not provided.");
  assert(basic_meminfo != NULL, "Basic memory info not provided.");
  _mem_lower = basic_meminfo->mem_lower;
  _mem_upper = basic_meminfo->mem_upper;

  mb2_mmap_entry* main_mem = mem_find_main_mem(mem_map);
  assert(main_mem != NULL, "Could not find main memory region.");
  uint64_t heap_addr = main_mem->addr + main_mem->len - HEAP_SIZE;
  for (uint64_t addr = heap_addr; addr < heap_addr + HEAP_SIZE; addr += PAGE_SIZE) {
    identity_map(addr); // todo: don't identity map
  }
  heap_init(heap_addr, HEAP_SIZE);

  for (mb2_mmap_entry* mmap = mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) mem_map + mem_map->size;
       mmap = (mb2_mmap_entry*) ((uint64_t) mmap + mem_map->entry_size)) {
    mem_range* range = (mem_range*) kmalloc(sizeof(mem_range));
    range->phys_addr = mmap->addr;
    range->virt_addr = MAX_VIRTUAL_ADDR;
    range->size = mmap->len;
    range->type = mmap->type;
    dq_add_tail(&_mem_map, (dq_node*) range);
  }

  mem_range heap_mem_range = {.phys_addr = heap_addr, .size = HEAP_SIZE};
  heap_mem_range.type = HEAP;
  heap_mem_range.virt_addr = heap_addr;
  mem_update_range(&heap_mem_range);
  return  true;
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
  for (mem_range* r = (mem_range*) _mem_map.head; r != NULL; r = (mem_range*) r->node.next) {
    if (addr >= r->phys_addr && addr < r->phys_addr + r->size) {
      *range = *r;
      return true;
    }
  }
  return false;
}

bool mem_update_range(mem_range* range) {
  for (mem_range* r = (mem_range*) _mem_map.head; r != NULL; r = (mem_range*) r->node.next) {
    if (range->phys_addr >= r->phys_addr && range->phys_addr + range->size <= r->phys_addr + r->size) {
      uint64_t addr1 = r->phys_addr, addr2 = range->phys_addr, addr3 = range->phys_addr + range->size,
        addr4 = r->phys_addr + r->size;
      if (addr4 > addr3) { // add the suffix range
        mem_range* nr = (mem_range*) kmalloc(sizeof(mem_range));
        *nr = *r;
        nr->phys_addr = addr3;
        nr->size = addr4 - addr3;
        dq_add_after((dq_node*) r, (dq_node*) nr);
      }
      { // add the main range
        mem_range* nr = (mem_range*) kmalloc(sizeof(mem_range));
        *nr = *range;
        dq_add_after((dq_node*) r, (dq_node*) nr);
      }
      if (addr2 > addr1) { // add the prefix range
        mem_range* nr = (mem_range*) kmalloc(sizeof(mem_range));
        *nr = *r;
        nr->size = addr2 - addr1;
        dq_add_after((dq_node*) r, (dq_node*) nr);
      }
      dq_remove((dq_node*) r); // remove old range
      return true;
    }
  }
  return false;
}

void mem_print_map() {
  printf(" Phys Start   Phys End     Virtual Addr     Size\n");
  for (mem_range* r = (mem_range*) _mem_map.head; r != NULL; r = (mem_range*) r->node.next) {
    printf(" %0.12lx %0.12lx %0.16lx %.12ld %s\n",
           r->phys_addr, r->phys_addr + r->size,
           r->virt_addr,
           r->size,
           _mem_type[r->type]);
  }
  printf(" lower %dKB, upper %dKB\n", _mem_lower, _mem_upper);
}