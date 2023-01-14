#include <kernel/cpu/cpu.h>
#include <kernel/mem/mem.h>
#include <kernel/shell/sh.h>
#include <kernel/cpu/process.h>

#define PCS_STACK_SIZE (16 * 1024)

pcs_process _processes[4];
uint64_t _pcs_stack_addr;

void idle0_main() {
  cpu_delay_s(1);
//  printf("I am the idle process\n");
  while (true) {
    cpu_delay_s(1);
  }
}

void* pcs_allocate_stack() {
  _pcs_stack_addr += PCS_STACK_SIZE;
  return (void*) _pcs_stack_addr;
}

bool pcs_init() {
  uint32_t core_count = cpu_core_count();
  _pcs_stack_addr = mem_get_heap_addr() - core_count * PCS_STACK_SIZE;
  try(mem_identity_map_range(_pcs_stack_addr, core_count * PCS_STACK_SIZE, MEMORY_STACKS), false, "");

  _processes[0].stack = pcs_allocate_stack();
  _processes[0].main = sh_start;

  _processes[1].stack = pcs_allocate_stack();
  _processes[1].main = idle0_main;

  return true;
}
