#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

#include <kernel/vga.h>

void terminal_initialize();
void terminal_scroll_up();
void terminal_scroll_down();
void terminal_setcolor(uint8_t color);
uint16_t terminal_getline();

void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void terminal_disable_cursor();
uint16_t terminal_get_cursor_pos();
void terminal_set_cursor_pos(uint16_t pos);
void terminal_set_cursor_pos_xy(uint8_t x, uint8_t y);

char terminal_getchar(uint16_t pos);
char * terminal_getbuffer(char * s, uint16_t offset, uint16_t n);
void terminal_putchar(char c);
void terminal_write(const char * data, size_t size);
void terminal_writestring(const char * data);
void terminal_clear();
void terminal_clearn(uint16_t pos, uint16_t n);

#endif
