#include <kernel/tty/tty.h>
#include <kernel/device/acpi.h>
#include <kernel/device/pci.h>
#include <kernel/mem/mem.h>
#include <kernel/boot/mb2_info.h>
#include <kernel/device/usb.h>
#include <kernel/device/apic.h>
#include <assert.h>
#include <kernel/cpu/smp.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/process.h>
#include <kernel/cpu/idt.h>

void validate_boot(unsigned long magic, unsigned long kernel_addr) {
  assert(magic == MB2_BOOTLOADER_MAGIC, "Invalid magic number: 0x%x\n", (unsigned) magic);
  assert((kernel_addr & 7) == 0, "Unaligned kernel_init: 0x%lx\n", kernel_addr);
}

void kernel_init(uint64_t kernel_addr) {
  mb2_info* mbi = mb2_info_init(kernel_addr);
  idt_init();
  mem_init(mbi->basic_meminfo_tag, mbi->mem_map_tag);
  tty_init(mbi->framebuffer_tag);
  cpu_init();
  acpi_init(mbi->rsdp_tag);
  assert(apic_init(), "Could not initialize PCI subsystem");
  warn(pci_init(), , "Could not initialize PCI subsystem");
  warn(usb_init(), , "Could not initialize USB subsystem");
  pcs_init();
  smp_init();
}

void kernel_main(uint64_t magic, uint64_t kernel_addr) {
  validate_boot(magic, kernel_addr);
  kernel_init(kernel_addr);
  enter_smp();
}
