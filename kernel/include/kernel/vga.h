#ifndef _KERNEL_VGA_H
#define _KERNEL_VGA_H

#include <stddef.h>
#include <stdint.h>
#include <sys/io.h>
#include <kernel/td.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

uint32_t get_width() {
  return VGA_WIDTH;
}

uint32_t get_height() {
  return VGA_HEIGHT;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t attr) {
  return (uint16_t) uc | (uint16_t) attr << 8;
}

void put_char(uint32_t x, uint32_t y, uint8_t ch, uint8_t attr) {
  ((uint16_t*) VGA_MEMORY)[y * VGA_WIDTH + x] = vga_entry(ch, attr);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor() {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}

void set_cursor(uint32_t x, uint32_t y) {
  uint16_t pos = y * VGA_WIDTH + x;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t) (pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void clear() {
  for (size_t y = 0; y < get_height(); y++) {
    for (size_t x = 0; x < get_width(); x++) {
      put_char(x, y, ' ', attributes(COLOR_LIGHT_GREEN, COLOR_BLACK));
    }
  }
}

tty_device vga_tty_device = {get_width, get_height, put_char, enable_cursor, disable_cursor, set_cursor, clear};

#endif
