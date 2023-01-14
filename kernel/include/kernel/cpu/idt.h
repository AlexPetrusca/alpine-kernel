#ifndef ALPINE_KERNEL_IDT_H
#define ALPINE_KERNEL_IDT_H

#include <stdint.h>

#include <sys/int128.h>

#define IDT_MAX_LENGTH 256

typedef union {
  uint128_t value;
  struct {
    uint128_t size: 16;    // The size of the table in bytes subtracted by 1
    uint128_t offset: 64;  // The linear address of the IDT (not the physical address, paging applies)
  };
} idt_descriptor;

typedef union {
  uint8_t value;
  struct {
    uint8_t gate_type: 4;   // 0xE = 64-bit Interrupt Gate and 0xF = 64-bit Trap Gate
    uint8_t _reserved: 1;
    uint8_t privilege: 2;   // CPU Privilege Level bits
    uint8_t present: 1;     // Present bit (must be set to 1 for the descriptor to be valid)
  };
} __attribute__((packed)) idt_access_byte;

typedef union {
  uint128_t value;
  struct {
    uint128_t offset1: 16;      // offset: A 64-bit value, represents the address of the Interrupt Service Routine
    uint128_t selector: 16;     // A Segment Selector which must point to a valid code segment in the GDT
    uint128_t ist: 3;           // offset into the Interrupt Stack Table (set to 0 to not use IST)
    uint128_t _reserved1: 5;
    uint128_t access_ctrl: 8;   // segment access control bits
    uint128_t offset2: 48;
    uint128_t _reserved2: 32;
  };
} __attribute__((packed)) idt_entry;

typedef struct {
  uint16_t length;
  idt_entry* entries;
} idt_handle;

bool idt_init();
idt_handle* idt_get_handle();
idt_descriptor* idt_get_descriptor();
uint64_t idt_entry_get_offset(idt_entry* entry);
idt_entry idt_create_entry(uint64_t ist_addr, uint16_t seg_select, idt_access_byte access_byte);

void idt_print_info(int argc, char** argv);
void idt_interrupt(int argc, char** argv);

#endif //ALPINE_KERNEL_IDT_H
