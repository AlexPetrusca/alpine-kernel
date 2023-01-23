#ifndef ALPINE_KERNEL_ISR_H
#define ALPINE_KERNEL_ISR_H

#include <stdint.h>

typedef struct {
  // TODO define the ISR stack frame here
} interrupt_frame;

void default_isr(interrupt_frame* frame);
void keyboard_isr(interrupt_frame* frame);
extern void isr_page_fault();

#endif //ALPINE_KERNEL_ISR_H
