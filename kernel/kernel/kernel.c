#include <stdio.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/sh.h>
#include <kernel/vga.h>
#include <kernel/cpu.h>
#include <stdio_tests.h>
#include <kernel/multiboot2.h>

char* mem_type[] = {"Available", "Reserved", "ACPI Reclaimable", "NVS", "Bad"};

unsigned long kernel_addr;

void mbi() {
  printf("Announced mbi size %d bytes\n", *(unsigned*) kernel_addr);

  struct multiboot_tag* tag;

  for (tag = (struct multiboot_tag*) (kernel_addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7))) {
    printf("Tag 0x%x, Size %d: ", tag->type, tag->size);
    switch (tag->type) {
      case MULTIBOOT_TAG_TYPE_CMDLINE:
        printf("Command line = %s\n", ((struct multiboot_tag_string*) tag)->string);
        break;

      case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
        printf("Boot loader name = %s\n", ((struct multiboot_tag_string*) tag)->string);
        break;

      case MULTIBOOT_TAG_TYPE_MODULE:
        printf("Module at 0x%x-0x%x. Command line %s\n",
               ((struct multiboot_tag_module*) tag)->mod_start,
               ((struct multiboot_tag_module*) tag)->mod_end,
               ((struct multiboot_tag_module*) tag)->cmdline);
        break;

      case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
        printf("mem_lower = %uKB, mem_upper = %uKB\n",
               ((struct multiboot_tag_basic_meminfo*) tag)->mem_lower,
               ((struct multiboot_tag_basic_meminfo*) tag)->mem_upper);
        break;

      case MULTIBOOT_TAG_TYPE_BOOTDEV:
        printf("Boot device 0x%x,%u,%u\n",
               ((struct multiboot_tag_bootdev*) tag)->biosdev,
               ((struct multiboot_tag_bootdev*) tag)->slice,
               ((struct multiboot_tag_bootdev*) tag)->part);
        break;

      case MULTIBOOT_TAG_TYPE_MMAP: {
        printf("mmap\n");
        multiboot_memory_map_t* mmap;
        for (mmap = ((struct multiboot_tag_mmap*) tag)->entries;
             (multiboot_uint8_t*) mmap < (multiboot_uint8_t*) tag + tag->size;
             mmap = (multiboot_memory_map_t*)
               ((unsigned long) mmap
                 + ((struct multiboot_tag_mmap*) tag)->entry_size))
          printf(" base_addr = 0x%x%x, length = 0x%x%x, type = %s\n",
                 (unsigned) (mmap->addr >> 32), (unsigned) (mmap->addr & 0xffffffff),
                 (unsigned) (mmap->len >> 32), (unsigned) (mmap->len & 0xffffffff),
                 mem_type[(unsigned) mmap->type]);
        break;
      }

      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
        struct multiboot_tag_framebuffer* tagfb = (struct multiboot_tag_framebuffer*) tag;
        printf("screen %d x %d\n", tagfb->common.framebuffer_width, tagfb->common.framebuffer_height);
        break;
      }

      case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR: {
        struct multiboot_tag_load_base_addr* base_addr = (struct multiboot_tag_load_base_addr*) tag;
        printf("load base addr 0x%x\n", base_addr->load_base_addr);
        break;
      }

      case MULTIBOOT_TAG_TYPE_APM: {
        struct multiboot_tag_apm* apm = (struct multiboot_tag_apm*) tag;
        printf("APM cseg=%d:%d, dseg=%d:%d\n", apm->cseg, apm->cseg_len, apm->dseg, apm->dseg_len);
        break;
      }

      case MULTIBOOT_TAG_TYPE_ELF_SECTIONS: {
        struct multiboot_tag_elf_sections* elf = (struct multiboot_tag_elf_sections*) tag;
        printf("%d ELF sections\n", elf->num);
        break;
      }

      case MULTIBOOT_TAG_TYPE_ACPI_OLD: {
        struct multiboot_tag_old_acpi* acpi = (struct multiboot_tag_old_acpi*) tag;
        printf("RSDP table, %d bytes\n", acpi->size - 8);
        break;
      }

      default:
        printf("Unknown\n");
    }
  }
  tag = (struct multiboot_tag*) ((multiboot_uint8_t*) tag + ((tag->size + 7) & ~7));
  printf("Total mbi size %d bytes\n", (unsigned) tag - kernel_addr);
}

void cmain(unsigned long magic, unsigned long _kernel_addr) {
  kernel_addr = _kernel_addr;
  terminal_initialize(&vga_tty_device);

  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
    printf("Invalid magic number: 0x%x\n", (unsigned) magic);
    return;
  }

  if (kernel_addr & 7) {
    printf("Unaligned mbi: 0x%x\n", kernel_addr);
    return;
  }

  sh_command commands[] = {{"cpu", print_cpu_info}, {"test", printf_tests}, {"mbi", mbi}, {"", NULL}};
  shell_start(commands);
}
