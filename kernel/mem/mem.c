#include <stdio.h>
#include <assert.h>

#include <kernel/mem/mem.h>
#include <kernel/mem/heap.h>

#define PAGE_NUMBER_MASK 0x0000FFFFFFFFF000
#define PAGE_PRESENT    (1 << 0)
#define PAGE_WRITE      (1 << 1)

#define PML4T_START   0xA000
#define PDPT_START    0xB000
#define PDT_START     0xC000
#define PT_START      0xD000

#define MAX_VIRTUAL_ADDR    0xFFFFFFFFFFFFFFFF
#define MAIN_MEM_START      0x100000 // todo: define known ranges in mem.h
#define HEAP_SIZE           512 * PAGE_SIZE

typedef struct {
  dll_node node;
  uint64_t phys_addr;
  uint64_t virt_addr;
  uint64_t size;
  mem_type type;
} mem_range_node;

bool mem_inited = false;
#define CHECK_INIT(def)  try(mem_inited, def, "Memory subsystem not initialized")

uint64_t _next_page_pointer = PT_START + PAGE_SIZE;

uint32_t _mem_lower = 0;
uint32_t _mem_upper = 0;
dll_list _mem_map;
uint64_t _mem_heap_addr;
char* _mem_type[] = {"", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad", "PCIe Config",
                     "USB (xHCI)", "Kernel Heap", "Frame Buffer", "ACPI", "LAPIC", "Process Stacks"};

void identity_map(uint64_t addr);
bool mem_update_range(mem_range_node* range);

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

uint64_t mem_get_pml4_addr() {
  return PML4T_START;
}

uint64_t mem_get_heap_addr() {
  return _mem_heap_addr;
}

void mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map) {
  assert(mem_map != NULL, "Memory map not provided.");
  assert(basic_meminfo != NULL, "Basic memory info not provided.");
  _mem_lower = basic_meminfo->mem_lower;
  _mem_upper = basic_meminfo->mem_upper;

  // initialize the heap
  mb2_mmap_entry* main_mem = mem_find_main_mem(mem_map);
  assert(main_mem != NULL, "Could not find main memory region.");
  _mem_heap_addr = main_mem->addr + main_mem->len - HEAP_SIZE;
  for (uint64_t addr = _mem_heap_addr; addr < _mem_heap_addr + HEAP_SIZE; addr += PAGE_SIZE) {
    identity_map(addr); // todo: don't identity map
  }
  heap_init(_mem_heap_addr, HEAP_SIZE);

  //  save the memory map
  for (mb2_mmap_entry* mmap = mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) mem_map + mem_map->size;
       mmap = (mb2_mmap_entry*) ((uint64_t) mmap + mem_map->entry_size)) {
    mem_range_node* range = (mem_range_node*) kmalloc(sizeof(mem_range_node));
    range->phys_addr = mmap->addr;
    range->virt_addr = MAX_VIRTUAL_ADDR;
    range->size = mmap->len;
    range->type = mmap->type;
    dll_add_tail(&_mem_map, (dll_node*) range);
  }

  // register the heap in the memory map
  mem_range_node heap_mem_range = {.phys_addr = _mem_heap_addr, .size = HEAP_SIZE};
  heap_mem_range.type = MEMORY_HEAP;
  heap_mem_range.virt_addr = _mem_heap_addr;
  mem_update_range(&heap_mem_range);

  mem_inited = true;
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
bool mem_identity_map_range(uint64_t phys_addr, uint64_t size, mem_type type) {
  CHECK_INIT(false);
  for (uint64_t addr = phys_addr; addr < phys_addr + size; addr += PAGE_SIZE) {
    identity_map(addr);
  }
  mem_range_node range = {.phys_addr = phys_addr, .virt_addr = phys_addr, .size = size, .type = type};
  mem_update_range(&range);
  return true;
}

bool mem_find_range(uint64_t addr, mem_range* range) {
  CHECK_INIT(false);
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    if (addr >= r->phys_addr && addr < r->phys_addr + r->size) {
      range->phys_addr = r->phys_addr;
      range->virt_addr = r->virt_addr;
      range->size = r->size;
      range->type = r->type;
      return true;
    }
  }
  return false;
}

bool mem_contains(mem_range_node* r1, mem_range_node* r2) {
  uint64_t x1 = r1->phys_addr, x2 = r1->phys_addr + r1->size;
  uint64_t y1 = r2->phys_addr, y2 = r2->phys_addr + r2->size;
  return y1 >= x1 && y2 <= x2;
}

bool mem_intersect(mem_range_node* r1, mem_range_node* r2) {
  uint64_t x1 = r1->phys_addr, x2 = r1->phys_addr + r1->size;
  uint64_t y1 = r2->phys_addr, y2 = r2->phys_addr + r2->size;
  return (y1 >= x1 && y1 < x2) || (y2 >= x1 && y2 < x2);
}

bool mem_update_range(mem_range_node* range) {
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    if (mem_intersect(r, range)) {
      if (!mem_contains(r, range)) {
        panic("Memory range not fully contained in existing range.");
      } else {
        uint64_t addr1 = r->phys_addr, addr2 = range->phys_addr, addr3 = range->phys_addr + range->size,
          addr4 = r->phys_addr + r->size;
        if (addr4 > addr3) { // add the suffix range
          mem_range_node* nr = (mem_range_node*) kmalloc(sizeof(mem_range_node));
          *nr = *r;
          nr->phys_addr = addr3;
          nr->size = addr4 - addr3;
          dll_add_after((dll_node*) r, (dll_node*) nr);
        }
        { // add the main range
          mem_range_node* nr = (mem_range_node*) kmalloc(sizeof(mem_range_node));
          *nr = *range;
          dll_add_after((dll_node*) r, (dll_node*) nr);
        }
        if (addr2 > addr1) { // add the prefix range
          mem_range_node* nr = (mem_range_node*) kmalloc(sizeof(mem_range_node));
          *nr = *r;
          nr->size = addr2 - addr1;
          dll_add_after((dll_node*) r, (dll_node*) nr);
        }
        dll_remove((dll_node*) r); // remove old range
        kfree(r);
        return true;
      }
    }
  }
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    if (range->phys_addr + range->size <= r->phys_addr) {
      mem_range_node* nr = (mem_range_node*) kmalloc(sizeof(mem_range_node));
      *nr = *range;
      dll_add_before((dll_node*) r, (dll_node*) nr);
      return true;
    }
  }
  return false;
}

void mem_print_map(__unused int argc, __unused char** argv) {
  CHECK_INIT();
  printf(" Phys Start   Phys End     Virtual Addr     Size\n");
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    printf(" %0.12lx %0.12lx %0.16lx %.12ld %s\n",
           r->phys_addr, r->phys_addr + r->size,
           r->virt_addr,
           r->size,
           _mem_type[r->type]);
  }
  printf(" lower %dKB, upper %dKB\n", _mem_lower, _mem_upper);
}

void mem_print_pt(__unused int argc, __unused char** argv) {
  CHECK_INIT();
  printf("PT 0x%lx - 0x%lx\n", (uint64_t)PT_START, _next_page_pointer);
}

void mem_print_range(char* text, uint64_t addr, uint64_t size) {
  printf("%s: ", text);
  uint8_t* p = (uint8_t*) addr;
  for (uint64_t i = 0; i < size; i++) {
    printf("%.2x ", p[i]);
  }
  printf("\n");
}
