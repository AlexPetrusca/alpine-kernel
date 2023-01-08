#ifndef ALPINE_KERNEL_VGA_TTYD_H
#define ALPINE_KERNEL_VGA_TTYD_H

#include <kernel/tty/ttyd.h>

#define VGA_TEXT_MODE_WIDTH       80
#define VGA_TEXT_MODE_HEIGHT      25
#define VGA_TEXT_MODE_PHYS_ADDR   0xB8000

tty_device vga_ttyd_init(uint64_t fb);
tty_device vga_ttyd_get_device();

#endif
