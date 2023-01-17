#include <stdint.h>
#include <kernel/boot/elf.h>
#include <stdio.h>
#include <stddef.h>
#include <math.h>

#define SHT_NULL  0
#define SHT_PROGBITS  1
#define SHT_SYMTAB  2
#define SHT_STRTAB  3
#define SHT_RELA  4
#define SHT_HASH  5
#define SHT_DYNAMIC  6
#define SHT_NOTE  7
#define SHT_NOBITS  8
#define SHT_REL  9
#define SHT_SHLIB  10
#define SHT_DYNSYM  11
#define SHT_INIT_ARRAY  14
#define SHT_FINI_ARRAY  15
#define SHT_PREINIT_ARRAY  16
#define SHT_GROUP  17
#define SHT_SYMTAB_SHNDX  18
#define SHT_LOOS  0x60000000
#define SHT_HIOS  0x6fffffff
#define SHT_LOPROC  0x70000000
#define SHT_HIPROC  0x7fffffff
#define SHT_LOUSER  0x80000000
#define SHT_HIUSER  0xffffffff

typedef struct {
  uint32_t name;
  uint32_t type;
  uint64_t flags;
  uint64_t addr;
  uint64_t offset;
  uint64_t size;
  uint32_t link;
  uint32_t info;
  uint64_t addr_align;
  uint64_t entry_size;
} __attribute((packed)) elf_section;
static_assert(sizeof(elf_section) == 64, "");

uint64_t elf_max_kernel_addr = 0;
mb2_tag_elf_sections* elf_sections;

void elf_init(mb2_tag_elf_sections* sections_tag) {
  elf_sections = sections_tag;
  for (uint32_t i = 0; i < elf_sections->count; i++) {
    elf_section* section = (elf_section*) (elf_sections->sections + i * elf_sections->entry_size);
    elf_max_kernel_addr = MAX(elf_max_kernel_addr, section->addr + section->size);
  }
}

elf_section* find_section(uint32_t type) {
  for (uint32_t i = 0; i < elf_sections->count; i++) {
    elf_section* section = (elf_section*) (elf_sections->sections + i * elf_sections->entry_size);
    if (section->type == type) {
      return section;
    }
  }
  return NULL;
}

uint64_t elf_get_max_kernel_addr() {
  return elf_max_kernel_addr;
}

void elf_print_info(__unused int argc, __unused char** argv) {
  elf_section* str_section = (elf_section*)
    (elf_sections->sections + elf_sections->section_name_table_index * elf_sections->entry_size);
  printf("  %2s  %2s  %9s  %6s\n", "#", "Type", "Addr", "Size");
  printf("  ==  ====       ====    ====\n");
  for (uint32_t i = 0; i < elf_sections->count; i++) {
    elf_section* section = (elf_section*) (elf_sections->sections + i * elf_sections->entry_size);
    char* name = (char*) (section->name + str_section->addr);
    printf("  %2d    %2d  %9lx  %6ld  %s\n", i, section->type, section->addr, section->size, name);
  }
  printf("  Kernel Range: %lx - %lx (%ld Kb)",
         KERNEL_LOAD_ADDR, elf_max_kernel_addr, (elf_max_kernel_addr - KERNEL_LOAD_ADDR) / 1024);
}
