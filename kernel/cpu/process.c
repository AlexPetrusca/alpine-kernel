#include <kernel/cpu/cpu.h>
#include <kernel/mem/mem.h>
#include <kernel/shell/sh.h>
#include <kernel/cpu/process.h>
#include <kernel/cpu/pic.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/asm.h>

pcs_process _processes[4];
uint64_t _pcs_stack_addr;

void idle_main() {
  while (true) {
    cpu_delay_s(1);
    printf("I am the idle process 1\n");
  }
}

void* pcs_allocate_stack() {
  _pcs_stack_addr += PCS_STACK_SIZE;
  return (void*) _pcs_stack_addr;
}

pcs_process create_process(void (* main)()) {
  pcs_process process;
  process.stack = pcs_allocate_stack();
  process.main = main;
  interrupt_frame* f = process.stack - sizeof(interrupt_frame);
  process.frame = f;
  __cr(f->cr0, f->cr2, f->cr3, f->cr4);
  f->es = DATA_SEG;
  f->cs = CODE_SEG;
  f->ds = DATA_SEG;
  f->fs = DATA_SEG;
  f->gs = DATA_SEG;
  f->ss = DATA_SEG;
  f->rfl.IF = 1; // enable interrupts
  f->rsp = f->rbp = (uint64_t) process.stack;
  f->rip = (uint64_t) main;
  return process;
}

bool pcs_init() {
  _pcs_stack_addr = STACK_VIRTUAL_ADDR;
  uint32_t size = cpu_core_count() * PCS_STACK_SIZE;
  // the stacks need to be fully allocated since we cannot rely on page faults (they use the stack!)
  try(mem_allocate_range(_pcs_stack_addr, size), false, "");
  memset((void*) _pcs_stack_addr, 0, size);
  mem_update_range(_pcs_stack_addr, 0, size, MEMORY_STACKS);

  _processes[0] = create_process(sh_start);
  _processes[1] = create_process(idle_main);

  return true;
}
