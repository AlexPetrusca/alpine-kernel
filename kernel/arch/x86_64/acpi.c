#include <string.h>
#include <stdio.h>
#include <kernel/acpi.h>

Acpi2Rsdp* rsdp;

void print_rsdt_info() {
  Rsdt* rsdt = (Rsdt*) rsdp->RsdtAddress;
  int entry_count = (rsdt->header.Length - sizeof(AcpiDescriptionHeader)) / 4;
  printf("Signature: %.4s\n", (char*) rsdt);
  printf("ACPI table count: %d\n", entry_count);
  for (int i = 0; i < entry_count; i++) {
    AcpiDescriptionHeader* table = (AcpiDescriptionHeader*) rsdt->pointer_to_other_sdt[i];
    printf("Table %d: %.4s, %d bytes\n", i + 1, (char*) table, table->Length);
  }
}

AcpiDescriptionHeader* find_acpi_table(char* name) {
  Rsdt* rsdt = (Rsdt*) rsdp->RsdtAddress;
  int entry_count = (rsdt->header.Length - sizeof(AcpiDescriptionHeader)) / 4;
  for (int i = 0; i < entry_count; i++) {
    AcpiDescriptionHeader* table = (AcpiDescriptionHeader*) rsdt->pointer_to_other_sdt[i];
    if (strnequ(name, (char*) table, 4)) {
      return table;
    }
  }
  return NULL;
}

Acpi2MultipleApicTableHeader* find_apic_table() {
  return (Acpi2MultipleApicTableHeader*) find_acpi_table("APIC");
}

void print_apic_info() {
  Acpi2MultipleApicTableHeader* apic = find_apic_table();

  printf("Signature: %.4s\n", (char*) &(apic->Header.Signature));
  printf("OEM Id: %.6s\n", (char*) &(apic->Header.OemId));
  printf("OEM Table Id: %.8s\n", (char*) &(apic->Header.OemTableId));
  printf("OEM Revision: %d\n", apic->Header.OemRevision);
  printf("Length: %d\n", apic->Header.Length);
  printf("Local APIC Address: 0x%x\n", apic->LocalApicAddress);

  uint8_t* ptr = (uint8_t*) apic + sizeof(Acpi2MultipleApicTableHeader);
  while (ptr < (uint8_t*) apic + apic->Header.Length) {
    ApicEntryHeader* header = (ApicEntryHeader*) ptr;
    if (header->entry_type == LOCAL_APIC) {
      LocalApicEntry* entry = (LocalApicEntry*) header;
      printf("Processor, Id: %d, APIC Id: %d, flags: %b\n", entry->acpi_processor_id, entry->apic_id, entry->flags);
    } else if (header->entry_type == IO_APIC) {
      IOApicEntry* entry = (IOApicEntry*) header;
      printf("IO APIC, Id: %d, address: 0x%x, GlobalSystemInterruptBase: %d\n",
          entry->apic_id, entry->ioapic_address, entry->global_system_interrupt_base);
    } else if (header->entry_type == IO_APIC_INTERRUPTS_SOURCE_OVERRIDE) {
      IOApicISREntry* entry = (IOApicISREntry*) header;
      printf("IO APIC ISO, bus: %d, irq: %d, global interrupt: %d, flags: %b\n",
          entry->bus_source, entry->irq_source, entry->global_system_interrupt, entry->flags);
    } else if (header->entry_type == LOCAL_APIC_NON_MASKABLE_INTERRUPTS) {
      LocalApicNMIEntry* entry = (LocalApicNMIEntry*) header;
      printf("Local APIC NMI, processor: %d, LINT: %d, flags: %b\n",
          entry->acpi_processor_id,
          entry->lint,
          entry->flags);
    } else {
      printf("Unsupported Entry Type: %d\n", header->entry_type);
    }
    ptr += header->record_length;
  }
}
