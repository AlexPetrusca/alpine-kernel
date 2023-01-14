#include <sys/io.h>
#include <kernel/cpu/pic.h>

/* reinitialize the PIC controllers, giving them specified vector offsets
   rather than 8h and 70h, as configured by default */

#define ICW1_ICW4	      0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	    0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	  0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	      0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	      0x10		/* Initialization - required! */

#define ICW4_8086	      0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	      0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	  0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	      0x10		/* Special fully nested (not) */

/*
arguments:
	vec_offset1 - vector offset for master PIC vectors on the master become vec_offset1..vec_offset1+7
	vec_offset2 - same for slave PIC: vec_offset2..vec_offset2+7
*/
void pic_remap(int vec_offset1, int vec_offset2) {
  unsigned char a1, a2;

  a1 = inb(PIC1_DATA);                        // save masks
  a2 = inb(PIC2_DATA);

  outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, vec_offset1);                 // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, vec_offset2);                 // ICW2: Slave PIC vector offset
  io_wait();
  outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();

  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, a1);   // restore saved masks.
  outb(PIC2_DATA, a2);
}

void pic_init() {
  pic_remap(32, 40);     // todo: don't hardcode
  outb(0x21, 0xfd);
  outb(0xa1, 0xff);
  __asm volatile ("sti");
}