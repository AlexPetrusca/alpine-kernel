#include <kernel/cpu/cpu.h>
#include <kernel/mem/mem.h>
#include <kernel/shell/sh.h>
#include <kernel/cpu/process.h>
#include <kernel/cpu/pic.h>

#define PCS_STACK_SIZE (16 * 1024)

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
  uint32_t core_count = cpu_core_count();
  mem_range main_mem;
  try(mem_find_range(MAIN_MEM_START, &main_mem), false, "");
  uint32_t size = core_count * PCS_STACK_SIZE;
  _pcs_stack_addr = main_mem.phys_addr + main_mem.size - size;
  // the stacks still need to be identity mapped because they are used by the trampoline?
  try(mem_identity_map_range(_pcs_stack_addr, size), false, "");
  mem_update_range(_pcs_stack_addr, _pcs_stack_addr, size, MEMORY_STACKS);

  _processes[0].stack = pcs_allocate_stack();
  _processes[0].main = shell_main;

  _processes[1].stack = pcs_allocate_stack();
  _processes[1].main = idle_main;

  return true;
}
