#include <kernel/mem.h>

#define PML4T_START   0x1000
#define PDPT_START    0x2000
#define PDT_START     0x3000
#define PT_START      0x4000
uint64_t _next_page_pointer = PT_START + PAGE_SIZE;

uint64_t allocate_page() {
  uint64_t* p = (uint64_t*) _next_page_pointer;
  _next_page_pointer += PAGE_SIZE;
  for (int i = 0; i < PAGE_SIZE / 8; i++) {
    *p++ = 0;
  }
  return (uint64_t) p >> 12;
}

uint64_t map_pt(VirtualAddress addr, uint64_t pfn) {
  PTEntry* entry = ((PTEntry*) (pfn << 12)) + addr.pt_index;
  entry->present = 1;             // Must be 1, region invalid if 0.
  entry->rw = 1;                  // If 0, writes not allowed.
  entry->pfn = addr.value >> 12; // The page frame number of the backing physical page, identity mapping.
  return 0;
}

uint64_t map_pd(VirtualAddress addr, uint64_t pfn) {
  PDEntry* entry = ((PDEntry*) (pfn << 12)) + addr.pd_index;
  if (entry->value == 0) {
    entry->present = 1;           // Must be 1, region invalid if 0.
    entry->rw = 1;                // If 0, writes not allowed.
    entry->page_size = 0;         // 0 - 4kB page, 1 - 1GB page.
    entry->pfn = allocate_page();    // The page frame number of the PT.
  }
  return map_pt(addr, entry->pfn);
}

uint64_t map_pdpt(VirtualAddress addr, uint64_t pfn) {
  PDPTEntry* entry = ((PDPTEntry*) (pfn << 12)) + addr.pdpt_index;
  if (entry->value == 0) {
    entry->present = 1;           // Must be 1, region invalid if 0.
    entry->rw = 1;                // If 0, writes not allowed.
    entry->page_size = 0;         // 0 - 4kB page, 1 - 1GB page.
    entry->pfn = allocate_page();    // The page frame number of the PD.
  }
  return map_pd(addr, entry->pfn);
}

uint64_t map_pml4(VirtualAddress addr) {
  PML4Entry* entry = ((PML4Entry*) PML4T_START) + addr.pml4_index;
  if (entry->value == 0) {
    entry->present = 1;             // Must be 1, region invalid if 0.
    entry->rw = 1;                  // If 0, writes not allowed.
    entry->pfn = allocate_page();   // The page frame number of the PDPT.
  }
  return map_pdpt(addr, entry->pfn);
}

void mem_identity_map_range(uint64_t startAddr, uint64_t endAddr) {
  for (uint64_t addr = startAddr; addr < endAddr; addr += PAGE_SIZE) {
    VirtualAddress vaddr = {addr};
    map_pml4(vaddr);
  }
}

//  VirtualAddress addr1 = {0xFFDD000};
//  printf("PML4 %d, PDPT %d, PD %d, PT %d, offset %d\n",
//         addr1.pml4_index, addr1.pdpt_index, addr1.pd_index, addr1.pt_index, addr1.offset);
