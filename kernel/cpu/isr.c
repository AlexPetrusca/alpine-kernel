#include <stdio.h>

#include <kernel/cpu/isr.h>
#include <sys/io.h>
#include <kernel/device/kb.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/asm.h>
#include <kernel/mem/pgm.h>
#include <kernel/tty/tty.h>

uint64_t keyboard_interrupt_count = 0;
uint64_t page_fault_interrupt_count = 0;

void default_isr(const interrupt_frame* frame) {
  panic(
    "Unhandled interrupt 0x%02lx, e=%04lx\n"
    "RAX=%016lx RBX=%016lx RCX=%016lx RDX=%016lx \n"
    "RSI=%016lx RDI=%016lx RBP=%016lx RSP=%016lx \n"
    "R8 =%016lx R9 =%016lx R10=%016lx R11=%016lx \n"
    "R12=%016lx R13=%016lx R14=%016lx R15=%016lx \n"
    "RIP=%016lx RFL=%016lx \n"
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
    frame->rip, frame->rflags,
    frame->es, frame->cs, frame->ss, frame->ds, frame->fs, frame->gs,
    frame->cr0, frame->cr2, frame->cr3, frame->cr4
  );
//"GDT=     0000000000107000 000000a0                                                 ",
//"IDT=     0000000000000000 000003ff                                                 ",
//"EFER=0000000000000d00                                                              ",
}

void keyboard_isr() {
//  // keyboard interrupt handling
//  int scan = kb_readscan();
//  int ch = kb_scan2ch(scan);
//  if (ch == KB_NUM_LOCK) {
//    kb_num_lock = !kb_num_lock;
//  } else if (ch == KB_CAPS_LOCK) {
//    kb_caps_lock = !kb_caps_lock;
//  } else if (ch == KB_SCROLL_LOCK) {
//    kb_scroll_lock = !kb_scroll_lock;
//  }
//  if (!kb_scan2release(scan)) {
//    printf("%c", ch);
//  }

  outb(0x20, 0x20); // send interrupt OK signal
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
