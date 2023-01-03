#ifndef KERNEL_ARCH_X86_64_USB_H_
#define KERNEL_ARCH_X86_64_USB_H_

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include <kernel/error.h>
#include <kernel/pci.h>
#include <kernel/mem.h>

#define HCSPARAMS1_MAX_SLOTS(reg)           (reg & 0x000000FF)
#define HCSPARAMS1_MAX_INTRS(reg)           ((reg & 0x0007FF00) >> 8)
#define HCSPARAMS1_MAX_PORTS(reg)           (reg >> 24)
#define DBOFF(reg)                          (reg & ~0b11)
#define RTSOFF(reg)                         (reg & ~0b11111)

//#define HCSPARAMS1_SET_MaxInterrupters(reg, v)   reg |= ((v << 8) & 0x0007FF00)

typedef struct {
  uint8_t CAPLENGTH; // Capability Register Length 5.3.1
  uint8_t reserved;
  uint16_t HCIVERSION; //Interface Version Number 5.3.2
  uint32_t HCSPARAMS1;// Structural Parameters 1 5.3.3
  uint32_t HCSPARAMS2;// Structural Parameters 2 5.3.4
  uint32_t HCSPARAMS3;// Structural Parameters 3 5.3.5
  uint32_t HCCPARAMS1;// Capability Parameters 1 5.3.6
  uint32_t DBOFF;// Doorbell Offset 5.3.7
  uint32_t RTSOFF;// Runtime Register Space Offset 5.3.8
  uint32_t HCCPARAMS2;// Capability Parameters 2 5.3.9
} __attribute__((packed)) usb_capability_regs;

bool usb_init();
void usb_print_info();

#endif //KERNEL_ARCH_X86_64_USB_H_
