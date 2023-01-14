#include <stdint.h>
#include <stdio.h>

#include <kernel/cpu/gdt.h>

// todo: refactor - not done

extern volatile uint16_t main_gdt_pointer_size; // The length of the table in bytes subtracted by 1
extern volatile uint64_t main_gdt_pointer_base; // The linear address of the GDT (not the physical address, paging applies)

//gdt_descriptor gdt_desc;
//
//void gdt_init() {
//  gdt_desc.offset = main_gdt_pointer_base;
//  gdt_desc.size = main_gdt_pointer_size + 1;
//}

gdt_descriptor gdt_get_descriptor() {
  gdt_descriptor descriptor;
  descriptor.size = main_gdt_pointer_size;
  descriptor.offset = main_gdt_pointer_base;
  return descriptor;
}

gdt_handle gdt_get_handle() {
  gdt_handle gdt;
  gdt.length = (main_gdt_pointer_size + 1) / sizeof(gdt_entry);
  gdt.entries = (gdt_entry*) main_gdt_pointer_base;
  return gdt;
}

gdt_entry gdt_create_entry(uint32_t base, uint32_t limit, gdt_access_byte access_byte, gdt_flags flags) {
  gdt_entry entry;
  entry.limit1 = limit;
  entry.limit2 = limit >> 16;
  entry.base1 = base;
  entry.base2 = base >> 24;
  entry.flags = flags.value;
  entry.access_ctrl = access_byte.value;
  return entry;
}

uint32_t gdt_entry_get_limit(gdt_entry* entry) {
  return entry->limit1 + (entry->limit2 << 16);
}

uint32_t gdt_entry_get_base(gdt_entry* entry) {
  return entry->base1 + (entry->base2 << 24);
}

void gdt_print_info(__unused int argc, __unused char** argv) {
  gdt_handle gdt = gdt_get_handle();
  printf("GDT size (in bytes): %d\n", main_gdt_pointer_size + 1);
  printf("GDT num entries: %d\n", gdt.length);
  for (int i = 0; i < gdt.length; i++) {
    gdt_entry entry = gdt.entries[i];
    printf("GDT entry %d: 0x%.16llx\n", i, entry.value);
    printf("  base address: 0x%.8x\n", gdt_entry_get_base(&entry));
    printf("  limit: 0x%.5x\n", gdt_entry_get_limit(&entry));
    printf("  access control: 0b%.8b\n", entry.access_ctrl);
    printf("  flags: 0b%.4b\n", entry.flags);
  }
}