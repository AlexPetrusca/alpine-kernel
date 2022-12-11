#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <kernel/tty.h>
#include <sys/io.h>

#define TAB_LENGTH 8

static uint8_t cursor_y;
static uint8_t cursor_x;
static uint8_t terminal_color;
static uint16_t * terminal_buffer;

void terminal_initialize() {
	cursor_y = 0;
	cursor_x = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t *) VGA_MEMORY;
	terminal_clear();
}

void terminal_clear_row(uint8_t row) {
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		terminal_buffer[row * VGA_WIDTH + i] = vga_entry(' ', terminal_color);
	}
}

void terminal_clear() {
	for (size_t i = 0; i < VGA_HEIGHT; i++) {
		terminal_clear_row(i);
	}
}

void terminal_clearn(uint16_t pos, uint16_t n) {
	for (size_t i = 0; i < n; i++) {
		terminal_buffer[pos + i] = vga_entry(' ', terminal_color);
	}
}

void terminal_scroll_up() {
	memmove(terminal_buffer + VGA_WIDTH, terminal_buffer, VGA_HEIGHT * VGA_WIDTH * sizeof(terminal_buffer));
	terminal_clear_row(0);
}

void terminal_scroll_down() {
	memmove(terminal_buffer, terminal_buffer + VGA_WIDTH, VGA_HEIGHT * VGA_WIDTH * sizeof(terminal_buffer));
	terminal_clear_row(VGA_HEIGHT - 1);
}

void terminal_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void terminal_disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void terminal_set_cursor_pos(uint16_t pos) {
	terminal_set_cursor_pos_xy(pos % VGA_WIDTH, pos / VGA_WIDTH);
}

void terminal_set_cursor_pos_xy(uint8_t x, uint8_t y) {
	cursor_x = x;
	cursor_y = y;
	uint16_t pos = y * VGA_WIDTH + x;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

uint16_t terminal_get_cursor_pos() {
    return cursor_y * VGA_WIDTH + cursor_x;
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(unsigned char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

char * terminal_getbuffer(char * s, uint16_t offset, uint16_t n) {
	for (size_t i = 0; i < n; i++) {
		s[i] = terminal_buffer[i + offset];
	}
	return s;
}

char terminal_getchar(uint16_t pos) {
	return terminal_buffer[pos];
}

void terminal_putchar(char c) {
	switch (c) {
		case '\t':
			while ((cursor_x + 1) % TAB_LENGTH != 0) {
				cursor_x++;
			}
			if (cursor_x < VGA_WIDTH - 1) {
				cursor_x++;
			}
			break;
		case '\b':
			cursor_x -= (cursor_x == 0) ? 0 : 1;
			break;
		case '\n':
			cursor_y++;
			cursor_x = 0;
			break;
		default:
			if (cursor_x == VGA_WIDTH) {
				cursor_x = 0;
				cursor_y++;
			}
			unsigned char uc = c;
			terminal_putentryat(uc, terminal_color, cursor_x, cursor_y);
			cursor_x++;
			break;
	}
	if (cursor_y >= VGA_HEIGHT) {
		cursor_y--;
		terminal_scroll_down();
	}
	terminal_set_cursor_pos_xy(cursor_x, cursor_y);
}

void terminal_write(const char * data, size_t size) {
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
}

void terminal_writestring(const char * data) {
	terminal_write(data, strlen(data));
}
