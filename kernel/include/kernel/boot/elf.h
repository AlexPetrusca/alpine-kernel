#ifndef KERNEL_INCLUDE_KERNEL_BOOT_ELF_H_
#define KERNEL_INCLUDE_KERNEL_BOOT_ELF_H_

#include <kernel/boot/mb2_info.h>

#define KERNEL_LOAD_ADDR      0x100000L

void elf_init(mb2_tag_elf_sections* sections_tag);
uint64_t elf_get_max_kernel_addr();
void elf_print_info(int argc, char** argv);

#endif //KERNEL_INCLUDE_KERNEL_BOOT_ELF_H_
