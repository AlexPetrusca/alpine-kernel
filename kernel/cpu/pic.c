#include <stdint.h>

#include <sys/io.h>
#include <kernel/cpu/pic.h>
#include <kernel/cpu/asm.h>

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define PIC_ICW1_ICW4        0x01    /* ICW4 (not) needed */
#define PIC_ICW1_SINGLE      0x02    /* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4    0x04    /* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL        0x08    /* Level triggered (edge) mode */
#define PIC_ICW1_INIT        0x10    /* Initialization - required! */

#define PIC_ICW4_8086        0x01    /* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO        0x02    /* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE    0x08    /* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER  0x0C    /* Buffered mode/master */
#define PIC_ICW4_SFNM        0x10    /* Special fully nested (not) */

/*
arguments:
	vec_offset1 - vector offset for master PIC vectors on the master become vec_offset1..vec_offset1+7
	vec_offset2 - same for slave PIC: vec_offset2..vec_offset2+7
*/
void pic_remap(int vec_offset1, int vec_offset2) {
  uint8_t a1 = inb(PIC1_DATA);                        // save masks
  uint8_t a2 = inb(PIC2_DATA);

  io_wait();
  outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, vec_offset1);             // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, vec_offset2);             // ICW2: Slave PIC vector offset
  io_wait();
  outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();

  outb(PIC1_DATA, PIC_ICW4_8086);
  io_wait();
  outb(PIC2_DATA, PIC_ICW4_8086);
  io_wait();

  outb(PIC1_DATA, a1);   // restore saved masks.
  outb(PIC2_DATA, a2);
}

void pic_eoi(uint8_t irq) {
  if (irq >= 8) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}

void pic_disable_irq(uint8_t irq) {
  uint16_t port;
  uint8_t value;
  if (irq < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq -= 8;
  }
  value = inb(port) | (1 << irq);
  outb(port, value);
}

void pic_enable_irq(uint8_t irq) {
  uint16_t port;
  uint8_t value;
  if (irq < 8) {
    port = PIC1_DATA;
  } else {
    port = PIC2_DATA;
    irq -= 8;
  }
  value = inb(port) & ~(1 << irq);
  outb(port, value);
}

/* OCW3 to PIC CMD to get the register values. PIC2 is chained, and
 * represents IRQs 8-15. PIC1 is IRQs 0-7, with 2 being the chain */
uint16_t __pic_get_irq_reg(int ocw3) {
  outb(PIC1_COMMAND, ocw3);
  outb(PIC2_COMMAND, ocw3);
  return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_get_irr() {
  return __pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t pic_get_isr() {
  return __pic_get_irq_reg(PIC_READ_ISR);
}

void pic_init() {
  pic_remap(PIC1_START_VECTOR, PIC2_START_VECTOR);
  pic_enable_irq(PIC_KEYBOARD_IRQ);
  pic_enable_irq(PIC1_SPURIOUS_IRQ);
  pic_enable_irq(PIC2_SPURIOUS_IRQ);
  STI();
}
