#include <kernel/cpu/cpu.h>
#include <kernel/mem/mem.h>
#include <kernel/shell/sh.h>
#include <kernel/cpu/process.h>
#include <kernel/cpu/pic.h>

pcs_process _processes[4];
uint64_t _pcs_stack_addr;

void idle_main() {
  while (true) {
    cpu_delay_s(1);
//    printf("I am the idle process\n");
  }
}

void shell_main() {
  pic_init();
  sh_start();
}

void* pcs_allocate_stack() {
  _pcs_stack_addr += PCS_STACK_SIZE;
  return (void*) _pcs_stack_addr;
}

bool pcs_init() {
  _pcs_stack_addr = STACK_VIRTUAL_ADDR;
  uint32_t size = cpu_core_count() * PCS_STACK_SIZE;
  // the stacks need to be fully allocated since we cannot rely on page faults (they use the stack!)
  try(mem_allocate_range(_pcs_stack_addr, size), false, "");
  mem_update_range(_pcs_stack_addr, 0, size, MEMORY_STACKS);

  _processes[0].stack = pcs_allocate_stack();
  _processes[0].main = shell_main;

  _processes[1].stack = pcs_allocate_stack();
  _processes[1].main = idle_main;

  return true;
}
