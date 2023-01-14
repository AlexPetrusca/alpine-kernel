#include <stdio.h>
#include <stdlib.h>

#include <kernel/mem/mem.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/pic.h>
#include <kernel/cpu/isr.h>

// todo: refactor - not done

extern volatile uint16_t main_idt_pointer_size; // The size of the table in bytes subtracted by 1
extern volatile uint64_t main_idt_pointer_base; // The linear address of the IDT (not the physical address, paging applies)

idt_descriptor idt_desc;
idt_handle idt_hand;
idt_entry idt[IDT_MAX_LENGTH];

static inline void idt_install() {
  __asm volatile ("lidt %0" : : "m"(idt_desc.value));
  // todo: move somewhere else
  __asm volatile ("sti"); // set the interrupt flag
}

bool idt_init() {
  uint64_t idt_addr = (uint64_t) &idt;
  printf("idt_addr: 0x%llx\n", idt_addr);
  size_t idt_size = IDT_MAX_LENGTH * sizeof(idt_entry);
//  try(mem_identity_map_range(idt_addr, idt_size, MEMORY_IDT), false, "");
  memset((char*) idt_addr, 0, idt_size);

  main_idt_pointer_size = idt_size - 1;
  main_idt_pointer_base = idt_addr;
  idt_desc.size = main_idt_pointer_size;
  idt_desc.offset = main_idt_pointer_base;
  idt_hand.length = (main_idt_pointer_size + 1) / sizeof(idt_entry);
  idt_hand.entries = (idt_entry*) main_idt_pointer_base;

  idt_access_byte access_byte;
  access_byte.present = 1;
  access_byte.privilege = 0;
  access_byte.gate_type = 0xE; // don't HARD CODE
  for (int i = 0; i < IDT_MAX_LENGTH; i++) {
    idt_hand.entries[i] = idt_create_entry((uint64_t) &isr, 0x0008, access_byte); // todo: use defines instead of hard-coding
  }
  idt_install();

  pic_init();

  return true;
}

// CODE_SEG    equ 0x0008
// DATA_SEG    equ 0x0010
idt_entry idt_create_entry(uint64_t ist_addr, uint16_t seg_select, idt_access_byte access_byte) {
  idt_entry entry;
  entry.offset1 = ist_addr;
  entry.offset2 = ist_addr >> 16;
  entry.selector = seg_select;
  entry.access_ctrl = access_byte.value;
  entry.ist = 0;
  return entry;
}

idt_descriptor* idt_get_descriptor() {
  return &idt_desc;
}

idt_handle* idt_get_handle() {
  return &idt_hand;
}

uint64_t idt_entry_get_offset(idt_entry* entry) {
  return entry->offset1 + (entry->offset2 << 16);
}

void idt_print_info(__unused int argc, __unused char** argv) {
  printf("IDT size (in bytes): %d\n", main_idt_pointer_size + 1);
  printf("IDT entry size (in bytes): %ld\n", sizeof(idt_entry));
  printf("IDT num entries: %d\n", idt_hand.length);
  for (int i = 0; i < idt_hand.length; i++) {
    idt_entry entry = idt_hand.entries[i];
    if (entry.value > 0) {
      printf("IDT entry %d: 0x%.16llx%.16llx\n", i, (uint64_t) (entry.value >> 64), (uint64_t) entry.value);
      printf("  offset: 0x%.16llx\n", idt_entry_get_offset(&entry));
      printf("  access control: 0b%.8b\n", entry.access_ctrl);
      printf("  ist: 0b%.8b\n", entry.ist);
    }
  }
}

#define IDT_INT_CASE_N(n)                      \
  case n:                                     \
    __asm__ volatile ("int %0" : : "i"(n));   \
    break;

void idt_interrupt(int argc, char** argv) {
  if (argc == 2) {
    int vec = (int) strtol(argv[1], NULL, 16);
    printf("Triggering interrupt vector %d (int 0x%x)", vec, vec);
    switch (vec) {
      IDT_INT_CASE_N(0);  IDT_INT_CASE_N(1);  IDT_INT_CASE_N(2);  IDT_INT_CASE_N(3);  IDT_INT_CASE_N(4);
      IDT_INT_CASE_N(5);  IDT_INT_CASE_N(6);  IDT_INT_CASE_N(7);  IDT_INT_CASE_N(8);  IDT_INT_CASE_N(9);
      IDT_INT_CASE_N(10); IDT_INT_CASE_N(11); IDT_INT_CASE_N(12); IDT_INT_CASE_N(13); IDT_INT_CASE_N(14);
      IDT_INT_CASE_N(15); IDT_INT_CASE_N(16); IDT_INT_CASE_N(17); IDT_INT_CASE_N(18); IDT_INT_CASE_N(19);
      IDT_INT_CASE_N(20); IDT_INT_CASE_N(21); IDT_INT_CASE_N(22); IDT_INT_CASE_N(23); IDT_INT_CASE_N(24);
      IDT_INT_CASE_N(25); IDT_INT_CASE_N(26); IDT_INT_CASE_N(27); IDT_INT_CASE_N(28); IDT_INT_CASE_N(29);
      IDT_INT_CASE_N(30); IDT_INT_CASE_N(31); IDT_INT_CASE_N(32); IDT_INT_CASE_N(33); IDT_INT_CASE_N(34);
      IDT_INT_CASE_N(35); IDT_INT_CASE_N(36); IDT_INT_CASE_N(37); IDT_INT_CASE_N(38); IDT_INT_CASE_N(39);
      IDT_INT_CASE_N(40); IDT_INT_CASE_N(41); IDT_INT_CASE_N(42); IDT_INT_CASE_N(43); IDT_INT_CASE_N(44);
      IDT_INT_CASE_N(45); IDT_INT_CASE_N(46); IDT_INT_CASE_N(47); IDT_INT_CASE_N(48); IDT_INT_CASE_N(49);
      IDT_INT_CASE_N(50); IDT_INT_CASE_N(51); IDT_INT_CASE_N(52); IDT_INT_CASE_N(53); IDT_INT_CASE_N(54);
      IDT_INT_CASE_N(55); IDT_INT_CASE_N(56); IDT_INT_CASE_N(57); IDT_INT_CASE_N(58); IDT_INT_CASE_N(59);
      IDT_INT_CASE_N(60); IDT_INT_CASE_N(61); IDT_INT_CASE_N(62); IDT_INT_CASE_N(63); IDT_INT_CASE_N(64);
      default:
        printf("int: invalid interrupt vector: %s", argv[1]);
    }
  } else {
    printf("Usage: int [vector]");
  }
}

#undef IDT_INT_CASE_N