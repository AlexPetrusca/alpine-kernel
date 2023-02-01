#include <kernel/cpu/cpu.h>
#include <kernel/mem/mem.h>
#include <kernel/shell/sh.h>
#include <kernel/cpu/process.h>
#include <kernel/cpu/pic.h>
#include <kernel/cpu/idt.h>
#include <kernel/cpu/asm.h>
#include <kernel/tty/tty.h>
#include <kernel/device/pit.h>

pcs_process _processes[1];    // only used by the trampoline
uint64_t _pcs_stack_addr = STACK_VIRTUAL_ADDR;
dll_list pcs_live_process_list;
dll_list pcs_dead_process_list;
pcs_process* pcs_current_process;
uint64_t pcs_next_id = 0;

void system_init_main() {
  pit_init();
  pcs_create_process(sh_start, "shell");
  pcs_die();
}

void* pcs_allocate_stack() {
  _pcs_stack_addr += PCS_STACK_SIZE;
  return (void*) _pcs_stack_addr;
}

bool pcs_init() {
  // the stacks need to be fully allocated since we cannot rely on page faults (they use the stack!)
//  uint32_t stacks_size = cpu_core_count() * PCS_STACK_SIZE;
//  try(mem_allocate_range(_pcs_stack_addr, stacks_size), false, "");
//  mem_update_range(_pcs_stack_addr, 0, stacks_size, MEMORY_STACKS);

  pcs_current_process = pcs_create_process(system_init_main, "init");
  _processes[0] = *pcs_current_process;

  return true;
}

pcs_process* pcs_schedule_round_robin() {
  pcs_process* next = pcs_current_process->node.next != NULL
                      ? (pcs_process*) pcs_current_process->node.next
                      : (pcs_process*) pcs_current_process->node.list->head;
  // if the process died move it to the dead list
  if (pcs_current_process->state == DEAD) {
    dll_remove((dll_node*) pcs_current_process);
    dll_add_tail(&pcs_dead_process_list, (dll_node*) pcs_current_process);
  }
  return next;
}

interrupt_frame* pcs_handle_scheduling(interrupt_frame* frame) {
  pcs_current_process->frame = frame;
  pcs_current_process = pcs_schedule_round_robin();
  return pcs_current_process->frame;
}

pcs_process* pcs_create_process(void (* main)(), char* name) {
  pcs_process* process;
  if (pcs_dead_process_list.size != 0) {
    // reuse a dead process
    process = (pcs_process*) dll_remove_tail(&pcs_dead_process_list);
  } else {
    // create a new process with a new stack
    process = kmalloc(sizeof(pcs_process));
    process->stack = pcs_allocate_stack();
  }

  memset(process->stack - PCS_STACK_SIZE, 0, PCS_STACK_SIZE);  // clear the stack

  process->id = pcs_next_id++;
  process->name = name;
  process->state = RUNNING;
  process->main = main;
  interrupt_frame* f = process->stack - sizeof(interrupt_frame);
  process->frame = f;
  __cr(f->cr0, f->cr2, f->cr3, f->cr4);
  f->es = DATA_SEG;
  f->cs = CODE_SEG;
  f->ds = DATA_SEG;
  f->fs = DATA_SEG;
  f->gs = DATA_SEG;
  f->ss = DATA_SEG;
  f->rfl.IF = 1; // enable interrupts
  f->rsp = f->rbp = (uint64_t) process->stack - 8;
  f->rip = (uint64_t) main;

  dll_add_tail(&pcs_live_process_list, (dll_node*) process);
  return process;
}

void pcs_die() {
  pcs_current_process->state = DEAD;
  INT(PIT_VECTOR);  // trigger the interrupt so the task is scheduled away
}

void pcs_join_process(pcs_process* process) {
  while (process->state == RUNNING);  // wait for process to terminate
}

void rotate(uint64_t count, int delay_ms, int pos) {
  char* seq = "|/-\\";
  for (uint64_t n = 0, i = 0; n < count; n++, i = (i + 1) % 4) {
    tty_putchar_xy(pos, 0, seq[i]);
    cpu_delay_ms(delay_ms);
  }
  tty_putchar_xy(pos, 0, ' ');
}

char* state_names[] = {"running", "dead"};

void pcs_print_processes(__unused int argc, __unused char** argv) {
  printf(" %6s %10s %16s\n", "ID", "State", "Stack Addr");
  printf(" %6s %10s %16s\n", "======", "========", "============");
  for (pcs_process* p = (pcs_process*) pcs_live_process_list.head; p != NULL; p = (pcs_process*) p->node.next) {
    printf(" %6ld %10s %16lx  %s\n", p->id, state_names[p->state], (uint64_t) p->stack, p->name);
  }
}

void spin_main() {
  printf("Spinning...");
  rotate(20, 100, 101);
  printf("done.\n");
  pcs_die();
}

void spin(__unused int argc, __unused char** argv) {
  pcs_join_process(pcs_create_process(spin_main, "spin"));
}
