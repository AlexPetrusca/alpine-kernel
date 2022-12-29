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

char* mem_type[] = {"", "Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad"};

uint64_t kernel_addr;

char* bootloader_name;
char* kernel_cmd_line;
uint64_t kernel_base_addr;
struct multiboot_tag_bootdev* boot_dev;
struct multiboot_tag_elf_sections* elf_sections;
struct multiboot_tag_basic_meminfo* mem_info;
struct multiboot_tag_mmap* mem_map;
struct multiboot_tag_framebuffer* frame_buffer;
struct multiboot_tag_apm* apm;

void parse_mbi(bool print) {
  if (print) printf("Announced parse_mbi size %d bytes\n", *(unsigned*) kernel_addr);

  struct multiboot_tag* tag;

  for (tag = (struct multiboot_tag*) (kernel_addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag*) ((uint8_t*) tag + ((tag->size + 7) & ~7))) {
    if (print) printf("Tag 0x%x, Size %d: ", tag->type, tag->size);
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE:
        kernel_cmd_line = ((struct multiboot_tag_string*) tag)->string;
        if (print) printf("Command line = %s\n", kernel_cmd_line);
        break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        bootloader_name = ((struct multiboot_tag_string*) tag)->string;
        if (print) printf("Boot loader name = %s\n", bootloader_name);
        break;

      case MULTIBOOT_TAG_TYPE_MODULE:
        if (print)
          printf("Module at 0x%x-0x%x. Command line %s\n",
                 ((struct multiboot_tag_module*) tag)->mod_start,
                 ((struct multiboot_tag_module*) tag)->mod_end,
                 ((struct multiboot_tag_module*) tag)->cmdline);
        break;

      case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        mem_info = (struct multiboot_tag_basic_meminfo*) tag;
        if (print) printf("mem_lower = %uKB, mem_upper = %uKB\n", mem_info->mem_lower, mem_info->mem_upper);
        break;

      case MULTIBOOT_TAG_TYPE_BOOTDEV:
        boot_dev = (struct multiboot_tag_bootdev*) tag;
        if (print) printf("Boot device 0x%x,%u,%u\n", boot_dev->biosdev, boot_dev->slice, boot_dev->part);
        break;

      case MULTIBOOT_TAG_TYPE_MMAP: {
        mem_map = (struct multiboot_tag_mmap*) tag;
        if (print) printf("mmap\n");
        for (multiboot_memory_map_t* mmap = mem_map->entries;
             (uint8_t*) mmap < (uint8_t*) tag + tag->size;
             mmap = (multiboot_memory_map_t*) ((uint64_t) mmap + mem_map->entry_size))
          if (print)
            printf(" %0.12lx - %0.12lx (%,ld): %s\n",
                   mmap->addr, mmap->addr + mmap->len, mmap->len,
                   mem_type[mmap->type]);
        break;
      }

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
        frame_buffer = (struct multiboot_tag_framebuffer*) tag;
        if (print)
          printf("screen %d x %d\n",
                 frame_buffer->common.framebuffer_width,
                 frame_buffer->common.framebuffer_height);
        break;
      }

      case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
        kernel_base_addr = ((struct multiboot_tag_load_base_addr*) tag)->load_base_addr;
        if (print) printf("load base addr 0x%x\n", kernel_base_addr);
        break;
      }

      case MULTIBOOT_TAG_TYPE_APM: {
        apm = (struct multiboot_tag_apm*) tag;
        if (print) printf("APM cseg=%d:%d, dseg=%d:%d\n", apm->cseg, apm->cseg_len, apm->dseg, apm->dseg_len);
        break;
      }

      case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
        elf_sections = (struct multiboot_tag_elf_sections*) tag;
        if (print) printf("%d ELF sections\n", elf_sections->num);
        break;
      }

      case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
        rsdp = (Acpi2Rsdp*) &(((struct multiboot_tag_old_acpi*) tag)->rsdp);
        if (print) printf("RSDP Sig: '%.8s', RSDT Address: 0x%x\n", (char*) rsdp, rsdp->RsdtAddress);
        break;
      }

      default:
        if (print) printf("Unknown\n");
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

void kernel_main(unsigned long magic, unsigned long _kernel_addr) {
  terminal_initialize(&vga_tty_device);
  validate_boot_info(magic, _kernel_addr);
  parse_mbi(false);

  mem_identity_map_range(  // The ACPI table range
    0x0FFDD000,
    0x10000000
  );
  mem_identity_map_range(  // The PCIe config range
    0xB0000000,  // 256M
    0xC0000000   // 256 busses, 32 devices/bus, 8 functions/device, 4096 bytes/function = 256M
  );

  pci_enumerate();

  sh_command commands[] = {
    {"cpu", print_cpu_info},
    {"test", PrintfTestSuite},
    {"acpi", print_acpi_info},
    {"apic", print_apic_info},
    {"mcfg", pci_PrintMcfg},
    {"enum", pci_enumerate},
    {"pci", pci_PrintDevices},
    {"", NULL}
  };
  shell_start(commands);
}
