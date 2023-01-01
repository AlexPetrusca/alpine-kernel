#ifndef ALPINE_KERNEL_MB2_INFO_H
#define ALPINE_KERNEL_MB2_INFO_H

#include <kernel/mb2_type.h>

#include <stdint.h>

typedef struct {
  char* bootloader_name;
  char* kernel_cmd_line;
  uint64_t kernel_base_addr;

  mb2_tag_basic_meminfo* basic_meminfo_tag;
  mb2_tag_mmap* mem_map_tag;
  mb2_tag_new_acpi* rsdp_tag;
  mb2_tag_bootdev* bootdev_tag;
  mb2_tag_elf_sections* elf_sections_tag;
  mb2_tag_framebuffer* framebuffer_tag;
  mb2_tag_vbe* vbe_tag;
  mb2_tag_apm* apm_tag;
} mb2_info;

mb2_info* mb2_info_init(uint64_t kernel_addr);
mb2_info* mb2_info_get();

void mb2_info_print();
void mb2_fb_info_print();

#endif //ALPINE_KERNEL_MB2_INFO_H
