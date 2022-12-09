#ifndef _KERNEL_KB_H
#define _KERNEL_KB_H



#include <sys/io.h>

unsigned char keyboard_us_layout[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 
    '\b',   /* Backspace */
    '\t',   /* Tab */
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i', 'o', 'p', '[', ']',
    '\n',   /* Enter Key */
    0,      /* Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`',
    0,      /* Left Shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',   
    0,      /* Right Shift */
    '*',
    0,	    /* Alt */
    ' ',    /* Space Bar */
    0,	    /* Caps Lock */
    0,	    /* F1 */
    0,	    /* F2 */ 
    0, 	    /* F3 */
    0, 	    /* F4 */
    0,	    /* F5 */ 
    0, 	    /* F6 */
    0, 	    /* F7 */
    0, 	    /* F8 */
    0,	    /* F9 */
    0,	    /* F10 */
    0,	    /* Num Lock*/
    0,	    /* Scroll Lock */
    0,	    /* Home Key */
    0,	    /* Up Arrow */
    0,	    /* Page Up */
    '-',
    0,	    /* Left Arrow */
    0,
    0,	    /* Right Arrow */
    '+',
    0,	    /* End Key*/
    0,	    /* Down Arrow */
    0,	    /* Page Down */
    0,	    /* Insert Key */
    0,	    /* Delete Key */
    0, 0, 0,
    0,	    /* F11 Key */
    0,	    /* F12 Key */
    0,	    /* All other keys are undefined */
};

static inline int keyboard_readscan() {
    return inb(0x60);
}

static inline int keyboard_scan2ch(int scan) {
    return keyboard_us_layout[scan & 0x7F];
}

static inline bool keyboard_scan2release(int scan) {
    return scan & 0x80;
}

#endif