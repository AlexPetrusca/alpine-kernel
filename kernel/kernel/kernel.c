#include <stdio.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/sh.h>
#include <kernel/vga.h>
#include <kernel/cpu.h>
#include <stdio_tests.h>
#include <kernel/multiboot2.h>
#include <kernel/acpi.h>
#include <kernel/pci.h>
#include <kernel/mem.h>

uint64_t kernel_addr;
char* bootloader_name;
char* kernel_cmd_line;
uint64_t kernel_base_addr;
struct multiboot_tag_bootdev* boot_dev;
struct multiboot_tag_elf_sections* elf_sections;
struct multiboot_tag_framebuffer* frame_buffer;
struct multiboot_tag_apm* apm;

void parse_mbi() {
  for (struct multiboot_tag* tag = (struct multiboot_tag*) (kernel_addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag*) ((uint8_t*) tag + ((tag->size + 7) & ~7))) {
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE:
        kernel_cmd_line = ((struct multiboot_tag_string*) tag)->string;
        break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        bootloader_name = ((struct multiboot_tag_string*) tag)->string;
        break;

      case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        _mem_lower = ((struct multiboot_tag_basic_meminfo*) tag)->mem_lower;
        _mem_upper = ((struct multiboot_tag_basic_meminfo*) tag)->mem_upper;
        break;

      case MULTIBOOT_TAG_TYPE_MMAP:
        _mem_map = (struct multiboot_tag_mmap*) tag;
        break;

      case MULTIBOOT_TAG_TYPE_BOOTDEV:
        boot_dev = (struct multiboot_tag_bootdev*) tag;
        break;

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
        frame_buffer = (struct multiboot_tag_framebuffer*) tag;
        break;

      case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
        kernel_base_addr = ((struct multiboot_tag_load_base_addr*) tag)->load_base_addr;
        break;

      case MULTIBOOT_TAG_TYPE_APM: {
        apm = (struct multiboot_tag_apm*) tag;
        break;
      }

      case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
        elf_sections = (struct multiboot_tag_elf_sections*) tag;
        break;
      }

      case MULTIBOOT_TAG_TYPE_ACPI_OLD:
        acpi_init((Acpi2Rsdp*) &(((struct multiboot_tag_old_acpi*) tag)->rsdp));
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

void print_mbi() {
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
    {"cpu", print_cpu_info},
    {"test", PrintfTestSuite},
    {"acpi", print_acpi_info},
    {"apic", print_apic_info},
    {"mcfg", pci_PrintMcfg},
    {"pci", pci_PrintDevices},
    {"mmap", mem_print_map},
    {"mbi", print_mbi},
    {"", NULL}
  };
  shell_start(commands);
}
