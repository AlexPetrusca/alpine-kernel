#include <kernel/device/apic.h>
#include <kernel/cpu/cpu.h>
#include <stdio.h>
#include <kernel/mem/mem.h>

uint64_t _apic_base_addr = 0;

uint32_t apic_read_reg(apic_reg reg) {
  return *((uint32_t*) (_apic_base_addr + reg));
}

void apic_write_reg(apic_reg reg, uint32_t value) {
  *((uint32_t*) (_apic_base_addr + reg)) = value;
}

bool apic_init() {
  uint64_t apic_base_msr = msr_read64(IA32_APIC_BASE_MSR);
  SET_APIC_GLOBAL_ENABLE(apic_base_msr);
  msr_write64(IA32_APIC_BASE_MSR, apic_base_msr);
  uint64_t apic_base = GET_APIC_BASE(apic_base_msr);
  try(mem_identity_map_range(apic_base, 4096, MEMORY_LAPIC), false, "");
  _apic_base_addr = apic_base;
  return true;
}

void apic_enable_lapic() {
  // Logical Destination Mode
  apic_write_reg(APIC_REG_DFR, 0xffffffff);   // Flat mode
  apic_write_reg(APIC_REG_LDR, 0x01000000);   // All cpus use logical id 1
  //  Configure Spurious Interrupt Vector Register
  apic_write_reg(APIC_REG_SIV, apic_read_reg(APIC_REG_SIV) | 0x1ff);
  // Clear task priority to enable all interrupts
  apic_write_reg(APIC_REG_TPR, 0);
}

void apic_send_command(uint32_t cmd_high, uint32_t cmd_low) {
  while (apic_read_reg(APIC_REG_ICR_LOW) & ICR_SEND_PENDING);
  apic_write_reg(APIC_REG_ICR_HIGH, cmd_high);
  apic_write_reg(APIC_REG_ICR_LOW, cmd_low);
  while (apic_read_reg(APIC_REG_ICR_LOW) & ICR_SEND_PENDING);
}

void apic_send_init() {
  apic_send_command(0, ICR_ALL_EXCLUDING_SELF | ICR_INIT | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | 0);
}

void apic_send_start() {
  apic_send_command(0, ICR_ALL_EXCLUDING_SELF | ICR_STARTUP | ICR_PHYSICAL | ICR_ASSERT | ICR_EDGE | 1); // vector 1
}

apic_table_header* find_apic_table() {
  return (apic_table_header*) acpi_find_table("APIC");
}

void apic_print_info(__unused int argc, __unused char** argv) {
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
             entry->acpi_processor_id, entry->lint, entry->flags);
    } else {
      printf("Unsupported Entry Type: %d\n", header->entry_type);
    }
    ptr += header->record_length;
  }
}

void apic_print_lapic_info(__unused int argc, __unused char** argv) {
  printf("LAPIC ID: %d\n", apic_read_reg(APIC_REG_ID));
  printf("LAPIC Version: %d\n", apic_read_reg(APIC_REG_VR));
  printf("LAPIC PPR: %d\n", apic_read_reg(APIC_REG_PPR));
}