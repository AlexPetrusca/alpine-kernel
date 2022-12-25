#ifndef _ACPI_H_
#define _ACPI_H_

#include "Acpi20.h"
#include <string.h>
#include <stdint.h>

extern EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* rsdp;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER header;
  uint32_t pointer_to_other_sdt[];
} Rsdt;

#define LOCAL_APIC 0
#define IO_APIC 1
#define IO_APIC_INTERRUPTS_SOURCE_OVERRIDE 2
#define LOCAL_APIC_NON_MASKABLE_INTERRUPTS 4

typedef struct {
  uint8_t entry_type;
  uint8_t record_length;
} ApicEntryHeader;

typedef struct {
  ApicEntryHeader header;
  uint8_t acpi_processor_id;
  uint8_t apic_id;
  uint32_t flags; // (bit 0 = Processor Enabled) (bit 1 = Online Capable)
} LocalApicEntry;

typedef struct {
  ApicEntryHeader header;
  uint8_t apic_id;
  uint8_t reserved;
  uint32_t ioapic_address;
  uint32_t global_system_interrupt_base;
} IOApicEntry;

typedef struct {
  ApicEntryHeader header;
  uint8_t bus_source;
  uint8_t irq_source;
  uint32_t global_system_interrupt;
  uint16_t flags;
} IOApicISREntry; // ISR = Interrupt Source Override

typedef struct {
  ApicEntryHeader header;
  uint8_t acpi_processor_id; // 0xFF means all processors
  uint8_t flags;
  uint8_t lint; // 0 or 1
} LocalApicNMIEntry; // NMI = NON_MASKABLE_INTERRUPTS

EFI_ACPI_DESCRIPTION_HEADER* find_acpi_table(char* name);
EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER* find_apic_table();
void print_rsdt_info();
void print_apic_info();

#endif //_ACPI_H_
