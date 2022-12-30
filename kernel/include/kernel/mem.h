#ifndef KERNEL_INCLUDE_KERNEL_MEM_H_
#define KERNEL_INCLUDE_KERNEL_MEM_H_

#include <stdint.h>
#include <stdbool.h>

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

typedef struct {
  uint64_t address;
  uint64_t size;
  uint32_t type;
} mem_range;

void mem_identity_map_range(uint64_t start_addr, uint64_t end_addr);
bool mem_find_range(uint64_t addr, mem_range* range);
void mem_print_map();

extern uint32_t _mem_lower;
extern uint32_t _mem_upper;
extern struct mb2_tag_mmap* _mem_map;

#endif //KERNEL_INCLUDE_KERNEL_MEM_H_
