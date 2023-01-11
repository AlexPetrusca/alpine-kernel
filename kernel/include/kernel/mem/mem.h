#ifndef KERNEL_INCLUDE_KERNEL_MEM_H_
#define KERNEL_INCLUDE_KERNEL_MEM_H_

#include <stdint.h>
#include <stdbool.h>

#include <sys/dll.h>
#include <kernel/boot/mb2_type.h>

#define PAGE_SIZE 4096

typedef union {
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
  MEMORY_LAPIC = 11,
  MEMORY_STACKS = 12,
  MEMORY_EBDA = 13,
  MEMORY_MOTHERBOARD_BIOS = 14,
} mem_type;

typedef struct {
  uint64_t phys_addr;
  uint64_t virt_addr;
  uint64_t size;
  mem_type type;
} mem_range;

uint32_t mem_read_8(uint64_t addr);
uint32_t mem_read_16(uint64_t addr);
uint32_t mem_read_32(uint64_t addr);
uint64_t mem_read_64(uint64_t addr);

uint64_t mem_get_pml4_addr();
uint64_t mem_get_heap_addr();
void mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map);
bool mem_identity_map_range(uint64_t phys_addr, uint64_t size, mem_type type) __attribute__ ((warn_unused_result));
bool mem_find_range(uint64_t addr, mem_range* range) __attribute__ ((warn_unused_result));

void mem_print_map(int argc, char** argv);
void mem_print_pt(int argc, char** argv);
void mem_print_range(char* text, uint64_t addr, uint64_t size);

#endif //KERNEL_INCLUDE_KERNEL_MEM_H_
