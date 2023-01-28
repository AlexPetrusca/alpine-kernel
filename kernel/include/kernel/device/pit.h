#ifndef ALPINE_KERNEL_PIT_H
#define ALPINE_KERNEL_PIT_H

#define PIT_CHANNEL_0   0x40    /* Channel 0 data port (read/write) */
#define PIT_CHANNEL_1   0x41    /* Channel 1 data port (read/write) */
#define PIT_CHANNEL_2   0x42    /* Channel 2 data port (read/write) */
#define PIT_COMMAND     0x43    /* Mode/Command register (write only, a read is ignored) */

void pit_init();

#endif //ALPINE_KERNEL_PIT_H
