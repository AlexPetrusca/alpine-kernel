#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <kernel/boot/mb2_type.h>
#include <kernel/tty/ttyd.h>

void tty_init(mb2_tag_framebuffer* fb_tag);
uint32_t tty_get_width();
uint32_t tty_get_height();

void tty_scroll_up();
void tty_scroll_down();
void tty_setcolor(tty_color fg, tty_color bg);
uint32_t tty_getline();

void tty_enable_cursor();
void tty_disable_cursor();
uint32_t tty_get_cursor_pos();
void tty_set_cursor_pos(uint64_t pos);
void tty_set_cursor_pos_xy(uint32_t x, uint32_t y);

void tty_putchar(char c);
void tty_write(const char* data, size_t size);
void tty_writestring(const char* data);
void tty_clear();
void tty_delete(size_t n);

#endif
