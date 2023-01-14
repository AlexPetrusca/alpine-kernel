#ifndef ALPINE_KERNEL_GDT_H
#define ALPINE_KERNEL_GDT_H

#include <stdint.h>

// https://wiki.osdev.org/Global_Descriptor_Table

typedef struct {
  uint16_t size;    // The length of the table in bytes subtracted by 1
  uint64_t offset;  // The linear address of the GDT (not the physical address, paging applies)
} gdt_descriptor;

typedef union {
  uint8_t value;
  struct {
    uint8_t accessed: 1;              // Best left clear (0), the CPU will set it when the segment is accessed
    uint8_t read_write: 1;            // For data segments: Writeable bit. For code segments: Readable bit.
    uint8_t direction_conforming: 1;  // For data segments: Direction bit. For code segments: Conforming bit.
    uint8_t executable: 1;            // If clear (0) the descriptor defines a data segment. If set (1) it defines a code segment.
    uint8_t type: 1;                  // If clear (0) the descriptor defines a system segment. If set (1) it defines a code or data segment.
    uint8_t privilege: 2;             // CPU Privilege level of the segment
    uint8_t present: 1;               // Must be set (1) for any valid segment.
  };
} __attribute__((packed)) gdt_access_byte;

typedef union {
  uint8_t value;
  struct {
    uint8_t _reserved: 1;
    uint8_t long_mode: 1;     // If set (1), the descriptor defines a 64-bit code segment. When set, DB should always be clear (0).
    uint8_t size: 1;          // If set (1) the descriptor defines a 32-bit protected mode segment, else it defines a 16-bit protected mode segment.
    uint8_t granularity: 1;   // If set (1), the Limit is in 4 KiB blocks (page granularity), else it is in 1 Byte blocks (byte granularity).
  };
} __attribute__((packed)) gdt_flags;

typedef union {
  uint64_t value;
  struct {
    uint64_t limit1: 16;      // limit: A 20-bit value, tells the maximum addressable unit
    uint64_t base1: 24;       // base: A 32-bit value containing the linear address where the segment begins
    uint64_t access_ctrl: 8;  // segment access control bits
    uint64_t limit2: 4;
    uint64_t flags: 4;         // segment flag bits
    uint64_t base2: 8;
  };
} __attribute__((packed)) gdt_entry;

typedef struct {
  uint16_t length;
  gdt_entry* entries;
} gdt_handle;

gdt_handle gdt_get_handle();
gdt_descriptor gdt_get_descriptor();
uint32_t gdt_entry_get_limit(gdt_entry* entry);
uint32_t gdt_entry_get_base(gdt_entry* entry);

void gdt_print_info(int argc, char** argv);

#endif //ALPINE_KERNEL_GDT_H
