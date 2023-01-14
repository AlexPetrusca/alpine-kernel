#ifndef ALPINE_KERNEL_ISR_H
#define ALPINE_KERNEL_ISR_H

struct interrupt_frame;

void isr(struct interrupt_frame* frame);

#endif //ALPINE_KERNEL_ISR_H
