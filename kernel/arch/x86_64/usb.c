#include <kernel/usb.h>

bool usb_inited = false;
usb_capability_regs usb_caps = {0};

#define CHECK_INIT()  try(usb_inited, , "USB subsystem not initialized")

bool usb_init() {
  pci_device device = {.base_class_code = 0x0C, .sub_class_code = 0x03, .pi_class_code = 0x30};
  try(pci_get_device(&device), false, "Could not initialize USB subsystem");
  uint64_t addr = pci_bar_addr_64(&device, 0);
  mem_range r = {.phys_addr = addr, .size=2 * 4096};
  mem_identity_map_range(&r);
  usb_caps = *(usb_capability_regs*) addr;
  usb_inited = true;
  return true;
}

void usb_print_info() {
  CHECK_INIT();
  printf("Interface Version: %d\n", usb_caps.HCIVERSION);
  printf("Capability Length: %d\n", usb_caps.CAPLENGTH);
  printf("Max Ports: %d\n", HCSPARAMS1_MAX_PORTS(usb_caps.HCSPARAMS1));
  printf("Max Device Slots: %d\n", HCSPARAMS1_MAX_SLOTS(usb_caps.HCSPARAMS1));
  printf("Max Interrupters: %d\n", HCSPARAMS1_MAX_INTRS(usb_caps.HCSPARAMS1));
  printf("Runtime Register Offset: %d\n", RTSOFF(usb_caps.RTSOFF));
  printf("Doorbell Array Offset: %d\n", DBOFF(usb_caps.DBOFF));
}
