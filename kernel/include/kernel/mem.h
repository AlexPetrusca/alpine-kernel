#ifndef KERNEL_INCLUDE_KERNEL_MEM_H_
#define KERNEL_INCLUDE_KERNEL_MEM_H_

#include <stdint.h>
#include <stdbool.h>
#include <kernel/mb2_type.h>
#include "dll.h"

#define PAGE_SIZE 4096

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

typedef enum {
  MEMORY_AVAILABLE = 1,
  MEMORY_RESERVED = 2,
  MEMORY_ACPI_RECLAIMABLE = 3,
  MEMORY_NVS = 4,
  MEMORY_BADRAM = 5,
  MEMORY_PCI_ECAM = 6,
  MEMORY_USB_DEVICE = 7,
  MEMORY_HEAP = 8,
  MEMORY_FRAME_BUFFER = 9,
  MEMORY_ACPI = 10,
} mem_type;

typedef struct {
  uint64_t phys_addr;
  uint64_t virt_addr;
  uint64_t size;
  mem_type type;
} mem_range;

bool mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map);
void mem_identity_map_range(uint64_t phys_addr, uint64_t size, mem_type type);
bool mem_find_range(uint64_t addr, mem_range* range);
void mem_print_map();

#endif //KERNEL_INCLUDE_KERNEL_MEM_H_
