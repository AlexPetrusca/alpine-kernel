#include <kernel/device/usb.h>

bool usb_inited = false;
usb_capability_regs usb_caps = {0};

#define CHECK_INIT()  try(usb_inited, , "USB subsystem not initialized")

bool usb_init() {
  pci_device device = {.base_class_code = 0x0C, .sub_class_code = 0x03, .pi_class_code = 0x30};
  try(pci_get_device(&device), false, "Could not initialize USB subsystem");
  uint64_t addr = pci_bar_addr(&device, 0);
  try(mem_map_range_4k(addr, addr, 2 * PAGE_SIZE_4K), false, "");
  mem_update_range(addr, addr, 2 * PAGE_SIZE_4K, MEMORY_USB_DEVICE);
  usb_caps.CAPLENGTH = mem_read_8(addr + 0x00);
  usb_caps.HCIVERSION = mem_read_16(addr + 0x02);
  usb_caps.HCSPARAMS1.value = mem_read_32(addr + 0x04);
  usb_caps.HCSPARAMS2 = mem_read_32(addr + 0x08);
  usb_caps.HCSPARAMS3 = mem_read_32(addr + 0x0C);
  usb_caps.HCCPARAMS1 = mem_read_32(addr + 0x10);
  usb_caps.DBOFF = mem_read_32(addr + 0x14);
  usb_caps.RTSOFF = mem_read_32(addr + 0x18);
  usb_caps.HCCPARAMS2 = mem_read_32(addr + 0x1C);
  return usb_inited = true;
}

void usb_print_info(__unused int argc, __unused char** argv) {
  CHECK_INIT();
  printf("Interface Version: %d\n", usb_caps.HCIVERSION);
  printf("Capability Length: %d\n", usb_caps.CAPLENGTH);
  printf("Max Ports: %d\n", usb_caps.HCSPARAMS1.MaxPorts);
  printf("Max Device Slots: %d\n", usb_caps.HCSPARAMS1.MaxSlots);
  printf("Max Interrupters: %d\n", usb_caps.HCSPARAMS1.MaxIntrs);
  printf("Runtime Register Offset: %d\n", RTSOFF(usb_caps.RTSOFF));
  printf("Doorbell Array Offset: %d\n", DBOFF(usb_caps.DBOFF));
}
