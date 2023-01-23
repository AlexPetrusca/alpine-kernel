#include <stdio.h>

#include <kernel/cpu/isr.h>
#include <sys/io.h>
#include <kernel/device/kb.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/asm.h>
#include <kernel/mem/pgm.h>

isr void default_isr(__unused interrupt_frame* frame) {
  printf("Default Interrupt Handler\n");
}

isr void keyboard_isr(__unused interrupt_frame* frame) {
//  printf(" frame: %p\n", frame);

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
