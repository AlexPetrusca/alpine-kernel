#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/circ_buf.h>
#include <sys/strsplit.h>
#include <kernel/device/kb.h>
#include <kernel/tty/tty.h>
#include <kernel/shell/sh.h>

// sh command includes
#include <kernel/device/acpi.h>
#include <kernel/device/apic.h>
#include <kernel/device/pci.h>
#include <kernel/device/usb.h>
#include <kernel/boot/mb2_info.h>
#include <kernel/font/psf_font.h>
#include <kernel/test/tests.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/gdt.h>
#include <kernel/cpu/idt.h>
#include <kernel/mem/mem.h>
#include <kernel/mem/heap.h>
#include <kernel/boot/elf.h>

#define SH_MAX_HISTORY 5
#define SH_MAX_COMMAND 4096

typedef struct {
  char* name;
  void (* run)(int argc, char** argv);
} sh_command;

void div0(int argc, __unused char** argv) {
  printf("%ld", 100L / (argc - 1));
}

sh_command sh_commands[] = {
  {"cpu", cpu_print_info},
  {"test", tests_run},
  {"acpi", acpi_print_info},
  {"apic", apic_print_info},
  {"lapic", apic_print_lapic_info},
  {"mcfg", pci_print_mcfg},
  {"pci", pci_print_devices},
  {"mmap", mem_print_map},
  {"pt", mem_print_pt_pgm},
  {"pgm", mem_print_main_pgm},
  {"mbi", mb2_info_print},
  {"fb", mb2_fb_info_print},
  {"usb", usb_print_info},
  {"font", psf_font_info_print},
  {"heap", heap_print_info},
  {"gdt", gdt_print_info},
  {"idt", idt_print_info},
  {"int", idt_interrupt},
  {"memdump", mem_memdump},
  {"elf", elf_print_info},
  {"isr", isr_print_info},
  {"div0", div0},
  {"", NULL}
};

char sh_history_buf[(SH_MAX_HISTORY + 1) * (SH_MAX_COMMAND + 1)];
circ_buf sh_history_circ_buf;

uint32_t sh_prompt_limit;
uint32_t sh_history_idx;
uint32_t sh_command_idx;

static inline char* sh_command_buffer() {
  return circ_buf_peek(&sh_history_circ_buf);
}

static inline void sh_write_command_buffer(unsigned char ch) {
  sh_command_buffer()[sh_command_idx] = (char) ch;
}

void sh_command_printf(const char* format, ...) {
  uint32_t pre_line = tty_getline();

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  uint32_t post_line = tty_getline();

  if (post_line > pre_line) {
    sh_prompt_limit -= tty_get_width() * (post_line - pre_line);
  }
}

void sh_prompt() {
  if (tty_get_cursor_pos() % tty_get_width() != 0) {
    printf("\n");
  }
  printf("$ ");
  sh_prompt_limit = tty_get_cursor_pos();
  sh_history_idx = 0;
  sh_command_idx = 0;
}

void sh_push_history() {
  char* cmdline = sh_command_buffer();
  void* last_cmdline = circ_buf_peekn(&sh_history_circ_buf, 1);
  if (!strequ(cmdline, last_cmdline)) {
    circ_buf_push(&sh_history_circ_buf, "\0");
  }
}

void sh_pull_history() {
  tty_delete(sh_command_idx);
  char* command = sh_command_buffer();
  if (sh_history_idx == 0) {
    command[0] = '\0';
    sh_command_idx = 0;
  } else {
    strcpy(command, circ_buf_peekn(&sh_history_circ_buf, sh_history_idx));
    sh_command_idx = strlen(command);
  }
  tty_set_cursor_pos(sh_prompt_limit);
  sh_command_printf("%s", command);
}

sh_command* sh_find_command(char* name) {
  for (sh_command* cmd = sh_commands; cmd->run != NULL; cmd++) {
    if (strcmp(cmd->name, name) == 0) {
      return cmd;
    }
  }
  return NULL;
}

void sh_print_commands() {
  for (sh_command* cmd = sh_commands; cmd->run != NULL; cmd++) {
    printf("%s ", cmd->name);
  }
  printf("\n");
}

void sh_print_history() {
  for (size_t i = 1; i < sh_history_circ_buf.size; i++) {
    printf("%lu: %s\n", i, (char*) circ_buf_peekn(&sh_history_circ_buf, i));
  }
}

bool sh_exit() {
  printf("Alpine shell terminated.");
  return false;
}

bool sh_execute_command(int argc, char** argv) {
  sh_command* sh_cmd = NULL;
  if (argc > 0) {
    char* cmd = argv[0];
    if (strequ(cmd, "help")) {
      sh_print_commands();
    } else if (strequ(cmd, "history")) {
      sh_print_history();
    } else if (strequ(cmd, "clear")) {
      tty_clear();
    } else if (strequ(cmd, "exit")) {
      return sh_exit();
    } else if ((sh_cmd = sh_find_command(cmd))) {
      sh_cmd->run(argc, argv);
    } else {
      printf("ash: command not found: %s", cmd);
    }
    sh_push_history();
  }
  return true;
}

bool sh_execute() {
  printf("\n");
  sh_write_command_buffer('\0');
  char* cmdline = sh_command_buffer();
  if (sh_command_idx > 0) {
    char* cmdline_copy = strdup(cmdline);
    vector* args = strsplit(cmdline_copy, " ");
    if (!sh_execute_command(args->length, (char**) args->items)) {
      return false;
    };
    free(cmdline_copy);
    vector_free(args);
  }
  sh_prompt();
  return true;
}

void sh_right_arrow() {
  uint32_t cursor_pos = tty_get_cursor_pos();
  if (cursor_pos != tty_get_height() * tty_get_width() - 1) {
    if (cursor_pos < sh_prompt_limit + sh_command_idx) {
      cursor_pos++;
    }
  } else {
    cursor_pos = (tty_get_height() - 1) * tty_get_width();
    tty_scroll_down();
  }
  tty_set_cursor_pos(cursor_pos);
}

void sh_up_arrow() {
  if (kb_is_pressed(KB_OPTION)) {
    tty_scroll_up();
  } else if ((sh_history_idx == 0 && sh_command_idx == 0) || sh_history_idx > 0) {
    if (sh_history_idx < sh_history_circ_buf.size - 1) {
      sh_history_idx++;
      sh_pull_history();
    }
  }
}

void sh_left_arrow() {
  uint32_t cursor_pos = tty_get_cursor_pos();
  if (cursor_pos != 0) {
    if (cursor_pos > sh_prompt_limit) {
      cursor_pos--;
    }
  } else {
    cursor_pos = tty_get_width() - 1;
    tty_scroll_up();
  }
  tty_set_cursor_pos(cursor_pos);
}

void sh_down_arrow() {
  if (kb_is_pressed(KB_OPTION)) {
    tty_scroll_down();
  } else if (sh_history_idx > 0) {
    sh_history_idx--;
    sh_pull_history();
  }
}

void sh_tab() {
  // todo: implement TAB
}

void sh_backspace() {
  if (tty_get_cursor_pos() > sh_prompt_limit) {
    sh_command_idx--;
    sh_write_command_buffer('\0');
    tty_delete(1);
  }
}

bool sh_handle_input(unsigned char ch) {
  switch (ch) {
    case KB_CONTROL ... KB_SCROLL_LOCK:
    case KB_F1 ... KB_F12:
    case KB_ESC:
      break;
    case KB_RIGHT_ARROW:
      sh_right_arrow();
      break;
    case KB_UP_ARROW:
      sh_up_arrow();
      break;
    case KB_LEFT_ARROW:
      sh_left_arrow();
      break;
    case KB_DOWN_ARROW:
      sh_down_arrow();
      break;
    case '\t':
      sh_tab();
      break;
    case '\b':
      sh_backspace();
      break;
    case '\n':
      return sh_execute();
    default:
      if (sh_command_idx < SH_MAX_COMMAND) {
        sh_command_printf("%c", ch);
        sh_write_command_buffer(ch);
        sh_command_idx++;
      }
  }
  return true;
}

void sh_initialize() {
  sh_history_circ_buf.buf = sh_history_buf;
  sh_history_circ_buf.capacity = SH_MAX_HISTORY + 1;
  sh_history_circ_buf.granularity = SH_MAX_COMMAND;
  circ_buf_push(&sh_history_circ_buf, "\0");

  tty_enable_cursor();
  printf("Welcome to alpine shell!\n\n");
  sh_prompt();
}

void sh_start() {
  sh_initialize();
  while (sh_handle_input(kb_getchar()));
}