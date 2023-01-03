#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <sys/vector.h>
#include <sys/circ_buf.h>
#include <kernel/kb.h>
#include <kernel/tty.h>
#include <kernel/sh.h>

#include <kernel/cpu.h>
#include <kernel/acpi.h>
#include <kernel/apic.h>
#include <kernel/pci.h>
#include <kernel/mem.h>
#include <kernel/mb2_info.h>
#include <kernel/usb.h>
#include <kernel/psf_font.h>
#include <kernel/tests.h>

#define MAX_HISTORY 5
#define MAX_COMMAND 4096

typedef struct {
  char* name;
  void (* run)(int argc, char** argv);
} sh_command;

sh_command commands[] = {
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

char history_buf[(MAX_HISTORY + 1) * (MAX_COMMAND + 1)];
struct circ_buf history_circ_buf;

int16_t prompt_limit;
size_t history_idx;
size_t command_idx;

static inline char* shell_command_buffer() {
  return circ_buf_peek(&history_circ_buf);
}

static inline void shell_write_command_buffer(char ch) {
  shell_command_buffer()[command_idx] = ch;
}

void shell_command_printf(const char* format, ...) {
  int pre_line = terminal_getline();

  va_list arg;
  va_start(arg, format);
  vprintf(format, arg);
  va_end(arg);

  int post_line = terminal_getline();

  if (post_line > pre_line) {
    prompt_limit -= terminal_get_width() * (post_line - pre_line);
  }
}

void shell_prompt() {
  if (terminal_get_cursor_pos() % terminal_get_width() != 0) {
    printf("\n");
  }
  printf("$ ");
  prompt_limit = terminal_get_cursor_pos();
  history_idx = 0;
  command_idx = 0;
}

void shell_load_history() {
  terminal_delete(command_idx);

  char* command = shell_command_buffer();
  if (history_idx == 0) {
    command[0] = '\0';
    command_idx = 0;
  } else {
    strcpy(command, circ_buf_peekn(&history_circ_buf, history_idx));
    command_idx = strlen(command);
  }
  terminal_set_cursor_pos(prompt_limit);
  shell_command_printf("%s", command);
}

sh_command* shell_find_command(char* name) {
  for (sh_command* cmd = commands; cmd->run != NULL; cmd++) {
    if (strcmp(cmd->name, name) == 0) {
      return cmd;
    }
  }
  return NULL;
}

void shell_print_commands() {
  for (sh_command* cmd = commands; cmd->run != NULL; cmd++) {
    printf("%s ", cmd->name);
  }
  printf("\n");
}

vector* shell_strsplit(char* str, const char* delim) {
  char copy[MAX_COMMAND];
  strcpy(copy, str);

  vector* args_vec = vector_new();
  char* token = strtok(copy, delim);
  while (token != NULL) {
    vector_add(args_vec, token);
    token = strtok(NULL, delim);
  }
  return args_vec;
}

bool shell_execute() {
  printf("\n");
  shell_write_command_buffer('\0');
  char* command_line = shell_command_buffer();
  sh_command* cmd = NULL;
  // todo: instead, check if input contains only whitespace
  if (command_idx > 0) {
    vector* args = shell_strsplit(command_line, " ");
    char* command = vector_get(args, 0);
    if (strequ(command, "history")) {
      for (size_t i = 1; i < history_circ_buf.size; i++) {
        printf("%d: %s\n", i, (char*) circ_buf_peekn(&history_circ_buf, i));
      }
    } else if (strequ(command, "clear")) {
      terminal_clear();
    } else if (strequ(command, "help")) {
      shell_print_commands();
    } else if (strequ(command, "exit")) {
      printf("Alpine shell terminated.");
      return false;
    } else if ((cmd = shell_find_command(command))) {
      cmd->run(args->length, (char**) args->items);
    } else {
      printf("ash: command not found: %s", command);
    }
    // todo: if command "ls" is executed multiple times, history should only contain "ls" once
    circ_buf_push(&history_circ_buf, "\0");
  }
  shell_prompt();
  return true;
}

void shell_right_arrow() {
  uint16_t cursor_pos = terminal_get_cursor_pos();
  if (cursor_pos != terminal_get_height() * terminal_get_width() - 1) {
    if (cursor_pos < prompt_limit + command_idx) {
      cursor_pos++;
    }
  } else {
    cursor_pos = (terminal_get_height() - 1) * terminal_get_width();
    terminal_scroll_down();
  }
  terminal_set_cursor_pos(cursor_pos);
}

void shell_up_arrow() {
  if (keyboard_is_pressed(OPTION)) {
    terminal_scroll_up();
  } else if ((history_idx == 0 && command_idx == 0) || history_idx > 0) {
    if (history_idx < history_circ_buf.size - 1) {
      history_idx++;
      shell_load_history();
    }
  }
}

void shell_left_arrow() {
  uint16_t cursor_pos = terminal_get_cursor_pos();
  if (cursor_pos != 0) {
    if (cursor_pos > prompt_limit) {
      cursor_pos--;
    }
  } else {
    cursor_pos = terminal_get_width() - 1;
    terminal_scroll_up();
  }
  terminal_set_cursor_pos(cursor_pos);
}

void shell_down_arrow() {
  if (keyboard_is_pressed(OPTION)) {
    terminal_scroll_down();
  } else if (history_idx > 0) {
    history_idx--;
    shell_load_history();
  }
}

void shell_tab() {
  // todo: implement TAB
}

void shell_backspace() {
  if (terminal_get_cursor_pos() > prompt_limit) {
    command_idx--;
    shell_write_command_buffer('\0');
    terminal_delete(1);
  }
}

bool shell_handle_input(int ch) {
  switch (ch) {
    case CONTROL ... SCROLL_LOCK:
    case F1 ... F12:
    case ESC:
      break;
    case RIGHT_ARROW:
      shell_right_arrow();
      break;
    case UP_ARROW:
      shell_up_arrow();
      break;
    case LEFT_ARROW:
      shell_left_arrow();
      break;
    case DOWN_ARROW:
      shell_down_arrow();
      break;
    case '\t':
      shell_tab();
      break;
    case '\b':
      shell_backspace();
      break;
    case '\n':
      return shell_execute();
    default:
      if (command_idx < MAX_COMMAND) {
        shell_command_printf("%c", ch);
        shell_write_command_buffer(ch);
        command_idx++;
      }
  }
  return true;
}

void shell_initialize() {
  history_circ_buf.buf = history_buf;
  history_circ_buf.capacity = MAX_HISTORY + 1;
  history_circ_buf.granularity = MAX_COMMAND;
  circ_buf_push(&history_circ_buf, "\0");

  terminal_enable_cursor();
  printf("Welcome to alpine shell!\n\n");
  shell_prompt();
}

void shell_start() {
  shell_initialize();
  while (shell_handle_input(keyboard_getchar()));
}