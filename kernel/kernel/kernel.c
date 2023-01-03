#include <kernel/tty.h>
#include <kernel/sh.h>
#include <kernel/mb2_type.h>
#include <kernel/acpi.h>
#include <kernel/pci.h>
#include <kernel/mem.h>
#include <kernel/heap.h>
#include <kernel/panic.h>
#include <kernel/mb2_info.h>
#include <kernel/usb.h>

void validate_boot(unsigned long magic, unsigned long kernel_addr) {
  if (magic != MB2_BOOTLOADER_MAGIC) {
    panic("Invalid magic number: 0x%x\n", (unsigned) magic);
  }
  if (kernel_addr & 7) {
    panic("Unaligned kernel_init: 0x%lx\n", kernel_addr);
  }
}

void kernel_init(uint64_t kernel_addr) {
  mb2_info* mbi = mb2_info_init(kernel_addr);
  terminal_init(mbi->framebuffer_tag);
  mem_init(mbi->basic_meminfo_tag, mbi->mem_map_tag);
  heap_init(512 * PAGE_SIZE);
  acpi_init(mbi->rsdp_tag);
  pci_init();
  usb_init();
}

void kernel_main(uint64_t magic, uint64_t kernel_addr) {
  validate_boot(magic, kernel_addr);
  kernel_init(kernel_addr);
  shell_start();
}
