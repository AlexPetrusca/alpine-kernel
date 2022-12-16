#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <sys/circ_buf.h>
#include <kernel/kb.h>
#include <kernel/tty.h>
#include <kernel/sh.h>

#define MAX_HISTORY 5
#define MAX_COMMAND 4096

char history_buf[(MAX_HISTORY + 1) * (MAX_COMMAND + 1)];
struct circ_buf history_circ_buf;

int16_t prompt_limit;
size_t history_idx;
size_t command_idx;

static inline char * shell_command_buffer() {
    return circ_buf_peek(&history_circ_buf);
}

static inline void shell_write_command_buffer(char ch) {
    shell_command_buffer()[command_idx] = ch;
}

int shell_printf(const char * format, ...) {
    // todo: can we drop use of terminal_getline
    int pre_line = terminal_getline();
    
    int written;
    va_list arg;
	va_start(arg, format);
    written = vprintf(format, arg);
    va_end(arg);

    int post_line = terminal_getline();

    if (post_line > pre_line) {
        prompt_limit -= VGA_WIDTH * (post_line - pre_line);
    }

    return written;
}

void shell_prompt() {
    if (terminal_get_cursor_pos() % VGA_WIDTH != 0) {
        shell_printf("\n");
    }
    shell_printf("$ ");
    prompt_limit = terminal_get_cursor_pos();
    history_idx = 0;
    command_idx = 0;
}

void shell_load_history() {
    terminal_delete(command_idx);

    char * command = shell_command_buffer();
    if (history_idx == 0) {
        command[0] = '\0';
        command_idx = 0;
    } else {
        strcpy(command, circ_buf_peekn(&history_circ_buf, history_idx));
        command_idx = strlen(command);
    }
    terminal_set_cursor_pos(prompt_limit);
    shell_printf("%s", command);
}

void shell_execute() {
    shell_printf("\n");
    shell_write_command_buffer('\0');
    char * command = shell_command_buffer();
    // todo: instead, check if input contains only whitespace
    if (command_idx > 0) {
        if (strequ(command, "history")) {
            for (size_t i = 1; i < history_circ_buf.size; i++) {
                printf("%d: %s\n", i, circ_buf_peekn(&history_circ_buf, i));
            }
        } else {
            shell_printf("ash: command not found: %s", command);
        }
        // todo: if command "ls" is executed multiple times, history should only contain "ls" once
        circ_buf_push(&history_circ_buf, "\0");
    }
    shell_prompt();
}

void shell_right_arrow() {
    uint16_t cursor_pos = terminal_get_cursor_pos();
    if (cursor_pos != VGA_HEIGHT * VGA_WIDTH - 1) {
        if (cursor_pos < prompt_limit + command_idx) {
            cursor_pos++;
        }
    } else {
        cursor_pos = (VGA_HEIGHT - 1) * VGA_WIDTH;
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
        cursor_pos = VGA_WIDTH - 1;
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
    // todo: fix backspace at shell_limit
    terminal_delete(1);
    command_idx--;
    shell_write_command_buffer('\0');
}

void shell_handle_input(int ch) {
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
            shell_execute();
            break;
        default:
            if (command_idx < MAX_COMMAND) {
                shell_printf("%c", ch);
                shell_write_command_buffer(ch);
                command_idx++;
            }
    }
}

void shell_initialize() {;
    history_circ_buf.buf = history_buf;
    history_circ_buf.capacity = MAX_HISTORY + 1;
    history_circ_buf.granularity = MAX_COMMAND;
    circ_buf_push(&history_circ_buf, "\0");

    terminal_enable_cursor(14, 15);
    shell_printf("Welcome to alpine shell!\n\n");
    shell_prompt();
}

void shell_start() {
    shell_initialize();
	while (true) {
        int ch = keyboard_getchar();
        shell_handle_input(ch);
	}
}