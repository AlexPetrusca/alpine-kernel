#ifndef ALPINE_KERNEL_VBE_TTYD_H
#define ALPINE_KERNEL_VBE_TTYD_H

#include <kernel/ttyd.h>

tty_device vbe_ttyd_init(uint64_t fb, uint32_t screen_width, uint32_t screen_height);
tty_device vbe_ttyd_get_device();

#endif
