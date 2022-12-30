#include <stdio.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/sh.h>
#include <kernel/vga.h>
#include <kernel/cpu.h>
#include <stdio_tests.h>
#include <kernel/multiboot2.h>
#include <kernel/acpi.h>
#include <kernel/apic.h>
#include <kernel/pci.h>
#include <kernel/mem.h>

uint64_t kernel_addr;
char* bootloader_name;
char* kernel_cmd_line;
uint64_t kernel_base_addr;
struct mb2_tag_bootdev* boot_dev;
struct mb2_tag_elf_sections* elf_sections;
struct mb2_tag_framebuffer* frame_buffer;
struct mb2_tag_apm* apm;

void parse_mbi() {
  for (struct mb2_tag* tag = (struct mb2_tag*) (kernel_addr + 8);
       tag->type != MB2_TAG_TYPE_END;
       tag = (struct mb2_tag*) ((uint8_t*) tag + ((tag->size + 7) & ~7))) {
    switch (tag->type) {
      case MB2_TAG_TYPE_CMDLINE:
        kernel_cmd_line = ((struct mb2_tag_string*) tag)->string;
        break;

      case MB2_TAG_TYPE_BOOT_LOADER_NAME:
        bootloader_name = ((struct mb2_tag_string*) tag)->string;
        break;

      case MB2_TAG_TYPE_BASIC_MEMINFO:
        _mem_lower = ((struct mb2_tag_basic_meminfo*) tag)->mem_lower;
        _mem_upper = ((struct mb2_tag_basic_meminfo*) tag)->mem_upper;
        break;

      case MB2_TAG_TYPE_MMAP:
        _mem_map = (struct mb2_tag_mmap*) tag;
        break;

      case MB2_TAG_TYPE_BOOTDEV:
        boot_dev = (struct mb2_tag_bootdev*) tag;
        break;

      case MB2_TAG_TYPE_FRAMEBUFFER:
        frame_buffer = (struct mb2_tag_framebuffer*) tag;
        break;

      case MB2_TAG_TYPE_LOAD_BASE_ADDR:
        kernel_base_addr = ((struct mb2_tag_load_base_addr*) tag)->load_base_addr;
        break;

      case MB2_TAG_TYPE_APM: {
        apm = (struct mb2_tag_apm*) tag;
        break;
      }

      case MB2_TAG_TYPE_ELF_SECTIONS: {
        elf_sections = (struct mb2_tag_elf_sections*) tag;
        break;
      }

      case MB2_TAG_TYPE_ACPI_OLD:
        acpi_init((acpi_rsdp*) &(((struct mb2_tag_old_acpi*) tag)->rsdp));
        break;

      case MB2_TAG_TYPE_ACPI_NEW:
        acpi_init((acpi_rsdp*) &(((struct mb2_tag_new_acpi*) tag)->rsdp));
        break;

      default:
        printf("Error: Unknown tag %d\n", tag->type);
    }
  }
}

void validate_boot_info(unsigned long magic, unsigned long _kernel_addr) {
  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("Invalid magic number: 0x%x\n", (unsigned) magic);
    return;
  }

  if (kernel_addr & 7) {
    printf("Unaligned parse_mbi: 0x%lx\n", _kernel_addr);
    return;
  }

  kernel_addr = _kernel_addr;
}

void mbi_print_info() {
  printf("Boot loader name = %s\n", bootloader_name);
  printf("Boot device 0x%x,%u,%u\n", boot_dev->biosdev, boot_dev->slice, boot_dev->part);
  printf("Load base addr 0x%lx\n", kernel_base_addr);
  printf("APM cseg=%d:%d, dseg=%d:%d\n", apm->cseg, apm->cseg_len, apm->dseg, apm->dseg_len);
  printf("ELF sections: %d\n", elf_sections->num);
  printf("Screen %d x %d\n", frame_buffer->common.framebuffer_width, frame_buffer->common.framebuffer_height);
  printf("Command line = %s\n", kernel_cmd_line);
}

void kernel_main(unsigned long magic, unsigned long _kernel_addr) {
  terminal_initialize(&vga_tty_device);
  validate_boot_info(magic, _kernel_addr);
  parse_mbi();
  pci_init();

  sh_command commands[] = {
    {"cpu", cpu_print_info},
    {"test", test_sprintf_suite},
    {"acpi", acpi_print_info},
    {"apic", apic_print_info},
    {"mcfg", pci_print_mcfg},
    {"pci", pci_print_devices},
    {"mmap", mem_print_map},
    {"mbi", mbi_print_info},
    {"", NULL}
  };
  shell_start(commands);
}
