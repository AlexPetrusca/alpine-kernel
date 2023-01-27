#include <kernel/device/kb.h>
#include <kernel/cpu/pic.h>

int kb_us_layout[256] = {
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

volatile bool kb_pressed[256];
volatile bool kb_num_lock;
volatile bool kb_caps_lock;
volatile bool kb_scroll_lock;

volatile circ_buf* kb_event_queue;

void kb_init() {
  kb_event_queue = circ_buf_new(16, 1);
  // clear any unprocessed kb event on the PIC
  inb(KB_PS2_DATA);
  pic_eoi(PIC_KEYBOARD_IRQ);
}

static inline int kb_scan2release(int scan) {
  return scan & 0x80;
}

int kb_scan2ch(int scan) {
  int scan_offset = scan & 0x7F;
  if (kb_is_pressed(KB_LEFT_SHIFT) || kb_is_pressed(KB_RIGHT_SHIFT) || kb_caps_lock) {
    // todo: CAPS_LOCK should only affect alphabetic characters
    // todo: when holding shift and CAPS_LOCK on, should type lowecase characters???
    return kb_us_layout[scan_offset + 128];
  } else {
    return kb_us_layout[scan_offset];
  }
}

int kb_getchar() {
  while (true) {
    if (kb_event_queue->size > 0) {
      int scan = kb_dequeue_event();
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
      } else {
        return ch;
      }
    }
  }
}
