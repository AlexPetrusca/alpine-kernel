#ifndef ALPINE_KERNEL_PIC_H
#define ALPINE_KERNEL_PIC_H

#define PIC1                0x20    /* IO base address for master PIC */
#define PIC2                0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND        (PIC1)
#define PIC1_DATA           (PIC1 + 1)
#define PIC2_COMMAND	       (PIC2)
#define PIC2_DATA           (PIC2 + 1)

#define PIC_EOI             0x20
#define PIC_READ_IRR        0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR        0x0b    /* OCW3 irq service next CMD read */

#define PIC1_START_VECTOR   0x20
#define PIC2_START_VECTOR   0x28

#define PIC_PIT_IRQ         0
#define PIC_KEYBOARD_IRQ    1       /* PS/2 keyboard IRQ */
#define PIC_CASCADE_IRQ     2       /* Used internally by the two PICs. never raised */
#define PIC_COM2_IRQ        3
#define PIC_COM1_IRQ        4
#define PIC_LPT2_IRQ        5
#define PIC_FLOPPY_IRQ      6
#define PIC_LPT1_IRQ        7
#define PIC_CMOS_CLOCK_IRQ  8
#define PIC_FREE1_IRQ       9
#define PIC_FREE2_IRQ       10
#define PIC_FREE3_IRQ       11
#define PIC_MOUSE_IRQ       12      /* PS/2 mouse IRQ */
#define PIC_FPU_IRQ         13
#define PIC_PRI_ATA_IRQ     14      /* Primary ATA IRQ */
#define PIC_SEC_ATA_IRQ     15      /* Secondary ATA IRQ */
// https://wiki.osdev.org/8259_PIC#Spurious_IRQs
#define PIC1_SPURIOUS_IRQ   7       /* Master spurious IRQ */
#define PIC2_SPURIOUS_IRQ   15      /* Slave spurious IRQ */

void pic_init();
void pic_remap(int vec_offset1, int vec_offset2);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);
void pic_eoi(uint8_t irq);

#endif //ALPINE_KERNEL_PIC_H
