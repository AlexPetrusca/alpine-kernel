#ifndef KERNEL_INCLUDE_KERNEL_TD_H_
#define KERNEL_INCLUDE_KERNEL_TD_H_

#include <stddef.h>
#include <stdint.h>

typedef enum _text_color {
  COLOR_BLACK = 0,
  COLOR_BLUE = 1,
  COLOR_GREEN = 2,
  COLOR_CYAN = 3,
  COLOR_RED = 4,
  COLOR_MAGENTA = 5,
  COLOR_BROWN = 6,
  COLOR_LIGHT_GREY = 7,
  COLOR_DARK_GREY = 8,
  COLOR_LIGHT_BLUE = 9,
  COLOR_LIGHT_GREEN = 10,
  COLOR_LIGHT_CYAN = 11,
  COLOR_LIGHT_RED = 12,
  COLOR_LIGHT_MAGENTA = 13,
  COLOR_LIGHT_BROWN = 14,
  COLOR_WHITE = 15,
} tty_color;

typedef struct _tty_device {
  uint32_t (* get_width)();
  uint32_t (* get_height)();
  void (* put_char)(uint32_t x, uint32_t y, uint8_t ch, uint8_t attr);
  void (* enable_cursor)(uint8_t cursor_start, uint8_t cursor_end);
  void (* disable_cursor)();
  void (* set_cursor)(uint32_t x, uint32_t y);
  void (* clear)();
} tty_device;

static inline uint8_t attributes(tty_color fg, tty_color bg) {
  return fg | bg << 4;
}

#endif //KERNEL_INCLUDE_KERNEL_TD_H_
