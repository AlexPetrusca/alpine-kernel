#ifndef KERNEL_INCLUDE_KERNEL_MEM_H_
#define KERNEL_INCLUDE_KERNEL_MEM_H_

#include <stdint.h>
#include <stdbool.h>

#include <sys/dll.h>
#include <kernel/boot/mb2_type.h>
#include <kernel/cpu/isr.h>

#define PAGE_SIZE_4K        4096L
#define PAGE_SIZE_2M        2 * 1024L * 1024L
#define MAIN_MEM_START      0x100000

#define HEAP_VIRTUAL_ADDR   0x0000010000000000
#define STACK_VIRTUAL_ADDR  0x0000020000000000
#define MAX_VIRTUAL_ADDR    0xFFFFFFFFFFFFFFFF

typedef union {
  uint64_t value;
  struct {
    uint64_t offset: 12;
    uint64_t pt_index: 9;
    uint64_t pdt_index: 9;
    uint64_t pdpt_index: 9;
    uint64_t pml4t_index: 9;
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
  MEMORY_SMP_TRAMPOLINE = 15,
  MEMORY_PAGE_TABLE = 16,
  MEMORY_PGM = 17,
  MEMORY_KERNEL = 18,
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
uint64_t mem_prev_page_addr_4k(uint64_t addr);
uint64_t mem_prev_page_addr_2m(uint64_t addr);

#define err __attribute__((warn_unused_result))

void mem_init(mb2_tag_basic_meminfo* basic_meminfo, mb2_tag_mmap* mem_map);
bool mem_map_range_4k(uint64_t phys_addr, uint64_t virt_addr, uint64_t size) err;
bool mem_map_range_2m(uint64_t phys_addr, uint64_t virt_addr, uint64_t size) err;
bool mem_allocate_range(uint64_t virt_addr, uint64_t size) err;
bool mem_update_range(uint64_t phys_addr, uint64_t virt_addr, uint64_t size, mem_type type);
bool mem_find_range(uint64_t addr, mem_range* range) err;
bool mem_find_range_by_type(mem_type type, mem_range* range);
uint64_t mem_get_pml4_addr();

void mem_print_map(int argc, char** argv);
void mem_print_pt_pgm(int argc, char** argv);
void mem_print_main_pgm(int argc, char** argv);
void mem_memdump(int argc, char** argv);

void mem_page_fault_handler(interrupt_frame* frame);

#endif //KERNEL_INCLUDE_KERNEL_MEM_H_
