#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

void terminal_initialize();
void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void terminal_disable_cursor();
void terminal_set_cursor_position(uint8_t x, uint8_t y);
uint16_t terminal_get_cursor_position();
void terminal_putchar(char c);
void terminal_write(const char * data, size_t size);
void terminal_writestring(const char * data);

#endif
