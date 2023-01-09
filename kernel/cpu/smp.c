#include <kernel/cpu/smp.h>
#include <kernel/device/apic.h>
#include <kernel/cpu/cpu.h>
#include <kernel/mem/mem.h>
#include <stdio.h>

extern volatile uint16_t main_idt_pointer_size;
extern volatile uint16_t smp_idt_pointer_size;
extern volatile uint32_t main_idt_pointer_base;
extern volatile uint32_t smp_idt_pointer_base;

extern volatile uint16_t main_gdt_pointer_size;
extern volatile uint16_t smp_gdt_pointer_size;
extern volatile uint64_t main_gdt_pointer_base;
extern volatile uint64_t smp_gdt_pointer_base;

extern volatile uint64_t smp_pml4_base;

void smp_init() {
  smp_gdt_pointer_size = main_gdt_pointer_size;
  smp_gdt_pointer_base = main_gdt_pointer_base;
  smp_idt_pointer_size = main_idt_pointer_size;
  smp_idt_pointer_base = main_idt_pointer_base;
  smp_pml4_base = mem_get_pml4_addr();

  apic_enable_lapic();

  apic_send_init();
  cpu_delay_ms(10);
  apic_send_start();
  cpu_delay_us(200);
  apic_send_start();
  cpu_delay_us(200);
}
