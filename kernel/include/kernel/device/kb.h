#ifndef _KERNEL_KB_H
#define _KERNEL_KB_H

#include <stdbool.h>
#include <sys/io.h>

#define KB_CONTROL 128
#define KB_OPTION 129
#define KB_LEFT_COMMAND 130
#define KB_RIGHT_COMMAND 131
#define KB_LEFT_SHIFT 132
#define KB_RIGHT_SHIFT 133
#define KB_CAPS_LOCK 134
#define KB_NUM_LOCK 135
#define KB_SCROLL_LOCK 136
#define KB_INSERT 137
#define KB_DELETE 138
#define KB_ESC 139
#define KB_HOME 140
#define KB_END 141
#define KB_PAGE_UP 142
#define KB_PAGE_DOWN 143
#define KB_RIGHT_ARROW 144
#define KB_UP_ARROW 145
#define KB_LEFT_ARROW 146
#define KB_DOWN_ARROW 147
#define KB_F1 148
#define KB_F2 149
#define KB_F3 150
#define KB_F4 151
#define KB_F5 152
#define KB_F6 153
#define KB_F7 154
#define KB_F8 155
#define KB_F9 156
#define KB_F10 157
#define KB_F11 158
#define KB_F12 159

// todo: some of these fields/functions don't need to be exposed to downstream consumers --> can move them to kb.c

static int kb_us_layout[256] = {
    0, KB_ESC, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', KB_CONTROL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', KB_LEFT_SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', KB_RIGHT_SHIFT, '*', KB_OPTION, ' ', KB_CAPS_LOCK, KB_F1,
    KB_F2, KB_F3, KB_F4, KB_F5, KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_NUM_LOCK,
    KB_SCROLL_LOCK, KB_HOME, KB_UP_ARROW, KB_PAGE_UP, '-', KB_LEFT_ARROW, 0, KB_RIGHT_ARROW, '+', KB_END,
    KB_DOWN_ARROW, KB_PAGE_DOWN, KB_INSERT, KB_DELETE, 0, 0, 0, KB_F11, KB_F12, 0,
    0, KB_LEFT_COMMAND, KB_RIGHT_COMMAND, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* Upper half when shift is pressed */
    0, KB_ESC, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', KB_CONTROL,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '\"', '~', KB_LEFT_SHIFT, '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?', KB_RIGHT_SHIFT, '*', KB_OPTION, ' ', KB_CAPS_LOCK, KB_F1,
    KB_F2, KB_F3, KB_F4, KB_F5, KB_F6, KB_F7, KB_F8, KB_F9, KB_F10, KB_NUM_LOCK,
    KB_SCROLL_LOCK, KB_HOME, KB_UP_ARROW, KB_PAGE_UP, '-', KB_LEFT_ARROW, 0, KB_RIGHT_ARROW, '+', KB_END,
    KB_DOWN_ARROW, KB_PAGE_DOWN, KB_INSERT, KB_DELETE, 0, 0, 0, KB_F11, KB_F12, 0,
    0, KB_LEFT_COMMAND, KB_RIGHT_COMMAND, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};

static bool kb_pressed[256];
static bool kb_num_lock;
static bool kb_caps_lock;
static bool kb_scroll_lock;

static inline bool kb_is_pressed(int ch) {
  return kb_pressed[ch];
}

static inline bool kb_is_released(int ch) {
  return !kb_pressed[ch];
}

static inline void kb_set_pressed(int ch) {
  kb_pressed[ch] = true;
}

static inline void kb_set_released(int ch) {
  kb_pressed[ch] = false;
}

static inline int kb_readscan() {
  return inb(0x60);
}

static inline int kb_scan2ch(int scan) {
  int scan_offset = scan & 0x7F;
  if (kb_is_pressed(KB_LEFT_SHIFT) || kb_is_pressed(KB_RIGHT_SHIFT) || kb_caps_lock) {
    // todo: CAPS_LOCK should only affect alphabetic characters
    // todo: when holding shift and CAPS_LOCK on, should type lowecase characters???
    return kb_us_layout[scan_offset + 128];
  } else {
    return kb_us_layout[scan_offset];
  }
}

static inline int kb_scan2release(int scan) {
  return scan & 0x80;
}

static inline int kb_getchar() {
  while (true) {
    int scan = kb_readscan();
    int ch = kb_scan2ch(scan);
    if (kb_scan2release(scan)) {
      kb_set_released(ch);
    } else if (kb_is_released(ch)) {
      kb_set_pressed(ch);
      if (ch == KB_NUM_LOCK) {
        kb_num_lock = !kb_num_lock;
      } else if (ch == KB_CAPS_LOCK) {
        kb_caps_lock = !kb_caps_lock;
      } else if (ch == KB_SCROLL_LOCK) {
        kb_scroll_lock = !kb_scroll_lock;
      }
      return ch;
    }
  }
}

#endif