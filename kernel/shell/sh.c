#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <sys/vector.h>
#include <sys/circ_buf.h>
#include <kernel/device/kb.h>
#include <kernel/tty/tty.h>
#include <kernel/shell/sh.h>

#include <kernel/cpu/cpu.h>
#include <kernel/device/acpi.h>
#include <kernel/device/apic.h>
#include <kernel/device/pci.h>
#include <kernel/mem/mem.h>
#include <kernel/boot/mb2_info.h>
#include <kernel/device/usb.h>
#include <kernel/font/psf_font.h>
#include <kernel/test/tests.h>

#define SH_MAX_HISTORY 5
#define SH_MAX_COMMAND 4096

typedef struct {
  char* name;
  void (* run)(int argc, char** argv);
} sh_command;

sh_command sh_commands[] = {
    {"cpu",  cpu_print_info},
    {"test", tests_run},
    {"acpi", acpi_print_info},
    {"apic", apic_print_info},
    {"mcfg", pci_print_mcfg},
    {"pci",  pci_print_devices},
    {"mmap", mem_print_map},
    {"mbi",  mb2_info_print},
    {"fb",   mb2_fb_info_print},
    {"usb",  usb_print_info},
    {"font", psf_font_info_print},
    {"", NULL}
};;

char sh_history_buf[(SH_MAX_HISTORY + 1) * (SH_MAX_COMMAND + 1)];
struct circ_buf sh_history_circ_buf;

uint32_t sh_prompt_limit;
uint32_t sh_history_idx;
uint32_t sh_command_idx;

static inline char* sh_command_buffer() {
  return circ_buf_peek(&sh_history_circ_buf);
}

static inline void sh_write_command_buffer(char ch) {
  sh_command_buffer()[sh_command_idx] = ch;
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

void sh_load_history() {
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

vector* sh_strsplit(char* str, const char* delim) {
  char copy[SH_MAX_COMMAND];
  strcpy(copy, str);

  vector* args_vec = vector_new();
  char* token = strtok(copy, delim);
  while (token != NULL) {
    vector_add(args_vec, token);
    token = strtok(NULL, delim);
  }
  return args_vec;
}

bool sh_execute() {
  printf("\n");
  sh_write_command_buffer('\0');
  char* command_line = sh_command_buffer();
  sh_command* cmd = NULL;
  // todo: instead, check if input contains only whitespace
  if (sh_command_idx > 0) {
    vector* args = sh_strsplit(command_line, " ");
    char* command = vector_get(args, 0);
    if (strequ(command, "history")) {
      for (size_t i = 1; i < sh_history_circ_buf.size; i++) {
        printf("%d: %s\n", i, (char*) circ_buf_peekn(&sh_history_circ_buf, i));
      }
    } else if (strequ(command, "clear")) {
      tty_clear();
    } else if (strequ(command, "help")) {
      sh_print_commands();
    } else if (strequ(command, "exit")) {
      printf("Alpine shell terminated.");
      return false;
    } else if ((cmd = sh_find_command(command))) {
      cmd->run(args->length, (char**) args->items);
    } else {
      printf("ash: command not found: %s", command);
    }
    // todo: if command "ls" is executed multiple times, history should only contain "ls" once
    circ_buf_push(&sh_history_circ_buf, "\0");
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
  if (kb_is_pressed(OPTION)) {
    tty_scroll_up();
  } else if ((sh_history_idx == 0 && sh_command_idx == 0) || sh_history_idx > 0) {
    if (sh_history_idx < sh_history_circ_buf.size - 1) {
      sh_history_idx++;
      sh_load_history();
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
  if (kb_is_pressed(OPTION)) {
    tty_scroll_down();
  } else if (sh_history_idx > 0) {
    sh_history_idx--;
    sh_load_history();
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

bool sh_handle_input(int ch) {
  switch (ch) {
    case CONTROL ... SCROLL_LOCK:
    case F1 ... F12:
    case ESC:
      break;
    case RIGHT_ARROW:
      sh_right_arrow();
      break;
    case UP_ARROW:
      sh_up_arrow();
      break;
    case LEFT_ARROW:
      sh_left_arrow();
      break;
    case DOWN_ARROW:
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