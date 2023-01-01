#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>

#include <kernel/mb2_type.h>
#include <kernel/ttyd.h>

void terminal_init(mb2_tag_framebuffer* fb_tag);
uint32_t terminal_get_width();
uint32_t terminal_get_height();

void terminal_scroll_up();
void terminal_scroll_down();
void terminal_setcolor(tty_color fg, tty_color bg);
uint16_t terminal_getline();

void terminal_enable_cursor();
void terminal_disable_cursor();
uint16_t terminal_get_cursor_pos();
void terminal_set_cursor_pos(uint64_t pos);
void terminal_set_cursor_pos_xy(uint32_t x, uint32_t y);

void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_clear();
void terminal_delete(size_t n);

#endif
