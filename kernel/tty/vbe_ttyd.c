#include <stdbool.h>
#include <stdint.h>

#include <kernel/font/psf_font.h>
#include <kernel/tty/vbe_ttyd.h>

tty_device vbe_tty_device;

uint32_t* vbe_fb;
uint32_t vbe_screen_width;
uint32_t vbe_screen_height;

uint32_t vbe_tty_fg;
uint32_t vbe_tty_bg;
psf_font* vbe_tty_font;

bool vbe_tty_cursor_enabled;
uint32_t vbe_tty_cursor_x;
uint32_t vbe_tty_cursor_y;

uint32_t vbe_ttyd_convert_color(tty_color color) {
  switch (color) {
    case COLOR_BLACK:
      return 0x000000;
    case COLOR_BLUE:
      return 0x0000AA;
    case COLOR_GREEN:
      return 0x00AA00;
    case COLOR_CYAN:
      return 0x00AAAA;
    case COLOR_RED:
      return 0xAA0000;
    case COLOR_MAGENTA:
      return 0xAA00AA;
    case COLOR_BROWN:
      return 0xAA5500;
    case COLOR_LIGHT_GREY:
      return 0xAAAAAA;
    case COLOR_DARK_GREY:
      return 0x555555;
    case COLOR_LIGHT_BLUE:
      return 0x5555FF;
    case COLOR_LIGHT_GREEN:
      return 0x55FF55;
    case COLOR_LIGHT_CYAN:
      return 0x55FFFF;
    case COLOR_LIGHT_RED:
      return 0xFF5555;
    case COLOR_LIGHT_MAGENTA:
      return 0xFF55FF;
    case COLOR_YELLOW:
      return 0xFFFF00;
    case COLOR_WHITE:
      return 0xFFFFFF;
    default:
      return 0x000000;
  }
};

uint32_t vbe_ttyd_get_width() {
  return vbe_screen_width / PSF1_CHAR_WIDTH;
}

uint32_t vbe_ttyd_get_height() {
  return vbe_screen_height / vbe_tty_font->glyph_height;
}

void vbe_ttyd_put_char(uint32_t x, uint32_t y, uint8_t ch) {
  char* glyph = vbe_tty_font->glyphs + ch * vbe_tty_font->glyph_size;
  uint32_t screen_x = x * vbe_tty_font->glyph_width;
  uint32_t screen_y = y * vbe_tty_font->glyph_height;
  for (uint32_t offset_y = 0; offset_y < vbe_tty_font->glyph_height; offset_y++) {
    if (vbe_tty_font->glyph_width <= PSF1_CHAR_WIDTH) {
      uint8_t mask = 1 << (PSF1_CHAR_WIDTH - 1);
      for (uint32_t offset_x = 0; offset_x < vbe_tty_font->glyph_width; offset_x++) {
        uint32_t screen_pos = (screen_y + offset_y) * vbe_screen_width + screen_x + offset_x;
        vbe_fb[screen_pos] = (glyph[offset_y] & mask) ? vbe_tty_fg : vbe_tty_bg;
        mask >>= 1;
      }
    } else {
      uint16_t mask = 1 << (PSF2_CHAR_WIDTH - 1);
      for (uint32_t offset_x = 0; offset_x < vbe_tty_font->glyph_width; offset_x++) {
        uint32_t screen_pos = (screen_y + offset_y) * vbe_screen_width + screen_x + offset_x;
        vbe_fb[screen_pos] = (((uint16_t*) glyph)[offset_y] & mask) ? vbe_tty_fg : vbe_tty_bg;
        mask >>= 1;
      }
    }
  }
}

void vbe_ttyd_set_fg(tty_color fg) {
  vbe_tty_fg = vbe_ttyd_convert_color(fg);
}

void vbe_ttyd_set_bg(tty_color bg) {
  vbe_tty_bg = vbe_ttyd_convert_color(bg);
}

void vbe_ttyd_enable_cursor() {
  vbe_tty_cursor_enabled = true;
}

void vbe_ttyd_disable_cursor() {
  vbe_tty_cursor_enabled = false;
}

void vbe_ttyd_set_cursor(uint32_t x, uint32_t y) {
  vbe_tty_cursor_x = x;
  vbe_tty_cursor_y = y;
}

void vbe_ttyd_clear() {
  uint64_t* fb_64 = (uint64_t*) vbe_fb;
  for (uint64_t i = 0; i < vbe_screen_width * vbe_screen_height / 2; i++) {
    fb_64[i] = (uint64_t) vbe_tty_bg << 32 | vbe_tty_bg;
  }
}

tty_device vbe_ttyd_init(uint64_t fb, uint32_t screen_width, uint32_t screen_height) {
  vbe_fb = (uint32_t*) fb;
  vbe_screen_width = screen_width;
  vbe_screen_height = screen_height;
  vbe_tty_font = psf_init_fonts();

  vbe_tty_device.get_width = vbe_ttyd_get_width;
  vbe_tty_device.get_height = vbe_ttyd_get_height;
  vbe_tty_device.put_char = vbe_ttyd_put_char;
  vbe_tty_device.set_fg = vbe_ttyd_set_fg;
  vbe_tty_device.set_bg = vbe_ttyd_set_bg;
  vbe_tty_device.enable_cursor = vbe_ttyd_enable_cursor;
  vbe_tty_device.disable_cursor = vbe_ttyd_disable_cursor;
  vbe_tty_device.set_cursor = vbe_ttyd_set_cursor;
  vbe_tty_device.clear = vbe_ttyd_clear;
  return vbe_tty_device;
}

tty_device vbe_ttyd_get_device() {
  return vbe_tty_device;
}
