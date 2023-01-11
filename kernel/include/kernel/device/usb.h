#ifndef KERNEL_ARCH_X86_64_USB_H_
#define KERNEL_ARCH_X86_64_USB_H_

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include <kernel/error/error.h>
#include <kernel/mem/mem.h>
#include <kernel/device/pci.h>

#define DBOFF(reg)                          (reg & ~0b11)
#define RTSOFF(reg)                         (reg & ~0b11111)

typedef union {
  uint32_t value;
  struct {
    uint32_t MaxSlots: 8;
    uint32_t MaxIntrs: 11;
    uint32_t : 5;           // reserved
    uint32_t MaxPorts: 8;
  };
} __attribute__((packed)) HCSPARAMS1;
static_assert(sizeof(HCSPARAMS1) == 4, "HCSPARAMS1 is 32 bits long");

typedef struct {
  uint8_t CAPLENGTH;      // Capability Register Length 5.3.1
  uint16_t HCIVERSION;    // Interface Version Number 5.3.2
  HCSPARAMS1 HCSPARAMS1;  // Structural Parameters 1 5.3.3
  uint32_t HCSPARAMS2;    // Structural Parameters 2 5.3.4
  uint32_t HCSPARAMS3;    // Structural Parameters 3 5.3.5
  uint32_t HCCPARAMS1;    // Capability Parameters 1 5.3.6
  uint32_t DBOFF;         // Doorbell Offset 5.3.7
  uint32_t RTSOFF;        // Runtime Register Space Offset 5.3.8
  uint32_t HCCPARAMS2;    // Capability Parameters 2 5.3.9
} __attribute__((packed)) usb_capability_regs;

bool usb_init() __attribute__ ((warn_unused_result));

void usb_print_info(int argc, char** argv);

#endif //KERNEL_ARCH_X86_64_USB_H_
