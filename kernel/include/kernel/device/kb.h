#ifndef _KERNEL_KB_H
#define _KERNEL_KB_H

#include <stdbool.h>
#include <sys/io.h>

#define CONTROL 128
#define OPTION 129
#define LEFT_COMMAND 130
#define RIGHT_COMMAND 131
#define LEFT_SHIFT 132
#define RIGHT_SHIFT 133
#define CAPS_LOCK 134
#define NUM_LOCK 135
#define SCROLL_LOCK 136
#define INSERT 137
#define DELETE 138
#define ESC 139
#define HOME 140
#define END 141
#define PAGE_UP 142
#define PAGE_DOWN 143
#define RIGHT_ARROW 144
#define UP_ARROW 145
#define LEFT_ARROW 146
#define DOWN_ARROW 147
#define F1 148
#define F2 149
#define F3 150
#define F4 151
#define F5 152
#define F6 153
#define F7 154
#define F8 155
#define F9 156
#define F10 157
#define F11 158
#define F12 159

// todo: some of these fields/functions don't need to be exposed to downstream consumers --> can move them to kb.c

static int kb_us_layout[256] = {
    0, ESC, '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', CONTROL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', LEFT_SHIFT, '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/', RIGHT_SHIFT, '*', OPTION, ' ', CAPS_LOCK, F1,
    F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM_LOCK,
    SCROLL_LOCK, HOME, UP_ARROW, PAGE_UP, '-', LEFT_ARROW, 0, RIGHT_ARROW, '+', END,
    DOWN_ARROW, PAGE_DOWN, INSERT, DELETE, 0, 0, 0, F11, F12, 0,
    0, LEFT_COMMAND, RIGHT_COMMAND, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    /* Upper half when shift is pressed */
    0, ESC, '!', '@', '#', '$', '%', '^', '&', '*',
    '(', ')', '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R',
    'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', CONTROL,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '\"', '~', LEFT_SHIFT, '|', 'Z', 'X', 'C', 'V', 'B', 'N',
    'M', '<', '>', '?', RIGHT_SHIFT, '*', OPTION, ' ', CAPS_LOCK, F1,
    F2, F3, F4, F5, F6, F7, F8, F9, F10, NUM_LOCK,
    SCROLL_LOCK, HOME, UP_ARROW, PAGE_UP, '-', LEFT_ARROW, 0, RIGHT_ARROW, '+', END,
    DOWN_ARROW, PAGE_DOWN, INSERT, DELETE, 0, 0, 0, F11, F12, 0,
    0, LEFT_COMMAND, RIGHT_COMMAND, 0, 0, 0, 0, 0, 0, 0,
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
  if (kb_is_pressed(LEFT_SHIFT) || kb_is_pressed(RIGHT_SHIFT) || kb_caps_lock) {
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
      if (ch == NUM_LOCK) {
        kb_num_lock = !kb_num_lock;
      } else if (ch == CAPS_LOCK) {
        kb_caps_lock = !kb_caps_lock;
      } else if (ch == SCROLL_LOCK) {
        kb_scroll_lock = !kb_scroll_lock;
      }
      return ch;
    }
  }
}

#endif