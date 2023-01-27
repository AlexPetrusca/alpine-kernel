#include <stdio.h>

#include <sys/io.h>
#include <kernel/cpu/isr.h>
#include <kernel/device/kb.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/asm.h>
#include <kernel/cpu/pic.h>
#include <kernel/mem/pgm.h>
#include <kernel/tty/tty.h>

uint64_t keyboard_interrupt_count = 0;
uint64_t page_fault_interrupt_count = 0;

void default_isr(const interrupt_frame* frame) {
  char flags[128];
  sprintf(flags, "[%s%s%s%s%s%s%s%s%s] IOPL=%d %s %s %s %s %s %s %s %s",
          frame->rfl.OF ? "O" : "-",
          frame->rfl.DF ? "D" : "-",
          frame->rfl.IF ? "I" : "-",
          frame->rfl.TF ? "T" : "-",
          frame->rfl.SF ? "S" : "-",
          frame->rfl.ZF ? "Z" : "-",
          frame->rfl.AF ? "A" : "-",
          frame->rfl.PF ? "P" : "-",
          frame->rfl.CF ? "C" : "-",
          frame->rfl.IOPL,
          frame->rfl.NT ? "NT" : "",
          frame->rfl.MD ? "MD" : "",
          frame->rfl.RF ? "RF" : "",
          frame->rfl.VM ? "VM" : "",
          frame->rfl.AC ? "AC" : "",
          frame->rfl.VIF ? "VIF" : "",
          frame->rfl.VIP ? "VIP" : "",
          frame->rfl.ID ? "ID" : ""
  );

  panic(
    "Unhandled interrupt 0x%02lx, e=%04lx\n"
    "RAX=%016lx RBX=%016lx RCX=%016lx RDX=%016lx \n"
    "RSI=%016lx RDI=%016lx RBP=%016lx RSP=%016lx \n"
    "R8 =%016lx R9 =%016lx R10=%016lx R11=%016lx \n"
    "R12=%016lx R13=%016lx R14=%016lx R15=%016lx \n"
    "RIP=%016lx RFL=%016lx %s\n"
    "ES =%04lx \n"
    "CS =%04lx \n"
    "SS =%04lx \n"
    "DS =%04lx \n"
    "FS =%04lx \n"
    "GS =%04lx \n"
    "CR0=%08lx CR2=%016lx CR3=%016lx CR4=%08lx \n",
    frame->vector_num, frame->error_code,
    frame->rax, frame->rbx, frame->rcx, frame->rdx,
    frame->rsi, frame->rdi, frame->rbp, frame->rsp,
    frame->r8, frame->r9, frame->r10, frame->r11,
    frame->r12, frame->r13, frame->r14, frame->r15,
    frame->rip, frame->rfl, flags,
    frame->es, frame->cs, frame->ss, frame->ds, frame->fs, frame->gs,
    frame->cr0, frame->cr2, frame->cr3, frame->cr4
  );
}

void keyboard_isr() {
  kb_enqueue_event();
  pic_eoi(PIC_KEYBOARD_IRQ); // send interrupt OK signal
}

void main_isr(interrupt_frame* frame) {
  switch (frame->vector_num) {
    case PAGE_FAULT_VECTOR:
      page_fault_interrupt_count++;
      mem_page_fault_handler(frame);
      break;
    case KEYBOARD_VECTOR:
      keyboard_interrupt_count++;
      keyboard_isr();
      break;
    default:
      default_isr(frame);
  }
}

void isr_print_info(__unused int argc, __unused char** argv) {
  printf(" %12s %12s\n", "ISR", "Count");
  printf(" %12s %12s\n", "==========", "========");
  printf(" %12s %12ld\n", "Page Fault", page_fault_interrupt_count);
  printf(" %12s %12ld\n", "Keyboard", keyboard_interrupt_count);
}
