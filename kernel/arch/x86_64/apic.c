#include <kernel/apic.h>
#include <kernel/cpu.h>
#include <stdio.h>

uint32_t apic_read_reg(apic_reg reg, uint64_t apic_base_addr) {
  return *((uint32_t*) (apic_base_addr + reg));
}

void apic_write_reg(apic_reg reg, uint32_t value, uint64_t apic_base_addr) {
  *((uint32_t*) (apic_base_addr + reg)) = value;
}

uint32_t apic_global_enable() {
  uint64_t apic_base_msr = msr_read64(IA32_APIC_BASE_MSR);
  if (!IS_APIC_GLOBAL_ENABLE(apic_base_msr)) {
    SET_APIC_GLOBAL_ENABLE(apic_base_msr);
    msr_write64(IA32_APIC_BASE_MSR, apic_base_msr);
  }
  return GET_APIC_BASE(apic_base_msr);
}

void apic_enable(uint64_t apic_base_addr) {
  uint32_t siv = apic_read_reg(APIC_REG_SIV, apic_base_addr);
  if (!(siv & APIC_REG_SIV_ENABLE)) {
    apic_write_reg(APIC_REG_SIV, siv | APIC_REG_SIV_ENABLE, apic_base_addr);
  }
}

void apic_send_init(uint64_t apic_base_addr) {
  apic_write_reg(APIC_REG_ICR_HIGH, 0, apic_base_addr);
  //  Vector: 00h
  //  Delivery mode: Startup
  //  Destination mode: ignored (0)
  //  Level: ignored (1)
  //  Trigger mode: ignored (0)
  //  Shorthand: All excluding self (3)
  //  Writing the low DWORD sent the IPI
  apic_write_reg(APIC_REG_ICR_LOW, 0x0c4500, apic_base_addr);
}

apic_table_header* find_apic_table() {
  return (apic_table_header*) acpi_find_table("APIC");
}

void apic_print_info() {
  apic_table_header* apic = find_apic_table();

  printf("Signature: %.4s\n", (char*) &(apic->header.signature));
  printf("OEM Id: %.6s\n", (char*) &(apic->header.oem_id));
  printf("OEM Table Id: %.8s\n", (char*) &(apic->header.oem_table_id));
  printf("OEM Revision: %d\n", apic->header.oem_revision);
  printf("Length: %d\n", apic->header.length);
  printf("Local APIC Address: 0x%x\n", apic->local_apic_address);

  uint8_t* ptr = (uint8_t*) apic + sizeof(apic_table_header);
  while (ptr < (uint8_t*) apic + apic->header.length) {
    apic_entry_header* header = (apic_entry_header*) ptr;
    if (header->entry_type == LOCAL_APIC) {
      apic_lapic_entry* entry = (apic_lapic_entry*) header;
      printf("Processor, Id: %d, APIC Id: %d, flags: %b\n", entry->acpi_processor_id, entry->apic_id, entry->flags);
    } else if (header->entry_type == IO_APIC) {
      apic_ioapic_entry* entry = (apic_ioapic_entry*) header;
      printf("IO APIC, Id: %d, address: 0x%x, GlobalSystemInterruptBase: %d\n",
          entry->apic_id, entry->ioapic_address, entry->global_system_interrupt_base);
    } else if (header->entry_type == IO_APIC_INTERRUPTS_SOURCE_OVERRIDE) {
      apic_ioapic_isr_entry* entry = (apic_ioapic_isr_entry*) header;
      printf("IO APIC ISO, bus: %d, irq: %d, global interrupt: %d, flags: %b\n",
          entry->bus_source, entry->irq_source, entry->global_system_interrupt, entry->flags);
    } else if (header->entry_type == LOCAL_APIC_NON_MASKABLE_INTERRUPTS) {
      apic_lapic_nmi_entry* entry = (apic_lapic_nmi_entry*) header;
      printf("Local APIC NMI, processor: %d, LINT: %d, flags: %b\n",
          entry->acpi_processor_id,entry->lint,entry->flags);
    } else {
      printf("Unsupported Entry Type: %d\n", header->entry_type);
    }
    ptr += header->record_length;
  }
}

void apic_print_lapic_info() {
  uint32_t apic_base_addr = apic_global_enable();
  printf("LAPIC ID: %d\n", apic_read_reg(APIC_REG_ID, apic_base_addr));
  printf("LAPIC Version: %d\n", apic_read_reg(APIC_REG_VR, apic_base_addr));
  printf("LAPIC PPR: %d\n", apic_read_reg(APIC_REG_PPR, apic_base_addr));
}