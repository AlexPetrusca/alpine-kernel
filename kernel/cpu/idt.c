#include <stdio.h>
#include <stdlib.h>

#include <kernel/mem/mem.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/pic.h>
#include <kernel/cpu/isr.h>
#include <kernel/cpu/asm.h>

#define CODE_SEG     0x0008
#define DATA_SEG     0x0010

idt_descriptor idt_desc;
extern uint16_t idt_size;
extern idt_entry idt[256];

static inline void idt_install() {
  LIDT(idt_desc);
  // todo: move somewhere else
  STI(); // set the interrupt flag
}

bool idt_init() {
  idt_access_byte access_byte = {.present = 1, .privilege = 0, .gate_type = 0xE /* don't HARD CODE */};

  for (int i = 0; i < idt_size; i++) {
    // the assembly wrapper places the ISR address in the first 64 bits of each IDT entry
    uint64_t isr_addr = *((uint64_t*) &idt + 2 * i);
    idt[i] = idt_create_entry(isr_addr, CODE_SEG, access_byte);
  }

  idt_desc.offset = (uint64_t) &idt;
  idt_desc.size = idt_size * sizeof(idt_entry) - 1;
  idt_install();
  return true;
}

idt_entry idt_create_entry(uint64_t isr_addr, uint16_t seg_select, idt_access_byte access_byte) {
  idt_entry entry = {
    .offset1 = isr_addr,
    .selector = seg_select,
    .ist = 0,
    .reserved1 = 0,
    .access_ctrl = access_byte.value,
    .offset2 = isr_addr >> 16,
    .reserved2 = 0
  };
  return entry;
}

idt_descriptor* idt_get_descriptor() {
  return &idt_desc;
}

uint64_t idt_entry_get_offset(idt_entry* entry) {
  return entry->offset1 + (entry->offset2 << 16);
}

void idt_print_info(__unused int argc, __unused char** argv) {
  printf("IDT entry size (in bytes): %ld\n", sizeof(idt_entry));
  printf("IDT num entries: %d\n", idt_size);
  for (int i = 0; i < idt_size; i++) {
    idt_entry entry = idt[i];
    printf("IDT entry %d\n", i);
    printf("  offset: 0x%.16lx\n", idt_entry_get_offset(&entry));
    printf("  access control: 0b%.8b\n", entry.access_ctrl);
    printf("  ist: 0b%.8b\n", entry.ist);
  }
}

#define IDT_INT_CASE_N(n)   case n: INT(n); break;

void idt_interrupt(int argc, char** argv) {
  if (argc == 2) {
    int vec = (int) strtol(argv[1], NULL, 16);
    printf("Triggering interrupt vector %d (int 0x%x)\n", vec, vec);
    switch (vec) {
      IDT_INT_CASE_N(0);
      IDT_INT_CASE_N(1);
      IDT_INT_CASE_N(2);
      IDT_INT_CASE_N(3);
      IDT_INT_CASE_N(4);
      IDT_INT_CASE_N(5);
      IDT_INT_CASE_N(6);
      IDT_INT_CASE_N(7);
      IDT_INT_CASE_N(8);
      IDT_INT_CASE_N(9);
      IDT_INT_CASE_N(10);
      IDT_INT_CASE_N(11);
      IDT_INT_CASE_N(12);
      IDT_INT_CASE_N(13);
      IDT_INT_CASE_N(14);
      IDT_INT_CASE_N(15);
      IDT_INT_CASE_N(16);
      IDT_INT_CASE_N(17);
      IDT_INT_CASE_N(18);
      IDT_INT_CASE_N(19);
      IDT_INT_CASE_N(20);
      IDT_INT_CASE_N(21);
      IDT_INT_CASE_N(22);
      IDT_INT_CASE_N(23);
      IDT_INT_CASE_N(24);
      IDT_INT_CASE_N(25);
      IDT_INT_CASE_N(26);
      IDT_INT_CASE_N(27);
      IDT_INT_CASE_N(28);
      IDT_INT_CASE_N(29);
      IDT_INT_CASE_N(30);
      IDT_INT_CASE_N(31);
      IDT_INT_CASE_N(32);
      IDT_INT_CASE_N(33);
      IDT_INT_CASE_N(34);
      IDT_INT_CASE_N(35);
      IDT_INT_CASE_N(36);
      IDT_INT_CASE_N(37);
      IDT_INT_CASE_N(38);
      IDT_INT_CASE_N(39);
      IDT_INT_CASE_N(40);
      IDT_INT_CASE_N(41);
      IDT_INT_CASE_N(42);
      IDT_INT_CASE_N(43);
      IDT_INT_CASE_N(44);
      IDT_INT_CASE_N(45);
      IDT_INT_CASE_N(46);
      IDT_INT_CASE_N(47);
      IDT_INT_CASE_N(48);
      IDT_INT_CASE_N(49);
      IDT_INT_CASE_N(50);
      IDT_INT_CASE_N(51);
      IDT_INT_CASE_N(52);
      IDT_INT_CASE_N(53);
      IDT_INT_CASE_N(54);
      IDT_INT_CASE_N(55);
      IDT_INT_CASE_N(56);
      IDT_INT_CASE_N(57);
      IDT_INT_CASE_N(58);
      IDT_INT_CASE_N(59);
      IDT_INT_CASE_N(60);
      IDT_INT_CASE_N(61);
      IDT_INT_CASE_N(62);
      IDT_INT_CASE_N(63);
      IDT_INT_CASE_N(64);
      default:
        printf("int: invalid interrupt vector: %s", argv[1]);
    }
  } else {
    printf("Usage: int [vector]");
  }
}

#undef IDT_INT_CASE_N