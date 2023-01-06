#include <kernel/cpu/smp.h>
#include <kernel/device/apic.h>
#include <kernel/cpu/cpu.h>
#include <stdio.h>

extern volatile uint16_t main_idt_pointer_size;
extern volatile uint16_t smp_idt_pointer_size;
extern volatile uint32_t main_idt_pointer_base;
extern volatile uint32_t smp_idt_pointer_base;

extern volatile uint16_t main_gdt_pointer_size;
extern volatile uint16_t smp_gdt_pointer_size;
extern volatile uint64_t main_gdt_pointer_base;
extern volatile uint64_t smp_gdt_pointer_base;

void smp_startup() {
  printf("IDT, %d, %x\n", main_idt_pointer_size, main_idt_pointer_base);
  printf("GDT, %d, %lx\n", main_gdt_pointer_size, main_gdt_pointer_base);

  smp_gdt_pointer_size = main_gdt_pointer_size;
  smp_gdt_pointer_base = main_gdt_pointer_base;

  smp_idt_pointer_size = main_idt_pointer_size;
  smp_idt_pointer_base = main_idt_pointer_base;

  printf("SMP IDT @%lx, %d, %x\n", (uint64_t)&smp_idt_pointer_size, smp_idt_pointer_size, smp_idt_pointer_base);
  printf("SMP GDT @%lx, %d, %lx\n", (uint64_t)&smp_gdt_pointer_size, smp_gdt_pointer_size, smp_gdt_pointer_base);

  apic_enable_lapic();

  apic_send_init();
  cpu_delay_ms(10);
  apic_send_sipi();
  cpu_delay_us(200);
  apic_send_sipi();
  cpu_delay_us(200);
}
