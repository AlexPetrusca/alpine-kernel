#include <stdio.h>
#include <assert.h>

#include <kernel/mem/mem.h>
#include <kernel/mem/heap.h>
#include <kernel/mem/pgm.h>
#include <kernel/cpu/asm.h>
#include <kernel/boot/elf.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/process.h>

#define PTE_PAGE_NUMBER_MASK_4K   0x0000FFFFFFFFF000   /* bits [12-47]*/
#define PTE_PAGE_NUMBER_MASK_2M   0x0000FFFFFFE00000   /* bits [21-47]*/
#define PTE_PAGE_PRESENT          (1 << 0)
#define PTE_PAGE_WRITE            (1 << 1)
#define PTE_PAGE_SIZE_2M          (1 << 7)

#define PML4T_START         0x3000
#define HEAP_SIZE           (512 * PAGE_SIZE_4K)

typedef struct {
  dll_node node;
  uint64_t phys_addr;
  uint64_t virt_addr;
  uint64_t size;
  mem_type type;
} mem_range_node;

bool mem_inited = false;
#define CHECK_INIT(def)  try(mem_inited, def, "Memory subsystem not initialized")

uint32_t _mem_lower = 0;
uint32_t _mem_upper = 0;
dll_list _mem_map;
uint64_t _mem_low_pgm_handle;
uint64_t _mem_main_pgm_handle;

char* _mem_type[] = {
  "", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad", "PCIe Config", "USB (xHCI)", "Kernel Heap",
  "Frame Buffer", "ACPI", "LAPIC", "Process Stacks", "EBDA", "Motherboard BIOS", "SMP Trampoline", "Page Table",
  "Page Manager", "Kernel"
};

uint32_t mem_read_8(uint64_t addr) {
  return *((uint8_t*) addr);
}

uint32_t mem_read_16(uint64_t addr) {
  return *((uint16_t*) addr);
}

uint32_t mem_read_32(uint64_t addr) {
  return *((uint32_t*) addr);
}

uint64_t mem_read_64(uint64_t addr) {
  return *((uint64_t*) addr);
}

mb2_mmap_entry* mem_find(mb2_tag_mmap* mem_map, uint64_t addr) {
  for (mb2_mmap_entry* mmap = mem_map->entries;
       (uint8_t*) mmap < (uint8_t*) mem_map + mem_map->size;
       mmap = (mb2_mmap_entry*) ((uint64_t) mmap + mem_map->entry_size)) {
    if (addr >= mmap->addr && addr < mmap->addr + mmap->len) {
      return mmap;
    }
  }
  return NULL;
}

uint64_t mem_get_pml4_addr() {
  return PML4T_START;
}

mem_range_node* mem_find_range_internal(uint64_t addr) {
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    if (addr >= r->phys_addr && addr < r->phys_addr + r->size) {
      return r;
    }
  }
  return false;
}

void mem_mark_range(uint64_t addr, mem_type type) {
  mem_range_node* range = mem_find_range_internal(addr);
  if (range != NULL) {
    range->type = type;
  }
}

uint64_t mem_prev_page_addr_4k(uint64_t addr) {
  return addr & PTE_PAGE_NUMBER_MASK_4K;
}

uint64_t mem_prev_page_addr_2m(uint64_t addr) {
  return addr & PTE_PAGE_NUMBER_MASK_2M;
}

uint64_t mem_next_page_addr(uint64_t addr) {
  return (addr + PAGE_SIZE_4K - 1) & PTE_PAGE_NUMBER_MASK_4K;
}

void mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map) {
  assert(mem_map != NULL, "Memory map not provided.");
  assert(basic_meminfo != NULL, "Basic memory info not provided.");
  _mem_lower = basic_meminfo->mem_lower;
  _mem_upper = basic_meminfo->mem_upper;

  mem_inited = true;

  // initialize the low page manager
  long pgm_start_addr = PML4T_START - PAGE_SIZE_4K;
  mb2_mmap_entry* lower_mem = mem_find(mem_map, pgm_start_addr);
  long pgm_end_addr = mem_prev_page_addr_4k(lower_mem->addr + lower_mem->len);
  _mem_low_pgm_handle = pgm_init(pgm_start_addr, pgm_end_addr - pgm_start_addr);
  pgm_allocate_page(_mem_low_pgm_handle); // pml4
  pgm_allocate_page(_mem_low_pgm_handle); // pdpt
  pgm_allocate_page(_mem_low_pgm_handle); // pdt

  // initialize the main page manager
  mb2_mmap_entry* main_mem = mem_find(mem_map, MAIN_MEM_START);
  assert(main_mem != NULL, "Could not find main memory region.");

  // for some reason one more page has to be allocated for the kernel, otherwise it fails
  uint64_t main_pgm_start_addr = mem_next_page_addr(elf_get_max_kernel_addr()) + PAGE_SIZE_4K;
  uint64_t main_pgm_end_addr = main_mem->addr + main_mem->len;
  long main_pgm_size = main_pgm_end_addr - main_pgm_start_addr;
  _mem_main_pgm_handle = pgm_init(main_pgm_start_addr, main_pgm_size);

  // initialize the heap
  heap_init(HEAP_VIRTUAL_ADDR, HEAP_SIZE);

  // save the memory map
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

  // mark some memory areas
  mem_mark_range(0xA0000 - 1, MEMORY_EBDA);         // EBDA, if exists, is just under 0xA0000
  mem_mark_range(0xF0000, MEMORY_MOTHERBOARD_BIOS); // The BIOS ROM is at 0xF0000
  mem_update_range(0x1000, 0x1000, PAGE_SIZE_4K, MEMORY_SMP_TRAMPOLINE);
  mem_mark_range(0x2000, MEMORY_PAGE_TABLE);
  mem_update_range(MAIN_MEM_START, MAIN_MEM_START, main_pgm_start_addr - MAIN_MEM_START, MEMORY_KERNEL);
  mem_update_range(main_pgm_start_addr, main_pgm_start_addr, main_pgm_size, MEMORY_PGM);
  mem_update_range(HEAP_VIRTUAL_ADDR, HEAP_VIRTUAL_ADDR, HEAP_SIZE, MEMORY_HEAP);
}

uint64_t allocate_page() {
  uint64_t page_ptr = pgm_allocate_page(_mem_low_pgm_handle);
  memset((void*) page_ptr, 0, PAGE_SIZE_4K);
  return page_ptr;
}

void mem_map_page(uint64_t virt_addr, uint64_t page_addr, bool use_2m_page) {
  VirtualAddress vaddr = {virt_addr};
  uint64_t* pml4t_entry = ((uint64_t*) PML4T_START) + vaddr.pml4t_index;
  if (*pml4t_entry == 0) {
    *pml4t_entry = allocate_page() | PTE_PAGE_PRESENT | PTE_PAGE_WRITE;
  }
  uint64_t* pdpt_entry = ((uint64_t*) (*pml4t_entry & PTE_PAGE_NUMBER_MASK_4K)) + vaddr.pdpt_index;
  if (*pdpt_entry == 0) {
    *pdpt_entry = allocate_page() | PTE_PAGE_PRESENT | PTE_PAGE_WRITE;
  }
  uint64_t* pdt_entry = ((uint64_t*) (*pdpt_entry & PTE_PAGE_NUMBER_MASK_4K)) + vaddr.pdt_index;
  if (use_2m_page) {
    *pdt_entry = (page_addr & PTE_PAGE_NUMBER_MASK_2M) | PTE_PAGE_PRESENT | PTE_PAGE_WRITE | PTE_PAGE_SIZE_2M;
  } else {
    if (*pdt_entry == 0) {
      *pdt_entry = allocate_page() | PTE_PAGE_PRESENT | PTE_PAGE_WRITE;
    }
    uint64_t* pt_entry = ((uint64_t*) (*pdt_entry & PTE_PAGE_NUMBER_MASK_4K)) + vaddr.pt_index;
    *pt_entry = (page_addr & PTE_PAGE_NUMBER_MASK_4K) | PTE_PAGE_PRESENT | PTE_PAGE_WRITE;
  }
}

bool mem_map_range_4k(uint64_t phys_addr, uint64_t virt_addr, uint64_t size) {
  CHECK_INIT(false);
  assert(mem_prev_page_addr_4k(phys_addr) == phys_addr, "Physical address of mapped range must be a 4K page boundary");
  assert(mem_prev_page_addr_4k(virt_addr) == phys_addr, "Virtual address of mapped range must be a 4K page boundary");
  assert(size % PAGE_SIZE_4K == 0, "Mapped range must contain an integer number of 4K pages");
  for (uint64_t pa = phys_addr, va = virt_addr; pa < phys_addr + size; pa += PAGE_SIZE_4K, va += PAGE_SIZE_4K) {
    mem_map_page(va, pa, false);
  }
  return true;
}

bool mem_map_range_2m(uint64_t phys_addr, uint64_t virt_addr, uint64_t size) {
  CHECK_INIT(false);
  assert(mem_prev_page_addr_2m(phys_addr) == phys_addr, "Physical address of mapped range must be a 2M page boundary");
  assert(mem_prev_page_addr_2m(virt_addr) == phys_addr, "Virtual address of mapped range must be a 2M page boundary");
  assert(size % PAGE_SIZE_2M == 0, "Mapped range must contain an integer number of 2M pages");
  for (uint64_t pa = phys_addr, va = virt_addr; pa < phys_addr + size; pa += PAGE_SIZE_2M, va += PAGE_SIZE_2M) {
    mem_map_page(va, pa, true);
  }
  return true;
}

bool mem_allocate_range(uint64_t virt_addr, uint64_t size) {
  CHECK_INIT(false);
  assert(mem_prev_page_addr_4k(virt_addr) == virt_addr, "Mapped range must start at 4K page boundary");
  assert(size % PAGE_SIZE_4K == 0, "Mapped range must contain an integer number of 4K pages");
  for (uint64_t addr = virt_addr; addr < virt_addr + size; addr += PAGE_SIZE_4K) {
    mem_map_page(addr, pgm_allocate_page(_mem_main_pgm_handle), false);
  }
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

bool mem_find_range_by_type(mem_type type, mem_range* range) {
  CHECK_INIT(false);
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    if (type == r->type) {
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

bool mem_update_range(uint64_t phys_addr, uint64_t virt_addr, uint64_t size, mem_type type) {
  mem_range_node range2 = {.phys_addr = phys_addr, .virt_addr = virt_addr, .size = size, .type = type};
  mem_range_node* range = &range2;
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
  mem_range_node* nr = (mem_range_node*) kmalloc(sizeof(mem_range_node));
  *nr = *range;
  dll_add_tail(&_mem_map, (dll_node*) nr);
  return false;
}

void mem_print_map(__unused int argc, __unused char** argv) {
  CHECK_INIT();
  printf(" %12s %12s %16s %12s %8s  %s\n", "Phys Start", "Phys End", "Virtual Addr", "Size", "Pages", "Type");
  printf(" %12s %12s %16s %12s %8s  %s\n", "==========", "========", "============", "====", "=====", "====");
  for (mem_range_node* r = (mem_range_node*) _mem_map.head; r != NULL; r = (mem_range_node*) r->node.next) {
    printf(" %12lx %12lx %16lx %12ld %8ld  %s\n",
           r->phys_addr,
           r->phys_addr + r->size,
           r->virt_addr == MAX_VIRTUAL_ADDR ? 0 : r->virt_addr,
           r->size,
           r->size / PAGE_SIZE_4K,
           _mem_type[r->type]);
  }
  printf(" lower %dKB, upper %dKB\n", _mem_lower, _mem_upper);
}

void mem_print_pt_pgm(__unused int argc, __unused char** argv) {
  CHECK_INIT();
  printf("Pages: %ld, Used: %ld, Free: %ld\n",
         pgm_total_pages(_mem_low_pgm_handle),
         pgm_used_pages(_mem_low_pgm_handle),
         pgm_free_pages(_mem_low_pgm_handle));
}

void mem_print_main_pgm(__unused int argc, __unused char** argv) {
  CHECK_INIT();
  printf("Pages: %ld, Used: %ld, Free: %ld\n",
         pgm_total_pages(_mem_main_pgm_handle),
         pgm_used_pages(_mem_main_pgm_handle),
         pgm_free_pages(_mem_main_pgm_handle));
}

void mem_memdump(int argc, char** argv) {
  if (argc == 3) {
    unsigned char* addr = (unsigned char*) strtoull(argv[1], NULL, 16);
    uint64_t size = strtoull(argv[2], NULL, 10);
    for (uint64_t i = 0; i < size; i++) {
      printf("%.2x ", addr[i]);
    }
    printf("\n");
  } else {
    printf("Usage: memdump [addr] [size]");
  }
}

void mem_handle_page_fault(uint64_t virt_addr) {
  mem_map_page(mem_prev_page_addr_4k(virt_addr), pgm_allocate_page(_mem_main_pgm_handle), false);
}
