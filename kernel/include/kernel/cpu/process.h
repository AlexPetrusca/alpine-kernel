#ifndef KERNEL_CPU_PROCESS_H_
#define KERNEL_CPU_PROCESS_H_

#include "isr.h"

#define PCS_STACK_SIZE (16 * 1024)

typedef enum {
  RUNNING = 0,
  DEAD = 1,
} volatile process_state;

typedef struct {
  dll_node node;
  void* stack;
  void (* main)();
  interrupt_frame* frame;
  process_state volatile state;
  uint64_t id;
  char* name;
} pcs_process;

bool pcs_init();
pcs_process* pcs_create_process(void (* main)(), char* name);
interrupt_frame* pcs_handle_scheduling(interrupt_frame* frame);
void pcs_join_process(pcs_process* process);
void pcs_die();
void pcs_print_processes(int argc, char** argv);
void spin(int argc, char** argv);

#endif //KERNEL_CPU_PROCESS_H_
