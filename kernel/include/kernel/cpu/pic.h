#ifndef ALPINE_KERNEL_PIC_H
#define ALPINE_KERNEL_PIC_H

#define PIC1 0x20		        /* IO base address for master PIC */
#define PIC2 0xA0		        /* IO base address for slave PIC */
#define PIC1_COMMAND	(PIC1)
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND	(PIC2)
#define PIC2_DATA (PIC2 + 1)

void pic_init();
void pic_remap(int vec_offset1, int vec_offset2);

#endif //ALPINE_KERNEL_PIC_H
