#include <stdint.h>
#include <sys/io.h>

#include <kernel/tty/vga_ttyd.h>

tty_device vga_tty_device;

uint16_t* vga_fb;
uint8_t vga_tty_fg;
uint8_t vga_tty_bg;

typedef union {
  uint16_t value;
  struct {
    uint16_t ch: 8;
    uint16_t fg: 4;
    uint16_t bg: 4;
  };
} __attribute__((packed)) vga_ttyd_entry;

uint32_t vga_ttyd_get_width() {
  return VGA_TEXT_MODE_WIDTH;
}

uint32_t vga_ttyd_get_height() {
  return VGA_TEXT_MODE_HEIGHT;
}

void vga_ttyd_put_char(uint32_t x, uint32_t y, uint8_t ch) {
  vga_ttyd_entry entry = { .ch = ch, .fg = vga_tty_fg, .bg = vga_tty_bg };
  vga_fb[y * VGA_TEXT_MODE_WIDTH + x] = entry.value;
}

void vga_ttyd_set_fg(tty_color fg) {
  vga_tty_fg = fg;
}

void vga_ttyd_set_bg(tty_color bg) {
  vga_tty_bg = bg;
}

void vga_ttyd_enable_cursor() {
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void vga_ttyd_disable_cursor() {
  outb(0x3D4, 0x0A);
  outb(0x3D5, 0x20);
}

void vga_ttyd_set_cursor(uint32_t x, uint32_t y) {
  uint16_t pos = y * VGA_TEXT_MODE_WIDTH + x;
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t) (pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void vga_ttyd_clear() {
  uint64_t* fb_64 = (uint64_t*) vga_fb;
  vga_ttyd_entry entry = { .ch = ' ', .fg = vga_tty_fg, .bg = vga_tty_bg };
  uint64_t raw_entry = (uint64_t) entry.value;
  for (uint64_t i = 0; i < vga_ttyd_get_width() * vga_ttyd_get_height() / 4; i++) {
    fb_64[i] = raw_entry << 48 | raw_entry << 32 | raw_entry << 16 | raw_entry;
  }
}

tty_device vga_ttyd_init(uint64_t fb) {
  vga_fb = (uint16_t*) fb;
  vga_tty_device.get_width = vga_ttyd_get_width;
  vga_tty_device.get_height = vga_ttyd_get_height;
  vga_tty_device.put_char = vga_ttyd_put_char;
  vga_tty_device.set_fg = vga_ttyd_set_fg;
  vga_tty_device.set_bg = vga_ttyd_set_bg;
  vga_tty_device.enable_cursor = vga_ttyd_enable_cursor;
  vga_tty_device.disable_cursor = vga_ttyd_disable_cursor;
  vga_tty_device.set_cursor = vga_ttyd_set_cursor;
  vga_tty_device.clear = vga_ttyd_clear;
  return vga_tty_device;
}

tty_device vga_ttyd_get_device() {
  return vga_tty_device;
}
