#ifndef KERNEL_CPU_PROCESS_H_
#define KERNEL_CPU_PROCESS_H_

#include "isr.h"

#define PCS_STACK_SIZE (16 * 1024)

typedef struct {
  void* stack;
  void (*main)();
  interrupt_frame* frame;
} pcs_process;

bool pcs_init();

#endif //KERNEL_CPU_PROCESS_H_
