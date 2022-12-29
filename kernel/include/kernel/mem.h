#ifndef KERNEL_INCLUDE_KERNEL_MEM_H_
#define KERNEL_INCLUDE_KERNEL_MEM_H_

#include <stdint.h>

#define PAGE_SIZE     4096

typedef union _VirtualAddress {
  uint64_t value;
  struct {
    uint64_t offset: 12;
    uint64_t pt_index: 9;
    uint64_t pd_index: 9;
    uint64_t pdpt_index: 9;
    uint64_t pml4_index: 9;
    uint64_t reserved: 16;
  };
} __attribute__((packed)) VirtualAddress;

typedef union _PML4Entry {
  uint64_t value;
  struct {
    uint64_t present: 1;            // Must be 1, region invalid if 0.
    uint64_t rw: 1;                 // If 0, writes not allowed.
    uint64_t user_supervisor: 1;    // If 0, user-mode accesses not allowed.
    uint64_t page_write_through: 1; // Determines the memory type used to access PDPT.
    uint64_t page_cache: 1;         // Determines the memory type used to access PDPT.
    uint64_t accessed: 1;           // If 0, this entry has not been used for translation.
    uint64_t ignored_1: 1;
    uint64_t page_size: 1;         // Must be 0 for PML4E.
    uint64_t ignored_2: 4;
    uint64_t pfn: 36;              // The page frame number of the PDPT of this PML4 entry.
    uint64_t reserved: 4;
    uint64_t ignored_3: 11;
    uint64_t nx: 1;                // If 1, instruction fetches not allowed.
  };
} __attribute__((packed)) PML4Entry;

typedef union _PDPTEntry {
  uint64_t value;
  struct {
    uint64_t present: 1;            // Must be 1, region invalid if 0.
    uint64_t rw: 1;                 // If 0, writes not allowed.
    uint64_t user_supervisor: 1;    // If 0, user-mode accesses not allowed.
    uint64_t page_write: 1;         // Determines the memory type used to access PD.
    uint64_t page_cache: 1;         // Determines the memory type used to access PD.
    uint64_t accessed: 1;           // If 0, this entry has not been used for translation.
    uint64_t ignored_1: 1;
    uint64_t page_size: 1;          // If 1, this entry maps a 1GB page.
    uint64_t ignored_2: 4;
    uint64_t pfn: 36;               // The page frame number of the PD of this PDPT entry.
    uint64_t reserved: 4;
    uint64_t ignored_3: 11;
    uint64_t nx: 1;                 // If 1, instruction fetches not allowed.
  };
}  __attribute__((packed)) PDPTEntry;

typedef union _PDEntry {
  uint64_t value;
  struct {
    uint64_t present: 1;            // Must be 1, region invalid if 0.
    uint64_t rw: 1;                 // If 0, writes not allowed.
    uint64_t user_supervisor: 1;    // If 0, user-mode accesses not allowed.
    uint64_t page_write: 1;         // Determines the memory type used to access PD.
    uint64_t page_cache: 1;         // Determines the memory type used to access PD.
    uint64_t accessed: 1;           // If 0, this entry has not been used for translation.
    uint64_t ignored_1: 1;
    uint64_t page_size: 1;          // If 1, this entry maps a 1GB page.
    uint64_t ignored_2: 4;
    uint64_t pfn: 36;               // The page frame number of the PT of this PD entry.
    uint64_t reserved: 4;
    uint64_t ignored_3: 11;
    uint64_t nx: 1;                 // If 1, instruction fetches not allowed.
  };
}  __attribute__((packed)) PDEntry;

typedef union _PTEntry {
  uint64_t value;
  struct {
    uint64_t present: 1;          // Must be 1, region invalid if 0.
    uint64_t rw: 1;               // If 0, writes not allowed.
    uint64_t user_supervisor: 1;  // If 0, user-mode accesses not allowed.
    uint64_t page_write: 1;       // Determines the memory type used to access the memory.
    uint64_t page_cache: 1;       // Determines the memory type used to access the memory.
    uint64_t accessed: 1;         // If 0, this entry has not been used for translation.
    uint64_t dirty: 1;            // If 0, the memory backing this page has not been written to.
    uint64_t page_access_type: 1; // Determines the memory type used to access the memory.
    uint64_t global: 1;           // If 1 and the PGE bit of CR4 is set, translations are global.
    uint64_t ignored_1: 3;
    uint64_t pfn: 36;             // The page frame number of the backing physical page.
    uint64_t reserved: 4;
    uint64_t ignored_2: 7;
    uint64_t protect_key: 4;      // If the PKE bit of CR4 is set, determines the protection key.
    uint64_t nx: 1;               // If 1, instruction fetches not allowed.
  };
}  __attribute__((packed)) PTEntry;

void mem_identity_map_range(uint64_t startAddr, uint64_t endAddr);

#endif //KERNEL_INCLUDE_KERNEL_MEM_H_
