#include <kernel/tty/tty.h>
#include <kernel/shell/sh.h>
#include <kernel/device/acpi.h>
#include <kernel/device/pci.h>
#include <kernel/mem/mem.h>
#include <kernel/boot/mb2_info.h>
#include <kernel/device/usb.h>
#include <assert.h>

void validate_boot(unsigned long magic, unsigned long kernel_addr) {
  assert(magic == MB2_BOOTLOADER_MAGIC, "Invalid magic number: 0x%x\n", (unsigned) magic);
  assert((kernel_addr & 7) == 0, "Unaligned kernel_init: 0x%lx\n", kernel_addr);
}

void kernel_init(uint64_t kernel_addr) {
  mb2_info* mbi = mb2_info_init(kernel_addr);
  mem_init(mbi->basic_meminfo_tag, mbi->mem_map_tag);
  tty_init(mbi->framebuffer_tag);
  acpi_init(mbi->rsdp_tag);
  warn(pci_init(), , "Could not initialize PCI subsystem");
  warn(usb_init(), , "Could not initialize USB subsystem");
}

void kernel_main(uint64_t magic, uint64_t kernel_addr) {
  validate_boot(magic, kernel_addr);
  kernel_init(kernel_addr);
  sh_start();
}
