#include <kernel/device/pit.h>
#include <kernel/cpu/pic.h>

void pit_init() {
  pic_enable_irq(PIC_PIT_IRQ);
}