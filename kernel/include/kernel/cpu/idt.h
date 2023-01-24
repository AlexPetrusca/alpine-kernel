#ifndef ALPINE_KERNEL_IDT_H
#define ALPINE_KERNEL_IDT_H

#include <stdint.h>
#include <kernel/cpu/isr.h>

typedef struct {
  uint16_t size;    // The size of the table in bytes subtracted by 1
  uint64_t offset;  // The linear address of the IDT (not the physical address, paging applies)
} __attribute__((packed)) idt_descriptor;
static_assert(sizeof(idt_descriptor) == 10, "");

typedef union {
  uint8_t value;
  struct {
    uint8_t gate_type: 4;   // 0xE = 64-bit Interrupt Gate and 0xF = 64-bit Trap Gate
    uint8_t : 1;            // reserved
    uint8_t privilege: 2;   // CPU Privilege Level bits
    uint8_t present: 1;     // Present bit (must be set to 1 for the descriptor to be valid)
  };
} __attribute__((packed)) idt_access_byte;
static_assert(sizeof(idt_access_byte) == 1, "");

typedef struct {
  uint16_t offset1: 16;      // The lower 16 bits of the ISR's address
  uint16_t selector: 16;     // A Segment Selector which must point to a valid code segment in the GDT
  uint8_t ist: 3;            // offset into the Interrupt Stack Table (set to 0 to not use IST)
  uint8_t reserved1: 5;      // reserved
  uint8_t access_ctrl: 8;    // segment access control bits
  uint64_t offset2: 48;      // The higher 48 bits of the ISR's address
  uint64_t reserved2: 32;    // reserved
} __attribute__((packed)) idt_entry;
static_assert(sizeof(idt_entry) == 16, "");

typedef struct {
  uint16_t length;
  idt_entry* entries;
} idt_handle;

bool idt_init();
idt_descriptor* idt_get_descriptor();
uint64_t idt_entry_get_offset(idt_entry* entry);
idt_entry idt_create_entry(uint64_t isr_addr, uint16_t seg_select, idt_access_byte access_byte);

void idt_print_info(int argc, char** argv);
void idt_interrupt(int argc, char** argv);

#endif //ALPINE_KERNEL_IDT_H
