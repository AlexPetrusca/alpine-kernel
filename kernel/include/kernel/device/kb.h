#ifndef _KERNEL_KB_H
#define _KERNEL_KB_H

#include <stdbool.h>
#include <stdio.h>

#include <sys/io.h>
#include <sys/circ_buf.h>
#include <kernel/cpu/isr.h>

#define KB_PS2_DATA       0x60
#define KB_PS2_COMMAND    0x64

#define KB_CONTROL        128
#define KB_OPTION         129
#define KB_LEFT_COMMAND   130
#define KB_RIGHT_COMMAND  131
#define KB_LEFT_SHIFT     132
#define KB_RIGHT_SHIFT    133
#define KB_CAPS_LOCK      134
#define KB_NUM_LOCK       135
#define KB_SCROLL_LOCK    136
#define KB_INSERT         137
#define KB_DELETE         138
#define KB_ESC            139
#define KB_HOME           140
#define KB_END            141
#define KB_PAGE_UP        142
#define KB_PAGE_DOWN      143
#define KB_RIGHT_ARROW    144
#define KB_UP_ARROW       145
#define KB_LEFT_ARROW     146
#define KB_DOWN_ARROW     147
#define KB_F1             148
#define KB_F2             149
#define KB_F3             150
#define KB_F4             151
#define KB_F5             152
#define KB_F6             153
#define KB_F7             154
#define KB_F8             155
#define KB_F9             156
#define KB_F10            157
#define KB_F11            158
#define KB_F12            159

extern volatile circ_buf* kb_event_queue;
extern volatile bool kb_pressed[256];
extern volatile bool kb_num_lock;
extern volatile bool kb_caps_lock;
extern volatile bool kb_scroll_lock;

static inline int kb_readscan() {
  return inb(KB_PS2_DATA);
}

static inline void kb_enqueue_event() {
  int scan = kb_readscan();
  circ_buf_enqueue((circ_buf*) kb_event_queue, &scan);
}

static inline int kb_dequeue_event() {
  return *(int*) circ_buf_dequeue((circ_buf*) kb_event_queue);
}

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

void kb_init();
int kb_getchar();

#endif